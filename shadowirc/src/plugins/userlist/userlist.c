/*
	ShadowIRC Userlist
	Copyright (C) 1997-2000 John Bafford
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

#include <Appearance.h>

#include "userlist.h"
#include "ULList.h"

ShadowIRCDataRecord* sidr;
prefsPtr mainPrefs;
RGBColor *shadowircColors = 0;
char hasAppearance11 = 0;
short genevaNum = 0;
int line = 0;

static short prefsPanel;

static char globalUserlist = 0;

static ULI gUserlist = 0;

const RGBColor white = {-1, -1, -1};
const RGBColor black = {0, 0, 0};
const RGBColor MedGrey = {cMedGrey, cMedGrey, cMedGrey};
const RGBColor MDkGrey = {cMDkGrey, cMDkGrey, cMDkGrey};

#pragma internal on

INLINE ULI ULIFromMW(MWPtr mw);
static pascal ULI ULIFromChannel(channelPtr ch);
static pascal ULI ULIFromWindow(WindowPtr w);

static pascal void ULDoubleClick(ULI ul, long num);
INLINE char IsDoubleClick(long time);

static pascal void ULHitHeader(ULI ul, const EventRecord *e);
static pascal void ULHitList(ULI ul, const EventRecord *e);
INLINE void ULInContentHit(ULI ul, const EventRecord *e);

static pascal void ULScrollbarLive(ULI ul, const EventRecord *e);
static pascal void ULScrollbarActionProc(ControlHandle vscr, short part);
INLINE void ULScrollbarHit(ULI ul, short part, const EventRecord *e);
INLINE void ULGotNamesList(pServiceULNamesEndData *p);
INLINE void ULSActivateWin(pServiceActivateWinData *p);
static pascal char MouseInWindow(ULI ul);

static pascal void ULDragTrack(pUIDragTrackData *p);
static pascal void ULDragReceive(pUIDragReceiveData *p);
INLINE void ULPaneDragTrack(pMWPaneDragTrackData *p);
INLINE void ULPaneDragReceive(pMWPaneDragReceiveData *p);

static pascal void ULContextualMenu(pCMPopupsData *p);
static pascal void ULContextualMenuProcess(pCMPopupsReturnData *p);
static pascal void ProcessShortcuts(pShortcutProcessData *p);

INLINE void IdleCursor(EventRecord *e);
INLINE void CreateGlobalUserlistWindow(void);
INLINE void DestroyGlobalUserlistWindow(void);
INLINE void ULMoveWindow(pUIWindowMoveDataRec *p);
INLINE void ULCloseWindow(pUIWindowCloseDataRec *p);
INLINE void ULWindowMenuSelect(pServiceWindowMenuData *p);
INLINE void ULGrowWindow(pUIInGrowData *p);
static pascal void ULResizeWin(ULI ul, short height, short wid);
INLINE void ULUpdateWin(pUIUpdateData *p);
INLINE void ULMouse(pUIMouseUpDownData *p);

INLINE void ULNewMessageWindow(pMWNewData *p);
static pascal void SULPaneUpdate(pMWPaneUpdateData *p);
static pascal void SULPaneDestroy(pMWPaneDestroyData *p);
static pascal void ULPaneResize(ULI ul, mwPanePtr o, const EventRecord *e);
static pascal void SULPaneClick(pMWPaneClickData *p);
INLINE void SULPaneActivate(pMWPaneActivateData *p);
INLINE void SULPaneResize(pMWPaneResizeData *p);

INLINE void SJoin(pServerJOINDataRec *p);
INLINE void SPart(pServerPARTDataRec *p);
INLINE void SMode(pServerMODEData *p);
INLINE void SQuit(pServerQUITDataRec *p);
INLINE void SNick(pServerNICKDataRec *p);
INLINE void SKick(pServerKICKDataRec *p);
INLINE void STrashChannel(pServiceULTrashChannelData *p);
INLINE void SULUserHosts(pServiceULUserhostsData *p);
INLINE void SIdle(pIdleMessageData *p);

static pascal ULI ULINew(WindowPtr w, long type);
static pascal void ULIDestroy(ULI ul);
static pascal void ULIPaneDrawBorder(ULI ul, mwPanePtr pane, char pressed);
static pascal void ULIKillAll(void);
static pascal void ULIAddAll(void);

INLINE void PWSet(pPWSetWindowData *p);
INLINE void PWGet(pPWGetWindowData  *p);
INLINE void PWHit(pPWItemHitData *p);
INLINE void PWClosed(void);

static pascal void displayOldVersionMsg(void);
static pascal void displayMultipleUserlistsMsg(void);
INLINE void setupMessages(char captureMessages[numMessages]);

INLINE ULI ULIFromMW(MWPtr mw)
{
	if(mw)
	{
		mwPanePtr o = MWFindPane(mw, kUserlistPane);
		if(o)
			return (ULI)o->data;
	}
	
	return 0;
}

static pascal ULI ULIFromChannel(channelPtr ch)
{
	if(globalUserlist) //global userlist
		return gUserlist;
	else //in-window userlist
	{
		if(!ch)
			return 0;
		else	
			return ULIFromMW(ch->window);
	}
}

static pascal ULI ULIFromWindow(WindowPtr w)
{
	ULI ul = (ULI)pluginGetWRefCon(w);
	MWPtr mw;
	
	if(ul)
	{
		if(ul->ulType == ulWindowRefCon) //global userlist
			return ul;
		else //not global list
		{
			mw = MWFromWindow(w);
			if(mw && mw->winType == chanWin)
				return ULIFromMW(mw);
		}
	}
	
	return 0;
}

#pragma mark -

INLINE char IsDoubleClick(long time)
{
	return TickCount() - time <= GetDblTime();
}

static pascal void ULDoubleClick(ULI ul, long num)
{
	LongString ls;
	
	//whois user
	if(num >=0 && ul->users && num<(**ul->users).num)
	{
		LSConcatStrAndStr("\pWHOIS ", (**ul->users).u[num].u->nick, &ls);
		HandleCommand((**ul->users).ch->link, &ls);
	}
}

static pascal void ULHitHeader(ULI ul, const EventRecord *e)
{
	if((ul->scrollbarLeft && e->where.h <= scrollbarWidth) || (!ul->scrollbarLeft && e->where.h > ul->uwinSize.right-scrollbarWidth))
	{
		ul->sortReverse = !ul->sortReverse;
		if(globalUserlist || e->modifiers & shiftKey)
			mainPrefs->userlistSortReverse = ul->sortReverse;
		ListHeader(ul);
		ListSort(ul, false);
		ListDraw(ul);
	}
	else //it's not a hit on the flip widget
	{
		//see if it's the divider
		short divider = ul->nickListWidth - 4;

		if(ul->scrollbarLeft)
			divider+=scrollbarWidth;
		if(ul->rightSide)
			divider+=kInWindowBorder;
		
		if(e->where.h >= divider - 3 && e->where.h <= divider + 3)
		{
			Point pt;
			short loc = e->where.h;
			Rect r = ul->uwinSize;
			short min;
			
			if(ul->scrollbarLeft)
				min = 55 + scrollbarWidth;
			else
				min = 55;

			r.right -= 10;
			while(StillDown())
			{
				GetMouse(&pt);
				if(ul->ulType == ulMessageWindow)
				{
					pt.h-=ul->pane->drawArea.left + (ul->rightSide?kInWindowBorder:0) + (ul->scrollbarLeft?scrollbarWidth:0);
					pt.v-=ul->pane->drawArea.top;
				}
				else if(ul->ulType==ulGlobal && ul->scrollbarLeft)
					pt.h-=scrollbarWidth;
				if(pt.h > min && pt.v > -20 && pt.h < r.right - 20 && pt.v < r.bottom)
				{
					if(pt.h != ul->nickListWidth)
					{
						ul->nickListWidth = pt.h +4;
						if(e->modifiers & shiftKey)
							mainPrefs->userlistNickWidth  = ul->nickListWidth;
						ListHeader(ul);
						ListDraw(ul);
					}
				}
			}
		}
		else if(e->where.h < divider) //nick
		{
			if(ul->sort != kSortNick)
			{
				ul->sort = kSortNick;
				if(globalUserlist || e->modifiers & shiftKey)
					mainPrefs->userlistSort = ul->sort;
				ListSort(ul, false);
				ListHeader(ul);
				ListDraw(ul);
			}
		}
		else //host
		{
			if(ul->sort != kSortUserhost)
			{
				ul->sort = kSortUserhost;
				if(globalUserlist || e->modifiers & shiftKey)
					mainPrefs->userlistSort = ul->sort;
				ListSort(ul, false);
				ListHeader(ul);
				ListDraw(ul);
			}
		}
	}
}

pascal void DragOneUser(ULI ul, int u, const EventRecord *e);
pascal void DragOneUser(ULI ul, int u, const EventRecord *e)
{
#pragma unused(ul, u, e)
/*
	DragReference drag;
	Rect dragBounds;
	RgnHandle dragOutline;
	Point pt;
	int num;
	OSErr err;
		
	
	pt = e->where;
	LocalToGlobal(&e->where);

	if(WaitMouseMoved(e->where))
	{
		if(!NewDrag(&drag))
		{
			DragAddPtr(drag, (ItemReference)ul, kUserFlavor, (**ul->users).u[u].u, flavorSenderOnly);
			
			num = u - GetControlValue(ul->bar);
			dragBounds.top = kTopOffset + (u*line);
			dragBounds.left = 1;
			dragBounds.bottom = dragBounds.top + line;
 			dragBounds.right = ul->uwinSize.right - (kRightOffset -2);
 			
			if(dragBounds.bottom >= ul->uwinSize.bottom)
				dragBounds.bottom = ul->uwinSize.bottom - 1;

			if(num>0 && (**ul->users).u[num-1].u->userlistIsSelected)
				dragBounds.top++;
			
			LocalToGlobal((Point*)&dragBounds.top);
			LocalToGlobal((Point*)&dragBounds.bottom);

			SetRect(&dragBounds, 50, 50, 100, 100);
			SetDragItemBounds(drag, (ItemReference)ul, &dragBounds);
			
			RectRgn(dragOutline, &dragBounds);
			{
				RgnHandle outlineRgn;
				
				CopyRgn ( dragOutline, outlineRgn ) ;
				InsetRgn ( outlineRgn, 1, 1 ) ;
				DiffRgn ( dragOutline, outlineRgn, dragOutline) ;
				DisposeRgn(outlineRgn);
			}
			
			err = TrackDrag(drag, e, dragOutline);
			
			DisposeRgn(dragOutline);
			DisposeDrag(drag);
			
			if(err)
			{
				DebugStr("\p");
			}
		}
	}
	
	e->where = pt;
*/
}

static pascal void ULHitList(ULI ul, const EventRecord *e)
{
	static int clickAnchor = 0;
	
	int x, p;
	ulPtr us;
	int cnt;
	int top;
	char dblClick = 0;
	
	HLock((Handle)ul->users);
	us = *ul->users;
	//Select cell
	//Figure out what line was hit
	top = GetControlValue(ul->bar);
	//Do bot...
	p = (e->where.v - kTopOffset - 2) / line + top;
	
	if(p>=us->num) //click in empty space, deselect everything unless shift/cmd down
	{
		if(!(e->modifiers & cmdKey) && !(e->modifiers & shiftKey))
		{
			cnt = 0;
			for(p=0;p<us->num;p++)
			{
				us->u[p].u->userlistIsSelected = false;
				if(p>=top)
					cnt++;
			}
			
			if(cnt)
				ListDraw(ul); //Pretty inefficient if only a few users are selected...
			
			goto finished;
		}
	}
	else //click on cell. Do Something.
	{
		//p is the cell that was selected. Toggle it's selection and redraw.
		//if the shift key is down, we have to do range checking.
		
		if(e->modifiers & shiftKey)
		{
			for(cnt=0,x=0;x<us->num;x++)
				if(us->u[x].u->userlistIsSelected)
					cnt++;
			
			//If no cells are selected, select this and do nothing else.
			if(!cnt)
			{
				us->u[p].u->userlistIsSelected = true;
				ListDrawOne(ul, p);
				clickAnchor = p;
			}
			else if(cnt == 1) //If one cell is selected extend the range.
			{
				for(x=0;x<us->num;x++)
					if(us->u[x].u->userlistIsSelected)
						break;
				//x is now the selected cell.
				
				if(x>p)
				{
					top = p;
					cnt = x;
				}
				else if(p>x)
				{
					top = x;
					cnt = p;
				}
				else //p==x
				{
					us->u[p].u->userlistIsSelected = false;
					ListDrawOne(ul, p);
					goto finished;
				}
				
				for(; top <= cnt; top++)
					us->u[top].u->userlistIsSelected = true;
				ListDraw(ul);
			}
			else
			{
				//Select everything between clickAnchor and p, deselect everything else.
				
				if(clickAnchor > p)
				{
					top = p;
					cnt = clickAnchor;
				}
				else
				{
					top = clickAnchor;
					cnt = p;
				}
				
				for(x=0;x<us->num;x++)
					us->u[x].u->userlistIsSelected = (x>=top && x<=cnt);
				
				ListDraw(ul);
				
			}
		}
		else //not shift key
		{
			if(!us->u[p].u->userlistIsSelected)
			{
				us->u[p].u->userlistIsSelected = true;
				clickAnchor = p;
				ListDrawOne(ul, p);
				
				if(sidr->hasDrag)
				{
					DragOneUser(ul, p, e);
				}
			}
			else //if it's selected
			{
				if(e->modifiers & cmdKey)
				{
					us->u[p].u->userlistIsSelected = false;
					ListDrawOne(ul, p);
					goto finished;
				}
				else //handle double clicks
				{
					if(IsDoubleClick(ul->lastClickTime))
					{
						dblClick = 1;
						ULDoubleClick(ul, p);
					}
					goto finished;
				}
			}
		}
	}
	
	//If not shift or command key, deselect other cells
	if(!(e->modifiers & cmdKey) && !(e->modifiers & shiftKey))
	{
		cnt = 0;
		for(x=0;x<us->num;x++)
			if(x!=p)
			{
				us->u[x].u->userlistIsSelected = false;
				if(x>=top)
					cnt++;
			}
			
		if(cnt)
			ListDraw(ul);
	}
	
finished:
	if(!dblClick)
	{
		ul->lastClick = p;
		ul->lastClickTime = TickCount();
	}
	else
		ul->lastClick = ul->lastClickTime = 0;
	
	HUnlock((Handle)ul->users);
}

INLINE void ULInContentHit(ULI ul, const EventRecord *e)
{
	//The GrafPort is set and e->where is local
	//Determine where clicked.
	
	if(e->where.v <= kHeaderSize)
		ULHitHeader(ul, e);
	else
		if(ul->users)
			ULHitList(ul, e);
	
	HUnlock((Handle)ul->users);
}

#pragma mark -

static ULI gULILiveScrollbar;

static pascal void ULScrollbarActionProc(ControlHandle bar, short part)
{
	short val;
	
	switch(part)
	{
		case kControlUpButtonPart:
			val = GetControlValue(bar);
			if(val > 0)
			{
				SetControlValue(bar, val-1);
				ListDraw(gULILiveScrollbar);
			}
			break;
			
		case kControlDownButtonPart:
			val = GetControlValue(bar);
			if(val < GetControlMaximum(bar))
			{
				SetControlValue(bar, val+1);
				ListDraw(gULILiveScrollbar);
			}
			break;

		case kControlPageUpPart:
			val = GetControlValue(bar);
			if(val > 0)
			{
				val -= gULILiveScrollbar->visLines;
				if(val < 0)
					val = 0;
				SetControlValue(bar, val);
				ListDraw(gULILiveScrollbar);
			}
			break;
		
		case kControlPageDownPart:
			val = GetControlValue(bar);
			if(val < GetControlMaximum(bar))
			{
				val += gULILiveScrollbar->visLines;
				SetControlValue(bar, val);
				ListDraw(gULILiveScrollbar);
			}
			break;
	}
}

static pascal void ULScrollbarLive(ULI ul, const EventRecord *e)
{
	IndicatorDragConstraint constraint;
	Point mouse;
	long initial, old, cur, max;
	short range, delta;

	HiliteControl(ul->bar, kControlIndicatorPart);

	*(Point*)&constraint.limitRect = e->where;
	MySendControlMessage(ul->bar, thumbCntl, (long)&constraint);
	
	range = constraint.limitRect.bottom - constraint.limitRect.top;
	
	initial=old=cur=GetControlValue(ul->bar);
	max=GetControlMaximum(ul->bar);
	
	while(StillDown())
	{
		GetMouse(&mouse);
		if(PtInRect(mouse, &constraint.slopRect))
		{
			delta=mouse.v - e->where.v;
			
			cur = initial + (float)max * ((float)delta/(float)range);
			if(cur<0)
				cur=0;
			if(cur>max)
				cur=max;
		}
		
		if(cur != old)
		{
			SetControlValue(ul->bar, cur);
			ListDraw(ul);
			old=cur;
		}
	}

	HiliteControl(ul->bar, kControlNoPart);
}

INLINE void ULScrollbarHit(ULI ul, short part, const EventRecord *e)
{
	#pragma unused(e)
	ControlActionUPP upp;
	
	gULILiveScrollbar = ul;
	switch(part)
	{
		case kControlUpButtonPart:
		case kControlDownButtonPart:
		case kControlPageUpPart:
		case kControlPageDownPart:
			upp = NewControlActionProc(ULScrollbarActionProc);
			TrackControl(ul->bar, e->where, upp);
			DisposeRoutineDescriptor(upp);
			break;
		
		case kControlIndicatorPart:
			ULScrollbarLive(ul, e);
			break;
	}
	gULILiveScrollbar=0;
}

INLINE void ULGotNamesList(pServiceULNamesEndData *p)
{
	ULI ul = ULIFromChannel(p->channel);

	if(ul && (!ul->users || (**ul->users).ch == p->channel))
		ListGenerate(ul, p->channel);
}

INLINE void ULSActivateWin(pServiceActivateWinData *p)
{
	static WindowPtr lastActivate = 0;
	ULI ul = ULIFromChannel(p->ch);
	
	if(p->activate)
	{
		if(p->w == lastActivate)
			return;

		lastActivate = p->w;
		
		if(globalUserlist)
		{
			if(p->ch && p->ch->active) //valid channel
			{
				ListGenerate(ul, p->ch);
				ListSetWTitle(ul);
			}
			else if(ul)
			{
				ListTrash(ul);
				ListSetWTitle(ul);
				ListDraw(ul);
			}
		}
	}
	else //deactivate
	{
	}

	if(!globalUserlist)
	{
		int i = WMSGetMenuItemNum(userlistServiceType);
		if(i>0)
		{
			CheckItem(GetMenuHandle(260), i, ul != 0);
			if(p->mw && p->mw->winType == chanWin)
				EnableItem(GetMenuHandle(260), i);
			else
				DisableItem(GetMenuHandle(260), i);
		}
	}
}

static pascal char MouseInWindow(ULI ul)
{
	GrafPtr gp;
	Point pt;
	
	GetPort(&gp);
	SetPort(ul->uwin);
	GetMouse(&pt);
	SetPort(gp);
	return WIsVisible(ul->uwin) && PtInRect(pt, &ul->uwinSize);
}

#pragma mark -

INLINE void SJoin(pServerJOINDataRec *p)
{
	ULI ul = ULIFromChannel(p->channel);
	
	if(ul && ul->users && p->channel == (**ul->users).ch)
	{
		ListAdd(ul, p->userPtr);
		
		if(MouseInWindow(ul))
			ListDraw(ul);
		else
		{
			if(TickCount() - ul->lastUpdate < 30)
				ul->updateList = 1;
			else
				ListDraw(ul);
		}
	}
}

INLINE void SPart(pServerPARTDataRec *p)
{
	ULI ul = ULIFromChannel(p->channel);
	
	if(ul && ul->users && p->channel == (**ul->users).ch)
	{
		ListDel(ul, p->userPtr);

		if(MouseInWindow(ul))
			ListDraw(ul);
		else
		{
			if(TickCount() - ul->lastUpdate < 30)
				ul->updateList = 1;
			else
				ListDraw(ul);
		}
	}
}

INLINE void SMode(pServerMODEData *p)
{
	ULI ul = ULIFromChannel(p->ch);

	if(ul && ul->users && p->ch && p->ch == (**ul->users).ch && !p->done && !ul->updateList)
	{
		//It's a mode change in my channel
		if(p->mode == 'v' || p->mode == 'o' || p->mode == 'h')
		{
			if(p->data)
				ListDrawOneUser(ul, (UserListPtr)p->data);
		}
	}
}

INLINE void SQuit(pServerQUITDataRec *p)
{
	//Find the userlist that applies. This means, the global one, or the one in all the channels in the link
	ULI ul;
	long l;
	channelPtr ch;
	
	if(globalUserlist)
		ul = gUserlist;
	else
	{
		ch = p->link->channelList;
		if(!ch)
			return;
		ul = ULIFromChannel(ch);
	}
	
	l = TickCount();
	do {
		if(ul && ul->users)
		{
			if(ListDelName(ul, p->username))
			{
				if(MouseInWindow(ul))
					ListDraw(ul);
				else
				{
					if(l - ul->lastUpdate < 30)
						ul->updateList = 1;
					else
						ListDraw(ul);
				}
			}
		}
		
		if(globalUserlist)
			return;
		else
		{
			ch=ch->next;
			if(ch)
				ul = ULIFromChannel(ch);
			else
				return;
		}
	} while (1);
}

INLINE void SNick(pServerNICKDataRec *p)
{
	//Find the userlist that applies. In this case, it might be more than one, so this may need to be rewritten.
	ULI ul;
	UserListPtr u;
	int num;
	channelPtr ch;
	short sort;
	
	if(globalUserlist)
		ul = gUserlist;
	else
	{
		ch = p->link->channelList;
		if(!ch)
			return;
		ul = ULIFromChannel(ch);
	}
	
	while(1)
	{
		if(ul && ul->users && (**ul->users).ch)
		{
			u = ULFindUserName((**ul->users).ch, p->newnick);
			if(u)
			{
				sort = ul->sort;
				ul->sort = kShitSort;
				num = ListFind(ul, u);
				ul->sort = sort;
				if(num != -1)
				{
					ListResortInd(ul, num);
					ListDraw(ul);
				}
			}
		}

		if(globalUserlist)
			return;
		else
		{
			ch=ch->next;
			if(ch)
				ul = ULIFromChannel(ch);
			else
				return;
		}
	}
}

INLINE void SKick(pServerKICKDataRec *p)
{
	ULI ul = ULIFromChannel(p->channel);

	if(ul && ul->users && p->channel == (**ul->users).ch)
	{
		if(p->isMe)
		{
			ListTrash(ul);
			ListDraw(ul);
		}
		else
		{
			ListDelName(ul, p->kickedNick);

			if(MouseInWindow(ul))
				ListDraw(ul);
			else
			{
				if(TickCount() - ul->lastUpdate < 30)
					ul->updateList = 1;
				else
					ListDraw(ul);
			}
		}
	}
}

INLINE void STrashChannel(pServiceULTrashChannelData *p)
{
	ULI ul = ULIFromChannel(p->channel);

	if(ul && ul->users && p->channel == (**ul->users).ch)
	{
		ListTrash(ul);
		ListDraw(ul);
		ListSetWTitle(ul);
	}
}

INLINE void SULUserHosts(pServiceULUserhostsData *p)
{
	if(globalUserlist)
	{
		if((gUserlist->users) && (**gUserlist->users).ch->link == p->link)
		{
			if(gUserlist->sort == kSortUserhost)
				ListSort(gUserlist, false);
			ListDraw(gUserlist);
		}
	}
	else
	{
		channelPtr ch = p->link->channelList;
		ULI ul;
		
		while(ch)
		{
			ul = ULIFromChannel(ch);
			if(ul)
			{
				if(ul->sort == kSortUserhost)
					ListSort(ul, false);
				ListDraw(ul);
			}
			ch=ch->next;
		}
	}
}

#pragma mark -

static int hiliteUser = -1;

static pascal void ULDragTrack(pUIDragTrackData *p)
{
	int top, loc;
	int mouseOver;
	Point pt;
	Rect r;
	ULI ul = ULIFromWindow(p->window);
	RGBColor rgb;
	GrafPtr gp;
	
	if(ul) //found a userlist
	{
		if(ul->users && DragIsTypeAvail(p->drag, flavorTypeHFS))
		{
			gp = ULISetupDrawing(ul);
			GetBackColor(&rgb);
			
			top = GetControlValue(ul->bar);
			GetMouse(&pt);
			//loc = (pt.v - kTopOffset - (kTopOffset - line)) / line;
			loc = 1 + ((pt.v - kTopOffset - kTopOffset) / line);
			mouseOver = top + loc;
			
			r.top = kTopOffset + 1 + (loc * line);
			r.left = 1;
			r.bottom =  r.top + line;
			r.right = ul->uwinSize.right - (kRightOffset -2);

			//Hilite the relevant user in the userlist.
			//If the user moves the mouse, move the focus ring
			if(p->message == kDragTrackingEnterWindow)
			{
				if(loc < (**ul->users).num && loc >= 0)
				{
					hiliteUser = mouseOver;
					RGBBackColor(&white);
					DragHilightRectBG(&r, p->drag, true);
				}
				else
					hiliteUser = -1;
			}
			else if(p->message == kDragTrackingLeaveWindow)
			{
				//erase box
				if(hiliteUser != -1)
				{
					HideDragHilite(p->drag);
					ListDrawOne(ul, hiliteUser);
					hiliteUser = -1;
				}
			}
			else if(p->message == kDragTrackingInWindow)
			{
				if(hiliteUser != mouseOver)
				{
					if(hiliteUser != -1)
					{
						//erase box
						HideDragHilite(p->drag);
						ListDrawOne(ul, hiliteUser);
					}
					
					ULIFinishDrawing(ul, gp);
					gp = ULISetupDrawing(ul);

					if(loc < (**ul->users).num && loc >= 0)
					{
						//draw box
						RGBBackColor(&white);
						DragHilightRectBG(&r, p->drag, true);
						hiliteUser = mouseOver;
					}
					else
						hiliteUser = -1;
				}
			}

			RGBBackColor(&rgb);
			ULIFinishDrawing(ul, gp);
		}
	}
}

static pascal void ULDragReceive(pUIDragReceiveData *p)
{
	ULI ul = ULIFromWindow(p->window);

	p->returnVal = dragNotAcceptedErr;
	if(ul)
	{
		if(hiliteUser != -1 && ul->users && DragIsTypeAvail(p->drag, flavorTypeHFS))
		{
			HFSFlavor *hf;
			FSSpec f;
			Str255 s;
			unsigned short items, i;
			ItemReference item;
			FlavorFlags flags;
			long siz;
			linkPtr l;
			
			pstrcpy((**ul->users).u[hiliteUser].u->nick, s);

			CountDragItems(p->drag, &items);
			for(i=1;i<=items;i++)
			{
				GetDragItemReferenceNumber(p->drag, i, &item);
				
				if(!GetFlavorFlags(p->drag, item, flavorTypeHFS, &flags))
				{
					GetFlavorDataSize(p->drag, item, flavorTypeHFS, &siz);
					hf = (HFSFlavor*)NewPtr(siz);
					GetFlavorData(p->drag, item, flavorTypeHFS, hf, &siz, 0);
					
					f = hf->fileSpec;
					if(ul->mw && ul->mw->link)
						l = ul->mw->link;
					else
						l = sidr->CurrentTarget->link;
					DoDCCSendFile(l, s, &f, true, false);

					DisposePtr((Ptr)hf);
					p->returnVal = 0;
				}
			}
		}
	}
}

INLINE void ULPaneDragTrack(pMWPaneDragTrackData *p)
{
	pUIDragTrackData p2;
	
	p2.window = p->pane->mw->w;
	p2.drag = p->drag;
	p2.message = p->message;
	
	ULDragTrack(&p2);
}

INLINE void ULPaneDragReceive(pMWPaneDragReceiveData *p)
{
	pUIDragReceiveData p2;
	
	p2.window = p->pane->mw->w;
	p2.drag = p->drag;
	
	ULDragReceive(&p2);
	
	p->received = (p2.returnVal == 0);
}

#pragma mark -

static pascal void ULContextualMenu(pCMPopupsData *p)
{
	//Find the userlist that applies.
	ULI ul = ULIFromWindow(p->w);
	int top, num, x;
	int cnt;
	ulPtr u;
	GrafPtr gp;
	Point pt;
	char unsel;
	int offset;
	
	if(!ul || !ul->users)
		return;
	
	if(ul->ulType == ulMessageWindow && (!p->pane || p->pane != ul->pane))
		return;
	
	GetPort(&gp);
	SetPort(ul->uwin);
	
	top = GetControlValue(ul->bar);

	//num is the thing clicked on.
	pt = p->where;
	GlobalToLocal(&pt);
	if(ul->ulType == kUserlistPane)
		offset = ul->pane->drawArea.top;
	else
		offset = 0;
	num = (pt.v - kTopOffset - offset) / line + top;
	
	HLock((Handle)ul->users);
	u = *ul->users;
	
	if(num < u->num)
	{
		for(cnt=0,x=0;x<u->num;x++)
			if(u->u[x].u->userlistIsSelected)
				cnt++;
		
		//if it's not selected and there's no other selection, hilight it, perform the operation, and deselect it.
		//else perform operation on selection
		
		if(cnt==0 && !u->u[num].u->userlistIsSelected && num<u->num) //select
		{
			unsel = 1;
			u->u[num].u->userlistIsSelected = true;
			ListDrawOne(ul, num);
		}
		else
			unsel = 0;
		
		//Create the menu. This is going to look suspiciously like what the popups plugin does.
		
	}
	
	HUnlock((Handle)ul->users);
	SetPort(gp);
}

static pascal void ULContextualMenuProcess(pCMPopupsReturnData *p)
{
	p=p;
}

inline void RepeatMunger(Handle text, long offset, const void* ptr1, long len1, const void* ptr2, long len2)
{
	int i;
	do {
		i = Munger(text, offset, ptr1, len1, ptr2, len2);
	} while(i >= 0);
}

static pascal void ProcessShortcuts(pShortcutProcessData *p)
{
	ConstStringPtr s;
	int x;
	int n;
	ulPtr u;
	ULI ul;
	UserListPtr user;
	
	if(globalUserlist)
		ul = gUserlist;
	else
		ul = ULIFromMW(*sidr->MWActive);
	
	if(!ul)
		return;
	
	if(ul->users)
	{
		//Get first selected user
		HLock((Handle)ul->users);
		u = *ul->users;
		
		n = u->num;
		for(x=0;x<n;x++)
			if(u->u[x].u->userlistIsSelected)
				break;
			
		if(x==n) //no selection
			user = 0;
		else
			user = u->u[x].u;
	}
	else
		user = 0;
	
	if(user)
		s = user->nick;
	else
		s = "\p";
	
	RepeatMunger(p->textH, 0, "$selecteduser", 13, &s[1], s[0]);
	RepeatMunger(p->textH, 0, "$selectednick", 13, &s[1], s[0]);
	
	if(user)
		s = user->userhost;
	
	RepeatMunger(p->textH, 0, "$selecteduserhost", 17, &s[1], s[0]);
	
/*
	do {
		x = Munger(p->textH, 0, "$selecteduname", 13, &s[1], s[0]);
	} while(x >= 0);
	
	do {
		x = Munger(p->textH, 0, "$selectedhost", 13, &s[1], s[0]);
	} while(x >= 0);

	
	do {
		x = Munger(p->textH, 0, "$selectedmask", 13, &s[1], s[0]);
	} while(x >= 0);
*/
	
	if(ul->users)
		HUnlock((Handle)ul->users);
}

#pragma mark -

static pascal void ULIDestroy(ULI ul)
{
	if(ul)
	{
		ListTrash(ul);
	
		if(ul->ulType == ulGlobal)
			pluginDisposeWindow(ul->uwin);
		else //ulMessageWindow
		{
			if(ul->bar)
			{
				DisposeControl(ul->bar);
				ul->bar = 0;
			}
		}

		DisposePtr((Ptr)ul);
	}
}

static pascal ULI ULINew(WindowPtr w, long type)
{
	ULI ul = (ULI)NewPtrClear(sizeof(UserListInstance));
	short ctrlProc;
	Rect r; //pos for scrollbar
	GrafPtr gp;
	FontInfo genevaInfo;
	MWPtr mw;
	
	if(mainPrefs->userlistNickWidth < 10)
		mainPrefs->userlistNickWidth = 80;
		
	ul->ulType = type;
	ul->uwin = w;
	ul->sortReverse = mainPrefs->userlistSortReverse;
	ul->sort = mainPrefs->userlistSort;
	ul->nickListWidth = mainPrefs->userlistNickWidth;
	ul->scrollbarLeft = mainPrefs->userlistScrollbarLeft;
	ul->updateList = false;
	
	ctrlProc = kControlScrollBarProc;

	ul->uwinSize.top = ul->uwinSize.left = 0;
	if(type == ulGlobal)
	{
		if(EmptyRect(&mainPrefs->userListRect) || //the rect is empty. Set a new one
			!RectInRgn(&mainPrefs->userListRect, GetGrayRgn())) //then it's displaying offscreen.
			SetRect(&mainPrefs->userListRect, 40, 40, 300, 300);
		
		ul->uwin = pluginNewWindow(&mainPrefs->userListRect, "\pUserlist", -1, pnwHasCloseBox | pnwHasGrowBox | pnwFloaterWindow | pnwFIsFloater);
		pluginSetWRefCon(ul->uwin, (long)ul);
		
		ul->uwinSize.right = mainPrefs->userListRect.right - mainPrefs->userListRect.left;
		ul->uwinSize.bottom =  mainPrefs->userListRect.bottom - mainPrefs->userListRect.top;

		ul->rightSide = false;
		r.top = kHeaderSize;
		if(ul->scrollbarLeft)
		{
			r.left = -1;
			r.right = scrollbarWidth - 1;
			r.bottom = (mainPrefs->userListRect.bottom - mainPrefs->userListRect.top) + 1;
		}
		else
		{
			r.left = (mainPrefs->userListRect.right - mainPrefs->userListRect.left) -14;
			r.right = r.left + scrollbarWidth;
			r.bottom = (mainPrefs->userListRect.bottom - mainPrefs->userListRect.top) - 13;
		}
		
		if(mainPrefs->userListOpen)
			WShow(ul->uwin);
		else
			SendBehind(ul->uwin, 0); //Throw it behind everything else so it doesn't pop up at fg/bg switch if hidden.
		
		ul->mw = 0;
		ul->pane = 0;
	}
	else if(type == ulMessageWindow)
	{
		mwPanePtr o;
		short align;
		
		ul->rightSide = mainPrefs->userlistInWindowRight;
		
		ul->mw = mw = MWFromWindow(w);
		if(mainPrefs->userlistInWindowWidth < kMinimumPaneWidth)
			mainPrefs->userlistInWindowWidth = 100;
		
		if(ul->rightSide)
			align = mwPaneRight;
		else
			align = mwPaneLeft;
		ul->pane = o = MWNewPane(mw, kUserlistPane, align, -1, mainPrefs->userlistInWindowWidth + kInWindowBorder);
		
		if(!o)
			return (ULI)MWFindPane(mw, kUserlistPane)->data;
			
		o->data = (long)ul;
		
		ul->uwinSize.right = o->drawArea.right - o->drawArea.left - kInWindowBorder;
		ul->uwinSize.bottom = o->drawArea.bottom - o->drawArea.top;
		
		r.top = kHeaderSize + o->drawArea.top;
		r.bottom = o->drawArea.bottom + 1;
		
		if(ul->scrollbarLeft)
		{
			r.left= o->drawArea.left -1;
			if(ul->rightSide)
				r.left+= kInWindowBorder+1;
			r.right = r.left + scrollbarWidth;
		}
		else
		{
			if(ul->rightSide)
				r.left = o->drawArea.right - 15;
			else
				r.left = o->drawArea.right - 15 + kInWindowBorder;
			r.right = r.left + scrollbarWidth;
		}
	}

	ul->bar=NewControl(ul->uwin, &r, "\p", true, 0, 0, 0, ctrlProc, 0);
	if(type != ulGlobal && !WIsActive(ul->uwin))
		DeactivateControl(ul->bar);
	
	GetPort(&gp);
	SetPort(ul->uwin);

	if(!line)
	{
		GetFNum("\pGeneva", &genevaNum);
		TextFont(genevaNum);
		TextSize(9);
		GetFontInfo(&genevaInfo);
		line = genevaInfo.ascent + genevaInfo.descent + genevaInfo.leading;
	}
	else if(type == ulGlobal)
	{
		TextFont(genevaNum);
		TextSize(9);
	}	
	
	SetPort(gp);

	if(type == ulMessageWindow)
	{
		MWPaneResize(mw);
		if(WIsVisible(mw->w))
			MWPaneUpdate(mw);
	}
	
	return ul;
}

static pascal void ULIPaneDrawBorder(ULI ul, mwPanePtr o, char pressed)
{
	if(ul->ulType == ulMessageWindow && o && WIsVisible(ul->uwin))
	{
		Rect r;
		long state;
		RGBColor front;

		GetForeColor(&front);
		if(WIsActive(ul->uwin))
			RGBForeColor(&black);
		else
			RGBForeColor(&MDkGrey);

		//Draw divider
		r = o->drawArea;
		
		if(ul->rightSide)
		{
			r.left = o->drawArea.left;
			r.right = r.left + kInWindowBorder;
		}
		else
		{
			r.right = o->drawArea.right + 1;
			r.left = r.right - kInWindowBorder;
		}

		if(pressed)
			state = kThemeStatePressed;
		else if(WIsActive(ul->uwin))
			state = kThemeStateActive;
		else
			state = kThemeStateInactive;
		
		DrawBorder(&r, state, true);
		WValidRect(ul->uwin, &r);

		//Draw border
		r = o->drawArea;
		
		r.top--;
		if(o->drawArea.top != 0)
		{
			MoveTo(r.left, r.top);
			LineTo(r.right, r.top);
		}
		
/* ¥¥¥ */
		if(ul->scrollbarLeft)
		{
			if(ul->rightSide)
			{
				r.left += kInWindowBorder;
				r.right = r.left + scrollbarWidth - 1;
			}
			else
			{
				r.right -= kInWindowBorder;
				r.left--;
			}
		}
		else
		{
			if(ul->rightSide)
			{
				r.left += kInWindowBorder;
			}
			else
			{
//¥¥¥FIXME!!
				r.right -= kInWindowBorder;
				r.left--;
			}
		}

		
		MoveTo(r.right, r.top);
		LineTo(r.right, r.bottom);
		LineTo(r.left, r.bottom);
		LineTo(r.left, r.top);
		LineTo(r.right, r.top);
		WValidRect(ul->uwin, &r);
/* ¥¥¥ */
	}
}

INLINE void ULNewMessageWindow(pMWNewData *p)
{
	if(!globalUserlist)
	{
		if(p->mw->winType == chanWin)
		{
			ULINew(p->mw->w, ulMessageWindow);
		}
	}
}

static char resizingPane;

static pascal void SULPaneUpdate(pMWPaneUpdateData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		
		if(ul)
		{
			ListDraw(ul);
			ListHeader(ul);
			ULIPaneDrawBorder(ul, p->pane, resizingPane);
		}
	}
}

static pascal void ULPaneResize(ULI ul, mwPanePtr o, const EventRecord *e)
{
#pragma unused(ul)
	Rect r;
	Rect dragRect, dr;
	RgnHandle dragRgn;
	long dragResult;
	
	resizingPane = true;
	ULIPaneDrawBorder(ul, o, true);
	r = WGetBBox(o->mw->w);
	GlobalToLocal((Point*)&r.bottom);
	
	dragRgn = NewRgn();
	dr.top = o->drawArea.top;
	dr.bottom = o->drawArea.bottom;
	
	if(ul->rightSide)
	{
		dr.left = o->drawArea.left;
		dr.right = o->drawArea.left + kInWindowBorder;
	}
	else
	{
		dr.left = o->drawArea.right - kInWindowBorder;
		dr.right = o->drawArea.right;
	}
	
	RectRgn(dragRgn, &dr);
	
	dragRect.top = o->drawArea.top;
	dragRect.bottom = o->drawArea.bottom;
	
	if(ul->rightSide)
	{
		dragRect.left = 0;
		dragRect.right = o->drawArea.right - kInWindowBorder + 1;
	}
	else
	{
		dragRect.left = o->drawArea.left + kInWindowBorder;
		dragRect.right = r.right - kInWindowBorder;
	}
	
	dragResult = DragGrayRgn(dragRgn, e->where, &dragRect, &dragRect, hAxisOnly, 0);
	DisposeRgn(dragRgn);
	
	dragResult &= 0xFFFF;
	if(dragResult != 0x8000)
	{
		if(ul->rightSide)
			o->askedWidth = o->drawArea.right - o->drawArea.left - dragResult;
		else
			o->askedWidth = o->drawArea.right - o->drawArea.left + dragResult;
		
		MWPaneRecalculate(o->mw);

		if(e->modifiers & shiftKey)
			mainPrefs->userlistInWindowWidth = o->givenWidth;

		MWPaneResize(o->mw);
		MWPaneUpdate(o->mw);
	}
	
/*
	int offset;
	short old;
	Point pt;

	pt = e->where;
	if(ul->rightSide)
		offset = o->drawArea.left - pt.h;
	else
		offset = pt.h - o->drawArea.right;
	
	while(StillDown())
	 {
		GetMouse(&pt);
		if(ul->rightSide)
			pt.h = o->drawArea.right - pt.h;
		else
			pt.h -= o->drawArea.left;
		
		pt.h -= offset;
		
		if(pt.h != o->givenWidth && pt.h > kMinimumPaneWidth)
		{
			if(o->givenWidth < scrollbarWidth + kInWindowBorder)
				HideControl(ul->bar);
			else
				ShowControl(ul->bar);
			if(pt.h >= kMinimumPaneWidth && pt.h < r.right)
			{
				o->askedWidth = pt.h;
				old = (o->drawArea.right - o->drawArea.left);
				MWPaneRecalculate(o->mw);
				if(o->drawArea.right - o->drawArea.left != old) //only update if something happened
				{
					if(e->modifiers & shiftKey)
						mainPrefs->userlistInWindowWidth = pt.h - kInWindowBorder;

					MWPaneResize(o->mw);
					MWPaneUpdate(o->mw, 0);
				}
			}
		}
	}
*/
	
	resizingPane = false;
	ULIPaneDrawBorder(ul, o, false);
}

static pascal void SULPaneClick(pMWPaneClickData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		ControlHandle c;
		long l;
		mwPanePtr o = p->pane;
		
		if((l = FindControl(p->e->where, ul->uwin, &c)) != 0)
		{
			ULScrollbarHit(ul, l, p->e);
		}
		else
		{
			if((ul->rightSide && (p->e->where.h <= o->drawArea.left + kInWindowBorder )) || (!ul->rightSide && (p->e->where.h > o->drawArea.right - kInWindowBorder)))
			{
				ULPaneResize(ul, o, p->e);
			}
			else
			{
				p->e->where.h -= p->pane->drawArea.left;
				p->e->where.v -= p->pane->drawArea.top;
				ULInContentHit(ul, p->e);
			}
		}
	}
}

INLINE void SULPaneActivate(pMWPaneActivateData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		
		if(ul)
		{
			if(p->activate && p->pane->givenWidth > scrollbarWidth + kInWindowBorder)
				ActivateControl(ul->bar);
			else
				DeactivateControl(ul->bar);

			ListHeader(ul);
			ListDraw(ul);
			ULIPaneDrawBorder(ul, p->pane, false);
		}
	}
}

INLINE void SULPaneResize(pMWPaneResizeData *p)
{
	Rect r;
	
	if(p->pane->type == kUserlistPane)
	{
		mwPanePtr o = p->pane;
		ULI ul = (ULI)o->data;
		
		ul->uwinSize.right = o->drawArea.right - o->drawArea.left - kInWindowBorder;
		ul->uwinSize.bottom =  o->drawArea.bottom - o->drawArea.top;

		r.top = kHeaderSize + o->drawArea.top;
		if(ul->scrollbarLeft)
		{
			r.left = o->drawArea.left -1;
			if(ul->rightSide)
				r.left+= kInWindowBorder +1;
			r.right = r.left + scrollbarWidth;
		}
		else
		{
			if(ul->rightSide)
				r.left = o->drawArea.right - 15;
			else
				r.left = o->drawArea.right - 15 - kInWindowBorder;
		}
		r.bottom = o->drawArea.bottom + 1;
		
		if(!WIsActive(ul->uwin) || o->givenWidth < scrollbarWidth + kInWindowBorder)
			DeactivateControl(ul->bar);
		else
			ActivateControl(ul->bar);

		SizeControl(ul->bar, 16, r.bottom - r.top);
		MoveControl(ul->bar, r.left, r.top);
	}
}

static pascal void SULPaneDestroy(pMWPaneDestroyData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		
		ULIDestroy(ul);
		p->pane->data = 0;
	}
}

static pascal void ULIKillAll(void)
{
	MWPtr mw = *sidr-> mwList;
	
	while(mw)
	{
		if(mw->winType == chanWin)
		{
			mwPanePtr o = MWFindPane(mw, kUserlistPane);
			if(o)
			{
				ULIDestroy((ULI)o->data);
				MWDestroyPane(o);
				MWPaneResize(mw);
				MWPaneUpdate(mw);
			}
		}
		mw=mw->next;
	} 
}

static pascal void ULIAddAll(void)
{
	MWPtr mw = *sidr->mwList;
	ULI ul;
	
	while(mw)
	{
		if(mw->winType == chanWin)
		{
			mwPanePtr o = MWFindPane(mw, kUserlistPane);
			if(!o)
			{
				ul = ULINew(mw->w, ulMessageWindow);
				ListGenerate(ul, MWGetChannel(mw));
			}
		}
		mw=mw->next;
	} 
}


#pragma mark -

INLINE void IdleCursor(EventRecord *e)
{
	static inSep = 0;
	GrafPtr gp;
	Point pt;
	ULI ul = 0;
	char in = 0;
	WindowPtr w;
	MWPtr mw;
	mwPanePtr o;
	
	if(globalUserlist)
	{
		ul = gUserlist;
		in = MouseInWindow(ul);
		w = ul->uwin;
	}
	else
	{
		FindWindow(e->where, &w);
		if(!w || !WIsActive(w))
			in = 0;
		else
		{
			mw = MWFromWindow(w);
			if(!mw)
				in = 0;
			else
			{
				o = MWFindPane(mw, kUserlistPane);
				if(!o)
					in = 0;
				else
				{	
					in = 1; //possibly
					ul = (ULI)o->data;
				}
			}
		}
	}

	if(in || inSep)
	{
		short divider;
		
		if(!ul || !w || !WIsActive(w))
			in = 0;
		else
		{
			divider = ul->nickListWidth - 4;
			sidr->yourInfo->idleThreshold = kIdleFast;
			
			if(ul->updateList && TickCount() - ul->lastUpdate > 10)
				ListDraw(ul);
			
			gp = ULISetupDrawing(ul);
			GetMouse(&pt);
			in = (pt.v >= 0 && pt.v <= kHeaderSize) && (pt.h >= divider - 3 && pt.h <= divider + 3);
			
			if(!in && ul->ulType == ulMessageWindow) //not in nick divider. see if it's in the pane divider
			{
				if(ul->rightSide)
				{
					if(ul->scrollbarLeft)
						divider = -scrollbarWidth - (kInWindowBorder / 2);
					else
						divider = - (kInWindowBorder / 2);
				}
				else
				{
					if(ul->scrollbarLeft)
						divider = ul->uwinSize.right - scrollbarWidth + (kInWindowBorder/2);
					else
						divider = ul->uwinSize.right + (kInWindowBorder/2);
				}
		
				in = (pt.v >= 0 && pt.v <= ul->uwinSize.bottom) &&  (pt.h >= divider - (kInWindowBorder/2) && pt.h <= divider + (kInWindowBorder/2));
			}
		}
		
		if(!inSep && in)
		{
			inSep = 1;
			if(APPEARANCE11)
				SetThemeCursor(kThemeResizeLeftRightCursor);
			else
			{
				SetCursor(*GetCursor(512));
			}
		}
		else if(inSep && !in)
		{
			inSep = 0;
			if(APPEARANCE11)
				SetThemeCursor(kThemeArrowCursor);
			else
			{
				InitCursor();
			}
		}
		
		if(ul)
			ULIFinishDrawing(ul, gp);
	}
	else
		sidr->yourInfo->idleThreshold = kIdleNormal;
}

INLINE void SIdle(pIdleMessageData *p)
{
	if(globalUserlist)
	{
		if(gUserlist->updateList && TickCount() - gUserlist->lastUpdate > 15)
			ListDraw(gUserlist);
	}
	else
	{
		long x = TickCount() - 15;
		MWPtr mw = *sidr->mwList;
		ULI ul;

		while(mw)
		{
			if(mw->winType == chanWin)
			{
				mwPanePtr o = MWFindPane(mw, kUserlistPane);
				if(o)
				{
					ul = (ULI)o->data;
					if(ul->updateList && ul->lastUpdate <= x)
						ListDraw(ul);
				}
			}
			mw=mw->next;
		} 
	}
	
	IdleCursor(p->e);
}


INLINE void ULMoveWindow(pUIWindowMoveDataRec *p)
{
	ULI ul = ULIFromWindow(p->w);
	
	if(ul) //then it's the global window
	{
		mainPrefs->userListRect = p->newpos;
	}
}

INLINE void ULCloseWindow(pUIWindowCloseDataRec *p)
{
	ULI ul = ULIFromWindow(p->w);
	
	if(ul) //then it's the global window
	{
		WHide(ul->uwin);
		mainPrefs->userListOpen = false;
	}
}

INLINE void ULWindowMenuSelect(pServiceWindowMenuData *p)
{
	if(p->key == userlistServiceType)
	{
		if(globalUserlist)
		{
			if(gUserlist)
			{
				if(WIsVisible(gUserlist->uwin)) //hide it
				{
					mainPrefs->userListOpen = false;
					WHide(gUserlist->uwin);
				}
				else //show it
				{
					mainPrefs->userListOpen = true;
					WShow(gUserlist->uwin);
				}

				CheckItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType), mainPrefs->userListOpen);
			}
			else //make it?
			{
			}
		}
		else //in-window
		{
			MWPtr mw = (*sidr->MWActive);
			
			if(mw && mw->winType == chanWin)
			{
				mwPanePtr o = MWFindPane(mw, kUserlistPane);
				
				if(o) //kill it
				{
					ULIDestroy((ULI)o->data);
					MWDestroyPane(o);
					MWPaneResize(mw);
					MWPaneUpdate(mw);
					o = 0;
				}
				else //add it
				{
					ULI ul = ULINew(mw->w, ulMessageWindow);
					MWPaneRecalculate(mw);
					MWPaneUpdate(mw);
					ListGenerate(ul, MWGetChannel(mw));
					o = (mwPanePtr)1;
				}
				
				CheckItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType), (char)o);
			}
		}
	}
}

//This is always going to be used for the global list
static pascal void ULResizeWin(ULI ul, short height, short wid)
{
	short ctrlHeight;
	
	SizeWindow(ul->uwin, wid, height, false);
	
	//Update prefs
	mainPrefs->userListRect.right = mainPrefs->userListRect.left + wid;
	mainPrefs->userListRect.bottom = mainPrefs->userListRect.top + height;
	
	if(ul->scrollbarLeft)
	{
		ctrlHeight = height - kHeaderSize + 1;
	}
	else
	{
		ctrlHeight = height - 13 - kHeaderSize;
	}
	
	//r.bottom is now the size of the window
	if(ul->scrollbarLeft)
		MoveControl(ul->bar, -1, kHeaderSize);
	else
		MoveControl(ul->bar, wid-scrollbarWidth+2, kHeaderSize);
	SizeControl(ul->bar, scrollbarWidth, ctrlHeight);

	ul->uwinSize.bottom = height;
	ul->uwinSize.right = wid;
	
	ListHeader(ul);
	ListDraw(ul);
}

INLINE void ULGrowWindow(pUIInGrowData *p)
{
	long l;
	Rect r;
	ULI ul= ULIFromWindow(p->window);
	
	if(ul)
	{
		SetRect(&r, kULMininumWidth, kULMinimumHeight, 32767, 32767);
		l = GrowWindow(p->window, p->e->where, &r);
		if(l)
		{
			ULResizeWin(ul, *(short*)&l, l & 0xFFFF);
		}
	}
}

INLINE void ULUpdateWin(pUIUpdateData *p)
{
	ULI ul = ULIFromWindow((WindowPtr)p->e->message);

	if(ul)
	{
		DrawControls(ul->uwin);
		ListDraw(ul);
		ListHeader(ul);
	}
}

INLINE void ULMouse(pUIMouseUpDownData *p)
{
	ULI ul;
	GrafPtr gp;
	ControlHandle c;
	short pa;
	
	if(p->down)
	{
 		ul = ULIFromWindow(p->window);
 		if(ul)
		{
			//This is an inContent mouse press, so deal accordingly...
			
			GetPort(&gp);
			SetPort(ul->uwin);
			
			GlobalToLocal(&p->e->where);
			pa = FindControl(p->e->where, ul->uwin, &c);
			
			if(c == ul->bar) //it's our scrollbar
			{
				ULScrollbarHit(ul, pa, p->e);
			}
			else if(!c) //not a control - mouse was clicked in the content
				ULInContentHit(ul, p->e);
			
			SetPort(gp);
		}
	}
}

INLINE void CreateGlobalUserlistWindow(void)
{
	if(!gUserlist)
	{
		gUserlist = ULINew(0, ulGlobal);
	}
}

INLINE void DestroyGlobalUserlistWindow(void)
{
	if(gUserlist)
	{
		ULIDestroy(gUserlist);		
		gUserlist = 0;
	}
}

#pragma mark -

char dInWindowRight, dScrollbarLeft;

INLINE void PWSet(pPWSetWindowData *p)
{
	if(p->id == prefsPanel)
	{
		setCheckBox(p->PrefsDlg, 4, mainPrefs->userlistInWindow);
		setCheckBox(p->PrefsDlg, 5, mainPrefs->userlistInWindowRight);
		setCheckBox(p->PrefsDlg, 6, mainPrefs->userlistScrollbarLeft);
	}
}

INLINE void PWGet(pPWGetWindowData *p)
{
	if(p->id == prefsPanel)
	{
		mainPrefs->userlistInWindow = getCheckBox(p->PrefsDlg, 4);
		mainPrefs->userlistInWindowRight = getCheckBox(p->PrefsDlg, 5);
		mainPrefs->userlistScrollbarLeft = getCheckBox(p->PrefsDlg, 6);
	}
}

INLINE void PWHit(pPWItemHitData *p)
{
	if(p->id == prefsPanel)
	{
		switch(p->itemNum)
		{
			case 4:
			case 5:
			case 6:
				setCheckBox(p->PrefsDlg, p->itemNum, !getCheckBox(p->PrefsDlg, p->itemNum));
		}
	}
}

INLINE void PWClosed(void)
{
	if(mainPrefs->userlistInWindow == globalUserlist) //it changed!
	{
		globalUserlist = !mainPrefs->userlistInWindow;
		
		if(globalUserlist) //kill all in-windows and turn off message
		{
			sidr->yourInfo->captureMessages[pMWNewMessage] = 0;
			ULIKillAll();
			mainPrefs->userListOpen = 1;
			CreateGlobalUserlistWindow();
			if((*sidr->MWActive) && (*sidr->MWActive)->winType == chanWin)
				ListGenerate(gUserlist, MWGetChannel(*sidr->MWActive));
			
			EnableItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType));
		}
		else
		{
			if(gUserlist)
				DestroyGlobalUserlistWindow();
			sidr->yourInfo->captureMessages[pMWNewMessage] = 1;
			ULIAddAll();
		}
	}
	else //see if anything else changed
	{
		if(mainPrefs->userlistInWindowRight != dInWindowRight)
		{
			if(!globalUserlist)
			{
				ULIKillAll();
				ULIAddAll();
			}
		}
		else if(mainPrefs->userlistScrollbarLeft != dScrollbarLeft)
		{
			if(globalUserlist)
			{
				Rect r = WGetBBox(gUserlist->uwin);
				gUserlist->scrollbarLeft = mainPrefs->userlistScrollbarLeft;
				ULResizeWin(gUserlist, r.bottom - r.top, r.right - r.left);
			}
			else
			{
				ULIKillAll();
				ULIAddAll();
			}
		}
	}
	
	dInWindowRight = mainPrefs->userlistInWindowRight;
	dScrollbarLeft = mainPrefs->userlistScrollbarLeft;
}

#pragma mark -

static pascal void displayOldVersionMsg(void)
{
	LongString ls;
	
	LSStrLS("\pThe userlist plugin requires ShadowIRC 1.1 or later.", &ls);
	if(!WIsVisible((*sidr->consoleWin)->w))
		WShow((*sidr->consoleWin)->w);
	SMPrefixIrcleColor(&ls, dsConsole, '2');
}

static pascal void displayMultipleUserlistsMsg(void)
{
	LongString ls;
	
	LSStrLS("\pYou have more than one userlist plugin installed.", &ls);
	if(!WIsVisible((*sidr->consoleWin)->w))
		WShow((*sidr->consoleWin)->w);
	SMPrefix(&ls, dsConsole);
}

INLINE void setupMessages(char captureMessages[numMessages])
{
	captureMessages[pIdleMessage]=
	captureMessages[pServerJOINMessage]=
	captureMessages[pServerPARTMessage]=
	captureMessages[pServerQUITMessage]=
	captureMessages[pServerMODEMessage]=
	captureMessages[pServerKICKMessage]=
	captureMessages[pShortcutProcessMessage]=1;
	captureMessages[pPWClosedMessage]=1;
	if(!globalUserlist)
		captureMessages[pMWNewMessage] = 1;
	captureMessages[pCMPopupsMessage] = 1;
}
#pragma internal off

pascal void main(ShadowIRCDataRecord* sidrIN)
{
	long l;
	#if !__POWERPC__
	EnterCodeResource();
	#endif

	switch(sidrIN->message)
	{
		case pVersionCheckMessage:
			sidr=sidrIN;
			mainPrefs = sidrIN->mainPrefs;
			shadowircColors = sidrIN->shadowircColors;
			hasAppearance11 = sidrIN->hasAppearance11;
			
			l=((pVersionCheckDataPtr)sidrIN->messageData)->version;
			if(l<0x0101003c) //1.1d60
			{
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionShadowIRCTooOld;
				displayOldVersionMsg();
			}
			else
			{
				l = _UndocumentedAPI('ulst', 11);
				if(l)
					((pVersionCheckDataPtr)sidrIN->messageData)->version = l;
				else
					((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionCheckMessageReply;
			}
			break;

		case pInitMessage:
			if(!ExistsService(userlistServiceType))
			{
				NewService(userlistServiceType); //We are registered
				
				setupMessages(sidrIN->yourInfo->captureMessages);
				sidrIN->yourInfo->idleThreshold = kIdleNormal;
				
				gSortForwardIcon = (CIconHandle)Get1IndResource('cicn', 500);
				gSortReverseIcon = (CIconHandle)Get1IndResource('cicn', 501);
//				gSortForwardIcon = Get1IndResource('ICON', 128);
//				gSortReverseIcon = Get1IndResource('ICON', 129);
				dInWindowRight = mainPrefs->userlistInWindowRight;
				dScrollbarLeft = mainPrefs->userlistScrollbarLeft;
				prefsPanel = PMLAdd("\pUserlist");

				globalUserlist= !mainPrefs->userlistInWindow;
				//Create userlist window
				if(globalUserlist)
					CreateGlobalUserlistWindow();
				else
					DisableItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType));
			}
			else
				displayMultipleUserlistsMsg();
			break;

		case pPWActivateMessage:
			if(((pPWActivateDataPtr)sidrIN->messageData)->id==prefsPanel)
				((pPWActivateDataPtr)sidrIN->messageData)->ditlNum = 1541;
			break;

		case pPWSetWindowMessage:
			PWSet((pPWSetWindowDataPtr)sidrIN->messageData);
			break;
		
		case pPWGetWindowMessage:
			PWGet((pPWGetWindowDataPtr)sidrIN->messageData);
			break;
			
		case pPWItemHitMessage:
			PWHit((pPWItemHitDataPtr)sidrIN->messageData);
			break;
			
		case pPWRedrawMessage:
			break;
		
		case pPWClosedMessage:
			PWClosed();
			break;

		case pUIWindowMoveMessage:
			ULMoveWindow((pUIWindowMoveDataPtr)sidrIN->messageData);
			break;
		
		case pUIWindowCloseMessage:
			ULCloseWindow((pUIWindowCloseDataPtr)sidrIN->messageData);
			break;
		
		case pServiceWindowMenu:
			ULWindowMenuSelect((pServiceWindowMenuPtr)sidrIN->messageData);
			break;
		
		case pUIInGrowMessage:
			ULGrowWindow((pUIInGrowDataPtr)sidrIN->messageData);
			break;
		
		case pUIUpdateMessage:
			ULUpdateWin((pUIUpdateDataPtr)sidrIN->messageData);
			break;
			
		case pUIMouseUpDownMessage:
			ULMouse((pUIMouseUpDownDataPtr)sidrIN->messageData);
			break;
		
		case pServiceULNamesEnd:
			ULGotNamesList((pServiceULNamesEndDataPtr)sidrIN->messageData);
			break;
		
		case pServiceActivateWin:
			ULSActivateWin((pServiceActivateWinDataPtr)sidrIN->messageData);
			break;
		
		case pServerJOINMessage:
			SJoin((pServerJOINDataPtr)sidrIN->messageData);
			break;

		case pServerPARTMessage:
			SPart((pServerPARTDataPtr)sidrIN->messageData);
			break;
		
		case pServerMODEMessage:
			SMode((pServerMODEDataPtr)sidrIN->messageData);
			break;
		
		case pServerQUITMessage:
			SQuit((pServerQUITDataPtr)sidrIN->messageData);
			break;

		case pServiceULNickChange:
			SNick((pServerNICKDataPtr)sidrIN->messageData);
			break;

		case pServerKICKMessage:
			SKick((pServerKICKDataPtr)sidrIN->messageData);
			break;

		case pServiceULTrashChannel:
			STrashChannel((pServiceULTrashChannelDataPtr)sidrIN->messageData);
			break;
		
		case pServiceULUserhosts:
			SULUserHosts((pServiceULUserhostsPtr)sidrIN->messageData);
			break;
		
		case pIdleMessage:
			SIdle((pIdleMessageDataPtr)sidrIN->messageData);
			break;
		
		case pShortcutProcessMessage:
			ProcessShortcuts((pShortcutProcessDataPtr)sidrIN->messageData);
			break;
		
		case pUIDragTrackMessage:
			ULDragTrack((pUIDragTrackDataPtr)sidrIN->messageData);
			break;
		
		case pUIDragReceiveMessage:
			ULDragReceive((pUIDragReceiveDataPtr)sidrIN->messageData);
			break;

		case pMWNewMessage:
			ULNewMessageWindow((pMWNewDataPtr)sidrIN->messageData);
			break;
		
		 case pMWPaneUpdateMessage:
		 	SULPaneUpdate((pMWPaneUpdateDataPtr)sidrIN->messageData);
		 	break;
		 
		 case pMWPaneClickMessage:
		 	SULPaneClick((pMWPaneClickDataPtr)sidrIN->messageData);
		 	break;
		 
		 case pMWPaneActivateMessage:
		 	SULPaneActivate((pMWPaneActivateDataPtr)sidrIN->messageData);
		 	break;
		 
		 case pMWPaneResizeMessage:
			 SULPaneResize((pMWPaneResizeDataPtr)sidrIN->messageData);
		 	break;
		 
		 case pMWPaneDestroyMessage:
		 	SULPaneDestroy((pMWPaneDestroyDataPtr)sidrIN->messageData);
		 	break;
		
		case pMWPaneDragTrackMessage:
			ULPaneDragTrack((pMWPaneDragTrackDataPtr)sidrIN->messageData);
			break;
			
		case pMWPaneDragReceiveMessage:
			ULPaneDragReceive((pMWPaneDragReceiveDataPtr)sidrIN->messageData);
			break;
		
		case pCMPopupsMessage:
			ULContextualMenu((pCMPopupsDataPtr)sidrIN->messageData);
			break;
		
		case pCMPopupsReturnMessage:
			ULContextualMenuProcess((pCMPopupsReturnDataPtr)sidrIN->messageData);
			break;
	}

	#if !__POWERPC__
	ExitCodeResource();
	#endif
}

/*
MacTek: http://cctr.umkc.edu/user/cgladish/faq/gspot2-faq.html
http://www.ozemail.com.au/~jdukes/docs/gspot.html
http://www.jmk.su.se/jmk/stud/magen/g-guntin/g-punkt.html
*/