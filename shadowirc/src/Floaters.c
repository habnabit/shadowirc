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

#include "Floaters.h"
#include "IRCGlobals.h"

extern char inBackground;

WindowActivateProcPtr ActivateWindowProcPtr;

Rect *WGetBBox(WindowPtr w, Rect *rr)
{
    RgnHandle r = NewRgn();
    GetWindowRegion(w, kWindowContentRgn, r);
    GetRegionBounds(r, rr);
    DisposeRgn(r);
    return rr;
}

pascal char WIsFloater(WindowPtr w)
{
	WindowClass wclass;
	
	GetWindowClass(w, &wclass);
	return wclass == kFloatingWindowClass;
}

//Not called if has 8.6
static pascal void WDeactivate(WindowPtr w)
{
	if(IsWindowHilited(w))
	{
		HiliteWindow(w, 0);
		ActivateWindowProcPtr(w, 0);
	}
}

pascal void EnterModalDialog(void)
{
	WindowPtr fnf;
	
	TEFromScrap();
	if(!inBackground)
	{
		fnf=ActiveNonFloatingWindow();
		
		if(fnf && IsWindowHilited(fnf))
			WDeactivate(fnf);
		
		HideFloatingWindows();
	}
}

pascal void ExitModalDialog(void)
{
	if(!inBackground)
	{
		WindowPtr fnf;
		
		fnf = ActiveNonFloatingWindow();

		//Directly do WActivate stuff since window is going to hilite itself...		
		if(fnf)
		{
			HiliteWindow(fnf, 1);
			ActivateWindowProcPtr(fnf, 1);
		}
		
		ShowFloatingWindows();
	}
	else
	{
		WResume();
		inBackground=false;
	}
	
	ClearCurrentScrap();
	TEToScrap();
}

pascal void WSelect(WindowPtr w)
{
	ShowWindow(w);
	SelectWindow(w);
}

pascal void WSuspend(void) //hide all floaters and deactivate first non-floater
{
	WindowPtr fnf;
	
	HideFloatingWindows();
	fnf=ActiveNonFloatingWindow();

/*	This is expanded here rather than calling WDeactivate() to ensure that it gets called, even though the window
		may already be deactivated.
*/
 	if(fnf)
	{
		HiliteWindow(fnf, 0);
		ActivateWindowProcPtr(fnf, 0);
	}
}

pascal void WResume(void)
{
	WindowPtr fnf;
	
	ShowFloatingWindows();
	fnf = ActiveNonFloatingWindow();
	if(fnf)
	{
		//This is WActivate() stuff. Moved here so window does get redrawn,
		//even though WActivate() prevents the window from being updated...
		HiliteWindow(fnf, 1);
		ActivateWindowProcPtr(fnf, 1);
	}
}

pascal WindowPtr WCreate(const Rect *boundsRect, ConstStr255Param title, short theProc, char goAwayFlag, long refCon, char isFloater)
{
	WindowPtr w = 0;
	UInt32 wClass;
	WindowAttributes windowAttrs = kWindowResizableAttribute;
	
	if(isFloater)
	{
		wClass = kFloatingWindowClass;
		windowAttrs |= kWindowStandardFloatingAttributes;
	}
	else
	{
		wClass = kDocumentWindowClass;
		windowAttrs |= kWindowStandardDocumentAttributes;
	}
	
	CreateNewWindow(wClass, windowAttrs, boundsRect, &w);

	if(w)
	{
		SetWTitle(w, title);
		SetWRefCon(w, refCon);
	}
	
	return w;	
}