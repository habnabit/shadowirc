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

#define __IRCNOTIFY__

#include "StringList.h"
#include "IRCGlobals.h"
#include "LongStrings.h"
#include "MsgWindows.h"
#include "IRCNotify.h"
#include "connections.h"
#include "utils.h"
#include "IRCChannels.h"
#include "Inline.h"
#include "IRCCommands.h"
#include "plugins.h"
#include "TextManip.h"

static pascal void trashNotify(linkPtr link, notifyPtr n);

pascal void ListNotify(linkPtr link)
{
	short i;
	LongString ls;
	notifyPtr n=link->notifyList;
	Str255 s;
	
	if(n)
	{
		i=0;
		LSGetIntString(&ls, spInfo, sNotifyList);
		while(n)
		{
			if(!n->ISON || debugOn)
			{
				if(ls.len+n->nick[0]> (maxLSlen-10))
				{
					SMPrefixColor(&ls, dsFrontWin, sicNotifyColor);
					ls.len=0;
				}
				LSConcatLSAndStr(&ls, n->nick, &ls);
				LSAppend1(ls, ' ');
				i++;
			}
			n=n->next;
		}
		if(ls.len)
			SMPrefixColor(&ls, dsFrontWin, sicNotifyColor);
		NumToString(i, s);
		LSParamString(&ls, GetIntStringPtr(spInfo, sUsersOnNotify), s, 0, 0, 0);
		SMPrefixColor(&ls, dsFrontWin, sicNotifyColor);
	}
	else
	{
		LSGetIntString(&ls, spInfo, spNooneOnNotify);
		SMPrefixColor(&ls, dsFrontWin, sicNotifyColor);
	}
}

pascal notifyPtr findNotify(linkPtr link, ConstStr255Param nick)
{
	notifyPtr n;
	Str255 s;
	
	pstrcpyucase(nick, s);
	linkfor(n, link->notifyList)
		if(pstrcasecmp2(s, n->nick))
			return n;
	
	return 0;
}

static pascal void trashNotify(linkPtr link, notifyPtr n)
{
	notifyPtr temp;
	
	if(n)
	{
		if(n->back)
			n->back->next=n->next;
		if(n->next)
			n->next->back=n->back;
		temp=n->next;
		if(link->notifyList == n)
			link->notifyList=temp;
		n->next = n->back = 0;
		DisposePtr((Ptr)n);
	}
}

pascal void killISONs(linkPtr link)
{
	notifyPtr n = link->notifyList;
	notifyPtr n2;
	
	while(n && n->ISON)
	{
		n2=n->next;
		trashNotify(link, n);
		n=n2;
	}
}

pascal void deleteNotify(linkPtr link, ConstStr255Param nick)
{
	notifyPtr n=findNotify(link, nick);
	notifyPtr n2;
	char l;
	
	if(n->ISON)
	{
		n2=n;
		l=n2->nick[0];
		n2->nick[0]=0;
		n=findNotify(link, nick);
		n2->nick[0]=l;
	}
	trashNotify(link, n);
}

pascal notifyPtr addNotify(linkPtr link, ConstStr63Param nick, char ison)
{
	notifyPtr n;
	notifyPtr n2, n3;
	
	n=(notifyPtr)NewPtr(sizeof(notifyRec));
	if(!ison)
	{
		n->next=link->notifyList;
		n->back=0;
		if(n->next)
			n->next->back=n;
		link->notifyList=n;
	}
	else
	{
		n2=link->notifyList;
		n3=0;
		while(n2 && n2->ISON)
		{
			n3=n2;
			n2=n2->next;
		}
		
		if(!n2)
			n2=n3;
		if(!n2)
		{
			n->next=link->notifyList;
			n->back=0;
			if(n->next)
				n->next->back=n;
			link->notifyList=n;
		}
		else
		{
			n->back = n2;
			n->next = n2->next;
			n2->next = n;
		}
	}
	
	pstrcpymax(nick, n->nick, 63);
	n->waitingForResponse=0;
	n->signedon=0;
	n->changed=0;
	n->ISON=ison;
	
	return n;
}

pascal void DoNotify(linkPtr link, LongString *in)
{
	Str255 str;
	char remove;
	LongString out;
	int x;
	
	if(in->len)
	{
		for(x=1;x<=in->len;x++)
			if(in->data[x]==',')
				in->data[x]=' ';
		while(in->len)
		{
			LSNextArg(in, str);
			remove=str[1]=='-';
			if(remove || (str[1]=='+'))
				pdelete(str, 1, 1);

			if(remove)
				deleteNotify(link, str);
			else
			{
				if(findNotify(link, str))
				{
					LSParamString(&out, GetIntStringPtr(spInfo, sAlreadyOnNotify), str, 0, 0, 0);
					SMPrefixColor(&out, dsFrontWin, sicNotifyColor);
				}
				else
					addNotify(link, str, false);
			}
		}
	}
	ListNotify(link);
}

pascal void RunNotify(void)
{
	linkPtr link;
	LongString out;
	notifyPtr n;
	
	linkfor(link, firstLink)
	{
		if(link->serverStatus == S_CONN && link->finishedMOTD && !link->numOutstandingISON)
		{
			n=link->notifyList;
			
			out.len=0;
			while(n)
			{
				if(out.len>500)
				{
					LSConcatStrAndLS("\pISON ", &out, &out);
					SendCommand(link, &out);
					link->numOutstandingISON++;
				}
				LSConcatLSAndStr(&out, n->nick, &out);
				LSAppend1(out, ' ');
				n->waitingForResponse=1;
				n=n->next;
			}
			
			if(out.len)
			{
				LSConcatStrAndLS("\pISON ", &out, &out);
				SendCommand(link, &out);
				link->numOutstandingISON++;
			}
		}
	}
}

pascal void HandleISON(linkPtr link, LongString *rest)
{
	LongString list;
	Str255 s;
	notifyPtr n;
	
	if(link && link->serverStatus==S_CONN && rest->len) //do nothing if not connected so we dont' screw with stuff
	{
		LSStrLS("\pISON ", &list);

		while(rest->len)
		{
			LSNextArgIRC(rest, s);
			n=addNotify(link, s, true);
			
			LSConcatLSAndStr(&list, s, &list);
			LSAppend1(list, ' ');
			
		}

		list.len--;
		SendCommand(link, &list);
		link->numOutstandingISON++;
	}
}

pascal void ProcessISON(linkPtr link, LongString *rest)
{
	LongString ls;
	notifyPtr n;
	ConstStringPtr s;
	Str255 st;
	short on, off;
	long tl;
	pNOTIFYData p;
	
	ls.len=0;
	link->numOutstandingISON--;
	if(rest->len)
	{
		while(rest->len)
		{
			LSNextArgIRC(rest, st);
			n=findNotify(link, st);
			if(n)
			{
				if(!n->signedon)
				{
					n->signedon=1;
					n->changed=1;
				}
				n->waitingForResponse=0;
			}
		}
	}
	
	if(!link->numOutstandingISON)
	{
		char doingISON;
		
		on=0;
		off=0;
		//Make the list of people who have signed on
		n=link->notifyList;
		
		ls.len=0;
		s = GetIntStringPtr(spInfo, sSignedOn);
		doingISON = n->ISON;
		while(n && (!doingISON || n->ISON))
		{
			if(n->signedon && (n->changed || doingISON))
			{
				LSConcatLSAndStr(&ls, n->nick, &ls);
				LSAppend1(ls, ' ');
				if(ls.len+s[0]>550)
				{
					LSConcatStrAndLS(s,&ls, &ls);
					SMPrefixLinkColor(link, &ls, dsFrontWin, sicNotifyColor);
					ls.len=0;
				}
				on++;
			}
			//n->waitingForResponse=0;
			n=n->next;
		}

		if(ls.len)
		{
			LSConcatStrAndLS(s,&ls, &ls);
			SMPrefixLinkColor(link, &ls, dsFrontWin, sicNotifyColor);
		}
		ls.len=0;
		
		s = GetIntStringPtr(spInfo, sSignedOff);
		//Make the list of people who have signed off
		n=(notifyPtr)link->notifyList;
		
		while(n && (!doingISON || n->ISON))
		{
		if((n->ISON && !n->signedon) || (!n->ISON && n->waitingForResponse && n->signedon))
			{
				off++;
				n->signedon=0;
				LSConcatLSAndStr(&ls, n->nick, &ls);
				LSAppend1(ls, ' ');
				if(ls.len+s[0]>550)
				{
					LSConcatStrAndLS(s,&ls, &ls);
					SMPrefixLinkColor(link, &ls, dsFrontWin, sicNotifyColor);
					ls.len=0;
				}
			}

			n=n->next;
		}
		
		if(ls.len)
		{
			LSConcatStrAndLS(s,&ls, &ls);
			SMPrefixLinkColor(link, &ls, dsFrontWin, sicNotifyColor);
		}

		p.link = link;
		p.notifyList = link->notifyList;
		p.signedOn = on;
		p.signedOff = off;
		runPlugins(pNOTIFYMessage, &p);
		if(on>0 && off>0)
		{
			tl = (on << 16) | off; 
			SoundService(sndNotify, tl);
		}

		if(doingISON)
			killISONs(link);
		else
		{
			n=link->notifyList;
			while(n)
			{
				n->changed=0;
				n->waitingForResponse=0;
				n=n->next;
			}
		}
	}
}