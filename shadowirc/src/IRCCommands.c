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

//#include <Timer.h>
#include "WASTE.h"

#include "LongStrings.h"
#include "ApplBase.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "DCC.h"
#include "IRCNotify.h"
#include "connections.h"
#include "MsgBuffer.h"
#include "channels.h"
#include "IRCChannels.h"
#include "IRCIgnore.h"
#include "inputline.h"
#include "plugins.h"
#include "IRCAux.h"
#include "IRCCommands.h"
#include "TCP.h"
#include "Inline.h"
#include "utils.h"
#include "IRCNotify.h"
#include "Floaters.h"
#include "IRCInput.h"
#include "InetConfig.h"
#include "TextManip.h"
#include "filesMan.h"
#include "MenuCommands.h"

static void doBanList(LongString *args, char e);
pascal void rejoinDeactiveChannels(linkPtr link);
static void TranslateCommand(linkPtr link, LongString *s);

pascal void GetSignoffMessage(linkPtr link, LongString *s)
{
	Handle lh;
	long i;
	
	if(link->linkPrefs->signoffMessage[0])
		LSStrLS(link->linkPrefs->signoffMessage, s);
	else
		LSStrLS(shadowIRCDefaultSignoff, s);
	
	lh = NewHandle(s->len);
	if(lh)
	{
		BlockMoveData(&s->data[1], *lh, s->len);

		do{
			i=Munger(lh, 0, "$version", 8, &shadowIRCDefaultSignoff[1], shadowIRCDefaultSignoff[0]);
		}while(i>=0);
		
		i=GetHandleSize(lh);
		if(i>500)
			i=500;
		s->len=i;
		BlockMoveData(*lh, &s->data[1], i);
		DisposeHandle(lh);
	}
}

pascal void StackModes(LongString *theMode, channelPtr channel, ConstStr255Param mode, ConstStr255Param arg)
{
	short p;
	Str255 s,s2;
	LongString ls;
	int numModes = HTFindNumericDefault(channel->link->serverOptions, "\pMODES", 4);
		
	if(mode)
	{
		p=LSPosChar(' ', theMode);
		if(!p)
		{
			if(!arg)
			{
				LSStrLS(mode, theMode);
				LSAppend1(*theMode, ' ');
			}
			else
				LSConcatStrAndStrAndStr(mode, "\p ", arg, theMode);
		}
		else
		{
			LSCopyString(theMode, 1, p-1, s);
			LSCopyString(theMode, p, 255, s2);
			SAppend1(s2, ' ');
			if(arg)
				LSStrCat4(theMode, s, mode, s2, arg);
			else
				LSConcatStrAndStrAndStr(s, mode, s2, theMode);
			p=LSPosChar(' ', theMode);
		}
	}
	
	if(p>= (numModes * 2) || (!mode && theMode->len))
	{
		LSConcatStrAndStr("\pMODE ", channel->chName, &ls);
		LSAppend1(ls, ' ');
		LSConcatLSAndLS(&ls, theMode, &ls);
		SendCommand(channel->link, &ls);
		theMode->len=0;
	}
}

pascal void DoServer(linkPtr link, LongString *rest, const LongString* reason)
{
	long i;
	Str255 s1;
	LongString ls;
	
	if(link)
	{
		link->neverConnected=0;
		LSNextArg(rest, s1); //serv

		if(s1[0])
		{
			switch(link->serverStatus)
			{
				case S_LOOKUP:
				case S_OPENING:
				case S_CLOSING:
					deleteConnection(&link->conn);
					break;
				case S_CONN:
				{
					const LongString* lsp;
					if(reason)
						lsp = reason;
					else
					{
						lsp = &ls;
						LSGetIntString(&ls, spError, sCChangingServers);
					}
					DoSignoff(link, lsp);
					break;
				}
				default:
					link->serverStatus=S_OFFLINE;
			}

			pstrcpy(s1, link->serverTryingName);

			LSNextArg(rest, s1); //port
			if(s1[0])
			{
				StringToNum(s1, &i);
				link->serverTryingPort=i;
			}
			else
				link->serverTryingPort=6667;

			//LSNextArg(rest, s2); //pass

			LSConcatStrAndStr("\pTrying ", link->serverTryingName, &ls);
			LSAppend1(ls, ':');
			LSConcatLSAndStr(&ls, s1, &ls);
			SMPrefixLink(link, &ls, dsConsole);
			LSGetIntString(&ls, spError, sCChangingServers);
			SMPrefixLink(link, &ls, dsConsole);
			
			OpenConnection(link);
		}
	}
}

pascal void DoSignoff(linkPtr link, const LongString *reason)
{
	if(link && link->conn)
	{
		LongString ls;
		int x;
		pServerQUITDataRec p;
		
		//Kill identd if it's open
		if(link->identConn)
			deleteConnection(&link->identConn);
		
		LSGetIntString(&ls, spError, sCClosing);
		SMPrefixLink(link, &ls, dsConsole);

		if(reason && reason->len)
		{
			ls=*reason;
			p.userhost = (StringPtr)1;
		}
		else
		{
			GetSignoffMessage(link, &ls);
			p.userhost = 0;
		}
		
		//callQuitPlugins
		p.link=link;
		p.username=link->CurrentNick;
		p.signoffMessage=&ls;
		p.isMe=1;
		p.dontDisplay = 0;
		p.dontSound = 1;
		runPlugins(pServerQUITMessage, &p);

		link->gotMOTD=0;
		link->neverConnected=1;
		
		if(link->connectStage == csOnline)
		{
			LSConcatStrAndLS("\pQUIT :", &ls, &ls);
			putServer(link, &ls);
		}

		x = signoffClearChannels(link);
		if(!x) //no channels. delete the connection
			deleteConnection(&link->conn);
		else
		{
			ConnStale(link->conn);
			link->conn = 0; //detach the connection, but don't delete it
		}
		
		link->serverStatus=S_OFFLINE;
		UpdateStatusLine();
		LSGetIntString(&ls, spError, sCClosed);
		SMPrefixLink(link, &ls, dsConsole);
		
		ConnectionMenuHilites();
	}
}

pascal int signoffClearChannels(linkPtr link)
{
	channelPtr ch = link->channelList;
	channelPtr next;
	int x = 0;
	
	while(ch)
	{
		next=ch->next;
		
		MWQuote(ch->window);
		ChDestroy(ch);
		x++;

		ch=next;
	}
	
	if(CurrentTarget.link == link)
		if(CurrentTarget.mw && CurrentTarget.mw->winType == chanWin)
			InvalTarget(&CurrentTarget);
	
	return x;
}

pascal void DNSLookup(Str255 addr, long saveReply)
{
	connectionPtr c;
	short p;
	UserListPtr u;
	LongString ls;
	DNSLookupDataPtr dldp;
	const char plugCall = sidr.yourInfo && saveReply;

	p = pos('@', addr);
	if(p)
		pdelete(addr, 1, p);
	
	p=countChar('.', addr);
	if(!p)
		if(addr[addr[0]]=='*')
		{
			addr[0]--;
			p=1;
		}
	
	if(!p)
	{
		u=ChFindUser(addr);
		if(u)
		{
			if(u->userhost[0])
			{
				p=pos('@', u->userhost);
				myCopy(u->userhost, p+1, 255, addr);
				saveReply=(long)NewPtr(sizeof(DNSLookupData));
				dldp=(DNSLookupDataPtr)saveReply;
				dldp->magic='SIRC';
				pstrcpy(u->nick, dldp->nick);
				pstrcpy(addr, dldp->host);
				dldp->u=(Ptr)u;
			}
			else
			{
				if(!saveReply)
				{
					LSParamString(&ls, GetIntStringPtr(spError, sDNSCantLookupYet),  addr, 0, 0, 0);
					SMPrefix(&ls, dsFrontWin);
				}
				addr[0]=0;
				return;
			}
		}
		else
		{
			if(!saveReply)
			{
				LSParamString(&ls, GetIntStringPtr(spError, sDNSNotInUserlist), addr, 0, 0, 0);
				SMPrefix(&ls, dsFrontWin);
			}
			
			addr[0]=0;
			return;
		}
	}
	
	if(isIPNumber(addr))
		c = ConnNewDNS(addr, connDNSNAME);
	else
		c = ConnNewDNS(addr, connDNSIP);
	
	if(c)
	{
		c->refCon=(void*)saveReply;
		if(plugCall)
			c->pluginRef=sidr.yourInfo;
		else
			c->pluginRef= 0;
	}
}

static void doBanList(LongString *args, char e)
{
	channelPtr ch;
	bansP f;
	long num;
	LongString ls;
	Str255 s1, s2;
	Str255 mask;
	ConstStringPtr str;
	
	ch=CurrentTarget.chan;
	if(ch)
	{
		LSNextArg(args, mask);
		str = GetIntStringPtr(spInfo, sBanSetBy);

		num=0;
		if(e)
			f = ch->exceptions;
		else
			f=ch->bans;

		while(f)
		{
			if(!mask[0] || maskMatch(f->ban, mask))
			{
				if(f->time)
				{
					DateString(f->time, shortDate, s1, 0);
					TimeString(f->time, true, s2, 0);
					LSParamString(&ls, str, f->ban, f->setBy, s1, s2);
				}
				else
					LSStrLS(f->ban, &ls);
				
				SMPrefix(&ls, dsNowhere);
				ChMsg(ch, &ls);
				num++;
			}
			f=f->next;
		}
		
		NumToString(num, s1);
		e = ((mask[0]!=0)*4) + ((num!=0)*2) + (e!=0);
		LSParamString(&ls, GetIntStringPtr(spInfo, sNoBans + e), ch->chName, mask, s1, 0);

		SMPrefix(&ls, dsNowhere);
		ChMsg(ch, &ls);
	}
}

pascal void doBroadcast(linkPtr link, const LongString *ls, char action)
{
	LongString out, chanList;
	int numInList;
	channelPtr ch;
	char allLinks = link == 0;
	
	if(allLinks)
		link=firstLink;
	while(link)
	{
		if(link->serverStatus == S_CONN)
		{
			FormatMessage(link->CurrentNick, ls, 0, 0, kNickSend | (action?kNickAction:kNickNormal), &out);
			
			for(ch=link->channelList;ch;ch=ch->next)
				if(ch->active)
					ChMsg(ch, &out);

			if(action)
			{
				LSConcatStrAndLS("\p\1ACTION ", ls, &out);
				LSAppend1(out, 1);
			}
			else
				out=*ls;
			
			LSConcatStrAndLS("\p :", &out, &out);
			
			LSStrLS("\pPRIVMSG ", &chanList);
			numInList=0;
			linkfor(ch, link->channelList)
			{
				if(ch->active)
				{
					if(!numInList || (ch->chName[0]+out.len+chanList.len<505))
					{
						LSConcatLSAndStr(&chanList, ch->chName, &chanList);
						LSAppend1(chanList, ',');
						numInList++;
					}
					else //flush
					{
						LSConcatLSAndLS(&chanList, &out, &chanList);
						SendCommand(link, &chanList);
						LSConcatStrAndStr("\pPRIVMSG ", ch->chName, &chanList);
						LSAppend1(chanList, ',');
						numInList=1;
					}
				}
			}
			
			if(numInList)
			{
				LSConcatLSAndLS(&chanList, &out, &chanList);
				SendCommand(link, &chanList);
			}
		}
		
		if(allLinks)
			link = link->next;
		else
			link=0;
	}
}

pascal void DoONotice(channelPtr ch, const LongString *text)
{
	LongString out;
	LongString wallopstext;
	UserListPtr ul;
	linkPtr link=ch->link;
	int cnt;
	int method;
	
	//Determine method to use based on server settings
	if(HTIsSet(link->serverOptions, "\pWALLCHOPS"))
		method = 3;
	else //use the prefs
		method = link->linkPrefs->onoticeMethod;
	
	//output
	LSConcatStrAndStrAndStr("\p[WALLOPS/", ch->chName, "\p] ", &wallopstext);
	LSConcatLSAndLS(&wallopstext, text, &wallopstext);
	switch(method)
	{
		case 1: //standard
			//Ok, this sucks. Gotta put together a list of names, and make sure the bitch isn't too long and all that fun crap...
			out.len=0;
			cnt = 0;
			linkfor(ul, ch->userlist)
				if(ul->isOp)
				{
					if(out.len+wallopstext.len>500)
					{
						LSConcatStrAndLS("\pNOTICE ", &out, &out);
						LSAppend1(out, ' ');
						LSConcatLSAndLS(&out, &wallopstext, &out);
						HandleCommand(link, &out);
						out.len=0;
						cnt = 0;
					}
					LSConcatLSAndStr(&out, ul->nick, &out);
					LSAppend1(out, ',');
					cnt++;
				}

			if(out.len)
			{
				LSConcatStrAndLS("\pNOTICE ", &out, &out);
				LSAppend1(out, ' ');
				LSConcatLSAndLS(&out, &wallopstext, &out);
				HandleCommand(link, &out);
			}
			break;
		
		case 2: //notice @#channel
			LSConcatStrAndStr("\pNOTICE @", ch->chName, &out);
			LSAppend1(out, ' ');
			LSConcatLSAndLS(&out, &wallopstext, &out);
			HandleCommand(link, &out);
			break;
		
		case 3: //wallchops
			LSConcatStrAndStr("\pWALLCHOPS ", ch->chName, &out);
			LSAppend1(out, ' ');
			LSConcatLSAndLS(&out, &wallopstext, &out);
			HandleCommand(link, &out);
			break;
	}
}

pascal void ChannelListAdd(linkPtr link, ConstStr255Param n)
{
	Str255 s1;
	int x;
	linkPrefsPtr lp = link->linkPrefs;
	
	pstrcpyucase(n,s1);
	for(x=0;x<10;x++)
	{
		if(lp->recentChannels[x][0])
		{
			if(pstrcasecmp2(s1,lp->recentChannels[x]))
			{
				if(x>0)
				{
					//ok, we found one, so we want to move up everything from rn[0] to rn[x-1]. rn[x] is getting moved to rn[0]
					for(;x>=0;x--)
						pstrcpy(lp->recentChannels[x-1], lp->recentChannels[x]);
					pstrcpy(n, lp->recentChannels[0]);
				}
				return;
			}
		}
		else
			break;
	}
	//didn't find
	if(x==9)
		x--;
	for(;x>=0;x--)
		pstrcpy(lp->recentChannels[x-1], lp->recentChannels[x]);
	pstrcpy(n, lp->recentChannels[0]);
}

pascal void NickListAdd(linkPtr link, ConstStr255Param n)
{
	Str255 s1;
	int x;
	linkPrefsPtr lp = link->linkPrefs;
	
	pstrcpyucase(n,s1);
	for(x=0;x<10;x++)
	{
		if(	lp->recentNicks[x][0])
		{
			if(pstrcasecmp2(s1,	lp->recentNicks[x]))
			{
				if(x>0)
				{
					//ok, we found one, so we want to move up everything from rn[0] to rn[x-1]. rn[x] is getting moved to rn[0]
					for(;x>=0;x--)
						pstrcpy(lp->recentNicks[x-1], lp->recentNicks[x]);
					pstrcpy(n, lp->recentNicks[0]);
				}
				return;
			}
		}
		else
			break;
	}
	//didn't find
	if(x==9)
		x--;
	for(;x>=0;x--)
		pstrcpy(lp->recentNicks[x-1], lp->recentNicks[x]);
	pstrcpy(n, lp->recentNicks[0]);
}

pascal void sendCTCP(linkPtr link, ConstStr255Param t, LongString *ls)
{
	short i;
	Str255 com;
	LongString tls;
	
	if(link && link->serverStatus==S_CONN)
	{
		i=LSPosChar(' ', ls);
		if(!i)
		{
			LSCopyString(ls, 1, 255, com);
			ls->len=0;
		}
		else
		{
			LSCopyString(ls, 1, i-1, com);
			LSDelete(ls, 1, i);
		}
		LSConcatStrAndStr("\pPRIVMSG ", t, &tls);
		LSAppend3(tls, 0x203a0100); //space + : + \1 + _
		ucase(com);
		LSConcatLSAndStr(&tls, com, &tls);
		LSAppend1(tls, ' ');
		LSConcatLSAndLS(&tls, ls, &tls);
		LSAppend1(tls, 1);
		SendCommand(link, &tls);
	}
	else
		StatusMsg(link, 1);
}

pascal void SendCommand(linkPtr link, LongString *ls)
{
	if(link && ls->len && link->serverStatus==S_CONN)
		putServer(link, ls);
}

pascal void rejoinDeactiveChannels(linkPtr link)
{
	MWPtr mw;
	Str255 s;
	LongString ls;

	ls.len=0;
	linkfor(mw, mwl)
		if(mw->link == link && mw->winType == chanWin)
		{
			MWGetName(mw, s);

			if(ls.len+s[0]<505)
			{
				if(ls.len)
					LSAppend1(ls, ',');
				LSConcatLSAndStr(&ls, s, &ls);
			}
			else
			{
				LSConcatStrAndLS("\pJOIN ", &ls, &ls);
				SendCommand(link, &ls);
				LSStrLS(s, &ls);
			}
		}
	
	if(ls.len)
	{
		LSConcatStrAndLS("\pJOIN ", &ls, &ls);
		SendCommand(link, &ls);
	}
}

pascal void finishConnect(linkPtr link)
{
	Str255 st;
	short i;
	LongString ls;
	linkPrefsPtr lp = link->linkPrefs;
	notifyPtr n;
	
	if(CurrentTarget.bad)
		SetTarget(consoleWin, &CurrentTarget);
	
	pstrcpy("\p +", st); //space+; can stack modechanges without multiple plusses
	if(lp->modeI)
		SAppend1(st, 'i');
	if(lp->modeW)
		SAppend1(st, 'w');
	if(lp->modeS)
		SAppend1(st, 's');

	if(st[0]>2)
	{
		LSConcatStrAndStrAndStr("\pMODE ", link->CurrentNick, st, &ls);
		SendCommand(link, &ls);
	}
	
	if(lp->useConnectMacro)
	{
		pstrcpy(lp->connectMacro, st);
		while(st[0])
		{
			i=pos(';', st);
			if(!i)
			{
				LSStrLS(st, &ls);
				st[0]=0;
			}
			else
			{
				BlockMoveData(st, ls.data, i);
				ls.len = i-1;
				pdelete(st, 1, i);
			}
			
			//Clear leading spaces
			while(ls.len && ls.data[1] == ' ')
				LSDelete(&ls, 1, 1);
			
			HandleCommand(link, &ls);
		}
	}
	
	if(mainPrefs->autoRejoinReconnect)
		rejoinDeactiveChannels(link);
	
	link->alreadyFinished=1;
	
	//reset notify list
	linkfor(n, link->notifyList)
	{
		n->waitingForResponse=0;
		n->signedon=0;
		n->changed=0;
	}
	
	link->connectionAttempts = -1;

	if(mainPrefs->resendAwayOnReconnect && link->awayString)
	{
		if(link->awayString[0] == 0) //unnecessary, but I do it anyways
		{
			DisposePtr((Ptr)link->awayString);
			link->awayString = 0;
		}
		else
		{
			LSConcatStrAndStr("\pAWAY :", link->awayString, &ls);
			SendCommand(link, &ls);
			LSParamString(&ls, GetIntStringPtr(spInfo, sReconnectAway), link->awayString, 0, 0, 0);
			SMPrefixLink(link, &ls, dsConsole);
		}
	}
}

pascal void RegUser(linkPtr link)
{
	if(!link->UserRegistered)
	{
		Str255 st;
		long m;
		channelPtr ch, next;
		LongString ls;
		linkPrefsPtr lp;
		pServiceUserRegData urp;
		
		link->gotMOTD = link->alreadyFinished = link->finishedMOTD=false;
		link->connectingNickAttempt=-2;
		LSGetIntString(&ls, spError, sCRegistering);
		SMPrefixLink(link, &ls, dsConsole);
		ch=link->channelList;
		while(ch)
		{
			next=ch->next;
			ChDestroy(ch);
			ch=next;
		}
		link->CurrentServer[0]=0;

		urp.link = link;
		urp.registered = false;
		runIndService(userRegistrationClass, pServiceUserRegMessage, &urp);
		
		if(!urp.registered)
		{
			lp = link->linkPrefs;
			
			//PASS always comes before NICK and USER
			if(lp->serverPass[0])
			{
				LSConcatStrAndStr("\pPASS ", lp->serverPass, &ls);
				SendCommand(link, &ls);
			}

			LSConcatStrAndStr("\pNICK ", link->CurrentNick, &ls);
			NickListAdd(link, link->CurrentNick);
			SendCommand(link, &ls);
			
			m = (lp->modeI << 5) + (lp->modeW << 6) + (lp->modeS << 7);
			NumToString(m, st);
			LSStrCat(6, &ls, "\pUSER ", lp->user, "\p ", st, "\p . :", lp->real);
			SendCommand(link, &ls);
		}
		
		link->connectedTime=now;
		link->UserRegistered=1;
		link->outstandingUSERHOST = 0; //Probabally unnecessary, but it can't hurt.
		LinkSetStage(link, csSentRegistration);
	}
}


#pragma mark -
#pragma mark ¥ÊCommand Handlers

static void _ucBroadcast(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	doBroadcast(0, rest, false);
	
	s->len = 0;
}

static void _ucBract(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	doBroadcast(0, rest, true);
	
	s->len = 0;
}

static void _ucQuit(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	doQuit(rest);
	
	s->len = 0;
}

static void _ucConnectDisconnect(char connect, LongString *rest, LongString *s)
{
	Str255 s1;
	char b;
	long l;
	
	LSNextArg(rest, s1);
	if(isNumber(s1))
	{
		StringToNum(s1, &l);
		l--;
	}
	else
	{
		linkPtr lp;
		
		ucase(s1);
		b=0;
		linkfor(lp, firstLink)
			if(pstrcasecmp2(s1, lp->linkPrefs->linkName))
			{
				b = 1;
				break;
			}
		if(!b)
			l=-1;
	}
	
	if(l>=0 && l < 10)
	{
		linkPtr lnk = GetLinkNum(l);
		
		if(connect)
		{
			if(rest->len)
				DoServer(lnk, rest, 0);
			else
				OpenConnection(lnk);
		}
		else
			DoSignoff(lnk, rest);
	}

	s->len=0;
}

static void _ucC(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	_ucConnectDisconnect(true, rest, s);
}

static void _ucD(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	_ucConnectDisconnect(false, rest, s);
}

static void _ucClear(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com,rest)
	MWPtr mw = CurrentTarget.mw;
	if(mw)
	{
		WEDeactivate(mw->we);
		WESetSelection(0,0x7FFFFFFF, mw->we);
		WEDelete(mw->we);
		SetControlMaximum(mw->vscr, 0);
		if(IsWindowHilited(mw->w))
			WEActivate(mw->we);
	}
	s->len=0;
}

static void _ucDNS(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1;
	
	LSNextArg(rest, s1);
	if(s1[0])
		DNSLookup(s1, 0);
	s->len=0;
}

static void _ucBans(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	doBanList(rest, false);
	s->len = 0;
}

static void _ucExceptions(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	doBanList(rest, true);
	s->len = 0;
}

static void _ucIgnore(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	DoIgnore(rest, false);
	s->len = 0;
}

static void _ucJoin(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	if(rest->len)
	{
		LSMakeStr(*rest);
		MakeChannel(rest->data);
		LSConcatStrAndStr("\pJOIN ", rest->data, s);
	}
}

static void _ucKill(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1;
	
	LSNextArg(rest, s1);
	if(s1[0])
	{
		LSConcatStrAndStrAndStr("\pKILL ", s1, "\p :", s);
		LSConcatLSAndLS(s, rest, s);
	}
}

static void _ucPart(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1;
	
	s1[0]=0;
	LSNextArgND(rest, s1);
	
	if(IsChannel(s1))
		LSNextArg(rest, 0);
	else
	{
		if(!CurrentTarget.bad && CurrentTarget.type == targChannel)
			pstrcpy(CurrentTarget.chan->chName, s1);
	}
	
	if(s1[0])
	{
		LSConcatStrAndStr("\pPART ", s1, s);
		if(rest->len)
		{
			LSConcatLSAndStrAndLS(s, "\p :",rest, s);
		}
	}
	else
		s->len=0;
}

static void _ucRping(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	UnsignedWide t;
	Str255 s1, s2, s3;
	
	LSNextArg(rest, s1);
	SAppend1(s1, ' ');
	Microseconds(&t);
	*(unsigned long long*)&t/=1000;
	ntohl_str(t.hi, s2);
	ntohl_str(t.lo, s3);
	LSStrCat4(s, "\pRPING ", s1, s2, s3);
}

static void _ucSay(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	if(rest->len)
		HandleMessage(rest, &CurrentTarget);
	s->len = 0;
}

static void _ucTopic(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1;
	
	LSNextArg(rest, s1);
	if(rest->len)
	{
		LSConcatStrAndStrAndStr("\pTOPIC ", s1, "\p :", s);
		LSConcatLSAndLS(s, rest, s);
	}
	else
		LSConcatStrAndStr("\pTOPIC ", s1, s);
}

static void _ucUnixtime(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1, s2, s3;
	long l;
	
	LSNextArg(rest, s1);
	if(s1[0])
	{
		StringToNum(s1, &l);
		l+=ircDateModifier;
		DateString(l, longDate, s2, 0);
		TimeString(l, true, s3, 0);
		LSConcatStrAndStrAndStr(s2, "\p @ ", s3, s);
		LSConcatStrAndStr("\pUnix time \"", s1, rest);
		LSAppend3(*rest, 0x223A2000); //": _
		LSConcatLSAndLS(rest, s, rest);
		SMPrefix(rest, dsFrontWin);
	}
	s->len=0;
}

static void _ucWallops(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	if(rest->len)
		LSConcatStrAndLS("\pWALLOPS :", rest, s);
}

static void _ucWallchops(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1;
	
	LSNextArg(rest, s1);
	if(s1[0] && rest->len)
	{
		LSConcatStrAndStrAndStr("\pWALLCHOPS ", s1, "\p :", s);
		LSConcatLSAndLS(s, rest, s);
	}
}

static void _ucNamesWho(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link)
	if(!rest->len && !CurrentTarget.bad)
	{
		switch(CurrentTarget.type)
		{
			case targChannel:
			case targQuery:
			case targDCC:
				LSConcatStrAndStrAndStr(com, "\p ", MWGetName(CurrentTarget.mw, 0), s);
				break;
		}
	}
}

static void _ucQuote(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	LSCopy(rest, 0, 0, s);
}

static void _ucWhois(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	if(!rest->len && !CurrentTarget.bad)
	{
		switch(CurrentTarget.type)
		{
			case targQuery:
			case targDCC:
				LSConcatStrAndStr("\pWHOIS ", MWGetName(CurrentTarget.mw, 0), s);
				break;
		}
	}
	else
		LSConcatStrAndLS("\pWHOIS ", rest, s);
}

static void _ucWhowas(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	LSConcatStrAndLS("\pWHOWAS ", rest, s);
}

static void _ucVersion(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com,s)
	LSConcatStrAndStrAndStr("\pClient is ShadowIRC ", CL_VERSION, "\p (", rest);
	LSConcatLSAndStr(rest, cdt, rest);
	LSAppend1(*rest, ')');
	SMPrefix(rest, dsFrontWin);
}

static void _ucDebug(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com,rest)
	debugOn=!debugOn;
	if(debugOn)
		LSStrLS("\pDebug On", s);
	else
		LSStrLS("\pDebug Off", s);
	LineMsg(s);
	s->len=0;
}

static void _ucHttp(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	LSConcatStrAndLS("\phttp://", rest, s);
	LSMakeStr(*s);
	OpenURL(s->data);
	s->len = 0;
}

static void _ucFtp(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	LSConcatStrAndLS("\pftp://", rest, s);
	LSMakeStr(*s);
	OpenURL(s->data);
	s->len = 0;
}

static void _ucNick(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	Str255 s1;
	
	LSNextArg(rest, s1);
	if(s1[0] || !link)
		LSConcatStrAndStr("\pNICK ", s1, s);
	else
		LSConcatStrAndStr("\pNICK ", link->linkPrefs->nick, s);
}

static void _ucUmode(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	if(link)
	{
		LSConcatStrAndStr("\pMODE ", link->CurrentNick, s);
		LSAppend1(*s, ' ');
		LSConcatLSAndLS(s, rest, s);
	}
}

static void _ucCTCP(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(link,com)
	Str255 s1;
	short i;
	
	i=LSPosChar(' ', rest);
	if(!i)
	{
		LSCopyString(rest, 1, 255, s1);
		rest->len=0;
	}
	else
	{
		LSCopyString(rest, 1, i-1, s1);
		LSDelete(rest, 1, i);
	}
	sendCTCP(link, s1, rest);
	s->len=0;
}

static void _ucDCC(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	LSMakeStr(*rest);
	DCCCommand(link, rest->data);
	s->len=0;
}

static void _ucSignoff(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	DoSignoff(link, rest);
	s->len = 0;
}

static void _ucISON(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	HandleISON(link, rest);
	s->len=0;
}

static void _ucMsg(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
	Str255 s1;
	char quiet = com[0] != 4;
	
	//That's not really a good way of determining if it's msgq, but, oh well...
	
	LSNextArg(rest, s1);

	if(mbInput && mbnum >= 0 && mbnum < MAXMB && pstrcmp(s1, messagebuffers[mbnum]->nick))
		link = messagebuffers[mbnum]->link;

	if(IsChannel(s1))
	{
		LSConcatStrAndStr("\p> ", s1, s);
		LSAppend1(*s, ' ');
		LSConcatLSAndLS(s, rest, s);
	}
	else
	{
		if(quiet) //only tack on if /msg; since ShadowIRC uses /msgq internally, this stops stuff like /ctcp from adding to list
		{
			MBNewMessage(link, s1);
			if(mainPrefs->autoQuery && (mainPrefs->autoQueryOpen != aqIncoming) && !pos(',', s1))
				DoJoinQuery(s1, link);
		}
		LSConcatStrAndStrAndStr("\p--> *", s1, "\p* ", s);
		LSConcatLSAndLS(s, rest, s);
	}
	
	if(quiet) //otherwise it's len 4 and msgq
	{
		MWColor(s, sicOutgoing);
		ChannelMsg(link, s1, s); //might be a channel
	}
	LSConcatStrAndStrAndStr("\pPRIVMSG ", s1, "\p :", s);
	LSConcatLSAndLS(s, rest, s);
	
	SendCommand(link, s);
	s->len = 0;
}

static void _ucNotice(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
	Str255 s1;
	char quiet = com[0] == 7;
	
	LSNextArg(rest, s1);
	FormatNick(s1, s, 0, kNickNotice + kNickSend);
	LSConcatLSAndLS(s, rest, s);
	if(!quiet)
	{
		MWColor(s, sicNotice);
		ChannelMsg(link, s1,s); //might be a channel
	}
	LSConcatStrAndStrAndStr("\pNOTICE ", s1, "\p :", s);
	LSConcatLSAndLS(s, rest, s);
}

static void _ucNotify(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	DoNotify(link, rest);
	s->len=0;
}

static void _ucONotice(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	if(CurrentTarget.chan)
		DoONotice(CurrentTarget.chan, rest);
	else
	{
		LSGetIntString(rest, spInfo, sOnoticeRequiresChannel); //The /onotice command can only be used in a channel.
		SMPrefixLink(link, rest, dsFrontWin);
	}
	s->len=0;
}

static void _ucQuery(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	Str255 s1;
	char b;
	
	LSNextArg(rest, s1);
	if(s1[0])
	{
		int x, m = s1[0];
		char ch;
		
		b=0;
		for(x=1;x<=m;x++)
		{
			ch = s1[x];
			if(ch == '~' || ch=='(' || ch == ')' || ch=='#' || ch=='&' || ch=='+')
			{
				LSGetIntString(rest, spInfo, sCantMakeQuery); //Can't make query window. Illegal character in nickname.
				SMPrefixLink(link, rest, dsFrontWin);
				b = 1;
				break;
			}
		}
		if(!b)
		{
			MWPtr mw = DoJoinQuery(s1, link);
			if(rest->len) //they have a message. Send it too.
			{
				target targ;
				
				SetTarget(mw, &targ);
				HandleMessage(rest, &targ);
			}
		}
	}
	s->len=0;
}

static void _ucServer(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	s->len=0;
	DoServer(link, rest, 0);
}

static void _ucAway(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	LSConcatStrAndLS("\pAWAY :", rest, s);
	LSMakeStr(*rest);
	if(link)
	{
		if(link->awayString)
			DisposePtr((Ptr)link->awayString);
		if(rest->data[0])
			link->awayString = NewPString(rest->data);
		else
			link->awayString = 0;
	}
}

static void _ucCping(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	Str255 s1, s2;
	UnsignedWide t;
	short i;
	
	Microseconds(&t);
	*(unsigned long long*)&t/=1000;
	ntohl_str(t.hi, s1);
	ntohl_str(t.lo, s2);
	LSConcatStrAndStrAndStr("\pPING ", s1, s2, s);
	i=LSPosChar(' ', rest);
	if(i)
	{
		LSCopy(rest, i, 255, rest);
		LSConcatLSAndLS(s, rest, s);
		rest->len=i-1;
	}
	if(!rest->len && link)
		pstrcpy(link->CurrentNick, s1);
	else
		LSCopyString(rest, 1, 255, s1);
	sendCTCP(link, s1, s);
	s->len=0;
}

static void _ucKick(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	Str255 s1, s2;
	
	if(rest->len)
	{
		LSNextArg(rest, s1);
		MakeChannel(s1);
		LSNextArg(rest, s2);
		LSStrCat(5, s, "\pKICK ", s1, "\p ", s2, "\p :");
		if(rest->len)
			LSConcatLSAndLS(s, rest, s);
		else if(link)
			LSConcatLSAndStr(s, link->linkPrefs->kickMessage, s);
	}
}

static void _ucMe(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	Str255 s1;
	ConstStringPtr sp;
	
	if(!CurrentTarget.bad)
	{
		LSCopyString(rest, 1, 255, s1);
		LSConcatStrAndLS("\pACTION ", rest, s);
		
		FormatMessage(CurrentTarget.link->CurrentNick, rest, 0, 0, kNickAction | kNickSend, rest);
		switch(CurrentTarget.type)
		{
			case targChannel:
				ChMsg(CurrentTarget.chan, rest);
				sendCTCP(link, CurrentTarget.chan->chName, s);
				break;
			
			case targQuery:
				sp = MWGetName(CurrentTarget.mw, 0);
				ChannelMsg(link, sp, rest);
				sendCTCP(link, sp, s);
				break;
			
			case targDCC:
				MWMessage(CurrentTarget.mw, rest);
				LSConcatStrAndLS("\p\1", s, s);
				LSAppend1(*s, 1);
				DCCSSayQuiet(&CurrentTarget.dcc, s);
				break;
		}
	}
	s->len=0;
}

static void _ucUserhost(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com,rest,s)
	if(link)
		link->outstandingUSERHOST++;
}

pascal void ServerCommands(LongString *ls, linkPtr link);
static void _ucServerCommand(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s)
{
#pragma unused(com)
	if(debugOn)
		ServerCommands(rest, link);
	s->len = 0;
}

typedef void (*UserCommandProcPtr)(linkPtr link, ConstStr255Param com, LongString *rest, LongString *s);

typedef struct UserCommandItem {
	const StringPtr commandName;
	const UserCommandProcPtr commandFunc;
} UserCommandItem;

static const UserCommandItem TranslateCommandList[] = {
	//Commands that don't have anything to do with the connection
	{"\pBROADCAST",	_ucBroadcast},
	{"\pBRACT",	_ucBract},
	{"\pBYE",	_ucQuit},
	{"\pQUIT",	_ucQuit},
	{"\pEXIT",	_ucQuit},
	{"\pC",	_ucC},
	{"\pD",	_ucD},
	{"\pCLEAR",	_ucClear},
	{"\pDNS",	_ucDNS},
	{"\pBANS",	_ucBans},
	{"\pEXCEPTIONS",	_ucExceptions},
	{"\pIGNORE",	_ucIgnore},
	{"\pJOIN",	_ucJoin},
	{"\pJ",	_ucJoin},
	{"\pKILL",	_ucKill},
	{"\pPART",	_ucPart},
	{"\pLEAVE",	_ucPart},
	{"\pRPING",	_ucRping},
	{"\pSAY",	_ucSay},
	{"\pTOPIC",	_ucTopic},
	{"\pUNIXTIME",	_ucUnixtime},
	{"\pWALLOPS",	_ucWallops},
	{"\pWALLCHOPS",	_ucWallchops},
	{"\pNAMES",	_ucNamesWho},
	{"\pWHO",	_ucNamesWho},
	{"\pQUOTE",	_ucQuote},
	{"\pRAW",	_ucQuote},
	{"\pWHOIS",	_ucWhois},
	{"\pWH",	_ucWhois},
	{"\pWHOWAS",	_ucWhowas},
	{"\pWW",	_ucWhowas},
	{"\pVERSION",	_ucVersion},
	{"\pDEBUG",	_ucDebug},
	{"\pHTTP",	_ucHttp},
	{"\pFTP",	_ucFtp},
	
	//Safe commands that directly access the link
	{"\pNICK",	_ucNick},
	{"\pUMODE",	_ucUmode},
	
	//Commands that deal with the connection, but don't directly access the link
	{"\pCTCP",	_ucCTCP},
	{"\pDCC",	_ucDCC},
	{"\pSIGNOFF",	_ucSignoff},
	{"\pISON",	_ucISON},
	{"\pM",	_ucMsg},
	{"\pMSG",	_ucMsg},
	{"\pMSGQ",	_ucMsg},
	{"\pNOTICE",	_ucNotice},
	{"\pNOTICEQ",	_ucNotice},
	{"\pNOTIFY",	_ucNotify},
	{"\pONOTICE",	_ucONotice},
	{"\pQ",	_ucQuery},
	{"\pQUERY",	_ucQuery},
	{"\pSERVER",	_ucServer},
	
	//Fixed unsafe commands
	{"\pAWAY",	_ucAway},
	{"\pPING",	_ucCping},
	{"\pCPING",	_ucCping},
	{"\pKICK",	_ucKick},
	{"\pME",	_ucMe},
	{"\pUSERHOST",	_ucUserhost},
	
	//Debugging commands
	{"\pSERVERCOMMAND", _ucServerCommand},
	
	{0, 0}
};

static char _ucTranslate(linkPtr link, ConstStringPtr com, LongString *rest, LongString *s)
{
	UserCommandItem const *uci = TranslateCommandList;
	
	while(uci->commandName)
	{
		if(pstrcmp(com, uci->commandName))
		{
			uci->commandFunc(link, com, rest, s);
			return true;
		}
		uci++;
	}
	
	return false;
}

static void TranslateCommand(linkPtr link, LongString *s)
{
	LongString rest;
	Str255 com;
	short i;
	pCommandDataRec p;
	
	if((s->len)&&(s->data[1]==CmdChar))
		LSDelete(s, 1, 1);

	if((s->len)&&(s->data[1]==CmdChar)) //in case we get a //command by accideent.
		LSDelete(s, 1, 1);

	i=LSPosChar(' ', s);
	if(!i)
	{
		LSCopyString(s, 1, 255, com);
		rest.len=0;
	}
	else
	{
		LSCopyString(s, 1, i-1, com);
		while((i<=s->len) && (s->data[i]==' '))
			i++;
		
		LSCopy(s, i, maxLSlen, &rest);
	}
	
	ucase(com);
	
	p.link=link;
	p.com=com;
	p.rest=&rest;
	p.outgoing=s;
	p.dontProcess=0;
	runPlugins(pUserCommandMessage, &p);
	
	if(!p.dontProcess)
		_ucTranslate(link, com, &rest, s);
}

pascal void HandleCommand(linkPtr link, LongString *ls)
{
	TranslateCommand(link, ls);
	if(link && ls->len)
	{
		if(link->serverStatus==S_CONN)
			putServer(link, ls);
		else
			StatusMsg(link, 1);
	}
	UpdateStatusLine();
}