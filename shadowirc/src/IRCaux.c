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

#include "LongStrings.h"
#include "StringList.h"
#include "AppearanceHelp.h"
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

static pascal void startConnection(linkPtr link);

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
			a = kServerOKBadSOCKS;
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
	
	conn->dataOut += len;
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

pascal void MakeChannel(StringPtr s)
{
	if(!IsChannel(s))
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

pascal void LinkSuccessfulConnection(linkPtr link, char reg)
{
	pServiceCWLinkStateChangeData p;
	
	p.link = link;
	link->serverStatus = S_CONN;
	p.connectStage = link->conn->connectStage=csConnected;
	runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
	UpdateStatusLine();
	if(reg)
		RegUser(link);
}

pascal void LinkFailedConnection(linkPtr link)
{
	pServiceCWLinkStateChangeData p;
	
	p.link = link;
	p.connectStage = link->conn->connectStage=csFailedToConnect;
	runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
	ServerOK(C_FailedToOpen, link);
}

static pascal void CreateIdentdConn(connectionPtr conn)
{
	if(mainPrefs->firewallType != fwSOCKS5) //Why?
	{
		LongString ls;
		connectionPtr identConn;
		linkPtr link = conn->link;
		
		link->identConn = identConn = newConnection(connIDENTD);
		if(identConn)
		{
			LSGetIntString(&ls, spError, sOpeningIdentd);
			SMPrefixLink(link, &ls, 1);
			
			identConn->ip = conn->ip;
			identConn->port = 113;
			identConn->link=link;
			if(mainPrefs->firewallType == fwSOCKS4A || mainPrefs->firewallType == fwSOCKS4)
				identConn->refCon = NewPString(mainPrefs->socksUser);
			else
				identConn->refCon = NewPString(conn->link->linkPrefs->user);
			
			//Allow for a 10 connection backlog. This is more than enough.
			ConnNewListen(identConn, 10);
		}
	}
}

//We found the IP for the server we're trying to connect to.
//connect to the server, and then open the identd connection
pascal void connection2(connectionPtr conn)
{
	if(conn->ip.s_addr != -1)
	{
		LongString ls;
		Str255 s;
		pServiceCWLinkStateChangeData p;
		linkPtr link = conn->link;
	
		inet_ntoa_str(conn->ip, s);
		LSParamString(&ls, GetIntStringPtr(spInfo, sIPForIs), conn->name, s, 0, 0);
		SMPrefix(&ls, dsConsole);
		if(conn->socksType == connIRC)
		{
			p.link = link;
			link->serverStatus=S_OPENING;
			UpdateStatusLine();
		}

		if(ConnNewActive(conn)) //this makes the connection to the (socks) server
		{
			CreateIdentdConn(conn);
			
			conn->connectStage=csOpeningConnection;
			if(conn->socksType == connIRC)
			{
				p.connectStage = conn->connectStage;
				runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
			}
		}
		else
		{
			conn->connectStage = csFailedToConnect;
			if(conn->socksType == connIRC)
			{
				p.connectStage = conn->connectStage;
				runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
				ServerOK(C_FailedToOpen, link);
			}
		}
	}
}

inline long IPStrtoHost(ConstStr255Param s);
inline long IPStrtoHost(ConstStr255Param s)
{
	unsigned char ip[20];
	short p;
	long l, l2;
	char *out = (char*)&l;
	int n;
	
	if(s[0] > 15) //if not real IP string, die.
		return -1;
	
	pstrcpy(s, ip);
	for(n = 3;n>=0;n--)
	{
		p=revPos('.', ip);
		if(p)
		{
			ip[p] = ip[0] - p;
			ip[0] = p-1;
		}
		
		StringToNum(&ip[p], &l2);
	
		if(l2 > 255)
			return -1;
		out[n] = l2;
	}
	
	return l;
}

pascal void startConnection(linkPtr link)
{
	linkPrefsPtr lp=link->linkPrefs;
	LongString ls;
	pServiceCWLinkStateChangeData p;
	StringPtr search;
	
//identd
	if(link->identConn)
		deleteConnection(&link->identConn);

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
		search = link->conn->socksName;
	else
		search = link->conn->name;
	
	p.link = link;
	if(!ConnFindAddress(link->conn, search))
	{
		if(mainPrefs->firewallType == fwSOCKS4)
			link->conn->connectStage=csLookingUp2;
		else
			link->conn->connectStage=csLookingUp;

		p.connectStage = link->conn->connectStage;
		runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
		LSParamString(&ls, GetIntStringPtr(spInfo, sLookingUpIP), search, 0, 0, 0);
		SMPrefix(&ls, dsConsole);
	}
	else
	{
		p.connectStage = link->conn->connectStage = csFailedToLookup;
		runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
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