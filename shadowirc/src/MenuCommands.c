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

#include "WASTE.h"

#include "inline.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "connections.h" //for plugins
#include "plugins.h"
#include "utils.h"
#include "Floaters.h"
#include "MsgWindows.h"
#include "IRCChannels.h"
#include "ApplBase.h"
#include "filesMan.h"
#include "IRCPreferences.h"
#include "Inputline.h"
#include "IRCInput.h"
#include "windowList.h"
#include "IRCAux.h"
#include "IRCCommands.h"
#include "InetConfig.h"
#include "Shortcuts.h"
#include "CMenus.h"
#include "MenuCommands.h"

MenuHandle gAppleMenu, gFileMenu, gEditMenu, gShortcutsMenu, gWindowMenu;
static MenuHandle gFontsMenu;

static void DoFind2(MWPtr mw);

void FontsMenuInit(void);

static long FindText(Handle t, long start, Str255 searchFor, char caseSen, char reverse)
{
	long x;
	char b;
	long max;
	char* p;
	long mag;
	int (*compareFunc)(const char *s1, const char *s2, size_t count);
	SInt8 hstate;
	
	max = GetHandleSize(t);
	hstate = HGetState(t);
	HLock(t);
	p = *t;
	
	if(reverse)
		mag = -1;
	else
		mag = 1;
	
	if(reverse && start + searchFor[0] > max)
		x = max - searchFor[0] - 1;
	else
		x = start;
	
	if(caseSen)
		compareFunc = strncmp;
	else
		compareFunc = strncasecmp;
	
	for(; x>=0 && x < max; x += mag)
	{
		b = compareFunc(&p[x], (char*)&searchFor[1], searchFor[0]);
		
		if(!b)
		{
			HSetState(t, hstate);
			return x;
		}
	}
	HSetState(t, hstate);

	return -1;
}

static struct FindInformation {
	Str255 searchFor;
	char caseSen;
	char reverse;
} find = {"\p", 0, 1};

static void DoFind2(MWPtr mw)
{
	Handle t;
	long s0, s1;
	long found;
	
	if(!mw)
		mw = GetActiveMW();
	
	if(find.searchFor[0] && mw)
	{
		t=WEGetText(mw->we);
		WEGetSelection(&s0, &s1, mw->we);
		if(s0==s1)
		{
			if(find.reverse)
				s0 = GetHandleSize(t) + 1;
			else
				s0=-1;
		}
		if(t)
		{
			if(find.reverse)
				s0--;
			else
				s0++;
			found=FindText(t, s0, find.searchFor, find.caseSen, find.reverse);
			if(found>=0)
			{
				WESetSelection(found, found+find.searchFor[0], mw->we);
				WESelView(mw->we);
			}
			else
				SysBeep(0);
		}
	}
}

#define kNibFind CFSTR("find")
#define KNibWinFind CFSTR("Find")

static int gFindWindowCount = 0;
static WindowPtr gFindWin = NULL;

enum {
	kFindWinText = 4,
	kFindWinCaseSensitive = 5,
	kFindWinReverse = 6
};

static void FindWindowGet(WindowPtr findWin)
{
	CFStringRef theString;
	const ControlID textID = { kApplicationSignature, kFindWinText };
	const ControlID senID = { kApplicationSignature, kFindWinCaseSensitive };
	const ControlID revID = { kApplicationSignature, kFindWinReverse };
	ControlRef ctrl = NULL;
	
	GetControlByID(findWin, &textID, &ctrl);
	GetControlData(ctrl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString, NULL);
	CFStringGetPascalString(theString, find.searchFor, sizeof(Str255), CFStringGetSystemEncoding());

	GetControlByID(findWin, &senID, &ctrl);
	find.caseSen = GetControlValue(ctrl);

	GetControlByID(findWin, &revID, &ctrl);
	find.reverse = GetControlValue(ctrl);
}

static void FindWindowSet(WindowPtr findWin)
{
	CFStringRef theString;
	const ControlID textID = { kApplicationSignature, kFindWinText };
	const ControlID senID = { kApplicationSignature, kFindWinCaseSensitive };
	const ControlID revID = { kApplicationSignature, kFindWinReverse };
	ControlRef ctrl = NULL;
	
	GetControlByID(findWin, &textID, &ctrl);
	theString = CFStringCreateWithPascalString(NULL, find.searchFor, kCFStringEncodingMacRoman);
	SetControlData(ctrl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString);
	CFRelease(theString);
	
	GetControlByID(findWin, &senID, &ctrl);
	SetControlValue(ctrl, find.caseSen);

	GetControlByID(findWin, &revID, &ctrl);
	SetControlValue(ctrl, find.reverse);
}

static OSStatus FindWindowEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	WindowPtr findWin = (WindowPtr)userData;
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassControl:
		{
			ControlRef theControl = NULL;
			UInt32 cmd;
			
			GetEventParameter(event, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &theControl);
			GetControlCommandID(theControl, &cmd);
			
			switch(cmd)
			{
				case kHICommandOK:
					FindWindowGet(findWin);
					
				case kHICommandCancel:
					DisposeWindow(findWin);
					gFindWindowCount--;
					
					if(cmd == kHICommandOK)
						DoFind2(0);
					return noErr;
			}
		}
		
		case kEventClassCommand:
		{
			HICommand hiCommand;
			
			GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(hiCommand), NULL, &hiCommand);
			
			switch(hiCommand.commandID)
			{
				case kHICommandClose:
					return noErr;
			}
			break;
		}
	}
	
	return result;
}

static void ActivateFindWindow(WindowPtr findWin)
{
	ControlRef itemCtrl;
	const ControlID textItem = {kApplicationSignature, 4 };
	
	ShowWindow(findWin);
	SelectWindow(findWin);
	
	GetControlByID(findWin, &textItem, &itemCtrl);
	SetKeyboardFocus(findWin, itemCtrl, kControlFocusNextPart);
}

void DoFind(char again)
{
	static EventHandlerUPP fwUPP = NULL;
	WindowRef findWin;
	IBNibRef findNib;
	OSStatus status;
	const EventTypeSpec ctSpec[] = {
		{ kEventClassControl, kEventControlHit },
		{ kEventClassCommand, kEventProcessCommand }
	};
	
	if(again && find.searchFor[0])
		DoFind2(0);
	else //throw a dialog
	{
		if(++gFindWindowCount > 1)
		{
			//Undo the change to the count and select the shortcuts window.
			gFindWindowCount--;
			if(gFindWin)
				ActivateFindWindow(gFindWin);
			return;
		}
		
		if(!fwUPP)
			fwUPP = NewEventHandlerUPP(FindWindowEventHandler);
		
		status = CreateNibReference(kNibFind, &findNib);
		require_noerr(status, CantFindDialogNib);
		
		status = CreateWindowFromNib(findNib, KNibWinFind, &findWin);
		require_noerr(status, CantCreateDialogWindow);
		
		DisposeNibReference(findNib);
		
		status = InstallWindowEventHandler(findWin, fwUPP, GetEventTypeCount(ctSpec), ctSpec,(void *)findWin, NULL);
		require_noerr(status, CantInstallDialogHandler);
		
		FindWindowSet(findWin);
		
		ActivateFindWindow(findWin);
		
		gFindWin = findWin;
	}

CantFindDialogNib:
CantCreateDialogWindow:
CantInstallDialogHandler:
	return;
}

#pragma mark -

char ToggleConsoleWindow()
{
		if(ActiveNonFloatingWindow() == consoleWin->w && IsWindowVisible(consoleWin->w))
		{
			HideWindow(consoleWin->w);
			return mainPrefs->consoleOpen = 0;
		}
		else
		{
			WSelect(consoleWin->w);
			return mainPrefs->consoleOpen = 1;
		}
}

#pragma mark -

void HitAppleURLMenu(short item)
{
	Str255 url;
	
	if(internetConfig)
	{
		//get url
		GetIntString(url, spAppleURL, item * 2);
		
		if(OpenURL(url))
		{
			//error
		}
	}
	else
	{
	}
}

			
pascal void HitEditMenu(short item)
{
	char mwFront, otherFront;
	WEReference we;
	long s0, s1;
	MWPtr activeMW;
	
	activeMW = GetActiveMW();
	if(activeMW)
	{
		mwFront = 1;
		otherFront = 0;
	}
	else
	{
		mwFront = 0;
		otherFront = !iwFront;
	}

	we=0;
	if(iwFront)
		we=ILGetWE();
	else if(mwFront)
		we=activeMW->we;
	else if(otherFront) //this was a dangerous assumption.
	{
		return; //don't know what to do.
	}
	
	if(we)
	{
		switch(item)
		{
			case 1:
				if(we)
				{
					if(!noFloatingInput)
						WEUndo(ILGetWE());
				}
				break;
				
			case 3: //cut
				if(iwFront)
					WECut(ILGetWE());
				break;
			
			case 4: //copy
				WECopy(we);
				break;
			
			case 5: //paste
				if(mwFront || iwFront)
				{
					WEReference il = ILGetWE();
					
					if(il)
					{
						WEPaste(il);
						processPaste(activeMW, false);
					}
				}
				break;
			
			case 6: //clear
				WEGetSelection(&s0, &s1, we);
				if(s0==s1)
				{
					WEDeactivate(we);
					WESetSelection(0, 0x7FFFFF, we);
				}
				WEDelete(we);
				WEActivate(we);
				break;
			
			case 7: //select all
				if(!iwFront)
				{
					WESetSelection(0, 0x7FFFFFF, we);
				}
				break;
		}
	}
}

int HitWindowPluginServiceMenu(const short item)
{
	if(wMenuServices && item <= wLastServiceItem)
	{
		pServiceWindowMenuData dp;
		
		wMSDataPtr service = &(**wMenuServices).service[item - wFirstServiceItem];
		dp.key = service->key;
		dp.menuItem = item;
		runIndPlugin(service->pluginRef, pServiceWindowMenu, &dp);
		
		return 1;
	}
	
	return 0;
}

int HitWindowSelectWindowMenu(const short item)
{
	if(item<windowsStart2)
	{
		//NOW Menus menu selection workaround
		//What should this be normally?
		if(item-windowsStart <= wmItems.channelsLength)
			WSelect((**wmItems.channels).mw[item-windowsStart-1]->w);
	}
	else
	{
		if(item-windowsStart2 <= wmItems.restLength)
			WSelect((**wmItems.rest).mw[item-windowsStart2-1]->w);
	}
	
	return 1;
}

#define kNibChannel CFSTR("channel")
#define kNibWinFontSize CFSTR("Font Size")

typedef struct sqData {
	WindowRef win;
	long size;
} sqData;

static OSStatus FontSizeEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	WindowRef aboutWindow;
	sqData *data = (sqData*)userData;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	aboutWindow = (WindowRef)userData;
	
	switch(eventClass)
	{
		case kEventClassControl:
		{
			ControlRef theControl = NULL;
			UInt32 cmd;
			ControlRef itemCtrl;
			const ControlID item = {'SIRC', 4};
			CFStringRef theStr;
			Str255 s;
			
			GetEventParameter(event, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &theControl);
			GetControlCommandID(theControl, &cmd);
			
			switch(cmd)
			{
				case kHICommandOK:
					GetControlByID(data->win, &item, &itemCtrl);
					
					GetControlData(itemCtrl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theStr, NULL);
					CFStringGetPascalString(theStr, s, sizeof(Str255), CFStringGetSystemEncoding());
					StringToNum(s, &data->size);
				
				case kHICommandCancel:
					QuitAppModalLoopForWindow(data->win);
					result = noErr;
					break;
			}
			break;
		}
	}
	
	return result;
}

void DoFontSizeWindow(void)
{
	MWPtr mw = GetActiveMW();
	static EventHandlerUPP swUPP = NULL;
	WindowRef fsWin;
	IBNibRef channelsNib;
	OSStatus status;
	sqData sq;
	ControlRef itemCtrl;
	const ControlID item = {'SIRC', 4};
	CFStringRef theStr;
	Str255 s;
	
	const EventTypeSpec ctSpec[] = {
		{ kEventClassControl, kEventControlHit }
	};
	
	//We don't do anything if there's no active mw
	if(!mw)
		return;
	
	if(!swUPP)
		swUPP = NewEventHandlerUPP(FontSizeEventHandler);
	
	status = CreateNibReference(kNibChannel, &channelsNib);
	require_noerr(status, CantFindDialogNib);
	
	status = CreateWindowFromNib(channelsNib, kNibWinFontSize, &fsWin);
	require_noerr(status, CantCreateDialogWindow);
	
	DisposeNibReference(channelsNib);
	
	status = InstallWindowEventHandler(fsWin, swUPP, GetEventTypeCount(ctSpec), ctSpec, (void *)&sq, NULL);
	require_noerr(status, CantInstallDialogHandler);
	
	//Put the current font size here.
	s[0] = 0;
	
	sq.win = fsWin;
	sq.size = 0;
	
	GetControlByID(fsWin, &item, &itemCtrl);
	theStr = CFStringCreateWithPascalString(NULL, s, kCFStringEncodingMacRoman);
	SetControlData(itemCtrl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theStr);
	CFRelease(theStr);
	
	ShowWindow(fsWin);
	SelectWindow(fsWin);
	
	SetKeyboardFocus(fsWin, itemCtrl, kControlFocusNextPart);
	
	status = RunAppModalLoopForWindow(fsWin);
	
	DisposeWindow(fsWin);
	
	if(sq.size >= 4 && sq.size < 32767)
	{
		MWSetFontSize(mw, -1, sq.size);
		WEActivate(mw->we);
	}
CantFindDialogNib:
CantCreateDialogWindow:
CantInstallDialogHandler:
	;
}

void HitFontsMenu(short item)
{
	Str255 s;
	long l;
	MWPtr mw = GetActiveMW();
	
	//We don't do anything if there's no active mw
	if(!mw)
		return;
	
	GetMenuItemText(gFontsMenu, item, s);
	if(item>fontsBegin)
	{
		FMFontFamily fontFamily = FMGetFontFamilyFromName(s);
		
		MWSetFontSize(mw, fontFamily, -1);
	}
	else
	{
		StringToNum(s, &l);
		MWSetFontSize(mw, -1, l);
	}
	
	WEActivate(mw->we);
}

void HitSelectConnectionMenu(short item)
{
	if(item > 0)
	{
		CurrentTarget.link = GetLinkNum(item - 1);
		UpdateStatusLine();
	}
}


#pragma mark -

pascal void MenuConnectionList(short item)
{
	linkPtr link;
	
	if(item)
	{
		link = firstLink;
		while(--item)
			link = link->next;
		
		if(OpenConnection(link))
		{
			InvalTarget(&CurrentTarget);
			CurrentTarget.bad=0;
			CurrentTarget.link=link;
		}
	}	
}

pascal void MenuSignoffConnectionList(short item)
{
	if(item)
	{
		linkPtr link = firstLink;
		
		while(--item)
			link = link->next;
		
		DoSignoff(link, 0);
		
		if(CurrentTarget.link == link)
			InvalTarget(&CurrentTarget);
		UpdateStatusLine();
	}
}

#pragma mark -

static OSStatus DoShowFontsMenu(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
	static int previousFontCheck = 0;
	static int previousSizeCheck = 0;
	Str255 s, s2;
	MenuHandle m;
	int x, y;
	long l;
	MWPtr mw = GetActiveMW();
	static MWPtr lastmw = NULL;
	Boolean firstOpen;
	
	GetEventParameter(event, kEventParamMenuFirstOpen, typeBoolean, NULL, sizeof(Boolean), NULL, &firstOpen);
	
	if(!firstOpen && lastmw == mw) //bail out if we're redisplaying the menu
		return noErr;
		
	lastmw = mw;
	
	//Update fonts menu, but only if we have one..
	if(mw)
	{
		m = gFontsMenu;
		
		if(previousSizeCheck)
			CheckMenuItem(m, previousSizeCheck, false);
		
		for(x=1;x<fontsBegin;x++)
		{
			GetMenuItemText(m, x, s);
			StringToNum(s, &l);
			if(l == mw->size)
			{
				CheckMenuItem(m, x, true);
				previousSizeCheck = x;
				break;
			}
		}
		
		if(previousFontCheck)
			CheckMenuItem(m, previousFontCheck, false);
		
		GetFontName(mw->font, s);
		y=CountMenuItems(m);
		for(x=fontsBegin+1;x<=y;x++)
		{
			GetMenuItemText(m, x, s2);
			if(pstrcmp(s, s2))
			{
				previousFontCheck = x;
				CheckMenuItem(m, x, true);
				break;
			}
		}
	}
	
	return noErr;
}

#pragma mark -


void AppleMenuURLInit(void)
{
	MenuHandle m;
	int x, y;
	int num;
	
	m = NewMenu(AppleURLMenu, "\p");
	InsertMenu(m, hierMenu);
	// Tell it where to put the hierarchical URL menu
	SetMenuItemHierarchicalID(gAppleMenu, 2, AppleURLMenu);
	
	num = *(short*)spAppleURL;
	if(num>0)
	{
		y = 0;
		for(x=1;x<=num;x+=2)
		{
			AppendMenu(m, "\p-");
			y++;
			SetMenuItemText(m, y, GetIntStringPtr(spAppleURL, x));
			SetMenuItemCommandID(m, y, 'AURL');
		}
	}
}

void FontsMenuInit(void)
{
	if(mainPrefs->noFontsMenu)
	{
		DeleteMenu(fontsMenu);
		gFontsMenu = 0;
	}
	else
	{
		int x, max;
		static EventHandlerUPP fmHUPP = NULL;
		const EventTypeSpec fmEvents[] = {
			{kEventClassMenu, kEventMenuOpening},
		};
		
		MenuHandle mh = GetMenuHandle(fontsMenu);
		gFontsMenu = mh;
		fontsBegin = CountMenuItems(mh);
		CreateStandardFontMenu(mh, fontsBegin, 0, kNilOptions, 0);
		
		max = CountMenuItems(mh);
		for(x = 1; x < fontsBegin - 1; x++)
			SetMenuItemCommandID(mh, x, 'FONT');
		for(x = fontsBegin + 1; x < max; x++)
			SetMenuItemCommandID(mh, x, 'FONT');
		
		if(!fmHUPP)
			fmHUPP = NewEventHandlerUPP(DoShowFontsMenu);
		
		InstallMenuEventHandler(mh, fmHUPP, GetEventTypeCount(fmEvents), fmEvents, mh, NULL);
	}
}

void ConnectionMenuInit(void)
{
	short x, y;
	linkPtr link;
	
	if(menuConnectionList)
		y = CountMenuItems(menuConnectionList);
	else
	{
		menuConnectionList = NewMenu(ConnectionListMenu, "\p");
		menuSignoffConnectionList = NewMenu(SignoffConnectionListMenu, "\p");
		menuSelectConnectionList = NewMenu(SelectConnectionListMenu, "\p");
		InsertMenu(menuConnectionList, hierMenu);
		InsertMenu(menuSignoffConnectionList, hierMenu);

		SetMenuItemHierarchicalID(gFileMenu, 1, ConnectionListMenu);
		SetMenuItemHierarchicalID(gFileMenu, 6, SignoffConnectionListMenu);
		
		y = 0;
	}

	for(x=1, link=firstLink; link; link=link->next,x++)
	{
		if(x > y)
		{
			AppendMenu(menuConnectionList, "\p-");
			AppendMenu(menuSignoffConnectionList, "\p-");
			AppendMenu(menuSelectConnectionList, "\p-");
			SetMenuItemCommandID(menuConnectionList, x, 'CONO');
			SetMenuItemCommandID(menuSignoffConnectionList, x, 'CONC');
			SetMenuItemCommandID(menuSelectConnectionList, x, 'CONX');
		}
		SetMenuItemText(menuConnectionList, x, link->linkPrefs->linkName);
		SetMenuItemText(menuSignoffConnectionList, x, link->linkPrefs->linkName);
		SetMenuItemText(menuSelectConnectionList, x, link->linkPrefs->linkName);
	}
	
	while(x < y)
	{
		DeleteMenuItem(menuConnectionList, y);
		DeleteMenuItem(menuSignoffConnectionList, y);
		DeleteMenuItem(menuSelectConnectionList, y--);
	}
	
	ConnectionMenuHilites();
}

void ConnectionMenuHilites(void)
{
	int x;
	linkPtr link;
	
	for(x=1, link=firstLink; link; link=link->next,x++)
	{
		if(link->conn == 0)
		{
			EnableMenuItem(menuConnectionList, x);
			DisableMenuItem(menuSelectConnectionList, x);
			DisableMenuItem(menuSignoffConnectionList, x);
		}
		else
		{
			DisableMenuItem(menuConnectionList, x);
			EnableMenuItem(menuSelectConnectionList, x);
			EnableMenuItem(menuSignoffConnectionList, x);
		}
	}
}

pascal void MenuInit(void)
{
	IBNibRef mainNibRef;

	if((CreateNibReference(CFSTR("main"), &mainNibRef) == noErr) && (SetMenuBarFromNib(mainNibRef, CFSTR("MenuBar")) == noErr))
	{
		DisposeNibReference(mainNibRef);
	}
	else
	{
		fprintf(stderr, "Could not create menu bar.\n");
		fflush(stderr);
		ExitToShell();
	}

	gAppleMenu = GetMenuHandle(appleMenu);
	gEditMenu = GetMenuHandle(EditMenu);
	gFileMenu = GetMenuHandle(FileMenu);
	FontsMenuInit();
	gShortcutsMenu = GetMenuHandle(shortcutsMenu);
	ShortcutsMenuInit(gShortcutsMenu);
	gWindowMenu = GetMenuHandle(windowMenu);
	
	if(hasAquaMenuMgr)
	{
		MenuRef menu;
		MenuItemIndex index;
		
		// Enable the Application menu Preferences... item
		EnableMenuCommand(NULL, kHICommandPreferences);

		GetIndMenuItemWithCommandID(NULL, kHICommandPreferences, 1, &menu, &index);
		SetMenuItemCommandKey(menu, index, false, ',');
	}
}
