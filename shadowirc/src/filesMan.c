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

#include <Carbon/Carbon.h>

#include "StringList.h"
#include "IRCGlobals.h"
#include "filesMan.h"
#include "utils.h"
#include "Inline.h"
#include "connections.h"
#include "plugins.h"
#include "floaters.h"
#include "TextManip.h"
#include "IRCPreferences.h"
#include "IRCCFPrefs.h"

#include "MoreFiles.h"
#include "MoreFilesX.h"
#include "StringKeys.h"
#include "MsgWindows.h"
#include "DCC.h"

short mainRefNum = 0, mainResNum = 0;
static FSSpec mainPrefsLoc;

typedef struct fileListRec {
	struct fileListRec *next, *prev;
	short fref;
	short count;
	char res;
} fileListRec, *fileListPtr;

static fileListPtr fileList = 0;

static fileListPtr FileFind(short fref);

static void ReadInPrefs(void);
static void CreateNewPrefsMain(void);
static void CreateNewPrefsLinks(void);
static void CreateNewPrefs(void);
static void AllocateNewPrefs(void);
static void FolderErrorToConsole(CFStringRef errorKey);

static pascal void DirSelRefNavHook(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD);

static fileListPtr FileFind(short fref)
{
	fileListPtr f = fileList;
	
	while(f)
	{
		if(f->fref == fref)
			return f;
		f=f->next;
	}
	
	return 0;
}

pascal void FileAdd(short fref, char res)
{
	fileListPtr f = FileFind(fref);
	
	if(fref)
	{
		if(!f)
		{
			f=(fileListPtr)NewPtr(sizeof(fileListRec));
			f->next = fileList;
			f->prev = 0;
			if(f->next)
				f->next->prev = f;
			f->fref = fref;
			f->count = 1;
			f->res = res;
			fileList = f;
		}
		else
			f->count++;
	}
}

pascal OSErr FileClose(short fref)
{
	fileListPtr f = FileFind(fref);
	short err;
	
	if(f)
	{
		f->count--;
		if(!f->count)
		{
			if(f->prev)
				f->prev->next = f->next;
			if(f->next)
				f->next->prev = f->prev;
			if(f==fileList)
				fileList = f->next;
			
			if(f->res)
			{
				CloseResFile(fref);
				err = ResError();
			}
			else
				err = FSClose(fref);
			DisposePtr((Ptr)f);
			return err;
		}
		return 0; //should return no such fref error
	}
	else
		return FSClose(fref);
}

OSStatus FileCloseFork(short fref)
{
	fileListPtr f = FileFind(fref);
	short err;
	
	if(f)
	{
		f->count--;
		if(!f->count)
		{
			if(f->prev)
				f->prev->next = f->next;
			if(f->next)
				f->next->prev = f->prev;
			if(f==fileList)
				fileList = f->next;
			
			if(f->res)
			{
				CloseResFile(fref);
				err = ResError();
			}
			else
				err = FSCloseFork(fref);
			DisposePtr((Ptr)f);
			return err;
		}
		return 0; //should return no such fref error
	}
	else
		return FSCloseFork(fref);
}

#pragma mark -

pascal void OpenPrefs(void)
{
	if(!mainRefNum)
	{
		FSpOpenDF(&mainPrefsLoc, fsRdWrPerm, &mainRefNum);
		mainResNum = FSpOpenResFile(&mainPrefsLoc, fsRdWrPerm);
	}
}

pascal void ClosePrefs(void)
{
	if(mainRefNum)
	{
		FSClose(mainRefNum);
		mainRefNum=0;
	}
	
	if(mainResNum)
	{
		UpdateResFile(mainResNum);
		CloseResFile(mainResNum);
		mainResNum = 0;
	}
}

#pragma mark -

pascal OSErr FindAppSpec(FSSpec *appSpec)
{
	ProcessSerialNumber thisProcess = {0, kCurrentProcess};
	ProcessInfoRec processInfo;
	
	bzero(&processInfo, sizeof(ProcessInfoRec));
	
	processInfo.processInfoLength = sizeof(ProcessInfoRec);
	processInfo.processName = NULL;
	processInfo.processNumber = thisProcess;
	processInfo.processAppSpec = appSpec;
	
	return GetProcessInformation(&thisProcess, &processInfo);
}

pascal short CreateUniqueFile(FSSpec *file, OSType creator, OSType type)
{
	OSErr err;
	
	err = FSpCreate(file, creator, type, 0);
	if(err)
	{
		Str255 st;
		int i;
		int strend;
		
		pstrcpy(file->name, st);
		if(st[0] > 27)
			st[0] = 27;
		
		strend = st[0] +1;
		
		i = 1;
		while(err == dupFNErr)
		{
			NumToString(i, &st[strend]);
			st[0] = strend + st[strend];
			st[strend] = '-';
			if(st[0] > 31)
				st[0] = 31;
			pstrcpy(st, file->name);
			err = FSpCreate(file, creator, type, 0);
			i++;
		}
	}
	
	return err;
}

#pragma mark -

pascal void CleanFolderFSp(FSSpec *fss)
{
	CInfoPBRec pb;

	pb.dirInfo.ioCompletion=0;
	pb.dirInfo.ioNamePtr=fss->name;
	pb.dirInfo.ioVRefNum=fss->vRefNum;
	pb.hFileInfo.ioDirID=fss->parID;
	pb.dirInfo.ioFDirIndex=0;
	PBGetCatInfoSync(&pb);
	
	fss->parID = pb.hFileInfo.ioDirID;
	fss->name[0] = 0;
}

typedef struct DirSelRec {
	FSRef *ref;
	Boolean cancel;
} DirSelRec, *DirSelRecPtr;

static pascal void DirSelRefNavHook(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
{
	OSStatus err = noErr;
	DirSelRecPtr dsRecP = (DirSelRecPtr)callBackUD;

	switch (callBackSelector)
	{
		case kNavCBStart:
		{
			OSStatus err = noErr;
			AEDesc theLocation = {typeNull, NULL};
			FSRef localRef;
			
			if(FSRefValid(dsRecP->ref))
			{
				BlockMoveData(dsRecP->ref, &localRef, sizeof(FSRef));
				err = AECreateDesc(typeFSRef, &localRef, sizeof(FSRef), &theLocation);
				if (err == noErr)
					err = NavCustomControl(callBackParms->context, kNavCtlSetLocation, (void*)&theLocation);
			}
			break;
		}
		case kNavCBUserAction:
		{
			NavReplyRecord reply;
			NavUserAction userAction = 0;
			
			if((err = NavDialogGetReply(callBackParms->context, &reply)) == noErr)
			{
				userAction = NavDialogGetUserAction(callBackParms->context);
				switch(userAction)
				{
					case kNavUserActionChoose:
						err = UnpackFSRefFromNavReply(&reply, dsRecP->ref);
						break;
					
					case kNavUserActionCancel:
						dsRecP->cancel = TRUE;
						break;
				}
				err = NavDisposeReply(&reply);
			}
			break;
		}
		case kNavCBTerminate:	
			NavDialogDispose(callBackParms->context);
			break;
	}
}

OSStatus DirectorySelectButtonRef(FSRef *ref)
{
	OSStatus err = noErr;
	NavDialogRef dialog;
	NavDialogCreationOptions dialogOptions;
	NavEventUPP eventUPP;
	DirSelRecPtr dsRecP;
	
	eventUPP = NewNavEventUPP(DirSelRefNavHook);
	
	err = NavGetDefaultDialogCreationOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavGetFolder;
	
	dialogOptions.message = CFCopyLocalizedString(kDirSelKey, NULL);
	
	dsRecP = (DirSelRecPtr)NewPtr(sizeof(DirSelRec));
	if(dsRecP != NULL)
	{
		dsRecP->ref = ref;
		dsRecP->cancel = FALSE;
	}
	
	if((err = NavCreateChooseFolderDialog(&dialogOptions, eventUPP, NULL, dsRecP, &dialog)) == noErr)
	{
		if(dialog != NULL)
		{
			if((err = NavDialogRun(dialog)) != noErr)
			{
				if(dialogOptions.message)
					CFRelease(dialogOptions.message);
				if(dsRecP)
					DisposePtr((Ptr)dsRecP);
				NavDialogDispose(dialog);
				DisposeNavEventUPP(eventUPP);
			}
			
			if(dsRecP->cancel == TRUE)
				err = paramErr;
		}
	}
	
	if(dialogOptions.message)
		CFRelease(dialogOptions.message);
	if(dsRecP)
		DisposePtr((Ptr)dsRecP);
	DisposeNavEventUPP(eventUPP);
	
	return err;
}

#pragma mark -

OSStatus CreateFileRef(CFStringRef name, const FSRef *parentRef, OSType fileCreator, OSType fileType, Boolean replacing, FSRef *ref)
{
	OSStatus err = noErr;
	UniChar *nameBuf = NULL;
	UniCharCount nameBufLen = 0;
	
	nameBufLen = (UniCharCount)CFStringGetLength(name);
	nameBuf = (UniChar *)NewPtr(nameBufLen * sizeof(UniChar));
	CFStringGetCharacters(name, CFRangeMake(0, nameBufLen), &nameBuf[0]);
	
	if(nameBuf != NULL)
	{
		FSCatalogInfo catalogInfo;
		
		if(replacing)
		{
			FSRef fileToDelete;
			if((err = FSMakeFSRefUnicode(parentRef, nameBufLen, nameBuf, kTextEncodingUnicodeDefault, &fileToDelete)) == noErr)
				err = FSDeleteObject(&fileToDelete);
			
			if(err != noErr) return err;
		}
		
		catalogInfo.textEncodingHint = kTextEncodingUnicodeDefault;
		
		err = FSCreateFileUnicode(parentRef, nameBufLen, nameBuf, kFSCatInfoTextEncoding, &catalogInfo, ref, NULL);
		
		if(err == noErr)
			FSChangeCreatorType(ref, fileCreator, fileType);
		
		DisposePtr((Ptr)nameBuf);
	}
	
	return err;
}

OSStatus UnpackFSRefFromNavReply(const NavReplyRecord *reply, FSRef *ref)
{
	OSStatus err = noErr;
	AEDesc actualDesc;
	FSRef fileRefParent;
	
	if((err = AECoerceDesc(&reply->selection, typeFSRef, &actualDesc)) == noErr)
	{
		if(&actualDesc != NULL)
			err = AEGetDescData(&actualDesc, (void *)&fileRefParent, sizeof(FSRef));
		
                if(err == noErr)
		{
			BlockMoveData(&fileRefParent, ref, sizeof(FSRef));
			err = AEDisposeDesc(&actualDesc);
		}
                else
		{
			BlockZero(ref, sizeof(FSRef));
			return err;
		}
	}
        
	return err;
}

OSStatus GetFSRefForResourcesFolder(FSRef *ref)
{
	OSStatus err = noErr;
	CFBundleRef bundle;
	CFURLRef resourcesURL;
	FSRef resourcesRef;
	
	bundle = CFBundleGetMainBundle();
	resourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
	
	if(resourcesURL != NULL)
	{
		if(CFURLGetFSRef(resourcesURL, &resourcesRef))
			BlockMoveData(&resourcesRef, ref, sizeof(FSRef));
		else
		{
			ref = NULL;
			err = paramErr;
		}
	}
	else
	{
		ref = NULL;
		err = paramErr;
	}
	
	return err;
}

PicHandle LoadAppLogoFromResources(void)
{
	OSStatus err = noErr;
	FSRef resourcesRef;
	PicHandle picture = NULL;
	
	err = GetFSRefForResourcesFolder(&resourcesRef);
	
	if(err == noErr)
	{
		CFStringRef fileName = CFSTR("ShadowIRCSplash.pict");
		FSRef pictureRef;
		UniChar *nameBuffer = NULL;
		UniCharCount sourceLength;
		
		sourceLength = (UniCharCount) CFStringGetLength(fileName);
		
		nameBuffer = (UniChar *)NewPtr(sourceLength * sizeof(UniChar));
		if(nameBuffer == NULL)
			return NULL;
		
		CFStringGetCharacters(fileName, CFRangeMake(0, sourceLength), &nameBuffer[0]);
		
		if((err = FSMakeFSRefUnicode(&resourcesRef, sourceLength, nameBuffer, kTextEncodingUnicodeDefault, &pictureRef)) == noErr)
		{
			HFSUniStr255 forkName;
			SInt16 forkRefNum = 0;
			
			err = FSGetDataForkName(&forkName);
			
			err = FSOpenFork(&pictureRef, forkName.length, forkName.unicode, fsRdPerm, &forkRefNum);
			if(err == noErr)
			{
				SInt64 forkSize, pictLength;
				
				FSGetForkSize(forkRefNum, &forkSize);
				
				pictLength = forkSize - 512;
				picture = (PicHandle)NewHandle(pictLength);
				if(picture != NULL)
				{
					HLock((Handle)picture);
					
					err = FSReadFork(forkRefNum, fsFromStart, 512, pictLength, *picture, NULL);
					
					HUnlock((Handle)picture);
				}
				
				FSCloseFork(forkRefNum);
			}
		}
		
		DisposePtr((Ptr)nameBuffer);
	}
	
	return picture;
}

pascal char MyStandardPutFile(ConstStr255Param message, ConstStr255Param fileName, long type, long creator, long navFlags, FSSpec *f, char allowReplace)
{
	char ret = kNoErr;

	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	OSErr err;

	NavGetDefaultDialogOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavPutFile;
	dialogOptions.dialogOptionFlags = navFlags;

	if(message)
		pstrcpy(message, dialogOptions.message);
	if(fileName)
		pstrcpy(fileName, dialogOptions.savedFileName);
	pstrcpy("\pShadowIRC", dialogOptions.clientName);
	
	err = NavPutFile(0, &theReply, &dialogOptions, NULL, type, creator, 0);

	if(theReply.validRecord && !err)
	{
		AEDesc resultDesc;	
		AEKeyword key;
		
		resultDesc.dataHandle = 0L;
		
		// retrieve the returned selection:
		if((err = AEGetNthDesc(&(theReply.selection),1,typeFSS, &key,&resultDesc)) == noErr)
		{
			AEGetDescData(&resultDesc, f, sizeof(FSSpec));

			if(theReply.replacing)
			{
				if(allowReplace)
				{
					if(FSpDelete(f)) //display error!
						ret = kCantReplace;
					else
						ret = kFileReplaced;
				}
				else
					ret = kNoReplace;
			}
				
			AEDisposeDesc(&resultDesc);
		}

		NavDisposeReply(&theReply);
	}
	else
		ret = kUserCanceled;
	
	return ret;
}

#pragma mark -

enum PrefsData {
	kShadowIRC10PreferencesVersion = 1,
	kShadowIRC11OldPreferencesVersion = 2,
	kShadowIRC11PreferencesVersion = 3,
	kShadowIRC12PreferencesVersion = 3,
	PreferencesVersion = kShadowIRC11PreferencesVersion,
	PrefsUpdateLevel = 5,
	SizeOfPrefsDataArea = sizeof(prefsRec) + (numSIColors * sizeof(RGBColor)) + (sizeof(linkPrefsRec)*maxLinks),
	SizeOfPrefs = sizeof(prefsStruct) + SizeOfPrefsDataArea
};

pascal void writeMainPrefs(void)
{
	long l;
	prefsStruct p;
	
	OpenPrefs();
	SetFPos(mainRefNum, fsFromStart, 0);
	l=SizeOfPrefs;
	SetEOF(mainRefNum, l);

	p.version=PreferencesVersion;
	p.prefsRecSize=sizeof(prefsRec);
	p.numColors=numSIColors;
	p.linkPrefsSize=sizeof(linkPrefsRec);
	
	p.prefsUpdate = PrefsUpdateLevel;
	for(l=0;l<19;l++)
		p.padding[l]=0;
	
	l=sizeof(prefsStruct);
	FSWrite(mainRefNum, &l, (Ptr)&p);
	l=sizeof(prefsRec);
	FSWrite(mainRefNum, &l, (Ptr)mainPrefs);
	l=sizeof(RGBColor) * numSIColors;
	FSWrite(mainRefNum, &l, (Ptr)shadowircColors);
	l=sizeof(linkPrefsRec) * 10;
	FSWrite(mainRefNum, &l, (Ptr)linkPrefsArray);
	
	ClosePrefs();
        

}

pascal void writeAllFiles(void)
{
	writeMainPrefs();
	runAllPlugins(pSavePreferencesMessage, 0);
	
	// Sync the CFPrefs to the data store. 
	// Currently we'll place this here. This might move in
	// the source files to a more appropriate location.
	CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
}

static void ReadInPrefs(void)
{
	prefsStruct p;
	long l=sizeof(p);
	long l2, l3=SizeOfPrefs;
	ConstStringPtr s, s2;
	int x;
	char valid;
	linkPtr lp;
	
	FSRead(mainRefNum, &l, &p);
	if(p.version==kShadowIRC10PreferencesVersion)
	{
		GetEOF(mainRefNum, &l2);
		
		valid = (l2 <= l3 && l2 >= (l3 - sizeof(RGBColor) * 2));
		if(!valid)
		{
			s2 = GetIntStringPtr(spError, sErrorLoadingPrefs);
			s =  GetIntStringPtr(spError, sOldOrDamagedPrefs);
			ParamText(s2, "\p", s, "\p");
			Alert(130, 0);
			ExitToShell();
			return;
		}
	}
	
	//Prefs are ok.
	l=sizeof(prefsRec);
	FSRead(mainRefNum, &l, (Ptr)mainPrefs);
	l=p.numColors * sizeof(RGBColor);
	FSRead(mainRefNum, &l, (Ptr)shadowircColors);
	l=sizeof(linkPrefsRec) * 10;
	FSRead(mainRefNum, &l, (Ptr)linkPrefsArray);
	
	if(p.version <= kShadowIRC11OldPreferencesVersion)
	{
		mainPrefs->colorMethod++;
		p.prefsUpdate = 2;
	}
	
	if(p.version == kShadowIRC11PreferencesVersion)
	{
		switch(p.prefsUpdate)
		{
			case 2:
				mainPrefs->displayJoin =
				mainPrefs->displayPart =
				mainPrefs->displayQuit =
				mainPrefs->displayModes = 
				mainPrefs->displayNicks = 
				mainPrefs->displayKicks = 
				mainPrefs->displayInvites = 1;
				mainPrefs->dccSendPacketSize = 4096;

				shadowircColors[13].red=25368;
				shadowircColors[13].green=0;
				shadowircColors[13].blue=25368;

				shadowircColors[14].red=
				shadowircColors[14].green=
				shadowircColors[14].blue=0;
				
				mainPrefs->userlistInWindowRight = 1;
				mainPrefs->nonGlobalInput = 0;
				
				for(x=0;x<17;x++)
					p.padding[x] = 0;
			
			case 3: //fix the position of recent nicks/channels
				for(x = 0; x < 10; x++)
				{
					linkPrefsPtr lpp = &linkPrefsArray[x];
					BlockMoveData(&lpp->unusedX, lpp->recentNicks, (64 * 10 + 256 * 10));
					lpp->unusedX = 0;
					lpp->regainNick = 0;
				}
				
			case 4:
				mainPrefs->dccUsePortRange = 0;
				mainPrefs->dccPortRangeLow = 2048;
				mainPrefs->dccPortRangeHigh = 4096;
				
				p.prefsUpdate = PrefsUpdateLevel;
		}
	}
	else if(p.version > PreferencesVersion)
	{
		s2 = GetIntStringPtr(spError, sErrorLoadingPrefs);
		s = GetIntStringPtr(spError, sPrefsVersionError);
//		else
//			s = GetIntStringPtr(spError, sUnrecognizedPrefs);
		ParamText(s2, "\p", s, "\p");
		Alert(130,0);
		ExitToShell();
	}
	
	//Note that the links haven't been set up yet!!
	//Consistency check the connection names
	x=0;
	linkfor(lp, firstLink)
	{
		x++;
		if(!lp->linkPrefs->linkName[0])
		{
			NumToString(x, &lp->linkPrefs->linkName[13]);
			valid = lp->linkPrefs->linkName[13];
			pstrcpy("\p¥¥Connection ", lp->linkPrefs->linkName);
			lp->linkPrefs->linkName[0] += valid;
		}
	}

	//write this back out
	SetFPos(mainRefNum, fsFromStart, 0);
	l=sizeof(p);
	FSWrite(mainRefNum, &l, &p);
}

OSStatus UseDirFSRef(const FSRef *parentRef, CFStringRef string, Boolean create, FSRef *ref)
{
	OSStatus err = noErr;
	FSCatalogInfo catalogInfo;
	UniChar *nameBuf = NULL;
	UniCharCount bufLen = 0;
	
	bufLen = (UniCharCount)CFStringGetLength(string);
	
	if(bufLen)
	{
		nameBuf = (UniChar *)NewPtr(bufLen * sizeof(UniChar));
		err = MemError();
		if(err != noErr)
			return err;
		
		CFStringGetCharacters(string, CFRangeMake(0, bufLen), &nameBuf[0]);
		
		if(nameBuf != NULL)
		{
			// To be a good directory creation citizen on OS X,
			// Apple recommends setting the text encoding hint (if nothing else)
			catalogInfo.textEncodingHint = kTextEncodingUnicodeDefault;
			
			err = FSMakeFSRefUnicode(parentRef, bufLen, nameBuf, kTextEncodingUnicodeDefault, ref);
			if((err == fnfErr) && (create == TRUE))
				err = FSCreateDirectoryUnicode(parentRef, bufLen, nameBuf, kFSCatInfoTextEncoding, &catalogInfo, ref, NULL, NULL);
			
			DisposePtr((Ptr)nameBuf);
		}
		else
			err = paramErr;
	}
	else
		err = paramErr;
	
	return err;
}

static void CreateNewPrefsMain(void)
{
	prefsPtr mp=mainPrefs;
	int x;
	
	pstrcpy("\p*** ", mp->serverMessagePrefix);
	ZeroRect(mp->consoleLoc);
	ZeroRect(mp->inputLoc);
mp->unusedIW=0;
	mp->consoleOpen=true;
	mp->userListOpen=0;
	ZeroRect(mp->userListRect);
	
	pstrcpy("\pMonaco", mp->defaultFontName);
	mp->defaultFontSize=9;
	mp->noFontsMenu=0;
	mp->disableColor=0;
	mp->colorMethod = cmIrcle;
	mp->disableStyles=0;
	mp->squelchBeeps=0;
	mp->textIndenting=1;
	mp->ircIIDisplay=0;
	mp->timestamp=0;
	mp->timestampSeconds=0;
	mp->timestampWhenAway=0;
	mp->escClearsInputline=1;
mp->unused5 = 0;
	mp->showEndMessages=0;
	mp->showUserHostsWithMsgs = 0;
	mp->beepOnPrivmsgs=0;
	mp->wallopsToConsole=0;
	mp->serverNoticesToConsole=0;
	mp->privmsgsToConsole=0;
	mp->displayPingsInConsole=1;
	mp->autoQuery=0;
	mp->autoRejoinAfterKick=0;
	mp->autoRejoinReconnect=1;
	mp->autoacceptInvites=0;
	mp->autoacceptInvitesWhenNotAway=0;
	mp->optionToMoveInputLine=0;
	mp->hideSplashScreen=0;
	mp->doWhowasIfFailedWhois=0;
	mp->dontActivateNewWindowsIfInputlineText=0;
	mp->quitAction = qaAutoSave;
	mp->dccWindowAutoOpen = 0;
	mp->dccUsePortRange = 0;

	mp->disableCTCP=0;
	mp->noCTCPUnknownErrMsg=1;
	mp->disableCTCPMessages=0;
	mp->enableDCC=1;
	mp->autoAcceptDCCChat=0;
	mp->autoAcceptDCCChatWhenNotAway=0;
	mp->autoLogging=0;
	mp->autoLogQueries=0;
	mp->autoLogDCCChat=0;
	mp->autoQueryOpen = 0;
	mp->nonGlobalInput = 0;
	
	mp->dccPortRangeLow = 2048;
	mp->dccPortRangeHigh = 4096;
	
	for(x=0;x<64;x++)
		mp->unused[x] = 0;

	for(x=0;x<30;x++)
		mp->shortcuts[x][0]=0;

	for(x=0;x<70;x++)
		mp->unused2[x] = 0;

	mp->autoDCCGet = 0;
	mp->autoSaveDCC = 0;
	mp->socksHost[0]=0;
	mp->socksPort=1080;
	mp->firewallType=0;
	
	mp->displayJoin =
	mp->displayPart =
	mp->displayQuit =
	mp->displayModes = 1;
	mp->FastDCCSends = 0;
	
	mp->unused3[0] = 
	mp->unused3[1] = 
	mp->unused3[2] = 
	mp->unused3[3] = 0;
	
	mp->displayNicks = 1;
	mp->noModesWidget = 0;
	mp->ddToSameWin = 0;
	mp->ctcpToConsole = 0;
	mp->dccSendPacketSize = 4096;

	mp->displayKicks = 1;
	mp->dccReverseSends = 0;
	mp->dccWindowOpen = 0;
	mp->displayInvites= 1;
	
	mp->socksUser[0] = mp->socksPass[0] = 0;
	ZeroRect(mp->dccWindowRect);
	mp->userlistNickWidth = 80;
	mp->userlistSort = 0;
	mp->userlistSortReverse = 0;
	mp->userlistInWindowWidth = 100;
	mp->userlistInWindow = 0;
	mp->resendAwayOnReconnect = 0;
	mp->userlistInWindowRight = 1;
	mp->userlistScrollbarLeft = 0;
	
	for(x=0;x<896 + (64);x++) // + 64 == pass
		mp->spare[x]=0;
}

static void CreateNewPrefsLinks(void)
{
	linkPrefsPtr lp;
	int x, y;

	for(x=0;x<maxLinks;x++)
	{
		lp=&linkPrefsArray[x];
		
		pstrcpy("\pConnection  ", lp->linkName);
		if(x<9)
			lp->linkName[12]='1'+x;
		else
		{
			lp->linkName[12] = '1';
			lp->linkName[13] = '0';
			lp->linkName[0]=13;
		}
		pstrcpy("\pNewUser", lp->nick);
		pstrcpy("\pNew_User", lp->alterNick);
		pstrcpy("\pshadowirc", lp->user);
		pstrcpy("\pShadowIRC", lp->real);
		lp->autoConnect=false;
		lp->reconnect=false;
		lp->displayMOTD=true;
		lp->useConnectMacro = false;
		pstrcpy("\p/join #macintosh", lp->connectMacro);
		pstrcpy("\pmesa.az.us.undernet.org", lp->serverName);
		lp->networkID = 'unet';
		lp->serverPass[0]=0;
		lp->serverPort=6667;
		lp->modeI=false;
		lp->modeW=false;
		lp->modeS=false;
		lp->onoticeMethod=1;
		for(y=0;y<10;y++)
		{
			lp->recentNicks[y][0]=0;
			lp->recentChannels[y][0]=0;
			
			ZeroRect(lp->windowLoc[y]);
		}
		
		lp->maxConnAtt=0;
		lp->retryDelay=0;
		lp->signoffMessage[0] = 0;
		lp->fingerMessage[0] = 0;
		lp->userinfoMessage[0] = 0;
		
		lp->regainNick = 0;
		lp->unusedC[0] = 
		lp->unusedC[1] =
		lp->unusedC[2] = 0;
		lp->unused[0] = 
		lp->unused[1] = 0;
		
		lp->kickMessage[0] = 0;
		
		for(y=0;y<763;y++)
			lp->spare[y]=0;
	}
}

static void CreateNewPrefs(void)
{
	CreateNewPrefsMain();
	CreateNewPrefsLinks();
	InitColorPrefs();
}

static void AllocateNewPrefs(void)
{
	mainPrefs=(prefsPtr)NewPtrClear(sizeof(prefsRec));
	linkPrefsArray=(linkPrefsPtr)NewPtrClear(sizeof(linkPrefsRec) * maxLinks);
	shadowircColors=(RGBColor*)NewPtr(sizeof(RGBColor)*(numSIColors)); //don't clear this because ALL of it is being set
}

static void FolderErrorToConsole(CFStringRef errorKey)
{
	CFStringRef string;
	Str255 pstring;
	LongString ls;
	
	string = CFCopyLocalizedString(errorKey, NULL);
	CFStringGetPascalString(string, pstring, sizeof(Str255), CFStringGetSystemEncoding());
	CFRelease(string);
	
	LSStrLS(pstring, &ls);
	LineMsg(&ls);
}

pascal char readMainPrefs(void)
{
	short vref;
	OSErr err;
	char created;
	Boolean isDir;
	FSSpec f2;
	FSSpec ShadowIRCFolder;
	FSRef localRef;
	long ShadowIRCFolderDirID, PreferencesFolderDirID;
	
	err = FindFolder(kUserDomain, kPreferencesFolderType, kDontCreateFolder, &vref, &PreferencesFolderDirID);
	err = FSMakeFSSpec(vref, PreferencesFolderDirID, GetIntStringPtr(spFiles, sPreferencesFolder), &ShadowIRCFolder);

	if(err) //this means that there's no preferences folder
	{
		if(FSpDirCreate(&ShadowIRCFolder, 0, &ShadowIRCFolderDirID)) //error creating the prefs folder
		{
			ExitToShell();
			return 0;
		}
		else
		{
			//Make the Prefs File loc
			err = FSMakeFSSpec(vref, ShadowIRCFolderDirID, GetIntStringPtr(spFiles, sPreferencesFile), &mainPrefsLoc);

			//Check for existance of ShadowIRC Prefs in root folder and move it there.
			if(!FSMakeFSSpec(vref, PreferencesFolderDirID, GetIntStringPtr(spFiles, sPreferencesFile), &f2)) //Then we have the prefs file. Move it. Otherwise, do nothing.
				err = FSpCatMove(&f2, &ShadowIRCFolder);
		}
	}
	else //this means that there is a preferences folder. Open the file in it.
	{
		err = FSpGetDirectoryID(&ShadowIRCFolder, &ShadowIRCFolderDirID, &isDir);
		err = FSMakeFSSpec(vref, ShadowIRCFolderDirID, GetIntStringPtr(spFiles, sPreferencesFile), &mainPrefsLoc);
	}
	
	AllocateNewPrefs();
	if(!err)
	{
		created = false;
		err=FSpOpenDF(&mainPrefsLoc, fsRdPerm, &mainRefNum);

		mainResNum=FSpOpenResFile(&mainPrefsLoc, fsRdPerm);
		if(ResError())
		{
			FSpCreateResFile(&mainPrefsLoc, kApplicationSignature, kPrefsType, 0);
			mainResNum=FSpOpenResFile(&mainPrefsLoc, fsRdPerm);
			if(ResError())
				SysBeep(0);
		}
		SetFPos(mainRefNum, fsFromStart, 0);
		ReadInPrefs();
	}
	else
	{
		created = true;
		err = FSpCreate(&mainPrefsLoc, kApplicationSignature, kPrefsType, 0);
		err=FSpOpenDF(&mainPrefsLoc, fsRdWrPerm, &mainRefNum);
		mainResNum=FSpOpenResFile(&mainPrefsLoc, fsRdWrPerm);
		if(ResError())
		{
			FSpCreateResFile(&mainPrefsLoc, kApplicationSignature, kPrefsType, 0);
			mainResNum=FSpOpenResFile(&mainPrefsLoc, fsRdWrPerm);
			if(ResError())
				SysBeep(0);
		}

		CreateNewPrefs();
		writeMainPrefs();
	}
	
	ReadShortCutDataCFPrefs(mainPrefs->shortcuts); // This will go somewhere else.
	
	err = SetupDCCFolder(&localRef);
	if(err == noErr)
		err = FSGetCatalogInfo(&localRef, kFSCatInfoNone, NULL, NULL, &dccFolderFSp, NULL);
	else if(err == paramErr)
	{
		FolderErrorToConsole(kNoDCCFolderKey);
		err = noErr;
	}
	
	err = SetupLogFolder(&localRef);
	if(err == noErr)
		BlockMoveData(&localRef, &logFolderRef, sizeof(FSRef));
	else if(err == paramErr)
	{
		FolderErrorToConsole(kNoLogFolderKey);
		err = noErr;
	}
        
	ClosePrefs();
	
	return created;
}
