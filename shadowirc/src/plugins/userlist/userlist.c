/*
	ShadowIRC Userlist
	Copyright (C) 1997-2005 John Bafford
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

#include <Carbon/Carbon.h>

#include "userlist.h"
#include "ULList.h"

static ShadowIRCDataRecord* sidr;
static prefsPtr mainPrefs;
static RGBColor *shadowircColors = 0;

static short prefsPanel;

static char globalUserlist = 0;

static ULI gUserlist = 0;

enum {
	cMedGrey = 44395,
	cMDkGrey = 35939
};

const RGBColor white = {-1, -1, -1};
const RGBColor black = {0, 0, 0};
const RGBColor MedGrey = {cMedGrey, cMedGrey, cMedGrey};
const RGBColor MDkGrey = {cMDkGrey, cMDkGrey, cMDkGrey};

INLINE ULI ULIFromMW(MWPtr mw);
static ULI ULIFromChannel(channelPtr ch);

INLINE void ULGotNamesList(pServiceULNamesEndData *p);
static void ULSActivateWin(pServiceActivateWinData *p);
static char MouseInWindow(ULI ul);

//static void ULContextualMenu(pCMPopupsData *p);
//static void ULContextualMenuProcess(pCMPopupsReturnData *p);
static void ProcessShortcuts(pShortcutProcessData *p);

INLINE void IdleCursor(void);
INLINE void CreateGlobalUserlistWindow(void);
INLINE void DestroyGlobalUserlistWindow(void);
static void ULCloseWindow(ULI ul);
INLINE void ULWindowMenuSelect(pServiceWindowMenuData *p);
static void ULResizeWin(ULI ul, short height, short wid);

INLINE void ULNewMessageWindow(pMWNewData *p);
static void SULPaneUpdate(pMWPaneUpdateData *p);
static void SULPaneDestroy(pMWPaneDestroyData *p);
static void ULPaneResize(ULI ul, mwPanePtr o, Point where, UInt32 modifiers);
static void SULPaneClick(pMWPaneClickData *p);
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

static ULI ULINew(WindowPtr w, long type);
static void ULIDestroy(ULI ul);
static void ULIPaneDrawBorder(ULI ul, mwPanePtr pane, char pressed);
static void ULIKillAll(void);
static void ULIAddAll(void);

INLINE void PWSet(pPWSetWindowData *p);
INLINE void PWGet(pPWGetWindowData  *p);
INLINE void PWHit(pPWItemHitData *p);
INLINE void PWClosed(void);

static void displayOldVersionMsg(void);
static void displayMultipleUserlistsMsg(void);
INLINE void setupMessages(char captureMessages[numMessages]);

static void SetIdleThreshold(long ticks);


enum {
	kUserlistSignature = 'ULST',
	kUserlistWindow = 'WIND'
};

static void ULSetWindowProperty(ULI ul)
{
	SetWindowProperty(ul->uwin, kUserlistSignature, kUserlistWindow, sizeof(ul), &ul);
}

static ULI ULGetWindowProperty(WindowPtr w)
{
	ULI ul;
	UInt32 actualSize;
	
	GetWindowProperty(w, kUserlistSignature, kUserlistWindow, sizeof(ul), &actualSize, &ul);
	
	return ul;
}

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

static ULI ULIFromChannel(channelPtr ch)
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

#pragma mark -

void DragOneUser(ULI ul, int u, const EventRecord *e);
void DragOneUser(ULI ul, int u, const EventRecord *e)
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

#pragma mark -

INLINE void ULGotNamesList(pServiceULNamesEndData *p)
{
	ULI ul = ULIFromChannel(p->channel);

	if(ul && (!ul->ch || ul->ch == p->channel))
		ListGenerate(ul, p->channel);
}

static void ULSActivateWin(pServiceActivateWinData *p)
{
	static WindowPtr lastActivate = 0;
	channelPtr ch = 0;
	ULI ul = 0;
	
	if(p->mw)
		ch = MWGetChannel(p->mw);
	ul = ULIFromChannel(ch);
	
	if(p->activate)
	{
		if(p->w == lastActivate)
			return;

		lastActivate = p->w;
		
		if(globalUserlist)
		{
			if(ch && ch->active) //valid channel
			{
				ListGenerate(ul, ch);
				ListScroll(ul);
				ListSetWTitle(ul);
			}
			else if(ul)
			{
				ListTrash(ul);
				ListSetWTitle(ul);
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
			CheckMenuItem(GetMenuHandle(260), i, ul != 0);
			if(p->mw && p->mw->winType == chanWin)
				EnableMenuItem(GetMenuHandle(260), i);
			else
				DisableMenuItem(GetMenuHandle(260), i);
		}
	}
}

static char MouseInWindow(ULI ul)
{
	GrafPtr gp;
	Point pt;
	
	GetPort(&gp);
	SetPortWindowPort(ul->uwin);
	GetMouse(&pt);
	SetPort(gp);
	return IsWindowVisible(ul->uwin) && PtInRect(pt, &ul->uwinSize);
}

#pragma mark -

INLINE void SJoin(pServerJOINDataRec *p)
{
	ULI ul = ULIFromChannel(p->channel);
	
	if(ul && ul->ch && p->channel == ul->ch)
	{
		ListAdd(ul, p->userPtr);
		
		if(!MouseInWindow(ul))
		{
			if(TickCount() - ul->lastUpdate < 30)
				ul->updateList = 1;
		}
	}
}

INLINE void SPart(pServerPARTDataRec *p)
{
	ULI ul = ULIFromChannel(p->channel);
	
	if(ul && ul->ch && p->channel == ul->ch)
	{
		ListDel(ul, p->userPtr);

		if(!MouseInWindow(ul))
		{
			if(TickCount() - ul->lastUpdate < 30)
				ul->updateList = 1;
		}
	}
}

INLINE void SMode(pServerMODEData *p)
{
	ULI ul = ULIFromChannel(p->ch);

	if(ul && ul->ch && p->ch && p->ch == ul->ch && !p->done && !ul->updateList)
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
	channelPtr ch = NULL;
	
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
		if(ul && ul->ch)
		{
			if(ListDelName(ul, p->username))
			{
				if(!MouseInWindow(ul))
				{
					if(l - ul->lastUpdate < 30)
						ul->updateList = 1;
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
	channelPtr ch = NULL;
	
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
		if(ul && ul->ch)
		{
			u = ULFindUserName(ul->ch, p->newnick);
			if(u)
				ListResortInd(ul, u);
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

	if(ul && ul->ch && p->channel == ul->ch)
	{
		if(p->isMe)
		{
			ListTrash(ul);
		}
		else
		{
			ListDelName(ul, p->kickedNick);

			if(!MouseInWindow(ul))
			{
				if(TickCount() - ul->lastUpdate < 30)
					ul->updateList = 1;
			}
		}
	}
}

INLINE void STrashChannel(pServiceULTrashChannelData *p)
{
	ULI ul = ULIFromChannel(p->channel);

	if(ul && ul->ch && p->channel == ul->ch)
	{
		ListTrash(ul);
		ListSetWTitle(ul);
	}
}

INLINE void SULUserHosts(pServiceULUserhostsData *p)
{
	if(globalUserlist)
	{
		ListRefresh(gUserlist);
	}
	else
	{
		channelPtr ch = p->link->channelList;
		ULI ul;
		
		while(ch)
		{
			ul = ULIFromChannel(ch);
			if(ul)
				ListRefresh(ul);
			ch=ch->next;
		}
	}
}

#pragma mark -

//static int hiliteUser = -1;

/*
static void ULDragTrack(pUIDragTrackData *p)
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

static void ULDragReceive(pUIDragReceiveData *p)
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
*/
#pragma mark -


inline void RepeatMunger(Handle text, long offset, const void* ptr1, long len1, const void* ptr2, long len2)
{
	int i;
	do {
		i = Munger(text, offset, ptr1, len1, ptr2, len2);
	} while(i >= 0);
}

static void ProcessShortcuts(pShortcutProcessData *p)
{
/*
	ConstStringPtr s;
	int x;
	int n;
	ulPtr u;
	ULI ul;
	UserListPtr user;
	
	if(globalUserlist)
		ul = gUserlist;
	else
		ul = ULIFromMW(GetActiveMW());
	
	if(!ul)
		return;
	
	if(ul->ch)
	{
		//Get first selected user
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
*/	
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
}

#pragma mark -

static void ULIDestroy(ULI ul)
{
	if(ul)
	{
		ListTrash(ul);
	
		if(ul->ulType == ulGlobal)
			pluginDisposeWindow(ul->uwin);
		else //ulMessageWindow
		{
			if(ul->browser)
			{
				DisposeControl(ul->browser);
				ul->browser = 0;
			}
		}

		DisposePtr((Ptr)ul);
	}
}

static OSStatus UlDoWindowEventHandler(EventHandlerCallRef nextHandler, EventRef event, void *userData)
{
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	WindowRef ilWindow;
	ULI ul = userData;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassWindow:
			GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL, sizeof (WindowRef), NULL, &ilWindow);
			
			switch (eventKind)
			{
				case kEventWindowGetMinimumSize:
				{
					Point minSize;
					minSize.h = kULMinimumHeight;
					minSize.v = kULMininumWidth;
					SetEventParameter(event, kEventParamDimensions, typeQDPoint, sizeof(minSize), &minSize);
					result = noErr;
					break;
				}
				case kEventWindowBoundsChanged:
				{
					Rect r;
					GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &r);
					ULResizeWin(ul, r.bottom - r.top, r.right - r.left);
					mainPrefs->userListRect = r;
					result = noErr;
					break;
				}
				case kEventWindowClose:
					ULCloseWindow(ul);
					result = noErr;
					break;
			}
			break;
	}
	
	return result;
}

static void ULInstallWindowHandlers(ULI ul)
{
	static EventHandlerUPP ulWindowHandler = NULL;
	const EventTypeSpec ulWindowTypes[] = {
		{kEventClassWindow, kEventWindowGetMinimumSize},
		{kEventClassWindow, kEventWindowBoundsChanged},
		{kEventClassWindow, kEventWindowClose}
	};
	
	if(!ulWindowHandler)
		ulWindowHandler = NewEventHandlerUPP(UlDoWindowEventHandler);
	
	InstallWindowEventHandler(ul->uwin, ulWindowHandler, GetEventTypeCount(ulWindowTypes), ulWindowTypes, ul, NULL);
}

static OSStatus DBGetSetItem(ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID prop, DataBrowserItemDataRef itemData, Boolean setValue)
{
	CFStringRef sr;
	UserListPtr u = (UserListPtr)item;
	
	if(prop == 'name')
	{
		sr = CFStringCreateWithPascalString(NULL, u->nick, kCFStringEncodingMacRoman);
		SetDataBrowserItemDataText(itemData, sr);
		CFRelease(sr);
	}
	else if(prop == 'host')
	{
		sr = CFStringCreateWithPascalString(NULL, u->userhost, kCFStringEncodingMacRoman);
		SetDataBrowserItemDataText(itemData, sr);
		CFRelease(sr);
	}
	else
		return errDataBrowserPropertyNotSupported;
	
	return noErr;
}

static char inupc(char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

static void pstrcpyucase(Str255 src, Str255 dest)
{
	int x;
	int n = src[0];
	
	dest[0]=src[0];
	for(x=1;x<=n;x++)
		dest[x]=inupc(src[x]);
}

static Boolean DBItemCompare(ControlRef browser, DataBrowserItemID itemOne, DataBrowserItemID itemTwo, DataBrowserPropertyID sortProperty)
{
	UserListPtr a, b;
	Str255 s1, s2;
	
	a = (UserListPtr)itemOne;
	b = (UserListPtr)itemTwo;
	
	if(sortProperty == 'name')
	{
		pstrcpyucase(a->nick, s1);
		pstrcpyucase(b->nick, s2);
	}
	else if(sortProperty == 'host')
	{
		pstrcpyucase(a->userhost, s1);
		pstrcpyucase(b->userhost, s2);
	}
	
	return !pstrgt(s1, s2);
}

static void ULDoubleClick(ULI ul, UserListPtr u)
{
	LongString ls;
	
	if(ul && ul->ch && u)
	{
		LSConcatStrAndStr("\pWHOIS ", u->nick, &ls);
		HandleCommand(ul->ch->link, &ls);
	}
}

static void DBItemNotify(ControlRef browser, DataBrowserItemID item, DataBrowserItemNotification message)
{
	ULI ul;
	UInt32 actualSize;
	UserListPtr u = (UserListPtr)item;
	
	switch(message)
	{
		case kDataBrowserItemDoubleClicked:
			GetControlProperty(browser, kUserlistSignature, kUserlistSignature, sizeof(ULI), &actualSize, &ul);
			ULDoubleClick(ul, u);
			break;
	}
}

void DBCustomDrawItem(ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID property, DataBrowserItemState itemState, const Rect* theRect, SInt16 gdDepth, Boolean colorDevice)
{
	UserListPtr user = (UserListPtr)item;
	
	if(property == 'name')
	{
		Str255 s;
		Style face = 0;
		RGBColor *color = 0;
		RGBColor hilite, oColor;
		CFStringRef cfStr;
		Rect hackDBRect = *theRect;
		int width;
		
		hackDBRect.left -= 7;
		hackDBRect.right += 7;
		
		width = theRect->right - theRect->left;
		
		if(user->isOper)
		{
			face |= italic;
			if(!user->isOp)
				color = &shadowircColors[sicUserOper];
		}
		if(user->isOp)
		{
			face |= bold;
			color = &shadowircColors[sicUserChannelOp];
		}
		else if((user->hasVoice || user->isHalfOp) && !user->isOper)
		{
			face |= underline;
			color = &shadowircColors[sicUserVoice];
		}
	//	else if(user->isIgored)
	//		RGBForeColor(&shadowircColors[sicUserIgnored]);
		else
			if(!color)
				color = &shadowircColors[sicUserRegular];
		
		if(user->isAway)
		{
			pstrcpy(user->nick, &s[1]);
			s[0] = s[1] +1;
			s[1] = '(';
			TruncString(width, s, truncEnd);
			SAppend1(s, ')');
		}
		else
		{
			pstrcpy(user->nick, s);
			TruncString(width, s, truncEnd);
		}
		
		GetForeColor(&oColor);
		if(itemState == kDataBrowserItemIsSelected)
		{
			Rect tr = hackDBRect;
			
			tr.bottom += 1;
			tr.right += 5;
			tr.left -= 5;
			
			GetPortHiliteColor(GetWindowPort(GetControlOwner(browser)), &hilite);
			RGBForeColor(&hilite);
			PaintRect(&tr);
		}
		
		RGBForeColor(color);
		TextFace(face);
		cfStr = CFStringCreateWithPascalString(NULL, s, kCFStringEncodingMacRoman);
		DrawThemeTextBox(cfStr, kThemeCurrentPortFont, kThemeStateActive, true, &hackDBRect, teFlushDefault, NULL);
		TextFace(0);
		
		RGBForeColor(&oColor);
		
//		if(user->userlistIsSelected)
	}
}

static Boolean DBAddDragItem(ControlRef browser, DragReference theDrag, DataBrowserItemID item, ItemReference *itemRef)
{
	UserListPtr u = (UserListPtr)item;
	
	if(u)
	{
		DragAddPtr(theDrag, (ItemReference)NULL, kUserFlavor, u, flavorSenderOnly);
		return true;
	}
	
	return false;
}

static void DBPostProcessDrag(ControlRef browser, DragReference theDrag, OSStatus trackDragResult)
{
}

static Boolean DBAcceptDrag(ControlRef browser, DragReference theDrag, DataBrowserItemID item)
{
	if(item && DragIsTypeAvail(theDrag, flavorTypeHFS))
		return true;
	
	return false;
}

static Boolean DBReceiveDrag(ControlRef browser, DragReference theDrag, DataBrowserItemID item)
{
	UserListPtr u = (UserListPtr)item;
	ULI ul = ULGetWindowProperty(GetControlOwner(browser));
	
	if(u && DragIsTypeAvail(theDrag, flavorTypeHFS))
	{
		HFSFlavor *hf;
		FSSpec f;
		Str255 s;
		unsigned short items, i;
		ItemReference item;
		FlavorFlags flags;
		long siz;
		linkPtr l;
		
		pstrcpy(u->nick, s);
	
		CountDragItems(theDrag, &items);
		for(i=1;i<=items;i++)
		{
			GetDragItemReferenceNumber(theDrag, i, &item);
			
			if(!GetFlavorFlags(theDrag, item, flavorTypeHFS, &flags))
			{
				GetFlavorDataSize(theDrag, item, flavorTypeHFS, &siz);
				hf = (HFSFlavor*)NewPtr(siz);
				GetFlavorData(theDrag, item, flavorTypeHFS, hf, &siz, 0);
				
				f = hf->fileSpec;
				if(ul->mw && ul->mw->link)
					l = ul->mw->link;
				else
					l = sidr->CurrentTarget->link;
				DoDCCSendFile(l, s, &f, true, false);
	
				DisposePtr((Ptr)hf);
			}
		}
		return true;
	}
	return false;
}

static DataBrowserDragFlags DBCAcceptDrag(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, const Rect *theRect, DragReference theDrag)
{
	return DBAcceptDrag(browser, theDrag, itemID);
}

static Boolean DBCReceiveDrag(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserDragFlags dragFlags, DragReference theDrag)
{
	return DBReceiveDrag(browser, theDrag, itemID);
}

static ULI ULINew(WindowPtr w, long type)
{
	ULI ul = (ULI)NewPtrClear(sizeof(UserListInstance));
	Rect r; //pos for scrollbar
	GrafPtr gp;
	MWPtr mw = NULL;
	
	if(mainPrefs->userlistNickWidth < 10)
		mainPrefs->userlistNickWidth = 80;
		
	ul->ulType = type;
	ul->uwin = w;
	ul->sortReverse = mainPrefs->userlistSortReverse;
	ul->sort = mainPrefs->userlistSort;
	ul->nickListWidth = mainPrefs->userlistNickWidth;
	ul->updateList = false;
	
	ul->uwinSize.top = ul->uwinSize.left = 0;
	if(type == ulGlobal)
	{
		if(EmptyRect(&mainPrefs->userListRect) || //the rect is empty. Set a new one
			!RectInRgn(&mainPrefs->userListRect, GetGrayRgn())) //then it's displaying offscreen.
			SetRect(&mainPrefs->userListRect, 40, 40, 300, 300);
		
		ul->uwinSize.right = mainPrefs->userListRect.right - mainPrefs->userListRect.left;
		ul->uwinSize.bottom =  mainPrefs->userListRect.bottom - mainPrefs->userListRect.top;

		ul->rightSide = false;
		r.top = 0;
		r.left = 0;
		r.bottom = ul->uwinSize.bottom;
		r.right = ul->uwinSize.right;
		
		ul->mw = 0;
		ul->pane = 0;
		
		//Create the window after fully setting up the ULI.
		ul->uwin = pluginNewWindow(&mainPrefs->userListRect, "\pUserlist", kWindowResizableAttribute | kWindowLiveResizeAttribute | kWindowStandardHandlerAttribute, true);
		ULSetWindowProperty(ul);
		
		ULInstallWindowHandlers(ul);
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
		
		r = o->drawArea;
		
		if(ul->rightSide)
			r.left += kInWindowBorder;
		else
			r.right -= kInWindowBorder;
	}
	
	CreateDataBrowserControl(ul->uwin, &r, kDataBrowserListView, &ul->browser);
	{
		DataBrowserListViewColumnDesc dbColumn;
		DataBrowserCallbacks dcb;
		static DataBrowserItemDataUPP dbIDUPP = NULL;
		static DataBrowserItemCompareUPP dbICUPP = NULL;
		static DataBrowserItemNotificationUPP dbINUPP = NULL;
		static DataBrowserAddDragItemUPP addDragItemCallback = NULL;
		static DataBrowserAcceptDragUPP dbADUPP = NULL;
		static DataBrowserReceiveDragUPP dbRDUPP = NULL;
		static DataBrowserPostProcessDragUPP postProcessDragCallback = NULL;
		DataBrowserCustomCallbacks dccb;
		static DataBrowserDrawItemUPP dbCDIUPP = NULL;
		static DataBrowserItemAcceptDragUPP acceptDragCallback = NULL;
		static DataBrowserItemReceiveDragUPP receiveDragCallback = NULL;
		static ControlFontStyleRec controlFontStyleStruc;
		
		controlFontStyleStruc.flags = kControlUseSizeMask | kControlUseFaceMask;
		controlFontStyleStruc.size = 10;
		controlFontStyleStruc.style = normal;
		
		dbColumn.propertyDesc.propertyID = 'name';
		dbColumn.propertyDesc.propertyType = kDataBrowserCustomType;
		dbColumn.propertyDesc.propertyFlags = kDataBrowserDefaultPropertyFlags | kDataBrowserListViewSortableColumn | kDataBrowserListViewSelectionColumn;
		dbColumn.headerBtnDesc.version= kDataBrowserListViewLatestHeaderDesc,
		dbColumn.headerBtnDesc.minimumWidth = 55;
		dbColumn.headerBtnDesc.maximumWidth = 500;
		dbColumn.headerBtnDesc.titleOffset = -7;
		dbColumn.headerBtnDesc.titleString = CFSTR("Nickname");
		dbColumn.headerBtnDesc.initialOrder = kDataBrowserOrderIncreasing;
		dbColumn.headerBtnDesc.btnFontStyle = controlFontStyleStruc;
		dbColumn.headerBtnDesc.btnContentInfo.contentType = kControlContentTextOnly;
		
		AddDataBrowserListViewColumn(ul->browser, &dbColumn, ULONG_MAX);

		dbColumn.propertyDesc.propertyID = 'host';
		dbColumn.propertyDesc.propertyType = kDataBrowserTextType;
		dbColumn.propertyDesc.propertyFlags = kDataBrowserDefaultPropertyFlags | kDataBrowserListViewSortableColumn | kDataBrowserListViewSelectionColumn;
		dbColumn.headerBtnDesc.version= kDataBrowserListViewLatestHeaderDesc,
		dbColumn.headerBtnDesc.minimumWidth = 55;
		dbColumn.headerBtnDesc.maximumWidth = 500;
		dbColumn.headerBtnDesc.titleOffset = -7;
		dbColumn.headerBtnDesc.titleString = CFSTR("Hostname");
		dbColumn.headerBtnDesc.initialOrder = kDataBrowserOrderIncreasing;
		dbColumn.headerBtnDesc.btnFontStyle = controlFontStyleStruc;
		dbColumn.headerBtnDesc.btnContentInfo.contentType = kControlContentTextOnly;
		
		AddDataBrowserListViewColumn(ul->browser, &dbColumn, ULONG_MAX);
		
		SetDataBrowserSortProperty(ul->browser, 'name');
		SetDataBrowserSortOrder(ul->browser, kDataBrowserOrderIncreasing);
		
		InitializeDataBrowserCallbacks(&dcb, kDataBrowserLatestCallbacks);
		
		if(!dbIDUPP)
			dbIDUPP = NewDataBrowserItemDataUPP(DBGetSetItem);
		dcb.u.v1.itemDataCallback = dbIDUPP;
		
		if(!dbICUPP)
			dbICUPP = NewDataBrowserItemCompareUPP(DBItemCompare);
		dcb.u.v1.itemCompareCallback = dbICUPP;
		
		if(!dbINUPP)
			dbINUPP = NewDataBrowserItemNotificationUPP(DBItemNotify);
		dcb.u.v1.itemNotificationCallback = dbINUPP;
		
		if(!addDragItemCallback)
			addDragItemCallback = NewDataBrowserAddDragItemUPP(DBAddDragItem);
		dcb.u.v1.addDragItemCallback = addDragItemCallback;

		if(!dbADUPP)
			dbADUPP = NewDataBrowserAcceptDragUPP(DBAcceptDrag);
		dcb.u.v1.acceptDragCallback = dbADUPP;
		
		if(!dbRDUPP)
			dbRDUPP = NewDataBrowserReceiveDragUPP(DBReceiveDrag);
		dcb.u.v1.receiveDragCallback = dbRDUPP;
		
		if(!postProcessDragCallback)
			postProcessDragCallback = NewDataBrowserPostProcessDragUPP(DBPostProcessDrag);
		dcb.u.v1.postProcessDragCallback = postProcessDragCallback;

		SetDataBrowserCallbacks(ul->browser, &dcb);
		SetDataBrowserHasScrollBars(ul->browser, false, true);
		SetDataBrowserTableViewNamedColumnWidth(ul->browser, 'name', ul->nickListWidth);
		SetDataBrowserTableViewNamedColumnWidth(ul->browser, 'host', ul->uwinSize.right - ul->nickListWidth);
		
		SetDataBrowserSelectionFlags(ul->browser, kDataBrowserDragSelect | kDataBrowserCmdTogglesSelection);
		SetDataBrowserTableViewHiliteStyle(ul->browser, kDataBrowserTableViewFillHilite);
		
		SetControlFontStyle(ul->browser, &controlFontStyleStruc);
		SetControlProperty(ul->browser, kUserlistSignature, kUserlistSignature, sizeof(ULI), &ul);
		SetDataBrowserTableViewRowHeight(ul->browser, 12);
		
		InitializeDataBrowserCustomCallbacks(&dccb, kDataBrowserLatestCustomCallbacks);
		if(!dbCDIUPP)
			dbCDIUPP = NewDataBrowserDrawItemUPP(DBCustomDrawItem);
		dccb.u.v1.drawItemCallback = dbCDIUPP;
		
		if(!acceptDragCallback)
			acceptDragCallback = NewDataBrowserItemAcceptDragUPP(DBCAcceptDrag);
		dccb.u.v1.acceptDragCallback = acceptDragCallback;
		
		if(!receiveDragCallback)
			receiveDragCallback = NewDataBrowserItemReceiveDragUPP(DBCReceiveDrag);
		dccb.u.v1.receiveDragCallback = receiveDragCallback;
	
		SetDataBrowserCustomCallbacks(ul->browser, &dccb);
		
		SetAutomaticControlDragTrackingEnabledForWindow(ul->uwin, type == ulGlobal);
	}
	
	GetPort(&gp);
	SetPortWindowPort(ul->uwin);

	SetPort(gp);

	if(type == ulMessageWindow)
	{
		MWPaneResize(mw);
		if(IsWindowVisible(mw->w))
			MWPaneUpdate(mw);
	}
	
	if(type == ulGlobal) //don't show the window until it's completely set up.
	{
		if(mainPrefs->userListOpen)
			ShowWindow(ul->uwin);
	}
	
	return ul;
}

static void ULIPaneDrawBorder(ULI ul, mwPanePtr o, char pressed)
{
	if(ul->ulType == ulMessageWindow && o && IsWindowVisible(ul->uwin))
	{
		Rect r;
		long state;
		RGBColor front;

		GetForeColor(&front);
		if(IsWindowActive(ul->uwin))
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
		else if(IsWindowActive(ul->uwin))
			state = kThemeStateActive;
		else
			state = kThemeStateInactive;
		
		DrawBorder(&r, state, true);
		ValidWindowRect(ul->uwin, &r);

		//Draw border
		r = o->drawArea;
		
		r.top--;
		if(o->drawArea.top != 0)
		{
			MoveTo(r.left, r.top);
			LineTo(r.right, r.top);
		}
		
// ¥¥¥
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

		
		MoveTo(r.right, r.top);
		LineTo(r.right, r.bottom);
		LineTo(r.left, r.bottom);
		LineTo(r.left, r.top);
		LineTo(r.right, r.top);
		ValidWindowRect(ul->uwin, &r);
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

static void SULPaneUpdate(pMWPaneUpdateData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		
		if(ul)
		{
			ULIPaneDrawBorder(ul, p->pane, resizingPane);
		}
	}
}

static void ULPaneResize(ULI ul, mwPanePtr o, Point where, UInt32 modifiers)
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
	
	dragResult = DragGrayRgn(dragRgn, where, &dragRect, &dragRect, hAxisOnly, 0);
	DisposeRgn(dragRgn);
	
	dragResult &= 0xFFFF;
	if(dragResult != 0x8000)
	{
		if(ul->rightSide)
			o->askedWidth = o->drawArea.right - o->drawArea.left - dragResult;
		else
			o->askedWidth = o->drawArea.right - o->drawArea.left + dragResult;
		
		MWPaneRecalculate(o->mw);

		if(modifiers & shiftKey)
			mainPrefs->userlistInWindowWidth = o->givenWidth;

		MWPaneResize(o->mw);
		MWPaneUpdate(o->mw);
	}
	
	resizingPane = false;
	ULIPaneDrawBorder(ul, o, false);
}

static void SULPaneClick(pMWPaneClickData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		ControlHandle c;
		long l;
		mwPanePtr o = p->pane;
		
		if((l = FindControl(p->where, ul->uwin, &c)) != 0)
		{
			HandleControlClick(c, p->where, p->modifiers, NULL);
		}
		else
		{
			if((ul->rightSide && (p->where.h <= o->drawArea.left + kInWindowBorder )) || (!ul->rightSide && (p->where.h > o->drawArea.right - kInWindowBorder)))
			{
				ULPaneResize(ul, o, p->where, p->modifiers);
			}
			else
			{
				p->where.h -= p->pane->drawArea.left;
				p->where.v -= p->pane->drawArea.top;
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
			if(p->activate)
				ActivateControl(ul->browser);
			else
				DeactivateControl(ul->browser);

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
		
		r = o->drawArea;
		if(ul->rightSide)
			r.left += kInWindowBorder;
		else
			r.right -= kInWindowBorder;
		
		if(!IsWindowActive(ul->uwin))
			DeactivateControl(ul->browser);
		else
			ActivateControl(ul->browser);
		
		SetControlBounds(ul->browser, &r);
	}
}

static void SULPaneDestroy(pMWPaneDestroyData *p)
{
	if(p->pane->type == kUserlistPane)
	{
		ULI ul = (ULI)p->pane->data;
		
		ULIDestroy(ul);
		p->pane->data = 0;
	}
}

static void ULIKillAll(void)
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

static void ULIAddAll(void)
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

INLINE void IdleCursor(void)
{
	static int inSep = 0;
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
		GetMouse(&pt);
		FindWindow(pt, &w);
		if(!w || !IsWindowActive(w))
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
	}
	else
		SetIdleThreshold(kIdleNormal);
}

static void ULCloseWindow(ULI ul)
{
	HideWindow(ul->uwin);
	mainPrefs->userListOpen = false;
	CheckMenuItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType), mainPrefs->userListOpen);
}

INLINE void ULWindowMenuSelect(pServiceWindowMenuData *p)
{
	if(p->key == userlistServiceType)
	{
		if(globalUserlist)
		{
			if(gUserlist)
			{
				if(IsWindowVisible(gUserlist->uwin)) //hide it
				{
					mainPrefs->userListOpen = false;
					HideWindow(gUserlist->uwin);
				}
				else //show it
				{
					mainPrefs->userListOpen = true;
					ShowWindow(gUserlist->uwin);
				}

				CheckMenuItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType), mainPrefs->userListOpen);
			}
			else //make it?
			{
			}
		}
		else //in-window
		{
			MWPtr mw = GetActiveMW();
			
			if(mw && mw->winType == chanWin)
			{
				mwPanePtr o = MWFindPane(mw, kUserlistPane);
				char check;
				
				if(o) //kill it
				{
					ULIDestroy((ULI)o->data);
					MWDestroyPane(o);
					MWPaneResize(mw);
					MWPaneUpdate(mw);
					check = 0;
				}
				else //add it
				{
					ULI ul = ULINew(mw->w, ulMessageWindow);
					MWPaneRecalculate(mw);
					MWPaneUpdate(mw);
					ListGenerate(ul, MWGetChannel(mw));
					check = 1;
				}
				
				CheckMenuItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType), check);
			}
		}
	}
}

//This is always going to be used for the global list
static void ULResizeWin(ULI ul, short height, short wid)
{
	if(ul->uwinSize.bottom != height || ul->uwinSize.right != wid) //size changed
	{
		ul->uwinSize.bottom = height;
		ul->uwinSize.right = wid;
		
		SetControlBounds(ul->browser, &ul->uwinSize);
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

char dInWindowRight;

INLINE void PWSet(pPWSetWindowData *p)
{
	if(p->id == prefsPanel)
	{
		setCheckBox(p->PrefsDlg, 4, mainPrefs->userlistInWindow);
		setCheckBox(p->PrefsDlg, 5, mainPrefs->userlistInWindowRight);
	}
}

INLINE void PWGet(pPWGetWindowData *p)
{
	if(p->id == prefsPanel)
	{
		mainPrefs->userlistInWindow = getCheckBox(p->PrefsDlg, 4);
		mainPrefs->userlistInWindowRight = getCheckBox(p->PrefsDlg, 5);
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
		MWPtr mw;
		
		if(globalUserlist) //kill all in-windows and turn off message
		{
			sidr->yourInfo->captureMessages[pMWNewMessage] = 0;
			ULIKillAll();
			mainPrefs->userListOpen = 1;
			CreateGlobalUserlistWindow();
			mw = GetActiveMW();
			if(mw && mw->winType == chanWin)
				ListGenerate(gUserlist, MWGetChannel(mw));
			
			EnableMenuItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType));
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
	}
	
	dInWindowRight = mainPrefs->userlistInWindowRight;
}

#pragma mark -

static void displayOldVersionMsg(void)
{
	LongString ls;
	
	LSStrLS("\pThe userlist plugin requires ShadowIRC 1.1 or later.", &ls);
	if(!IsWindowVisible((*sidr->consoleWin)->w))
		ShowWindow((*sidr->consoleWin)->w);
	SMPrefixIrcleColor(&ls, dsConsole, '2');
}

static void displayMultipleUserlistsMsg(void)
{
	LongString ls;
	
	LSStrLS("\pYou have more than one userlist plugin installed.", &ls);
	if(!IsWindowVisible((*sidr->consoleWin)->w))
		ShowWindow((*sidr->consoleWin)->w);
	SMPrefix(&ls, dsConsole);
}

INLINE void setupMessages(char captureMessages[numMessages])
{
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

static void ULDoIdle(EventLoopTimerRef timer, void* data)
{
	IdleCursor();
}

static void SetIdleThreshold(long ticks)
{
	static EventLoopTimerUPP timerUPP = 0;
	static EventLoopTimerRef timer = 0;
	EventLoopRef mainLoop = GetMainEventLoop();
	
	if(timerUPP == 0)
		timerUPP = NewEventLoopTimerUPP(ULDoIdle);
	
	if(timer)
	{
		RemoveEventLoopTimer(timer);
		timer = 0;
	}
	
	if(ticks)
		InstallEventLoopTimer(mainLoop, TicksToEventTime(ticks), TicksToEventTime(ticks), timerUPP, NULL, &timer);
}

void pluginMain(ShadowIRCDataRecord* sidrIN)
{
	long l;

	switch(sidrIN->message)
	{
		case pVersionCheckMessage:
			sidr=sidrIN;
			mainPrefs = sidrIN->mainPrefs;
			shadowircColors = sidrIN->shadowircColors;
			
			l=((pVersionCheckDataPtr)sidrIN->messageData)->version;
			if(l<0x0200000F) //2.0a15
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
				SetIdleThreshold(kIdleNormal);
				
				dInWindowRight = mainPrefs->userlistInWindowRight;
				prefsPanel = PMLAdd("\pUserlist");

				globalUserlist= !mainPrefs->userlistInWindow;
				//Create userlist window
				if(globalUserlist)
					CreateGlobalUserlistWindow();
				else
					DisableMenuItem(GetMenuHandle(260), WMSGetMenuItemNum(userlistServiceType));
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

		case pServiceWindowMenu:
			ULWindowMenuSelect((pServiceWindowMenuPtr)sidrIN->messageData);
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
		
		case pShortcutProcessMessage:
			ProcessShortcuts((pShortcutProcessDataPtr)sidrIN->messageData);
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
		
		case pCMPopupsMessage:
			//ULContextualMenu((pCMPopupsDataPtr)sidrIN->messageData);
			break;
		
		case pCMPopupsReturnMessage:
			//ULContextualMenuProcess((pCMPopupsReturnDataPtr)sidrIN->messageData);
			break;
	}
}
