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

#define __INPUTLINE__
#include <Carbon/Carbon.h>
#include "WASTE.h"

#include "StringList.h"
#include "StringKeys.h"
#include "IRCGlobals.h"
#include "utils.h"
#include "AppearanceHelp.h"
#include "Floaters.h"
#include "MsgWindows.h"
#include "DCC.h"
#include "connections.h"
#include "WindowList.h"
#include "InputLine.h"
#include "channels.h"
#include "IRCChannels.h"
#include "plugins.h"
#include "IRCAux.h"
#include "IRCCommands.h"
#include "IRCInput.h"
#include "Inline.h"
#include "DragDrop.h"
#include "MenuCommands.h"
#include "TextManip.h"
#include "CMenus.h"
#include "IRCInput.h"
#include "InputControl.h"

static InputWindowData inputLine;
char noFloatingInput = 0;

#pragma mark -
#pragma mark Status Control

typedef struct ILCSData {
	InputWindowDataPtr inputLine;
	ControlRef theControl;
	EventHandlerRef controlEvents;
	Rect bounds;

	iwWidgetPtr objectList;
	iwWidgetPtr status;
} ILCSData, *ILCSDataPtr;

static void IWRecalculateRects(ILCSDataPtr ilcsd)
{
	short lRight, rLeft;
	short t;
	iwWidgetPtr o;
	Rect wp;
	
	if(noFloatingInput)
		return;
	
	wp = ilcsd->bounds;
	
	lRight = 0;
	rLeft = wp.right - 2;
	
	//Go to the end of the object list.
	o = ilcsd->objectList;
	while(o->next)
		o = o->next;
	
	//o now contains the last object in the list. this will be all of the right-aligned objects.
	//Scan through and stop when we hit the first left aligned object and give everything the space it wants.
	while(o && o->align == iwRight)
	{
		t=rLeft - 1;
		rLeft -= o->askedWidth + 1;
		
		SetRect(&o->drawArea, rLeft, wp.top, t, wp.bottom);
		o->givenWidth = t - rLeft + 1;
		o=o->prev;
	}
	
	//Now that that's dealt with, we start giving everything the space it wants until we can't go any further.
	//That means that unless someone does somethign really stupid, the only thing we're gonna truncate is the
	//status line, which we can identify as having a creator/type of SIRC/stat.
	o = ilcsd->objectList;
	while(o && o->align==iwLeft)
	{
		t = lRight +1;
		if(o->askedWidth==-1) //give them as much as posssible. that is, rLeft - lRight -2 space.
			lRight = rLeft -2;
		else
			lRight += o->askedWidth;
		if(lRight > rLeft) //Houston, we have a problem
		{
			if(o->creator == iwShadowIRCObject && o->type == iwStatusLine)
				lRight = rLeft - 2;
			else // it's a plugin. Give 'em 16 pixels.
				lRight = t + 15;
		}
		SetRect(&o->drawArea, t, wp.top, lRight, wp.bottom);
		
		o->givenWidth = lRight - t;
		o=o->next;
	}
}

static iwWidgetPtr IWNewWidget(ILCSDataPtr ilcsd, long type, short align, short width)
{
	iwWidgetPtr p;
	iwWidgetPtr prev;
	iwWidgetPtr o;

	if(noFloatingInput)
		return 0;

	o = (iwWidgetPtr)NewPtr(sizeof(iwWidgetRec));
	
	p = ilcsd->objectList;
	prev=0;
	if(align==iwLeft) //then tack it onto the begining.
	{
		o->next=p;
		if(p)
			p->prev=o;
		ilcsd->objectList = o;
	}
	else if(align==iwRight) //then tack it on before the first right object
	{
		//Find the first align right object and insert there.
		while(p)
		{
			if(p->align == iwRight)
				break;
			prev=p;
			p=p->next;
		}

		if(prev)
			prev->next = o;
		else
			ilcsd->objectList = o;
		o->next=p;
		o->prev=prev;
		if(p)
			p->prev=o;
	}

	o->type=type;
	o->data=0;
	o->askedWidth=width;
	if(sidr.yourInfo)
	{
		o->creator = iwPluginObject;
		o->pluginRef=sidr.yourInfo;
	}
	else
	{
		o->creator = iwShadowIRCObject;
		o->pluginRef = 0;
	}
	o->align=align;

	IWRecalculateRects(ilcsd);
	
	return o;
}

static void IWInternalDraw(ILCSDataPtr ilcsd, iwWidgetPtr o)
{
	Str255 s;
	switch(o->type)
	{
		case iwStatusLine:
		{
			linkPtr link;
			Str255 nickname, channel, server;
			channelPtr ch;
			iwStatusObjectPtr stats = (iwStatusObjectPtr)o->data;
			short strwid, curPos, maxPos = o->drawArea.right;
			char boldedPopups = (ilcsd->inputLine->statuslineFlags & kBoldedPopups) == kBoldedPopups;
			
			link = CurrentTarget.link;
			if(link==0)
				link=firstLink;
			
			//prepare status text
			//¥nickname
			pstrcpy(link->CurrentNick, nickname);
			if(nickname[0]>22)
			{
				nickname[0]=21;
				nickname[21]='É';
			}
			
			//¥channel
			if(CurrentTarget.bad)
				GetIntString(channel, spInfo, sNobody);
			else
			{
				if(CurrentTarget.type==targChannel)
				{
					ch=CurrentTarget.chan;
					pstrcpy(CurrentTarget.chan->chName, channel);
					if(ch->hasOps)
					{
						pstrcpy(channel, &channel[1]);
						channel[0]++;
						channel[1]='@';
					}
					else if(ch->hasVoice)
					{
						pstrcpy(channel, &channel[1]);
						channel[0]++;
						channel[1]='+';
					}
				}
				else if(CurrentTarget.type==targDCC)
				{
					pstrcpy(CurrentTarget.dcc->dcc->dccUserName, &channel[1]);
					channel[0]=channel[1]+2;
					channel[1]='=';
					channel[channel[0]]='=';
				}
				else
					MWGetName(CurrentTarget.mw, channel);

			}
			if(channel[0]>22)
			{
				channel[0]=21;
				channel[21]='É';
			}
			SAppend1(channel, ' ');

			//¥server
			switch(link->serverStatus)
			{
				case S_OFFLINE:
					GetIntString(server, spInfo, sOnline);
					break;

				case S_LOOKUP:
					GetIntString(server, spInfo, sLookup);
					break;

				case S_OPENING:
					GetIntString(server, spInfo, sOpening);
					break;

				case S_CONN:
					if(link->CurrentServer[0])
					{
						pstrcpy(link->CurrentServer, &server[2]);
						server[0]=server[2]+2;
						server[1] = ' ';
						server[2] = '(';
					}
					else
						server[0]=0;
					break;

				case S_CLOSING:
					GetIntString(server, spInfo, sClosing);
					break;

				default:
					NumToString(link->serverStatus, &server[5]);
					server[0]=server[5]+4;
					*(long*)&server[1]=' Err';
					server[5]='(';
			}
			if(server[0]>35)
			{
				server[0]=33;
				server[31]='É';
			}
			
			//Draw status text
			MoveTo(o->drawArea.left, ilcsd->inputLine->statusLinePos); //o->drawArea.top);
			if(boldedPopups)
				TextFace(bold);
			else
				TextFace(0);
			DrawString(nickname);
			curPos= o->drawArea.left + StringWidth(nickname);
			stats->nickStart=o->drawArea.left;
			stats->nickEnd=curPos;
			if(boldedPopups)
				TextFace(0);
			
			if((ilcsd->inputLine->statuslineFlags & kUmodeFlagsOn) && link && link->yourUmodes)
			{
				int x;
				
				pstrcpy("\p [", s);
				if(ilcsd->inputLine->statuslineFlags & kUmodeFlagsPlus)
					SAppend1(s, '+');
				
				for(x=1;x<=link->yourUmodes[0];x++)
					if(link->yourUmodes[x])
						SAppend1(s, link->yourUmodes[x]);
				
				if(s[0]>2)
				{
					SAppend1(s, ']');
					DrawString(s);
					curPos += StringWidth(s);
				}
			}
			
			if(link->isAway)
			{
				DrawString("\p (Away)");
				curPos+=stats->awayWidth;
			}
			DrawString("\p is talking with ");
			curPos+=stats->istalkingwithWidth;
			
			stats->targStart=curPos;
			if(boldedPopups)
				TextFace(bold);
			DrawString(channel);
			curPos+=StringWidth(channel);
			stats->targEnd=curPos;
			
			stats->servStart=stats->servEnd=-1;
			if(boldedPopups)
				TextFace(0);
			curPos+=stats->onWidth;
			if(curPos< maxPos)
			{
				DrawString("\pon ");

				stats->servStart=curPos;
				
				if(boldedPopups)
					TextFace(bold);
				pstrcpy(link->linkPrefs->linkName, s);
				strwid=StringWidth(s);
				if(curPos + strwid > maxPos)
					TruncString(maxPos - curPos, s, truncEnd);
				DrawString(s);
				if(boldedPopups)
					TextFace(0);
				curPos += strwid;
				
				if(server[server[0]]==')')
					server[0]--;
				strwid=StringWidth(server) + stats->closeParenWid;
				if(curPos + strwid > maxPos)
					TruncString(maxPos - curPos - stats->closeParenWid, server, truncEnd);
				if(server[0] >= 4) //contains more than ' (É'
				{
					SAppend1(server, ')');
					DrawString(server);
				}

				curPos+=strwid;
				stats->servEnd = curPos;
			}
			break;
		}
	}
}

void UpdateStatusLine(void)
{
	if(noFloatingInput)
		return;
	
	if(!IsWindowVisible(inputLine.w))
		return;
	
	HIViewSetNeedsDisplay(inputLine.statusControl, true);
}

static void _UpdateStatusLine(ILCSDataPtr ilcsd)
{
	static int reentrant = 0;
	GrafPtr p0;
	Rect r;
	iwWidgetPtr o;
	pIWUpdateData pp;
	
	if(++reentrant > 1)
		return;
	
	GetPort(&p0);
	
	SetPortWindowPort(ilcsd->inputLine->w);
restart:
	r = ilcsd->bounds;
	
/*
	r.left = -2;
	
	if(inputLine.lock)
	{
		r.top = inputLine.statusLineHeight;
		RGBBackColor(&MedGrey);
		EraseRect(&r);
		RGBBackColor(&white);
	}
	
	r.top = -2;
	r.bottom = inputLine.statusLineHeight;
	
	r.top = r.left = -3;
	r.right ++;
*/
	DrawThemePlacard(&r, kThemeStateActive);
	
	RGBBackColor(&white);
	RGBForeColor(&shadowircColors[sicStatusLine]);
	
	linkfor(o, ilcsd->objectList)
	{
		TextFace(0);
		MoveTo(o->drawArea.left, ilcsd->bounds.bottom);
		if(o->creator == iwShadowIRCObject && !o->pluginRef)
			IWInternalDraw(ilcsd, o);
		else
		{
			pp.widget=o;
			runIndPlugin(o->pluginRef, pIWUpdateMessage, &pp);
		}
		
		if(reentrant > 1)
		{
			reentrant = 1;
			goto restart;
		}
	}
	
	SetPort(p0);
	reentrant = 0;
}

static long IWPopUpMenu(ILCSDataPtr ilcsd, Point p, MenuHandle m, long curItem)
{
	long newItem;

	LocalToGlobal(&p);

	SetMenuFont(m, ilcsd->inputLine->fontnum, ilcsd->inputLine->fontsize);

	InsertMenu(m, -1);
	newItem = PopUpMenuSelect(m, p.v, p.h, curItem);
	
	return newItem;
}

static void IWStatusLineWidgetClick(ILCSDataPtr ilcsd, iwWidgetPtr o, Point where, short modifiers)
{
	char sk=(modifiers&shiftKey)==shiftKey;
	iwStatusObjectPtr stats = (iwStatusObjectPtr)o->data;
	linkPtr link;
	MenuHandle m, wm, rcm;
	short i,x, ni;
	Point p;
	long newItem, curItem;
	LongString ls;
	Str255 s;
	short dashPoint;
	char b;
	MWPtr activeMW;
	
	if(where.h>=stats->nickStart && where.h<=stats->nickEnd)
	{
		link = CurrentTarget.link;
		
		m=GetMenu(1002);
		for(x=0;x<10;x++)
			if(link->linkPrefs->recentNicks[x][0])
			{
				InsertMenuItem(m, "\p-", 0+x);
				SetMenuItemText(m,1+x,link->linkPrefs->recentNicks[x]);
			}
		
		p.v=0;
		p.h=stats->nickStart;
		ni=CountMenuItems(m);
		newItem=IWPopUpMenu(ilcsd, p, m, ni);
		i=newItem&0xFF;
		if(i>1) //Don't change nick, because the current one is the top one in the list...
		{
			if(i==ni)
			{
				LSStrLS("\p/nick ", &ls);
				IADSetText(ilcsd->inputLine->inputData, &ls);
			}
			else if(i==ni-1)
			{
				for(x=1;x<10;x++)
					link->linkPrefs->recentNicks[x][0]=0;
			}
			else
			{
				GetMenuItemText(m, i, s);
				LSConcatStrAndStr("\pNICK ", s, &ls);
				SendCommand(link, &ls);
			}
		}
		DeleteMenu(1002);
		DisposeMenu(m);
	}
	else if((where.h>=stats->targStart) && (stats->targEnd>=where.h))
	{	//Clicked on channel
		//Build Menu
		if(!sk)
		{
			wm=gWindowMenu;
			m=GetMenu(1001);
			curItem=0;
			newItem=2;

			//Read out of windows menu, and stop at '-'...
			i=windowsStart;
			
			for(i=0;i<wmItems.channelsLength;i++)
			{
				GetWTitle((**wmItems.channels).mw[i]->w, s);
				AppendMenu(m, "\p-");
				SetMenuItemText(m, ++newItem, s);
			}
			
			if(wmItems.restLength)
			{
				AppendMenu(m, "\p-");
				dashPoint = ++newItem;
				
				for(i=0;i<wmItems.restLength;i++)
				{
					GetWTitle((**wmItems.rest).mw[i]->w, s);
					AppendMenu(m, "\p-");
					SetMenuItemText(m, ++newItem, s);
				}
			}
			else
				dashPoint = 255;
			
			activeMW = GetActiveMW();
			//Determine active target
			if(!activeMW || (activeMW == consoleWin))
				curItem=0;
			else
			{
				b=0;
				if(activeMW->winType==chanWin)
				{
					for(x=0;x<wmItems.channelsLength;x++)
						if(activeMW == (**wmItems.channels).mw[x])
						{
							b=1;
							curItem=x+3;
							break;
						}
					if(!b)
						curItem=1;
				}
				else
				{
					for(x=0;x<wmItems.restLength;x++)
						if(activeMW == (**wmItems.rest).mw[x])
						{
							curItem=x+dashPoint+1;
							break;
						}
				}
			}
			
			ni=CountMenuItems(m) + 2;
			AppendMenu(m, "\p-");
			AppendMenu(m, GetIntStringPtr(spInfo, sRecentChannels));
			SetItemCmd(m, ni, hMenuCmd);
			SetItemMark(m, ni, 100);
		}
		rcm=GetMenu(100);
		i=0;
		for(x=0;x<10;x++)
			if(CurrentTarget.link->linkPrefs->recentChannels[x][0])
			{
				i++;
				InsertMenuItem(rcm, "\p-", 1+i);
				SetMenuItemText(rcm, 2+i, CurrentTarget.link->linkPrefs->recentChannels[x]);
			}
		if(i)
			InsertMenuItem(rcm, "\p-", 2+i); //Insert the second dash
		p.v=0;
		p.h=stats->targStart;
		if(!sk)
		{
			InsertMenu(rcm, -1);
			newItem = IWPopUpMenu(ilcsd, p, m, curItem);
		}
		else
			newItem = IWPopUpMenu(ilcsd, p, rcm, CountMenuItems(rcm)-2);

		i = *(((short*)&newItem)+1);
		switch(*(short*)&newItem)
		{
			case 1001: //channel liust
				if(i==1) //console
					ToggleConsoleWindow();
				else if(i<dashPoint) //channel window
					HitWindowSelectWindowMenu(i - 2 + windowsStart);
				else //other window
					HitWindowSelectWindowMenu(windowsStart2+i-dashPoint);
				break;
			
			case 100: //recent channels
				ni=CountMenuItems(rcm);
				if(i==ni)
				{
					for(x=0;x<10;x++)
						CurrentTarget.link->linkPrefs->recentChannels[x][0]=0;
				}
				else
				{
					channelPtr ch;
					GetMenuItemText(rcm, i, s);
					ch=ChFind(s, CurrentTarget.link);
					if(ch)
						WSelect(ch->window->w);
					else
					{
						LSConcatStrAndStr("\pJOIN ", s, &ls);
						SendCommand(CurrentTarget.link, &ls);
					}
				}
				break;
		}
		//determine new target
		//destroy menu
		if(!sk)
		{
			DeleteMenu(1001);
			DisposeMenu(m);
		}
		DeleteMenu(100);
		DisposeMenu(rcm);
	}
	else if((where.h>=stats->servStart) && (stats->servEnd>=where.h))
	{
		linkPtr lp;

		m = GetMenu(101);
		for(x=0,lp=firstLink;lp;lp=lp->next,x++)
		{
			AppendMenu(m, "\p-");
			SetMenuItemText(m, x+4, lp->linkPrefs->linkName);
			if(lp->serverStatus != S_CONN || lp->connectStage != csOnline)
				DisableMenuItem(m, x+4);
		}
		
		ConnectionMenuHilites();
		p.v=0;
		p.h=stats->servStart;
		newItem = IWPopUpMenu(ilcsd, p, m, 4 + CurrentTarget.link->linkNum);
		i= *(((short*)&newItem)+1);
		
		switch(*(short*)&newItem)
		{
			case 101:
				if(i)
				{
					i-=4; //link number
					if(CurrentTarget.link->linkNum != i) //do nothing if current link
					{
						MWPtr w = GetActiveMW();
						linkPtr lnk = GetLinkNum(i);
						
						if(w->link == lnk)
							SetTarget(w, &CurrentTarget);
						else
						{
							InvalTarget(&CurrentTarget);
							CurrentTarget.link = lnk;
						}
						
						UpdateStatusLine();
					}
				}
				break;
		}
		
		DeleteMenu(101);
		DisposeMenu(m);
	}
}

static void StatusLineClick(ILCSDataPtr ilcsd, Point where, short modifiers)
{
	iwWidgetPtr o;
	pIWClickData pp;
	
	linkfor(o, ilcsd->objectList)
		if(PtInRect(where, &o->drawArea))
		{
			if(o->creator == iwShadowIRCObject && !o->pluginRef)
			{
				if(o->type == iwStatusLine)
					IWStatusLineWidgetClick(ilcsd, o, where, modifiers);
			}
			else
			{
				pp.widget = o;
				pp.where = where;
				runIndPlugin(o->pluginRef, pIWClickMessage, &pp);
			}
			
			break;
		}
 }

static const EventTypeSpec ilcsControlEvents[] = {
	{ kEventClassControl, kEventControlDraw },
	{ kEventClassControl, kEventControlBoundsChanged },
	{ kEventClassControl, kEventControlDispose },
	{ kEventClassControl, kEventControlHitTest },
	{ kEventClassControl, kEventControlActivate },
	{ kEventClassControl, kEventControlDeactivate },
	{ kEventClassControl, kEventControlTrack },
};

static OSStatus ILCSControlEventHandler(EventHandlerCallRef myHandler, EventRef event, void* userData)
{
#pragma unused(myHandler)
	ILCSDataPtr ilcsd = (ILCSDataPtr)userData;
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	OSStatus err;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassControl:
		{
			switch(eventKind)
			{
				case kEventControlTrack:
				{
					Point where;
					UInt32 modifiers;
					
					GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &where);
					GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(modifiers), NULL, &modifiers);
					
					StatusLineClick(ilcsd, where, modifiers);
					result = noErr;
					break;
				}
				case kEventControlDispose:
					free(ilcsd);
					break;
				
				case kEventControlDraw:
					_UpdateStatusLine(ilcsd);
					result = noErr;
					break;
				
				case kEventControlBoundsChanged:
				{
					GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &ilcsd->bounds);
					
					IWRecalculateRects(ilcsd);
					result = noErr;
					break;
				}
				
				case kEventControlHitTest:
				{
					//We only have one part, so we just check if they clicked in the bounds
					ControlPartCode thePart;
					Point where;
					
					err = GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(where), NULL, &where);
					if(!err)
					{
						if(PtInRect(where, &ilcsd->bounds))
							thePart = 1;
						else
							thePart = 0;
						
						err = SetEventParameter( event, kEventParamControlPart, typeControlPartCode, sizeof(thePart), &thePart);
					}
					
					result = err;
					break;
				}
				
				case kEventControlClick:
				case kEventControlActivate:
				case kEventControlDeactivate:
					result = noErr;
					break;
			}
			break;
		}
	}
	
	return result;
}

static OSStatus ILSCAttachToExistingControl(ControlRef theControl, InputWindowDataPtr inputLine)
{
	static EventHandlerUPP ilcsEventHandlerUPP = NULL;
	ILCSDataPtr ilcsd;
	UInt32 outCommandID;
	OSStatus err;
	iwStatusObjectPtr iws;
	
	ilcsd = calloc(1, sizeof(ILCSData));
	ilcsd->inputLine = inputLine;
	ilcsd->theControl = theControl;
	GetControlBounds(theControl, &ilcsd->bounds);
	
	//Allocate default Inputline objects
	ilcsd->status = IWNewWidget(ilcsd, iwStatusLine, iwLeft, -1);
	
	//Now, create the data pointer for the status object
	ilcsd->status->data = (long)NewPtr(sizeof(iwStatusObjectRec));
	iws = (iwStatusObjectPtr)ilcsd->status->data;
	iws->awayWidth = StringWidth("\p (Away)");
	iws->istalkingwithWidth = StringWidth("\p is talking with ");
	iws->onWidth = StringWidth("\pon ");
	iws->closeParenWid = CharWidth('(');
	iws->maxLineWid = (CharWidth('n') * (22+22+32+1)) + iws->awayWidth + iws->istalkingwithWidth + iws->onWidth;
	
	/* set our control's command id.  we don't actually use it, but it must
	be non-zero for our control to be sent command events.  only set it
	if it has not already been set.  */
	err = GetControlCommandID(theControl, &outCommandID);
	if(err == noErr)
	{
		if(outCommandID == 0)
			err = SetControlCommandID(theControl, 1);
	}
	
	if(!ilcsEventHandlerUPP)
		ilcsEventHandlerUPP = NewEventHandlerUPP(ILCSControlEventHandler);
	
	InstallEventHandler(GetControlEventTarget(theControl), ilcsEventHandlerUPP, GetEventTypeCount(ilcsControlEvents), ilcsControlEvents, ilcsd, &ilcsd->controlEvents);
	
	return noErr;
}

static OSStatus CreateInputLineStatusControl(WindowRef theWindow, InputWindowDataPtr inputLine, Rect *bounds, ControlRef *theControl)
{
	UInt32 featureSet;
	OSStatus err;
	ControlRef newControl;
	
	featureSet = kControlSupportsEmbedding | kControlWantsActivate | kControlHasSpecialBackground;
	
	err = CreateUserPaneControl(theWindow, bounds, featureSet, &newControl);
	if(!err)
	{
		err = ILSCAttachToExistingControl(newControl, inputLine);
		if(!err)
			*theControl = newControl;
		else
			DisposeControl(newControl);
	}
	
	return err;
}



#pragma mark -

/*
pascal short IWOverride(long type, iwWidgetPtr *object)
{
	iwWidgetPtr o;
	
	if(noFloatingInput)
		return iwOverrideNotFound;

	*object=0;
	linkfor(o, inputLine.objectList)
		if(o->type==type)
		{
			if(o->creator!=iwShadowIRCObject)
				return iwOverridePluginObject;
			else
			{
				if(!o->pluginRef)
				{
					*object=o;
					o->pluginRef = sidr.yourInfo;
					return iwOverrideNoErr;
				}
				else
					return iwOverrideAlready;
			}
		}
	
	return iwOverrideNotFound;
}

pascal char IWDeleteWidget(iwWidgetPtr o)
{
	if(o && (!sidr.yourInfo || sidr.yourInfo == o->pluginRef))
	{
		if(o->prev)
			o->prev->next = o->next;
		if(o->next)
			o->next->prev = o->prev;
		
		DisposePtr((Ptr)o);
		IWRecalculateRects();
		
		return 1;
	}
	else
		return 0;
}
*/

#pragma mark -

static void IWGrow(WindowRef window, Rect r)
{
	Rect textRect;
	
	mainPrefs->inputLoc = r;
	
	textRect.top = inputLine.statusLineHeight;
	textRect.left = 0;
	textRect.bottom = r.bottom - r.top;
	textRect.right = r.right - r.left - 10;
	IADSetFieldBounds(inputLine.inputData, textRect);
	
	textRect.bottom = textRect.top;
	textRect.top = 0;
	textRect.right =  r.right - r.left;
	SetControlBounds(inputLine.statusControl, &textRect);
	
	GetPortBounds(GetWindowPort(window), &r);
	InvalWindowRect(window, &r);
}

static OSStatus InputLineWindowEventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
#pragma unused(handlerCallRef, userData)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	HICommand hiCommand;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassWindow:
		{
			WindowRef ilWindow;
			GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL, sizeof(WindowRef), NULL, &ilWindow);
			switch (eventKind)
			{
				//For now, we do nothing as it is consistent with what we used to do. This will need to be fixed, however...
				case kEventWindowActivated:
				case kEventWindowShown:
					IADActivate(inputLine.inputData, true);
					result = noErr;
					break;
					
				case kEventWindowDeactivated:
				case kEventWindowHidden:
					IADActivate(inputLine.inputData, false);
					result = noErr;
					break;
				
				case kEventWindowGetMinimumSize:
				{
					const Point minSize = {32, 200};
					
					SetEventParameter(event, kEventParamDimensions, typeQDPoint, sizeof(Point), &minSize);
					result = noErr;
					break;
				}
				case kEventWindowBoundsChanged:
				{
					Rect growRect;
					
					GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &growRect);
					IWGrow(ilWindow, growRect);
					result = noErr;
					break;
				}
			}
			break;
		}
		
		case kEventClassCommand:
		{
			GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(hiCommand), NULL, &hiCommand);
			
			switch(eventKind)
			{
				case kEventCommandUpdateStatus:
					switch(hiCommand.commandID)
					{
						case kHICommandCut:
						case kHICommandCopy:
						case kHICommandPaste:
						case kHICommandClear:
						case kHICommandSelectAll:
							break;
					}
					break;
			}
			break;
		}
	}

	return result;
}

static OSErr IWDragTrackingHandler(DragTrackingMessage message, WindowPtr window, void* refCon, DragReference drag)
{
#pragma unused(refCon)
	return IADFieldTrackDrag(inputLine.inputData, message, window, drag);
}

static OSErr IWDragReceiveHandler(WindowPtr window, void* refCon, DragReference drag)
{
#pragma unused(refCon)
	return IADFieldReceiveDrag(inputLine.inputData, window, drag);
}

void OpenInputLine()
{
	if(!inputLine.w)
	{
		Rect textRect;
		GrafPtr p0;
		short line2;
		Rect sb;
		char zeroPosition;
		char offscreen;
		CFStringRef inputWindowTitle;
		Rect wr;
		Rect statusControlRect;
		const EventTypeSpec ilSpec[] = {
			{kEventClassWindow, kEventWindowGetMinimumSize},
			{kEventClassWindow, kEventWindowBoundsChanged},
			{kEventClassWindow, kEventWindowActivated},
			{kEventClassWindow, kEventWindowDeactivated},
			{kEventClassWindow, kEventWindowShown},
			{kEventClassWindow, kEventWindowHidden},
		};
		static EventHandlerUPP ilUPP = NULL;
		static DragTrackingHandlerUPP iwTrackingHandlerUPP = nil;
		static DragReceiveHandlerUPP iwReceiveHandlerUPP = nil;

		if(mainPrefs->nonGlobalInput)
		{
			noFloatingInput = 1;
			return;
		}
		
		// add Live Resize and attach the Standard Handler [smcgovern]
		CreateNewWindow(kFloatingWindowClass, kWindowStandardFloatingAttributes | kWindowResizableAttribute | kWindowLiveResizeAttribute | kWindowStandardHandlerAttribute | kWindowCompositingAttribute, &mainPrefs->inputLoc, &inputLine.w);
		inputWindowTitle = CFCopyLocalizedString(kInputWindowTitle, NULL);
		SetWindowTitleWithCFString(inputLine.w, inputWindowTitle);
		CFRelease(inputWindowTitle);
		
		if(inputLine.w)
		{
			// Get rid of the close box
			ChangeWindowAttributes(inputLine.w, NULL, kWindowCloseBoxAttribute);
			
			GetPort(&p0);
			SetPortWindowPort(inputLine.w);
			
			//Determine the height of the status control
			inputLine.fontnum = FMGetFontFamilyFromName(mainPrefs->defaultFontName);
			inputLine.fontsize = mainPrefs->defaultFontSize;
			
			TextFont(inputLine.fontnum);
			TextSize(inputLine.fontsize);
			GetFontInfo(&inputLine.fi);
			
			inputLine.statusLinePos = inputLine.fi.ascent + inputLine.fi.leading + 2;
			inputLine.statusLineHeight = inputLine.statusLinePos + inputLine.fi.descent + 3;
			line2 = inputLine.statusLineHeight + inputLine.fi.leading + inputLine.fi.ascent;
			
			//Set the inputline position
			zeroPosition = ((mainPrefs->inputLoc.left == 0) && (mainPrefs->inputLoc.right == 0));
			offscreen = !RectInRgn(&mainPrefs->inputLoc, GetGrayRgn());
			if(zeroPosition || offscreen)
			{
				short h,v;
				
				GetAvailableWindowPositioningBounds(GetGDevice(), &sb);
				if(zeroPosition)
				{
					h = 80* inputLine.fi.widMax + 16;
					v = line2 + inputLine.fi.descent + inputLine.fi.leading + 10;
				}
				else
				{
					h = mainPrefs->inputLoc.right - mainPrefs->inputLoc.left;
					v = mainPrefs->inputLoc.bottom - mainPrefs->inputLoc.top;
				}
				
				SizeWindow(inputLine.w, h, v, 1);
				GetPortBounds(GetWindowPort(inputLine.w), &wr);
				MoveWindow(inputLine.w, (sb.right-sb.left-wr.right+2)/2 -1, sb.bottom - wr.bottom-5, false);
				WGetBBox(inputLine.w, &mainPrefs->inputLoc);
				
				mainPrefs->inputLoc.right = mainPrefs->inputLoc.left + h;
				mainPrefs->inputLoc.bottom = mainPrefs->inputLoc.top + v;
			}

			//Add Drag Handlers
			if(!iwTrackingHandlerUPP)
			{
				iwTrackingHandlerUPP = NewDragTrackingHandlerUPP(IWDragTrackingHandler);
				iwReceiveHandlerUPP = NewDragReceiveHandlerUPP(IWDragReceiveHandler);
			}
			InstallTrackingHandler(iwTrackingHandlerUPP, inputLine.w, inputLine.w);
			InstallReceiveHandler(iwReceiveHandlerUPP, inputLine.w, inputLine.w);
			
			inputLine.statuslineFlags = kUmodeFlagsOn | kUmodeFlagsPlus | kBoldedPopups;
			
/*
			WESetInfo(wePreTrackDragHook, &sPreTrackerUPP, twe);
*/
			
			textRect.top = inputLine.statusLineHeight;
			textRect.left = -2;
			textRect.right = (mainPrefs->inputLoc.right - mainPrefs->inputLoc.left) - 10; //local coord
			textRect.bottom = (mainPrefs->inputLoc.bottom - mainPrefs->inputLoc.top); //local coord
			
			inputLine.inputData = IADNew(inputLine.w, textRect, NewKey);
			
			statusControlRect.top = 0;
			statusControlRect.left = 0;
			statusControlRect.bottom = inputLine.statusLineHeight;
			statusControlRect.right = mainPrefs->inputLoc.right - mainPrefs->inputLoc.left;
			CreateInputLineStatusControl(inputLine.w, &inputLine, &statusControlRect, &inputLine.statusControl);
			
			WSelect(inputLine.w);
			
			SetUserFocusWindow(inputLine.w);
			
			// assign Event Handler to the window [smcgovern]
			if(!ilUPP)
				ilUPP = NewEventHandlerUPP(InputLineWindowEventHandler);
	
			InstallWindowEventHandler(inputLine.w, ilUPP, GetEventTypeCount(ilSpec), ilSpec, NULL, NULL);

			SetPort(p0);
		}
	}
}

#pragma mark -

InputWindowDataPtr GetInputWindowData(void)
{
	if(noFloatingInput)
		return NULL;
	else
		return &inputLine;
}

inputAreaDataPtr ILGetInputDataFromMW(MWPtr mw)
{
	if(!noFloatingInput)
		return inputLine.inputData;
	else if(mw)
		return mw->inputData;
	else //no mw specified; try the current window and see if that works
	{
		mw = MWFromWindow(ActiveNonFloatingWindow());
		if(mw)
			return mw->inputData;
		else
			return 0;
	}
}


#pragma mark -

void IWLock(void)
{
	if(!noFloatingInput && !inputLine.lock)
	{
/*
		inputLine.lock = 1;
		
		IADSetTextPtr(inputLine,inputData, NULL, 0);
		WEFeatureFlag(weFReadOnly, weBitSet, inputLine.inputData->il);
		UpdateStatusLine();
*/
	}
}

void IWUnlock(void)
{
	if(!noFloatingInput && inputLine.lock)
	{
/*
		Rect r;
		GrafPtr p0;

		GetPort(&p0);

		SetPortWindowPort(inputLine.w);
		WGetBBox(inputLine.w, &r);
		GlobalToLocal((Point*)&r.bottom);

		r.left = -2;
		r.top = inputLine.statusLineHeight;
		RGBBackColor(&white);
		EraseRect(&r);
		inputLine.lock = 0;
		SetPort(p0);
		WEFeatureFlag(weFReadOnly, weBitClear, inputLine.inputData->il);
*/
	}
}
