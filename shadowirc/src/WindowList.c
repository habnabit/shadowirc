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

#define WINDOWLIST
#define MSGWINDOWS

#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "WindowList.h"
#include "utils.h"
#include "MenuCommands.h"
#include "inline.h"
#include "channels.h"

wmItemsData wmItems = {0,0,0,0};

pascal void wmItemsInit(void)
{
	wmItems.channels=(wmListHand)NewHandle(0);
	wmItems.rest=(wmListHand)NewHandle(0);
}

pascal void wmChange(MWPtr mw, ConstStr255Param newName)
{
	int x;
	MenuHandle mh=gWindowMenu;
	
	SetWTitle(mw->w, newName);
	if(mw->winType==chanWin)
	{
		for(x=0;x<wmItems.channelsLength;x++)
			if(mw==(**wmItems.channels).mw[x])
			{
				SetMenuItemText(mh, 1+windowsStart + x, newName);
				break;
			}
	}
	else
	{
		for(x=0;x<wmItems.restLength;x++)
			if(mw==(**wmItems.rest).mw[x])
			{
				SetMenuItemText(mh, 1+windowsStart2 + x, newName);
				break;
			}
	}
}

pascal void wmDelete(MWPtr mw)
{
	int x,y;
	MenuHandle mh=gWindowMenu;
	
	if(mw->winType==chanWin)
	{
		for(x=0;x<wmItems.channelsLength;x++)
			if(mw==(**wmItems.channels).mw[x])
			{
				DeleteMenuItem(mh, 1+windowsStart + x);
				for(y=x+1;y<wmItems.channelsLength;y++)
					(**wmItems.channels).mw[y-1]=(**wmItems.channels).mw[y];

				SetHandleSize((Handle)wmItems.channels, --wmItems.channelsLength * sizeof(MWPtr));
				break;
			}
	}
	else
	{
		for(x=0;x<wmItems.restLength;x++)
			if(mw==(**wmItems.rest).mw[x])
			{
				DeleteMenuItem(mh, 1+windowsStart2 + x);
				for(y=x+1;y<wmItems.restLength;y++)
					(**wmItems.rest).mw[y-1]=(**wmItems.rest).mw[y];

				SetHandleSize((Handle)wmItems.rest, --wmItems.restLength * sizeof(MWPtr));
				
				if(wmItems.restLength == 0) //last item, delete separator
					DeleteMenuItem(mh, windowsStart2);
				break;
			}
	}
}

pascal void wmAdd(MWPtr mw)
{
	int x;
	MenuHandle mh=gWindowMenu;
	char b=0;
	Str255 s;
	int item;
	
	if(mw->winType==chanWin)
	{
		for(x=0;x<wmItems.channelsLength;x++)
			if(mw==(**wmItems.channels).mw[x])
			{
				b=1;
				break;
			}
		
		if(!b)
		{
			SetHandleSize((Handle)wmItems.channels, ++wmItems.channelsLength * sizeof(MWPtr));
			(**wmItems.channels).mw[wmItems.channelsLength-1] = mw;
			
			item = windowsStart+wmItems.channelsLength;
			InsertMenuItem(mh, "\p-", item - 1);
			GetWTitle(mw->w, s);
			SetMenuItemText(mh, item, s);
			SetMenuItemCommandID(gWindowMenu, item, kCommandWindowSelect);
		}
	}
	else
	{
		for(x=0;x<wmItems.restLength;x++)
			if(mw==(**wmItems.rest).mw[x])
			{
				b=1;
				break;
			}
		
		if(!b)
		{
			SetHandleSize((Handle)wmItems.rest, ++wmItems.restLength * sizeof(MWPtr));
			(**wmItems.rest).mw[wmItems.restLength-1] = mw;
			
			if(wmItems.restLength == 1) //first extra item, add separator
				AppendMenu(mh, "\p-");
			
			item = windowsStart2+wmItems.restLength;
			
			AppendMenu(mh, "\p-");
			GetWTitle(mw->w, s);
			SetMenuItemText(mh, item, s);
			SetMenuItemCommandID(gWindowMenu, item, kCommandWindowSelect);
		}
	}
}

#pragma mark -

pascal MWPtr MWFindQuery(ConstStr255Param s, linkPtr link)
{
	MWPtr mw;
	Str255 s1;
	
	pstrcpyucase(s, s1);
	
	linkfor(mw, mwl)
		if(mw->link == link && mw->winType == queryWin)
			if(pstrcasecmp2(s1, mw->name))
				return mw;
	
	return 0;
}

pascal MWPtr MWFindDCC(ConstStr255Param s, linkPtr link)
{
	MWPtr mw;
	Str255 s1;
	
	pstrcpyucase(s, s1);
	if(s1[1] == '=')
	{
		pdelete(s1, 1, 1);
		s1[0]--;
	}
	
	linkfor(mw, mwl)
		if(mw->link == link && mw->winType == dccWin)
			if(pstrcasecmp2(s1, mw->name))
				return mw;
	
	return 0;
}

pascal MWPtr MWFindChannel(channelPtr ch)
{
	MWPtr mw;
	linkPtr link = ch->link;
	Str255 s1;
	
	pstrcpyucase(ch->chName, s1);
	
	linkfor(mw, mwl)
		if(mw->link == link && mw->winType == chanWin)
			if(mw->mwdata.chan == ch || pstrcasecmp2(s1, mw->name))
				return mw;
	
	return 0;
}
