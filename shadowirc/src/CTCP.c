/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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


static void DisplayCTCP(linkPtr link, ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, ConstStr255Param comm, const LongString *ctcp, char ign);
static void CTCPQuoteLS(const LongString *in, LongString *out);
static void CTCPUnquoteLS(LongString *ls);
static char CTCPComm(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, ConstStr255Param ta, ConstStr255Param co, const LongString *text);
static void CTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, const LongString *text);
static void DisplayAction(linkPtr link, ConstStr255Param ta, dccPtr dcc, ConstStr255Param fr, const LongString *text);
static char SplitCTCPCommand(const LongString *ls, Str255 cmd, LongString *text);

static void CTCPQuoteLS(const LongString *in, LongString *out)
{
	int i, o, m = in->len;
	
	out->len = in->len;
	
	for(i = 1, o = 1; i <= m; i++, o++)
	{
		switch(in->data[i])
		{
			case 0:
				out->data[o] = 16;
				out->data[++o] = '0';
				break;
			case 10:
				out->data[o] = 16;
				out->data[++o] = 'n';
				break;
			case 13:
				out->data[o] = 16;
				out->data[++o] = 'r';
				break;
			case 16:
				out->data[o] = 16;
				out->data[++o] = 16;
				break;
			default:
				out->data[o] = in->data[i];
		}
	}
}

static void CTCPUnquoteLS(LongString *ls)
{
	const unsigned char quoteCharSearchStr[]= {1, 16};
	int i = 0;

	do
	{
		i = LSPosCustom(quoteCharSearchStr, ls, i + 1);
		if(!i)
			break;
		else if(i == ls->len)
			ls->len--;
		else
		{
			switch(ls->data[i+1])
			{
				case '0':
					ls->data[i] = 0;
					break;
				case 'n':
					ls->data[i] = 10;
					break;
				case 'r':
					ls->data[i] = 13;
					break;
				case 16:
					ls->data[i] = 16;
					break;
			}
			LSDelete(ls, i+1, i+1);
		}
	} while(1);
}

void CTCPSendReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, const LongString *st)
{
	LongString ls;
	LongString quotedST;
	
	if(!mainPrefs->disableCTCP)
	{
		CTCPQuoteLS(st, &quotedST);
		LSStrCat4(&ls, "\pNOTICE ", fr, "\p :\1", co);
		LSAppend1(ls, ' ');
		LSConcatLSAndLS(&ls, &quotedST, &ls);
		LSAppend1(ls, 1);
		SendCommand(link, &ls);
	}
}

static void DisplayCTCP(linkPtr link, ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, ConstStr255Param comm, const LongString *text, char ign)
{
	LongString ls;
	channelPtr targChan;
	short targ;
	ConstStringPtr sp;
	
	if(!mainPrefs->disableCTCPMessages)
	{
		LongString ctcp;
		
		targChan = ChFind(target, link);
		if(targChan)
			sp = GetIntStringPtr(spServer, sReceivedChannelCTCP);
		else
			sp = GetIntStringPtr(spServer, sReceivedCTCP);
		
		//NOTE: This trims the output to 255 chars for command + text, but it was like this before...
		if(text->len)
			LSConcatStrAndStrAndLS(comm, "\p ", text, &ctcp);
		else
			LSStrLS(comm, &ctcp);
		
		LSMakeStr(ctcp);
		LSParamString(&ls, sp, ctcp.data, from, target, 0);
		
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

static void DisplayAction(linkPtr link, ConstStr255Param ta, dccPtr dcc, ConstStr255Param fr, const LongString *text)
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
	
	FormatMessage(fr, text, 0, 0, type, &ls);
	
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

static char CTCPComm(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, ConstStr255Param ta, ConstStr255Param co, const LongString *text)
{
	Str255 st;
	LongString ls;
	channelPtr targChan;
	StringPtr sp;
	
	if(pstrcmp(co, "\pACTION"))
	{
		DisplayAction(link, ta, 0, fr, text);
		return 0;
	}
	else if(pstrcmp(co, "\pCREATOR"))
	{
		LSStrLS("\pJohn Bafford (DShadow), dshadow@shadowirc.com", &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pCLIENTINFO"))
	{
		LSStrLS("\pACTION CREATOR CLIENTINFO DCC FINGER PING SOURCE TIME USERINFO VERSION", &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pDCC"))
	{
		LSCopyString(text, 1, 255, st);
		DCCRequest(link, fr, uah, st);
		return 0;
	}
	else if(pstrcmp(co, "\pFINGER"))
	{
		NumToString(now-idleTime, st);
		if(link->linkPrefs->fingerMessage[0])
			sp = link->linkPrefs->fingerMessage;
		else
			sp = link->linkPrefs->user;
		LSConcatLSAndStrAndStr(text, "\p :", sp, &ls);
		LSConcatLSAndStrAndStr(&ls, "\p :idle ", st, &ls);
		LSConcatLSAndStr(&ls, "\p seconds", &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pPING"))
	{
		CTCPSendReply(link, fr, co, text);
		SoundService(sndGotPing, 0);
	}
	else if(pstrcmp(co, "\pSOURCE"))
	{
		LSStrLS("\pShadowIRC can be downloaded from http://www.shadowirc.com/", &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pTIME"))
	{
		DateString(now, longDate, ls.data, 0);
		ls.len = ls.data[0];
		TimeString(now, true, st, 0);
		LSAppend1(ls, ' ');
		LSConcatLSAndStr(&ls, st, &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pUSERINFO"))
	{
		if(link->linkPrefs->userinfoMessage[0])
			LSStrLS(link->linkPrefs->userinfoMessage, &ls);
		else
			LSStrLS(shadowIRCDefaultSignoff, &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else if(pstrcmp(co, "\pVERSION"))
	{
		LSConcatStrAndStrAndStr("\pShadowIRC ", CL_VERSION, "\p © John Bafford 1996-2004 (", &ls);
		LSConcatLSAndStrAndStr(&ls, cdt, "\p)", &ls);
		CTCPSendReply(link, fr, co, &ls);
	}
	else
	{
		targChan=ChFind(ta, link);
		if(targChan)
			sp = "\pUnknown channel CTCP \"";
		else
			sp = "\pUnknown CTCP \"";
		
		LSConcatStrAndStrAndStr(sp, co, "\p ", &ls);
		LSConcatLSAndLS(&ls, text, &ls);
		
		if(targChan)
			LSConcatLSAndStrAndStr(&ls, "\p\" for ", ta, &ls);
		
		LSConcatLSAndStrAndStr(&ls, "\p\" from ", fr, &ls);
		
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
			CTCPSendReply(link, fr, "\pERRMSG", &ls);
		}
		return 0;
	}
	return 1;
}

static void CTCPReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, const LongString *text)
{
	LongString ls;
	Str255 re;
	
	LSCopyString(text, 1, 255, re);
	
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

static char SplitCTCPCommand(const LongString *ls, Str255 cmd, LongString *text)
{
	const unsigned char ctrlASearchString[] = {1, 1};
	int endOfCTCP, endOfCmd;
	
	if(ls->data[1] != 1)
		return 0;
	
	//endOfCTCP will now contain the index of the closing ^A (or one character past the end of the string if there's no closing ^A)
	endOfCTCP = LSPosCustom(ctrlASearchString, ls, 2);
	if(!endOfCTCP)
		endOfCTCP = ls->len + 1;
	
	//endOfCmd will now contain the index of the space separating the command from the data (or the index of the trailing ^A)
	endOfCmd = LSPosChar(' ', ls);
	if(!endOfCmd || endOfCmd > endOfCTCP)
		endOfCmd = endOfCTCP;
	
	LSCopyString(ls, 2, endOfCmd - 2, cmd);
	ucase(cmd);
	
	LSCopy(ls, endOfCmd + 1, endOfCTCP - (endOfCmd + 1), text);
	CTCPUnquoteLS(text);
	
	return 1;
}

char DCCCTCP(connectionPtr conn, const LongString *s)
{
	Str255 comm;
	LongString text;
	
	if(!SplitCTCPCommand(s, comm, &text))
		return 0;
	
	if(pstrcmp(comm, "\pACTION"))
	{
		DisplayAction(0, 0, conn->dcc, conn->dcc->dccUserName, &text);
		return 1;
	}
	else if(pstrcmp(comm, "\pDCC"))
	{
		//NOTE: Should probably change DCCRequest to accept a longstring for DCC parameters
		LSCopyString(s, 1, 255, comm);
		DCCRequest(conn->link, conn->dcc->dccUserName, conn->name, comm);
		return 1;
	}
	
	return 1;
}

char doCTCP(ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, const LongString *s, char isReply, char ign, linkPtr link)
{
	Str255 comm;
	LongString text;
	pCTCPDataRec p;
	char ok;
	
	if(!SplitCTCPCommand(s, comm, &text))
		return 0;
	
	p.link=link;
	p.dcc=0;
	p.username=from;
	p.userhost=uah;
	p.target=target;
	p.cmd=comm;
	p.rest = &text;
	p.isReply=isReply;
	p.ignored=&ign;
	p.completelyProcessed=0;
	
	ok = 0;
	
	runPlugins(pCTCPMessage, &p);
	if(!p.completelyProcessed && !ign)
	{
		if(isReply)
			CTCPReply(link, from, comm,  &text);
		else
			ok = CTCPComm(link, from, uah, target, comm, &text);
	}
	
	if(!ign && !isReply && ok)
		DisplayCTCP(link, from, uah, target, comm, &text, false);

	return 1;	
}