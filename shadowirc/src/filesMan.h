/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.com

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _filesman_
#define _filesman_

enum navPrefKey { 
	kNavGetFile,
	kNavGetFolder,
	kNavPutFile,
	kNavGetDCCFile
};

enum FileErrors {
	//Successful
	kFileReplaced = -1,
	kNoErr = 0,
	
	kUserCanceled = 1,
	kNoReplace = 2,
	kCantReplace = 3
};

/*
	kNavNoTypePopup				= 0x00000001,					//don't show file type/extension popup on Open/Save
	kNavDontAutoTranslate		= 0x00000002,					//don't automatically translate on Open
	kNavDontAddTranslateItems	= 0x00000004,					//don't add translation choices on Open/Save
	kNavAllFilesInPopup			= 0x00000010,					//"All Files" menu item in the type popup on Open
	kNavAllowStationery			= 0x00000020,					//allow saving of stationery files
	kNavAllowPreviews			= 0x00000040,					//allow to show previews
	kNavAllowMultipleFiles		= 0x00000080,					//allow multiple items to be selected
	kNavAllowInvisibleFiles		= 0x00000100,					//allow invisible items to be shown
	kNavDontResolveAliases		= 0x00000200,					//don't resolve aliases
	kNavSelectDefaultLocation	= 0x00000400,					//make the default location the browser selection
	kNavSelectAllReadableItem	= 0x00000800					//make the dialog select "All Readable Documents" on Open
*/

enum NavOpts {
	kNDefault = 0x000000E4,

	kNNoTypePopup = kNDefault | 0x00000001,	//kNavNoTypePopup
	kNOneFile = kNDefault & ~0x00000080,			//kNavAllowMultipleFiles

	kNOneFileNoTypePopup =  0x00000045
};

enum openRsrcs {
	kOpenColor = 301
};

pascal void writeMainPrefs(void);
pascal void writeAllFiles(void);
pascal char readMainPrefs(void); //Return true if the prefs were just created

OSStatus UseDirFSRef(const FSRef *parentRef, CFStringRef string, Boolean create, FSRef *ref);
OSStatus CreateFileRef(CFStringRef name, const FSRef *parentRef, OSType fileCreator, OSType fileType, Boolean replacing, FSRef *ref);

OSStatus UnpackFSRefFromNavReply(const NavReplyRecord *reply, FSRef *ref);
OSStatus GetFSRefForResourcesFolder(FSRef *ref);
PicHandle LoadAppLogoFromResources(void);

void FolderErrorToConsole(CFStringRef errorKey);

pascal char MyStandardPutFile(ConstStr255Param message, ConstStr255Param fileName, long type, long creator, long navFlags, FSSpec *f, char allowReplace);

OSStatus DirectorySelectButtonRef(FSRef *ref);
pascal void CleanFolderFSp(FSSpec *fss);

pascal void FileAdd(short fref, char res);
pascal OSErr FileClose(short fref);
OSStatus FileCloseFork(short fref);
pascal short CreateUniqueFile(FSSpec *file, OSType creator, OSType type);

pascal void OpenPrefs(void);
pascal void ClosePrefs(void);
extern short mainResNum;
#endif