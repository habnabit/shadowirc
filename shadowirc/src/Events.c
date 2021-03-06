/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 2002-2004 John Bafford, Chris Campbell
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

#include "channels.h"
#include "MsgWindows.h"
#include "DragDrop.h"
#include "MWPanes.h"
#include "InputLine.h"
#include "plugins.h"
#include "filesMan.h"
#include "ApplBase.h"
#include "utils.h"
#include "IRCChannels.h"
#include "Shortcuts.h"
#include "Floaters.h"
#include "MenuCommands.h"
#include "CMenus.h"
#include "CommandsMenu.h"
#include "IRCInput.h"

static void ServiceGetTypesHandler(EventRef theEvent, MWPtr mw);
static void ServiceCopyHandler(EventRef theEvent, MWPtr mw);
static void ServicePasteHandler(EventRef theEvent, MWPtr mw);

static void DoAbout(void);

static void cascade2(WindowPtr w);
static void DoCascadeWindows(void);
static void DoTileWindows(void);
static void DoCycleCommand(char next);

enum {
	kSIRCBundleNameID = 130,
	kSIRCVersionInfoID = 132
};

/*
* Carbon Event Handlers
*/


static const OSType appServiceDataTypes[] =
{
    'TEXT'
};

static void ServiceGetTypesHandler(EventRef theEvent, MWPtr mw)
{
	CFMutableArrayRef copyTypes, pasteTypes;
	short index, count;
	SInt32 selStart, selEnd;
	Boolean textSelection = FALSE;
	
	GetEventParameter (theEvent, kEventParamServiceCopyTypes, typeCFMutableArrayRef, NULL, sizeof (CFMutableArrayRef), NULL, &copyTypes);
	GetEventParameter (theEvent, kEventParamServicePasteTypes, typeCFMutableArrayRef, NULL, sizeof (CFMutableArrayRef), NULL, &pasteTypes);
	
	WEGetSelection(&selStart, &selEnd, mw->we);
	if (selStart != selEnd) textSelection = TRUE;
	
	count = sizeof (appServiceDataTypes) / sizeof (OSType);
	for ( index = 0; index < count; index++ ) 
	{
		CFStringRef type = 
			    CreateTypeStringWithOSType (appServiceDataTypes[index]);
		if (type)
		{
			if (textSelection) CFArrayAppendValue (copyTypes, type);
			CFArrayAppendValue (pasteTypes, type);
			CFRelease (type);
		}
	}
}

static void ServiceCopyHandler(EventRef theEvent, MWPtr mw)
{
	SInt32 selStart, selEnd;
	
	WEGetSelection(&selStart, &selEnd, mw->we);
	
	if (selStart != selEnd) {
		ScrapRef    currentScrap, specificScrap;
		short       count, index;
		
		WECopy (mw->we);
		
		GetCurrentScrap (&currentScrap);
		GetEventParameter (theEvent, kEventParamScrapRef, typeScrapRef, NULL, sizeof(ScrapRef), NULL, &specificScrap);
		
		count = sizeof (appServiceDataTypes) / sizeof (OSType);
		for (index = 0; index <count; index++) {
			Size        byteCount;
			OSStatus    err;
			
			err = GetScrapFlavorSize(currentScrap, appServiceDataTypes[index], &byteCount); 
			if (err == noErr) {
				void*   buffer;
				
				buffer = malloc (byteCount);
				
				if (buffer != NULL) {
					err = GetScrapFlavorData(currentScrap, appServiceDataTypes[index], &byteCount, buffer);
					if (err == noErr)
					    PutScrapFlavor(specificScrap, appServiceDataTypes[index], 0, byteCount, buffer);
					
					free (buffer);
				}
			}
		}
	}
}

static void ServicePasteHandler(EventRef theEvent, MWPtr mw)
{
	ScrapRef   specificScrap, currentScrap;
	CFIndex     index, count;
	
	GetEventParameter (theEvent, kEventParamScrapRef, typeScrapRef, NULL, sizeof (ScrapRef), NULL, &specificScrap);
	
	ClearCurrentScrap ();
	GetCurrentScrap (&currentScrap);
	count = sizeof (appServiceDataTypes) / sizeof (OSType);
	
	for (index = 0; index <count; index++)
	{
		Size        byteCount;
		OSStatus    err;
		
		err = GetScrapFlavorSize(specificScrap, appServiceDataTypes[index], &byteCount);

		if (err == noErr) 
		{
			void*   buffer = malloc(byteCount);
			
			if (buffer != NULL) 
			{
				err = GetScrapFlavorData(specificScrap, appServiceDataTypes[index], &byteCount, buffer);
				
				if (err == noErr) 
					PutScrapFlavor(currentScrap, appServiceDataTypes[index], 0, byteCount, buffer);

				free (buffer);
			}
		}
	}
	
	WEPaste(mw->we);
	processPaste(mw, FALSE);
}

static const EventTypeSpec mwCommandEventTypes[] = {
		{kEventClassService, kEventServiceGetTypes},
		{kEventClassService, kEventServiceCopy}, 
		{kEventClassService, kEventServicePaste}, 
		{kEventClassCommand, kEventCommandUpdateStatus},
		{kEventClassCommand, kEventProcessCommand}
};

static OSStatus MWUICommandHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData)
{
#pragma unused(nextHandler)
	OSStatus result = eventNotHandledErr;
	HICommand hiCommand;
	UInt32 eventClass, eventKind;
	MWPtr mw = (MWPtr)userData;
	
	eventClass = GetEventClass(theEvent);
	eventKind = GetEventKind(theEvent);
	
	switch(eventClass)
	{
		case kEventClassService:
		{
			switch(eventKind)
			{
				case kEventServiceGetTypes:
					ServiceGetTypesHandler(theEvent, mw);
					result = noErr;
					break;
				
				case kEventServiceCopy:
					ServiceCopyHandler(theEvent, mw);
					result = noErr;
					break;
				
				case kEventServicePaste:
					ServicePasteHandler(theEvent, mw);
					result = noErr;
					break;
			}
			break;
		}
			
		case kEventClassCommand:
		{
			GetEventParameter(theEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(hiCommand), NULL, &hiCommand);
			
			switch(eventKind)
			{
				case kEventCommandUpdateStatus:
					switch(hiCommand.commandID)
					{
						case kHICommandUndo:
						case kHICommandCut:
							DisableMenuCommand(NULL, hiCommand.commandID);
							break;
						
						case kHICommandCopy:
						{
							SInt32 selStart, selEnd;
							WEGetSelection(&selStart, &selEnd, mw->we);
							if (selStart != selEnd)
							{
								EnableMenuCommand(NULL, hiCommand.commandID);
								result = noErr;
							}
							else
								DisableMenuCommand(NULL, hiCommand.commandID);
							break;
						}
							
						case kHICommandPaste:
						{
							ScrapRef scrap;
							ScrapFlavorFlags scrapFlags;
							
							GetCurrentScrap(&scrap);
							result = GetScrapFlavorFlags(scrap, kScrapFlavorTypeText, &scrapFlags);
							if (result == noErr)
								EnableMenuCommand(NULL, hiCommand.commandID);
							else
								DisableMenuCommand(NULL, hiCommand.commandID);
							break;
						}
						
						case kHICommandClear:
						case kHICommandSelectAll:
						case 'FIND':
						case 'FAGN':
							EnableMenuCommand(NULL, hiCommand.commandID);
							return noErr;
					}
					break;

				case kEventProcessCommand:
					switch(hiCommand.commandID)
					{
						//Edit Menu
						case kHICommandUndo:
						case kHICommandCut:
						case kHICommandCopy:
						case kHICommandPaste:
						case kHICommandClear:
						case kHICommandSelectAll:
							HitEditMenu(mw, hiCommand.menu.menuItemIndex);
							return noErr;
						
						case 'FIND':
							DoFind(mw, false);
							return noErr;
						
						case 'FAGN':
							DoFind(mw, true);
							return noErr;
						
						//Font Menu
						case 'FONT':
							HitFontsMenu(mw, hiCommand.menu.menuItemIndex);
							return noErr;
						
						case 'FTSZ':
							DoFontSizeWindow(mw);
							return noErr;
					}
					break;
			}
		}
	}
	
	return result;
}

static OSStatus MWConsoleEventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
#pragma unused(handlerCallRef, userData)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassWindow:
		{
			switch(eventKind)
			{
				case kEventWindowShown:
				case kEventWindowHidden:
					CheckMenuItem(gWindowMenu, wConsoleItem, eventKind == kEventWindowShown);
					break;
			}
		}
		break;
	}
	
	return result;
}

static const EventTypeSpec mwEventTypes[] = {
		{kEventClassWindow, kEventWindowActivated},
		{kEventClassWindow, kEventWindowDeactivated},
		{kEventClassWindow, kEventWindowDrawContent},
		{kEventClassWindow, kEventWindowGetMinimumSize},
		{kEventClassWindow, kEventWindowBoundsChanged},
		{kEventClassWindow, kEventWindowClose},
		{kEventClassWindow, kEventWindowHandleContentClick},
};

static OSStatus MWEventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
#pragma unused(handlerCallRef)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	MWPtr mw = (MWPtr)userData;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassWindow:
		{
			switch(eventKind)
			{
				case kEventWindowActivated:
				case kEventWindowDeactivated:
				{
					char activate = eventKind == kEventWindowActivated;
					
					MWPaneActivate(mw, activate);
					
					if(activate)
					{
						SetTarget(mw, &CurrentTarget);
						UpdateStatusLine();
						DrawMWinStatus(consoleWin);
						
						IADActivate(ILGetInputDataFromMW(mw), true);
					}
					break;
				}
					
				case kEventWindowDrawContent:
					MWPaneUpdate(mw);
					break;
				
				case kEventWindowGetMinimumSize:
				{
					const Point minSize = {60, 125};
					
					SetEventParameter(event, kEventParamDimensions, typeQDPoint, sizeof(Point), &minSize);
					result = noErr;
					break;
				}
				case kEventWindowBoundsChanged:
				{
					Rect growRect;
					UInt32 attrib;
					
					GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &growRect);
					GetEventParameter(event, kEventParamAttributes, typeUInt32, NULL, sizeof(UInt32), NULL, &attrib);
					
					if(attrib & kWindowBoundsChangeSizeChanged)
					{
						MWPaneRecalculate(mw);
						MWRecalculateRects(mw);
						
						MWPaneResize(mw);
						MWReposition(mw);
						result = noErr;
					}
					else if(attrib & kWindowBoundsChangeOriginChanged)
					{
						MWReposition(mw);
						result = noErr;
					}
					break;
				}
				
				case kEventWindowClose:
					MWPart(mw);
					result = noErr;
					break;
				
				case kEventWindowHandleContentClick:
					if(!CMClick(mw->w, event))
						MWPaneClick(mw, event);
					break;
			}
			break;
		}
	}
	
	return result;
}

static const EventTypeSpec mwTextEventTypes[] = {
	{kEventClassTextInput, kEventUnicodeForKeyEvent},
};

static OSStatus MWTextEventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
#pragma unused(handlerCallRef)
	MWPtr mw = userData;
	
	return IADHandleTextEvent(ILGetInputDataFromMW(mw), event);
}

void MWInstallEventHandlers(MWPtr mw)
{
	static EventHandlerUPP uiCommandHandler = NULL;
	static EventHandlerUPP mwEventHandler = NULL;
	static EventHandlerUPP mwTextEventHandler = NULL;
	
	if(!uiCommandHandler)
	{
		uiCommandHandler = NewEventHandlerUPP(MWUICommandHandler);
		mwEventHandler = NewEventHandlerUPP(MWEventHandler);
		mwTextEventHandler = NewEventHandlerUPP(MWTextEventHandler);
	}
	
	InstallWindowEventHandler(mw->w, uiCommandHandler, GetEventTypeCount(mwCommandEventTypes), mwCommandEventTypes, mw, NULL);
	InstallWindowEventHandler(mw->w, mwEventHandler, GetEventTypeCount(mwEventTypes), mwEventTypes, mw, NULL);
	InstallWindowEventHandler(mw->w, mwTextEventHandler, GetEventTypeCount(mwTextEventTypes), mwTextEventTypes, mw, NULL);
	
	if(mw->winType == conWin)
	{
		static EventHandlerUPP conWinHandler = NULL;
		const EventTypeSpec conEvents[] = {
			{kEventClassWindow, kEventWindowShown},
			{kEventClassWindow, kEventWindowHidden},
		};
		
		if(!conWinHandler)
			conWinHandler = NewEventHandlerUPP(MWConsoleEventHandler);
		
		InstallWindowEventHandler(mw->w, conWinHandler, GetEventTypeCount(conEvents), conEvents, mw, NULL);
	}
	
	MWInitDrag(mw);
}

#pragma mark -

static const EventTypeSpec awEventTypes[] = {
	{ kEventClassControl, kEventControlHit }
};

static OSStatus AboutWindowEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
#pragma unused(myHandler)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	WindowRef aboutWindow;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	aboutWindow = (WindowRef)userData;
	
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
					QuitAppModalLoopForWindow(aboutWindow);
					HideWindow(aboutWindow);
					DisposeWindow(aboutWindow);
					result = noErr;
					break;
			}
			break;
		}
	}
	
	return result;
}
 
static void DoAbout(void)
{
	IBNibRef mainNibRef;
	WindowRef aboutWindow = NULL;
	static EventHandlerUPP awUPP = NULL;
	OSStatus status;
	
	if(!awUPP)
		awUPP = NewEventHandlerUPP(AboutWindowEventHandler);
	
	if((CreateNibReference(CFSTR("main"), &mainNibRef) == noErr) && (CreateWindowFromNib(mainNibRef, CFSTR("AboutWindow"), &aboutWindow) == noErr))
	{
		CFBundleRef appBundle;
		CFStringRef text;
		ControlID bundleNameID = { kApplicationSignature, kSIRCBundleNameID };
		ControlID versionInfoID = { kApplicationSignature, kSIRCVersionInfoID };
		ControlRef theControl = NULL;
		ControlFontStyleRec controlStyle;
		ControlButtonContentInfo content;
		Rect boundsRect;
		
		DisposeNibReference(mainNibRef);
		
		appBundle = CFBundleGetMainBundle();
		
		text = (CFStringRef) CFBundleGetValueForInfoDictionaryKey(appBundle, CFSTR("CFBundleName"));
		GetControlByID(aboutWindow, &bundleNameID, &theControl);
		SetControlData(theControl, kControlLabelPart, kControlStaticTextCFStringTag, sizeof(CFStringRef), &text);
		controlStyle.flags = kControlUseJustMask | kControlUseSizeMask;
		controlStyle.just = teCenter;
		controlStyle.size = 24;
		SetControlFontStyle(theControl, &controlStyle);
		
		text = (CFStringRef) CFBundleGetValueForInfoDictionaryKey(appBundle, CFSTR("CFBundleGetInfoString"));
		GetControlByID(aboutWindow, &versionInfoID, &theControl);
		SetControlData(theControl, kControlLabelPart, kControlStaticTextCFStringTag, sizeof(CFStringRef), &text);
		controlStyle.size = 12;
		SetControlFontStyle(theControl, &controlStyle);

		content.contentType = kControlContentPictHandle;
		content.u.picture = LoadAppLogoFromResources();
		
		SetPortWindowPort(aboutWindow);
		
		SetRect(&boundsRect, 0, 0, 360, 144);
		CreatePictureControl(aboutWindow, &boundsRect, &content, TRUE, &theControl);
		
		status = InstallWindowEventHandler(aboutWindow, awUPP, GetEventTypeCount(awEventTypes), awEventTypes, (void *)aboutWindow, NULL);
		
		ShowWindow(aboutWindow);
		SelectWindow(aboutWindow);
		
		status = RunAppModalLoopForWindow(aboutWindow);
		
		DisposeHandle((Handle)content.u.picture);
		if(theControl)
			DisposeControl(theControl);
	}
 }
 
#pragma mark -

static void DoCycleCommand(char next)
{
	MWPtr activeW, mw;
	
	if(!ActiveNonFloatingWindow()) //do nothing if no front window.
		return;
	
	activeW = GetActiveMW();
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
		while(!IsWindowVisible(mw->w))
		{
			mw=mw->prev;
			
			if(mw==activeW)
				break;
			else if(!mw)
				mw=consoleWin;
		}
		
		WSelect(mw->w);
	}
	else //exact same as above, except prev instead of next
	{
		if(!activeW->next)
			mw = mwl;
		else
			mw=activeW->next;
		
		//window to activate is in mw, and w is correct
		while(!IsWindowVisible(mw->w))
		{
			mw=mw->next;
			
			if(mw==activeW)
				break;
			else if(!mw)
				mw = mwl;
		}
		
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
		MWReposition(mw);
		MWNewPosition(&r); //twice to give some more space
	}
}

static void DoCascadeWindows(void)
{
	WindowPtr w;
	
	SetRect(&cornerstone, 5, GetMBarHeight() + 16, 425, 340);
	w=ActiveNonFloatingWindow();
	
	if(w)
		cascade2(w);
}

static void DoTileWindows(void)
{
	WindowPtr w = ActiveNonFloatingWindow();
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
		if(IsWindowVisible(w) && MWFromWindow(w))
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
			if(IsWindowVisible(w))
			{
				MWPtr mw = MWFromWindow(w);
				if(mw)
				{
					MWSetDimen(mw, r.left, r.top, r.right-r.left, r.bottom-r.top);
					MWReposition(mw);
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
#pragma unused(nextHandler, userData)
	OSStatus myErr = eventNotHandledErr;
	UInt32 eventKind;
	HICommand hiCommand;
	
	GetEventParameter(theEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(hiCommand), NULL, &hiCommand);
	
	eventKind = GetEventKind(theEvent);
	
	switch(eventKind)
	{
		case kEventCommandUpdateStatus:
			switch(hiCommand.commandID)
			{
				//Edit Menu
				case kHICommandUndo:
				case kHICommandCut:
				case kHICommandCopy:
				case kHICommandPaste:
				case kHICommandClear:
				case kHICommandSelectAll:
				
				//Font Menu
				case 'FIND':
				case 'FAGN':
					DisableMenuCommand(NULL, hiCommand.commandID);
					return noErr;
			}
			break;
		
		case kEventProcessCommand:
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
				case kHICommandSave:
					writeAllFiles();
					return noErr;
				
				case 'CONO':
					HitConnectionListMenu(hiCommand.menu.menuItemIndex);
					return noErr;
				
				case 'CONC':
					HitSignoffConnectionListMenu(hiCommand.menu.menuItemIndex);
					return noErr;
				
				case 'CONX':
					HitSelectConnectionMenu(hiCommand.menu.menuItemIndex);
					return noErr;
				
				//Command Menu
				case 'COMD':
					HitCommandsMenu(hiCommand.menu.menuItemIndex);
					return noErr;
				
				//Window Menu
				case 'CONS':
					ToggleConsoleWindow();
					return noErr;
				
				case kCommandPrevWin:
					DoCycleCommand(false);
					return noErr;
					
				case kCommandNextWin:
					DoCycleCommand(true);
					return noErr;
					
				case 'WCAS':
					DoCascadeWindows();
					return noErr;
				
				case kCommandWindowService:
					HitWindowPluginServiceMenu(hiCommand.menu.menuItemIndex);
					return noErr;
				
				case kCommandWindowSelect:
					HitWindowSelectWindowMenu(hiCommand.menu.menuItemIndex);
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
				
				//Apple URL Menu
				case 'AURL':
					HitApplicationURLMenu(hiCommand.menu.menuItemIndex);
					return noErr;
			}
			break;
	}
	
	return myErr;
}

void InitEventHandlers(void)
{
	const EventTypeSpec commandType[] = {
		{kEventClassCommand, kEventCommandUpdateStatus},
		{kEventClassCommand, kEventProcessCommand},
	};
	
	InstallApplicationEventHandler(NewEventHandlerUPP(DoCommandEvent), 2, commandType, NULL, NULL);
}
