/*
**	Apple Macintosh Developer Technical Support
**
**	by Jim Luther, Apple Developer Technical Support Emeritus
**
**	File:		FullPath.c
**
**	Copyright © 1995-1999 Apple Computer, Inc.
**	All rights reserved.
**
**	You may incorporate this sample code into your applications without
**	restriction, though the sample code has been provided "AS IS" and the
**	responsibility for its operation is 100% yours.  However, what you are
**	not permitted to do is to redistribute the source as "DSC Sample Code"
**	after having made changes. If you're going to re-distribute the source,
**	we require that you make it clear in the source that the code was
**	descended from Apple Sample Code, but that you've made changes.
*/

/*
	This file is a condensed version of MoreFiles 1.5, including only the functionality that ShadowIRC requires.
*/

#include <Carbon/Carbon.h>

#include "MoreFiles.h"

pascal	OSErr GetCatInfoNoName(short vRefNum,
							   long dirID,
							   ConstStr255Param name,
							   CInfoPBPtr pb)
{
	Str31 tempName;
	OSErr error;
	
	/* Protection against File Sharing problem */
	if ( (name == NULL) || (name[0] == 0) )
	{
		tempName[0] = 0;
		pb->dirInfo.ioNamePtr = tempName;
		pb->dirInfo.ioFDirIndex = -1;	/* use ioDirID */
	}
	else
	{
		pb->dirInfo.ioNamePtr = (StringPtr)name;
		pb->dirInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
	}
	pb->dirInfo.ioVRefNum = vRefNum;
	pb->dirInfo.ioDrDirID = dirID;
	error = PBGetCatInfoSync(pb);
	pb->dirInfo.ioNamePtr = NULL;
	return ( error );
}

/*****************************************************************************/

pascal	OSErr	GetDirectoryID(short vRefNum,
							   long dirID,
							   ConstStr255Param name,
							   long *theDirID,
							   Boolean *isDirectory)
{
	CInfoPBRec pb;
	OSErr error;

	error = GetCatInfoNoName(vRefNum, dirID, name, &pb);
	if ( error == noErr )
	{
		*isDirectory = (pb.hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0;
		if ( *isDirectory )
		{
			*theDirID = pb.dirInfo.ioDrDirID;
		}
		else
		{
			*theDirID = pb.hFileInfo.ioFlParID;
		}
	}
	
	return ( error );
}

/*****************************************************************************/

pascal	OSErr	FSpGetDirectoryID(const FSSpec *spec,
								  long *theDirID,
								  Boolean *isDirectory)
{
	return ( GetDirectoryID(spec->vRefNum, spec->parID, spec->name,
			 theDirID, isDirectory) );
}

/*****************************************************************************/
