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

#include "IRCGlobals.h"
#include "utils.h"
#include "MsgBuffer.h"
#include "inputline.h"
#include "MsgWindows.h"
#include "channels.h"
#include "connections.h"
#include "IRCChannels.h"
#include "TextManip.h"
#include "plugins.h"
#include "IRCAux.h"
#include "IRCCommands.h"
#include "Inline.h"
#include "ApplBase.h"
#include "IRCNotify.h"
#include "StringList.h"
#include "Floaters.h"

#pragma internal on
inline void checkConnections(void);
void main(void);

pascal void processStale(void* c, connectionPtr conn);

inline void checkConnections(void)
{
	connectionPtr conn, conn2;
	LongString ls;
	
	conn=fConn;
	while(conn)
	{
		conn2=conn->next;
		if(conn->connType == connSTALE && now >= conn->closeTime)
			processStale(0, conn);
		else if(conn->connType == connIRC && !conn->tryingToConnect)
		{
			//Test and kill stale irc connections
			if(conn->link && conn->link->conn != conn) //stale connection!
			{
				LSConcatStrAndStr("\pStale connection deleted for link \"", conn->link->linkPrefs->linkName, &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				deleteConnection(&conn);
			}
			else if(conn->lastData<(now-300))
			{
				LSConcatStrAndStr("\pPING ", conn->link->CurrentNick, &ls);
				conn->lastData+=60;
				ConnPutLS(&conn, &ls);
			}
		}
		conn=conn2;
	}
}

static void ProcessUserHostsOutgoing(linkPtr link, LongString *ts)
{
	LSConcatStrAndLS("\pUSERHOST", ts, ts);
	SendCommand(link, ts);
	link->outstandingUSERHOST++;
	ts->len=0;
}

#define processUHOutgoing {if((++i)==5) { i=0;\
ProcessUserHostsOutgoing(link, &ts);\
if((++ii) == 5) return;}}

inline void ProcessUserHosts(linkPtr link)
{
	LongString ts;
	int i, ii;
	UserListPtr u;
	channelPtr ch;
	
	ts.len=0;
	i = ii = 0;
	if(link->needUserHosts)
	{
		linkfor(ch, link->channelList)
			linkfor(u, ch->userlist)
				if(!u->userhost[0] && (u->lastUHUpdate!=-1))
				{
					u->lastUHUpdate=-1;
					LSAppend1(ts, ' ');
					LSConcatLSAndStr(&ts, u->nick, &ts);
					processUHOutgoing;
				}
		link->needUserHosts=0;
	}
	else
	{
		//Ok, we're here. That means we've gotten ALL the userhost information from people who
		//previously existed. That means we have to scan through, find userhosts updated more than five minutes ago,
		//and update.
		
		linkfor(ch, link->channelList)
			linkfor(u, ch->userlist)
				if((!u->userhost[0] || (now-u->lastUHUpdate>300)) && (u->lastUHUpdate!=-1)) //5 minute update time
				{
					u->lastUHUpdate=-1;
					LSAppend1(ts, ' ');
					LSConcatLSAndStr(&ts, u->nick, &ts);
					processUHOutgoing;
				}
	}
	
	if(i>0)
		ProcessUserHostsOutgoing(link, &ts);
}

pascal void initAppl(void);
pascal void IRCInitAll(void);

void main(void)
{
	EventRecord e;
	linkPtr curLink;
	unsigned long userhostsTimer, minuteTimer, periodicTasksTimer;

	EventAvail(everyEvent, &e);
	if(e.modifiers & controlKey)
		return;
	
	GetDateTime(&now);
	initAppl();
	IRCInitAll();

	makePlugsDB();
	UseResFile(gApplResFork);
	
	CurrentTarget.link=firstLink;
	if(allowConnections && !(e.modifiers & shiftKey))
		linkfor(curLink, firstLink)
			if(curLink->linkPrefs->autoConnect)
				OpenConnection(curLink);
	
	userhostsTimer = minuteTimer = periodicTasksTimer = now;
	InitCursor();
	DrawMenuBar();
	
	while(!QuitRequest)
	{
		GetDateTime(&now);
		
		linkfor(curLink, firstLink)
			if(curLink->serverStatus==S_OFFLINE && !curLink->neverConnected)
			{
				if(curLink->waitingToRetry)
				{
					if(now >= curLink->lastConnAttempt + curLink->linkPrefs->retryDelay)
					{
						OpenConnection(curLink);
						curLink->connectionAttempts++;
						if(curLink->linkPrefs->maxConnAtt && curLink->connectionAttempts >= curLink->linkPrefs->maxConnAtt)
							curLink->neverConnected = true; //to prevent reconnects
					}

				}
				else if(curLink->linkPrefs->reconnect)
					curLink->waitingToRetry = true;
			}
		
		//Periodic tasks
		if(now-periodicTasksTimer>5)
		{
			periodicTasksTimer=now;
			checkConnections();
			
			if(now-userhostsTimer>20)
			{
				userhostsTimer = now;
				linkfor(curLink, firstLink)
					if(curLink->serverStatus==S_CONN)
						ProcessUserHosts(curLink);

				if(now-minuteTimer>60)
				{
					minuteTimer = now;
					MBIdle();
					RunNotify();
				}
			}
			
			UpdateStatusLine();
		}
		
		ApplRun();
	}
	ApplExit();
}