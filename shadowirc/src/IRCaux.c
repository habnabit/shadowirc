/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2005 John Bafford
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
#include "StringList.h"
#include "MsgWindows.h"
#include "IRCGlobals.h"
#include "DCC.h"
#include "connections.h"
#include "IRCAux.h"
#include "utils.h"
#include "channels.h"
#include "IRCChannels.h"
#include "IRCCommands.h"
#include "TCP.h"
#include "InputLine.h"
#include "Inline.h"
#include "plugins.h"
#include "IRCNotify.h"
#include "TextManip.h"
#include "MenuCommands.h"

static void startConnection(linkPtr link);

pascal void ServerOK(short status, linkPtr link)
{
	short a, n;
	Str255 s;
	LongString ls;

	n=0;
	switch(status)
	{
		case C_SearchFailed:
			a=S_OFFLINE;
			n=3;
			break;
		case C_FailedToOpen:
			a=S_OFFLINE;
			n=5;
			break;
		case C_Closed:
			a=S_OFFLINE;
			n=S_OFFLINE;
			break;
		case C_NoEvent:
			a=C_NoEvent;
			break;
		case kServerOKBadSOCKS:
			a = kServerOKBadSOCKS; //keeps it from reconnecting
			n = -1;
			break;
		case C_Established:
			a = S_CONN;
			break;
		default:
			NumToString(status, s);
			LSStrCat4(&ls, "\pserverok:unknown status: ", s, "\p: ", link->linkPrefs->linkName);
			LineMsg(&ls);
			return;
	}
	
	link->serverStatus=a;
	if(n)
	{
		LinkSetStage(link, csNil);
		signoffClearChannels(link);
		deleteConnection(&link->conn);
		
		if(n>0)
		{
			LSGetIntString(&ls, spError, n);
			SMPrefixLink(link, &ls, 1);
		}
		
		ConnectionMenuHilites();
	}
	UpdateStatusLine();
}

pascal void ConnPut(connectionPtr *cn, const void* p, long len)
{
	short status;
	connectionPtr conn = *cn;
	
	status = ConnSend(conn, p, len);
	if(conn->connType==connIRC)
		ServerOK(status, conn->link);
	else if(conn->connType==connPLUGIN)
	{
		pConnectionData p;

		p.conn=conn;
		p.event=status;
		p.sendEvent=true;
		p.data=0;
		
		if(conn->textOrBinary)
			runIndPlugin(conn->pluginRef, pTextConnectionMessage, &p);
		else
			runIndPlugin(conn->pluginRef, pDataConnectionMessage, &p);
	}
}

pascal void ConnPutText(connectionPtr *cn, void* p, long len)
{
	long i;

	for(i=0;i<len;i++)
		((unsigned char*)p)[i]=ISOEncode[((unsigned char*)p)[i]];
	
	if(debugOn)
	{
		LongString ls;
		
		ls.len=len;
		if(ls.len>maxLSlen-5)
			ls.len=maxLSlen-6;
		BlockMoveData(p, &ls.data[6], ls.len);
		
		*(long*)&ls.data[0]=0x053E3E20;
		*(short*)&ls.data[4]=0x08FF;
		ls.len+=5;
		LineMsg(&ls);
	}

	ConnPut(cn, p, len);
}

pascal void ConnPutLS(connectionPtr *cn, LongString *ls)
{
	short x = ls->len;
	connectionPtr conn = *cn;
	
	if(conn->lineTerm == eolLF)
		LSAppend1(*ls, 10)
	else if(conn->lineTerm == eolCR)
		LSAppend1(*ls, 13)
	else if(conn->lineTerm == eolCRLF)
		LSAppend2(*ls, 0x0D0A)
	
	ConnPutText(cn, &ls->data[1], ls->len);
	ls->len=x;
}

pascal void putServer(linkPtr link, LongString *ls)
{
	if(link && link->conn)
		ConnPutLS(&link->conn, ls);
}

void MakeChannel(linkPtr link, StringPtr s)
{
	if(!IsChannel(link, s))
	{
		if(s[0]>254)
			s[0]--;
		pstrcpy(s, &s[1]);
		s[0]++;
		s[1]='#';
	}
}

pascal void StatusMsg(linkPtr link, short n)
{
	LongString ls;
	
	LSGetIntString(&ls, spError, n);
	SMPrefixLink(link, &ls, 1);
}

#define myvp(x) do{StatusMsg(link, x);c=0;}while(0)
pascal char ValidPrefs(linkPtr link)
{
	char c=1;
	linkPrefsPtr lp=link->linkPrefs;
	
	if(!lp->serverName[0])
		myvp(sPrefNoServerSpecified);
	if(lp->serverPort<1)
		myvp(sPrefNoServerPort);
	if(!lp->nick[0])
		myvp(sPrefNoNick);
	if(!lp->user[0])
		myvp(sPrefNoUsername);
	if(!lp->real[0])
		myvp(sPrefNoRealname);
	
	return c;
}

void LinkSetStage(linkPtr link, short stage)
{
	pServiceCWLinkStateChangeData p;
	
	p.link = link;
	p.connectStage = link->connectStage = stage;
	runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
}

pascal void LinkSuccessfulConnection(linkPtr link, char reg)
{
	link->serverStatus = S_CONN;
	LinkSetStage(link, csConnected);
	UpdateStatusLine();
	if(reg)
		RegUser(link);
}

static void CreateIdentdConn(linkPtr link)
{
	if(mainPrefs->firewallType != fwSOCKS5) //Why?
	{
		LongString ls;
		connectionPtr identConn;
		connectionPtr conn = link->conn;
		
		link->identConn = identConn = newConnection(connIDENTD);
		if(identConn)
		{
			LSGetIntString(&ls, spError, sOpeningIdentd);
			SMPrefixLink(link, &ls, 1);
			
			memcpy(identConn->sas, conn->sas, sizeof(struct sockaddr_storage));
			identConn->port = 113;
			identConn->link=link;
			if(mainPrefs->firewallType == fwSOCKS4A || mainPrefs->firewallType == fwSOCKS4)
				identConn->refCon = NewPString(mainPrefs->socksUser);
			else
				identConn->refCon = NewPString(conn->link->linkPrefs->user);
			
			//Allow for a 10 connection backlog. This is more than enough.
			ConnNewListen(identConn, AF_UNSPEC, 10);
		}
	}
}

void DisplayLookupResult(connectionPtr conn)
{
	LongString ls;
	char hbuf[NI_MAXHOST];
	Str255 pstr;

	getnameinfo((struct sockaddr *)conn->sas, conn->sas->ss_len, hbuf, sizeof(hbuf), NULL, 0, NI_NUMERICHOST);
	CopyCStringToPascal(hbuf, pstr);
	LSParamString(&ls, GetIntStringPtr(spInfo, sIPForIs), conn->name, pstr, 0, 0);
	SMPrefix(&ls, dsConsole);
}

//We found the IP for the server we're trying to connect to. Connect to the server.
int connection2(connectionPtr conn)
{
	if(ConnNewActive(conn)) //this makes the connection to the (socks) server
	{
		if(conn->socks.type == connIRC)
		{
			CreateIdentdConn(conn->link);
			LinkSetStage(conn->link, csOpeningConnection);
			conn->link->serverStatus=S_OPENING;
			UpdateStatusLine();
		}
	}
	else
	{
		if(conn->socks.type == connIRC)
		{
			LinkSetStage(conn->link, csFailedToConnect);
			ServerOK(C_FailedToOpen, conn->link);
		}
                return -1;
	}
        return 0;
}

static void startConnection(linkPtr link)
{
	linkPrefsPtr lp=link->linkPrefs;
	LongString ls;
	StringPtr search;
	
	pstrcpy(lp->nick, link->CurrentNick);
	pstrcpy(lp->serverName, link->CurrentServer);
	
	link->alreadyFinished = 
	link->finishedMOTD=
	link->gotMOTD = 
	link->UserRegistered = 
	link->neverConnected = false;
	link->serverStatus=S_LOOKUP;
	link->firstHalfOfIncoming.len=0;
	link->numOutstandingISON=0;
	link->lastConnAttempt = now;
	link->waitingToRetry = 
	link->needUserHosts = false;
	link->isAway = false;
	UpdateStatusLine();
	
	killISONs(link);
	newIRCConnection(link);
	if(!link->serverTryingName[0])
		ConnSetup(link->conn, lp->serverName, lp->serverPort);
	else
		ConnSetup(link->conn, link->serverTryingName, link->serverTryingPort);
		
	if(mainPrefs->firewallType == fwSOCKS4)
	{
		link->conn->socks.secondLookup = 1;
		search = link->conn->socks.name;
	}
	else
		search = link->conn->name;
	
	if(!ConnFindAddress(link->conn, search))
	{
		LinkSetStage(link, csLookingUp);
		LSParamString(&ls, GetIntStringPtr(spInfo, sLookingUpIP), search, 0, 0, 0);
		SMPrefix(&ls, dsConsole);
	}
	else
	{
		LinkSetStage(link, csFailedToLookup);
		ServerOK(C_SearchFailed, link);
	}
}

pascal char OpenConnection(linkPtr link)
{
	if((link->serverStatus != S_CONN) && ValidPrefs(link))
	{
		startConnection(link);
		ConnectionMenuHilites();
		return 1;
	}
	return 0;
}