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

#define __CONNECTIONS__

#include "IRCGlobals.h"
#include "inline.h"
#include "TCP.h"
#include "connections.h"
#include "IRCInputPrivate.h"
#include "plugins.h"
#include "utils.h"

connectionPtr fConn;

linkPtr firstLink;

static pascal void ConnSetInputFunc(connectionPtr conn);

pascal short CountLinks(void)
{
	linkPtr lp;
	int i = 0;
	
	linkfor(lp, firstLink)
		i++;
	
	return i;
}

pascal linkPtr GetLinkNum(int i)
{
	linkPtr x;
	
	linkfor(x, firstLink)
		if(!i--)
			return x;
	
	return 0;	
}

static pascal void InitLink(linkPtr l, linkPrefsPtr lp, int linkNum)
{
	int y;
	
	l->next = 0;
	
	l->conn=0;
	l->identConn=0;
	l->connectedTime=0;
	
	l->serverStatus=S_OFFLINE;
	l->connectingNickAttempt=-2;
	l->neverConnected=1;
	l->UserRegistered=
	l->gotMOTD=
	l->finishedMOTD=
	l->alreadyFinished=0;
	l->isAway=0;
	
	for(y=0;y<10;y++)
	{
		l->lastNumerics[y]=0;
		l->windows[y]=0;
	}
	
	l->linkPrefs = lp;
	l->CurrentServer[0]=0;
	l->serverTryingName[0]=0;
	l->serverTryingPort=6667;
	l->linkNum=linkNum;
	l->firstHalfOfIncoming.len=0;
	l->notifyList = 0;
	l->numOutstandingISON=0;
	l->lastConnAttempt = 0;
	
	l->waitingToRetry = 0;
	l->connectionAttempts = -1;
	l->needUserHosts = false;
	
	l->outstandingUSERHOST = 0;
	l->umodes = l->yourUmodes = l->chModes = 0;
	l->awayString = 0;
	l->channelList = 0;
	
	l->serverOptions = 0;
	
	pstrcpy(l->linkPrefs->nick, l->CurrentNick);
}

pascal void initLinks(void)
{
	linkPtr prev = 0;
	linkPtr l;
	int x;
	
	for(x=0;x<maxLinks;x++)
	{
		l = (linkPtr)NewPtr(sizeof(Link));
		if(x==0)
			firstLink = l;
		l->prev = prev;
		if(prev)
			prev->next = l;
		prev = l;
		
		InitLink(l, &linkPrefsArray[x], x);
	}
}

pascal char LinkCanMode(linkPtr link, char mode, char umode)
{
	if(link)
	{
		if(umode && link->umodes)
		{
			int x, n = link->umodes[0];
			for(x=1;x<=n;x++)
				if(link->umodes[x] == mode)
					return 1;
		}
		else if(link->chModes)
		{
			int x, n = link->chModes[0];
			for(x=1;x<=n;x++)
				if(link->chModes[x] == mode)
					return 1;
		}
	}
	
	return 0;
}

pascal void deleteConnection(connectionPtr *c)
{
	connectionPtr c2;
	connectionPtr cc;
	
	if(!c || !*c)
		return;
	
	cc = *c;
	
	if(cc->private_socket)
	{
		//If it's a DNS connection, we don't zap it.
		if(cc->connType != connDNSIP && cc->connType != connDNSNAME)
			AbortConnection(cc->private_socket);
		cc->private_socket = 0;
	}
	
	if(cc==fConn)
		fConn=cc->next;
	else
	{
		c2=fConn;
		while(c2->next !=cc)
			c2=c2->next;
		
		c2->next=cc->next;
	}

	cc->link = 0;
	cc->next = 0;
	cc->connType = connNIL;
	cc->InputFunc = 0;
	cc->DCCInputFunc = 0;
	DisposePtr((Ptr)cc);
	*c=0;
}

pascal connectionPtr findConnectionSock(long sock)
{
	connectionPtr c;
	
	linkfor(c, fConn)
		if(c->private_socket==sock)
			return c;
	
	return 0;
}

pascal void ConnSetup(connectionPtr c, ConstStr255Param name, unsigned short port)
{
	if(c->connType == connSOCKS)
	{
		if(mainPrefs->socksHost[0] && mainPrefs->socksPort)
		{
			c->port = mainPrefs->socksPort;
			c->socksPort=port;
			pstrcpy(mainPrefs->socksHost, c->name);
			pstrcpy(name, c->socksName);
		}
		else //error message
		{
			c->port = port;
			pstrcpy(name, c->name);
		}
	}
	else
	{
		c->port = port;
		pstrcpy(name, c->name);
	}
}

static pascal void ConnSetInputFunc(connectionPtr conn)
{
	switch(conn->connType)
	{
		case connIRC:
			conn->InputFunc = processServer;
			break;
		case connIDENTD:
			conn->InputFunc = processIdentd;
			break;
		case connDCC:
			conn->InputFunc = dccEvent;
			break;
		case connDNSIP:
		case connDNSNAME:
			conn->InputFunc = dnsEvent;
			break;
		case connPLUGIN:
			conn->InputFunc = processPlugin;
			break;
		case connSOCKS:
			conn->InputFunc = processSOCKS;
			break;
		case connSTALE:
			conn->InputFunc = processStale;
			break;
		default:
			conn->InputFunc = 0;
	}
}

pascal connectionPtr newConnection(short connType)
{
	connectionPtr c =(connectionPtr)NewPtr(sizeof(Connection));

	if(c)
	{
		c->next=fConn;
		fConn=c;

		c->link=0;
		c->private_socket=0;
		c->outgoing=0;
		if(mainPrefs->firewallType && (connType < connDNSIP || connType > connDNSNAME))
		{
			c->connType=connSOCKS;
		}
		else
			c->connType=connType;
		c->ip=0;
		c->name[0]=0;
		c->port=0;
		c->tryingToConnect=0;
		c->connectStage=csNil;
		c->lastData=now;
		c->dcc=0;
		c->pluginRef=0;
		c->textOrBinary=0;
		c->lineTerm=0;
		c->dataIn = c->dataOut = 0;
		c->closeTime = 0;
		c->realConnType = 0;
		c->socksName[0]=0;
		c->socksPort=0;
		c->socksType=connType;
		c->socksMethodVersion = c->socksMethod = 0;
		c->ip2 = 0;
		
		ConnSetInputFunc(c);
	}
	
	return c;
}

pascal void newIRCConnection(linkPtr link)
{
	connectionPtr c=newConnection(connIRC);
	if(c)
	{
		c->outgoing=true;
		c->connectStage=csStartingToConnect;
		c->tryingToConnect=true;
		c->link=link;
		link->conn=c;
	}
}

pascal connectionPtr ConnNewDNSIP(ConstStr255Param name)
{
	connectionPtr c = newConnection(connDNSIP);
	
	if(c)
	{
		pstrcpy(name, c->name);
		c->outgoing = 1;
		FindAddress(&c->private_socket, c->name);
	}
	
	return c;
}

pascal connectionPtr ConnNewDNSName(ConstStr255Param ip)
{
	connectionPtr c = newConnection(connDNSNAME);
	
	if(c)
	{
		pstrcpy(ip, c->name);
		c->outgoing = 1;
		c->ip = IPStringToLong(ip);
		FindName(&c->private_socket, c->ip);
	}
	
	return c;
}

pascal char IsLinkValid(linkPtr link)
{
	linkPtr x;
	
	if(link)
		linkfor(x, firstLink)
			if(x == link)
				return 1;
	
	return 0;
}

#pragma internal on
pascal void ConnStale(connectionPtr conn)
{
	conn->realConnType = conn->connType;
	conn->connType = connSTALE;
	ConnSetInputFunc(conn);
}

pascal void ConnDeSOCKS(connectionPtr conn)
{
	conn->connType = conn->socksType;
	ConnSetInputFunc(conn);
}
#pragma internal reset
#pragma mark -

pascal connectionPtr pluginNewConnection(char textOrBinary)
{
	connectionPtr c=newConnection(connPLUGIN);
	c->pluginRef=sidr.yourInfo;
	c->textOrBinary=textOrBinary;
	return c;
}

pascal void ConnAbort(connectionPtr conn)
{
	if(conn->private_socket)
	{
		AbortConnection(conn->private_socket);
		conn->private_socket=0;
	}
}

pascal void ConnClose(connectionPtr conn)
{
	if(conn->private_socket)
		CloseTCPConnection(conn->private_socket);
}

pascal char ConnNewActive(connectionPtr c)
{
	return NewActiveConnection(&c->private_socket, 8192, 0, c->ip, c->port)==0;
}

pascal char ConnNewPassive(connectionPtr c)
{
	return NewPassiveConnection(&c->private_socket, 8192, c->port, c->ip, 0)==0;
}

pascal OSErr ConnGetData(connectionPtr conn, Ptr d, short len)
{
	return TCPReceiveChars(GetConnectionTCPC(conn->private_socket), d, len);
}

pascal long ConnCharsAvail(connectionPtr conn)
{
	return TCPCharsAvailable(GetConnectionTCPC(conn->private_socket));
}

pascal unsigned short ConnGetLocalPort(connectionPtr conn)
{
	return TCPLocalPort(GetConnectionTCPC(conn->private_socket));
}

pascal OSErr ConnSend(connectionPtr conn, const void* writePtr, short writeCount, char push)
{
	return TCPSendAsync(GetConnectionTCPC(conn->private_socket), writePtr, writeCount, push);
}

pascal OSErr ConnFindAddress(connectionPtr conn, ConstStr255Param host)
{
	return FindAddress(&conn->private_socket, host);
}