/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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

#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "connections.h"
#include "channels.h"
#include "plugins.h"
#include "IRCChannels.h"
#include "Inputline.h"
#include "MenuCommands.h"
#include "IRCInput.h"
#include "utils.h"
#include "inline.h"
#include "Shortcuts.h"
#include "IRCCFPrefs.h"

inline void RepeatMunger(Handle text, long offset, const void* ptr1, long len1, const void* ptr2, long len2);
inline void PluginShortcutText(Handle lh);
static void ParseShortcutText(ConstStr255Param s);

#define kNibShortcuts CFSTR("shortcuts")
#define kNibWinShortcuts CFSTR("Shortcuts")

enum {
	kShortcutItemSignature = 'SHOR',
	kShortcutDataProperty = 'SHOR',
	
	kShortcutSelectCommand = 'SSET'
};

static int gShortcutsWindowCount = 0;
static WindowRef gShortcutsWin = 0;

typedef struct ShortcutWindowData {
	int curSet;
	Str255 shorts[30];
} ShortcutWindowData, *ShortcutWindowDataPtr;

static ShortcutWindowDataPtr NewShortcutWindowData(void)
{
	ShortcutWindowDataPtr sdp = (ShortcutWindowDataPtr)NewPtr(sizeof(ShortcutWindowData));
	
	sdp->curSet = 0;
	BlockMoveData(mainPrefs->shortcuts, sdp->shorts, 30 * 256);
	
	return sdp;
}

static ShortcutWindowDataPtr GetShortcutWindowData(WindowRef shortcutsWin)
{
	ShortcutWindowDataPtr shorts;
	UInt32 actualSize;
	
	GetWindowProperty(shortcutsWin, kApplicationSignature, kShortcutDataProperty, sizeof(shorts), &actualSize, &shorts);
	
	return shorts;
}

static void SaveShortcutWindow(WindowRef shortcutsWin)
{
	ShortcutWindowDataPtr sdp = GetShortcutWindowData(shortcutsWin);
	
	// Send the data to the main prefs. This is to keep compatibility.
	// The application will keep it's own local cache of the prefs to
	// keep over modification of the current application down and also
	// excessive conversions between CF data types and native data types. 
	BlockMoveData(sdp->shorts, mainPrefs->shortcuts, sizeof(sdp->shorts));
        
	//New way to save the shortcut data.
	SaveShortcutDataCFPrefs(sdp->shorts);
	ShortcutsMenuUpdate();
}

static void ReadShortcutWindow(WindowRef shortcutsWin)
{
	ControlRef itemCtrl;
	ControlID item = { kShortcutItemSignature, 0};
	CFStringRef theStr;
	int x;
	ShortcutWindowDataPtr sdp;
	int group;
	
	sdp = GetShortcutWindowData(shortcutsWin);
	group = sdp->curSet * 10;
	
	for(x = 1; x <= 10; x++)
	{
		item.id = x;
		GetControlByID(shortcutsWin, &item, &itemCtrl);
		
		GetControlData(itemCtrl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theStr, NULL);
		CFStringGetPascalString(theStr, sdp->shorts[x + group - 1], sizeof(Str255), CFStringGetSystemEncoding());
	}
}

static void SetShortcutWindow(WindowRef shortcutsWin, int group)
{
	ControlRef itemCtrl;
	ControlID item = { kShortcutItemSignature, 0};
	CFStringRef theStr;
	int x;
	ShortcutWindowDataPtr sdp = GetShortcutWindowData(shortcutsWin);
	
	sdp->curSet = group;
	group *= 10;
	for(x = 1; x <= 10; x++)
	{
		item.id = x;
		GetControlByID(shortcutsWin, &item, &itemCtrl);
		
		theStr = CFStringCreateWithPascalString(NULL, sdp->shorts[x + group - 1], kCFStringEncodingMacRoman);
		SetControlData(itemCtrl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theStr);
		CFRelease(theStr);
		DrawOneControl(itemCtrl);
	}
}

static const EventTypeSpec shortEventTypes[] = {
	{ kEventClassWindow, kEventWindowActivated },
	{ kEventClassWindow, kEventWindowDeactivated },
	{ kEventClassWindow, kEventWindowClosed },
	{ kEventClassControl, kEventControlHit },
	{ kEventClassCommand, kEventProcessCommand }
};

static OSStatus ShortcutsEditorEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
#pragma unused(myHandler)
	OSStatus result = eventNotHandledErr;
	WindowRef shortcutsWin = (WindowRef)userData;
	UInt32 eventClass, eventKind;
	ShortcutWindowDataPtr sdp;
	UInt32 menuItem;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassWindow:
		{
			WindowRef theWindow = NULL;
			
			GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL, sizeof(WindowRef), NULL, &theWindow);
			
			switch(eventKind)
			{
				case kEventWindowActivated:
					HideFloatingWindows();
					result = noErr;
					break;
				case kEventWindowDeactivated:
				case kEventWindowClosed:
					ShowFloatingWindows();
					result = noErr;
					break;
			}
		}
		
		case kEventClassControl:
		{
			ControlRef theControl = NULL;
			UInt32 cmd;
			
			GetEventParameter(event, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &theControl);
			GetControlCommandID(theControl, &cmd);
			
			switch(cmd)
			{
				case kShortcutSelectCommand:
					ReadShortcutWindow(shortcutsWin);
					
					menuItem = GetControl32BitValue(theControl);
					
					SetShortcutWindow(shortcutsWin, menuItem - 1);
					break;
				
				case kHICommandOK:
					ReadShortcutWindow(shortcutsWin);
					SaveShortcutWindow(shortcutsWin);
					
				case kHICommandCancel:
					gShortcutsWin = 0;
					sdp = GetShortcutWindowData(shortcutsWin);
					DisposePtr((Ptr)sdp);
					DisposeWindow(shortcutsWin);
					
					gShortcutsWindowCount--;
					result = noErr;
					break;
			}
			break;
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

static void ActivateShortcutWindow(WindowRef shortcutsWin)
{
	ControlRef itemCtrl;
	const ControlID item = { kShortcutItemSignature, 1};
	
	GetControlByID(shortcutsWin, &item, &itemCtrl);

	ShowWindow(shortcutsWin);
	SelectWindow(shortcutsWin);
	
	SetKeyboardFocus(shortcutsWin, itemCtrl, kControlFocusNextPart);
}

void DoShortcutsEditor(void)
{
	static EventHandlerUPP swUPP = NULL;
	WindowRef shortcutsWin;
	ShortcutWindowDataPtr sdp;
	IBNibRef shortcutsNib;
	OSStatus status;
	
	if(++gShortcutsWindowCount > 1)
	{
		//Undo the change to the count and select the shortcuts window.
		gShortcutsWindowCount--;
		if(gShortcutsWin)
			ActivateShortcutWindow(gShortcutsWin);
		return;
	}
	
	sdp = NewShortcutWindowData();
	
	if(!swUPP)
		swUPP = NewEventHandlerUPP(ShortcutsEditorEventHandler);
	
	status = CreateNibReference(kNibShortcuts, &shortcutsNib);
	require_noerr(status, CantFindDialogNib);
	
	status = CreateWindowFromNib(shortcutsNib, kNibWinShortcuts, &shortcutsWin);
	require_noerr(status, CantCreateDialogWindow);
	
	DisposeNibReference(shortcutsNib);
	
	SetWindowProperty(shortcutsWin, kApplicationSignature, kShortcutDataProperty, sizeof(sdp), &sdp);
	
	status = InstallWindowEventHandler(shortcutsWin, swUPP, GetEventTypeCount(shortEventTypes), shortEventTypes, (void *)shortcutsWin, NULL);
	require_noerr(status, CantInstallDialogHandler);
	
	SetShortcutWindow(shortcutsWin, 0);
	
	ActivateShortcutWindow(shortcutsWin);
	
	gShortcutsWin = shortcutsWin;
	
CantFindDialogNib:
CantCreateDialogWindow:
CantInstallDialogHandler:
	return;
}

pascal void ShortcutsMenuUpdate(void)
{
	short x;
	Str255 s;
	MenuHandle mh;
	
	mh=gShortcutsMenu;
	for(x=0;x<30;x++)
	{
		pstrcpy(mainPrefs->shortcuts[x], s);
		if(!s[0])
		{
			*(short*)&s=0x0120; //' '
			DisableMenuItem(mh, 3+x);
		}
		else
			EnableMenuItem(mh, 3+x);
		
		if(s[0]>35)
		{
			s[0]=36;
			s[36]='É';
		}
		SetMenuItemText(mh, 3+x, s);
	}
}

#pragma mark -

inline void PluginShortcutText(Handle lh)
{
	pShortcutProcessData p;
	
	p.textH = lh;
	runPlugins(pShortcutProcessMessage, &p);
}

inline void RepeatMunger(Handle text, long offset, const void* ptr1, long len1, const void* ptr2, long len2)
{
	int i;
	do {
		i = Munger(text, offset, ptr1, len1, ptr2, len2);
	} while(i >= 0);
}

pascal void ProcessShortcutText(LongString *ls, short *cursorPos, short *endPos)
{
	Handle lh;
	long s;
	Str255 st;
	long l;
	LongString ilt;
	
	if(ls->len)
	{
		lh=NewHandle(ls->len);
		if(lh)
		{
			inputAreaDataPtr iad = ILGetInputDataFromMW(0);
			
			BlockMoveData(&ls->data[1], *lh, ls->len);
			
			if(CurrentTarget.bad)
				st[0]=0;
			else
				MWGetName(CurrentTarget.mw, st);
			
			RepeatMunger(lh, 0, "$channel", 8, &st[1], st[0]);
			RepeatMunger(lh, 0, "$nick", 5, &CurrentTarget.link->CurrentNick[1], CurrentTarget.link->CurrentNick[0]);
			RepeatMunger(lh, 0, "$server", 7, &CurrentTarget.link->CurrentServer[1], CurrentTarget.link->CurrentServer[0]);
			
			if(CurrentTarget.chan)
				pstrcpy(CurrentTarget.chan->lastJoiner, st);
			else if((CurrentTarget.dcc && CurrentTarget.dcc->dcc) ||
						(CurrentTarget.type==targQuery &&!CurrentTarget.bad))
				MWGetName(CurrentTarget.mw, st);
			else
				st[0]=0;

			RepeatMunger(lh, 0, "$lastjoiner", 11, &st[1], st[0]);
			RepeatMunger(lh, 0, "$version", 8, &CL_VERSION[1], CL_VERSION[0]);
			
			GetDateTime((unsigned long*)&l);
			DateString(l, longDate, st, 0);
			RepeatMunger(lh, 0, "$date.l", 7, &st[1], st[0]);
			
			DateString(l, shortDate, st, 0);
			RepeatMunger(lh, 0, "$date", 5, &st[1], st[0]);
			
			TimeString(l, true, st, 0);
			RepeatMunger(lh, 0, "$time.s", 7, &st[1], st[0]);
			
			TimeString(l, false, st, 0);
			RepeatMunger(lh, 0, "$time", 5, &st[1], st[0]);
			
			PluginShortcutText(lh);
			
			IADGetText(iad, &ilt);
			RepeatMunger(lh, 0, "$il", 3, &ilt.data[1], ilt.len);
			
			if(cursorPos)
			{
				*cursorPos=Munger(lh, 0, "$->", 3, (Ptr)-1, 0);
				if(endPos)
					*endPos=Munger(lh, 0, "$<-", 3, (Ptr)-1, 0);
			}
			
			s=GetHandleSize(lh);
			if(s>maxLSlen)
				s=maxLSlen-20;
			ls->len=s;
			BlockMoveData(*lh, &ls->data[1], s);
			DisposeHandle(lh);
		}
	}
}

static void ParseShortcutText(ConstStr255Param s)
{
	Str255 s2,s3;
	LongString ls;
	int p;
	int x;
	short cp, ep;
	Handle h;
	long l1,l2;
	inputAreaDataPtr iad = ILGetInputDataFromMW(0);
	
	if(!iad)
		return;
	
	pstrcpy(s, s2);
	do
	{
		x=1;
		p=0;
		while(x<s2[0])
		{
			if(s2[x]=='\\')
			{
				if(s2[x+1]=='n')
				{
					p=x;
					break;
				}
				else
					x+=2;
			}
			else
				x++;
		}
		if(p)
		{
			myCopy(s2, 1, p-1, s3);
			LSStrLS(s3, &ls);
			pdelete(s2, 1, p+1);
			ProcessShortcutText(&ls, &cp, &ep);
			IADSetText(iad, &ls);
			GetLine(false, CurrentTarget.mw);
		}
		else
		{
			LSStrLS(s2, &ls);
			ProcessShortcutText(&ls, &cp, &ep);
			p=0;
			x=0;
			if(cp>=0) //check for a selection and get it, if there is one
			{
				IADGetCursorSelection(iad, &l1, &l2);
				if(l1!=l2)
				{
					IADGetTextHandleRange(iad, &h, l1, l2);
					IADSetText(iad, &ls);
					if(ep<0)
						ep=cp;
					
					if(l2-l1!=0)
					{
						HLock(h);
						IADSetTextPtrRange(iad, *h, l2-l1, cp, ep);
						IADSetCursorSelection(iad, cp, (l2-l1)+cp);
					}
					else
						IADSetCursorSelection(iad, cp, ep);
					
					DisposeHandle(h);
				}
				else
				{
					IADSetText(iad, &ls);
					if(ep<cp)
						ep=cp;
					IADSetCursorSelection(iad, cp,ep);
				}
			}
			else
				IADSetText(iad, &ls);
			s2[0]=0;
		}
	}while(s2[0]);
}

pascal void ShortcutsMenu(short item)
{
	if(item >= 3)
	{
		if(mainPrefs->shortcuts[item-3][0])
			ParseShortcutText(mainPrefs->shortcuts[item-3]);
	}
}

void ShortcutsMenuInit(MenuHandle sm)
{
	int x;
	const int kShortcutsOffset = 2;
	
	for(x = 1; x <= 30; x++)
	{
		AppendMenuItemText(sm, "\p.");
		SetMenuItemCommandID(sm, kShortcutsOffset + x, 'SHOR');
		SetItemCmd(sm, kShortcutsOffset + x, '0' + x % 10);
		if(x >= 11 && x <= 20)
			SetMenuItemModifiers(sm, kShortcutsOffset + x, kMenuOptionModifier);
		else if(x > 20)
			SetMenuItemModifiers(sm, kShortcutsOffset + x, kMenuControlModifier | kMenuNoCommandModifier);
	}
}

