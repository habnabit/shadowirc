/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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
#include <Carbon/Carbon.h>

#include "LongStrings.h"
#include "AppearanceHelp.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "MWPanes.h"
#include "Inputline.h"
#include "IRCChannels.h"
#include "IRCInput.h"
#include "channels.h"
#include "DCC.h"
#include "connections.h"
#include "plugins.h"
#include "IRCCommands.h"
#include "inline.h"
#include "DragDrop.h"
#include "utils.h"
#include "Floaters.h"

enum flavors{
	kTextFlavor		= 'TEXT',
	kMWinFlavor		= 'MWIN',
	kUserFlavor		= 'USER',
};

enum {
	kEntering 	= kDragTrackingEnterWindow,
	kIn			= kDragTrackingInWindow,
	kLeaving 	= kDragTrackingLeaveWindow
};


WEPreTrackDragUPP sPreTrackerUPP = 0;

static OSErr MWTrackDragPane2(mwPanePtr o, Point mouse, DragTrackingMessage message, DragReference drag);
inline OSErr MWReceiveDrag(MWPtr mw, DragReference drag);
static OSErr MWTextPaneReceiveDrag(MWPtr mw, DragReference drag);
static OSErr MWTrackDrag(MWPtr mw,  DragTrackingMessage message, DragReference drag);
static OSErr MWTrackDragWidget2(mwWidgetPtr o, Point mouse, short state, DragReference drag);
static OSErr MWWidgetTrackDrag(MWPtr mw,  Point mouse, DragTrackingMessage message, DragReference drag);
static OSErr MWWidgetReceiveDrag(MWPtr mw, Point mouse, DragReference drag);

static OSErr MWTrackingHandler(DragTrackingMessage message, WindowPtr window, void* refCon, DragReference drag);
static OSErr MWReceiveHandler(WindowPtr window, void* refCon, DragReference drag);

static OSErr PreDrag(DragReference drag, WEReference we);

pascal void DragHilightRect(const Rect *re, DragReference drag)
{
	RgnHandle r = NewRgn();

	RectRgn(r, re);
	ShowDragHilite(drag, r, true);
	DisposeRgn(r);
}

pascal void DragHilightRectBG(const Rect *re, DragReference drag, char hilite)
{
	RgnHandle r = NewRgn();
	RgnHandle r2 = NewRgn();
	
	RectRgn(r, re);
	CopyRgn(r, r2);
	InsetRgn(r2, 2, 2);
	DiffRgn(r, r2, r);

	DisposeRgn(r2);
	
	if(hilite)
	{
		EraseRgn(r);
		ShowDragHilite(drag, r, true);
	}
	else
	{
		GrafPtr gp;
		WindowPtr win;
		MWPtr mw;
		
		HideDragHilite(drag);

		//This is glorious overkill, but who cares?
		GetPort(&gp);
		win = GetWindowFromPort(gp);
		mw = MWFromWindow(win);
		if(mw)
		{
			InvalWindowRgn(win, r);
			BeginUpdate(win);
			DrawMWinStatus(mw);
			EndUpdate(win);
		}
	}
	DisposeRgn(r);
}

pascal char DragGetType(DragReference drag, long type, void* *data, long *size)
{
	unsigned short items, i;
	FlavorFlags flags;
	ItemReference item;
	OSErr result;
	long siz;
	
	CountDragItems(drag, &items);
	for(i=1;i<=items;i++)
	{
		GetDragItemReferenceNumber(drag, i, &item);
		
		result = GetFlavorFlags(drag, item, type, &flags);
		if(!result)
		{
			GetFlavorDataSize(drag, item, type, &siz);
			*data = NewPtr(siz);
			GetFlavorData(drag, item, type, *data, &siz, 0);
			if(size)
				*size = siz;
			return true;
		}
	}
	
	*data = 0;
	return false;
}

pascal char DragAddPtr(DragReference drag, ItemReference item, long type, void* data, long flags)
{
	return AddDragItemFlavor(drag, item, type, &data, sizeof(data), flags);
}

pascal char DragGetPtr(DragReference drag, long type, void* *data)
{
	Ptr d;
	char b;
	
	b=DragGetType(drag, type, (void**)&d, 0);
	*data = *(Ptr*)d;
	DisposePtr(d);
	return b;
}

pascal char DragIsTypeAvail(DragReference drag, long type)
{
	unsigned short items, i;
	FlavorFlags flags;
	ItemReference item;
	OSErr result;
	
	CountDragItems(drag, &items);
	for(i=1;i<=items;i++)
	{
		GetDragItemReferenceNumber(drag, i, &item);
		
		result = GetFlavorFlags(drag, item, type, &flags);
		if(!result)
			return true;
	}
	
	return false;
}

static OSErr MWWidgetReceiveDrag(MWPtr mw, Point mouse, DragReference drag)
{
	channelPtr ch;
	mwWidgetPtr o;
	OSErr ret = dragNotAcceptedErr;
	
	o=MWFindWidgetPoint(mw, mouse);
	if(o)
	{
		switch(o->type)
		{
			case mwTopicWidget:
				ch = MWGetChannel(mw);
				if(ch)
				{
					if(DragIsTypeAvail(drag, kTextFlavor))
					{
						Ptr text;
						long size;
						long idx;
						Str255 s;
						LongString ls;
						DragGetType(drag, kTextFlavor, (void**)&text, &size);
						for(idx=0;idx<=size && text[idx]!=13;idx++)
							;
						
						idx--;
						if(idx>255)
							idx=255;
						BlockMoveData(text, &s[1], idx);
						s[0]=idx;
						LSStrCat4(&ls, "\pTOPIC ", ch->chName, "\p :", s);
						SendCommand(ch->link, &ls);
						DisposePtr(text);
						ret = 0;
					}
				}
				break;
			
			default:
				if(o->creator == 'PLUG')
				{
					pMWWidgetDragReceiveData p;
					
					p.widget = o;
					p.drag = drag;
					p.mouse = mouse;
					p.received = false;
					runIndPlugin(o->pluginRef, pMWWidgetDragReceiveMessage, &p);
					if(p.received)
						ret = 0;
				}
		}
	}
	return ret;
}

static OSErr MWTrackDragWidget2(mwWidgetPtr o, Point mouse, short state, DragReference drag)
{
	MWPtr mw;
	
	if(o)
	{
		switch(o->type)
		{
			case mwTopicWidget:
				mw = o->mw;
				if(mw->winType==chanWin)
				{
					if(DragIsTypeAvail(drag, kTextFlavor))
					{
						if(state == kEntering)
						{
							Rect r = o->drawArea;
							r.top++;
							DragHilightRectBG(&r, drag, true);
						}
						else if(state == kLeaving)
						{
							Rect r = o->drawArea;
							r.top++;
							DragHilightRectBG(&r, drag, false);
						}
					}
				}
				break;

			default:
				if(o->creator == 'PLUG')
				{
					pMWWidgetDragTrackData p;
					
					p.widget = o;
					p.drag = drag;
					p.message = state;
					p.mouse = mouse;
					runIndPlugin(o->pluginRef, pMWWidgetDragTrackMessage, &p);
				}
		}
	}
	
	return 0;
}

static mwWidgetPtr lastWidget = 0;
static mwPanePtr lastPane = 0;

static OSErr MWWidgetTrackDrag(MWPtr mw,  Point mouse, short state, DragReference drag)
{
	mwWidgetPtr o;
	OSErr ret = 0;
	
	if(state == kLeaving)
	{
		if(lastWidget)
			MWTrackDragWidget2(lastWidget, mouse, kLeaving, drag);
		lastWidget = 0;
	}
	else if(state == kEntering)
	{
		o=MWFindWidgetPoint(mw, mouse);
		if(o)
		{
			ret  = MWTrackDragWidget2(o, mouse, kEntering, drag);
			lastWidget = o;
		}
	}
	else
	{
		o=MWFindWidgetPoint(mw, mouse);
		if(o!=lastWidget)
		{
			if(lastWidget)
				MWTrackDragWidget2(lastWidget, mouse, kLeaving, drag);
			
			if(o)
				ret = MWTrackDragWidget2(o, mouse, kEntering, drag);
		}
		else
			ret = MWTrackDragWidget2(o, mouse, kIn, drag);
		
		lastWidget = o;
	}
	
	return ret;
}

static OSErr MWTrackDragPane2(mwPanePtr o, Point mouse, DragTrackingMessage message, DragReference drag)
{
	OSErr ret;
	WEReference il;
	
	if(o)
	{
		MWPtr mw = o->mw;
		
		switch(o->type)
		{
			case mwTextPane:
				if(!DragIsTypeAvail(drag, kTextFlavor) && DragIsTypeAvail(drag, kDragFlavorTypeHFS))
				{
					if(!mw->inactive && (mw->winType == queryWin || mw->winType == dccWin))
					{
						if(message == kEntering)
						{
							Rect r = o->drawArea;
							r.right-=15;
							DragHilightRect(&r, drag);
						}
						else if(message == kLeaving)
							HideDragHilite(drag);
					}
				}
				else
					if(mw->winType != conWin)
						ret = WETrackDrag(message, drag, mw->we);
				break;
			
			case mwWidgetsPane:
				ret = MWWidgetTrackDrag(mw, mouse, message, drag);
				break;
			
			case mwInputPane:
				il = ILGetWEFromMW(mw);
				ret = WETrackDrag(message, drag, il);
				break;

			default:
				if(o->creator == 'PLUG')
				{
					pMWPaneDragTrackData p;
					
					p.pane = o;
					p.drag = drag;
					p.message = message;
					p.mouse = mouse;
					runIndPlugin(o->pluginRef, pMWPaneDragTrackMessage, &p);
				}
		}
	}
	
	return ret;
}

static OSErr MWTrackDrag(MWPtr mw,  DragTrackingMessage message, DragReference drag)
{
	mwPanePtr o;
	Point mouse;
	OSErr ret;
	
	GetDragMouse(drag, &mouse, 0);
	GlobalToLocal(&mouse);
	
	ret = 0;
	
	if(message == kEntering)
	{
		lastPane = o = MWFindPanePoint(mw, mouse);
		
		if(o)
			ret = MWTrackDragPane2(o, mouse, kEntering, drag);
		return ret;
	}
	else if(message == kLeaving)
	{
		if(lastPane)
			ret = MWTrackDragPane2(lastPane, mouse, kLeaving, drag);

		lastPane = 0;
		
		return ret;
	}
	else
	{
		o = MWFindPanePoint(mw, mouse);
		if(o!=lastPane)
		{
			if(lastPane)
				MWTrackDragPane2(lastPane, mouse, kLeaving, drag);
			
			if(o)
				ret = MWTrackDragPane2(o, mouse, kEntering, drag);
		}
		else
		{
			if(o)
				ret = MWTrackDragPane2(o, mouse, message, drag);
		}
		
		lastPane = o;
		return ret;
	}
}

static OSErr MWTextPaneReceiveDrag(MWPtr mw, DragReference drag)
{
	OSErr ret;
	MWPtr mwFrom;
	
	ret = dragNotAcceptedErr;
	if(!DragIsTypeAvail(drag, kTextFlavor) && DragIsTypeAvail(drag, kDragFlavorTypeHFS))
	{
		if(!mw->inactive && (mw->winType == queryWin || mw->winType == dccWin))
		{
			HFSFlavor *hf;
			Str255 s;
			unsigned short items, i;
			ItemReference item;
			FlavorFlags flags;
			long siz;
			
//			if(mw->winType == queryWin)
				MWGetName(mw, s);
//			else
//				pstrcpy(MWGetDCC(mw)->dcc->dccUserName, s);
			CountDragItems(drag, &items);
			for(i=1;i<=items;i++)
			{
				GetDragItemReferenceNumber(drag, i, &item);
				
				if(!GetFlavorFlags(drag, item, kDragFlavorTypeHFS, &flags))
				{
					GetFlavorDataSize(drag, item, kDragFlavorTypeHFS, &siz);
					hf = (HFSFlavor*)NewPtr(siz);
					GetFlavorData(drag, item, kDragFlavorTypeHFS, hf, &siz, 0);
					
					DoDCCSendFile(mw->link, s, &hf->fileSpec, true, mainPrefs->dccReverseSends);

					DisposePtr((Ptr)hf);
					ret = 0;
				}
			}
			
			return ret;
		}
	}
	
	if(mw->winType != conWin)
	{
		Point mouse;
		mwPanePtr tp;
		Rect r;
		char ok;
		
		GetMouse(&mouse);
		tp =  mw->textPane;
		r = tp->drawArea;
		if(PtInRect(mouse, &r) && DragIsTypeAvail(drag, 'TEXT'))
		{
			Ptr text;
			long size;
			
			DragGetType(drag, kTextFlavor, (void**)&text, &size);
			mwFrom = 0;
			if(DragIsTypeAvail(drag, kMWinFlavor))
				DragGetPtr(drag, kMWinFlavor, (void**)&mwFrom);
			
			ok=1;
			if(mw->winType == chanWin && DragIsTypeAvail(drag, kUserFlavor))
			{
				channelPtr ch;
				UserListPtr user;
				
				DragGetPtr(drag, kUserFlavor, (void**)&user);
				ch = MWGetChannel(mw);
				if(ch)
				{
					LongString ls;
					LSStrCat4(&ls, "\pINVITE ", user->nick, "\p ", ch->chName);
					HandleCommand(ch->link, &ls);
					ok=0;
				}
			}
			
			if(mwFrom == mw)
			{
				if(!mainPrefs->ddToSameWin)
					ok = false;
				else
				{
					long s0, s1;
					GrafPtr gp;
					LongPt dropLocation;
					WEEdge dropEdge;
					long io;
					
					GetPort(&gp);
					SetPortWindowPort(mw->w);
					
					WEPointToLongPoint(mouse, &dropLocation);
					io=WEGetOffset(&dropLocation, &dropEdge, mw->we);
					WEGetSelection(&s0, &s1, mw->we);
					if(dropEdge == kTrailingEdge)
						io--;
					ok = !(io >= s0 && io < s1);
					SetPort(gp);
				}
			}
			
			if(ok && text && size)
			{
				ProcessInputHunk(text, size, mw, true, 0);
				ret = 0;
			}
			
			if(text)
				DisposePtr(text);
		}
	}
	
	return ret;
}

inline OSErr MWReceiveDrag(MWPtr mw, DragReference drag)
{
	mwPanePtr o;
	Point mouse;
	GrafPtr gp;
	OSErr ret = dragNotAcceptedErr;

	GetPort(&gp);
	SetPortWindowPort(mw->w);
	
	GetMouse(&mouse);
	o = MWFindPanePoint(mw, mouse);
	if(o)
	{
		switch(o->type)
		{
			case mwTextPane:
				ret = MWTextPaneReceiveDrag(mw, drag);
				break;
			
			case mwInputPane:
			{
				OSErr err = WEReceiveDrag(drag, ILGetWEFromMW(mw));
				if(!err)
				{
					processPaste(mw, true);
					ret = 0;
				}
				break;
			}
			
			case mwWidgetsPane:
				ret = MWWidgetReceiveDrag(mw, mouse, drag);
				break;
			
			default:
				if(o->creator == 'PLUG')
				{
					pMWPaneDragReceiveData p;
					
					p.pane = o;
					p.drag = drag;
					p.mouse = mouse;
					p.received = false;
					runIndPlugin(o->pluginRef, pMWPaneDragReceiveMessage, &p);
					if(p.received)
						ret = 0;
				}
		}
	}
	SetPort(gp);
	
	return ret;
}

static OSErr MWTrackingHandler(DragTrackingMessage message, WindowPtr window, void* refCon, DragReference drag)
{
	MWPtr mw;
	OSErr ret = dragNotAcceptedErr;
	
	if(!inputLocked)
	{
		if(message == kDragTrackingEnterHandler || message == kDragTrackingLeaveHandler)
		{
			lastWidget = 0;
			lastPane = 0;
		}
		
		if(window)
		{
			if(!(message == kDragTrackingEnterHandler || message == kDragTrackingLeaveHandler))
			{
				mw = (MWPtr)refCon;
				ret = MWTrackDrag(mw, message, drag);
			}
		}
	}
	
	return ret;
}

static OSErr MWReceiveHandler(WindowPtr window, void* refCon, DragReference drag)
{
	MWPtr mw;
	OSErr err = dragNotAcceptedErr;
	
	if(!inputLocked && window)
	{
		mw = (MWPtr)refCon;
		err = MWReceiveDrag(mw, drag);
	}
	
	return err;
}

static OSErr PreDrag(DragReference drag, WEReference we)
{
	MWPtr mw = 0;
	Str255 title;
	OSErr err;
	long s0, s1;
	Str255 s;
	Handle h;
	channelPtr ch;
	
	if(WEGetInfo(weRefCon, &mw, we))
		return paramErr;
	
	if(mw->magic == MW_MAGIC)
	{
		//¥¥Fix for channel, dcc windows
		GetWTitle(mw->w, title);
	}
	else if((WindowPtr)mw == inputLine.w)
	{
		GetWTitle((WindowPtr)mw, title);
	}
	else
		return paramErr;
	
	//	add a 'clnm' flavor containing the name of the document originating the drag
	//	this flavor is used by the Finder (version 8.0 and later) to determine the
	//	name of the clipping file
	err = AddDragItemFlavor(drag, (ItemReference)we, kFlavorTypeClippingName, title, title[0]+1, flavorNotSaved);
	
	//Then, add a 'MWIN' flavor containing the MWPtr to the window.
	if(!err)
		err = DragAddPtr(drag, (ItemReference)we, kMWinFlavor, mw, flavorSenderOnly);
	
	//Check and see if we have a single word selected.
	ch = MWGetChannel(mw);
	if(ch)
	{
		UserListPtr u;
		
		WEGetSelection(&s0, &s1, we);
		if(s1 - s0 > 0&& s1 - s0 < 64)
		{
			h = WEGetText(we);
			BlockMoveData(&(*h)[s0], &s[1], s1-s0);
			s[0]=s1-s0;
			u = ULFindUserName(ch, s);
			if(u)
				DragAddPtr(drag, (ItemReference)we, kUserFlavor, u, flavorSenderOnly);
		}
	}
	return err;
}

void InitDrag(void)
{
	sPreTrackerUPP = NewWEPreTrackDragProc(PreDrag);
}

void MWInitDrag(MWPtr mw)
{
	static DragTrackingHandlerUPP sMWTrackingHandlerUPP = nil;
	static DragReceiveHandlerUPP sMWReceiveHandlerUPP = nil;
	
	//Create handlers if necessary
	if(!sMWTrackingHandlerUPP)
	{
		sMWTrackingHandlerUPP = NewDragTrackingHandlerUPP(MWTrackingHandler);
		sMWReceiveHandlerUPP = NewDragReceiveHandlerUPP(MWReceiveHandler);
	}
	
	InstallTrackingHandler(sMWTrackingHandlerUPP, mw->w, mw);
	InstallReceiveHandler(sMWReceiveHandlerUPP, mw->w, mw);
}
