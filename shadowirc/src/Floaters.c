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

#define USE_FLOATERS (0)
//#define USE_FLOATERS (hasFloatingWindows)

#include "Floaters.h"
#include "IRCGlobals.h"

extern char inBackground;
extern char hasAppearance;

static pascal WindowPtr LastFloater(void);
static pascal void WActivate(WindowPtr w);
static pascal void WDeactivate(WindowPtr w);
static pascal void ShowAllFloaters(void);
static pascal void HideAllFloaters(void);

WindowActivateProcPtr ActivateWindowProcPtr;

enum {
	kFloaterKind = 7
};

pascal char WIsFloater(WindowPtr w)
{
	if(USE_FLOATERS)
	{
		WindowClass wclass;
		
		GetWindowClass(w, &wclass);
		return wclass == kFloatingWindowClass;
	}
	else
		return GetWindowKind(w)==kFloaterKind;
}

//Not called if has 8.6
static pascal WindowPtr LastFloater(void)
{
	WindowPtr w = GetWindowList();
	WindowPtr prevW=0;
	
	while(w && WIsFloater(w))
	{
		prevW=w;
		w=GetNextWindow(w);
	}
	
	//when we get here, prevW is the previous window, which was a floater, and w is the next window, wich ISN'T a floater
	if(prevW && WIsFloater(prevW))
		return prevW;
	else
		return 0;
}

pascal WindowPtr FrontNonFloater(void)
{
	if(USE_FLOATERS)
		return FrontNonFloatingWindow();
	else
	{
		WindowPtr w=FrontWindow();
		
		while(w && WIsFloater(w))
			w=GetNextWindow(w);
		
		//W is now the frontmost non-floating window.
		while(w && !IsVisible(w))
			w=GetNextWindow(w);	//obviously, it can't be a front window if it's not visible!

		//when we get here, w isn't a floater, and it's visible, so it's the front non floater.
		
		return w;
	}
}

pascal void WMoveToFront(WindowPtr w)
{
	if(USE_FLOATERS)
		BringToFront(w);
	else
	{
		if(WIsFloater(w))
			BringToFront(w);
		else
		{
			WindowPtr lf = LastFloater();
			//If there aren't any floating windows, we have to move it to the front, instead of behind the floater
			//But...if we have a modal dialog, it stays BEHIND that.
			
			if(lf)
			{
				while(IsDialog(lf))
				{
					WindowPtr w2 = GetNextWindow(lf);
					if(IsDialog(w2))
						lf = w2;
					else
						break;
				}
				
				SendBehind(w, lf);
			}
			else
				BringToFront(w);
		}
	}
}

//Not called if has 8.6
static pascal void WActivate(WindowPtr w)
{
	if(!inBackground)
	{
		HiliteWindow(w, 1);
		ActivateWindowProcPtr(w, 1);
	}
}

//Not called if has 8.6
static pascal void WDeactivate(WindowPtr w)
{
	if(IsActive(w))
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
		fnf=FrontNonFloater();
		
		if(fnf && IsActive(fnf))
			WDeactivate(fnf);
		
		HideAllFloaters();
	}
}

pascal void ExitModalDialog(void)
{
	if(!inBackground)
	{
		WindowPtr fnf;
		
		fnf = FrontNonFloater();

		//Directly do WActivate stuff since window is going to hilite itself...		
		if(fnf)
		{
			HiliteWindow(fnf, 1);
			ActivateWindowProcPtr(fnf, 1);
		}
		
		ShowAllFloaters();
	}
	else
	{
		WResume();
		inBackground=false;
	}
	
	ZeroScrap();
	TEToScrap();
}

pascal void WSelect(WindowPtr w)
{
	if(USE_FLOATERS)
		SelectWindow(w);
	else
	{
		WindowPtr fw = FrontNonFloater();
		
		if(!WIsFloater(w))
		{
			char isD = IsDialog(fw);

			if(!isD)
				WMoveToFront(w);
			if(!IsVisible(w))
				ShowHide(w, 1);

			if(!isD)
			{
				if(fw && IsActive(fw))
					WDeactivate(fw);
									
				WActivate(w);
			}
		}
		else //it's a floater, so just move it to the front.
		{
			WMoveToFront(w);
			if(!IsVisible(w))
				WShow(w);
		}
	}
}

pascal void WShow(WindowPtr w)
{
	WindowPtr fnf;
	char isD;
	
	if(USE_FLOATERS)
		ShowWindow(w);
	else
		if(!IsVisible(w)) //If the window is not visible, do something.
		{
			fnf = FrontNonFloater();
			isD = IsDialog(fnf);
			
			ShowHide(w, 1); //Ok, let's show it.
			if(WIsFloater(w)) //if it's a floater, activate it and throw it to the front
			{
				BringToFront(w);
				HiliteWindow(w, 1);
			}
			else 	//it's a regular window. If the window is frontmost, select it.
			{
				if(fnf==w && !isD)
				{
					if(!IsActive(w))
						WSelect(w);
					//Now, We need to deactivate the previous front window, since we bumped them out..
					
					fnf=GetNextWindow(w);
					WDeactivate(fnf);
				}
				
				//else it's not frontmost, so just show it, which we've already done
			}
		}
}

pascal void WHide(WindowPtr w)
{
	WindowPtr fnf;
	
	if(USE_FLOATERS)
		HideWindow(w);
	else
		if(IsVisible(w))
		{
			ShowHide(w, 0);
			SendBehind(w, 0);
			WDeactivate(w);
			fnf=FrontNonFloater();
			if(fnf && (fnf != w)) //If there's a front non floater and it's not the window we just hid, activate it
				WActivate(fnf);
		}
}

static void SAF2(WindowPtr w)
{
	if(w && WIsFloater(w))
	{
		SAF2(GetNextWindow(w));
		BringToFront(w);
		HiliteWindow(w, 1);
		ShowHide(w, 1);
	}
}

static pascal void ShowAllFloaters(void)
{
	if(USE_FLOATERS)
		ShowFloatingWindows();
	else
		SAF2(GetWindowList());
}

static pascal void HideAllFloaters(void)
{
	WindowPtr w;
	
	if(USE_FLOATERS)
		HideFloatingWindows();
	else
	{
		w=FrontWindow();
		while(w)
		{
			if(WIsFloater(w))
				ShowHide(w, 0);
			w=GetNextWindow(w);
		}
	}
}

pascal void WSuspend(void) //hide all floaters and deactivate first non-floater
{
	WindowPtr fnf;
	
	HideAllFloaters();
	fnf=FrontNonFloater();

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
	
	ShowAllFloaters();
	fnf = FrontNonFloater();
	if(fnf)
	{
		//This is WActivate() stuff. Moved here so window does get redrawn,
		//even though WActivate() prevents the window from being updated...
		HiliteWindow(fnf, 1);
		ActivateWindowProcPtr(fnf, 1);
	}
}

//Set the ClipRgn to the ClipRgn minus the structRgns of all the windows in front of w.
static pascal void ClipPortAbove(GrafPtr gp, WindowPtr win)
{
	WindowPtr w = GetWindowList();
	RgnHandle clip = NewRgn();
	RgnHandle struc = NewRgn();
	
	GetPortClipRegion(gp, clip);
	while(w  && w != win)
	{
		#if TARGET_CARBON //or, appearance11
			GetWindowRegion(w, kWindowStructureRgn, struc);
		#else
			GetWindowStructureRgn(w, struc);
		#endif
		
		DiffRgn(clip, struc, clip);
		w = GetNextWindow(w);
	}
	
	SetPortClipRegion(gp, clip);
	DisposeRgn(clip);
	DisposeRgn(struc);
}

pascal void WDrag(WindowPtr w, Point startPoint, const Rect *boundsRect)
{
	Rect dragRect;
	KeyMap	keyMap;
	GrafPtr gp;
	RgnHandle dragRegion;
	unsigned long dragResult;
	short topLimit, newH, newV, offH, offV;
	char commandKeyDown;
	GrafPtr screenPort;
	RgnHandle gray;
	
	if(USE_FLOATERS)
	{
		DragWindow(w, startPoint, boundsRect);
		return;
	}
	
	if(WaitMouseUp())
	{
		//Check for command key
		GetKeys(keyMap);
		commandKeyDown = (keyMap[1] & 0x8000) != 0;
		
		if(!commandKeyDown)
			if(GetWindowKind(w)!=kFloaterKind && w != FrontNonFloater()) //bring forward now
				WSelect(w);
		
		// Set the top of the dragRect to be below the menubar
		topLimit=GetMBarHeight()+4;
		dragRect=*boundsRect;
		if(dragRect.top<topLimit)
			dragRect.top=topLimit;
		
		GetPort(&gp);
		
		screenPort = CreateNewPort();
		OpenPort(screenPort);
		SetPort(screenPort);
		gray = GetGrayRgn();
		SetPortBounds(screenPort, GetRegionBounds(gray, 0));
		SetPortVisibleRegion(screenPort, gray);
		SetClip(gray);
		
		if((commandKeyDown) || (GetWindowKind(w) != kFloaterKind))
		{
			if(!commandKeyDown) // If there are floating windows, clip the dragging outline to draw behind the floaters.
				ClipPortAbove(screenPort, FrontNonFloater());
			else // If the command key was down, clip the outline to draw behind any windows above the window being dragged.
				ClipPortAbove(screenPort, w);
		}
		
		// Create a region to drag
		dragRegion=NewRgn();
		GetWindowRegion(w, kWindowStructureRgn, dragRegion);
		
/*
		if(hasAppearance)
		{
			RgnHandle tempRgn = NewRgn();
			CopyRgn(dragRegion, tempRgn);
			InsetRgn(tempRgn, 2, 2);
			DiffRgn(dragRegion, tempRgn, dragRegion);
			DisposeRgn(tempRgn);
		}
*/
		
		// Drag the window around
		dragResult= DragGrayRgn(dragRegion, startPoint, &dragRect, &dragRect, noConstraint, 0);
		
		//Restore grafport
		ClosePort(screenPort);
		SetPort(gp);
		DisposePort(screenPort);
		
		if(dragResult)
		{
			offH=dragResult & 0xFFFF;
			offV=dragResult >> 16;
			
			//only move if it stayed inside the dragging box
			if(offV != -32768)
			{
				Rect r;
				GrafPtr gpw;
				
				GetPort(&gp);
				gpw = (GrafPtr)GetWindowPort(w);
				SetPort(gpw);
				GetPortBounds((CGrafPtr)gpw, &r);
				LocalToGlobal((Point*)&r);
				SetPort(gp);
				
				newH = r.left + offH;
				newV = r.top + offV;

				MoveWindow(w, newH, newV, false);
			}
		}
		
		DisposeRgn(dragRegion);
	}
}

pascal void WMove(WindowPtr w, short h, short v, char front)
{
	if(USE_FLOATERS)
	{
		MoveWindow(w, h, v, front);
	}
	else
	{
		MoveWindow(w, h, v, false);
		if(front)
			WMoveToFront(w);
	}
}

pascal WindowPtr WCreate(const Rect *boundsRect, ConstStr255Param title, short theProc, char goAwayFlag, long refCon, char isFloater)
{
	WindowPtr w = 0;
	
	if(USE_FLOATERS && isFloater)
	{
		UInt32 wClass;
		
		if(isFloater)
			wClass = kFloatingWindowClass;
		else
			wClass = kDocumentWindowClass;
		
		CreateNewWindow(wClass, kWindowStandardFloatingAttributes | kWindowResizableAttribute, boundsRect, &w);

		if(w)
		{
			SetWTitle(w, title);
			SetWRefCon(w, refCon);
		}
	}
	else
	{
		//All windows are careated at the back of the window list and invisible.
		w=NewCWindow(0, boundsRect, title, false, theProc, 0, goAwayFlag, refCon);

		if(isFloater)
			SetWindowKind(w, kFloaterKind);
	}

	return w;	
}