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

#define MSGWINDOWS

#include <Carbon/Carbon.h>

#include "WASTE.h"

#include "IRCGlobals.h"
#include "LongStrings.h"
#include "Floaters.h"
#include "utils.h"
#include "MsgWindows.h"
#include "IRCChannels.h"
#include "plugins.h"
#include "MWPanes.h"
#include "AppearanceHelp.h"
#include "inline.h"
#include "TextManip.h"

static pascal void MWDestroyWidget2(mwWidgetPtr w);
inline mwPanePtr MWInsertPaneAfter(MWPtr mw, short align);
static pascal void UpdateTextRegion(mwPanePtr w);
static pascal void UpdateInputRegion(mwPanePtr w);
static pascal void DrawInputPane(mwPanePtr o);

static pascal void UpdateInputRegion(mwPanePtr w)
{
	MWPtr mw = w->mw;
	LongRect lr;
	
	EraseRect(&w->drawArea);
	
	lr.top = w->drawArea.top + 10;
	lr.left = w->drawArea.left + 6;
	lr.bottom = w->drawArea.bottom - 6;
	lr.right = w->drawArea.right-6;
	
	WESetDestRect(&lr, mw->il);
	WESetViewRect(&lr, mw->il);
	
	WECalText(mw->il);
	WESelView(mw->il);
	WEUpdate(0, mw->il);
}

static pascal void MWInputPaneClick(mwPanePtr o, const EventRecord *e)
{
	extern char iwFront;
	
	MWPtr mw = o->mw;
	
	iwFront = true;
	if(e->where.v < o->drawArea.top + 8) //grow bar
	{
		unsigned long dragResult;
		RgnHandle dragRgn;
		Rect r = o->drawArea;
		
		r.bottom = r.top + 5;
		DrawBorder(&r, kThemeStatePressed, true);
			
		r = o->drawArea;
		dragRgn = NewRgn();
		r.bottom = r.top + 5;
		RectRgn(dragRgn, &r);
		
		r.top = o->drawArea.bottom / 2;
		r.bottom = o->drawArea.bottom - 32;
		
		dragResult = DragGrayRgn(dragRgn, e->where, &r, &r, vAxisOnly, 0);
		DisposeRgn(dragRgn);
		dragResult >>= 16;
		if(dragResult != 0x8000)
		{
			o->askedHeight = o->drawArea.bottom - o->drawArea.top - dragResult;
			
			MWPaneRecalculate(o->mw);
			MWPaneResize(o->mw);
			MWPaneUpdate(o->mw);
		}

		r = o->drawArea;
		r.bottom = r.top + 5;
		DrawBorder(&r, kThemeStateActive, true);
	}
	else if(e->where.v >= o->drawArea.top + 8) //field
	{
		WEClick(e->where, e->modifiers, e->when, mw->il);
	}
}

static pascal void DrawInputPane(mwPanePtr o)
{
	Rect r;
	MWPtr mw = o->mw;
	char ia;
	RGBColor oldFront, oldBack;
	RgnHandle rgn1, rgn2;
	
	r = o->drawArea;
	r.bottom = r.top + 5;
	ia = IsWindowHilited(mw->w);

	if(ia)
		DrawBorder(&r, kThemeStateActive, true);
	else
		DrawBorder(&r, kThemeStateInactive, true);
	
	r = o->drawArea;
	r.top += 6;
	
	GetBackColor(&oldBack);
	RGBBackColor(&shadowircColors[sicWindowBG]);
	rgn1 = NewRgn();
	RectRgn(rgn1, &r);
	r.bottom -= 3;
	r.left +=3;
	r.right -=3;
	rgn2 = NewRgn();
	RectRgn(rgn2, &r);
	DiffRgn(rgn1, rgn2, rgn1);
	EraseRgn(rgn1);
	DisposeRgn(rgn1);
	DisposeRgn(rgn2);
	
	InsetRect(&r, -1, -1);
	r.top++;

	GetForeColor(&oldFront);
	
	if(ia)
		RGBForeColor(&black);
	else
		RGBForeColor(&VDkGrey);
	
	FrameRect(&r);
	RGBForeColor(&oldFront);
	
	if(ia)
	{
		WEActivate(mw->il);
		WEUpdate(0, mw->il);
	}
	else
	{
		WEUpdate(0, mw->il);
		WEDeactivate(mw->il);
	}
	RGBBackColor(&oldBack);
}

#pragma mark -

pascal char MWDestroyPane(mwPanePtr o)
{
	MWPtr mw;
	
	if(o && (!sidr.yourInfo || sidr.yourInfo == o->pluginRef))
	{
		mw = o->mw;
		
		if(o->prev)
			o->prev->next = o->next;
		if(o->next)
			o->next->prev = o->prev;
		
		DisposePtr((Ptr)o);
		MWPaneRecalculate(mw);
		
		return 1;
	}
	else
		return 0;
}

pascal void MWDestroyAllPanes(MWPtr mw)
{
	mwPanePtr o;
	mwPanePtr o2;
	GrafPtr g;
	
	GetPort(&g);
	SetPortWindowPort(mw->w);
	
	o = mw->paneList;
	while(o)
	{
		o2=o->next;
		if(o->creator == mwShadowIRCPane && !o->pluginRef)
		{
			if(o->type==mwTextPane)
			{
				if(mw->we)
					WEDispose(mw->we);
			}
			else if(o->type == mwInputPane)
			{
				if(mw->hist)
					DisposeHandle(mw->hist);
				if(mw->il)
					WEDispose(mw->il);
			}
		}
		else
		{
			pMWPaneDestroyData p;
			
			p.pane = o;
			runIndPlugin(o->pluginRef, pMWPaneDestroyMessage, &p);
		}
		DisposePtr((Ptr)o);
		o=o2;
	}
	
	mw->paneList = 0;
	SetPort(g);
}

pascal void MWPaneActivate(MWPtr mw, char activate)
{
	mwPanePtr o;
	pMWPaneActivateData p;
	GrafPtr g;
	char windowActive = IsWindowActive(mw->w);
	
	GetPort(&g);
	SetPortWindowPort(mw->w);
	
	linkfor(o, mw->paneList)
		if(o->creator == mwShadowIRCPane && !o->pluginRef)
		{
			if(o->type==mwTextPane)
			{
				if(activate && windowActive)
				{
					ActivateControl(mw->vscr);
					WEActivate(mw->we);
				}
				else
				{
					DeactivateControl(mw->vscr);
					WEDeactivate(mw->we);
				}
			}
			else if(o->type == mwWidgetsPane)
				DrawMWinStatus(mw);
			else if(o->type == mwInputPane)
				DrawInputPane(o);
		}
		else
		{
			p.pane = o;
			p.activate = activate;
			runIndPlugin(o->pluginRef, pMWPaneActivateMessage, &p);
		}
	
	SetPort(g);
}

pascal mwPanePtr MWFindPanePoint(MWPtr mw, Point p)
{
	mwPanePtr o;
	
	linkfor(o, mw->paneList)
		if(PtInRect(p, &o->drawArea))
			return o;
	
	return 0;
}

pascal void MWPaneClick(MWPtr mw, EventRecord *e)
{
	mwPanePtr o;
	Rect textRect;
	pMWPaneClickData p;
	GrafPtr g;
	
	GetPort(&g);
	SetPortWindowPort(mw->w);

	o=MWFindPanePoint(mw,e->where);
	if(o)
	{
		if(o->creator == mwShadowIRCPane && !o->pluginRef)
		{
			if(o->type == mwTextPane)
			{
				//CalcTextRect
				textRect.top = o->drawArea.top + 3;
				textRect.left = o->drawArea.left + 3;
				textRect.bottom = o->drawArea.bottom - 3;
				textRect.right = o->drawArea.right - 18;
				if(PtInRect(e->where, &textRect))
					WEClick(e->where, e->modifiers, e->when, mw->we);
			}
			else if(o->type == mwWidgetsPane)
				MWStatusClick(mw, e->where);
			else if(o->type == mwInputPane)
				MWInputPaneClick(o, e);
		}
		else
		{
			p.pane = o;
			p.e = e;
			runIndPlugin(o->pluginRef, pMWPaneClickMessage, &p);
		}
	}
	
	SetPort(g);
}

pascal void MWPaneUpdate(MWPtr mw)
{
	mwPanePtr o;
	pMWPaneUpdateData p;
	GrafPtr g;
	Rect r;
	RgnHandle rgn, rgn2;
	RGBColor rgb;
	
	GetPort(&g);
	SetPortWindowPort(mw->w);
	
	linkfor(o, mw->paneList)
		if(o->creator == mwShadowIRCPane && !o->pluginRef)
		{
			if(o->type == mwTextPane)
			{
				rgn = NewRgn();
				rgn2 = NewRgn();
				
				r = o->drawArea;
				r.right -= 15; //don't clear scroll bar area
				RectRgn(rgn, &r);
				InsetRect(&r, 2, 2);
				RectRgn(rgn2, &r);
				DiffRgn(rgn, rgn2, rgn);
				EraseRgn(rgn);
				DisposeRgn(rgn);
				DisposeRgn(rgn2);

				WEUpdate(0, mw->we);
				
				r.left = o->drawArea.left-1;
				r.right = o->drawArea.right+1;
				r.top = o->drawArea.top-1;
				r.bottom = o->drawArea.bottom+1;
				
				GetForeColor(&rgb);
				if(IsWindowHilited(mw->w))
					RGBForeColor(&black);
				else
					RGBForeColor(&VDkGrey);
				FrameRect(&r);
				RGBForeColor(&rgb);
			}
			else if(o->type == mwWidgetsPane)
				DrawMWinStatus(mw);
			else if(o->type == mwInputPane)
				DrawInputPane(o);
		}
		else
		{
			p.pane = o;
			runIndPlugin(o->pluginRef, pMWPaneUpdateMessage, &p);
		}
	SetPort(g);
}

static pascal void UpdateTextRegion(mwPanePtr w)
{
	MWPtr mw = w->mw;
	LongRect lr;
	short height;
	Rect r;
	RgnHandle rgn, rgn2;

	lr.top = w->drawArea.top + 3;
	lr.left = w->drawArea.left + 3;
	lr.bottom = w->drawArea.bottom - 3;
	lr.right = w->drawArea.right-18;
	
	WESetDestRect(&lr, mw->we);
	WESetViewRect(&lr, mw->we);

	rgn = NewRgn();
	rgn2 = NewRgn();

	//Resize scrollbar
	height = w->drawArea.bottom - w->drawArea.top;
	MoveControl(mw->vscr, w->drawArea.right-15, w->drawArea.top-1);
	SizeControl(mw->vscr, 16, height + 2);

	mw->vislines = height/mw->scrpHeight;
	
	SetRectRgn(rgn, w->drawArea.left, w->drawArea.top, w->drawArea.right - 15, w->drawArea.bottom);
	InsetRect(&r, 3, 3);
	RectRgn(rgn2, &r);
	DiffRgn(rgn, rgn2, rgn);
	EraseRgn(rgn);
	DisposeRgn(rgn);
	DisposeRgn(rgn2);

	WECalText(mw->we);
	WESelView(mw->we);
}

pascal void MWPaneResize(MWPtr mw)
{
	mwPanePtr o;
	pMWPaneResizeData p;
	GrafPtr gp;
	Rect portRect;
	
	GetPort(&gp);
	SetPortWindowPort(mw->w);
	
	GetPortBounds(GetWindowPort(mw->w), &portRect);
	InvalWindowRect(mw->w, &portRect);
	linkfor(o, mw->paneList)
		if(o->creator == mwShadowIRCPane && !o->pluginRef)
		{
			if(o->type == mwTextPane)
				UpdateTextRegion(o);
			else if(o->type == mwInputPane)
				UpdateInputRegion(o);
		}
		else
		{
			p.pane = o;
			runIndPlugin(o->pluginRef, pMWPaneResizeMessage, &p);
		}

	SetPort(gp);
}

pascal mwPanePtr MWFindPane(MWPtr mw, long type)
{
	mwPanePtr p;
	
	linkfor(p, mw->paneList)
		if(p->type == type)
			return p;
	
	return 0;
}

pascal void MWPaneRecalculate(MWPtr mw)
{
	mwPanePtr o = mw->paneList;
	char stop;
	char mostBottom;
	int x;
	Rect r;
	
	GetPortBounds(GetWindowPort(mw->w), &r);
	
	mostBottom = true;
	stop = 0;
	while(o)
	{
		switch(o->align)
		{
			case mwPaneTop:
				SetRect(&o->drawArea, r.left, r.top, r.right, r.top + o->askedHeight);
				r.top+=o->askedHeight + 1;
				break;
				
			case mwPaneBottom:
				if(r.bottom - r.top < o->askedHeight)
					x = r.bottom - r.top - 1;
				else
					x = o->askedHeight;
				
				SetRect(&o->drawArea, r.left, r.bottom - x, r.right, r.bottom);
				r.bottom-=x + 1;
				if(mostBottom)
				{
					if(o->askedHeight <= 14)	//only make it narrower if it's only as tall as the scroll area
						o->drawArea.right-=15;
					mostBottom = false;
				}
				break;
				
			case mwPaneLeft:
				if(o->askedWidth > (r.right - r.left) * 0.75)
					x = (r.right - r.left) * 0.75;
				else
					x = o->askedWidth;
				SetRect(&o->drawArea, r.left, r.top, r.left + x, r.bottom);
				r.left += x + 1;
				break;
				
			case mwPaneRight:
				if(o->askedWidth > (r.right - r.left) * 0.75)
					x = (r.right - r.left) * 0.75;
				else
					x = o->askedWidth;
				SetRect(&o->drawArea, r.right - x, r.top, r.right, r.bottom);
				r.right -= x + 1;
				break;
				
			case mwPaneCenter:
				stop = 1;
				break;
		}
		
		if(!stop)
		{
			o->givenHeight = o->drawArea.bottom - o->drawArea.top;
			o->givenWidth = o->drawArea.right - o->drawArea.left;
			o=o->next;
		}
		else
			break;
	}

	//We've assigned everything except the mwPaneCenters
	//For now, assume there's only one.
	while(o)
	{
		o->drawArea = r;

		o->givenHeight = o->drawArea.bottom - o->drawArea.top;
		o->givenWidth = o->drawArea.right - o->drawArea.left;
		o=o->next;
	}
	
}

inline mwPanePtr MWInsertPaneAfter(MWPtr mw, short align)
{
	mwPanePtr p = mw->paneList;
	
	if(p)
	{
		while(p->align <= align)
			if(p->next && p->next->align <= align)
				p=p->next;
			else
				break;

		if(p && p->align > align)
			p=p->prev;
	}
	//So, this gives us p == 0 or the last item in this alignment class
	return p;
}

pascal mwPanePtr MWNewPane(MWPtr mw, long type, short align, short height, short width)
{
	mwPanePtr insertAfter;
	mwPanePtr o;

	if(MWFindPane(mw, type))
		return 0;
	
	o=(mwPanePtr)NewPtr(sizeof(mwPaneRec));
	
	if(!o)
		return 0;
	
	insertAfter = MWInsertPaneAfter(mw, align);
	
	if(insertAfter)
	{
		o->next = insertAfter->next;
		insertAfter->next = o;
	}
	else
	{
		o->next = mw->paneList;
		if(o->next)
			o->next->prev = o;
		mw->paneList = o;
	}
	o->prev = insertAfter;
	
	if(sidr.yourInfo)
	{
		o->creator=mwPluginPane;
		o->pluginRef=sidr.yourInfo;
	}
	else
	{
		o->creator=mwShadowIRCPane;
		o->pluginRef = 0;
	}
	o->type = type;
	o->data = 0;
	o->mw = mw;
	o->align = align;
	o->askedHeight = height;
	o->askedWidth = width;
	
	MWPaneRecalculate(mw);
	return o;
}

#pragma mark -
pascal void MWRecalculateRects(MWPtr mw)
{
	short lRight, rLeft;
	short t;
	mwWidgetPtr o = mw->widgetList;
	mwPanePtr p;
	
	if(o) //don't do anything if we don't have any widgets...
	{
		p = MWFindPane(mw, mwWidgetsPane);
		
		lRight = -1;
		rLeft = p->drawArea.right;
		
		//Go to the end of the object list.
		while(o->next && o->next->align != mwPlaceholder)
			o=o->next;
		
		//o now contains the last object in the list. this will be all of the right-aligned objects.
		//Scan through and stop when we hit the first left aligned object and give everything the space it wants.
		while(o && o->align==mwAlignRight)
		{
			t=rLeft;
			rLeft -= o->askedWidth + 1;
			
			SetRect(&o->drawArea, rLeft, p->drawArea.bottom - 15, t, p->drawArea.bottom);
			o->givenWidth = t - rLeft;
			o=o->prev;
		}
		
		//Now that that's dealt with, we start giving everything the space it wants until we can't go any further.
		//That means that unless someone does somethign really stupid, the only thing we're gonna truncate is the
		//status line, which we can identify as having a creator/type of SIRC/stat.
		
		o=mw->widgetList;
		while(o && o->align<=mwAlignLeft)//<= so we cover mwForceLeft
		{
			t = lRight +2;
			if(o->askedWidth==-1) //give them as much as posssible. that is, rLeft - lRight -2 space.
				lRight = rLeft -2;
			else
				lRight += o->askedWidth;
			if(lRight > rLeft) //Houston, we have a problem
			{
				if(o->creator==mwShadowIRCWidget && o->type == mwTopicWidget)
					lRight = rLeft - 2;
				else // it's a plugin. Give 'em 16 pixels.
					lRight = t + 15;
			}
			SetRect(&o->drawArea, t, p->drawArea.bottom - 15, lRight, p->drawArea.bottom);
			
			o->givenWidth = lRight - t + 1;
			o=o->next;
		}
	}
}

pascal mwWidgetPtr MWFindWidget(MWPtr mw, long type)
{
	mwWidgetPtr p;
	
	linkfor(p, mw->widgetList)
		if(p->type == type)
			return p;

	return 0;
}

pascal mwWidgetPtr MWFindWidgetPoint(MWPtr mw, Point p)
{
	mwWidgetPtr o;
	
	if(mw)
		linkfor(o, mw->widgetList)
			if(PtInRect(p, &o->drawArea))
				return o;
	
	return 0;
}

pascal mwWidgetPtr MWNewWidget(MWPtr mw, long type, short align, short width)
{
	mwWidgetPtr p, prev, o;
	long creator;

	if(MWFindWidget(mw, type))
		return 0;
	
	o = (mwWidgetPtr)NewPtr(sizeof(mwWidgetRec));
	if(!o)
		return 0;
	
	if(sidr.yourInfo)
		creator=mwPluginWidget;
	else
		creator=mwShadowIRCWidget;
	
	prev = 0;
	p = mw->widgetList;
	if(align==mwForceLeft) //tack it after all other forcelefts
	{
		while(p && p->align == mwForceLeft)
			p=p->next;
		
		//p is now the one we want to insert before
		
		o->next = p;
		if(p)
			o->prev = p->prev;
		else
			o->prev = 0;
		
		if(o->next)
			o->next->prev = o;
		if(o->prev)
			o->prev->next = o;
			
		if(!o->prev)
			mw->widgetList = o;
	}
	else if(align==mwAlignLeft) //then tack it after the forceLefts.
	{
		//We need to step through and make sure we advance past the forceLefts
		while(p && p->align == mwForceLeft)
			p=p->next;
		
		if(p)
			prev=p->prev;
		else
			prev=0;
		
		o->next = p;
		o->prev = prev;
		if(p)
			p->prev = o;
		if(prev)
			prev->next = o;
		
		if(!prev) //then we didn't have a forceleft, since p->prev would be nil
			mw->widgetList=o;
	}
	else if(align==mwAlignRight) //then tack it on before the first right object
	{
		//Find the first align right object and insert there.
		while(p)
		{
			if(p->align == mwAlignRight)
				break;
			prev=p;
			p=p->next;
		}

		if(prev)
			prev->next=o;
		else
			mw->widgetList=o;
		o->next=p;
		o->prev=prev;
		if(prev)
			prev->next=o;
		if(p)
			p->prev=o;
	}
	else if(align==mwPlaceholder) //tack at end
	{
		while(p && p->next)
			p=p->next;
		
		p->next = o;
		o->next = 0;
		o->prev = p;
	}

	o->mw = mw;
	o->creator=creator;
	o->type=type;
	o->data=0;
	o->askedWidth=width;
	o->pluginRef=sidr.yourInfo;
	o->align=align;

	MWRecalculateRects(mw);
	return o;
}

static pascal void MWDestroyWidget2(mwWidgetPtr w)
{
	pMWWidgetDestroyData p;
	
	if(w->creator == mwShadowIRCWidget && !w->pluginRef)
	{
		if(w->type == mwModesWidget)
		{
			DisposePtr((Ptr)w->data);
		}
	}
	else if(w->creator == mwPluginWidget)
	{
		p.widget = w;
		runIndPlugin(w->pluginRef, pMWWidgetDestroyMesage, &p);
	}
	if(w->prev)
		w->prev->next = w->next;
	if(w->next)
		w->next->prev = w->prev;
	DisposePtr((Ptr)w);
}

pascal void MWDestroyWidget(mwWidgetPtr w)
{
	MWPtr mw = w->mw;
	MWDestroyWidget2(w);
	MWRecalculateRects(mw);
}

pascal void MWDestroyAllWidgets(MWPtr mw)
{
	mwWidgetPtr w = mw->widgetList, n;
	
	while(w)
	{
		n=w->next;
		MWDestroyWidget2(w);
		w=n;
	}
	
	mw->widgetList = 0;
}