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

#define CHANNELS

#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "connections.h"
#include "channels.h"
#include "utils.h"
#include "Inline.h"
#include "plugins.h"

char IsChannel(ConstStringPtr s)
{
	if((s[0]!=0) && ((s[1]=='#') || (s[1]=='&') || (s[1]=='+')))
		return true;
	else
		return false;
}

static void ULKillAllUsers(channelPtr channel);

long userNum=0;

pascal void ULDeleteUser(UserListPtr user)
{
	if(user)
	{
		channelPtr ch = user->channel;
		UserListPtr ul=ch->userlist;
	
		user->userNum=0;
		ch->numUsers--;

		if(user==ul)
			ch->userlist=user->next;
		else
			if(ul)
			{
				while(ul->next!=user)
					ul=ul->next;
				
				ul->next=user->next;
			}

		DisposePtr((Ptr)user);
	}
}

static void ULKillAllUsers(channelPtr channel)
{
	UserListPtr ul, ul2;
	
	runIndService(userlistServiceType, pServiceULTrashChannel, &channel);

	ul = channel->userlist;
	if(ul)
	{
		channel->userlist=0;
		channel->numUsers=0;
		while(ul)
		{
			ul2=ul->next;
			DisposePtr((Ptr)ul);
			ul=ul2;
		}
	}
}

pascal UserListPtr ULAddUser(channelPtr channel, ConstStr255Param nick, ConstStr255Param userhost)
{
	UserListPtr ul;
	
	ul=ULFindUserName(channel,nick);
	if(!ul)
	{
		ul=(UserListPtr)NewPtr(sizeof(UserListRec));
		if(ul)
		{
			ul->next=channel->userlist;
			ul->lastUHUpdate=0;
			ul->nick[0]=0;
			ul->userhost[0]=0;
			ul->isOper=
			ul->isAway=
			ul->isOp=
			ul->hasVoice=
			ul->isHalfOp=0;
			ul->userlistServiceRefCon=0;
			ul->userNum=++userNum;
			ul->userlistIsSelected=0;
			ul->facesServiceRefCon=0;
			ul->isIgnored = ul->pad = 0;
			channel->userlist=ul;
			
			pstrcpy(nick, ul->nick);
			pstrcpy(userhost, ul->userhost);
			channel->numUsers++;
			ul->channel=channel;
		}
	}
	return ul;
}

pascal UserListPtr ULFindUserName(channelPtr channel, ConstStr255Param nick)
{
	UserListPtr ul;
	Str255 s1;
	
	if(channel)
	{
		pstrcpyucase(nick,s1);
		linkfor(ul, channel->userlist)
			if(pstrcasecmp2(s1,ul->nick))
				return ul;
	}
	
	return 0;
}

pascal UserListPtr ChFindUser(ConstStr255Param user)
{
	channelPtr ch;
	UserListPtr u;
	Str255 s;
	linkPtr link;
	
	pstrcpyucase(user, s);
	linkfor(link, firstLink)
		linkfor(ch, link->channelList)
			linkfor(u, ch->userlist)
				if(pstrcasecmp2(s, u->nick))
					return u;
	
	return 0;
}

pascal channelPtr ChFind(ConstStr255Param name, linkPtr link)
{
	channelPtr ch;

	if(link && IsChannel(name))
		linkfor(ch, link->channelList)
			if(pstrcasecmp(name, ch->chName))
				return ch;

	return 0;
}

pascal bansP ChFindBan(channelPtr ch, ConstStr255Param n, char e)
{
	bansP p;
	
	if(e)
		p = ch->exceptions;
	else
		p = ch->bans;
	
	linkfor(p, p)
		if(pstrcasecmp(n, p->ban))
			return p;
	
	return 0;
}

pascal bansP ChMatchBan(channelPtr ch, ConstStr255Param n, char e)
{
	bansP b;
	
	if(e)
		b = ch->exceptions;
	else
		b = ch->bans;
	
	linkfor(b, b)
		if(maskMatch(n, b->ban))
			return b;
	
	return 0;
}

pascal char ChGetBan(channelPtr ch, ConstStr255Param from, bansP *f, char e)
{
	bansP bp=ChFindBan(ch, from, e);
	
	if(!bp)
	{
		bp=(bansP)NewPtr(sizeof(bansRec));
		if(bp)
		{
			pstrcpy(from,bp->ban);
			bp->setBy[0]=0;
			bp->time=now;

			if(e)
			{
				bp->next = ch->exceptions;
				ch->exceptions = bp;
			}
			else
			{
				bp->next=ch->bans;
				ch->bans=bp;
			}
			if(f)
				*f=bp;
		}
		return 0;
	}
	else
	{
		if(f)
			*f=bp;
		return 1;
	}
}

pascal void ChKillBan(channelPtr ch, bansP *x, char e)
{
	bansP p;
	bansP xx;
	
	if(x && *x)
	{
		xx = *x;
		if(xx==ch->bans)
			ch->bans=xx->next;
		else if(xx == ch->exceptions)
			ch->exceptions = xx->next;
		else
		{
			if(e)
				p = ch->exceptions;
			else
				p=ch->bans;
			
			if(p)
			{
				while(p->next!=xx)
					p=p->next;
				
				p->next=xx->next;
			}
		}
		DisposePtr((Ptr)xx);
		*x = 0;
	}
}

pascal void ChKillAllBans(channelPtr ch, char e)
{
	bansP p, p2;
	
	if(e)
		p = ch->exceptions;
	else
		p=ch->bans;
	
	while(p)
	{
		p2=p->next;
		DisposePtr((Ptr)p);
		p=p2;
	}
	
	if(e)
		ch->exceptions = 0;
	else
		ch->bans=0;
}

pascal void ChClear(channelPtr ch)
{
	if(ch)
	{
		if(ch->userlist)
			ULKillAllUsers(ch);
		if(ch->bans)
			ChKillAllBans(ch, false);
		if(ch->exceptions)
			ChKillAllBans(ch, true);
		ch->active=0;
	
		ch->hasOps = ch->hasVoice = ch->topic[0] = 0;
		//...
		ch->gotNames = 0;
	}
}

pascal void ChDestroy(channelPtr ch)
{
	channelPtr x;
	
	if(ch)
	{
		if(ch->window)
			MWSetChannel(ch->window, 0);

		ChClear(ch);
		x=ch->link->channelList;
		if(ch==x)
			ch->link->channelList=x->next;
		else
		{
			while(x->next!=ch)
				x=x->next;
			
			//assume it finds a match
			x->next=ch->next;
		}
		
		ch->link = 0;
		DisposePtr((Ptr)ch);
	}
}

pascal channelPtr ChCreate(ConstStr255Param name, linkPtr link)
{
	channelPtr x;

	x=ChFind(name, link);
	if(x)
	{
		ChClear(x);
		x->active=1;
		return x;
	}
	
	x=(channelPtr)NewPtrClear(sizeof(Channel));
	x->next=link->channelList;
	pstrcpy(name, x->chName);
	x->active=1;
	x->link=link;
	link->channelList=x;
	
	return x;
}

pascal char IsChannelValid(channelPtr c)
{
	channelPtr x;
	
	if(c && IsLinkValid(c->link))
		linkfor(x, c->link->channelList)
			if(x==c)
				return 1;
	
	return 0;
}

pascal char IsUserValid(UserListPtr u)
{
	UserListPtr x;
	
	if(u && IsChannelValid(u->channel) && u->userNum > 0 && u->userNum <= userNum)
		linkfor(x, u->channel->userlist)
			if(x==u)
				return 1;
		
	return 0;
}