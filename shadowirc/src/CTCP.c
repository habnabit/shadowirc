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
#include "Timer.h"
#include "TextManip.h"

#if !__POWERPC__
#pragma dont_inline on
#endif


#pragma internal on
static pascal void DisplayCTCP(linkPtr link, ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, LongString *ctcp, char ign);
inline void quote(LongString *s);
inline void unquote(StringPtr s);
static pascal char CTCPComm(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, ConstStr255Param ta, ConstStr255Param co, StringPtr re);
static pascal void CTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, Str255 re);
static pascal void DisplayAction(linkPtr link, ConstStr255Param ta, dccPtr dcc, ConstStr255Param fr, ConstStr255Param re);
#pragma internal reset

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

static pascal void DisplayCTCP(linkPtr link, ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, LongString *ctcp, char ign)
{
	LongString ls;
	channelPtr targChan;
	short targ;
	
	if(!mainPrefs->disableCTCPMessages)
	{
		targChan = ChFind(target, link);
		LSMakeStr(*ctcp);
		if(targChan)
			LSStrCat(6, &ls, "\pReceived channel CTCP \"", ctcp->data, "\p\" for ", target, "\p from ", from);
		else
			LSStrCat4(&ls, "\pReceived CTCP \"", ctcp->data, "\p\" from ", from);
		
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

static pascal void DisplayAction(linkPtr link, ConstStr255Param ta, dccPtr dcc, ConstStr255Param fr, ConstStr255Param re)
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
			
			//All this extra crap is for magn0lia, so the window pops up on an action.
			if(mainPrefs->autoQuery && FreeMem() > 16000) //don't open if < 16k free
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

static pascal char CTCPComm(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, ConstStr255Param ta, ConstStr255Param co, StringPtr re)
{
	Str255 st;
	LongString ls;
	channelPtr targChan;
	StringPtr sp;
	
	if(pstrcmp6(co, 'ACT', 'IO', 'N'))
	{
		DisplayAction(link, ta, 0, fr, re);
		return 0;
	}
	else if(pstrcmp7(co, 'CRE', 'ATOR'))
	{
		LSStrLS("\pJohn Bafford (DShadow), dshadow@shadowirc.com", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pCLIENTINFO"))
	{
		LSStrLS("\pACTION CREATOR CLIENTINFO DCC FINGER PING SOURCE TIME USERINFO VERSION", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp3(co, 'DCC'))
	{
		DCCRequest(link, fr, uah, re);
		return 0;
	}
	else if(pstrcmp6(co, 'FIN', 'GE', 'R'))
	{
		NumToString(now-idleTime, st);
		if(link->linkPrefs->fingerMessage[0])
			sp = link->linkPrefs->fingerMessage;
		else
			sp = link->linkPrefs->user;
		LSStrCat(6, &ls, re, "\p :", sp, "\p :idle ", st, "\p seconds");
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp4(co, 'PIN', 'G'))
	{
		LSStrLS(re, &ls);
		SendCTCPReply(link, fr, co, &ls);
		SoundService(sndGotPing, 0);
	}
	else if(pstrcmp6(co, 'SOU', 'RC', 'E'))
	{
		LSStrLS("\pShadowIRC can be downloaded from http://www.shadowirc.com/", &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp4(co, 'TIM', 'E'))
	{
		DateString(now, longDate, ls.data, 0);
		ls.len = ls.data[0];
		TimeString(now, true, st, 0);
		LSAppend1(ls, ' ');
		LSConcatLSAndStr(&ls, st, &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp8(co, 'USE', 'RINF', 'O'))
	{
		if(link->linkPrefs->userinfoMessage[0])
			LSStrLS(link->linkPrefs->userinfoMessage, &ls);
		else
			LSStrLS(shadowIRCDefaultSignoff, &ls);
		SendCTCPReply(link, fr, co, &ls);
	}
	else if(pstrcmp7(co, 'VER', 'SION'))
	{
		LSConcatStrAndStrAndStr("\pShadowIRC ", CL_VERSION, "\p © John Bafford 1996-2000 (", &ls);
		LSConcatLSAndStrAndStr(&ls, cdt, "\p), ", &ls);
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

static pascal void CTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, Str255 re)
{
	LongString ls;
	
	if(pstrcmp4(co, 'PIN', 'G'))
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
		LSStrCat(6, &ls, "\pCPING time for ", fr, "\p: ", st, re, "\p seconds");
		if(l1 == 1 && l2 == 0)
			ls.len--;
		SoundService(sndCPing, lo);
	}
	else
	{
		LSStrCat(6, &ls, "\pCTCP ", co, "\p reply from ", fr, "\p: ", re);
	}

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
	if(pstrcmp6(comm, 'ACT', 'IO', 'N'))
	{
		DisplayAction(0, 0, conn->dcc, conn->dcc->dccUserName, rest);
		return 1;
	}
	else if(pstrcmp3(comm, 'DCC'))
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
		if(isReply)
			CTCPReply(link, from, comm, rest);
		else
			ok=CTCPComm(link, from, uah, target, comm, rest);

	if(!ign && !isReply && ok)
	{
		s->len=j;
		LSDelete(s, 1, 1); //delete initial ctrl-a
		DisplayCTCP(link, from, uah, target, s, false);
	}

	return 1;	
}