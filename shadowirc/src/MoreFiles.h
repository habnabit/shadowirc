#ifndef _MOREFILES_
#define _MOREFILES_

pascal	OSErr GetCatInfoNoName(short vRefNum,
							   long dirID,
							   ConstStr255Param name,
							   CInfoPBPtr pb);

pascal	OSErr	GetDirectoryID(short vRefNum,
							   long dirID,
							   ConstStr255Param name,
							   long *theDirID,
							   Boolean *isDirectory);


pascal	OSErr	FSpGetDirectoryID(const FSSpec *spec,
								  long *theDirID,
								  Boolean *isDirectory);

pascal	OSErr	FSpGetFullPath(const FSSpec *spec,
							   short *fullPathLength,
							   Handle *fullPath);

#endif