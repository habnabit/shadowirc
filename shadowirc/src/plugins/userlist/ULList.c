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

void ListRefresh(ULI ul)
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
	
	NumToString(num, s2);
	pstrcpy(ch->chName, s);
	SAppend1(s, ':');
	SAppend1(s, ' ');
	pstrcat(s, s2, s);
	pstrcat(s, "\p Users", s);
	
	if(ch->numUsers == 1)
		s[0]--;
	
	GetWTitle(ul->uwin, s2);
	if(!pstrcmp(s, s2))
		SetWTitle(ul->uwin, s);
}