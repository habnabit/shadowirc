/*
	File:		MoreFilesX.c

	Contains:	A collection of useful high-level File Manager routines
				which use the HFS Plus APIs wherever possible.

	Version:	MoreFilesX 1.0.1

	Copyright:	© 1992-2002 by Apple Computer, Inc., all rights reserved.

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple’s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	File Ownership:

		DRI:				Apple Macintosh Developer Technical Support

		Other Contact:		For bug reports, consult the following page on
							the World Wide Web:
								http://developer.apple.com/bugreporter/

		Technology:			DTS Sample Code

	Writers:

		(JL)	Jim Luther

	Change History (most recent first):

		 <4>	 8/22/02	JL		[3016251]  Changed FSMoveRenameObjectUnicode to not use
		 							the Temporary folder because it isn't available on
		 							NFS volumes.
		 <3>	 4/19/02	JL		[2853905]  Fixed #if test around header includes.
		 <2>	 4/19/02	JL		[2850624]  Fixed C++ compile errors and Project Builder
		 							warnings.
		 <2>	 4/19/02	JL		[2853901]  Updated standard disclaimer.
		 <1>	 1/25/02	JL		MoreFilesX 1.0
*/

#if defined(__MACH__)
	#include <Carbon/Carbon.h>
	#include <string.h>
#else
	#include <Carbon.h>
	#include <string.h>
#endif

#include "MoreFilesX.h"

/*****************************************************************************/

#pragma mark ----- File/Directory Manipulation Routines -----

/*****************************************************************************/

Boolean FSRefValid(const FSRef *ref)
{
	return ( noErr == FSGetCatalogInfo(ref, kFSCatInfoNone, NULL, NULL, NULL, NULL) );
}

/*****************************************************************************/

OSErr
FSBumpDate(
	const FSRef *ref)
{
	OSStatus		result;
	FSCatalogInfo	catalogInfo;
	UTCDateTime		oldDateTime;
#if !BuildingMoreFilesXForMacOS9
	FSRef			parentRef;
	Boolean			notifyParent;
#endif

#if !BuildingMoreFilesXForMacOS9
	/* Get the node flags, the content modification date and time, and the parent ref */
	result = FSGetCatalogInfo(ref, kFSCatInfoNodeFlags + kFSCatInfoContentMod, &catalogInfo, NULL, NULL, &parentRef);
	require_noerr(result, FSGetCatalogInfo);
	
	/* Notify the parent if this is a file */
	notifyParent = (0 == (catalogInfo.nodeFlags & kFSNodeIsDirectoryMask));
#else
	/* Get the content modification date and time */
	result = FSGetCatalogInfo(ref, kFSCatInfoContentMod, &catalogInfo, NULL, NULL, NULL);
	require_noerr(result, FSGetCatalogInfo);
#endif
	
	oldDateTime = catalogInfo.contentModDate;

	/* Get the current date and time */
	result = GetUTCDateTime(&catalogInfo.contentModDate, kUTCDefaultOptions);
	require_noerr(result, GetUTCDateTime);
	
	/* if the old date and time is the the same as the current, bump the seconds by one */
	if ( (catalogInfo.contentModDate.fraction == oldDateTime.fraction) &&
		 (catalogInfo.contentModDate.lowSeconds == oldDateTime.lowSeconds) &&
		 (catalogInfo.contentModDate.highSeconds == oldDateTime.highSeconds) )
	{
		++catalogInfo.contentModDate.lowSeconds;
		if ( 0 == catalogInfo.contentModDate.lowSeconds )
		{
			++catalogInfo.contentModDate.highSeconds;
		}
	}
	
	/* Bump the content modification date and time */
	result = FSSetCatalogInfo(ref, kFSCatInfoContentMod, &catalogInfo);
	require_noerr(result, FSSetCatalogInfo);

#if !BuildingMoreFilesXForMacOS9
	/*
	 * The problem with FNNotify is that it is not available under Mac OS 9
	 * and there's no way to test for that except for looking for the symbol
	 * or something. So, I'll just conditionalize this for those who care
	 * to send a notification.
	 */
	
	/* Send a notification for the parent of the file, or for the directory */
	result = FNNotify(notifyParent ? &parentRef : ref, kFNDirectoryModifiedMessage, kNilOptions);
	require_noerr(result, FNNotify);
#endif

	/* ignore errors from FSSetCatalogInfo (volume might be write protected) and FNNotify */
FNNotify:
FSSetCatalogInfo:
	
	return ( noErr );
	
	/**********************/
	
GetUTCDateTime:
FSGetCatalogInfo:

	return ( result );
}

/*****************************************************************************/

OSErr
FSChangeCreatorType(
	const FSRef *ref,
	OSType fileCreator,
	OSType fileType)
{
	OSErr			result;
	FSCatalogInfo	catalogInfo;
	FSRef			parentRef;
	
	/* get nodeFlags, finder info, and parent FSRef */
	result = FSGetCatalogInfo(ref, kFSCatInfoNodeFlags + kFSCatInfoFinderInfo, &catalogInfo , NULL, NULL, &parentRef);
	require_noerr(result, FSGetCatalogInfo);
	
	/* make sure FSRef was to a file */
	require_action(0 == (catalogInfo.nodeFlags & kFSNodeIsDirectoryMask), FSRefNotFile, result = notAFileErr);
	
	/* If fileType not 0x00000000, change fileType */
	if ( fileType != (OSType)0x00000000 )
	{
		((FileInfo *)&catalogInfo.finderInfo)->fileType = fileType;
	}
	
	/* If creator not 0x00000000, change creator */
	if ( fileCreator != (OSType)0x00000000 )
	{
		((FileInfo *)&catalogInfo.finderInfo)->fileCreator = fileCreator;
	}
	
	/* now, save the new information back to disk */
	result = FSSetCatalogInfo(ref, kFSCatInfoFinderInfo, &catalogInfo);
	require_noerr(result, FSSetCatalogInfo);
	
	/* and attempt to bump the parent directory's mod date to wake up */
	/* the Finder to the change we just made (ignore errors from this) */
	verify_noerr(FSBumpDate(&parentRef));
	
FSSetCatalogInfo:
FSRefNotFile:
FSGetCatalogInfo:

	return ( result );
}

/*****************************************************************************/
