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

MenuHandle gAppleMenu, gEditMenu, gShortcutsMenu, gWindowMenu;
static MenuHandle gFileMenu, gFontsMenu;
static short applMItems;
static short gFontSizeOtherItem;

UserItemUPP AboutDlgVersionFilter;

static pascal void DoFind2(void);
static pascal void DoFind(char again);

static pascal void cascade2(WindowPtr w);
static pascal void DoCascadeWindows(void);
static pascal void DoTileWindows(void);
static pascal void DoCycleCommand(char next);

static pascal void DoAbout(void);
static pascal void HitFontsMenu(short item);
static pascal void MenuAppleURL(short item);
static pascal void HitFileMenu(int item);
static pascal void CloseFrontWindow(void);

inline void FontsMenuInit(void);

#pragma internal on

#pragma dont_inline off

static pascal char _strncmp(const char *s1, const char *s2, unsigned long len)
{
	int x;
	
	for(x=0; x < len; x++, s1++, s2++)
		if(*s1 != *s2)
			return 0;
	
	return 1;
}

static pascal char _strncmpcase(const char *s1, const char *s2, unsigned long len)
{
	int x;
	
	for(x=0; x < len; x++, s1++, s2++)
		if(inupc(*s1) != inupc(*s2))
			return 0;
	
	return 1;
}

static pascal long FindText(Handle t, long start, Str255 searchFor, char caseSen, char reverse)
{
	long x;
	char b;
	long max;
	char* p;
	long mag;
	
	max = GetHandleSize(t);
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
	
	for(; x>=0 && x < max; x += mag)
	{
		if(caseSen)
			b = _strncmp(&p[x], (char*)&searchFor[1], searchFor[0]);
		else
			b = _strncmpcase(&p[x], (char*)&searchFor[1], searchFor[0]);
		
		if(b)
			return x;
	}
	HUnlock(t);

	return -1;
}

#pragma dont_inline reset

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
	
	if(find.searchFor[0] && MWActive)
	{
		t=WEGetText(MWActive->we);
		WEGetSelection(&s0, &s1, MWActive->we);
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
				WESetSelection(found, found+find.searchFor[0], MWActive->we);
				WESelView(MWActive->we);
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


static pascal void CloseFrontWindow(void)
{
	WindowClose(FrontNonFloater());
}

#pragma mark -

static pascal void DoCycleCommand(char next)
{
	MWPtr activeW, mw;
	
	if(!FrontNonFloater()) //do nothing if no front window.
		return;
	
	activeW=MWActive;
	if(!activeW) //no active mw. find one
	{
		return;
	}
	
	if(next)
	{
		if(!activeW->prev)
			mw=consoleWin;
		else
			mw=activeW->prev;
		
		//window to activate is in mw, and w is correct
		while(!IsVisible(mw->w))
		{
			mw=mw->prev;
			
			if(mw==activeW)
				break;
			else if(!mw)
				mw=consoleWin;
		}
		MWActive=mw;
		WSelect(mw->w);
	}
	else //exact same as above, except prev instead of next
	{
		if(!activeW->next)
			mw = mwl;
		else
			mw=activeW->next;
		
		//window to activate is in mw, and w is correct
		while(!IsVisible(mw->w))
		{
			mw=mw->next;
			
			if(mw==activeW)
				break;
			else if(!mw)
				mw = mwl;
		}
		MWActive=mw;
		WSelect(mw->w);
	}
}

pascal void WindowClose(WindowPtr wp)
{
	long i;
	
	if(wp)
	{
		i=GetWRefCon(wp);
		if(i)
		{
			if(((MWPtr)i)->magic==MW_MAGIC)
				MWPart((MWPtr)i);
			else if(((pluginDlgInfoPtr)i)->magic==PLUGIN_MAGIC)
				pluginCloseWindow((WindowPtr)wp, (pluginDlgInfoPtr)i);
		}
	}
}

static pascal void cascade2(WindowPtr w)
{
	WindowPtr w2;
	MWPtr mw;
	Rect r;
	
	w2=GetNextWindow(w);
	if(w2)
		cascade2(w2);
	
	mw=MWFromWindow(w);
	if(mw)
	{
		MWNewPosition(&r);
		MWSetDimen(mw, r.left, r.top, r.right-r.left, r.bottom-r.top);
		UpdateWindowPosition(mw->w);
		MWNewPosition(&r); //twice to give some more space
	}
}

static pascal void DoCascadeWindows(void)
{
	WindowPtr w;
	
	SetRect(&cornerstone, 5, GetMBarHeight() + 16, 425, 340);
	w=FrontNonFloater();
	
	if(w)
		cascade2(w);
}

static pascal void DoTileWindows(void)
{
	WindowPtr w = FrontNonFloater();
	WindowPtr w2=w;
	int cnt = 0;
	int height;
	short windowTopHeight, windowBotHeight;
	short windowLeftWid, windowRightWid;
	
	if(w)
	{
		RgnHandle cr, sr;
		RectPtr crr, srr;
		
		if(hasAppearance11)
		{
			cr = NewRgn();
			sr = NewRgn();
			
			GetWindowRegion(w, kWindowContentRgn, cr);
			GetWindowRegion(w, kWindowStructureRgn, sr);
			crr = GetRegionBounds(cr, 0);
			srr = GetRegionBounds(sr, 0);
		}
#if !TARGET_CARBON
		else
		{
			WindowPeek wp = (WindowPeek)w;

			crr = &(**wp->contRgn).rgnBBox;
			srr = &(**wp->strucRgn).rgnBBox;
		}
#endif

		windowTopHeight = crr->top - srr->top;
		windowBotHeight = srr->bottom - crr->bottom;

		windowLeftWid = crr->left - srr->left;
		windowRightWid = srr->right - crr->right;

		if(hasAppearance11)
		{
			DisposeRgn(cr);
			DisposeRgn(sr);
		}
	}
	
	while(w)
	{
		if(IsVisible(w) && MWFromWindow(w))
			cnt++;
		
		w = GetNextWindow(w);
	}
	
	if(cnt>0)
	{
		short totHeight;
		Rect r;
		short mbarHeight = GetMBarHeight();
		Rect sb;
#if TARGET_CARBON
		sb = GetQDGlobalsScreenBits(0)->bounds;
#else
		sb=qd.screenBits.bounds;
#endif

		SetRect(&sb, -sb.left, -sb.top + mbarHeight + windowTopHeight, sb.right - sb.left, sb.bottom - sb.top - 35);
		totHeight = sb.bottom - sb.top;
		height = (totHeight / cnt) - windowTopHeight - windowBotHeight;
		
		r.left = windowLeftWid;
		r.right = sb.right - windowRightWid;
		r.top = sb.top;
		r.bottom = r.top + height;
		
		w = w2;
		while(w)
		{
			if(IsVisible(w))
			{
				MWPtr mw = MWFromWindow(w);
				if(mw)
				{
					MWSetDimen(mw, r.left, r.top, r.right-r.left, r.bottom-r.top);
					UpdateWindowPosition(mw->w);
					r.top+=height + windowTopHeight + windowBotHeight;
					r.bottom+=height + windowTopHeight + windowBotHeight;
				}
			}
			
			w = GetNextWindow(w);
		}
	}
}


#pragma mark -

static pascal void DoAbout(void)
{
	DialogPtr d;
	short i;
	Rect itemRect;
	Handle itemHandle;
	short itemType;
	
	d = GetNewDialog(128, 0, (WindowPtr)-1);
	SetDialogDefaultItem(d, 1);
	SetDlogFont(d);
	EnterModalDialog();
	ShowWindow(GetDialogWindow(d));

	GetDialogItem(d, 3, &itemType, &itemHandle, &itemRect);
	SetDialogItem(d, 3, itemType, (Handle)AboutDlgVersionFilter, &itemRect);
	GetDialogItem(d, 4, &itemType, &itemHandle, &itemRect);
	SetDialogItem(d, 4, itemType, (Handle)AboutDlgVersionFilter, &itemRect);
	GetDialogItem(d, 5, &itemType, &itemHandle, &itemRect);
	SetDialogItem(d, 5, itemType, (Handle)AboutDlgVersionFilter, &itemRect);

	do {
		ModalDialog(StdDlgFilter, &i);
	} while(!i);

	DisposeDialog(d);
	ExitModalDialog();
}

#pragma mark -

static pascal void HitFileMenu(int item)
{
	switch(item)
	{
		case 2:
			NewTextWin();
			break;
		
		case 3:
			TWSelect();
			break;
			
		case 4:
			CloseFrontWindow();
			break;
		
		case 6:
			if(MWActive && MWActive->winType == textWin)
				TWSave(MWActive, false);
			else
				writeAllFiles();
			break;

		case 9:
			doQuit(0);
			break;
	}
}

pascal void HitEditMenu(short item)
{
	char mwFront, otherFront;
	TEHandle te;
	WEReference we;
	long s0, s1;
	WindowPtr w = FrontNonFloater();
	
	if(item == 12)
	{
		OpenPreferencesWindow(-1);
		return;
	}
	
	if(w && IsDialog(w))
	{
		DialogPtr d = GetDialogFromWindow(w);
		
		if(item == 3)
		{
			DialogCut(d);
			ClearCurrentScrap();
			TEToScrap();
		}
		else if(item == 4)
		{
			DialogCopy(d);
			ClearCurrentScrap();
			TEToScrap();
		}
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
	
	if(MWActive)
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
		we=MWActive->we;
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
					if(MWActive && MWActive->winType == textWin)
						WEUndo(MWActive->we);
					else if(!noFloatingInput)
						WEUndo(ILGetWE());
				}
				break;
				
			case 3: //cut
				if(iwFront)
					WECut(ILGetWE());
				else if(mwFront && MWActive->winType==textWin)
					WECut(we);
				else if(otherFront)
				{
					TECut(te);
					ClearCurrentScrap();
					TEToScrap();
				}
				break;
			
			case 4: //copy
				if(otherFront)
				{
					TECopy(te);
					ClearCurrentScrap();
					TEToScrap();
				}
				else
					WECopy(we);
				break;
			
			case 5: //paste
				if(mwFront || iwFront)
				{
					if(!iwFront && MWActive->winType == textWin)
						WEPaste(MWActive->we);
					else
					{
						WEReference il = ILGetWE();
						
						if(il)
						{
							WEPaste(il);
							processPaste(MWActive, false);
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
	if(item == wConsoleItem)
	{
		if(FrontNonFloater() == consoleWin->w && IsVisible(consoleWin->w))
		{
			WHide(consoleWin->w);
			mainPrefs->consoleOpen=0;
		}
		else
		{
			 if(!IsVisible(consoleWin->w)) //not front and visible
			 	WShow(consoleWin->w);
			WSelect(consoleWin->w);
			mainPrefs->consoleOpen=1;
		}
	}
	else //check for service items
	{
		if(wMenuServices && item <= wLastServiceItem)
		{
			pServiceWindowMenuData dp;
			
			wMSDataPtr service = &(**wMenuServices).service[item - wFirstServiceItem];
			dp.key = service->key;
			dp.menuItem = item;
			runIndPlugin(service->pluginRef, pServiceWindowMenu, &dp);
		}
		else if(item == wNextWindowItem)
			DoCycleCommand(true);
		else if(item == wPrevWindowItem)
			DoCycleCommand(false);
		else if(item == wCascadeItem)
			DoCascadeWindows();
		else if(item == wTileItem)
			DoTileWindows();
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
}

static pascal void HitFontsMenu(short item)
{
	Str255 s;
	short i;
	long l;
	DialogPtr d;
	char b;
	
	if(item == gFontSizeOtherItem)
	{
		if(MWActive)
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
					MWSetFontSize(MWActive, -1, l);
					WEActivate(MWActive->we);
				}
			}
		}
	}
	else
	{
		GetMenuItemText(gFontsMenu, item, s);
		if(item>fontsBegin)
		{
			if(MWActive)
			{
				GetFNum(s, &i);
				MWSetFontSize(MWActive, i, -1);
				WEActivate(MWActive->we);
			}
		}
		else
		{
			StringToNum(s, &l);
			if(MWActive)
			{
				MWSetFontSize(MWActive, -1, l);
				WEActivate(MWActive->we);
			}
		}
	}
}

#pragma mark -

pascal void MenuConnectionList(short item)
{
	linkPtr link;
	
	if(item)
	{
		ConnStartNetworking();
		
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
		
		if(link->identConn)
			deleteConnection(&link->identConn);

		if(CurrentTarget.link == link)
			InvalTarget(&CurrentTarget);
		UpdateStatusLine();
	}
}

static pascal void MenuAppleURL(short item)
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
			case appleMenu:
				if(itemNum==1)
					DoAbout();
			#if !TARGET_CARBON
				else
					if(itemNum>applMItems)
					{
						Str255 s;
						
						GetMenuItemText(gAppleMenu, itemNum, s);
						OpenDeskAcc(s);
					}
			#endif
				break;
			
			case FileMenu:
				HitFileMenu(itemNum);
				break;
			case EditMenu:
				HitEditMenu(itemNum);
				break;
			case windowMenu:
				HitWindowMenu(itemNum);
				break;
			case shortcutsMenu:
				ShortcutsMenu(itemNum);
				break;
			case commandsMenu:
				HitCommandsMenu(itemNum, e->modifiers);
				break;
			case fontsMenu:
				HitFontsMenu(itemNum);
				break;
			
			case ConnectionListMenu:
				MenuConnectionList(itemNum);
				break;
			case SignoffConnectionListMenu:
				MenuSignoffConnectionList(itemNum);
				break;
			
			case AppleURLMenu:
				MenuAppleURL(itemNum);
				break;
			
		#if !TARGET_CARBON
			case kHMHelpMenuID:
				ShowHelp(itemNum - defaultHelpItems);
				break;
		#endif
		}
	}
	HiliteMenu(0);
}

pascal void MenuBarClick(const EventRecord *e)
{
	static previousFontCheck = 0;
	static previousSizeCheck = 0;
	Str255 s, s2;
	MenuHandle m;
	int x, y;
	long l;
	WindowPtr w;
	
	//Update file menu
	SetMenuItemText(gFileMenu, 6, GetIntStringPtr(spCM, smSavePreferences + (MWActive && MWActive->winType == textWin)));
	
	//Update edit menu
	if(MWActive)
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
	if(MWActive && gFontsMenu)
	{
		m = gFontsMenu;
		
		if(previousSizeCheck)
			CheckMenuItem(m, previousSizeCheck, false);
		
		for(x=1;x<fontsBegin;x++)
		{
			GetMenuItemText(m, x, s);
			StringToNum(s, &l);
			if(l == MWActive->size)
			{
				CheckMenuItem(m, x, true);
				previousSizeCheck = x;
				break;
			}
		}
		
		if(previousFontCheck)
			CheckMenuItem(m, previousFontCheck, false);
		
		GetFontName(MWActive->font, s);
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
	if((w = FrontNonFloater())!=0 && (w=GetNextWindow(w))!=0 && IsVisible(w))
	{
		EnableMenuItem(gWindowMenu, wPrevWindowItem);
		EnableMenuItem(gWindowMenu, wNextWindowItem);
	}
	else
	{
		DisableMenuItem(gWindowMenu, wPrevWindowItem);
		DisableMenuItem(gWindowMenu, wNextWindowItem);
	}
	CheckMenuItem(gWindowMenu, wConsoleItem, IsVisible(consoleWin->w));
	DoMenuEvent(MenuSelect(e->where), e);
}

#pragma mark -


inline void FontsMenuInit(void)
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
		AppendResMenu(mh, 'FONT');
	}
}

pascal void MenuInit(void)
{
	MenuHandle m;
	
	SetMenuBar(GetNewMBar(menuBar));
	gAppleMenu = m = GetMenuHandle(appleMenu);
	applMItems=CountMenuItems(m);
#if !TARGET_CARBON
	AppendResMenu(m, 'DRVR');
#endif
	
	gEditMenu = GetMenuHandle(EditMenu);
	gFileMenu = GetMenuHandle(FileMenu);
	FontsMenuInit();
	gShortcutsMenu = GetMenuHandle(shortcutsMenu);
	gWindowMenu = GetMenuHandle(windowMenu);
}