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

#include <Carbon/Carbon.h>
#include "WASTE.h"

#include "AppearanceHelp.h"

#include "StringList.h"
#include "Inline.h"
#include "IRCGlobals.h"
#include "TCP.h"
#include "connections.h"
#include "LongStrings.h"
#include "MsgWindows.h"
#include "filesMan.h"
#include "InputLine.h"
#include "channels.h"
#include "plugins.h"

#include "DragDrop.h"
#include "Floaters.h"
#include "IRCCommands.h"
#include "IRCChannels.h"
#include "IRCInput.h"
#include "IRCInputPrivate.h"
#include "IRCNotify.h"
#include "IRCAux.h"
#include "TextManip.h"
#include "Notify.h"
#include "MsgBuffer.h"
#include "CMenus.h"
#include "InetConfig.h"
#include "MenuCommands.h"
#include "Events.h"
#include "utils.h"
#include "ApplBase.h"

static void doTCPEvent(CEPtr message);
static void ApplExit(void);

static void AsyncSoundCleanup(EventLoopTimerRef timer, void* data);
static void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord);

inline void checkConnections(void);
inline void ProcessUserHosts(linkPtr link);

typedef struct aspData {
	long refcon;
	SndListHandle sound;
	SndChannelPtr soundChannel;
	
	struct aspData *next, *prev;
	plugsPtr pluginRef;

	char isFinished;
	char disposeOfChannel;
} pAsyncSoundCompletionData, *pAsyncSoundCompletionDataPtr, aspData, *aspDataPtr;

static aspDataPtr aspList=0;
static SndCallBackUPP scb;
static EventLoopTimerRef AsyncSoundTimer;

static void AsyncSoundCleanup(EventLoopTimerRef timer, void* data)
{
#pragma unused(data)
	aspDataPtr asp = aspList;
	aspDataPtr next;
	
	while(asp)
	{
		next= asp->next;
		
		if(asp->isFinished)
		{
			if(asp->pluginRef) //call a plugin and complain
				runIndPlugin(asp->pluginRef, pAsyncSoundCompletionMessage, asp);
			
			if(asp->disposeOfChannel)
				SndDisposeChannel(asp->soundChannel, false);
			
			if(asp->next)
				asp->next->prev=asp->prev;
			if(asp->prev)
				asp->prev->next=next;
			
			if(asp==aspList)
				aspList=next;
			DisposePtr((Ptr)asp);
		}
		asp=next;
	}
	
	SetEventLoopTimerNextFireTime(timer, kEventDurationForever);
}

static void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord)
{
	#pragma unused(theSoundChannel)
	
	aspDataPtr asp = (aspDataPtr)infoRecord->param2;
	asp->isFinished=1;
	
	SetEventLoopTimerNextFireTime(AsyncSoundTimer, kEventDurationNoWait);
}

OSErr AsyncSoundPlay(Handle sound, long refcon, Ptr *channel)
{
	SndChannelPtr soundChannel=0;
	OSErr err = noErr;
	char disposeOfChannel = !(channel && *channel);
	
	if(sound)
	{
		if(channel)
			soundChannel=(SndChannelPtr)*channel;
		if(!soundChannel)
		{
			err=SndNewChannel(&soundChannel, 0, 0, scb);
			if(channel)
				*channel=(Ptr)soundChannel;
		}
		else
			soundChannel=(SndChannelPtr)*channel;

		if(err || !soundChannel)
			return err;
		
		err=SndPlay(soundChannel, (SndListHandle)sound, true);
		if(err)
		{
			SndDisposeChannel(soundChannel, false);
			return err;
		}
		else
		{
			SndCommand soundCommand;
			aspDataPtr asp = (aspDataPtr)NewPtr(sizeof(aspData));
			
			asp->prev=0;
			asp->next=aspList;
			if(asp->next)
				asp->next->prev=asp;
			aspList=asp;
			asp->refcon=refcon;
			asp->sound=(SndListHandle)sound;
			asp->soundChannel=(SndChannelPtr)soundChannel;
			asp->isFinished=0;
			asp->disposeOfChannel = disposeOfChannel;
			
			asp->pluginRef=sidr.yourInfo;
			
			soundCommand.cmd = callBackCmd;
			soundCommand.param2 = (long)asp;
			SndDoCommand(soundChannel, &soundCommand, false);
		}
	}
	
	if(channel)
		*channel=(Ptr)soundChannel;
	return err;
}

#pragma mark -

#define kNibWinConfirmQuit CFSTR("SavePrefs")
#define kNibMain CFSTR("main")

typedef struct sqData {
	WindowRef win;
	int save;
} sqData;

static OSStatus ConfirmQuitEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
#pragma unused(myHandler)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	WindowRef aboutWindow;
	sqData *data = (sqData*)userData;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	aboutWindow = (WindowRef)userData;
	
	switch(eventClass)
	{
		case kEventClassControl:
		{
			ControlRef theControl = NULL;
			UInt32 cmd;
			
			GetEventParameter(event, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &theControl);
			GetControlCommandID(theControl, &cmd);
			
			switch(cmd)
			{
				case kHICommandSave:
				case kHICommandOK:
				case kHICommandCancel:
					if(cmd == kHICommandSave)
						data->save = 1;
					else if(cmd == kHICommandOK)
						data->save = 0;
					
					QuitAppModalLoopForWindow(data->win);
					result = noErr;
					break;
			}
			break;
		}
	}
	
	return result;
}

char doQuit(LongString *reason)
{
	linkPtr link;
	sqData sq;
	
	if(mainPrefs->quitAction == qaAutoSave)
		sq.save = 1;
	else
	{
		sq.save = -1; //update from dialog
		if(mainPrefs->quitAction == qaConfirm)
		{
			static EventHandlerUPP swUPP = NULL;
			WindowRef cqWin;
			IBNibRef mainNibRef;
			OSStatus status;
			
			const EventTypeSpec ctSpec[] = {
				{ kEventClassControl, kEventControlHit }
			};
			
			if(!swUPP)
				swUPP = NewEventHandlerUPP(ConfirmQuitEventHandler);
			
			status = CreateNibReference(kNibMain, &mainNibRef);
			require_noerr(status, CantFindDialogNib);
			
			status = CreateWindowFromNib(mainNibRef, kNibWinConfirmQuit, &cqWin);
			require_noerr(status, CantCreateDialogWindow);
			
			DisposeNibReference(mainNibRef);
			
			status = InstallWindowEventHandler(cqWin, swUPP, GetEventTypeCount(ctSpec), ctSpec, (void*)&sq, NULL);
			require_noerr(status, CantInstallDialogHandler);
			
			ShowWindow(cqWin);
			
			sq.win = cqWin;
			status = RunAppModalLoopForWindow(cqWin);
			
			DisposeWindow(cqWin);
			
CantFindDialogNib:
CantCreateDialogWindow:
CantInstallDialogHandler:
			if(sq.save == -1)
				return 0;
		}
	}
		
	if(sq.save)
		writeAllFiles();
	
	linkfor(link, firstLink)
		if(link->serverStatus==S_CONN)
			DoSignoff(link, reason);
	
	QuitApplicationEventLoop();
	
	return 1;
}

#pragma mark -

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
		else if(conn->connType == connIRC)
		{
			if(conn->link && conn->link->connectStage == csOnline)
			{
				//Test and kill stale irc connections
				if(conn->link->conn != conn) //stale connection!
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

#define AddUserHostRequest \
{ \
	u->lastUHUpdate=-1; \
	LSAppend1(ts, ' '); \
	LSConcatLSAndStr(&ts, u->nick, &ts); \
	processUHOutgoing; \
}

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
					AddUserHostRequest;

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
					AddUserHostRequest;
	}
	
	if(i>0)
		ProcessUserHostsOutgoing(link, &ts);
}

static void RetryConnections(void)
{
	linkPtr curLink;
	
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
}

static void MinuteTimer(EventLoopTimerRef timer, void* data)
{
#pragma unused(timer, data)
	MBIdle();
	RunNotify();
}

static void Timer20(EventLoopTimerRef timer, void* data)
{
#pragma unused(timer, data)
	linkPtr curLink;
	
	linkfor(curLink, firstLink)
		if(curLink->serverStatus==S_CONN)
			ProcessUserHosts(curLink);
}

static void Timer5(EventLoopTimerRef timer, void* data)
{
#pragma unused(timer, data)
	checkConnections();
	UpdateStatusLine();
}

static void Timer1(EventLoopTimerRef timer, void* data)
{
#pragma unused(timer, data)
	GetDateTime(&now);
}

static void TimerTick(EventLoopTimerRef timer, void* data)
{
#pragma unused(timer, data)
	RetryConnections();
}

EventLoopTimerRef NetworkTimer;

static void NetworkProcess(EventLoopTimerRef timer, void* data)
{
#pragma unused(timer, data)
	connectionEventRecord connEvt;
	int x = -8;
	
	connEvt.sas = safe_malloc(sizeof(struct sockaddr_storage));
	
	while(GetConnectionEvent(&connEvt))
	{
		doTCPEvent(&connEvt);
		if(!++x)
			break;
	}
	
	free(connEvt.sas);
}

static void InitTimers()
{
	EventLoopRef mainLoop = GetMainEventLoop();
	EventLoopTimerUPP mtu, t20u, t5u, t1u, AsyncSoundTimerProc, tick, net;
	EventLoopTimerRef timer;
	
	mtu = NewEventLoopTimerUPP(MinuteTimer);
	t20u = NewEventLoopTimerUPP(Timer20);
	t5u = NewEventLoopTimerUPP(Timer5);
	t1u = NewEventLoopTimerUPP(Timer1);
	tick = NewEventLoopTimerUPP(TimerTick);
	
	InstallEventLoopTimer(mainLoop, 60 * kEventDurationSecond, 60 * kEventDurationSecond, mtu, NULL, &timer);
	InstallEventLoopTimer(mainLoop, 20 * kEventDurationSecond, 20 * kEventDurationSecond, t20u, NULL, &timer);
	InstallEventLoopTimer(mainLoop, 5 * kEventDurationSecond, 5 * kEventDurationSecond, t5u, NULL, &timer);
	InstallEventLoopTimer(mainLoop, 1 * kEventDurationSecond, 1 * kEventDurationSecond, t1u, NULL, &timer);
	InstallEventLoopTimer(mainLoop, TicksToEventTime(1), TicksToEventTime(1), tick, NULL, &timer);
	
	AsyncSoundTimerProc = NewEventLoopTimerUPP(AsyncSoundCleanup);
	InstallEventLoopTimer(mainLoop, kEventDurationForever, kEventDurationForever, AsyncSoundTimerProc, NULL, &AsyncSoundTimer);

	net = NewEventLoopTimerUPP(NetworkProcess);
	InstallEventLoopTimer(mainLoop, TicksToEventTime(1), TicksToEventTime(1), net, NULL, &NetworkTimer);
}

#pragma mark -

static OSStatus EventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *data)
{
#pragma unused(handlerCallRef, data)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassWindow:
		{
			WindowRef win;
			
			GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL, sizeof(WindowRef), NULL, &win);
			switch(eventKind)
			{
				case kEventWindowActivated:
				case kEventWindowDeactivated:
				{
					pServiceActivateWinData ps;
					MWPtr mw = MWFromWindow(win);
					char activate = eventKind == kEventWindowActivated;
					
					if(!WIsFloater(win))
					{
						if(activate)
						{
							if(!mw) //if it's not a message window, then it's an invalid target
							{
								DisableMenuCommand(gEditMenu, 'FIND');
								DisableMenuCommand(gEditMenu, 'FAGN');
						
								InvalTarget(&CurrentTarget);
								UpdateStatusLine();
								DrawMWinStatus(consoleWin);
							}
						}
						
						ps.activate = activate;
						ps.w = win;
						ps.mw = mw;
						
						runService(pServiceActivateWin, &ps);
					}
					
					result = noErr;
					break;
				}
				
				case kEventWindowHandleContentClick:
				{
					iwFront = 0; //inputline doesn't fall through.
					
					result = noErr;
					break;
				}
			}
			break;
		}
	}
	
	return result;
}

static OSStatus DoModifierKeysChangedEvent(EventHandlerCallRef handlerCallRef, EventRef event, void *data)
{
#pragma unused(handlerCallRef, data)
	UInt32 modifiers;
	OSStatus result = eventNotHandledErr;
	static UInt32 oldModifiers = 0;
	
	result = GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
	
	if(result == noErr)
	{
		//Test for changes with the control key.
		if((modifiers & controlKey) != (oldModifiers & controlKey))
			CMSetCursor((modifiers & controlKey) == controlKey);
		
		oldModifiers = modifiers;
	}
	
	return result;
}

static OSStatus DoResumeEvent(EventHandlerCallRef handlerCallRef, EventRef event, void *data)
{
#pragma unused(handlerCallRef, event, data)
	pContextSwitchDataRec p;
	
	inBackground=0;
	InitCursor();
	
	if(!noFloatingInput)
		WEActivate(ILGetWEFromMW(0));
	
	NotifyRemove(); //Clear notification manager requests.

	p.inBackground=inBackground;
	runPlugins(pContextSwitchMessage, &p);
	
	return noErr;
}

static OSStatus DoSuspendEvent(EventHandlerCallRef handlerCallRef, EventRef event, void *data)
{
#pragma unused(handlerCallRef, event, data)
	pContextSwitchDataRec p;
	
	inBackground=1;
	
	if(!noFloatingInput)
		WEDeactivate(ILGetWEFromMW(0));
	
	p.inBackground=inBackground;
	runPlugins(pContextSwitchMessage, &p);
	
	return noErr;
}

static void doTCPEvent(CEPtr c)
{
	connectionPtr conn;
	
	conn=findConnectionSock(c->connection);
	if(!conn) //dunno how we're supposed to process this...
		return;

	switch(c->event)
	{
		case C_Found:
			memcpy(conn->sas, c->sas, sizeof(struct sockaddr_storage));
			DisplayLookupResult(conn);
                        /*
                         * The socket connect() can fail, even when non-blocking
                         * If we do not immediately return when this occurs
                         * then we'll jump off to 0x0 when running conn->InputFunc
                         */
			if(!conn->socks.secondLookup)
				if(connection2(conn) != 0)
                                    return;
			break;
		
		case C_Established:
			memcpy(conn->sas, c->sas, sizeof(struct sockaddr_storage));
			break;
	}
	
	conn->InputFunc(c, conn);
}

static void RunStartupConnections(void)
{
	linkPtr curLink;

	linkfor(curLink, firstLink)
		if(curLink->linkPrefs->autoConnect)
			OpenConnection(curLink);
}

void ApplRun(void)
{
	RunStartupConnections();
	RunApplicationEventLoop();
	ApplExit();
}

static void MyIAEH(long class, long type, EventHandlerProcPtr handlerFunc)
{
	EventTypeSpec eventType = {class, type};
	
	InstallApplicationEventHandler(NewEventHandlerUPP(handlerFunc), 1, &eventType, NULL, NULL);
}

static void InitLocalEventHandlers()
{
	EventTypeSpec events[] = 
	{
		{kEventClassWindow, kEventWindowActivated},
		{kEventClassWindow, kEventWindowDeactivated},
		{kEventClassWindow, kEventWindowHandleContentClick},
	};
	
	MyIAEH(kEventClassApplication, kEventAppActivated, DoResumeEvent);
	MyIAEH(kEventClassApplication, kEventAppDeactivated, DoSuspendEvent);
	MyIAEH(kEventClassKeyboard, kEventRawKeyModifiersChanged, DoModifierKeysChangedEvent);
	
	InstallApplicationEventHandler(NewEventHandlerUPP(EventHandler), GetEventTypeCount(events), events, NULL, NULL);
}

void ApplInit(void)
{
	InitEventHandlers();
	InitLocalEventHandlers();

	FlushEvents(everyEvent, 0);
	
	scb=NewSndCallBackUPP(AsyncSoundCallback);
	
	InitTimers();
}

static void ApplExit(void)
{
	runAllPlugins(pQuitMessage, 0);
	
	ConnStopNetworking();
	
	StopIC();
	
	UnregisterAppearanceClient();
	
	ExitToShell();
}
