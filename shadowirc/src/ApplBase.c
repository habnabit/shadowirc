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
#include "MWPanes.h"
#include "filesMan.h"
#include "utils.h"
#include "InputLine.h"
#include "channels.h"
#include "plugins.h"

#include "DragDrop.h"
#include "Floaters.h"
#include "DCC.h"
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
#include "Shortcuts.h"
#include "Events.h"
#include "ApplBase.h"

static pascal void WindowActivate(WindowPtr window, char activate);
static pascal void floatingWindowClick(EventRecord *e);
static pascal void inZoomInOutHandler(const EventRecord *e, short part);
static pascal void inDragHandler(EventRecord *e);
static pascal void inContentHandler(EventRecord *e);
inline void inGoAwayHandler(const EventRecord *e);
static pascal void inGrowHandler(const EventRecord *e);
static pascal void doMouseUp(EventRecord *e);
static pascal void doMouseDown(EventRecord *e);
static void doTCPEvent(CEPtr message);
static pascal void ApplEvents(EventRecord *e);
static pascal void ApplExit(void);

static pascal void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord);

static RgnHandle mouseRgn;

static plugsPtr mouseDownPluginRef=0;

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

static pascal void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord)
{
	#pragma unused(theSoundChannel)
	
	aspDataPtr asp = (aspDataPtr)infoRecord->param2;
	asp->isFinished=1;
	
	SetEventLoopTimerNextFireTime(AsyncSoundTimer, kEventDurationNoWait);
}

pascal OSErr AsyncSoundPlay(Handle sound, long refcon, Ptr *channel)
{
	SndChannelPtr soundChannel=0;
	OSErr err;
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

static pascal OSStatus ConfirmQuitEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
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

pascal char doQuit(LongString *reason)
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
	
	QuitRequest = 1;
	
	return QuitRequest;
}

static pascal void WindowActivate(WindowPtr window, char activate)
{
	MWPtr p;
	pUIActivateData pl;
	channelPtr ch;
	pServiceActivateWinData ps;
	
	if(window==inputLine.w)
		return;

	p=MWFromWindow(window);
	if(p)
	{
		MWPaneActivate(p, activate);
		
		ch = MWGetChannel(p);
		
		EnableMenuCommand(gEditMenu, 'FIND');
		EnableMenuCommand(gEditMenu, 'FAGN');
	}
	else
	{
		pluginDlgInfoPtr l;
		
		ch=0;

		DisableMenuCommand(gEditMenu, 'FIND');
		DisableMenuCommand(gEditMenu, 'FAGN');
		
		l= (pluginDlgInfoPtr)GetWRefCon(window);
		if(l && l->magic==PLUGIN_MAGIC)
		{
			pl.window=window;
			pl.activate=activate;
			runIndPlugin(l->pluginRef, pUIActivateMessage, &pl);
		}
	}
	
	if(activate) //activatechannels
	{
		if(p && IsWindowActive(p->w))
			SetTarget(p, &CurrentTarget);
		else
			InvalTarget(&CurrentTarget);

		UpdateStatusLine();
		DrawMWinStatus(consoleWin);
	}
	
	ps.ch = ch;
	ps.activate=activate;
	ps.w = window;
	ps.mw = p;
	runService(pServiceActivateWin, &ps);
}

pascal void UpdateWindowPosition(WindowPtr win)
{
	MWPtr mw;
	Rect r;
	pUIWindowMoveDataRec p;
	
	if(win==inputLine.w)
	{
		WGetBBox(win, &r);
		if(r.bottom-r.top>32)
			mainPrefs->inputLoc = r;
	}
	else
	{
		mw=MWFromWindow(win);
		if(mw)
			MWReposition(mw);
		else
		{
			pluginDlgInfoPtr pl = (pluginDlgInfoPtr)GetWRefCon(win);
			if(pl && (pl->magic==PLUGIN_MAGIC))
			{
				p.w=win;
				WGetBBox(win, &p.newpos);
				runIndPlugin(pl->pluginRef, pUIWindowMoveMessage, &p);
			}
		}
	}
}

static pascal void floatingWindowClick(EventRecord *e) //this also takes care of handling the float clicks
{
	GrafPtr p;
	short i;
	WindowPtr wp;
	pluginDlgInfoPtr pd;
	
	i=FindWindow(e->where, &wp);
	if(wp==inputLine.w)
	{
		if(i==inContent)
		{
			GetPort(&p);
			SetPortWindowPort(wp);
			GlobalToLocal(&e->where);
			if(e->where.v>inputLine.statusLineHeight)
				WEClick(e->where, e->modifiers, e->when, ILGetWE());
			else
				StatusLineClick(e->where, e->modifiers, e->when);
			SetPort(p);
		}
		else if(i==	inGrow)
		{
			e->message=(long)wp;
			inGrowHandler(e);
		}
		else if(i==inDrag)
		{
			e->message=(long)wp;
			inDragHandler(e);
		}
		
		iwFront=1;
		return;
	}
	else //it's another floater...
	{
		if(WIsFloater(wp))
		{
			pd=(pluginDlgInfoPtr)GetWRefCon(wp);
			if(pd && (pd->magic==PLUGIN_MAGIC))
			{
				mouseDownPluginRef = pd->pluginRef;
				if(i==inContent)
				{
					pUIMouseUpDownData pdd;
					pdd.window=wp;
					pdd.e = e;
					pdd.down=true;
					runIndPlugin(pd->pluginRef, pUIMouseUpDownMessage, &pdd);
				}
				else if(i==inGrow)
				{
					pUIInGrowData pgrow;
					pgrow.window=wp;
					pgrow.e = e;
					runIndPlugin(pd->pluginRef, pUIInGrowMessage, &pgrow);
				}
				return;
			}
		}
		else
		{ //it's not a floater I know of.
			iwFront=0;
		}
	}
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

static pascal void RetryConnections(void)
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
	MBIdle();
	RunNotify();
}

static void Timer20(EventLoopTimerRef timer, void* data)
{
	linkPtr curLink;
	
	linkfor(curLink, firstLink)
		if(curLink->serverStatus==S_CONN)
			ProcessUserHosts(curLink);
}

static void Timer5(EventLoopTimerRef timer, void* data)
{
	checkConnections();
	UpdateStatusLine();
}

static void TimerTick(EventLoopTimerRef timer, void* data)
{
	//Stuff we need to do periodically when we're in the foreground
	if(!inBackground)
	{
		if(!inputLine.lock)
		{
			WEReference ilWE = ILGetWE();
			if(ilWE)
				WEIdle(0, ilWE);
		}
	}
	
	RetryConnections();
}

EventLoopTimerRef NetworkTimer;

static void NetworkProcess(EventLoopTimerRef timer, void* data)
{
	connectionEventRecord connEvt;
	int x = -8;
	
	while(GetConnectionEvent(&connEvt))
	{
		doTCPEvent(&connEvt);
		if(!++x)
			break;
	}
}

static void InitTimers()
{
	EventLoopRef mainLoop = GetMainEventLoop();
	EventLoopTimerUPP mtu, t20u, t5u, AsyncSoundTimerProc, tick, net;
	EventLoopTimerRef timer;
	
	mtu = NewEventLoopTimerUPP(MinuteTimer);
	t20u = NewEventLoopTimerUPP(Timer20);
	t5u = NewEventLoopTimerUPP(Timer5);
	tick = NewEventLoopTimerUPP(TimerTick);
	
	InstallEventLoopTimer(mainLoop, 60 * kEventDurationSecond, 60 * kEventDurationSecond, mtu, NULL, &timer);
	InstallEventLoopTimer(mainLoop, 20 * kEventDurationSecond, 20 * kEventDurationSecond, t20u, NULL, &timer);
	InstallEventLoopTimer(mainLoop, 5 * kEventDurationSecond, 5 * kEventDurationSecond, t5u, NULL, &timer);
	InstallEventLoopTimer(mainLoop, TicksToEventTime(1), TicksToEventTime(1), tick, NULL, &timer);
	
	AsyncSoundTimerProc = NewEventLoopTimerUPP(AsyncSoundCleanup);
	InstallEventLoopTimer(mainLoop, kEventDurationForever, kEventDurationForever, AsyncSoundTimerProc, NULL, &AsyncSoundTimer);

	net = NewEventLoopTimerUPP(NetworkProcess);
	InstallEventLoopTimer(mainLoop, TicksToEventTime(1), TicksToEventTime(1), net, NULL, &NetworkTimer);
}

static pascal void IdleTasks(EventRecord *e)
{
	idlePlugins(e);
}

#pragma mark -

static pascal void inZoomInOutHandler(const EventRecord *e, short part)
{
	MWPtr p;
	Rect r;
	
	p=MWFromWindow((WindowPtr)e->message);
	if(p)
	{
		if(TrackBox(p->w, e->where, part))
		{
			ZoomWindow(p->w, part, 0);
			WGetBBox(p->w, &r);
			MWSetDimen(p, r.left, r.top, r.right-r.left, r.bottom-r.top);
		}
	}
}

static pascal void inGrowHandler(const EventRecord *e)
{
	MWPtr p;
	Rect r;
	long ii;
	GrafPtr port;
	
	GetPort (&port);
	if(inputLine.w==(WindowPtr)e->message)
		IWGrow(e);
	else
	{
		p=MWFromWindow((WindowPtr)e->message);
		if(p)
		{
			SetRect(&r, 125, 60, 32767, 32767);
			ii=GrowWindow(p->w, e->where, &r);
			if(ii)
			{
				WGetBBox(p->w, &r);
				MWSetDimen(p, r.left, r.top, ii&0x0000FFFF, ii >> 16);
				UpdateWindowPosition(p->w);
			}
		}
		else
		{
			WindowPtr wp = (WindowPtr)e->message;
			pluginDlgInfoPtr pd;
			
			pd=(pluginDlgInfoPtr)GetWRefCon(wp);
			if(pd && (pd->magic==PLUGIN_MAGIC))
			{
				pUIInGrowData pgrow;
				pgrow.window=wp;
				pgrow.e = e;
				mouseDownPluginRef=pd->pluginRef;
				runIndPlugin(pd->pluginRef, pUIInGrowMessage, &pgrow);
			}
		}
	}
	SetPort(port);
}

static pascal void inContentHandler(EventRecord *e)
{
	MWPtr mw;
	pUIMouseUpDownData pdd;
	GrafPtr gp;
	WindowPtr w;
	
	GetPort(&gp);
	SetPortWindowPort(w);
	
	mw = MWFromWindow(w);
	if(mw)
		MWHitContent(mw, e);
	else
	{
		pluginDlgInfoPtr pd = (pluginDlgInfoPtr)GetWRefCon(w);
		if(pd && (pd->magic==PLUGIN_MAGIC))
		{
			pdd.window = w;
			pdd.e = e;
			pdd.down=true;
			mouseDownPluginRef=pd->pluginRef;
			runIndPlugin(pd->pluginRef, pUIMouseUpDownMessage, &pdd);
		}
	}

	SetPort(gp);
}

static pascal void inDragHandler(EventRecord *e)
{
	if(IsWindowPathSelectClick((WindowPtr)e->message, e))
		WindowPathSelect((WindowPtr)e->message, 0, 0);
	else
	{
		Rect WindowDragRect;
		GetRegionBounds(GetGrayRgn(), &WindowDragRect);
		DragWindow((WindowPtr)e->message, e->where, &WindowDragRect);
		UpdateWindowPosition((WindowPtr)e->message);
	}
}

inline void inGoAwayHandler(const EventRecord *e)
{
	MWPtr p;
	pluginDlgInfoPtr pl;
	
	p=MWFromWindow((WindowPtr)e->message);
	if(p)
	{
		if(TrackGoAway(p->w, e->where))
			MWPart(p);
	}
	else
	{
		if(TrackGoAway((WindowPtr)e->message, e->where))
		{
			pl=(pluginDlgInfoPtr)GetWRefCon((WindowPtr)e->message);
			if(pl && pl->magic==PLUGIN_MAGIC)
				pluginCloseWindow((WindowPtr)e->message, pl);
		}
	}
}

static OSStatus DoModifierKeysChangedEvent(EventHandlerCallRef handlerCallRef, EventRef event, void *data)
{
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
	pContextSwitchDataRec p;
	
	inBackground=0;
	InitCursor();
	
	if(!noFloatingInput)
		WEActivate(ILGetWE());
	
	NotifyRemove(); //Clear notification manager requests.

	p.inBackground=inBackground;
	runPlugins(pContextSwitchMessage, &p);
	
	return noErr;
}

static OSStatus DoSuspendEvent(EventHandlerCallRef handlerCallRef, EventRef event, void *data)
{
	pContextSwitchDataRec p;
	
	inBackground=1;
	
	if(!noFloatingInput)
		WEDeactivate(ILGetWE());
	
	p.inBackground=inBackground;
	runPlugins(pContextSwitchMessage, &p);
	
	return noErr;
}

pascal void doUpdateEvent(EventRecord *e)
{
	GrafPtr gp;
	
	if((WindowPtr)e->message == inputLine.w) //inputline
	{
		GetPort(&gp);
		SetPortWindowPort(inputLine.w);
		BeginUpdate(inputLine.w);
		WEUpdate(0, ILGetWE());
		UpdateStatusLine();
		EndUpdate(inputLine.w);
		SetPort(gp);
	}
	else
	{
		MWPtr mw=MWFromWindow((WindowPtr)e->message);

		if(mw)
		{
			BeginUpdate(mw->w);
			DrawControls(mw->w);
			MWPaneUpdate(mw);
			EndUpdate(mw->w);
		}
		else //not a message window - user window
		{	//cancel updates
			pUIUpdateData p;
			pluginDlgInfoPtr l;
			
			GetPort(&gp);
			SetPortWindowPort((WindowPtr)e->message);
			BeginUpdate((WindowPtr)e->message);
			l= (pluginDlgInfoPtr)GetWRefCon((WindowPtr)e->message);
			if(l && l->magic==PLUGIN_MAGIC)
			{
				p.e = e;
				runIndPlugin(l->pluginRef, pUIUpdateMessage, &p);
			}
			EndUpdate((WindowPtr)e->message);
			SetPort(gp);
		}
	}
}

static pascal void doMouseUp(EventRecord *e)
{
	pUIMouseUpDownData pdd;
	
	if(mouseDownPluginRef)
	{
		pdd.window=0;
		pdd.e = e;
		pdd.down=false;
		runIndPlugin(mouseDownPluginRef, pUIMouseUpDownMessage, &pdd);
		mouseDownPluginRef = 0;
	}
}

static pascal void doMouseDown(EventRecord *e)
{
	short i;
	MWPtr mw;
	WindowPtr p;
	
	i=FindWindow(e->where, &p);
	switch(i)
	{
		case inMenuBar:
			MenuBarClick(e);
			break;
		
		default:
			if((i==inContent || i==inGrow) && (p!=ActiveNonFloatingWindow())) // || i==inDrag
			{
				if(!WIsFloater(p))
				{
					WSelect(p);
					mw = MWFromWindow(p);
				}
				
				if(!CMClick(p, e))
				{
					if(WIsFloater(p))
						floatingWindowClick(e);
					else
						iwFront=0;
				}
			}
			else
			{
				e->message=(long)p;
				switch(i)
				{
					case inContent:
						if(CMClick(p, e))
							;
						else if(WIsFloater(p))
							floatingWindowClick(e);
						else
						{
							iwFront=0;
							inContentHandler(e);
						}
						break;
					
					case inGrow:
						inGrowHandler(e);
						break;
					
					case inGoAway:
						inGoAwayHandler(e);
						break;
					
					case inDrag:
						inDragHandler(e);
						break;
					
					case inZoomIn:
					case inZoomOut:
						inZoomInOutHandler(e, i);
						break;
				}
			}
	}
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
			memcpy(&conn->ip, &c->addr, sizeof(conn->ip));
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
			memcpy(&conn->ip, &c->addr, sizeof(conn->ip));
			break;
	}
	
	conn->InputFunc(c, conn);
}

static pascal void ApplEvents(EventRecord *e)
{
	switch(e->what)
	{
		case nullEvent:
			IdleTasks(e);
			break;
		
		case updateEvt:
			doUpdateEvent(e);
			break;
		
		case activateEvt:
			WindowActivate((WindowPtr)e->message, e->modifiers & 1);
			break;
		
		case kHighLevelEvent:
			AEProcessAppleEvent(e);
			break;
			
		case autoKey:
			if(!(e->modifiers & cmdKey))
		case keyDown:
				Key(e, !!(e->modifiers & cmdKey));
			break;
			
		case mouseDown:
			doMouseDown(e);
			break;
		
		case mouseUp:
			doMouseUp(e);
			break;
	}
}

pascal void ApplRun(void)
{
	EventRecord e;
	
	while(!QuitRequest)
	{
		WaitNextEvent(everyEvent, &e, 60, mouseRgn);
		GetDateTime(&now);
		ApplEvents(&e);
	}
	
	ApplExit();
}

static void MyIAEH(long class, long type, EventHandlerProcPtr handlerFunc)
{
	EventTypeSpec eventType = {class, type};
	
	InstallApplicationEventHandler(NewEventHandlerUPP(handlerFunc), 1, &eventType, NULL, NULL);
}

static void InitLocalEventHandlers()
{
	MyIAEH(kEventClassApplication, kEventAppActivated, DoResumeEvent);
	MyIAEH(kEventClassApplication, kEventAppDeactivated, DoSuspendEvent);
	MyIAEH(kEventClassKeyboard, kEventRawKeyModifiersChanged, DoModifierKeysChangedEvent);
}

void ApplInit(void)
{
	mouseRgn=NewRgn();
	SetRectRgn(mouseRgn, -32767, -32767, 32766, 32766);
	
	InitEventHandlers();
	InitLocalEventHandlers();

	FlushEvents(everyEvent, 0);
	
	scb=NewSndCallBackUPP(AsyncSoundCallback);
	
	InitTimers();
}

static pascal void ApplExit(void)
{
	runAllPlugins(pQuitMessage, 0);
	
	ConnStopNetworking();
	
	StopIC();
	
	RemoveDragHandlers();
	UnregisterAppearanceClient();
	
	NavUnload();
	
	ExitToShell();
}