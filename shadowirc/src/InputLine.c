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

#define __INPUTLINE__
#include <Carbon/Carbon.h>
#include "WASTE.h"

#include "StringList.h"
#include "IRCGlobals.h"
#include "utils.h"
#include "OffscreenDrawing.h"
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

inputLineRec inputLine;

char noFloatingInput = 0;

inline void IWInternalDraw(iwWidgetPtr o);

static pascal char _ILInsertLine(WEReference il, LongString *ls, char select);
static pascal char _ILSetLine(WEReference il, LongString *ls);
static pascal char _ILGetLine(WEReference il, LongString *ls);
static pascal long _ILGetHpos(MWPtr mw);
static pascal CharsHandle _ILGetHist(MWPtr mw);
static pascal void _ILSetHpos(MWPtr mw, long hp);

pascal void IWRecalculateRects(void)
{
	short lRight, rLeft;
	short t;
	iwWidgetPtr o;
	Rect wp;
	
	if(noFloatingInput)
		return;
	
	GetPortBounds(GetWindowPort(inputLine.w), &wp);
	
	lRight = 0;
	rLeft = wp.right - 2;
	
	//Go to the end of the object list.
	o = inputLine.objectList;
	while(o->next)
		o=o->next;
	
	//o now contains the last object in the list. this will be all of the right-aligned objects.
	//Scan through and stop when we hit the first left aligned object and give everything the space it wants.
	while(o && o->align==iwRight)
	{
		t=rLeft - 1;
		rLeft -= o->askedWidth + 1;
		
		SetRect(&o->drawArea, rLeft, 0, t, inputLine.statusLineHeight-2);
		o->givenWidth = t - rLeft + 1;
		o=o->prev;
	}
	
	//Now that that's dealt with, we start giving everything the space it wants until we can't go any further.
	//That means that unless someone does somethign really stupid, the only thing we're gonna truncate is the
	//status line, which we can identify as having a creator/type of SIRC/stat.
	o=inputLine.objectList;
	while(o && o->align==iwLeft)
	{
		t = lRight +1;
		if(o->askedWidth==-1) //give them as much as posssible. that is, rLeft - lRight -2 space.
			lRight = rLeft -2;
		else
			lRight += o->askedWidth;
		if(lRight > rLeft) //Houston, we have a problem
		{
			if(o->creator==iwShadowIRCObject && o->type == iwStatusLine)
				lRight = rLeft - 2;
			else // it's a plugin. Give 'em 16 pixels.
				lRight = t + 15;
		}
		SetRect(&o->drawArea, t, 0, lRight, inputLine.statusLineHeight-2);
		
		o->givenWidth = lRight - t;
		o=o->next;
	}
}

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

pascal iwWidgetPtr IWNewWidget(long type, short align, short width)
{
	iwWidgetPtr p;
	iwWidgetPtr prev;
	iwWidgetPtr o;

	if(noFloatingInput)
		return 0;

	o = (iwWidgetPtr)NewPtr(sizeof(iwWidgetRec));
	
	p = inputLine.objectList;
	prev=0;
	if(align==iwLeft) //then tack it onto the begining.
	{
		o->next=p;
		if(p)
			p->prev=o;
		inputLine.objectList=o;
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
			prev->next=o;
		else
			inputLine.objectList=o;
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

	IWRecalculateRects();
	return o;
}

pascal void IWGrow(const EventRecord *e)
{
	Rect r;
	long ii;
	LongRect lr;
	GrafPtr gp;
	WEReference il;
	
	if(noFloatingInput)
		return;
	
	il = ILGetWE();
	
	SetRect(&r, 300, 32, 32767, 32767);
	ii=GrowWindow(inputLine.w, e->where, &r);
	if(ii)
	{
		WGetBBox(inputLine.w, &r);
		r.right=r.left+(ii&0x0000FFFF);
		r.bottom=r.top+*(short*)&ii;
		
		SizeWindow(inputLine.w,(ii&0x0000FFFF), *(short*)&ii, 1);
		mainPrefs->inputLoc=r;
		
		GetPort(&gp);
		SetPortWindowPort(inputLine.w);
		GlobalToLocal((Point*)&r.bottom);
		
		r.left = r.right-14;
		r.top = inputLine.statusLineHeight+2;
		EraseRect(&r);
		
		WEGetDestRect(&lr, il);
		lr.top=inputLine.statusLineHeight+2;
		lr.right=r.right-16;
		lr.bottom=r.bottom;
		
		WESetDestRect(&lr, il);
		WESetViewRect(&lr, il);
		WECalText(il);
		GetPortBounds(GetWindowPort(inputLine.w), &r);
		InvalWindowRect(inputLine.w, &r);
		SetPort(gp);
		IWRecalculateRects();
	}
}

pascal char ILWEIsInput(WEReference we)
{
	long s = 0;
	
	if(!WEGetUserInfo(kInputField, &s, we) && s == kInputField)
		return true;
	else
		return false;
}

pascal void OpenInputLine()
{
	if(!inputLine.w)
	{
		LongRect dr;
		GrafPtr p0;
		short line2;
		Rect sb;
		iwStatusObjectPtr iws;
		char zeroPosition;
		char offscreen;
		ConstStringPtr s;
		Rect wr;

		if(mainPrefs->nonGlobalInput)
		{
			noFloatingInput = 1;
			return;
		}
			
		inputLine._hist=(CharsHandle)NewHandle(1);
		(*inputLine._hist)[0]=0;

		s = GetIntStringPtr(spInfo, sInputline);
		inputLine.w= WCreate(&mainPrefs->inputLoc, s, kWindowFloatGrowProc, 0, 0, true);
		if(inputLine.w)
		{
            // Get rid of the close box
            ChangeWindowAttributes(inputLine.w, NULL, kWindowCloseBoxAttribute);
            
			GetPort(&p0);
			SetPortWindowPort(inputLine.w);
			SetOrigin(-2,-2);
			GetFNum(mainPrefs->defaultFontName, &inputLine.fontnum);
			TextFont(inputLine.fontnum);
			TextSize(mainPrefs->defaultFontSize);
			inputLine.fontsize=mainPrefs->defaultFontSize;
			GetFontInfo(&inputLine.fi);
			inputLine.statusLinePos=inputLine.fi.ascent+inputLine.fi.leading;
			
			inputLine.statusLineHeight=inputLine.statusLinePos+inputLine.fi.descent+3;
			line2=inputLine.statusLineHeight + inputLine.fi.leading + inputLine.fi.ascent;

			//Set the inputline position
			zeroPosition=((mainPrefs->inputLoc.left==0)&&(mainPrefs->inputLoc.right==0));
			offscreen=!RectInRgn(&mainPrefs->inputLoc, GetGrayRgn());
			if(zeroPosition || offscreen)
			{
				BitMap screenBits;
				short h,v;
				GetQDGlobalsScreenBits(&screenBits);
				sb = screenBits.bounds;
				if(zeroPosition)
				{
					h=80* inputLine.fi.widMax+16;
					v=line2 + inputLine.fi.descent + inputLine.fi.leading+10;
				}
				else
				{
					h=mainPrefs->inputLoc.right - mainPrefs->inputLoc.left;
					v=mainPrefs->inputLoc.bottom - mainPrefs->inputLoc.top;
				}
				SizeWindow(inputLine.w, h, v, 1);
				GetPortBounds(GetWindowPort(inputLine.w), &wr);
				MoveWindow(inputLine.w, (sb.right-sb.left-wr.right+2)/2 -1, sb.bottom - wr.bottom-5, false);
                WGetBBox(inputLine.w, &mainPrefs->inputLoc);
				mainPrefs->inputLoc.right=mainPrefs->inputLoc.left+h;
				mainPrefs->inputLoc.bottom=mainPrefs->inputLoc.top+v;
			}

			//Allocate default Inputline objects
			inputLine.status=IWNewWidget(iwStatusLine, iwLeft, -1);
			
			//Now, create the data pointer for the status object
			inputLine.status->data = (long)NewPtr(sizeof(iwStatusObjectRec));
			iws = (iwStatusObjectPtr)inputLine.status->data;
			iws->awayWidth=StringWidth("\p (Away)");
			iws->istalkingwithWidth=StringWidth("\p is talking with ");
			iws->onWidth=StringWidth("\pon ");
			iws->closeParenWid = CharWidth('(');
			
			//Then, setup the WASTE field			
			dr.left=1;
			dr.top=inputLine.statusLineHeight+2;
			dr.right=(mainPrefs->inputLoc.right - mainPrefs->inputLoc.left)-14; //local coord
			dr.bottom=(mainPrefs->inputLoc.bottom - mainPrefs->inputLoc.top) - 5; //local coord
			
			WENew(&dr,&dr, weDoUndo + weDoAutoScroll + weDoDrawOffscreen + weDoMonoStyled + weDoDragAndDrop,&inputLine._il);
			WESetInfo(weRefCon, &inputLine.w, inputLine._il);
			WESetInfo(wePreTrackDragHook, &sPreTrackerUPP, inputLine._il);
			WESetUserInfo(kInputField, kInputField, inputLine._il);
			
			WEActivate(inputLine._il);
			SetPort(p0);
			WSelect(inputLine.w);
			
			iws->maxLineWid=(CharWidth('n') * (22+22+32+1)) + iws->awayWidth + iws->istalkingwithWidth + iws->onWidth;
			
			inputLine.statuslineFlags = kUmodeFlagsOn + kUmodeFlagsPlus + kBoldedPopups;
		}
	}
}

pascal long IWPopUpMenu(Point p, MenuHandle m, long curItem)
{
	long newItem;

	if(noFloatingInput)
		return 0;

	LocalToGlobal(&p);

	SetMenuFont(m, inputLine.fontnum, inputLine.fontsize);

	InsertMenu(m, -1);
	newItem=PopUpMenuSelect(m, p.v, p.h, curItem);
	
	return newItem;
}

inline void IWInternalDraw(iwWidgetPtr o)
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
			char boldedPopups = (inputLine.statuslineFlags & kBoldedPopups) == kBoldedPopups;
			
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
			MoveTo(o->drawArea.left, inputLine.statusLinePos);//o->drawArea.top);
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
			
			if((inputLine.statuslineFlags & kUmodeFlagsOn) && link && link->yourUmodes)
			{
				int x;
				
				pstrcpy("\p [", s);
				if(inputLine.statuslineFlags & kUmodeFlagsPlus)
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

#pragma mark -

pascal void ILAddHistory(MWPtr mw, LongString *line)
{
	int i;
	CharsHandle hist = _ILGetHist(mw);
	
	if(GetHandleSize((Handle)hist)>MAXHIST)
	{
		i=1;
		while((*hist)[i])
			i++;
		
		Munger((Handle)hist, 0, 0, i, (const void*)1, 0);
	}
	
	i=line->len+1;
	if(i>1)
	{
		line->data[i]=0;
		PtrAndHand(&line->data[1], (Handle)hist, i);
		_ILSetHpos(mw, GetHandleSize((Handle)hist)-1);
	}
}

static void RecallLine(WEReference il, CharsHandle hist, int p)
{
	int i;
	LongString ls;
		
	//On 68k ShadowIRC, the il is getting trashed here.
	ILSetHpos(p);
	i=0;
	do
	{
		p++;
		i++;
		ls.data[i]=(*hist)[p];
	} while(ls.data[i]);
	
	ls.len=i-1;
	_ILSetLine(il, &ls);
}

pascal void RecallLineUp(void)
{
	int i;
	WEReference il = ILGetWE();

	if(il)
	{
		CharsHandle hist = ILGetHist();
		
		i = ILGetHpos();
		if(i>0)
		{
			while((*hist)[--i])
				;
			RecallLine(il, hist, i);
		}
	}
}

pascal void RecallLineDown(void)
{
	int i;
	WEReference il = ILGetWE();

	if(il)
	{
		CharsHandle hist = ILGetHist();
		long z = GetHandleSize((Handle)hist);
		
		i=ILGetHpos();
		if(i<z-1)
		{
			while((*hist)[++i])
				;
			
			if(i<z-1)
				RecallLine(il, hist, i);
			else
			{
				int ls = 0;
				_ILSetLine(il, (LongString*)&ls);
			}
		}
	}
}

#pragma mark -

pascal WEReference _ILGetWE(MWPtr mw)
{
	if(!noFloatingInput)
		return inputLine._il;
	else
	{
		if(mw)
			return mw->il;
		else
			return 0;
	}
}

pascal WEReference ILGetWE(void)
{
	if(!noFloatingInput)
		return inputLine._il;
	else
	{
		MWPtr mw = MWFromWindow(FrontNonFloater());
		if(mw)
			return mw->il;
		else
			return 0;
	}
}

static pascal CharsHandle _ILGetHist(MWPtr mw)
{
	if(!noFloatingInput)
		return inputLine._hist;
	else
	{
		if(mw)
			return mw->hist;
		else
			return 0;
	}
}

pascal CharsHandle ILGetHist(void)
{
	return _ILGetHist(MWFromWindow(FrontNonFloater()));
}

static pascal void _ILSetHpos(MWPtr mw, long hp)
{
	if(!noFloatingInput)
		inputLine._hpos = hp;
	else
	{
		if(mw)
			mw->hpos = hp;
	}
}

pascal void ILSetHpos(long hp)
{
	MWPtr mw = MWFromWindow(FrontNonFloater()); //fix a crash in 68k - compiler bug
	_ILSetHpos(mw, hp);
}

static pascal long _ILGetHpos(MWPtr mw)
{
	if(!noFloatingInput)
		return inputLine._hpos;
	else
	{
		if(mw)
			return mw->hpos;
		else
			return 0;
	}
}

pascal long ILGetHpos(void)
{
	return _ILGetHpos(MWFromWindow(FrontNonFloater()));
}

pascal void SetInputLineCursorSelection(long start, long finish)
{
	WESetSelection(start, finish, ILGetWE());
}

pascal void GetInputLineCursorSelection(long *start, long *finish)
{
	WEGetSelection(start, finish, ILGetWE());
}

static pascal char _ILSetLine(WEReference il, LongString *ls)
{
	if(il)
	{
		WEDeactivate(il);
		WESetSelection(0,0x7FFFFFFF, il);
		WEDelete(il);
		_ILInsertLine(il, ls,false);
		WESetSelection(0x7FFFFFFF,0x7FFFFFFF,il);
		WESelView(il);
		WEActivate(il);
		return true;
	}
	else
		return false;
}

pascal char ILSetLine(MWPtr mw, LongString *ls)
{
	if(mw && mw->il)
		return _ILSetLine(mw->il, ls);
	else
		return _ILSetLine(ILGetWE(), ls);
}

pascal void SetInputLine(LongString *ls)
{
	_ILSetLine(ILGetWE(), ls);
}

static pascal char _ILGetLine(WEReference il, LongString *ls)
{
	long i;
	
	if(il)
	{
		i=WEGetTextLength(il);
		if(i>500)
			i=500;
		
		BlockMoveData(*WEGetText(il), &ls->data[1], i);
		ls->len=i;
		return true;
	}
	else
	{
		ls->len = 0;
		return false;
	}
}

pascal char ILGetLine(MWPtr mw, LongString *ls)
{
	if(mw && mw->il)
		return _ILGetLine(mw->il, ls);
	else
		return _ILGetLine(ILGetWE(), ls);
}

pascal void GetInputLine(LongString *line)
{
	_ILGetLine(ILGetWE(), line);
}

static pascal char _ILInsertLine(WEReference il, LongString *ls, char select)
{
	short i;
	long sl, el;
	
	if(il)
	{
		while((i = LSPosChar(13, ls)) != 0)
			LSDelete(ls, i, i);
		
		if(ls->len)
		{
			WEGetSelection(&sl, &el, il);
			WEInsert(&ls->data[1], ls->len, 0, 0, il);
			if(select)
				WESetSelection(sl, sl+ls->len, il);
			WESelView(il);
		}
		return true;
	}
	else
		return false;
}

pascal char ILInsertLine(MWPtr mw, LongString *ls, char select)
{
	if(mw && mw->il)
		return _ILInsertLine(mw->il, ls, select);
	else
		return _ILInsertLine(ILGetWE(), ls, select);
}

#pragma mark -

pascal void IWLock()
{
	if(!noFloatingInput && !inputLine.lock)
	{
		long l = 0;
		inputLine.lock = 1;
		SetInputLine((LongString*)&l);
		WEFeatureFlag(weFReadOnly, weBitSet, ILGetWE());
		UpdateStatusLine();
	}
}

pascal void IWUnlock()
{
	if(!noFloatingInput && inputLine.lock)
	{
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
		WEFeatureFlag(weFReadOnly, weBitClear, ILGetWE());
	}
}

pascal void UpdateStatusLine(void)
{
	static int reentrant = 0;
	GrafPtr p0;
	Rect r;
	iwWidgetPtr o;
	pIWUpdateData pp;
	Ptr offscreen;
	
	if(noFloatingInput)
		return;
	
	if(!IsVisible(inputLine.w))
		return;
	
	if(++reentrant > 1)
		return;
	
	GetPort(&p0);
	
	SetPortWindowPort(inputLine.w);
restart:
	WGetBBox(inputLine.w, &r);
	GlobalToLocal((Point*)&r.bottom);

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
	
	StartDrawingOffscreen(&offscreen, &r, false);
	r.top = r.left = -3;
	r.right ++;
	DrawThemePlacard(&r, kThemeStateActive);
	
	RGBBackColor(&white);
	RGBForeColor(&shadowircColors[sicStatusLine]);
	
	linkfor(o, inputLine.objectList)
	{
		TextFace(0);
		MoveTo(o->drawArea.left, inputLine.statusLinePos);
		if(o->creator==iwShadowIRCObject && !o->pluginRef)
			IWInternalDraw(o);
		else
		{
			pp.widget=o;
			runIndPlugin(o->pluginRef, pIWUpdateMessage, &pp);
		}
		
		if(reentrant>1)
		{
			reentrant=1;
			AbortDrawingOffscreen(offscreen);
			goto restart;
		}
	}

	EndDrawingOffscreen(offscreen);
	
	SetPort(p0);
	reentrant = 0;
}

static pascal void IWStatusLineWidgetClick(iwWidgetPtr o, Point where, short modifiers);
static pascal void IWStatusLineWidgetClick(iwWidgetPtr o, Point where, short modifiers)
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
		newItem=IWPopUpMenu(p, m, ni);
		i=newItem&0xFF;
		if(i>1) //Don't change nick, because the current one is the top one in the list...
		{
			if(i==ni)
			{
				LSStrLS("\p/nick ", &ls);
				SetInputLine(&ls);
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
			
			//Determine active target
			if((MWActive==consoleWin) || !MWActive)
				curItem=0;
			else
			{
				b=0;
				if(MWActive->winType==chanWin)
				{
					for(x=0;x<wmItems.channelsLength;x++)
						if(MWActive == (**wmItems.channels).mw[x])
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
						if(MWActive == (**wmItems.rest).mw[x])
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
			newItem=IWPopUpMenu(p, m, curItem);
		}
		else
			newItem=IWPopUpMenu(p, rcm, CountMenuItems(rcm)-2);

		i = *(((short*)&newItem)+1);
		switch(*(short*)&newItem)
		{
			case 1001: //channel liust
				if(i==1) //console
					ToggleConsoleWindow();
				else if(i<dashPoint) //channel window
					HitWindowMenu(i - 2 + windowsStart);
				else //other window
					HitWindowMenu(windowsStart2+i-dashPoint);
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
		newItem = IWPopUpMenu(p, m, 4 + CurrentTarget.link->linkNum);
		i= *(((short*)&newItem)+1);
		
		switch(*(short*)&newItem)
		{
			case 101:
				if(i)
				{
					i-=4; //link number
					if(CurrentTarget.link->linkNum != i) //do nothing if current link
					{
						MWPtr w = MWActive;
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

pascal void StatusLineClick(Point where, short modifiers, long when)
{
#pragma unused(when)
	iwWidgetPtr o;
	pIWClickData pp;
	
	linkfor(o, inputLine.objectList)
		if(PtInRect(where, &o->drawArea))
		{
			if(o->creator==iwShadowIRCObject && !o->pluginRef)
			{
				if(o->type == iwStatusLine)
					IWStatusLineWidgetClick(o, where, modifiers);
			}
			else
			{
				pp.widget=o;
				pp.where=where;
				runIndPlugin(o->pluginRef, pIWClickMessage, &pp);
			}
			
			break;
		}
 }