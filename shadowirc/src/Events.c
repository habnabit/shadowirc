/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 2002 John Bafford, Chris Campbell
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

#include "WASTE.h" // WECountLines()
#include "Events.h"
#include "IRCPreferences.h" // OpenPreferencesWindow()
#include "IRCGlobals.h"
#include "LongStrings.h"
#include "plugins.h"
#include "filesMan.h"
#include "ApplBase.h"
#include "utils.h"
#include "MsgWindows.h"
#include "IRCChannels.h"
#include "Shortcuts.h"
#include "Floaters.h"
#include "MenuCommands.h"
#include "TextWindows.h"

static void DoAbout(void);

static void cascade2(WindowPtr w);
static void DoCascadeWindows(void);
static void DoTileWindows(void);
static void DoCycleCommand(char next);

/*
* Carbon Event Handlers
*/

static OSStatus MWDoMouseWheelEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData)
{
	OSStatus myErr = eventNotHandledErr;
	Point mouseLoc;
	EventMouseWheelAxis wheelAxis;
	long wheelDelta;
	MWPtr mw = userData;
	
	GetEventParameter(theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(mouseLoc), NULL, &mouseLoc);
	GetEventParameter(theEvent, kEventParamMouseWheelAxis, typeMouseWheelAxis, NULL, sizeof(wheelAxis), NULL, &wheelAxis);
	GetEventParameter(theEvent, kEventParamMouseWheelDelta, typeLongInteger, NULL, sizeof(wheelDelta), NULL, &wheelDelta);
	
	if(wheelAxis == kEventMouseWheelAxisY)
	{
		// Scroll the vertical scroll bar
		int maxValue = GetControl32BitMaximum(mw->vscr);
		int numLines = WECountLines(mw->we);
		float lineValue = (float)maxValue/(float)numLines;
		long adjustedDelta = (((float)lineValue * (float)wheelDelta) + ((wheelDelta < 0)?(-0.5):(0.5)));
		
		MWScroll((MWPtr)userData, 4 * -adjustedDelta);
		myErr = noErr;
	}
	
	return myErr;
}

void MWInstallMouseWheelHandlers(MWPtr mw)
{
	EventTypeSpec wheelType = {kEventClassMouse, kEventMouseWheelMoved};
	
	InstallWindowEventHandler(mw->w, NewEventHandlerUPP(MWDoMouseWheelEvent), 1, &wheelType, mw, NULL);
}

#pragma mark -

void WindowClose(WindowPtr wp)
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

static void CloseFrontWindow(void)
{
	WindowClose(FrontNonFloater());
}

#pragma mark -

static void DoAbout(void)
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

static void DoCycleCommand(char next)
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

static void cascade2(WindowPtr w)
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

static void DoCascadeWindows(void)
{
	WindowPtr w;
	
	SetRect(&cornerstone, 5, GetMBarHeight() + 16, 425, 340);
	w=FrontNonFloater();
	
	if(w)
		cascade2(w);
}

static void DoTileWindows(void)
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
		Rect crr, srr;
		
		cr = NewRgn();
		sr = NewRgn();
			
		GetWindowRegion(w, kWindowContentRgn, cr);
		GetWindowRegion(w, kWindowStructureRgn, sr);
		GetRegionBounds(cr, &crr);
		GetRegionBounds(sr, &srr);

		windowTopHeight = crr.top - srr.top;
		windowBotHeight = srr.bottom - crr.bottom;

		windowLeftWid = crr.left - srr.left;
		windowRightWid = srr.right - crr.right;

		DisposeRgn(cr);
		DisposeRgn(sr);
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
		BitMap screenBits;
		Rect sb;
		
		GetQDGlobalsScreenBits(&screenBits);
		sb = screenBits.bounds;

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


static OSStatus DoCommandEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData)
{
	OSStatus myErr = eventNotHandledErr;
	HICommand hiCommand;
	
	GetEventParameter(theEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(hiCommand), NULL, &hiCommand);
	
	switch(hiCommand.commandID)
	{
		//Apple Menu
		case kHICommandAbout:
			DoAbout();
			return noErr;
		
		case kHICommandPreferences: // Open the Preferences window
			OpenPreferencesWindow(-1);
			return noErr;
		
		
		//File Menu
		case 'NTWN':
			NewTextWin();
			return noErr;
		
		case kHICommandOpen:
			TWSelect();
			return noErr;
		
		case kHICommandClose:
			CloseFrontWindow();
			return noErr;
		
		case kHICommandSave:
			if(MWActive && MWActive->winType == textWin)
				TWSave(MWActive, false);
			else
				writeAllFiles();
			return noErr;
		
		//Window Menu
		case 'CONS':
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
			return noErr;
		
		case 'PWIN':
			DoCycleCommand(false);
			return noErr;
			
		case 'NWIN':
			DoCycleCommand(true);
			return noErr;
			
		case 'WCAS':
			DoCascadeWindows();
			return noErr;
			
		case 'WTIL':
			DoTileWindows();
			return noErr;
		
		//Shortcuts Menu
		case 'ESHT':
			DoShortcutsEditor();
			return noErr;
		
		case 'SHOR':
			ShortcutsMenu(hiCommand.menu.menuItemIndex);
			return noErr;
		
		//Help Menu
		case 'HWIN':
			ShowHelp(hiCommand.menu.menuItemIndex - defaultHelpItems);
			return noErr;
	}
	
	return myErr;
}

void InitEventHandlers()
{
	EventTypeSpec commandType;
	
	commandType.eventClass = kEventClassCommand;
	commandType.eventKind  = kEventProcessCommand;
	
	InstallApplicationEventHandler(NewEventHandlerUPP(DoCommandEvent), 1, &commandType, NULL, NULL);
}
