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

#include "userlist.h"
#include "ULList.h"

#ifdef __POWERPC__
#include "SmartScrollAPI.h"
#endif

CIconHandle gSortForwardIcon, gSortReverseIcon;

inline char pstrgt(Str255 one, Str255 two);
static pascal void pstrcpyucase(Str255 src, Str255 dest);
inline char inupc(char c);

static pascal void ListScrollbarSet(ULI ul);

pascal GrafPtr ULISetupDrawing(ULI ul)
{
	GrafPtr gp;
	
	GetPort(&gp);
	SetPort(ul->uwin);
	if(ul->ulType == ulGlobal)
	{
		if(ul->scrollbarLeft)
		{
			SetOrigin(-(scrollbarWidth-2),0);
		}
	}
	else if(ul->ulType == ulMessageWindow)
	{
		mwPanePtr o = ul->pane;
		if(o)
		{
			SetOrigin(-(o->drawArea.left + (ul->rightSide?kInWindowBorder:0) + (ul->scrollbarLeft?scrollbarWidth-1:0)), -o->drawArea.top);
			TextFont(genevaNum);
			TextSize(9);
		}
	}
	
	return gp;
}

#if !enableinline
INLINE void ULIFinishDrawing(ULI ul, GrafPtr gp)
{
#pragma unused(ul)
	SetOrigin(0,0);
	SetPort(gp);
}
#endif

pascal void DrawWindowHeader(Rect *r, long state)
{
	if(APPEARANCE11)
	{
		DrawThemePlacard(r, state);
	}
	else //whether it has appearance 1.x or not, it looks the same...
	{
		DrawPlacard(r, state); //looks the same under platinum anyways
	}
}

#pragma mark -

pascal void ListHeader(ULI ul)
{
	Rect r;
	Ptr offscreen;
	GrafPtr gp;
	Str255 s;
	RGBColor back;
	long state;
	char active;
	int sortWidgetLeft;
	
	if(!WIsVisible(ul->uwin))
		return;
	
	if(ul->ulType == ulMessageWindow)
	{
		if(ul->uwinSize.right - ul->uwinSize.left <= kInWindowBorder)
			return;
		else if(!ul->rightSide)
		{
			if(ul->uwinSize.right - ul->uwinSize.left <= scrollbarWidth + kInWindowBorder)
				return;
		}
 	}
	
	gp = ULISetupDrawing(ul);
	GetBackColor(&back);
	r.top = 0;
	r.bottom = kHeaderSize + 1;
	
	if(ul->scrollbarLeft)
	{
		r.left = -scrollbarWidth + 1;
		r.right = ul->uwinSize.right - scrollbarWidth + 1;
	}
	else
	{
		r.left = 0;
		r.right = ul->uwinSize.right;
	}

	WValidRect(ul->uwin, &r);
	StartDrawingOffscreen(&offscreen, &r, false);

	active = WIsActive(ul->uwin);
	
	if(active)
		state = kThemeStateActive;
	else
		state = kThemeStateInactive;
	
	sortWidgetLeft = ul->uwinSize.right + 1 - scrollbarWidth;
	if(ul->ulType != ulMessageWindow)
		sortWidgetLeft++;
	
	//Draw userhost
	r.top = -1;
	r.right = sortWidgetLeft + 1;
	r.left = ul->nickListWidth -4;
	if(ul->sort == kSortUserhost && active)
		DrawWindowHeader(&r, kThemeStatePressed);
	else
		DrawWindowHeader(&r, state);
	
	r.right = r.left + 1;
	r.left = -1;
	if(r.right > sortWidgetLeft)
		r.right = sortWidgetLeft + 1;
	
	if(ul->sort == kSortNick && active)
		DrawWindowHeader(&r, kThemeStatePressed);
	else
		DrawWindowHeader(&r, state);

	if(active)
		SetTextColor(kThemeTextColorWindowHeaderActive);
	else
		SetTextColor(kThemeTextColorWindowHeaderInactive);

	MoveTo(kLeftOffset, kHeaderText);

	DrawString("\pNickname");
	MoveTo(ul->nickListWidth, kHeaderText);
	pstrcpy("\pHostname", s);
	TruncString(ul->uwinSize.right - scrollbarWidth - ul->nickListWidth, s, truncEnd);
	DrawString(s);
	TextMode(srcCopy);

	//draw the sort widget
	if(ul->scrollbarLeft)
	{
		if(ul->rightSide)
		{
			r.left = -scrollbarWidth + 1;
			r.right = 1;
		}
		else
		{
			if(ul->ulType == ulGlobal)
			{
				r.left = -scrollbarWidth;
				r.right = 1;
			}
			else
			{
				r.left = -scrollbarWidth -1;
				r.right = 0;
			}
		}
	}
	else
	{
		r.right = ul->uwinSize.right + 1;
		r.left = sortWidgetLeft;
	}

	DrawWindowHeader(&r, state);
	
	RGBBackColor(&white);
	RGBForeColor(&black);

/*
	r.top = 3;
	r.bottom = 11;
	r.right -= 4;
	r.left = r.right - 8;

	if(!ul->sortReverse)
		PlotCIcon(&r, gSortForwardIcon);
	else
		PlotCIcon(&r, gSortReverseIcon);
*/

	r.top = 3 - 4;
	r.bottom = 11 + 4;
	r.left = r.right - 16;
	if(!ul->sortReverse)
		PlotIconID(&r, kAlignAbsoluteCenter, kTransformNone, 500);
	else
		PlotIconID(&r, kAlignAbsoluteCenter, kTransformNone, 501);

	RGBBackColor(&back);
	TextMode(srcOr);
	EndDrawingOffscreen(offscreen);
	ULIFinishDrawing(ul, gp);
}

pascal void ListDrawUser(ULI ul, int num, short po)
{
	MyListPtr item = &(**ul->users).u[num];
	UserListPtr user = item->u;
	Str255 s;
	Style face = 0;
	Rect r;
	char color = 0;
	
	//Assume initial setup ok. Draw the item.
	if(user->isOper)
	{
		face |= italic;
		if(!user->isOp)
			RGBForeColor(&shadowircColors[sicUserOper]);
		color = 1;
	}
	if(user->isOp)
	{
		face |= bold;
		RGBForeColor(&shadowircColors[sicUserChannelOp]);
	}
	else if((user->hasVoice || user->isHalfOp) && !user->isOper)
	{
		face |= underline;
		RGBForeColor(&shadowircColors[sicUserVoice]);
	}
//	else if(user->isIgored)
//		RGBForeColor(&shadowircColors[sicUserIgnored]);
	else
		if(!color)
			RGBForeColor(&shadowircColors[sicUserRegular]);
		
	MoveTo(kLeftOffset, po * line  + kTopOffset - 1);

	if(user->isAway)
	{
		pstrcpy(user->nick, &s[1]);
		s[0] = s[1] +1;
		s[1] = '(';
		TruncString(ul->nickListWidth - kLeftOffset - 17, s, truncEnd);
		s[++s[0]] = ')';
	}
	else
	{
		pstrcpy(user->nick, s);
		TruncString(ul->nickListWidth - kLeftOffset - 12, s, truncEnd);
	}
	
	TextFace(face);
	DrawString(s);
	TextFace(0);
	
	MoveTo(ul->nickListWidth, po * line + kTopOffset - 1);
	RGBForeColor(&shadowircColors[sicUserRegular]);
	DrawString(user->userhost);
	
	if(user->userlistIsSelected)
	{
		r.top = (po-1) * line  + kTopOffset +1;
		r.bottom = r.top + line;
		r.left = 2;
		r.right = ul->uwinSize.right - (kRightOffset - 2);
		
		//¥¥This isn't working, look at later: the unsorted column doesn't get hilighted
		//LMSetHiliteMode(LMGetHiliteMode() & 0x7F);
		InvertRect(&r);
	}

}

pascal char ListCanDraw(ULI ul, long num)
{
	//Return true if the num is visible, false if not
	int low = GetControlValue(ul->bar);
	
	if(num < low) //it is off the top of the list
		return false;
	else
	{
		return num - low <= ((ul->uwinSize.bottom - kBottomOffset - kTopOffset) / line);
	}
}

pascal void ListDrawOne(ULI ul, long num)
{
	GrafPtr gp;
	Ptr offscreen;
	Rect drawRect, sortRect;
	int p;
	RGBColor back;
	
	if(ListCanDraw(ul, num) && WIsVisible(ul->uwin))
	{
		//Set up the graphics area and draw it
		gp = ULISetupDrawing(ul);

		p = num - GetControlValue(ul->bar);

		drawRect.top = kTopOffset + p * line;
		drawRect.left = 1;
		drawRect.bottom =  drawRect.top + 2 + line;
		drawRect.right = ul->uwinSize.right - (kRightOffset -2);
		
		if(drawRect.bottom >= ul->uwinSize.bottom)
			drawRect.bottom = ul->uwinSize.bottom - 1;
		
		if(num>0 && (**ul->users).u[num-1].u->userlistIsSelected)
			drawRect.top++;
		
		StartDrawingOffscreen(&offscreen, &drawRect, false);

		GetBackColor(&back);
		SetBackground(kThemeBrushListViewBackground);
		EraseRect(&drawRect);
		
		//Set sort
		SetBackground(kThemeBrushListViewSortColumnBackground);
		sortRect = drawRect;
		if(ul->sort == kSortNick)
			sortRect.right = ul->nickListWidth - 4;
		else// if(ul->sort == kSortUserhost)
			sortRect.left = ul->nickListWidth -4;
		EraseRect(&sortRect);

		ListDrawUser(ul, num, p+1);
		EndDrawingOffscreen(offscreen);
		RGBBackColor(&back);
		ULIFinishDrawing(ul, gp);
	}
}

pascal void ListDrawOneUser(ULI ul, UserListPtr u)
{
	long num = ListFind(ul, u);
	
	if(num != -1)
		ListDrawOne(ul, num);
}

pascal void ListDraw(ULI ul)
{
	GrafPtr gp;
	Rect drawRect, sortRect;
	Ptr offscreen;
	short bot;
	int x, m, c, highVis;
	ulPtr u;
	RGBColor back;
	RgnHandle rgn;
	
	ul->updateList = 0;
	ul->lastUpdate = TickCount();
	
	if(!WIsVisible(ul->uwin))
		return;
	
	ListScrollbarSet(ul);

	drawRect.left = 1;
	drawRect.top = kTopOffset;
	drawRect.right = ul->uwinSize.right - (kRightOffset - 3);
	drawRect.bottom = ul->uwinSize.bottom;

	if(EmptyRect(&drawRect))
		return;

	gp = ULISetupDrawing(ul);
	GetBackColor(&back);

	bot = drawRect.bottom - kTopOffset;
	
	WValidRect(ul->uwin, &drawRect);
	StartDrawingOffscreen(&offscreen, &drawRect, false);
	
	SetBackground(kThemeBrushListViewBackground);
	EraseRect(&drawRect);
	
	//Set sort
	SetBackground(kThemeBrushListViewSortColumnBackground);

	sortRect = drawRect;
	if(ul->sort == kSortNick)
		sortRect.right = ul->nickListWidth - 4;
	else// if(ul->sort == kSortUserhost)
		sortRect.left = ul->nickListWidth - 4;

	EraseRect(&sortRect);
	
	if(ul->users && (**ul->users).ch)
	{
		drawRect.right-=1;
		drawRect.bottom-=2;
		rgn = NewRgn();
		GetClip(rgn);
		ClipRect(&drawRect);
		HLock((Handle)ul->users);
		
		u = *ul->users;
		m = u->num;
		
		x = GetControlValue(ul->bar) ;
		c=1;
		highVis = bot / line + x;
		while(x < m && x <= highVis)
			ListDrawUser(ul, x++, c++);
		
		SetClip(rgn);
		DisposeRgn(rgn);

		HUnlock((Handle)ul->users);
	}

	EndDrawingOffscreen(offscreen);
	RGBBackColor(&back);
	ULIFinishDrawing(ul, gp);
}

pascal void ListGenerate(ULI ul, channelPtr ch)
{
	//Add everything to the list, then sort it. Prolly should do insertion sort.
	long x, n;
	long siz;
	ulPtr u;
	UserListPtr up;
	
	if(!ul)
		return;
	
	if(ul->users)
		DisposeHandle((Handle)ul->users);
	
	if(!ch)
	{
		ul->users = 0;
		return;
	}
	
	n = ch->numUsers;
	siz = sizeof(UserListBegin) + (n * sizeof(MyList));
	ul->users = (ulHand)NewHandle(siz);
	
	HLock((Handle)ul->users);
	u=*ul->users;
	u->ch = ch;
	
	u->num = n;
	
	up = ch->userlist;
	for(x = 0; x<n;x++)
	{
		u->u[x].u = up;
		up=up->next;
	}
	
	//Everything is added. Sort.
	ListSort(ul, true);
	
	//Draw
	ListDraw(ul);
	ListSetWTitle(ul);
	HUnlock((Handle)ul->users);
}

#pragma dont_inline off
inline char pstrgt(Str255 one, Str255 two)
{
	int i;
	
	one[one[0]+1] = 0;
	two[two[0]+1] = 0;
	
	if(one[0] && !two[0])
		return false;
	else if(two[0] && !one[0])
		return true;
	
	i=1;
	for(;;)
	{
		if(one[i] == two[i])
			i++;
		else
			return one[i] > two[i];
	}
}

inline char inupc(char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

static pascal void pstrcpyucase(Str255 src, Str255 dest)
{
	int x;
	int n = src[0];
	
	dest[0]=src[0];
	for(x=1;x<=n;x++)
		dest[x]=inupc(src[x]);
}


pascal char ListSortPart(ULI ul, MyList *one, MyList *two)
{
	Str255 name, name2;
	char ret;
	
	if(ul->sort == kSortNick || (ul->sort == kSortUserhost && !two->u->userhost[0]))
	{
		pstrcpyucase(one->u->nick, name);
		pstrcpyucase(two->u->nick, name2);
	}
	else //if(ul->sort == kSortUserhost)
	{
		pstrcpyucase(one->u->userhost, name);
		pstrcpyucase(two->u->userhost, name2);
	}
//	else
//		return false;
	
	ret = pstrgt(name, name2);

	if(!ul->sortReverse)
		return ret;
	else
		return !ret;
}
#pragma dont_inline reset

enum {
	kMoveLeft,
	kMoveRight
};

pascal char ListResortInd(ULI ul, int num)
{
	MyList my;
	int x;
	int max;
	int cnt = 0;
	char move;
	ulPtr u;
	
	//Ok...this nick needs to be resorted. We do this as follows:
	//If the nick is less than the previous one, we shift it up and loop.
	//else if it is greater than the next one, we shift it down and loop.
	//else it has remained in sorted order.
	
	HLock((Handle)ul->users);
	u = *ul->users;
	
	if(num == 0) //first
		move = kMoveRight;
	else if(num == u->num-1) //last
		move = kMoveLeft;
	else if(ListSortPart(ul, &u->u[num], &u->u[num+1])) //this is greater than the one on the right
		move = kMoveRight;
	else
		move = kMoveLeft;
	
	my = u->u[num];
	//The problem with this is that it bubbles the thing down, rather than
	//BlockMoveData()ing stuff around. Not horribly inefficient since these
	//are longs, but it can add up if something's getting moved a long distance.
	if(move == kMoveRight)
	{
		max = u->num - 1;
		for(x=num; x<max;x++)
		{
			if(ListSortPart(ul, &my, &u->u[x+1])) //swap
			{
				u->u[x] = u->u[x+1];
				u->u[x+1] = my;
				cnt++;
			}
			else
				break;
		}
	}
	else //move left
	{
		for(x=num - 1; x>=0;x--)
		{
			if(ListSortPart(ul, &u->u[x], &my)) //swap
			{
				u->u[x+1] = u->u[x];
				u->u[x] = my;
				cnt++;
			}
			else
				break;
		}
	}
	
	HUnlock((Handle)ul->users);
	return cnt != 0;
}

pascal void ListSort(ULI ul, char locked)
{
	ulPtr u;
	long i, j;
	long max, max2;
	MyList temp;
	
	if(!ul->users || !(**ul->users).ch)
		return;
	
	if(!locked)
		HLock((Handle)ul->users);
	
	u = *ul->users;
	max2 = max = u->ch->numUsers -1;
	
	//This is a bubblesort. eeewwwie
	for(i=0;i<max;i++)
	{
		for(j=0;j<max2;j++)
		{
			if(ListSortPart(ul, &u->u[j], &u->u[j+1]))
			{
				temp = u->u[j];
				u->u[j] = u->u[j+1];
				u->u[j+1] = temp;
			}
		}
		max2--;
	}
	
	if(!locked)
		HUnlock((Handle)ul->users);
}

pascal void ListTrash(ULI ul)
{
	if(ul && ul->users)
	{
		SetControlMaximum(ul->bar, 0);
		DisposeHandle((Handle)ul->users);
		ul->users = 0;
	}
}

pascal void ListAdd(ULI ul, UserListPtr u)
{
	int i, max;
	ulPtr us;
	MyList itemToAdd;
	
	//Add this user to the list. Assume list is pre-sorted, and add it in sorted order.
	//SetHandleSize((Handle)ul->users, GetHandleSize((Handle)ul->users) + sizeof(MyList));

	SetHandleSize((Handle)ul->users, sizeof(UserListBegin) + (sizeof(MyList) * ++(**ul->users).num));
	HLock((Handle)ul->users);
	us = *ul->users;
	
	itemToAdd.u = u;
	max = us->num - 1;
	
	//This is linear search. Should do a binary search, since we're searching
	//a string!
	for(i=0;i<max;i++)
		if(!ListSortPart(ul, &itemToAdd, &us->u[i]))
			break;
	
	//so this one is greater than x
	//which means x should be inserted before it
	
	BlockMoveData(&us->u[i], &us->u[i+1], sizeof(MyList) * (max-i));
	us->u[i] = itemToAdd;

	ListSetWTitle(ul);
	HUnlock((Handle)ul->users);
}

pascal void ListDel(ULI ul, UserListPtr u)
{
	//Delete this user from the list.
	ulPtr us;
	int x, n;
	
	HLock((Handle)ul->users);
	us = *ul->users;
	
	//Linear search. Not too concerned about making this binary since it's
	//only comparing a long and not a string
	n = us->num;
	for(x=0;x<n;x++)
		if(us->u[x].u == u)
			break;
	
	if(n == x)
	{
		HUnlock((Handle)ul->users);
		return;
	}
	
	//x == the number of the user we delete.
	//To delete 'em from the list, BlockMoveData everything down over top of it.
	BlockMoveData(&us->u[x+1], &us->u[x], sizeof(MyList) * (n - x - 1));

	us->num--;
	ListSetWTitle(ul);
	HUnlock((Handle)ul->users);
	SetHandleSize((Handle)ul->users, sizeof(UserListBegin) + (sizeof(MyList) * us->num)); //GetHandleSize((Handle)ul->users) - sizeof(MyList));
}

pascal char ListDelName(ULI ul, Str255 nick)
{
	//Find the nick, and then delete it.
	if(ul->users && (**ul->users).ch)
	{
		UserListPtr u = ULFindUserName((**ul->users).ch, nick);
		if(u)
		{
			ListDel(ul, u);
			return true;
		}
	}
	
	return false;
}

pascal long ListFind(ULI ul, UserListPtr u)
{
	
	if(ul->users && (**ul->users).ch)
	{
		if(ul->sort != kSortNick)
		{
			int x, m;
	
			//shit search
			m = (**ul->users).num;
			for(x=0;x<m;x++)
				if((**ul->users).u[x].u == u)
					return x;
		}
		else //binary search
		{
			int low, high, mid;
			MyList *us = (**ul->users).u;
			MyList *u1, u2;
			MyList *a, *b;

			low = 0;
			high = (**ul->users).num -1;

			u2.u=u;
			while(low <= high)
			{
				mid = (low + high)/2;
				
				if(us[mid].u == u)
					return mid;
				else
				{
					u1 = &us[mid];
					if(ul->sortReverse)
						a = u1, b = &u2;
					else
						a = &u2, b = u1;
					
					if(ListSortPart(ul, a, b)) // a is greater than b, so search high end
						low = mid + 1;
					else
						high = mid - 1;
				}
			}
		}
	}
	
	return -1;
}

#pragma mark -

static pascal void ListScrollbarSet(ULI ul)
{
	long total, extra;
	//Set the maximum to the number of users in the list that aren't visible
	
	if(!ul->users || !(**ul->users).ch || !(**ul->users).num)
	{
		if(APPEARANCE11)
			SetControl32BitMaximum(ul->bar, 0);
		else
			SetControlMaximum(ul->bar, 0);
		return;
	}
	
	ul->visLines = ul->uwinSize.bottom / line - 1;
	total = (**ul->users).num * line;
	extra = total - ul->uwinSize.bottom + kTopOffset + kBottomOffset;

	if(APPEARANCE11)
	{
		 if(extra <= 0)
		 	SetControl32BitMaximum(ul->bar, 0);
		 else
		 	SetControl32BitMaximum(ul->bar, extra / line + 1);
		 
		 SetControl32BitValue(ul->bar, GetControlValue(ul->bar));
		 SetControlViewSize(ul->bar, ul->visLines);
	}
	else
	{
		 if(extra <= 0)
		 	SetControlMaximum(ul->bar, 0);
		 else
		 	SetControlMaximum(ul->bar, extra / line + 1);

		SetSmartScrollInfo(ul->bar, ul->visLines, total);
	}
}

pascal void ListSetWTitle(ULI ul)
{
	Str255 s;
	Str255 s2;
	char x;
	channelPtr ch;
	
	if(!ul || ul->ulType != ulGlobal)
		return;
	
	if(!ul->users)
		ch = 0;
	else
		ch = (**ul->users).ch;
	
	if(!ch)
	{
		GetWTitle(ul->uwin, s2);
		if(!pstrcmp(s2, "\pUserlist"))
			SetWTitle(ul->uwin, "\pUserlist");
		return;
	}
	
	if(ch && !ch->numUsers) //get it next time.
		return;
	
	NumToString(ch->numUsers, &s[ch->chName[0]+2]);
	pstrcpy(ch->chName, s);
	s[0] = ch->chName[0] + s[ch->chName[0]+2] + 2;
	*(short*)&s[ch->chName[0]+1] = ': ';
	pstrcpy("\pUsers", &s[s[0]+1]);
	x = s[s[0]+1];
	s[s[0]+1] = ' ';
	s[0] += x + 1;
	
	if(ch->numUsers == 1)
		s[0]--;
	
	GetWTitle(ul->uwin, s2);
	if(!pstrcmp(s, s2))
		SetWTitle(ul->uwin, s);
}