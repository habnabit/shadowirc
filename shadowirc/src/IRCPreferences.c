/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2000 John Bafford
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

/*	ChangeLog
		2000-11-10 JB	Added enum PrefsWindowID
		2000-11-09 JB	Fixed one prefs crash
		2000-10-16	JB	Various code cleanup.
*/

#include <Carbon/Carbon.h>

#include "WASTE.h"
#include "StringList.h"
#include "AppearanceHelp.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "connections.h"
#include "plugins.h"
#include "utils.h"
#include "Inline.h"
#include "Floaters.h"
#include "FilesMan.h"
#include "IRCPreferences.h"
#include "IRCAux.h"
#include "ApplBase.h"
#include "TextManip.h"
#include "ServerList.h"
#include "MoreFiles.h"

enum PrefsWindowID {
	kwPrefConn = 1001,
	kwPrefMisc = 1002,
	kwShortcuts = 1003,
	kwPrefMain = 1004,
	kwPrefDCC = 1005,
	kwPrefCTCP = 1006,
	kwPrefText = 1007,
	kwPrefColor = 1008,
	kwPrefLogging = 1009,
	kwPrefFirewall = 1010,
	kwPrefMessages = 1011,
	kwPrefWindows = 1012,
	kwPrefStartup = 1013
};

static DialogPtr PrefsDlg;

static ListHandle connPrefsList = 0;
static short connPrefsRowNum = 0;

static pascal short dccsiztomenu(void);
static pascal long dccmenutosiz(short num);

static pascal char ColorFileSelect(FSSpec *out);
static pascal char ColorFilePut(FSSpec *f);
static pascal void SetColorsPanel(void);
static pascal void ExportColors(void);
static pascal void ImportColors(void);

static pascal void DoDirSel(FSSpec *fss, short item);
pascal unsigned char PrefsDialogFilter(DialogPtr d, EventRecord *e, short *item);
static pascal void RedrawPreferencesWindow(DialogPtr d);

static pascal void ClosePreferencesWindow(short windowNum, char finished);
static pascal void LoadPreferencesWindow(short windowNum);

static pascal void firewallItemSetup(short fw);
static pascal void connectionsItemSetup(char reconnect);

static pascal void SetPreferencesWindow(short windowNum, short connListNum);
static pascal char GetPreferencesWindow(short windowNum, StringPtr errorString, short connListNum);
static pascal char RetrievePreferencesWindow(short windowNum);
static pascal void HitPreferencesWindow(short windowNum, short item);
static pascal void PDConnectionsInit(void);
static pascal void PDConnectionsTrash(void);
static pascal void PDConnectionsListHit(short windowNum);

typedef struct prefsCopy {
	Handle mainPrefs;
	Handle linkPrefs;
	Handle shadowircColors;
	char ok;
} prefsCopy;

static prefsCopy pc;

static pascal char SetupCancel(void);
static pascal void EndCancel(char cancel);

typedef struct PreferencesMenuItemRec {
	long dlgID; //0 = none.
	plugsPtr pluginRef; //!=0 == internal
	char hasConnList;
	char x1, x2, x3;
	Str63 name;
} PreferencesMenuItemRec, *PreferencesMenuItemPtr;

typedef struct PreferencesMenuItemList {
	long num;
	PreferencesMenuItemRec list[1];
} PreferencesMenuItemList, *PreferencesMenuItemListPtr, **PreferencesMenuItemListHand;

static PreferencesMenuItemListHand pmlList;


pascal short PMLAdd(ConstStr63Param name)
{
	if(name[0])
	{
		long i;
		PreferencesMenuItemPtr pp;
		
		i = (**pmlList).num++;
		SetHandleSize((Handle)pmlList, sizeof(long)+((i+1)*sizeof(PreferencesMenuItemRec)));
		pp = &(**pmlList).list[i];
		
		pp->dlgID = 0;
		pp->pluginRef = sidr.yourInfo;
		pp->hasConnList = pp->x1 = pp->x2 = pp->x3 = 0;
		pstrcpy(name, pp->name);
		return i;
	}
	else
		return -1;
}

static pascal void _PMLAdd(ConstStr63Param name, short id)
{
	long i = PMLAdd(name);
	if(i>=0)
	{
		if(id < 0)
		{
			(**pmlList).list[i].hasConnList = 1;
			id = -id;
		}
		(**pmlList).list[i].dlgID=id;
	}
}

#pragma mark -

pascal void InitPrefs(void)
{
	Handle h = Get1Resource('STR#', 129);
	
	pmlList = (PreferencesMenuItemListHand)NewHandleClear(sizeof(long));
	
	if(h)
	{
		Ptr p;
		int num;
		int x;
		ConstStringPtr name, id;
		long l;
		
		HLockHi((Handle)h);
		p = *h;
		
		num = *(short*)p;
		
		for(x = 1; x<= num; x+=2)
		{
			name = GetIntStringPtr((STRnList*)p, x);
			id = GetIntStringPtr((STRnList*)p, x+1);
			StringToNum(id, &l);
			
			_PMLAdd(name, l);
		}
		
		ReleaseResource(h);
	}
	
	_PMLAdd("\p-", 0);
}

static pascal void PDConnectionsInit(void)
{
	Cell c;
	linkPtr lp;
	
	connPrefsList = GetAppearanceListBoxHandle(PrefsDlg, 13);
	
	(**connPrefsList).selFlags = lOnlyOne + lNoNilHilite;
	
	LAddRow(CountLinks(), 0, connPrefsList);
	
	c.h = c.v = 0;
	//c is already inited to (0,0), so only need to change v
	linkfor(lp, firstLink)
	{
		LSetCell(&lp->linkPrefs->linkName[1], lp->linkPrefs->linkName[0], c, connPrefsList);
		c.v++;
	}
	
	c.v = connPrefsRowNum;
	LSetSelect(true, c, connPrefsList);
	LSetDrawingMode(true, connPrefsList);
}

static pascal void PDConnectionsTrash(void)
{
	connPrefsList = 0;
}

static pascal void PDConnectionsListHit(short windowNum)
{
	Point c;
	
	if(RetrievePreferencesWindow(windowNum))
	{
		RGBColor old;
		
		SetPt(&c, 0, connPrefsRowNum);
		GetBackColor(&old);
		RGBBackColor(&white);
		LSetCell(&linkPrefsArray[connPrefsRowNum].linkName[1], linkPrefsArray[connPrefsRowNum].linkName[0], c, connPrefsList);
		RGBBackColor(&old);
		
		c.h=c.v=0;
		if(LGetSelect(true, &c, connPrefsList))
		{
			if(connPrefsRowNum != c.v)
			{
				connPrefsRowNum = c.v;
				SetPreferencesWindow(windowNum, c.v);
			}
		}
	}
	else
	{
		c.h = 0;
		c.v = connPrefsRowNum;
		selectOneCell(connPrefsList, c);
	}
}

#pragma mark -

static pascal short dccsiztomenu(void)
{
	switch(mainPrefs->dccSendPacketSize)
	{
		case 16384:
			return 1;
		case 8192:
			return 2;
		case 2048:
			return 4;
		case 1024:
			return 5;
		case 512:
			return 6;

		case 4096:
		default:
			return 3;
	}
}

static pascal long dccmenutosiz(short num)
{
	if(num < 1 || num > 6)
		return 4096;
	else
		return 1 << (15-num);
/*
	switch(num)
	{
		case 1:
			return 16384;
		case 2:
			return 8192;
		case 4:
			return 2048;
		case 5:
			return 1024;
		case 6:
			return 512;

		case 3:
		default:
			return 4096;
	}
*/
}

pascal void SetColorsPanel(void)
{
	int x;
	RGBColorHdl rgbc;
	short itemType;
	Rect box;
	ControlHandle item;

	for(x=0;x<numSIColors; x++)
	{
		GetDialogItem(PrefsDlg, x+4, &itemType, (Handle*)&item, &box);
		rgbc=(RGBColorHdl)GetControlDataHandle(item);
		**rgbc = shadowircColors[x];
		Draw1Control(item);
	}
}

static pascal char ColorFileSelect(FSSpec *out)
{
	char retVal;
	OSErr theErr;
	long count;
	NavReplyRecord theReply;
	NavDialogOptions dialogOptions;
	AEDesc 	resultDesc;
	FInfo	fileInfo;
	long index;
	NavTypeListHandle openList;
	AEKeyword key;

	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavGetFile;
	dialogOptions.dialogOptionFlags = kNOneFileNoTypePopup;
	GetIntString(dialogOptions.message, spFile, sSelectColorFileGetLoc);
	
	pstrcpy("\pShadowIRC", dialogOptions.clientName);
	openList = (NavTypeListHandle)GetResource('open', kOpenColor);

	theErr = NavGetFile(0, &theReply, &dialogOptions, StdNavFilter, 0, 0, openList, 0);
	
	if(openList)
		ReleaseResource((Handle)openList);
	
	retVal = 0;
	if(theReply.validRecord && !theErr)
	{
		// we are ready to open the document(s), grab information about each file for opening:
		count = 0;
		theErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++)
		{
			resultDesc.dataHandle = 0L;
			
			theErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, &key, &resultDesc);
			if(theErr == noErr)
			{
				AEGetDescData(&resultDesc, out, sizeof(FSSpec));
				
				theErr = FSpGetFInfo(out,&fileInfo);
				if(theErr == noErr)
					if (fileInfo.fdType == 'COLR')
						retVal = true;
				
				theErr = AEDisposeDesc(&resultDesc);
			}
		}
	}
	theErr = NavDisposeReply(&theReply);	// clean up after ourselves	

	return retVal;
}

static pascal char ColorFilePut(FSSpec *f)
{
	ConstStringPtr s = GetIntStringPtr(spFile, sSelectColorFileSaveLoc);
	return MyStandardPutFile(s, "\pColor Set", 'COLR', 'SIRC', kNDefault, f, true) <= 0;
}

static pascal void ExportColors(void)
{
	FSSpec f;
	short ref;
	long l;
	
	if(ColorFilePut(&f))
	{
		
		if(!FSpCreate(&f, 'SIRC', 'COLR', 0) && !FSpOpenDF(&f, fsRdWrPerm, &ref))
		{
			l=sizeof(RGBColor) * numSIColors;
			SetEOF(ref, l);
			SetFPos(ref, fsFromStart, 0);
			FSWrite(ref, &l, shadowircColors);
			FSClose(ref);
		}
		else
		{
		}
	}
}

static pascal void ImportColors(void)
{
	FSSpec f;
	short ref;
	long l;
	
	if(ColorFileSelect(&f))
	{
		if(!FSpOpenDF(&f, fsRdPerm, &ref))
		{
			l=sizeof(RGBColor) * numSIColors;
			FSRead(ref, &l, shadowircColors);
			FSClose(ref);
			
			SetColorsPanel();
		}
		else
		{
		}
	}
}

static pascal void firewallItemSetup(short fw)
{
	if(fw == fwNone)
	{
		setButtonEnable(PrefsDlg, 5, false);
		setButtonEnable(PrefsDlg, 6, false);
		setButtonEnable(PrefsDlg, 7, false);
		setButtonEnable(PrefsDlg, 8, false);
		setButtonEnable(PrefsDlg, 9, false);
		setButtonEnable(PrefsDlg, 10, false);
		setButtonEnable(PrefsDlg, 11, false);
		setButtonEnable(PrefsDlg, 12, false);
	}
	else if(fw == fwSOCKS5)
	{
		setButtonEnable(PrefsDlg, 5, true);
		setButtonEnable(PrefsDlg, 6, true);
		setButtonEnable(PrefsDlg, 7, true);
		setButtonEnable(PrefsDlg, 8, true);
		setButtonEnable(PrefsDlg, 9, true);
		setButtonEnable(PrefsDlg, 10, true);
		setButtonEnable(PrefsDlg, 11, true);
		setButtonEnable(PrefsDlg, 12, true);
	}
	else if(fw == fwSOCKS4A || fw == fwSOCKS4)
	{
		setButtonEnable(PrefsDlg, 5, true);
		setButtonEnable(PrefsDlg, 6, true);
		setButtonEnable(PrefsDlg, 7, true);
		setButtonEnable(PrefsDlg, 8, true);
		setButtonEnable(PrefsDlg, 9, true);
		setButtonEnable(PrefsDlg, 10, false);
		setButtonEnable(PrefsDlg, 11, true);
		setButtonEnable(PrefsDlg, 12, false);
	}
}

static pascal void connectionsItemSetup(char reconnect)
{
	setButtonEnable(PrefsDlg, 29, reconnect);
	setButtonEnable(PrefsDlg, 30, reconnect);
	setButtonEnable(PrefsDlg, 31, reconnect);
	setButtonEnable(PrefsDlg, 32, reconnect);
}

#pragma mark -

static pascal void SetPreferencesWindow(short windowNum, short connListNum)
{
	short x, y;
	ControlHandle item;
	Str255 s;
	prefsPtr mp = mainPrefs;
	MenuHandle mh;
	linkPrefsPtr lp;
	StringHandle sth;
	PreferencesMenuItemPtr pp= &(**pmlList).list[windowNum];
	
	if(pp->pluginRef)
	{
		pPWSetWindowData p;
		
		p.PD=PrefsDlg;
		p.id=windowNum;
		p.connListNum = connListNum;
		runIndPlugin((**pmlList).list[p.id].pluginRef, pPWSetWindowMessage, &p);
	}
	else
		switch(pp->dlgID)
		{
			case kwPrefConn:
				lp = &linkPrefsArray[connPrefsRowNum];
				SetText(PrefsDlg, 4, lp->linkName);
				SetText(PrefsDlg, 5, lp->serverName);
				NumToString(lp->serverPort, s);
				SetText(PrefsDlg, 6,  s);
				SetText(PrefsDlg, 7, lp->serverPass);
				SetText(PrefsDlg, 8, lp->nick);
				SetText(PrefsDlg, 9, lp->alterNick);
				SetText(PrefsDlg, 10, lp->user);
				SetText(PrefsDlg, 11, lp->real);
				setCheckBox(PrefsDlg, 15, lp->autoConnect);
				setCheckBox(PrefsDlg, 27, lp->modeI);
				setCheckBox(PrefsDlg, 18, lp->reconnect);
				setCheckBox(PrefsDlg, 20, lp->displayMOTD);
				setCheckBox(PrefsDlg, 21, lp->modeS);
				setCheckBox(PrefsDlg, 22, lp->modeW);
				NumToString(lp->maxConnAtt, s);
				SetText(PrefsDlg, 31, s);
				NumToString(lp->retryDelay, s);
				SetText(PrefsDlg, 32, s);
				setCheckBox(PrefsDlg, 33, lp->regainNick);
				
				setButtonEnable(PrefsDlg, 34, gTalkCityPlug);
				setCheckBox(PrefsDlg, 34, lp->isTalkCity);
				
				connectionsItemSetup(lp->reconnect);
				break;
			
			case kwPrefStartup:
				lp = &linkPrefsArray[connPrefsRowNum];
				setCheckBox(PrefsDlg, 4, lp->useConnectMacro);
				setButtonEnable(PrefsDlg, 5, lp->useConnectMacro);
				setButtonEnable(PrefsDlg, 6, lp->useConnectMacro);
				SetText(PrefsDlg, 5,  lp->connectMacro);
				break;
			
			case kwPrefMessages:
				lp = &linkPrefsArray[connPrefsRowNum];
				SetText(PrefsDlg, 4, lp->signoffMessage);
				SetText(PrefsDlg, 5, lp->fingerMessage);
				SetText(PrefsDlg, 6, lp->userinfoMessage);
				SetText(PrefsDlg, 11, lp->kickMessage);
				break;

			case kwPrefText:
				setCheckBox(PrefsDlg, 4, mp->showEndMessages);
				setCheckBox(PrefsDlg, 5, mp->showUserHostsWithMsgs);
				setCheckBox(PrefsDlg, 6, mp->timestamp);
				setCheckBox(PrefsDlg, 7, mp->timestampSeconds);
				setCheckBox(PrefsDlg, 8, mp->serverNoticesToConsole);
				setCheckBox(PrefsDlg, 9, mp->wallopsToConsole);
				setCheckBox(PrefsDlg, 10, mp->disableColor);
				setCheckBox(PrefsDlg, 11, mp->disableStyles);
				SetText(PrefsDlg, 13, mp->serverMessagePrefix);
				setCheckBox(PrefsDlg, 14, mp->timestampWhenAway);
				setButtonEnable(PrefsDlg, 7, mp->timestamp);
				setButtonEnable(PrefsDlg, 14, mp->timestamp);
				setCheckBox(PrefsDlg, 15, mp->privmsgsToConsole);
				setCheckBox(PrefsDlg, 19, mp->textIndenting);
				setCheckBox(PrefsDlg, 20, mp->displayPingsInConsole);
				setCheckBox(PrefsDlg, 22, mp->ircIIDisplay);
				setCheckBox(PrefsDlg, 24, mp->displayJoin);
				setCheckBox(PrefsDlg, 25, mp->displayPart);
				setCheckBox(PrefsDlg, 26, mp->displayQuit);
				setCheckBox(PrefsDlg, 27, mp->displayModes);
				setCheckBox(PrefsDlg, 28, mp->displayNicks);
				setCheckBox(PrefsDlg, 29, mp->displayKicks);
				setCheckBox(PrefsDlg, 30, mp->displayInvites);
				NumToString(mp->defaultFontSize, s);
				SetText(PrefsDlg, 18, s);
				item=GetControlHandle(PrefsDlg, 16);
				mh=GetControlMenu(item);
				y=CountMenuItems(mh);
				for(x=1;x<=y;x++)
				{
					GetMenuItemText(mh, x, s);
					if(pstrcmp(s, mp->defaultFontName))
					{
						SetControlValue(item, x);
						break;
					}
				}
				SetControlValue(GetControlHandle(PrefsDlg, 21), mainPrefs->colorMethod);
				SelectDialogItemText(PrefsDlg, 13, 0, 255);
				break;
			
			case kwPrefCTCP:
				setCheckBox(PrefsDlg, 4, mp->disableCTCP);
				setCheckBox(PrefsDlg, 5, mp->disableCTCPMessages);
				setCheckBox(PrefsDlg, 6, mp->noCTCPUnknownErrMsg);
				setCheckBox(PrefsDlg, 7, mp->ctcpToConsole);
				break;
			
			case kwPrefDCC:
				setCheckBox(PrefsDlg, 4, mp->autoAcceptDCCChat);
				setCheckBox(PrefsDlg, 5, mp->autoAcceptDCCChatWhenNotAway);
				setCheckBox(PrefsDlg, 6, mp->enableDCC);
				setCheckBox(PrefsDlg, 10, mp->autoSaveDCC);
				setCheckBox(PrefsDlg, 11, mp->autoDCCGet);
				setCheckBox(PrefsDlg, 12, mp->FastDCCSends);
				setCheckBox(PrefsDlg, 14, mp->dccReverseSends);
				setButtonEnable(PrefsDlg, 5, mp->autoAcceptDCCChat);
				x=255;
				FSpGetFullPath(&dccFolderFSp, &x, (char ***)&sth);
				if(sth)
				{
					BlockMoveData(*sth, &s[1], x);
					DisposeHandle((Handle)sth);
				}
				s[0]=x;
				SetText(PrefsDlg, 9, s);
				SetControlValue(GetControlHandle(PrefsDlg, 13), dccsiztomenu());
				
				if(!ExistsService(dccWindowServiceType))
					setButtonEnable(PrefsDlg, 15, false);
				
				setCheckBox(PrefsDlg, 15, mp->dccWindowAutoOpen);
				break;
			
			case kwPrefWindows:
				setCheckBox(PrefsDlg, 4,  mp->autoQuery);
				setButtonEnable(PrefsDlg, 5, mp->autoQuery);
				setButtonEnable(PrefsDlg, 6, mp->autoQuery);
				setButtonEnable(PrefsDlg, 7, mp->autoQuery);
				setCheckBox(PrefsDlg, 5+mp->autoQueryOpen, 1);
				setCheckBox(PrefsDlg, 8,  mp->cursorFocus);
				setButtonEnable(PrefsDlg, 9, mp->cursorFocus);
				setCheckBox(PrefsDlg, 9,  mp->cursorFocusDontActivate);
				setCheckBox(PrefsDlg, 10, mp->noModesWidget);
				setCheckBox(PrefsDlg, 11, mp->ddToSameWin);
				setCheckBox(PrefsDlg, 12, mp->nonGlobalInput);
				setCheckBox(PrefsDlg, 13, mp->dontActivateNewWindowsIfInputlineText);
				break;
			
			case kwPrefMisc:
				setCheckBox(PrefsDlg, 5+mp->quitAction, 1);
				setCheckBox(PrefsDlg, 8,  mp->beepOnPrivmsgs);
				setCheckBox(PrefsDlg, 9,  mp->autoRejoinAfterKick);
				setCheckBox(PrefsDlg, 10,  mp->autoRejoinReconnect);
				setCheckBox(PrefsDlg, 11,  mp->autoacceptInvites);
				setButtonEnable(PrefsDlg,12,  mp->autoacceptInvites);
				setCheckBox(PrefsDlg, 12,  mp->autoacceptInvitesWhenNotAway);
				setCheckBox(PrefsDlg, 13,  mp->escClearsInputline);
				setCheckBox(PrefsDlg, 14,  mp->optionToMoveInputLine);
				setCheckBox(PrefsDlg, 15,  mp->squelchBeeps);
				setCheckBox(PrefsDlg, 16,  mp->hideSplashScreen);
				setCheckBox(PrefsDlg, 17,  mp->doWhowasIfFailedWhois);
				setCheckBox(PrefsDlg, 18, mp->noFontsMenu);
				setCheckBox(PrefsDlg, 19, mp->resendAwayOnReconnect);
				SetControlValue(GetControlHandle(PrefsDlg, 20), linkPrefsArray[0].onoticeMethod);
				break;
			
			case kwPrefColor:
				SetColorsPanel();
				break;
			
			case kwPrefLogging:
				setCheckBox(PrefsDlg, 4,  mp->autoLogging);
				setCheckBox(PrefsDlg, 8,  mp->autoLogQueries);
				setCheckBox(PrefsDlg, 9,  mp->autoLogDCCChat);
				x=255;
				FSpGetFullPath(&logFolderFSp, &x, (char ***)&sth);
				if(sth)
				{
					BlockMoveData(*sth, &s[1], x);
					DisposeHandle((Handle)sth);
				}
				s[0]=x;
				SetText(PrefsDlg, 7, s);
				break;
			
			case kwPrefFirewall:
				SetControlValue(GetControlHandle(PrefsDlg, 4), mainPrefs->firewallType + 1);
				firewallItemSetup(mainPrefs->firewallType);
				SetText(PrefsDlg, 7, mp->socksHost);
				NumToString(mp->socksPort, s);
				SetText(PrefsDlg, 8, s);
				SetText(PrefsDlg, 11, mp->socksUser);
				SetText(PrefsDlg, 12, mp->socksPass);
				break;
		}
}

static pascal char RetrievePreferencesWindow(short windowNum)
{
	Str255 err;
	char out;
	short connListNum;
	pPWGetWindowData p;
	PreferencesMenuItemPtr pp= &(**pmlList).list[windowNum];

	if(connPrefsList)
		connListNum = connPrefsRowNum;
	else
		connListNum = -1;
	
	if(pp->pluginRef)
	{
		p.PD=PrefsDlg;
		p.id=windowNum;
		p.dontClose=false;
		p.connListNum = connListNum;
		runIndPlugin((**pmlList).list[p.id].pluginRef, pPWGetWindowMessage, &p);
		out = !p.dontClose;
	}
	else
		out = GetPreferencesWindow(windowNum, err, connListNum);
	
	if(!out)
	{
		ParamText(GetIntStringPtr(spError, sPrefsError), "\p", err, "\p");
		Alert(136, 0);
	}
	
	return out;
}

static pascal char GetPreferencesWindow(short windowNum, StringPtr errorString, short connListNum)
{
	Str255 s;
	long l;
	prefsPtr mp=mainPrefs;
	ControlHandle ch;
	linkPrefsPtr lp;
	char b;
	linkPtr ll;
	PreferencesMenuItemPtr pp= &(**pmlList).list[windowNum];
	
	switch(pp->dlgID)
	{
		case kwPrefConn:
			lp = &linkPrefsArray[connListNum];
			GetText(PrefsDlg, 4, s);
			if(s[0]>31)
				s[0]=31;
			
			if(s[0]==0)
			{
				SelectDialogItemText(PrefsDlg, 4, 0, 255);
				GetIntString(errorString, spError, sNoLinkName);
				return 0;
			}
			else if(pos(':', s))
			{
				SelectDialogItemText(PrefsDlg, 4, 0, 255);
				GetIntString(errorString, spError, sNoColonInLinkName);
				return 0;
			}
			
			pstrcpy(s, lp->linkName);
			
			GetText(PrefsDlg, 5, lp->serverName);
			GetText(PrefsDlg, 7, lp->serverPass);
			GetText(PrefsDlg, 6, s);
			StringToNum(s, &l);
			lp->serverPort=l;
			
			GetText(PrefsDlg, 8, s);
			if(s[0]>63)
				s[0]=63;
			pstrcpy(s, lp->nick);
			GetText(PrefsDlg, 9, s);
			if(s[0]>63)
				s[0]=63;
			pstrcpy(s, lp->alterNick);
			GetText(PrefsDlg, 10, lp->user);
			GetText(PrefsDlg, 11, lp->real);

			lp->autoConnect = getCheckBox(PrefsDlg, 15);
			lp->modeI = getCheckBox(PrefsDlg, 27);
			lp->reconnect = getCheckBox(PrefsDlg, 18);
			lp->displayMOTD = getCheckBox(PrefsDlg, 20);
			lp->modeW= getCheckBox(PrefsDlg, 22);
			lp->modeS= getCheckBox(PrefsDlg, 21);
			lp->regainNick= getCheckBox(PrefsDlg, 33);
			lp->isTalkCity= getCheckBox(PrefsDlg, 34);

			GetText(PrefsDlg, 31, s);
			StringToNum(s, &l);
			lp->maxConnAtt=l;
			GetText(PrefsDlg, 32, s);
			StringToNum(s, &l);
			lp->retryDelay=l;

			ConnectionMenuSetup();
			break;
		
		case kwPrefStartup:
			lp = &linkPrefsArray[connPrefsRowNum];
			lp->useConnectMacro = getCheckBox(PrefsDlg, 4);
			GetText(PrefsDlg, 5, lp->connectMacro);
			break;
		
		case kwPrefMessages:
			lp = &linkPrefsArray[connListNum];
			GetText(PrefsDlg, 4, lp->signoffMessage);
			GetText(PrefsDlg, 5, lp->fingerMessage);
			GetText(PrefsDlg, 6, lp->userinfoMessage);
			GetText(PrefsDlg, 11, lp->kickMessage);
			break;
			
		case kwPrefText:
			mp->showEndMessages=getCheckBox(PrefsDlg, 4);
			mp->showUserHostsWithMsgs=getCheckBox(PrefsDlg, 5);
			mp->timestamp=getCheckBox(PrefsDlg, 6);
			mp->timestampSeconds=getCheckBox(PrefsDlg, 7);
			mp->serverNoticesToConsole=getCheckBox(PrefsDlg, 8);
			mp->wallopsToConsole=getCheckBox(PrefsDlg, 9);
			mp->disableColor=getCheckBox(PrefsDlg, 10);
			mp->disableStyles=getCheckBox(PrefsDlg, 11);
			GetText(PrefsDlg, 13, mp->serverMessagePrefix);
			mp->timestampWhenAway=getCheckBox(PrefsDlg, 14);
			mp->privmsgsToConsole=getCheckBox(PrefsDlg, 15);
			mp->textIndenting=getCheckBox(PrefsDlg, 19);
			mp->displayPingsInConsole=getCheckBox(PrefsDlg, 20);
			GetText(PrefsDlg, 18, s);
			StringToNum(s, &l);
			mp->defaultFontSize=l;
			ch=GetControlHandle(PrefsDlg, 16);
			GetMenuItemText(GetControlMenu(ch), GetControlValue(ch), mp->defaultFontName);
			mainPrefs->colorMethod=GetControlValue(GetControlHandle(PrefsDlg, 21));
			mainPrefs->ircIIDisplay=getCheckBox(PrefsDlg, 22);
			mainPrefs->displayJoin=getCheckBox(PrefsDlg, 24);
			mainPrefs->displayPart=getCheckBox(PrefsDlg, 25);
			mainPrefs->displayQuit=getCheckBox(PrefsDlg, 26);
			mainPrefs->displayModes=getCheckBox(PrefsDlg, 27);
			mainPrefs->displayNicks=getCheckBox(PrefsDlg, 28);
			mainPrefs->displayKicks=getCheckBox(PrefsDlg, 29);
			mainPrefs->displayInvites=getCheckBox(PrefsDlg, 30);
			break;
		
		case kwPrefCTCP:
			mp->disableCTCP=getCheckBox(PrefsDlg, 4);
			mp->disableCTCPMessages=getCheckBox(PrefsDlg, 5);
			mp->noCTCPUnknownErrMsg=getCheckBox(PrefsDlg, 6);
			mp->ctcpToConsole=getCheckBox(PrefsDlg, 7);
			break;
		
		case kwPrefDCC:
			mp->autoAcceptDCCChat=getCheckBox(PrefsDlg, 4);
			mp->autoAcceptDCCChatWhenNotAway=getCheckBox(PrefsDlg, 5);
			mp->enableDCC=getCheckBox(PrefsDlg, 6);
			mp->autoSaveDCC=getCheckBox(PrefsDlg, 10);
			mp->autoDCCGet=getCheckBox(PrefsDlg, 11);
			mp->FastDCCSends=getCheckBox(PrefsDlg, 12);
			mp->dccSendPacketSize=dccmenutosiz(GetControlValue(GetControlHandle(PrefsDlg, 13)));
			mp->dccReverseSends=getCheckBox(PrefsDlg, 14);
			mp->dccWindowAutoOpen=getCheckBox(PrefsDlg, 15);
			break;
		
		case kwPrefWindows:
			mp->autoQuery=getCheckBox(PrefsDlg, 4);
			b = getCheckBox(PrefsDlg, 6);
			if(!b)
				b = 2 * getCheckBox(PrefsDlg, 7);
			
			mp->autoQueryOpen = b;
			
			mp->cursorFocus=getCheckBox(PrefsDlg, 8);
			mp->cursorFocusDontActivate=getCheckBox(PrefsDlg, 9);
			mp->noModesWidget = getCheckBox(PrefsDlg, 10);
			mp->ddToSameWin = getCheckBox(PrefsDlg, 11);
			mp->nonGlobalInput = getCheckBox(PrefsDlg, 12);
			mp->dontActivateNewWindowsIfInputlineText=getCheckBox(PrefsDlg, 13);
			break;
		
		case kwPrefMisc:
			b = getCheckBox(PrefsDlg, 6);
			if(!b)
				b = 2 * getCheckBox(PrefsDlg, 7);
			
			mp->quitAction = b;

			mp->beepOnPrivmsgs=getCheckBox(PrefsDlg, 8);
			mp->autoRejoinAfterKick=getCheckBox(PrefsDlg, 9);
			mp->autoRejoinReconnect=getCheckBox(PrefsDlg, 10);
			mp->autoacceptInvites=getCheckBox(PrefsDlg, 11);
			mp->autoacceptInvitesWhenNotAway=getCheckBox(PrefsDlg, 12);
			mp->escClearsInputline=getCheckBox(PrefsDlg, 13);
			mp->optionToMoveInputLine=getCheckBox(PrefsDlg, 14);
			mp->squelchBeeps=getCheckBox(PrefsDlg, 15);
			mp->hideSplashScreen=getCheckBox(PrefsDlg, 16);
			mp->doWhowasIfFailedWhois=getCheckBox(PrefsDlg, 17);
			mp->noFontsMenu=getCheckBox(PrefsDlg, 18);
			mp->resendAwayOnReconnect = getCheckBox(PrefsDlg, 19);
			b=GetControlValue(GetControlHandle(PrefsDlg, 20));
			linkfor(ll, firstLink)
				ll->linkPrefs->onoticeMethod = b;
			break;
		
		case kwPrefColor:
			break;
	
		case kwPrefLogging:
			mp->autoLogging=getCheckBox(PrefsDlg, 4);
			mp->autoLogQueries=getCheckBox(PrefsDlg, 8);
			mp->autoLogDCCChat=getCheckBox(PrefsDlg, 9);
			break;
		
		case kwPrefFirewall:
			mp->firewallType = GetControlValue(GetControlHandle(PrefsDlg, 4)) - 1;
			GetText(PrefsDlg, 7, s);
			if(s[0]>63)
				s[0]=63;
			pstrcpy(s, mp->socksHost);
			GetText(PrefsDlg, 8, s);
			StringToNum(s, &l);
			mp->socksPort = l;

			GetText(PrefsDlg, 11, s);
			if(s[0]>31)
				s[0]=31;
			pstrcpy(s, mp->socksUser);
			GetText(PrefsDlg, 12, s);
			if(s[0]>31)
				s[0]=31;
			pstrcpy(s, mp->socksPass);
			break;
	}
	return 1;
}

static pascal void DoDirSel(FSSpec *fss, short item)
{
	short len;
	Str255 st;
	StringHandle sth;
	OSErr err;
	
	DirectorySelectButton(fss);
	len = 255;
	err = FSpGetFullPath(fss, &len, (char ***)&sth);
	if(len)
	{
		BlockMoveData(*sth, &st[1], len);
		DisposeHandle((Handle)sth);
		st[0]=len;
		SetText(PrefsDlg, item, st);
	}
	else
		SetText(PrefsDlg, item, "\p");
	
	setButtonEnable(PrefsDlg, 1, true);
	setButtonEnable(PrefsDlg, 2, pc.ok);
}

static EventRecord prefsWindowEventRecord;

static pascal void HitPreferencesWindow(short windowNum, short item)
{
	char b;
	Point pt;
	short itemType;
	ControlHandle hnd;
	Rect box;
	RGBColor rgb;
	PreferencesMenuItemPtr pp = &(**pmlList).list[windowNum];

	if(pp->pluginRef)
	{
		pPWItemHitData p;
		
		if(pp->hasConnList && item == 13)
			PDConnectionsListHit(windowNum);
		else
		{
			p.PD=PrefsDlg;
			p.id=windowNum;
			p.itemNum=item;
			p.e = &prefsWindowEventRecord;
			GetMouse(&p.pt);
			runIndPlugin((**pmlList).list[p.id].pluginRef, pPWItemHitMessage, &p);
		}
	}
	else
		switch(pp->dlgID)
		{
			case kwPrefConn:
				switch(item)
				{
					case 13:
						PDConnectionsListHit(windowNum);
						break;
					
					case 18:
						b = !getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						connectionsItemSetup(b);
						break;
					
					case 14:
					case 15:
					case 20:
					case 21:
					case 22:
					case 27:
					case 33:
					case 34:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
					
					case 35:
					{
						linkPrefsPtr lp = &linkPrefsArray[connPrefsRowNum];
						ServerListServiceData p;
						long l;
						
						p.data = p.result = 0;
						
						p.network = lp->networkID;
						GetText(PrefsDlg, 6, p.serverName);
						StringToNum(p.serverName, &l);
						p.port = l;
						GetText(PrefsDlg, 5, p.serverName);
						
						DoServerList(&p);
						if(p.result)
						{
							SetText(PrefsDlg, 5, p.serverName);
							NumToString(p.port, p.serverName);
							SetText(PrefsDlg, 6, p.serverName);
							lp->networkID = p.network;
							
						}
						break;
					}
				}
				break;
			
			case kwPrefStartup:
				switch(item)
				{
					case 13:
						PDConnectionsListHit(windowNum);
						break;
					
					case 4:
						b = !getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						setButtonEnable(PrefsDlg, 5, b);
						setButtonEnable(PrefsDlg, 6, b);
						break;
				}
				break;
			
			case kwPrefMessages:
				switch(item)
				{
					case 13:
						PDConnectionsListHit(windowNum);
						break;
				}
				break;
					
			case kwPrefText:
				switch(item)
				{
					case 4:
					case 5:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 14:
					case 15:
					case 19:
					case 20:
					case 22:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 30:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
					case 6:
						b=!getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						setButtonEnable(PrefsDlg, 7, b);
						setButtonEnable(PrefsDlg, 14, b);
						break;
				}
				break;
			
			case kwPrefCTCP:
				switch(item)
				{
					case 4:
					case 5:
					case 6:
					case 7:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
				}
				break;
			
			case kwPrefDCC:
				switch(item)
				{
					case 4:
						b=!getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						setButtonEnable(PrefsDlg, 5, b);
						break;
					
					case 5:
					case 6:
					case 10:
					case 11:
					case 12:
					case 14:
					case 15:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
					
					case 8:
						DoDirSel(&dccFolderFSp, 9);
						break;
				}
				break;
			
			case kwPrefWindows:
				switch(item)
				{
					case 4:
						b=!getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						setButtonEnable(PrefsDlg, 5, b);
						setButtonEnable(PrefsDlg, 6, b);
						setButtonEnable(PrefsDlg, 7, b);
						break;
					
					case 5:
					case 6:
					case 7:
						if(item!=5)
							setCheckBox(PrefsDlg, 5, 0);
						if(item!=6)
							setCheckBox(PrefsDlg, 6, 0);
						if(item!=7)
							setCheckBox(PrefsDlg, 7, 0);
						setCheckBox(PrefsDlg, item, 1);
						break;
						
					case 8:
						b=!getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						setButtonEnable(PrefsDlg, 9, b);
						break;
					
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
				}
				break;
			
			case kwPrefMisc:
				switch(item)
				{
					case 5:
					case 6:
					case 7:
						if(item!=5)
							setCheckBox(PrefsDlg, 5, 0);
						if(item!=6)
							setCheckBox(PrefsDlg, 6, 0);
						if(item!=7)
							setCheckBox(PrefsDlg, 7, 0);
						setCheckBox(PrefsDlg, item, 1);
						break;

					case 8:
					case 9:
					case 10:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
					case 17:
					case 18:
					case 19:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
					
					case 11:
						b=!getCheckBox(PrefsDlg, item);
						setCheckBox(PrefsDlg, item, b);
						setButtonEnable(PrefsDlg, 12, b);
						break;
				}
				break;
			
			case kwPrefColor:
				if((item>=4) && (item<=3+numSIColors))
				{
					pt.h=100;
					pt.v=100;
					if(GetColor(pt, "\p", &shadowircColors[item-4], &rgb))
					{
						GetDialogItem(PrefsDlg, item, &itemType, (Handle*)&hnd, &box);
						shadowircColors[item-4]=rgb;
						**(RGBColorHdl)GetControlDataHandle(hnd) = rgb;
						Draw1Control(hnd);
					}
				}
				else if(item == 20) //export
					ExportColors();
				else if(item == 21) //import
					ImportColors();
				else if(item == 22) //defaults
				{
					InitColorPrefs();
					SetColorsPanel();
				}
				break;
			
			case kwPrefLogging:
				switch(item)
				{
					case 4:
					case 8:
					case 9:
						setCheckBox(PrefsDlg, item, !getCheckBox(PrefsDlg, item));
						break;
					
					case 6:
						DoDirSel(&logFolderFSp, 7);
						break;
				}
				break;
			
			case kwPrefFirewall:
				switch(item)
				{
					case 4:
						firewallItemSetup(GetControlValue(GetControlHandle(PrefsDlg, 4)) - 1);
						break;
				}
				break;
		}
}

static pascal void ClosePreferencesWindow(short windowNum, char finished)
{
	Rect r;
	RgnHandle oldRgn;
	PreferencesMenuItemPtr pp= &(**pmlList).list[windowNum];
	
	if(pp->hasConnList)
		PDConnectionsTrash();	

	if(!finished)
	{
		//Clear out the rect
		SetRect(&r, 4, 28, 522, 346);
		EraseRect(&r);
		
		oldRgn = NewRgn();
		GetClip(oldRgn);
		ZeroRect(r);
		ClipRect(&r);
		ShortenDITL(PrefsDlg, CountDITL(PrefsDlg) - 3);
		SetClip(oldRgn);
		DisposeRgn(oldRgn);
	}
}

static short currentPrefsWindowNum;
static pascal void LoadPreferencesWindow(short windowNum)
{
	short oldRes;
	int ok;
	PreferencesMenuItemPtr pp;
	
	oldRes = CurResFile();
	
	pp = &(**pmlList).list[windowNum];
	
	if(!pp->pluginRef) //internal
	{
		ok = 1;
		UseResFile(gApplResFork);
	}
	else
	{
		plugsPtr pn;
		pPWActivateData p;
		
		p.id = windowNum;
		pn = pp->pluginRef;
		p.ditlNum = 0;
		p.hasConnectionsList = false;
		runIndPlugin(pn, pPWActivateMessage, &p);
		
		ok = p.ditlNum != 0;
		if(ok)
		{
			pp->dlgID = p.ditlNum;
			pp->hasConnList = p.hasConnectionsList;
			
			UseResFile(pn->resFileRefNum);
		}
	}
	
	if(ok)
	{
		AppendDialogItemList(PrefsDlg,  pp->dlgID, 0);
		
		if(pp->hasConnList)
			PDConnectionsInit();

		UseResFile(oldRes);
		
		currentPrefsWindowNum = windowNum;
		SetPreferencesWindow(windowNum, 0);
	}
}

static pascal void RedrawPreferencesWindow(DialogPtr d)
{
	PreferencesMenuItemPtr pp = &(**pmlList).list[currentPrefsWindowNum];
	
	if(pp->pluginRef)
	{
		pPWRedrawData p;
		p.PrefsDlg = d;
		p.id = currentPrefsWindowNum;
		runIndPlugin((**pmlList).list[currentPrefsWindowNum].pluginRef, pPWRedrawMessage, &p);
	}
}

pascal unsigned char PrefsDialogFilter(DialogPtr d, EventRecord *e, short *item)
{
	char ok;
	
	prefsWindowEventRecord = *e;
	ok = StandardDialogFilter(d, e, item);
	
	if(!ok)
	{
		if(e->what == updateEvt) //our window
			RedrawPreferencesWindow(d);
		
		return false; //we aren't completely processing this!
	}
	else
		return true;
}

static pascal char SetupCancel(void)
{
	long s;
	OSErr err;
	
	pc.mainPrefs = pc.linkPrefs = pc.shadowircColors = 0;
	pc.ok = false;
	
	s = GetPtrSize((Ptr)mainPrefs);
	pc.mainPrefs = TempNewHandle(s, &err);
	if(!err)
	{
		BlockMoveData(mainPrefs, *pc.mainPrefs, s);
		s = GetPtrSize((Ptr)linkPrefsArray);
		pc.linkPrefs = TempNewHandle(s, &err);
		if(!err)
		{
			BlockMoveData(linkPrefsArray, *pc.linkPrefs, s);
			s = GetPtrSize((Ptr)shadowircColors);
			pc.shadowircColors = TempNewHandle(s, &err);
			if(!err)
			{
				BlockMoveData(shadowircColors, *pc.shadowircColors, s);
				pc.ok = true;
				return 1;
			}
		}
	}
	
	if(pc.mainPrefs)
		DisposeHandle(pc.mainPrefs);
	if(pc.linkPrefs)
		DisposeHandle(pc.linkPrefs);
	if(pc.shadowircColors)
		DisposeHandle(pc.shadowircColors);
	
	pc.mainPrefs = pc.linkPrefs = pc.shadowircColors = 0;
	return 0;
}

static pascal void EndCancel(char cancel)
{
	long s;
	
	if(cancel && pc.ok)
	{
		s = GetPtrSize((Ptr)mainPrefs);
		BlockMoveData(*pc.mainPrefs,mainPrefs, s);
		s = GetPtrSize((Ptr)linkPrefsArray);
		BlockMoveData(*pc.linkPrefs, linkPrefsArray, s);
		s = GetPtrSize((Ptr)shadowircColors);
		BlockMoveData(*pc.shadowircColors, shadowircColors, s);
	}
	
	if(pc.mainPrefs)
		DisposeHandle(pc.mainPrefs);
	if(pc.linkPrefs)
		DisposeHandle(pc.linkPrefs);
	if(pc.shadowircColors)
		DisposeHandle(pc.shadowircColors);
	
	pc.mainPrefs = pc.linkPrefs = pc.shadowircColors = 0;
	pc.ok = false;
}

pascal void OpenPreferencesWindow(short panelID)
{
	static short loadPane = 0;
	short i, i2, x;
	short menuSelection, oldMenuSelection;
	ControlHandle ch;
	MenuHandle mh;
	GrafPtr gp;
	ModalFilterUPP df = NewModalFilterUPP(PrefsDialogFilter);
	short rf = CurResFile();
	char cancelPrefs = 0;
	pPWClosedData pd;
	MWPtr xmw;
	TextStyle ts;
				
	UseResFile(gApplResFork);
	GetPort(&gp);
	PrefsDlg=GetNewDialog(kwPrefMain, 0, (WindowPtr)-1);
	SetPortDialogPort(PrefsDlg);
	
	//Create the menu and add the items to the menu
	mh = NewMenu(10000, "\pPrefs Items");
	i2=(**pmlList).num;
	
	if(i2>0)
		for(x=0; x<i2; x++)
			AppendMenu(mh, (**pmlList).list[x].name);
	
	//Get the control and set the maximum items for the menu
	ch = GetControlHandle(PrefsDlg, 3);
	SetControlData(ch, 0, kControlGroupBoxMenuHandleTag, sizeof(MenuHandle), (Ptr)&mh);
	SetControlMaximum(ch, i2);
	
	SetDlogFont(PrefsDlg);
	if(panelID==-1)
		oldMenuSelection=loadPane;
	else
		oldMenuSelection=panelID;
	
	if(oldMenuSelection < 0 || oldMenuSelection > (**pmlList).num)
		oldMenuSelection = 0;
	
	SetControlValue(ch, oldMenuSelection + 1);
	LoadPreferencesWindow(oldMenuSelection);
	
	if(!SetupCancel())
		setButtonEnable(PrefsDlg, 2, false);
	runPlugins(pPWOpenedMessage, &pd);

	SetupModalDialog(PrefsDlg, 1, 2);
	do
	{
		ModalDialog(df, &i);
		
		switch(i)
		{
			case 1:
				if(!RetrievePreferencesWindow(oldMenuSelection))
					i=0;
				break;
			
			case 2:
				cancelPrefs = 1;
				//i=0;
				break;
			
			case 3:
				menuSelection=GetControlValue(ch) - 1;
				if(menuSelection!=oldMenuSelection)
				{
					if(RetrievePreferencesWindow(oldMenuSelection))
					{
						ClosePreferencesWindow(oldMenuSelection, false);
						LoadPreferencesWindow(menuSelection);
						oldMenuSelection=menuSelection;
					}
					else
						SetControlValue(ch, oldMenuSelection + 1);
				}
				break;
			
			default:
				HitPreferencesWindow(oldMenuSelection, i);
		}
		DrawDialog(PrefsDlg); // Seems to be necessary in OS X
	} while(i!=1 && i!= 2);
	
	loadPane = GetControlValue(ch) - 1;
	ClosePreferencesWindow(oldMenuSelection,true);
	DisposeDialog(PrefsDlg);
	DisposeMenu(mh);
	
	DisposeModalFilterUPP(df);
	SetPort(gp);
	FinishModalDialog();
	UseResFile(rf);
	
	EndCancel(cancelPrefs);
	
	ts.tsColor = shadowircColors[sicStandard];
	
	linkfor(xmw, mwl)
		if(!xmw->protect)
		{
			SetWindowContentColor(xmw->w, &shadowircColors[sicWindowBG]);
			if(xmw->il)
				WESetStyle(weDoColor, &ts, xmw->il);
		}
	
	pd.canceled = cancelPrefs;
	runPlugins(pPWClosedMessage, &pd);
}

#pragma mark -

pascal void InitColorPrefs(void)
{
	RGBColor *sic=shadowircColors;
	
	SetRGBColor(sic[0], 28180, 15728, 0);
	SetRGBColor(sic[1], 0, 0, 0);
	SetRGBColor(sic[2], -1, 0, 0);
	SetRGBColor(sic[sicCTCP], 0, 42281, 12684);
	SetRGBColor(sic[4], 0, 0, -1);
	SetRGBColor(sic[5], 28180, 15728, 0);
	SetRGBColor(sic[6], -1, -1, -1);
	SetRGBColor(sic[7], -1, 0, 0);
	SetRGBColor(sic[8], 0, 51773, 17040);
	SetRGBColor(sic[9], -1, 32768, 0);
	SetRGBColor(sic[10], 0, 0, 0);
	SetRGBColor(sic[11], 0, 0, 0);
	SetRGBColor(sic[12], 18350, 26214, 1966);
	
	//notify
	SetRGBColor(sic[13], 25368, 0, 25368);
	
	//timestamp color
	SetRGBColor(sic[sicTimestampColor], 0, 0, 0);
}
