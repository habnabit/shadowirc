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

#include "StringList.h"
#include "IRCGlobals.h"
#include "utils.h"
#include "MsgWindows.h"
#include "MWPanes.h"
#include "InputLine.h"
#include "IRCChannels.h"
#include "DCC.h"
#include "connections.h"
#include "plugins.h"
#include "CMenus.h"
#include "IRCCommands.h"
#include "IRCAux.h"
#include "ApplBase.h"
#include "AppearanceHelp.h"
#include "Floaters.h"
#include "IRCInput.h"
#include "MenuCommands.h"

static void _CMILAdd(cmmwData *d, ConstStr255Param item, menuCommand cmd, long key, short *id, MenuHandle *m);
static void CMILAdd(cmmwData *d, ConstStr255Param item, menuCommand cmd, long key);
inline void CMILDelete(cmmwData *d, short item);
inline void CMILStripEnd(cmmwData *d);
static void CMILDispose(cmmwData *d);
static void CMSubmenu(cmmwData *d, short menuID, short item);

static void CMMWMake(cmmwData *d);
static void CMDoPopup(cmmwData *d, short item);
static void MyWEFindWord(long offset, long *selStart, long *selEnd, WEReference we);

static OSErr MyCMSelect(MenuRef inMenuRef, Point inGlobalLocation, Boolean inBalloonAvailable, UInt32 inHelpType, ConstStr255Param inHelpItemString, const AEDesc* inSelection, UInt32* outUserSelectionType, SInt16* outMenuID, UInt16* outMenuItem);

static char CMMW(MWPtr mw, Point where, char optCM);
static char CMIW(Point where);
static char CMPlugin(WindowPtr w, Point where);

void CMSetCursor(char ctrlDown)
{
	if(ctrlDown)
		SetThemeCursor(kThemeContextualMenuArrowCursor);
	else
		SetThemeCursor(kThemeArrowCursor);
}

static OSErr MyCMSelect(MenuRef inMenuRef, Point inGlobalLocation, Boolean inBalloonAvailable, UInt32 inHelpType, ConstStr255Param inHelpItemString, const AEDesc* inSelection, UInt32* outUserSelectionType, SInt16* outMenuID, UInt16* outMenuItem)
{
	OSErr ret;
	GrafPtr gp;
	
	GetPort(&gp);

	ret = ContextualMenuSelect(inMenuRef, inGlobalLocation, inBalloonAvailable, inHelpType, inHelpItemString, inSelection, (unsigned long*)outUserSelectionType, outMenuID, outMenuItem);

	SetPort(gp);
	
	return ret;
}

pascal char CMClick(WindowPtr w, const EventRecord *e)
{
	char stdCM = IsShowContextualMenuClick(e);
	char optCM = 0;
	
	if(!stdCM&& ((e->modifiers & (optionKey | shiftKey | controlKey | cmdKey)) == optionKey))
		optCM = 1;
	
	if(stdCM || optCM)
	{
		if(!WIsFloater(w) && !IsWindowHilited(w))
			WSelect(w);
		
		return DetermineCM(w, e->where, optCM);
	}
	else
		return 0;
}

#pragma mark -

pascal void CMAdd(cmmwData *d, ConstStr255Param item, long key)
{
	_CMILAdd(d, item, mcPlugin, key, 0, 0);
}

pascal void CMAddSubmenu(cmmwData *d, ConstStr255Param item, short *id, MenuHandle *m)
{
	_CMILAdd(d, item, mcSubmenu, 0, id, m);
}

pascal void CMSetCheckmark(cmmwData *d, long key, char checked)
{
	CMItemListH list = d->items;
	CMItem *items;
	long i, max;
	
	if(list)
	{
		items = (**list).list;
		max = (**list).numItems;
		for(i=1;i<= max;i++)
			if(items[i].key == key)
			{
				CheckMenuItem(d->m, i, checked);
				break;
			}
	}
}

static int CMCountItems(cmmwData *d)
{
	CMItemListH list = d->items;
	
	if(!list)
		return 0;
	else
		return (**list).numItems;
}

static void _CMILAdd(cmmwData *d, ConstStr255Param item, menuCommand cmd, long key, short *id, MenuHandle *m)
{
	CMItemListH list = d->items;
	int i;
	CMItem *items;
	Str255 s;
	
	if(!list)
		list = d->items = (CMItemListH)NewHandleClear(sizeof(int)+sizeof(CMItem));
	
	i = ++(**list).numItems;
	if(i>1)
	{
		GetMenuItemText(d->m, i-1, s);
		if(*(short*)s == 0x012D && *(short*)item == 0x012D) //Don't add two separators next to ech other
			return;
	}
	SetHandleSize((Handle)list, sizeof(int) + (sizeof(CMItem) * (i+1)));
	
	HLock((Handle)list);
	items = (**list).list;
	AppendMenu(d->m, "\p-");
	SetMenuItemText(d->m, i, item);
	items[i].command = cmd;
	if(cmd == mcPlugin)
	{
		items[i].pluginRef = sidr.yourInfo;
		items[i].key = key;
	}
	
	if(cmd == mcSubmenu)
	{
		items[i].pluginRef = sidr.yourInfo;
		items[i].key = 0;

		if(++d->popupsMenuID==0)
			d->popupsMenuID = 150;
		SetItemCmd(d->m, i, hMenuCmd);
		SetItemMark(d->m, i, d->popupsMenuID);
		*id = items[i].mid = d->popupsMenuID;
		*m = items[i].m = NewMenu(d->popupsMenuID, "\p");
		InsertMenu(*m, -1);
		
	}
	else
	{
		items[i].mid = 0;
		items[i].m = 0;
	}

	HUnlock((Handle)list);
}

pascal void CMILAdd(cmmwData *d, ConstStr255Param item, menuCommand cmd, long key)
{
 	_CMILAdd(d, item, cmd, key, 0, 0);
}

inline void CMILDelete(cmmwData *d, short item)
{
	CMItemListH list = d->items;
	
	if(list && (item == (**list).numItems))
	{
		SetHandleSize((Handle)list, sizeof(int) + (sizeof(CMItem) * item));
		DeleteMenuItem(d->m, item);
		(**list).numItems--;
	}
}

inline void CMILStripEnd(cmmwData *d)
{
	CMItemListH list = d->items;
	Str255 s;
	int i;
	
	 if(list)
	 {
		i= (**list).numItems;
		
		//Trash ll the separators at the end.
		while(1)
		{
			GetMenuItemText(d->m, i, s);
			if(*(short*)s == 0x012D) //-
			{
				CMILDelete(d, i);
				i--;
			}
			else
				break;
		}
	}
}

static void CMILDispose(cmmwData *d)
{
	int x;
	CMItem *list;
	int i;
	
	if(d->items)
	{
		HLock((Handle)d->items);
		
		list = (**d->items).list;
		i= (**d->items).numItems;
		for(x=1;x<=i;x++)
		{
			if(list[x].command == mcSubmenu)
			{
				DeleteMenu(list[x].mid);
				DisposeMenu(list[x].m);
			}
		}

		d->items = 0;
		DisposeHandle((Handle)d->items);
	}

	d->popupsMenuID = -1;
}

#pragma mark -

static void CMDoPopup(cmmwData *d, short item)
{
	CMItem *list = (**d->items).list;
	
	if(list[item].pluginRef) //plugin
	{
		d->menuID = -1;
		d->id = list[item].key;
		runIndPlugin(list[item].pluginRef, pCMPopupsReturnMessage, d);
	}
}

static void CMSubmenu(cmmwData *d, short menuID, short item)
{
	CMItem *list;
	int x;
	
	HLock((Handle)d->items);
	list = (**d->items).list;
	
	for(x = 1; x <= (**d->items).numItems; x++)
	{
		if(list[x].command == mcSubmenu && list[x].mid == menuID)
		{
			d->menuID = menuID;
			d->id = item;
			runIndPlugin(list[x].pluginRef, pCMPopupsReturnMessage, d);
			break;
		}
	}

	HUnlock((Handle)d->items);
}

static void CMMWMake(cmmwData *d)
{
	MWPtr mw = d->mw;
	linkPtr link = mw->link;
	int item;
	Str255 s;
	int x;
	MenuHandle m;
	
	m = d->m = NewMenu(200, "\p");
	WEGetSelection(&d->s0, &d->s1, mw->we);
	d->noSelection = d->s0 == d->s1;
	
	if(!d->noSelection)
	{
		Handle text = WEGetText(mw->we);
		Ptr p = *text;
		int x;
		
		d->oneWord = true;
		for(x = d->s0; x<d->s1; x++)
			if(p[x] == ' ')
			{
				d->oneWord = false;
				break;
			}
		
		if(d->oneWord)
		{
			x = d->s1 - d->s0 +1;
			if(x>255)
				x=255;
			BlockMoveData(&p[d->s0], &d->theWord[1], x-1);
			d->theWord[0] = x-1;
		}
	}
	
	if(d->noSelection && d->pane->creator == kApplicationSignature)
	{
		CMILAdd(d, GetIntStringPtr(spCM, sCloseWindow), mcCloseWindow, 0);
		CMILAdd(d, "\p-", mcNull, 0);
		CMILAdd(d, GetIntStringPtr(spCM, sCopyAll), mcCopy, 0);

		CMILAdd(d, GetIntStringPtr(spCM, sClearAll), mcClear, 0);
		
		CMILAdd(d, "\p-", mcNull, 0);
		
		CMILAdd(d, GetIntStringPtr(spCM, sColorStyle), mcNull, 0);
		item = (**d->items).numItems;
		SetItemCmd(m, item, hMenuCmd);
		SetItemMark(m, item, 203);

		d->colorMenu = GetMenu(203);
		
		InsertMenuItem(d->colorMenu, GetIntStringPtr(spCM, sNone), 0);
		InsertMenuItem(d->colorMenu, "\p-", 1);
		InsertMenu(d->colorMenu, -1);
		if(mw->colorMethod == cmNone)
			x = 1;
		else
			x = mw->colorMethod +2;
		CheckMenuItem(d->colorMenu, x, true);
	}
	else //selection
	{
		if(d->pane->creator == kApplicationSignature)
		{
			CMILAdd(d, GetIntStringPtr(spCM, sCopySelection), mcCopy, 0);
			CMILAdd(d, GetIntStringPtr(spCM, sCopyToInputline), mcCopyIL, 0);

			if(!d->oneWord)
				CMILAdd(d, GetIntStringPtr(spCM, sClearSelection), mcClear, 0);
		}
	}

	if(d->pane->creator == kApplicationSignature)
	{
		ConstStringPtr s;
		
		if(!d->oneWord)
			CMILAdd(d, "\p-", mcNull, 0);

		if(d->noSelection)
		{
			if(mw->logRefNum)
				s = GetIntStringPtr(spCM, sStopLogging);
			else
				s = GetIntStringPtr(spCM, sStartLogging);
			
			CMILAdd(d, s, mcLog, 0);
		}
		
		if(!d->oneWord)
		{
			if(d->noSelection)
				s = GetIntStringPtr(spCM, sSaveWindowTextToFile);
			else
				s = GetIntStringPtr(spCM, sSaveSelectionToFile);
			CMILAdd(d, s, mcSaveText, 0);
		}
	}
	
	if(d->noSelection && d->pane->creator == kApplicationSignature)
	{
		if(mw->winType == conWin)
		{
			CMILAdd(d, "\p-", mcNull, 0);
			
			CMILAdd(d, GetIntStringPtr(spCM, sConnectMenu), mcNull, 0);

			item = (**d->items).numItems;
			SetItemCmd(m, item, hMenuCmd);
			SetItemMark(m, item, ConnectionListMenu);

			CMILAdd(d, GetIntStringPtr(spCM, sDisconnectMenu), mcNull, 0);
			
			SetItemCmd(m, item+1, hMenuCmd);
			SetItemMark(m, item+1, SignoffConnectionListMenu);
		}
		else if(mw->winType == chanWin && link)
		{
			CMILAdd(d, "\p-", mcNull, 0);
			GetIntString(s, spCM, sConnectTo + (link->conn != 0));
			ParamString(s, link->linkPrefs->linkName, 0, 0, 0);
			CMILAdd(d, s, mcConnect, 0);
		}
		else if(mw->winType == dccWin && !mw->inactive)
		{
			CMILAdd(d, "\p-", mcNull, 0);
			CMILAdd(d, GetIntStringPtr(spCM, sCloseDCCConn), mcConnect, 0);
		}
	}
	
	if(CMCountItems(d) > 0)
		CMILAdd(d, "\p-", mcNull, 0);
	
	runPlugins(pCMPopupsMessage, d);
}

inline char IsWS(char c)
{
	if(c==' ' || c== 10 || c==13 || c=='\t')
		return 1;
	else
		return 0;
}

static char IsPunc(char c)
{
	switch(c)
	{
		case '!':
		case '@':
		case '(':
		case ')':
		case '.':
		case ',':
		case ':':
		case '+':
		case '"':
		case '\'':
		case '?':
		case '<':
		case '>':
		case '*':
			return 1;
		
		default:
			return 0;
	}
}

static void MyWEFindWord(long offset, long *selStart, long *selEnd, WEReference we)
{
	Handle text;
	Ptr t;
	long s0, s1;
	
	*selStart = *selEnd = 0;
	text = WEGetText(we);
	t = *text;
	
	if(IsWS(t[offset]))
		return;
	
	s0 = s1 = offset;
	
	//Expand until whitespace
	for(;s1 && !IsWS(t[s1]);s1++)
		;
	s1--;
	
	for(;s0 && !IsWS(t[s0]);s0--)
		;
	s0++;
	
	//Skip punc at begining...
	while(IsPunc(t[s0]))
		s0++;
	
	//Move end back past punc; stop at s0
	while(s1 > s0 && IsPunc(t[s1]))
		s1--;

	s1++;
	*selStart = s0;
	*selEnd = s1;
}

static void CMCreate(cmmwData *d)
{
	d->type = 0;

	d->window = 0;
	d->mw = 0;
	d->pane = 0;
	*(long*)&d->where = 0;

	d->s0 = d->s1 = 0;
	d->noSelection=0;
	d->oneWord = 0;
	d->theWord[0] = 0;

	d->id = 0;
	d->menuID = 0;
	d->popupsMenuID = -1;

	d->colorMenu = 0;
	d->m = 0;
	d->items=0;
}

static char CMMW(MWPtr mw, Point where, char optCM)
{
	cmmwData d;
	short outMenuID;
	unsigned short outMenuItem;
	long outUserSelectionType;
	Point local = where;
	char textPaneOCM;
	long ps0;
	GrafPtr gp;
	
	GetPort(&gp);
	SetPortWindowPort(mw->w);
	GlobalToLocal(&local);
	SetPort(gp);

	CMCreate(&d);
	d.type = cmMW;
	d.window = mw->w;
	d.mw = mw;
	d.where = where;
	d.pane = MWFindPanePoint(mw, local);

	if(!d.pane)
	{
		d.pane = mw->textPane;
		textPaneOCM = false;
	}
	else
	{
		if(d.pane->type == mwTextPane && optCM)
		{
			LongPt pt;
			long off;
			WEEdge edge;
			long s0, s1;
			
			pt.h = local.h;
			pt.v = local.v;
			
			WEGetSelection(&ps0, &s1, mw->we);
			off = WEGetOffset(&pt, &edge, mw->we);
			MyWEFindWord(off, &s0, &s1, mw->we);
			if(s0 && s1)
				WESetSelection(s0, s1, mw->we);

			textPaneOCM = true;
		}
		else
			textPaneOCM = false;
	}
	
	CMMWMake(&d);
	CMILStripEnd(&d);
	
	InsertMenu(d.m, -1);
	MyCMSelect(d.m, where, 0, 0, 0, 0, (unsigned long*)&outUserSelectionType, &outMenuID, &outMenuItem);
	
	switch(outMenuID)
	{
		case 200: //MW menu
			if(mw->winType == pluginWin && outMenuItem>100)
			{
			}
			else if(outMenuItem > 0)
			{
				switch((**d.items).list[outMenuItem].command)
				{
					case mcCloseWindow: //Close window
						MWPart(mw);
						break;
					
					case mcCopy:
						if(d.noSelection)
						{
							WEDeactivate(mw->we);
							WESetSelection(0, 0x7FFFFFFF, mw->we);
						}
						WECopy(mw->we);
						WESetSelection(d.s0, d.s0,mw->we);
						WEActivate(mw->we);
						break;
					
					case mcCopyIL:
						WECopy(mw->we);
						WESetSelection(d.s0, d.s0, mw->we);
						HitEditMenu(5);
						break;
					
					case mcClear:
						if(d.noSelection)
						{
							WEDeactivate(mw->we);
							WESetSelection(0, 0x7FFFFFFF, mw->we);
						}
						WEDelete(mw->we);
						WEActivate(mw->we);
						break;
					
					case mcLog:
						if(mw->logRefNum) //is logging, so stop
							MWStopLogging(mw);
						else
							MWStartLogging(mw);
						break;
					
					case mcSaveText: //log window to file
						MWLogToFile(mw, d.s0, d.s1);
						break;
						
					case mcConnect: //connect/disconnect
						if(mw->winType == chanWin)
						{
							if(mw->link->conn)
								DoSignoff(mw->link, 0);
							else
								OpenConnection(mw->link);
						}
						else if(mw->winType == dccWin)
						{
							connectionPtr c = MWGetDCC(mw);
							DCCClose(&c, false);
							MWSetDCC(mw, 0, "\p");
						}
						break;
					
					case mcPlugin:
						CMDoPopup(&d, outMenuItem);
						WESetSelection(d.s0, d.s0, mw->we);
						break;
				}
			}
			break;
		
		case ConnectionListMenu: //from console
			MenuConnectionList(outMenuItem);
			break;
		
		case SignoffConnectionListMenu: //from console
			MenuSignoffConnectionList(outMenuItem);
			break;
			
		case 203: //color styles
			if(outMenuItem > 0)
			{
				if(outMenuItem == 1)
					mw->colorMethod = cmNone;
				else
					mw->colorMethod = outMenuItem - 2;
			}
			break;
		
		default: //plugin popup
			if(outMenuID > 0)
			{
				WESetSelection(d.s0, d.s0, mw->we);
				CMSubmenu(&d, outMenuID, outMenuItem);
			}
	}
	
	DeleteMenu(200);
	DisposeMenu(d.m);
	CMILDispose(&d);
	if(d.colorMenu)
	{
		DeleteMenu(203);
		DisposeMenu(d.colorMenu);
	}
	
	if(textPaneOCM && MWValid(mw))
		WESetSelection(ps0, ps0, mw->we);
	
	return true;
}

static char CMIW(Point where)
{
	cmmwData d;
	short outMenuID;
	unsigned short outMenuItem;
	long outUserSelectionType;
	WEReference il = ILGetWE();
	
	CMCreate(&d);
	d.type = cmInputline;
	d.window = inputLine.w;
	d.where = where;
	
	d.m=NewMenu(201, "\p");
	WEGetSelection(&d.s0, &d.s1, il);
	if(d.s0==d.s1) //No selection
	{
		LongString ls;
		
		GetInputLine(&ls);
		if(ls.len)
		{
			CMILAdd(&d, GetIntStringPtr(spCM, sCutAll), mcILCut, 0);
			CMILAdd(&d, GetIntStringPtr(spCM, sCopyAll), mcILCopy, 0);
		}
		CMILAdd(&d, GetIntStringPtr(spCM, sPaste), mcILPaste, 0);
		
		if(ls.len)
			CMILAdd(&d, GetIntStringPtr(spCM, sClearAll), mcILClear, 0);
	}
	else
	{
		CMILAdd(&d, GetIntStringPtr(spCM, sCutS), mcILCut, 0);
		CMILAdd(&d, GetIntStringPtr(spCM, sCopyS), mcILCopy, 0);
		CMILAdd(&d, GetIntStringPtr(spCM, sPaste), mcILPaste, 0);
		CMILAdd(&d, GetIntStringPtr(spCM, sClearS), mcILClear, 0);
	}
	
	InsertMenu(d.m, -1);
	MyCMSelect(d.m, where, 0, 0, 0, 0, (unsigned long*)&outUserSelectionType, &outMenuID, &outMenuItem);
	
	switch(outMenuID)
	{
		case 201: //Inputline
			if(outMenuItem > 0)
			{
				switch((**d.items).list[outMenuItem].command)
				{
					case mcILCut:
						if(d.s0==d.s1)
						{
							WEDeactivate(il);
							WESetSelection(0, 0x7FFFFF, il);
							WECut(il);
							WEActivate(il);
						}
						else
							WECut(il);
						break;
						
					case mcILCopy:
						if(d.s0==d.s1)
						{
							WEDeactivate(il);
							WESetSelection(0, 0x7FFFFF, il);
							WECopy(il);
							WESetSelection(d.s0, d.s1, il);
							WEActivate(il);
						}
						else
							WECopy(il);
						break;
						
					case mcILPaste:
						HitEditMenu(5);
						break;
						
					case mcILClear:
					{
						WEDeactivate(il);
						if(d.s0==d.s1)
							WESetSelection(0,0x7FFFFFFF, il);
						WEDelete(il);
						WEActivate(il);
					}
					break;
					
					case mcPlugin:
						break;
				}
				break;
			}

		default: //plugin popup
			if(outMenuID > 0)
				CMSubmenu(&d, outMenuID, outMenuItem);
	}

	DeleteMenu(201);
	DisposeMenu(d.m);
	CMILDispose(&d);
	return 1;
}

static char CMPlugin(WindowPtr w, Point where)
{
	cmmwData d;
	MenuHandle m;
	unsigned long outUserSelectionType;
	unsigned short outMenuItem;
	char r;
	long l=GetWRefCon(w);
	
	if(l && ((pluginDlgInfoPtr)l)->magic==PLUGIN_MAGIC)
	{
		CMCreate(&d);
		d.type = cmPlugin;
		d.window = w;
		d.where = where;
		m = d.m = NewMenu(200, "\p");
		
		runIndPlugin(((pluginDlgInfoPtr)l)->pluginRef, pCMPopupsMessage, &d);
		
		if(CountMenuItems(d.m))
		{
			InsertMenu(d.m, -1);
			MyCMSelect(d.m, where, 0, 0, 0, 0, (unsigned long*)&outUserSelectionType, &d.menuID, &outMenuItem);
			
			if(d.menuID > 0)
			{
				if(d.menuID == 200)
					CMDoPopup(&d, outMenuItem);
				else
					CMSubmenu(&d, d.menuID, outMenuItem);
			}
			r=1;
		}
		else
			r=0;
			
		DeleteMenu(200);
		DisposeMenu(d.m);
		CMILDispose(&d);
		return r;
	}
	else
		return 0;
}

pascal char DetermineCM(WindowPtr w, Point where, char optCM)
{
	MWPtr mw = MWFromWindow(w);
	char ret;
	
	if(mw)
		ret = CMMW(mw, where, optCM);
	else if(w==inputLine.w)
		ret = CMIW(where);
	else
		ret = CMPlugin(w, where);
	
	return ret;
}
