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

#include <MacHeaders.h>

/*
	Note: these functions (and their use) are based around my believe of how
	the functions are supposed to work.
	
	In all likelyhood, I'm wrong about something here.
*/

#define CreateNewPort() ((GrafPtr)NewPtr(sizeof(GrafPort)))
#define DisposePort(gp) DisposePtr((Ptr)gp)
inline Rect* GetRegionBounds(RgnHandle region, Rect* bounds)
{
	Rect* rp = &(**region).rgnBBox;
	if(bounds)
		*bounds = *rp;
	return rp;
}

inline Rect* GetPortBounds(CGrafPtr port, Rect* bounds)
{
	Rect* rp = &port->portRect;
	if(bounds)
		*bounds = *rp;
	return rp;
}

#define EnableMenuItem EnableItem
#define DisableMenuItem DisableItem

#define GetPortTextFont(gp) ((gp)->txFont)
#define GetPortTextSize(gp) ((gp)->txSize)
#define GetPortTextFace(gp) ((gp)->txFace)
#define GetPortTextMode(gp) ((gp)->txMode)
#define GetPortPixMap(gp) ((gp)->portPixMap)
#define GetPortVisibleRegion(gp, rgn) CopyRgn(gp->visRgn, rgn)
#define GetPortClipRegion(gp, clip) CopyRgn(gp->clipRgn, clip)

#define SetPortClipRegion(gp, clip) CopyRgn(clip, gp->clipRgn)
#define SetPortVisibleRegion(gp, rgn) CopyRgn((rgn), (gp)->visRgn)
#define SetPortBounds(gp, rectP) ((gp)->portRect = *rectP)

#define IsPortColor( port ) (((port)->portBits.rowBytes & 0xC000) == 0xC000)

#define GetDialogPort(d) ((GrafPtr)d)

#define GetWindowFromPort(gp) ((WindowPtr)gp)
#define GetDialogFromWindow(w) ((DialogPtr)w)
#define GetDialogTextEditHandle(d) (((DialogPeek)d)->textH)
#define SetPortDialogPort(dlg) SetPort(dlg)

#define GetControlHilite(c) ((**c).contrlHilite)
#define GetControlOwner(c) ((**c).contrlOwner)
#define GetControlDataHandle(c) ((**c).contrlData)

#define GetWindowList() LMGetWindowList()

#include "CarbonLibTransitional.h"