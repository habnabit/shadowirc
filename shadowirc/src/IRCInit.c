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
#include "MoreFilesX.h"
#include "plugins.h"


inline void AppleMenuURLInit(void);
inline void CheckPreferences(void);
static void ToolboxInit(void);
void ApplicationInit(void);
inline void SetupUPPs(void);

void ApplInit();
void setVers(void);

STRnPtr spFiles, spCM, spError, spInfo, spDCC,  spTopic, spAppleURL, spServices, spSOCKS, spServer, spFile, spWhois;

enum {
    kSIRCVersionInfoID = 3
};

#define GetStrN(sr) *(STRnHand)Get1Resource('STR#', sr)

inline void CheckPreferences(void)
{
	LongString ls;
	
	if(!FSRefValid(&logFolderRef) && mainPrefs->autoLogging)
	{
		LSGetIntString(&ls, spError, sLogsFolderInvalid); //logs folder invalid
		SMPrefix(&ls, dsConsole);
	}
}

inline void SetupUPPs(void)
{
	StdDlgFilter = NewModalFilterUPP(StandardDialogFilter);
}

static void AppleHelpInit()
{
	/* Register help book */
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	if(mainBundle)
	{
		CFURLRef bundleURL = NULL;
		CFRetain(mainBundle);
		bundleURL = CFBundleCopyBundleURL(mainBundle);

		if(bundleURL)
		{
			FSRef bundleFSRef;
			if(CFURLGetFSRef(bundleURL, &bundleFSRef))
				AHRegisterHelpBook(&bundleFSRef);
					
			CFRelease(bundleURL);
		}
			
		CFRelease(mainBundle);
	}
}

void ApplicationInit(void)
{
	WindowRef splashWindow = NULL;
	LongString ls;
	ControlRef theControl;
	ControlButtonContentInfo content;
	
	ToolboxInit();
	
	spFiles = GetStrN(srFiles);
	spCM=GetStrN(srCM);
	spError=GetStrN(srError);
	spInfo=GetStrN(srInfo);
	spDCC=GetStrN(srDCC);
	spTopic=GetStrN(srTopic);
	spAppleURL=GetStrN(srAppleURL);
	spServices=GetStrN(srServices);
	spSOCKS=GetStrN(srSOCKS);
	spServer=GetStrN(srServer);
	spFile=GetStrN(srFile);
	spWhois=GetStrN(srWhois);
	
	GetDateTime(&now);
	
	setVers();
	
	InstallAEHandlers();
	
	readMainPrefs();
	initLinks();
	SetupUPPs();
	
	if(!mainPrefs->hideSplashScreen)
	{
		IBNibRef mainNibRef;

		if((CreateNibReference(CFSTR("main"), &mainNibRef) == noErr) && (CreateWindowFromNib(mainNibRef, CFSTR("Splash"), &splashWindow) == noErr))
		{
			CFBundleRef appBundle;
			CFStringRef text;
			ControlID versionInfoID = { kApplicationSignature, kSIRCVersionInfoID };
			ControlFontStyleRec controlStyle;
			Rect boundsRect;
			
			DisposeNibReference(mainNibRef);
			
			appBundle = CFBundleGetMainBundle();
			text = (CFStringRef) CFBundleGetValueForInfoDictionaryKey(appBundle, CFSTR("CFBundleGetInfoString"));
			GetControlByID(splashWindow, &versionInfoID, &theControl);
			SetControlData(theControl, kControlLabelPart, kControlStaticTextCFStringTag, sizeof(CFStringRef), &text);
			controlStyle.flags = kControlUseJustMask | kControlUseSizeMask;
			controlStyle.just = teCenter;
			controlStyle.size = 12;
			SetControlFontStyle(theControl, &controlStyle);
			
			content.contentType = kControlContentPictHandle;
			content.u.picture = LoadAppLogoFromResources();
			
			SetPortWindowPort(splashWindow);
			
			SetRect(&boundsRect, 0, 0, 360, 144);
			CreatePictureControl(splashWindow, &boundsRect, &content, TRUE, &theControl);
			
 			ShowWindow(splashWindow);
			SelectWindow(splashWindow);
		}
	}
	
	MenuInit();
	ApplInit();
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
	
	ConnStartNetworking();

	launchTime = idleTime = lastKey = lastInput = now;
	ISOEncode=*GetResource('Tabl', 256);
	ISODecode=*GetResource('Tabl', 257);
	
	ShortcutsMenuUpdate();
	MBInit();
	ConnectionMenuInit();
	
	AppleMenuURLInit();
	AppleHelpInit();
	
	if(splashWindow)
	{
		if(content.u.picture)
			DisposeHandle((Handle)content.u.picture);
		
		if(theControl)
			DisposeControl(theControl);
		
		DisposeWindow(splashWindow);
	}

	makePlugsDB();
	UseResFile(gApplResFork);
	
	CurrentTarget.link=firstLink;
}

static void Gestalts(void)
{
	long response;
	OSErr err;

	if(!Gestalt(gestaltSystemVersion, &response)) //this selector returns a short
	{
		// Check for Aqua Menu Manager
		err = Gestalt(gestaltMenuMgrAttr, &response);
		hasAquaMenuMgr = (!err && (response & gestaltMenuMgrAquaLayoutMask));
	}
}

static void ToolboxInit(void)
{
	gApplResFork = CurResFile();
	
	Gestalts();
	
	RegisterAppearanceClient();
	
	StartupIC();

	InitDrag();
}
