/*
	File:		MoreFilesX.h

	Contains:	A collection of useful high-level File Manager routines
				which use the HFS Plus APIs wherever possible.

	Version:	MoreFilesX 1.0.1

	Copyright:	? 1992-2002 by Apple Computer, Inc., all rights reserved.

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple?s
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

		 <3>	 4/19/02	JL		[2853905]  Fixed #if test around header includes.
		 <2>	 4/19/02	JL		[2853901]  Updated standard disclaimer.
		 <1>	 1/25/02	JL		MoreFilesX 1.0
	
	Notes:
		What do those arrows in the documentation for each routine mean?
			
			--> The parameter is an input
			
			<-- The parameter is an output. The pointer to the variable
				where the output will be returned (must not be NULL).
			
			<** The parameter is an optional output. If it is not a
				NULL pointer, it points to the variable where the output
				will be returned. If it is a NULL pointer, the output will
				not be returned and will possibly let the routine and the
				File Manager do less work. If you don't need an optional output,
				don't ask for it.
			**> The parameter is an optional input. If it is not a
				NULL pointer, it points to the variable containing the
				input data. If it is a NULL pointer, the input is not used
				and will possibly let the routine and the File Manager
				do less work.
*/

#ifndef __MOREFILESX__
#define __MOREFILESX__

#ifndef __CARBON__
	#if defined(__MACH__)
		#include <Carbon/Carbon.h>
	#else
		#include <Carbon.h>
	#endif
#endif

#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
	#pragma pack(2)
#endif

/*****************************************************************************/

#pragma mark ----- File/Directory Manipulation Routines -----

/*****************************************************************************/

#pragma mark FSRefValid

Boolean FSRefValid(const FSRef *ref);

/*
	The FSRefValid function determines if an FSRef is valid. If the result is
	true, then the FSRef refers to an existing file or directory.
	
	ref					--> FSRef to a file or directory.
*/

/*****************************************************************************/

#pragma mark FSBumpDate

OSErr
FSBumpDate(
	const FSRef *ref);

/*
	The FSBumpDate function changes the content modification date of a file
	or directory to the current date/time. If the content modification date
	is already equal to the current date/time, then add one second to the
	content modification date.

	ref					--> FSRef to a file or directory.
*/

/*****************************************************************************/

#pragma mark FSChangeCreatorType

OSErr
FSChangeCreatorType(
	const FSRef *ref,
	OSType fileCreator,
	OSType fileType);

/*
	The FSChangeCreatorType function changes the creator and/or file type of a file.

	ref					--> FSRef to a file.
	creator				--> The new creator type or 0x00000000 to leave
							the creator type alone.
	fileType			--> The new file type or 0x00000000 to leave the
							file type alone.
*/


#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
	#pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MOREFILESX__ */

