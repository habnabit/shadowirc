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

pascal void WSelect(WindowPtr w)
{
	ShowWindow(w);
	SelectWindow(w);
}

WindowPtr WCreate(const Rect *boundsRect, ConstStr255Param title, WindowAttributes windowAttrs, long refCon, char isFloater)
{
	WindowPtr w = 0;
	UInt32 wClass;
	
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