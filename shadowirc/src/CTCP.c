/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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

#include "StringList.h"
#include "LongStrings.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "channels.h"
#include "IRCChannels.h"
#include "utils.h"
#include "CTCP.h"
#include "DCC.h"
#include "connections.h"
#include "plugins.h"
#include "IRCCommands.h"
#include "Inline.h"
#include "TextManip.h"


static void DisplayCTCP(linkPtr link, ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, LongString *ctcp, char ign);
inline void quote(LongString *s);
inline void unquote(StringPtr s);
static char CTCPComm(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, ConstStr255Param ta, ConstStr255Param co, StringPtr re);
static void CTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, Str255 re);
static void DisplayAction(linkPtr link, ConstStr255Param ta, dccPtr dcc, ConstStr255Param fr, ConstStr255Param re);

inline void quote(LongString *s)
{
	int i=1;
	
	while(i<=s->len)
	{
		switch(s->data[i])
		{
			case 0:
				s->data[i]=16;
				LSInsertChar(0, i+1, s);
				break;
			case 10:
				s->data[i]=16;
				LSInsertChar('n', i+1, s);
				break;
			case 13:
				s->data[i]=16;
				LSInsertChar('r', i+1, s);
				break;
			case 16:
				s->data[i]=16;
				i++;
				LSInsertChar(16, i, s);
				break;
		}
		i++;
	}
}

inline void unquote(StringPtr s)
{
	int i;

	do
	{
		i=pos(16, s);
		if(!i)
			break;
		else if(i==s[0])
			s[0]=i-1;
		else
		{
			switch(s[i+i])
			{
				case 48:
					s[i+1]=0;
					break;
				case 110:
					s[i+1]=10;
					break;
				case 114:
					s[i+1]=13;
					break;
				case 16:
					s[i+1]=16;
					break;
			}
			pdelete(s,i,1);
		}
	} while(1);
}

pascal void SendCTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, LongString *st)
{
	LongString ls;
	
	if(!mainPrefs->disableCTCP)
	{
		quote(st);
		LSStrCat4(&ls, "\pNOTICE ", fr, "\p :\1", co);
		LSAppend1(ls, ' ');
		LSConcatLSAndLS(&ls, st, &ls);
		LSAppend1(ls, 1);
		SendCommand(link, &ls);
	}
}

static void DisplayCTCP(linkPtr link, ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, LongString *ctcp, char ign)
{
	LongString ls;
	channelPtr targChan;
	short targ;
	ConstStringPtr sp;
	
	if(!mainPrefs->disableCTCPMessages)
	{
		targChan = ChFind(target, link);
		LSMakeStr(*ctcp);
		if(targChan)
			sp = GetIntStringPtr(spServer, sReceivedChannelCTCP);
		else
			sp = GetIntStringPtr(spServer, sReceivedCTCP);
		
		LSParamString(&ls, sp, ctcp->data, from, target, 0);
		
		if(mainPrefs->showUserHostsWithMsgs)
		{
			LSAppend1(ls, '[');
			LSConcatLSAndStr(&ls, uah, &ls);
			LSAppend1(ls, ']');
		}

		if(ign || mainPrefs->disableCTCP)
			LSConcatLSAndStr(&ls, "\p, ignoring.", &ls);
		
		if(mainPrefs->ctcpToConsole)
			targ = dsConsole;
		else if(!targChan)
			targ = dsFrontWin;
		else
			targ = dsNowhere;
		
		if(!targChan)
			SMPrefixLinkColor(link, &ls, targ, sicCTCP);
		else
		{
			SMPrefixColor(&ls, targ, sicCTCP);
			if(targ==dsNowhere)
				ChMsg(targChan, &ls);
		}
	}
}

static void DisplayAction(linkPtr link, ConstStr255Param ta, dccPtr dcc, ConstStr255Param fr, ConstStr255Param re)
{
	LongString ls;
	channelPtr targChan;
	int type = kNickAction;
	
	if(ta)
	{
		targChan=ChFind(ta, link);
		if(!targChan)
			type |= kNickPrivmsg;
	}
	
	LSStrLS(re, &ls);
	FormatMessage(fr, &ls, 0, 0, type, &ls);
	
	if(ta)
	{
		if(targChan)
			ChMsg(targChan, &ls);
		else //not a channel. which makes it a private message
		{
			MWPtr mw;
			
			//Open a window on private action. (For magn0lia)
			if(mainPrefs->autoQuery)
				mw = DoJoinQuery(fr, link);
			else
			{
				mw=ChannelWindow(link, fr);
				if(mw->link != link)
					SMLink(link, &ls);
			}
			
			MWMessage(mw, &ls);
		}
	}
	else if(dcc)
	{
		MWMessage(((dccCHATDataPtr)dcc->dccData)->dccWindow, &ls);
	}
}

static char CTCPComm(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, ConstStr255Param ta, ConstStr255Param co, StringPtr re)
{
	Str255 st;
	LongString ls;
	channelPtr targChan;
	StringPtr sp;
	
	if(pstrcmp(co, "\pACTION"))
	{
		DisplayAction(link, ta, 0, fr, re);
		return 0;
	}
	else if(pstrcmp(co, "\pCREATOR"))
	{
		LSStrLS("\pJohn Bafford (DShadow), dshadow@shadowirc.com", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pCLIENTINFO"))
	{
		LSStrLS("\pACTION CREATOR CLIENTINFO DCC FINGER PING SOURCE TIME USERINFO VERSION", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pDCC"))
	{
		DCCRequest(link, fr, uah, re);
		return 0;
	}
	else if(pstrcmp(co, "\pFINGER"))
	{
		NumToString(now-idleTime, st);
		if(link->linkPrefs->fingerMessage[0])
			sp = link->linkPrefs->fingerMessage;
		else
			sp = link->linkPrefs->user;
		LSStrCat(6, &ls, re, "\p :", sp, "\p :idle ", st, "\p seconds");
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pPING"))
	{
		LSStrLS(re, &ls);
		SendCTCPReply(link, fr, co, &ls);
		SoundService(sndGotPing, 0);
	}
	else if(pstrcmp(co, "\pSOURCE"))
	{
		LSStrLS("\pShadowIRC can be downloaded from http://www.shadowirc.com/", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pTIME"))
	{
		DateString(now, longDate, ls.data, 0);
		ls.len = ls.data[0];
		TimeString(now, true, st, 0);
		LSAppend1(ls, ' ');
		LSConcatLSAndStr(&ls, st, &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pUSERINFO"))
	{
		if(link->linkPrefs->userinfoMessage[0])
			LSStrLS(link->linkPrefs->userinfoMessage, &ls);
		else
			LSStrLS(shadowIRCDefaultSignoff, &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pVERSION"))
	{
		LSConcatStrAndStrAndStr("\pShadowIRC ", CL_VERSION, "\p � John Bafford 1996-2003 (", &ls);
		LSConcatLSAndStrAndStr(&ls, cdt, "\p)", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else
	{
		targChan=ChFind(ta, link);
		if(targChan)
			LSStrCat(8, &ls, "\pUnknown channel CTCP \"", co, "\p ", re, "\p\" for ", ta, "\p from ", fr);
		else
			LSStrCat(6, &ls, "\pUnknown CTCP \"", co, "\p ", re, "\p\" from ", fr);
		
		if(mainPrefs->showUserHostsWithMsgs)
		{
			LSAppend1(ls, '[');
			LSConcatLSAndStr(&ls, uah, &ls);
			LSAppend1(ls, ']');
		}
		if(mainPrefs->noCTCPUnknownErrMsg)
			LSConcatLSAndStr(&ls, "\p, ignoring.", &ls);
		
		if(!targChan)
			SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
		else
		{
			SMPrefixColor(&ls, dsNowhere, sicCTCP);
			ChMsg(targChan, &ls);
		}

		if(!mainPrefs->noCTCPUnknownErrMsg)
		{
			LSConcatStrAndStr(co, "\p :unknown query", &ls);
			SendCTCPReply(link, fr, "\pERRMSG", &ls);
		}
		return 0;
	}
	return 1;
}

static void CTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, Str255 re)
{
	LongString ls;
	
	if(pstrcmp(co, "\pPING"))
	{
		unsigned long l1, l2, lo;
		Str255 st;
		unsigned long long t1, td;
		
		Microseconds((UnsignedWide*)&td);
		td/=1000;
		ulong64val(re, &t1);
		td-=t1;
		
		lo = td;
		l2=lo%1000;
		l1=(lo-l2)/1000;
		if(l1<0)
			l1=0;
		
		NumToString(l1, st);
		SAppend1(st, '.');
		NumToString(l2, re);
		padBegin(re, 3, re);
		if(re[1]==' ')
			re[1]='0';
		if(re[2]==' ')
			re[2]='0';
		
		LSParamString(&ls, GetIntStringPtr(spServer, sCPINGTime), fr, st, re, 0);
		SoundService(sndCPing, lo);
	}
	else
		LSParamString(&ls, GetIntStringPtr(spServer, sCTCPReply), co, fr, re, 0);

	SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
}

pascal char DCCCTCP(connectionPtr conn, LongString *s)
{
	short j, k;
	Str255 comm, rest;

	if(s->data[1]!=1)
		return 0;
	
	LSCopyString(s, 2, 255, comm);
	j=pos(1, comm);
	if(!j)
		j=254;
	k=pos(' ', comm);
	if(!k)
		k=j;
	myCopy(comm, k+1, j-1, rest);
	comm[0]=k-1;
	unquote(rest);

	ucase(comm);
	if(pstrcmp(comm, "\pACTION"))
	{
		DisplayAction(0, 0, conn->dcc, conn->dcc->dccUserName, rest);
		return 1;
	}
	else if(pstrcmp(comm, "\pDCC"))
	{
		DCCRequest(conn->link, conn->dcc->dccUserName, conn->name, rest);
		return 1;
	}
	
/*	
	p.link=conn->link;
	p.dcc=conn;
	p.username=from;
	p.userhost=uah;
	p.target=target;
	p.cmd=comm;
	p.rest=rest;
	p.isReply=isReply;
	p.ignored=&ign;
	p.completelyProcessed=0;

	ucase(comm);

	runPlugins(pCTCPMessage, (Ptr)&p);
	if(!p.completelyProcessed && !ign)
		if(isReply)
			CTCPReply(link, from, comm, rest);
		else
			ok=CTCPComm(link, from, uah, target, comm, rest);

	if(!ign && !isReply && ok)
	{
		s->len=j;
		DisplayCTCP(link, from, uah, target, s);
	}

*/
	return 1;
}

pascal char doCTCP(ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, LongString *s, char isReply, char ign, linkPtr link)
{
	short j, k;
	Str255 comm, rest;
	pCTCPDataRec p;
	char ok;

	//Only accept ctrl-a if first character!
	if(s->data[1]!=1)
		return 0;
	
	if(s->len == 1)
	{
		LSStrLS("\p<<EMPTY>>", s);
		DisplayCTCP(link, from, uah, target, s, true);
		return 1;
	}
	
	LSCopyString(s, 2, 255, comm);
	j=pos(1, comm);
	if(!j)
		j=comm[0];
	k=pos(' ', comm);
	if(!k)
		k=j;
	myCopy(comm, k+1, j-1, rest);
	comm[0]=k-1;
	unquote(rest);
	
	p.link=link;
	p.dcc=0;
	p.username=from;
	p.userhost=uah;
	p.target=target;
	p.cmd=comm;
	p.rest=rest;
	p.isReply=isReply;
	p.ignored=&ign;
	p.completelyProcessed=0;

	ucase(comm);

	runPlugins(pCTCPMessage, &p);
	if(!p.completelyProcessed && !ign)
	{
		if(isReply)
			CTCPReply(link, from, comm, rest);
		else
			ok=CTCPComm(link, from, uah, target, comm, rest);
	}
	
	if(!ign && !isReply && ok)
	{
		s->len=j;
		LSDelete(s, 1, 1); //delete initial ctrl-a
		DisplayCTCP(link, from, uah, target, s, false);
	}

	return 1;	
}