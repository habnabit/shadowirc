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

static void ConnSetInputFunc(connectionPtr conn);

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

static void InitLink(linkPtr l, linkPrefsPtr lp, int linkNum)
{
	int y;
	
	l->next = 0;
	
	l->conn=0;
	l->identConn = 0;
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
	l->connectStage=csNil;
	
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
			ConnClose(cc);
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
	DisposePtr((Ptr)cc->sas);
	DisposePtr((Ptr)cc->localsas);
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
			c->socks.port=port;
			pstrcpy(mainPrefs->socksHost, c->name);
			pstrcpy(name, c->socks.name);
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

static void ConnSetInputFunc(connectionPtr conn)
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
		case connDNSNAME:
		case connDNSIP:
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
		if(mainPrefs->firewallType && (connType < connDNSIP || connType > connDNSNAME))
		{
			c->connType=connSOCKS;
		}
		else
			c->connType=connType;
		c->sas = (struct sockaddr_storage *)NewPtrClear(sizeof(struct sockaddr_storage));
		c->localsas = (struct sockaddr_storage *)NewPtrClear(sizeof(struct sockaddr_storage));
		c->name[0]=0;
		c->port=0;
		c->lastData=now;
		c->dcc=0;
		c->pluginRef=0;
		c->textOrBinary=0;
		c->lineTerm=0;
		c->closeTime = 0;
		c->realConnType = 0;
		
		/*
		 * SOCKS firewalling does not yet support IPv6
		 */
		c->socks.name[0]=0;
		c->socks.port=0;
		c->socks.type=connType;
		c->socks.methodVersion = c->socks.method = c->socks.stage = 0;
		c->socks.secondLookup = 0;
		c->socks.ip.s_addr = 0;
		
		ConnSetInputFunc(c);
	}
	
	return c;
}

pascal void newIRCConnection(linkPtr link)
{
	connectionPtr c=newConnection(connIRC);
	if(c)
	{
		link->connectStage=csStartingToConnect;
		c->link=link;
		link->conn=c;
	}
}

pascal connectionPtr ConnNewDNS(ConstStr255Param name, short type)
{
	connectionPtr c = newConnection(type);
	
	if(c)
	{
		pstrcpy(name, c->name);
		FindAddress(&c->private_socket, c->name);
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



char ConnStartNetworking(void)
{
	static char inited = 0;
	
	if(!inited)
	{
		InitConnections();
		inited = 1;
	}
	
	return true;
}

static void ConnFinishStaleConnections(void)
{
	connectionPtr conn, conn2;
	int x=0, y=0, z=0;
	
	//Trash all non-stale connections
	conn  = fConn;
	while(conn)
	{
		x++;
		conn2 = conn->next;
		if(conn->connType < connSTALE)
			deleteConnection(&conn);
		else
			y++;
		conn = conn2;
	}
	
	//Find irc connetions ith no channel windows
	conn = fConn;
	while(conn)
	{
		conn2 = conn->next;
		if(conn->realConnType == connIRC)
		{
			linkPtr link = conn->link;
			
			if(!link->channelList) //no cative channels, kill the link now
			{
				deleteConnection(&conn);
				z++;
			}
		}

		conn=conn2; 
	}
	
	if(fConn) //Now, if there are still stale connections left, let them time out.
	{
		long time = now;
		
		while(fConn) //while there's still a (stale) connection. Spin for _at most_ two seconds, and then ditch.
		{
			connectionEventRecord connEvt;
			
			connEvt.sas = safe_malloc(sizeof(struct sockaddr_storage));
			
			if(GetConnectionEvent(&connEvt))
			{
				conn=findConnectionSock(connEvt.connection);
				if(conn)
				{
					if(conn->connType == connSTALE)
						processStale(&connEvt, conn);
					else
						deleteConnection(&conn);
				}
			}
			
			GetDateTime(&now);
			conn  = fConn;
			while(conn)
			{
				conn2 = conn->next;
				if(conn->closeTime < now)
					deleteConnection(&conn);
				conn = conn2;
			}
			
			if(now - time > 2) //too long. ditch.
				break;
			
			free(connEvt.sas);
		}
	}
}

void ConnStopNetworking(void)
{
	ConnFinishStaleConnections();
	FinishEverything();
}

pascal void ConnStale(connectionPtr conn)
{
	conn->realConnType = conn->connType;
	conn->connType = connSTALE;
	conn->closeTime = now + 15;
	
	ConnSetInputFunc(conn);
}

pascal void ConnDeSOCKS(connectionPtr conn)
{
	conn->connType = conn->socks.type;
	ConnSetInputFunc(conn);
}

#pragma mark -

pascal connectionPtr pluginNewConnection(char textOrBinary)
{
	connectionPtr c=newConnection(connPLUGIN);
	c->pluginRef=sidr.yourInfo;
	c->textOrBinary=textOrBinary;
	return c;
}

pascal void ConnClose(connectionPtr conn)
{
	if(conn->private_socket)
		CloseTCPConnection(conn->private_socket);
}

char ConnNewActive(connectionPtr c)
{
	return NewActiveConnection(&c->private_socket, (struct sockaddr *)c->sas, c->port)==0;
}

char ConnNewListen(connectionPtr c, int af, int backlog)
{
	return NewListenConnection(&c->private_socket, af, c->port, backlog)==0;
}

size_t ConnGetData(connectionPtr conn, Ptr d, size_t len)
{
	return TCPReceiveChars(GetConnectionSocket(conn->private_socket), d, len);
}


size_t ConnGetUntil(connectionPtr conn, Ptr d, char c, size_t len)
{
        return TCPReceiveUntil(GetConnectionSocket(conn->private_socket), d, c, len);
}

int ConnGetLocalPort(connectionPtr conn)
{
	return TCPLocalPort(GetConnectionSocket(conn->private_socket));
}

pascal OSErr ConnSend(connectionPtr conn, const void* d, size_t len)
{
	return TCPSendChars(GetConnectionSocket(conn->private_socket), d, len);
}

pascal OSErr ConnFindAddress(connectionPtr conn, ConstStr255Param host)
{
	return FindAddress(&conn->private_socket, host);
}

OSErr ConnGetLocalIP(connectionPtr conn, struct sockaddr *sa)
{
	return TCPLocalIP(GetConnectionSocket(conn->private_socket), sa);
}
