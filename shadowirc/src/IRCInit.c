/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
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

#include <Balloons.h>
#include <Appearance.h>
#include <Navigation.h>

#include "LongStrings.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "DCC.h"
#include "connections.h"
#include "IRCAux.h"
#include "filesMan.h"
#include "InputLine.h"
#include "MsgBuffer.h"
#include "utils.h"
#include "WindowList.h"
#include "IRCIgnore.h"
#include "Inline.h"
#include "TCP.h"
#include "IRCInput.h"
#include "IRCPreferences.h"
#include "Shortcuts.h"
#include "InetConfig.h"
#include "AEvents.h"
#include "DragDrop.h"
#include "AppearanceHelp.h"
#include "ApplBase.h"
#include "Floaters.h"
#include "TextManip.h"
#include "CalcCRC.h"
#include "IRCPreferences.h"
#include "MenuCommands.h"
#include "IRCChannels.h"

#pragma internal on
inline void AppleMenuURLInit(void);
inline void CheckPreferences(void);
pascal void ToolboxInit(void);
pascal void ApplicationInit(void);
inline void SetupUPPs(void);

pascal void ApplInit();
pascal void setVers(void);
inline void SetupHelpMenu(void);

STRnPtr spFiles, spCM, spError, spInfo, spDCC, spHelp, spTopic, spAppleURL, spServices, spSOCKS, spServer, spFile, spWhois;

inline void CheckPreferences(void)
{
	LongString ls;
	
	if(!ValidFSSpec(&logFolderFSp) && mainPrefs->autoLogging)
	{
		LSGetIntString(&ls, spError, sLogsFolderInvalid); //logs folder invalid
		SMPrefix(&ls, dsConsole);
	}
}

inline void AppleMenuURLInit(void)
{
	MenuHandle m;
	int x, y;
	int num;
	
	m = NewMenu(AppleURLMenu, "\p");
	InsertMenu(m, -1);
	
	num = *(short*)spAppleURL;
	if(num>0)
	{
		y = 0;
		for(x=1;x<=num;x+=2)
		{
			AppendMenu(m, "\p-");
			y++;
			SetMenuItemText(m, y, GetIntStringPtr(spAppleURL, x));
		}
	}
}

static pascal void AboutDlgVersion(DialogPtr d, short i);
static pascal void AboutDlgVersion(DialogPtr d, short i)
{
	Str255 s;
	Rect r;
	short type;
	Handle item;
		
	if(i == 3) //version
	{
		GetDialogItem(d, i, &type, &item, &r);
		MoveTo(r.left, r.bottom-5);
		GetIndString(s, 128, 1);
		DrawString(s);
		DrawString(CL_VERSION);
	}
	else if(i==4) //registered to
	{
	}
	else if(i==5) //serial number
	{
	}
}

inline void SetupUPPs(void)
{
	StdDlgFilter = NewModalFilterProc(StandardDialogFilter);
	AboutDlgVersionFilter = NewUserItemProc(AboutDlgVersion);
	if(hasNav)
		StdNavFilter = NewNavEventProc(NavDialogFilter);
}

#if !TARGET_CARBON
inline void SetupHelpMenu(void)
{
	OSErr err;
	short x, num;
	ConstStringPtr s;
	
	menuHelpWidget = NewMenu(mHelpWidget, "\p");
	err=HMGetHelpMenuHandle(&helpMenu);
	if(!err)
	{
		if(helpMenu)
		{
			defaultHelpItems = CountMenuItems(helpMenu);
			num = *(short*)spHelp;
			if(num>0)
				for(x=1;x<=num;x++)
				{
					s = GetIntStringPtr(spHelp, x);
					AppendMenu(helpMenu, s);
					AppendMenu(menuHelpWidget, s);
				}
		}
	}
	
	normHelpMenuItems = CountMenuItems(helpMenu)+2;
	InsertMenu(menuHelpWidget, -1);
}
#endif

#define GetStrN(sr) *(STRnHand)Get1Resource('STR#', sr)

pascal void ApplicationInit(void)
{
	DialogPtr splashDlg;
	LongString ls;
	
	spFiles = GetStrN(srFiles);
	spCM=GetStrN(srCM);
	spError=GetStrN(srError);
	spInfo=GetStrN(srInfo);
	spDCC=GetStrN(srDCC);
	spHelp=GetStrN(srHelp);
	spTopic=GetStrN(srTopic);
	spAppleURL=GetStrN(srAppleURL);
	spServices=GetStrN(srServices);
	spSOCKS=GetStrN(srSOCKS);
	spServer=GetStrN(srServer);
	spFile=GetStrN(srFile);
	spWhois=GetStrN(srWhois);
	
	setVers();
	
	InstallAEHandlers();
	
	
	InitCalcCRC();
	
	readMainPrefs();
	initLinks();
	SetupUPPs();
	
	if(!mainPrefs->hideSplashScreen)
	{
		Rect itemRect;
		Handle itemHandle;
		short itemType;
		
		splashDlg=GetNewDialog(1000, 0, (WindowPtr)-1);
		SetDlogFont(splashDlg);

		GetDialogItem(splashDlg, 2, &itemType, &itemHandle, &itemRect);
		SetDialogItem(splashDlg, 2, itemType, (Handle)AboutDlgVersionFilter, &itemRect);

		DrawDialog(splashDlg);
		AboutDlgVersion(splashDlg,2);
	}
	else
		splashDlg=0;
	
	ApplInit();
#if !TARGET_CARBON
	SetupHelpMenu();
#endif
	GetIcons();
	wmItemsInit();
	InitPrefs();
	OpenInputLine();
	InitMsgWindows();
	
	LSConcatStrAndStr("\pShadowIRC ", CL_VERSION, &ls);
	LineMsg(&ls);
	LSStrLS("\phttp://www.shadowirc.com/", &ls);
	LineMsg(&ls);

	CheckPreferences();
	DCCTypeInit();
	
	InitTCP();

	launchTime = idleTime = lastKey = lastInput = now;
	ISOEncode=*GetResource('Tabl', 256);
	ISODecode=*GetResource('Tabl', 257);
	
	ShortcutsMenuUpdate();
	MBInit();
	ConnectionMenuSetup();
	AppleMenuURLInit();
	
	if(splashDlg)
		DisposeDialog(splashDlg);
}

static pascal void Gestalts(void)
{
	long tempResult;

	if(!Gestalt(gestaltSystemVersion, &tempResult)) //this selector returns a short
	{
		if(tempResult < 0x0710)
		{
			ParamText(GetIntStringPtr(spError, sOnlyOneShadowIRC), "\p", "\p", "\p");
			Alert(130, 0);
			ExitToShell();
		}
		
		has75=tempResult >= 0x0750;
		
		#if __POWERPC__
		has85=tempResult >= 0x0850;
		has86=tempResult >= 0x0860;

		if(!Gestalt(gestaltWindowMgrAttr, &tempResult))
		{
			if(tempResult & gestaltWindowMgrPresent) //has WM 1.1
			{
				hasWM11 = true;
				//if(tempResult & gestaltHasFloatingWindows)
				if(tempResult & 2)
					hasFloatingWindows = true;
hasFloatingWindows = 0;
			}
		}
		#endif
	}
	
	if(!Gestalt(gestaltAppearanceAttr, &tempResult))
		hasAppearance = tempResult & 1;
	#if __POWERPC__
		if(hasAppearance)
		{
			hasAppearance = ((long)CreateRootControl != kUnresolvedCFragSymbolAddress);
			if(hasAppearance && !Gestalt(gestaltAppearanceVersion, &tempResult))
			{
				if(tempResult >= 0x0110) //appearance 1.1
					hasAppearance11 = true;
			}
		}
		else
		{
			ParamText(GetIntStringPtr(spError, 48), "\p", "\p", "\p");
			Alert(130, 0);
			ExitToShell();
		}
	#endif
	
	//Assume AppleEvents are present since we require 7.1

	if(!Gestalt(gestaltContextualMenuAttr, &tempResult))
		hasCM= (tempResult & (1 << gestaltContextualMenuTrapAvailable)) && !InitContextualMenus();

	if(!Gestalt(gestaltSpeechAttr, &tempResult))
		hasSpeech = (tempResult & (1 << gestaltSpeechMgrPresent)) != 0;
}

pascal void ToolboxInit(void)
{
	long tempResult;
	
#if !TARGET_CARBON
	InitGraf(&qd.thePort);
	InitFonts();
	if(hasFloatingWindows)
		InitFloatingWindows();
	else
		InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0);
	MaxApplZone();
#endif
	
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	
	gApplResFork = CurResFile();
	
	GetCurrentProcess(&gPSN);

	Gestalts();
	
	if(hasAppearance)
		RegisterAppearanceClient();
	
	if(!Gestalt(gestaltComponentMgr, &tempResult))
		if(tempResult)
			StartupIC();

	InitDrag();
	
	#if __POWERPC__
	hasNav = NavServicesAvailable() && !NavLoad();
	#endif
}