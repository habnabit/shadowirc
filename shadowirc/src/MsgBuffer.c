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

#include "IRCGlobals.h"
#include "utils.h"
#include "connections.h"
#include "MsgBuffer.h"
#include "Inline.h"

#pragma internal on
messageBufferPtr messagebuffers[10];
messageBufferRec mblist[10];
int mbnum;
char mbInput = false;

#pragma internal reset

pascal messageBufferPtr MBFindNick(linkPtr link, ConstStr255Param nick)
{
	int x;
	Str255 s1;
	
	pstrcpyucase(nick,s1);
	
	for(x=0;x<MAXMB;x++)
		if(messagebuffers[x]->nick[0])
			if((!link || messagebuffers[x]->link == link) && pstrcasecmp2(s1,messagebuffers[x]->nick))
				return messagebuffers[x];
	
	return 0;
}

pascal void MBNewMessage(linkPtr link, ConstStr255Param nick)
{
	messageBufferPtr mb=MBFindNick(link, nick);
	int p;
	
	if(mb)
	{
		for(p=MAXMB-1;(p>0) && mb!=messagebuffers[p];)p--; //Find the position of mb
		if(p>0)
		{
			for(;p>0;p--) //Move everything up
				messagebuffers[p]=messagebuffers[p-1];
			messagebuffers[0]=mb; //set to the current message
		}
		mb->lastmessage=now;
	}
	else //add a new one
	{
		//See if there's an empty one...
		for(p=0;(p<MAXMB) && messagebuffers[p]->nick[0];)p++;
		if(p>=MAXMB) //didn't find one in the buffer, so we bump the top and move them up
		{
			mb=messagebuffers[MAXMB-1];
			for(p=MAXMB-1;p>0;p--)
				messagebuffers[p]=messagebuffers[p-1];
		}
		else //there's an empty one
		{
			mb=messagebuffers[p];
			for(;p>0;p--)
				messagebuffers[p]=messagebuffers[p-1];
		}
		messagebuffers[0]=mb;
		myCopy(nick, 1, 63, mb->nick);
		messagebuffers[0]->lastmessage=now;
		messagebuffers[0]->link = link;
	}
}

pascal void MBIdle(void)
{
	int x;
	
	for(x=MAXMB-1;x>=0;x--)
		if(messagebuffers[x]->nick[0])
			if(now-messagebuffers[x]->lastmessage>(600)) //10 minutes
			{
				messagebuffers[x]->nick[0]=0; //Clear nick
				messagebuffers[x]->link=0;
				messagebuffers[x]->unused[0]=0;
				messagebuffers[x]->unused[1]=0;
			}
}

#pragma internal on
pascal void MBInit(void)
{
	int x;
	
	for(x=0;x<MAXMB;x++)
	{
		mblist[x].lastmessage=now;
		messagebuffers[x]=&mblist[x];
	}
}