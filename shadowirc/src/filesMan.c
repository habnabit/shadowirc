/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
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

#include "MyMemory.h"
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

#include "MoreFiles.h"

/*
	Best way to get fsspec of something in the app's dir:
	GetProcessInformation() to get a spec to your application, then replace the name field of that spec with the other file name.
*/
NavEventUPP StdNavFilter;

enum aliases {
	logAlias = 128,
	dccAlias = 129
};

short mainRefNum = 0, mainResNum = 0;
static FSSpec mainPrefsLoc;

typedef struct fileListRec {
	struct fileListRec *next, *prev;
	short fref;
	short count;
	char res;
} fileListRec, *fileListPtr;

static fileListPtr fileList = 0;

static pascal fileListPtr FileFind(short fref);

static pascal void ReadInPrefs(void);
static pascal void CreateNewPrefsMain(void);
static pascal void CreateNewPrefsLinks(void);
static pascal void CreateNewPrefs(void);
static pascal void AllocateNewPrefs(void);

static pascal void WriteAlias(const FSSpec *fs, short id);
static pascal void ReadAlias(FSSpec *fs, short id);

static pascal fileListPtr FileFind(short fref)
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
	
	MZero(&processInfo, sizeof(ProcessInfoRec));
	
	processInfo.processInfoLength = sizeof(ProcessInfoRec);
	processInfo.processName = NULL;
	processInfo.processNumber = thisProcess;
	processInfo.processAppSpec = appSpec;
	
	return GetProcessInformation(&thisProcess, &processInfo);
}

pascal char ValidFSSpec(const FSSpec *f)
{
	FSSpec f2;
	return !FSMakeFSSpec(f->vRefNum, f->parID, f->name, &f2);
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

pascal char DirectorySelectButton(FSSpec *fss)
{
	NavReplyRecord theReply;
	NavDialogOptions dialogOptions;
	OSErr theErr = noErr;
	char b = 0;
	AEKeyword key;
	
	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavGetFolder;
	
	GetIntString(dialogOptions.message, spFile, sPleaseChooseAFolder);
	
	theErr = NavChooseFolder(NULL, &theReply, &dialogOptions, StdNavFilter, NULL, 0);

	if ((theReply.validRecord)&&(theErr == noErr))
	{
		// grab the target FSSpec from the AEDesc:	
		AEDesc 	resultDesc;

		if((theErr = AEGetNthDesc(&(theReply.selection),1,typeFSS, &key,&resultDesc)) == noErr)
		{
			AEGetDescData(&resultDesc, fss, sizeof(FSSpec));
			CleanFolderFSp(fss);
			b = 1;
		}

		AEDisposeDesc(&resultDesc);
		theErr = NavDisposeReply(&theReply);
	}
	
	return b;
}

#pragma mark -

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
	
	err = NavPutFile(0, &theReply, &dialogOptions, StdNavFilter, type, creator, 0);

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

pascal OSErr PFOpen(ConstStr255Param name, char resFork, short *refNum, FSSpec *fss)
{
	FSSpec temp=pluginPrefsFSSpec;
	short err;
	
	pstrcpy(name, temp.name);
	if(resFork)
	{
		*refNum=FSpOpenResFile(&temp, fsRdWrPerm);
		err=ResError();
	}
	else
		err=FSpOpenDF(&temp, fsRdWrPerm, refNum);
	
	FileAdd(*refNum, resFork);

	if(fss)
		*fss = temp;
	
	return err;
}

pascal char PFExists(ConstStr255Param name)
{
	FSSpec temp=pluginPrefsFSSpec;
	FInfo f;
	
	pstrcpy(name, temp.name);
	return !FSpGetFInfo(&temp, &f);
	
}

pascal OSErr PFCreate(ConstStr255Param name, FourCharCode type, FourCharCode creator, char resFork)
{
	FSSpec temp=pluginPrefsFSSpec;
	
	pstrcpy(name, temp.name);
	if(resFork)
	{
		FSpCreateResFile(&temp, creator, type, 0);
		return ResError();	
	}
	else
	{
		return FSpCreate(&temp, creator, type, 0);
	}
}

pascal OSErr PFWrite(short refNum, const void* p, long *size)
{
	return FSWrite(refNum, size, p);
}

pascal OSErr PFRead(short refNum, void* p, long *size)
{
	return FSRead(refNum, size, p);
}

pascal OSErr PFSize(short refNum, long *size)
{
	return GetEOF(refNum, size);
}

pascal OSErr PFResize(short refNum,  long newSize)
{
	return SetEOF(refNum, newSize);
}

pascal OSErr PFSetPos(short refNum, long pos)
{
	return SetFPos(refNum, fsFromStart, pos);
}

pascal OSErr PFClose(short refNum)
{
	return FileClose(refNum);
}

pascal OSErr PFDelete(ConstStr255Param name)
{
	FSSpec temp=pluginPrefsFSSpec;

	pstrcpy(name, temp.name);
	return FSpDelete(&temp);
}

#pragma mark -

enum PrefsData {
	kShadowIRC10PreferencesVersion = 1,
	kShadowIRC11OldPreferencesVersion = 2,
	kShadowIRC11PreferencesVersion = 3,
	kShadowIRC12PreferencesVersion = 3,
	PreferencesVersion = kShadowIRC11PreferencesVersion,
	PrefsUpdateLevel = 4,
	SizeOfPrefsDataArea = sizeof(prefsRec) + (numSIColors * sizeof(RGBColor)) + (sizeof(linkPrefsRec)*maxLinks),
	SizeOfPrefs = sizeof(prefsStruct) + SizeOfPrefsDataArea
};

static pascal void WriteAlias(const FSSpec *fs, short id)
{
	AliasHandle alias;
	Handle rh;
	short err;
	long siz;

	if(mainResNum)
	{
		err = NewAlias(0, fs, &alias);
		if(!err)
		{
			rh = Get1Resource('alis', id);
			if(rh)
			{
				siz =GetHandleSize((Handle)alias);
				SetHandleSize(rh, siz);
				BlockMoveData(*alias, *rh, siz);
				ChangedResource(rh);
				DisposeHandle((Handle)alias);
			}
			else
			{
				AddResource((Handle)alias, 'alis', id, 0);
				rh = (Handle)alias;
			}
			if(rh)
			{
				WriteResource(rh);
				ReleaseResource(rh);
				UpdateResFile(mainResNum);
			}
		}
	}
}

static pascal void ReadAlias(FSSpec *fs, short id)
{
	AliasHandle alias;
	short err;
	Boolean b;
	
	if(mainResNum)
	{
		alias = (AliasHandle)Get1Resource('alis', id);
		
		if(alias)
		{
			err = ResolveAlias(0, alias, fs, &b);
			CleanFolderFSp(fs);
			if(b)
			{
				ChangedResource((Handle)alias);
				WriteResource((Handle)alias);
			}
			if(!err)
				ReleaseResource((Handle)alias);
		}
	}
}

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
	
	//Make aliases for the fsspecs and save them
	WriteAlias(&logFolderFSp, logAlias);
	WriteAlias(&dccFolderFSp, dccAlias);
	ClosePrefs();
}

pascal void writeAllFiles(void)
{
	writeMainPrefs();
	runAllPlugins(pSavePreferencesMessage, 0);
}

static pascal void ReadInPrefs(void)
{
	prefsStruct p;
	long l=sizeof(p);
	long l2, l3=SizeOfPrefs;
	ConstStringPtr s, s2;
	int x;
	char valid;
	linkPtr lp;
	short resNum;
	
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
				mainPrefs->unused4 = 0;
				
				for(x=0;x<17;x++)
					p.padding[x] = 0;
			
			case 3: //fix the position of recent nicks/channels
				for(x = 0; x < 10; x++)
				{
					linkPrefsPtr lpp = &linkPrefsArray[x];
					BlockMoveData(&lpp->unusedX, lpp->recentNicks, (64 * 10 + 256 * 10));
					lpp->unusedX = 0;
					lpp->regainNick = 0;
					lpp->isTalkCity = 0;
				}
				
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
	resNum = CurResFile();
	UseResFile(mainResNum);
	ReadAlias(&logFolderFSp, logAlias);
	ReadAlias(&dccFolderFSp, dccAlias);
	UseResFile(resNum);
}

static pascal void CreateNewPrefsMain(void)
{
	prefsPtr mp=mainPrefs;
	int x;
	
	pstrcpy("\p*** ", mp->serverMessagePrefix);
	ZeroRect(mp->consoleLoc);
	ZeroRect(mp->inputLoc);
	mp->inputLineMemoryDisplay=0;
	mp->consoleOpen=true;
	mp->userListOpen=0;
	ZeroRect(mp->userListRect);
	
	pstrcpy("\pMonaco", mp->defaultFontName);
	mp->defaultFontSize=9;
	mp->noFontsMenu=0;
	mp->disableColor=0;
	mp->colorMethod=0;
	mp->disableStyles=0;
	mp->squelchBeeps=0;
	mp->textIndenting=1;
	mp->ircIIDisplay=0;
	mp->timestamp=0;
	mp->timestampSeconds=0;
	mp->timestampWhenAway=0;
	mp->escClearsInputline=1;
	mp->cursorFocus=0;
	mp->cursorFocusDontActivate=0;
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
	mp->quitAction = qaConfirm;
	mp->dccWindowAutoOpen = 0;
mp->unused4=0;

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
	
	for(x=0;x<68;x++)
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

static pascal void CreateNewPrefsLinks(void)
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
			*(short*)&lp->linkName[12]='10';
			lp->linkName[0]=13;
		}
		pstrcpy("\pNewUser", lp->nick);
		pstrcpy("\pNew_User", lp->alterNick);
		pstrcpy("\pshadowirc", lp->user);
		pstrcpy("\pShadowIRC", lp->real);
		lp->autoConnect=false;
		lp->reconnect=false;
		lp->displayMOTD=true;
		lp->useConnectMacro=true;
		pstrcpy("\p/join #macintosh", lp->connectMacro);
		pstrcpy("\pnewbrunswick.nj.us.undernet.org", lp->serverName);
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
		
		lp->regainNick = lp->isTalkCity = 0;
		lp->unusedC[0] = 
		lp->unusedC[1] = 0;
		lp->unused[0] = 
		lp->unused[1] = 0;
		
		lp->kickMessage[0] = 0;
		
		for(y=0;y<763;y++)
			lp->spare[y]=0;
	}
}

static pascal void CreateNewPrefs(void)
{
	CreateNewPrefsMain();
	CreateNewPrefsLinks();
	InitColorPrefs();
}

static pascal void AllocateNewPrefs(void)
{
	mainPrefs=(prefsPtr)NewPtrClear(sizeof(prefsRec));
	linkPrefsArray=(linkPrefsPtr)NewPtrClear(sizeof(linkPrefsRec) * maxLinks);
	shadowircColors=(RGBColor*)NewPtr(sizeof(RGBColor)*(numSIColors)); //don't clear this because ALL of it is being set
}

pascal char readMainPrefs(void)
{
	short vref;
	OSErr err;
	char created;
	Boolean isDir;
	FSSpec f2;
	FSSpec ShadowIRCFolder;
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
			FSpCreateResFile(&mainPrefsLoc, creatorType, prefsType, 0);
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
		err = FSpCreate(&mainPrefsLoc, creatorType, prefsType, 0);
		err=FSpOpenDF(&mainPrefsLoc, fsRdWrPerm, &mainRefNum);
		mainResNum=FSpOpenResFile(&mainPrefsLoc, fsRdWrPerm);
		if(ResError())
		{
			FSpCreateResFile(&mainPrefsLoc, creatorType, prefsType, 0);
			mainResNum=FSpOpenResFile(&mainPrefsLoc, fsRdWrPerm);
			if(ResError())
				SysBeep(0);
		}

		CreateNewPrefs();
		writeMainPrefs();
	}
	
	ClosePrefs();
	
	return created;
}