/*
	ShadowIRC Userlist
	Copyright (C) 1997-2002 John Bafford
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

#define SAppend1(s,ch){(s)[++((s)[0])]=ch;}

char pstrgt(Str255 one, Str255 two)
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

/*
void ListDrawUser(ULI ul, int num, short po)
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
		SAppend1(s, ')');
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
*/

void ListDrawOne(ULI ul, UserListPtr u)
{
	ListResortInd(ul, u);
}

void ListDrawOneUser(ULI ul, UserListPtr u)
{
	DataBrowserItemID items;
	
	items = (DataBrowserItemID)u;
	UpdateDataBrowserItems(ul->browser, kDataBrowserNoItem, 1, &items, kDataBrowserItemNoProperty, kDataBrowserItemNoProperty);
}

void ListGenerate(ULI ul, channelPtr ch)
{
	//Add everything to the list, then sort it. Prolly should do insertion sort.
	UserListPtr up;
	
	if(!ul)
		return;
	
	if(!ch)
	{
		ul->ch = 0;
		return;
	}
	
	ul->ch = ch;
	
	RemoveDataBrowserItems(ul->browser, kDataBrowserNoItem, 0, NULL, kDataBrowserItemNoProperty);
	linkfor(up, ch->userlist)
	{
		DataBrowserItemID item;
		
		item = (DataBrowserItemID)up;
		AddDataBrowserItems(ul->browser, kDataBrowserNoItem, 1, &item, kDataBrowserItemNoProperty);
	}
	
	ListSetWTitle(ul);
}

void ListResortInd(ULI ul, UserListPtr u)
{
	DataBrowserItemID items;
	
	items = (DataBrowserItemID)u;
	UpdateDataBrowserItems(ul->browser, kDataBrowserNoItem, 1, &items, kDataBrowserItemNoProperty, kDataBrowserItemNoProperty);
}

void ListSort(ULI ul)
{
	ListResortInd(ul, kDataBrowserNoItem); //refresh everything
}

void ListTrash(ULI ul)
{
	if(ul && ul->ch)
	{
		RemoveDataBrowserItems(ul->browser, kDataBrowserNoItem, 0, NULL, kDataBrowserItemNoProperty);
		ul->ch = 0;
	}
}

void ListAdd(ULI ul, UserListPtr u)
{
	DataBrowserItemID items;
	
	items = (DataBrowserItemID)u;
	AddDataBrowserItems(ul->browser, kDataBrowserNoItem, 1, &items, kDataBrowserItemNoProperty);
	ListSetWTitle(ul);
}

void ListDel(ULI ul, UserListPtr u)
{
	DataBrowserItemID items;
	
	items = (DataBrowserItemID)u;
	RemoveDataBrowserItems(ul->browser, kDataBrowserNoItem, 1, &items, kDataBrowserItemNoProperty);
	ListSetWTitle(ul);
}

char ListDelName(ULI ul, Str255 nick)
{
	//Find the nick, and then delete it.
	if(ul->ch)
	{
		UserListPtr u = ULFindUserName(ul->ch, nick);
		if(u)
		{
			ListDel(ul, u);
			return true;
		}
	}
	
	return false;
}

#pragma mark -

void ListSetWTitle(ULI ul)
{
	Str255 s;
	Str255 s2;
	char x;
	channelPtr ch;
	long num;
	
	if(!ul || ul->ulType != ulGlobal)
		return;
	
	ch = ul->ch;
	
	if(!ch)
	{
		GetWTitle(ul->uwin, s2);
		if(!pstrcmp(s2, "\pUserlist"))
			SetWTitle(ul->uwin, "\pUserlist");
		return;
	}
	
	if(ch && !ch->numUsers) //get it next time.
		return;
	
	GetDataBrowserItemCount(ul->browser, kDataBrowserNoItem, false, 0, &num);
	
	NumToString(num, &s[ch->chName[0]+2]);
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