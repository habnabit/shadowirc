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
#include "TextWindows.h"
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
#include "CommandsMenu.h"
#include "CMenus.h"
#include "MenuCommands.h"

MenuHandle gAppleMenu, gFileMenu, gEditMenu, gShortcutsMenu, gWindowMenu;
static MenuHandle gFontsMenu;
static short gFontSizeOtherItem;

static pascal void DoFind2(void);
static pascal void DoFind(char again);

static pascal void HitFontsMenu(short item);

void FontsMenuInit(void);

static pascal long FindText(Handle t, long start, Str255 searchFor, char caseSen, char reverse)
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

static pascal void DoFind2(void)
{
	Handle t;
	long s0, s1;
	long found;
	MWPtr mw = GetActiveMW();
	
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

static pascal void DoFind(char again)
{
	DialogPtr d;
	short i;
	
	if(again && find.searchFor[0])
		DoFind2();
	else //throw a dialog
	{
		d=GetNewDialog(135, 0, (WindowPtr)-1);
		setCheckBox(d, 5, find.caseSen);
		setCheckBox(d, 6, find.reverse);
		SetText(d, 4, find.searchFor);
		SelectDialogItemText(d, 4, 0, 255);

		SetupModalDialog(d, 1, 2);
		do {
			ModalDialog(StdDlgFilter, &i);
			
			if(i==5 || i==6)
				setCheckBox(d, i, !getCheckBox(d, i));
		} while((i!=1) && (i!=2));
		
		if(i==1)
		{
			find.caseSen=getCheckBox(d, 5);
			find.reverse=getCheckBox(d, 6);
			GetText(d, 4, find.searchFor);
		}

		DisposeDialog(d);
		FinishModalDialog();
		
		if(i==1)
			DoFind2();
	}
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
	TEHandle te;
	WEReference we;
	long s0, s1;
	WindowPtr w = ActiveNonFloatingWindow();
	MWPtr activeMW;
	
	if(w && IsDialog(w))
	{
		DialogPtr d = GetDialogFromWindow(w);
		
		if(item == 3)
			DialogCut(d);
		else if(item == 4)
			DialogCopy(d);
		else if(item==5)
			DialogPaste(d);
		else if(item==6) //clear
			DialogDelete(d);
		else if(item==7) //select all
		{
			te = GetDialogTextEditHandle(d);
			if(te)
				SelectDialogItemText(d, GetDialogKeyboardFocusItem(d), 0, 32766);
		}
		return;
	}
	
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

	te=0;
	we=0;
	if(iwFront)
		we=ILGetWE();
	else if(mwFront)
		we=activeMW->we;
	else if(otherFront) //this was a dangerous assumption.
	{
		if(ContextWindow) //this isn't one of my windows. Is it a plugin's?
		{
			s0 = GetWRefCon(ContextWindow);
			if(s0 && ((pluginDlgInfoPtr)s0)->magic == PLUGIN_MAGIC)
			{
				plugsPtr plug = GetPluginWindowOwner(ContextWindow);
				pMenuItemSelectedData p;
				
				p.contextWindow = ContextWindow;
				p.menuID = 258;
				p.itemID = item;
				
				runIndPlugin(plug, pMenuItemSelectedMessage, &p);
			}
			
			return; //don't know what to do.
		}
	}
	
	if(te || we)
	{
		switch(item)
		{
			case 1:
				if(we)
				{
					if(activeMW && activeMW->winType == textWin)
						WEUndo(activeMW->we);
					else if(!noFloatingInput)
						WEUndo(ILGetWE());
				}
				break;
				
			case 3: //cut
				if(iwFront)
					WECut(ILGetWE());
				else if(mwFront && activeMW->winType==textWin)
					WECut(we);
				else if(otherFront)
					TECut(te);
				break;
			
			case 4: //copy
				if(otherFront)
					TECopy(te);
				else
					WECopy(we);
				break;
			
			case 5: //paste
				if(mwFront || iwFront)
				{
					if(!iwFront && activeMW->winType == textWin)
						WEPaste(activeMW->we);
					else
					{
						WEReference il = ILGetWE();
						
						if(il)
						{
							WEPaste(il);
							processPaste(activeMW, false);
						}
					}
				}
				else if(otherFront)
					TEPaste(te);
				break;
			
			case 6: //clear
				if(otherFront)
					TEDelete(te);
				else
				{
					WEGetSelection(&s0, &s1, we);
					if(s0==s1)
					{
						WEDeactivate(we);
						WESetSelection(0, 0x7FFFFF, we);
					}
					WEDelete(we);
					WEActivate(we);
				}
				break;
			
			case 7: //select all
				if(te)
					TESetSelect(0, 32767, te);
				else if(we)
					WESetSelection(0, 0x7FFFFFF, we);
				break;
			
			case 9:	
				DoFind(false);
				break;
			case 10:
				DoFind(true);
				break;
		}
	}
}

pascal void HitWindowMenu(const short item)
{
	//check for service items
	if(wMenuServices && item <= wLastServiceItem)
	{
		pServiceWindowMenuData dp;
		
		wMSDataPtr service = &(**wMenuServices).service[item - wFirstServiceItem];
		dp.key = service->key;
		dp.menuItem = item;
		runIndPlugin(service->pluginRef, pServiceWindowMenu, &dp);
	}
	else
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
	}	
}

static pascal void HitFontsMenu(short item)
{
	Str255 s;
	short i;
	long l;
	DialogPtr d;
	char b;
	MWPtr mw = GetActiveMW();
	
	//We don't do anything if there's no active mw
	if(!mw)
		return;
	
	if(item == gFontSizeOtherItem)
	{
		s[0] = 0;
		EnterModalDialog();
		d = GetNewDialog(141, 0, (WindowPtr)-1);
		
		SetText(d, 4,"\p");
		SetupModalDialog(d, 1, 2);
		b = 0;
		do {
			ModalDialog(StdDlgFilter, &i);
			
			switch(i)
			{
				case 1:
					GetText(d, 4, s);
					
				case 2:
					b = i;
					break;
			}
		} while(!b);
		
		DisposeDialog(d);
		FinishModalDialog();
		
		if(s[0])
		{
			StringToNum(s, &l);
			if(l >= 4 && l < 32767)
			{
				MWSetFontSize(mw, -1, l);
				WEActivate(mw->we);
			}
		}
	}
	else
	{
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
		
		if(link->serverStatus==S_CONN)
			DoSignoff(link, 0);
		
		if(CurrentTarget.link == link)
			InvalTarget(&CurrentTarget);
		UpdateStatusLine();
	}
}

#pragma mark -

pascal void DoMenuEvent(long menuitem, const EventRecord *e)
{
	int menuNum, itemNum;
	
	menuNum = menuitem >> 16;
	itemNum = menuitem & 0x0000FFFF;
	
	if(menuNum)
	{
		switch(menuNum)
		{
			case EditMenu:
				HitEditMenu(itemNum);
				break;
			case windowMenu:
				HitWindowMenu(itemNum);
				break;
			case commandsMenu:
				HitCommandsMenu(itemNum, e->modifiers);
				break;
			case fontsMenu:
				HitFontsMenu(itemNum);
				break;
		}
	}
	HiliteMenu(0);
}

pascal void MenuBarClick(const EventRecord *e)
{
	static int previousFontCheck = 0;
	static int previousSizeCheck = 0;
	Str255 s, s2;
	MenuHandle m;
	int x, y;
	long l;
	WindowPtr w;
	MWPtr mw = GetActiveMW();
	
	//Update file menu
	SetMenuItemText(gFileMenu, 6, GetIntStringPtr(spCM, smSavePreferences + (mw && mw->winType == textWin)));
	
	//Update edit menu
	if(mw)
	{
		EnableMenuItem(gEditMenu, 9);
		EnableMenuItem(gEditMenu, 10);
	}
	else
	{
		DisableMenuItem(gEditMenu, 9);
		DisableMenuItem(gEditMenu, 10);
	}
	
	//Update fonts menu, but only if we have one..
	if(mw && gFontsMenu)
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
	
	//Update window menu
	if((w = ActiveNonFloatingWindow())!=0 && (w=GetNextWindow(w))!=0 && IsWindowVisible(w))
	{
		EnableMenuCommand(gWindowMenu, kCommandPrevWin);
		EnableMenuCommand(gWindowMenu, kCommandNextWin);
	}
	else
	{
		DisableMenuCommand(gWindowMenu, kCommandPrevWin);
		DisableMenuCommand(gWindowMenu, kCommandNextWin);
	}
	CheckMenuItem(gWindowMenu, wConsoleItem, IsWindowVisible(consoleWin->w));
	DoMenuEvent(MenuSelect(e->where), e);
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
		MenuHandle mh = GetMenuHandle(fontsMenu);
		gFontsMenu = mh;
		fontsBegin=CountMenuItems(mh);
		gFontSizeOtherItem = fontsBegin - 1;
		CreateStandardFontMenu(mh, fontsBegin, 0, kNilOptions, 0);
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
		SetMenuItemHierarchicalID(gFileMenu, 8, SignoffConnectionListMenu);
		
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
			EnableMenuItem(menuSelectConnectionList, x);
			DisableMenuItem(menuSignoffConnectionList, x);
		}
		else
		{
			DisableMenuItem(menuConnectionList, x);
			DisableMenuItem(menuSelectConnectionList, x);
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
		SetMenuItemCommandKey(menu, index, false, '=');
	}
}
