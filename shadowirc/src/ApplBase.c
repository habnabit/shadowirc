/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
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

#include <Appearance.h>
#include <balloons.h>
#include <Sound.h>
#include <Navigation.h>
#include "WASTE.h"
#include <ControlDefinitions.h>

#include "AppearanceHelp.h"
#include "SmartScrollAPI.h"

#include "StringList.h"
#include "Inline.h"
#include "IRCGlobals.h"
#include "TCP.h"
#include "connections.h"
#include "LongStrings.h"
#include "MsgWindows.h"
#include "MWPanes.h"
#include "TextWindows.h"
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
#include "ApplBase.h"

#pragma internal on
static pascal void WindowActivate(WindowPtr window, char activate);
static pascal void floatingWindowClick(EventRecord *e);
static pascal void inZoomInOutHandler(const EventRecord *e, short part);
static pascal void inDragHandler(EventRecord *e);
inline void inProxyHandler(EventRecord *e);
static pascal void inContentHandler(EventRecord *e);
inline void inGoAwayHandler(const EventRecord *e);
static pascal void inGrowHandler(const EventRecord *e);
static pascal char CheckMem(void);
static pascal void DoMouseMovedEvent(const EventRecord *e);
static pascal void doMouseUp(EventRecord *e);
static pascal void doMouseDown(EventRecord *e);
static pascal void doTCPEvent(CEPtr message);
static pascal void ApplEvents(EventRecord *e);
static pascal char doDialogEvent(EventRecord *e);
static pascal void FinishStaleConnections(void);
static pascal void ApplExit(void);

WindowPtr ContextWindow = 0;

static pascal void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord);

static RgnHandle mouseRgn;

long sScrollStep;

static plugsPtr mouseDownPluginRef=0;

typedef struct aspData {
	long refcon;
	SndListHandle sound;
	SndChannelPtr soundChannel;
	
	struct aspData *next, *prev;
	char *gSoundIsFinished;
	plugsPtr pluginRef;

	char isFinished;
	char disposeOfChannel;
} pAsyncSoundCompletionData, *pAsyncSoundCompletionDataPtr, aspData, *aspDataPtr;

static aspDataPtr aspList=0;
static char gSoundIsFinished = 0;

static SndCallBackUPP scb;

static pascal void AsyncSoundCheck(void)
{
	aspDataPtr asp = aspList;
	aspDataPtr next;
	
	gSoundIsFinished = 0;
	
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
}

static pascal void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord)
{
	#pragma unused(theSoundChannel)
	
	aspDataPtr asp = (aspDataPtr)infoRecord->param2;
	asp->isFinished=1;
	*asp->gSoundIsFinished=1;
}

#pragma internal reset

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
			asp->gSoundIsFinished = &gSoundIsFinished;
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


pascal char doQuit(LongString *reason)
{
	short i;
	linkPtr link;
	int save;
	
	if(TWCloseAll())
	{
		if(mainPrefs->quitAction == qaAutoSave)
			save = 1;
		else
		{
			save = 0;
			if(mainPrefs->quitAction == qaConfirm)
			{
				DialogPtr d=GetNewDialog(131, 0, (WindowPtr)-1);
				SetupModalDialog(d, 1, 5);
				do {
					ModalDialog(0, &i);
				} while(!i);
				
				DisposeDialog(d);
				FinishModalDialog();
				
				if(i == 5)
					return 0;
				else if(i == 1) //Else they chose save or not.
					save = 1;
			}
		}
		
		if(save)
			writeAllFiles();
		
		linkfor(link, firstLink)
			if(link->serverStatus==S_CONN)
				DoSignoff(link, reason);
		
		QuitRequest=1;
	}
	
	return QuitRequest;
}

static pascal void WindowActivate(WindowPtr window, char activate)
{
	MWPtr p;
	pUIActivateData pl;
	channelPtr ch;
	pServiceActivateWinData ps;
	
	if(activate)
		ContextWindow = window;
	
	if(window==inputLine.w)
		return;

	if(MWActive)
		MWLast = MWActive;
	
	p=MWFromWindow(window);
	if(p)
	{
		if(activate)
			MWActive=p;
		else
			MWActive=0;
		
		MWPaneActivate(p, activate);
		
		ch = MWGetChannel(p);
	}
	else
	{
		pluginDlgInfoPtr l;
		
		ch=0;
		MWActive=0;
		
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
		if(p && !inBackground && MWActive)
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

enum memorySizes {
	LOFREEMEM = 1500,		//die if we have less than 1.5k free
	HIFREEMEM = 25600		//warn if we have less than 25k free
};

static pascal char CheckMem(void)
{
	static long lastCheck = 0;
	static long lastFmem = 0;
	static char lowmemWarning = 0;
	static char purged = 0, dieNow = 0;
	long fm = FreeMem();
	LongString ls;
	long i;
	linkPtr link;
	MWPtr mw;
	char purgedAnything = 0;
	
	if(fm>HIFREEMEM)
	{
		purged=dieNow=lowmemWarning=0;
		goto exitCheckMem;
	}
	else if(fm>LOFREEMEM)
	{
		dieNow=0;
	}
	
	if(dieNow)
	{
		LSStrLS("\pQUIT :Ran out of memory", &ls);
		linkfor(link, firstLink)
			SendCommand(link, &ls);
			
		Delay(90, (unsigned long*)&i);
		
		SysBeep(0);
		ParamText(GetIntStringPtr(spError, sRanOutOfMemory), "\p", "\p", "\p");
		Alert(130, 0);
		ApplExit();
	}
	else if(purged)
	{
		if(fm<LOFREEMEM)
		{
			//Before we give up, let's try one last ditch effort to free up memory.
			//That means, troll through the windows and find the help window. If there's one,
			//close it and hope we don't get back here.
			//If there isn't, troll through and ask to close text windows. If they close one, stop,
			//else keep trolling.
			linkfor(mw, mwl)
				if(mw->winType == helpWin)
				{
					CloseHelp();
					purgedAnything = 1;
					goto exitCheckMem;
				}
			
			//If no help window
			linkfor(mw, mwl)
				if(mw->winType == textWin)
					if(TWClose(mw, true))
					{
						purgedAnything = 1;
						goto exitCheckMem;
					}
			
			//if no help window, and we didn't close a text window, we're in trouble.
			//We should scan through and find inactive channel/query/dcc windos and close them
			
			dieNow=1;
			goto exitCheckMem;
		}
	}
	
	if(fm < LOFREEMEM && lowmemWarning)
	{
		if(purged)
			dieNow=1;
		else
		{
			purged=1;
			PurgeMem(maxSize);
			SysBeep(1);
		}
	}
	else if(fm < (HIFREEMEM + 1536)) //at himem + 1.5k, purge stuff.
	{
		//Troll through the message windows, and for those windows that have more than 10k of text, delete the first 1k.
		long len;
		long s0, s1;
		WEReference we;
		
		linkfor(mw, mwl)
		{
			if(mw->winType != helpWin && mw->winType != textWin)
			{
				we = mw->we;
				len=WEGetTextLength(we);
				if(len>7168) //if there's more than 7kb of text in the window
				{
					int shorten =  len - 6144; //shorten it to 6kb
					
					WEGetSelection(&s0, &s1, we);
					WEDeactivate(we);
					WEFeatureFlag(weFAutoScroll, weBitClear, we);
					WESetSelection(0, shorten, we);
					WEDelete(we);
					s0-=shorten;
					if(s0<0)
						s0=0;
					s1-=shorten;
					if(s1<0)
						s1=0;
					WESetSelection(s0, s1, we);
					WEFeatureFlag(weFAutoScroll, weBitSet, we);
					if(mw == MWActive)
						WEActivate(we);

					purgedAnything = 1;
				}
			}
		}
		LSGetIntString(&ls, spError, sLowOnMem);
		LineMsg(&ls);
		
		fm=FreeMem();
		if(fm < HIFREEMEM)
		{
			if(!lowmemWarning)
			{
				ConstStringPtr s, s2;
				
				lowmemWarning=1;
				s = GetIntStringPtr(spError, sShadowIRCLowMem);
				s2 = GetIntStringPtr(spError, sShadowIRCLowMem2);
				ParamText(s, s2, "\p", "\p");
				SysBeep(0);
				EnterModalDialog();
				Alert(136, 0);
				ExitModalDialog();
				
				//check and see if we're in the bg
				if(inBackground)
				{
				}
			}
		}
	}

exitCheckMem:	
	lastFmem = fm;
	lastCheck = TickCount();
	return purgedAnything;
}

pascal void UpdateWindowPosition(WindowPtr win)
{
	MWPtr mw;
	Rect r;
	pUIWindowMoveDataRec p;
	
	if(win==inputLine.w)
	{
		r=WGetBBox(win);
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
				p.newpos=WGetBBox(win);
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
	ContextWindow = wp;
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

inline pascal void checkCursorFocus(EventRecord *e)
{
	static Point prevLoc = {0,0};
	MWPtr mw;
	WindowPtr p;
	
	//If we're doing cursor focus, the command key isn't down, and the mouse has moved
	if(mainPrefs->cursorFocus &&!(e->modifiers & cmdKey) && (*(long*)&prevLoc != *(long*)&e->where))
	{
		prevLoc = e->where;
		
		FindWindow(e->where, &p);
		if(p && !WIsFloater(p))
		{
			if(!mainPrefs->cursorFocusDontActivate) //autoraise
			{
				if(FrontNonFloater()!=p)
					WSelect(p);
			}
			else //autoraise disabled
			{
				if(p==consoleWin->w)
				{
					if(!CurrentTarget.bad)
					{
						InvalTarget(&CurrentTarget);
						UpdateStatusLine();
					}
				}
				else if((mw=MWFromWindow(p)) != nil)
				{
					if(mw != CurrentTarget.mw)
					{
						SetTarget(mw, &CurrentTarget);
						UpdateStatusLine();
					}
				}
			}
		}
	}
}

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

static pascal void IdleTasks(EventRecord *e)
{
	static unsigned long userhostsTimer, minuteTimer, periodicTasksTimer;
	linkPtr curLink;
	
	if(gSoundIsFinished)
		AsyncSoundCheck();
	
	//Stuff we need to do periodically when we're in the foreground
	if(!inBackground)
	{
		if(MWActive && MWActive->winType == textWin && !iwFront)
			WEIdle(0, MWActive->we);
		else
			if(!inputLine.lock)
			{
				WEReference ilWE = ILGetWE();
				if(ilWE)
					WEIdle(0, ilWE);
			}

		checkCursorFocus(e); //perhaps this should be in a mouse moved event?
		CMSetCursor();
	}
	
	RetryConnections();
	
	
	//Periodic tasks
	if(now-periodicTasksTimer>5)
	{
		periodicTasksTimer=now;
		CheckMem();
		checkConnections();
		
		if(now-userhostsTimer>20)
		{
			userhostsTimer = now;
			linkfor(curLink, firstLink)
				if(curLink->serverStatus==S_CONN)
					ProcessUserHosts(curLink);
		}
		
		if(now-minuteTimer>60)
		{
			minuteTimer = now;
			MBIdle();
			RunNotify();
		}
		
		UpdateStatusLine();
	}
	
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
			r=WGetBBox(p->w);
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
				r=WGetBBox(p->w);
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
	MWPtr p;
	ControlHandle c;
	short pa;
	ControlActionUPP upp;
	long pageSize;
	LongRect viewRect;
	pUIMouseUpDownData pdd;
	GrafPtr gp;
	
	GetPort(&gp);
	SetPortWindowPort((WindowPtr)e->message);
	ContextWindow = (WindowPtr)e->message;
	p=MWFromWindow((WindowPtr)e->message);
	if(p)
	{
		MWActive=p;
		GlobalToLocal(&e->where);
		pa=FindControl(e->where, p->w, &c);
		if(c == p->vscr) //kludge, since the only control I know about is the vscr.
		{
			switch(pa)
			{
				case kControlPageUpPart:
				case kControlPageDownPart:
				case kControlUpButtonPart:
				case kControlDownButtonPart:
					WEGetViewRect(&viewRect, p->we);
					pageSize=viewRect.bottom-viewRect.top;
					if(pa== kControlUpButtonPart)
						sScrollStep=-p->scrpHeight;
					else if(pa == kControlDownButtonPart)
						sScrollStep=p->scrpHeight;
					else if(pa == kControlPageUpPart)
						sScrollStep=-(pageSize-p->scrpHeight);
					else// if(pa == kControlPageDownPart)
						sScrollStep=(pageSize-p->scrpHeight);

					upp = NewControlActionUPP(MWVScrollTrack);
					pa=TrackControl(c, e->where, upp);
					DisposeControlActionUPP(upp);
					break;
				
				case kControlIndicatorPart:
					MWLiveScroll(p, e->where);
					break;
			}
		}
		else
			MWPaneClick(p, e);
	}
	else
	{
		pluginDlgInfoPtr pd = (pluginDlgInfoPtr)GetWRefCon((WindowPtr)e->message);
		if(pd && (pd->magic==PLUGIN_MAGIC))
		{
			pdd.window=(WindowPtr)e->message;
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
	if(hasWM11 && (IsWindowPathSelectClick((WindowPtr)e->message, e)))
		WindowPathSelect((WindowPtr)e->message, 0, 0);
	else
	{
		Rect WindowDragRect;
		GetRegionBounds(GetGrayRgn(), &WindowDragRect);
		WDrag((WindowPtr)e->message, e->where, &WindowDragRect);
		UpdateWindowPosition((WindowPtr)e->message);
	}
}

inline void inProxyHandler(EventRecord *e)
{
	if(IsWindowPathSelectClick((WindowPtr)e->message, e))
		WindowPathSelect((WindowPtr)e->message, 0, 0);
	else
		TrackWindowProxyDrag((WindowPtr)e->message, e->where);
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

pascal void DoSuspendResumeEvent(EventRecord *e)
{
	pContextSwitchDataRec p;
	
	if(e->message & resumeFlag)
	{
		inBackground=0;
		TEFromScrap();
		InitCursor();
		
		if(!noFloatingInput)
			WEActivate(ILGetWE());
		WResume();
		
		NotifyRemove(); //Clear notification manager requests.
	}
	else
	{
		inBackground=1;
		
		if(!noFloatingInput)
			WEDeactivate(ILGetWE());
		WSuspend();
	}
	
	p.inBackground=inBackground;
	runPlugins(pContextSwitchMessage, &p);
}

static pascal void DoMouseMovedEvent(const EventRecord *e)
{
	#pragma unused(e)
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
			if((i==inContent || i==inGrow) && (p!=FrontNonFloater())) // || i==inDrag
			{
				if(!WIsFloater(p))
				{
					WSelect(p);
					MWActive = mw = MWFromWindow(p);
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
				if(CMClick(p, e))
					break;
				
				e->message=(long)p;
				switch(i)
				{
					case inContent:
						if(WIsFloater(p))
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

					case inProxyIcon:
						inProxyHandler(e);
						break;
				}
			}
	}
}

static pascal char processOutgoingConnectionEvent(CEPtr c, connectionPtr conn)
{
	if(conn->tryingToConnect)
	{
		LongString ls;
		pServiceCWLinkStateChangeData p;
		linkPtr link;
		p.link = link = conn->link;

		if(conn->connectStage == csLookingUp || conn->connectStage == csLookingUp2)
		{
			if(c->event == C_Found)
			{
				if(conn->connectStage == csLookingUp2)
				{
					conn->ip2 = c->value;
					ConnFindAddress(conn, link->conn->name);
					
					LSParamString(&ls, GetIntStringPtr(spInfo, sLookingUpIP), link->conn->name, 0, 0, 0);
					SMPrefix(&ls, dsConsole);

					p.connectStage = conn->connectStage = csLookingUp;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
				}
				else
				{
					conn->ip = c->value;
					connection2(conn);
				}
			}
			else
			{
				if(conn->socksType == connIRC)
				{
					p.connectStage = conn->connectStage = csFailedToLookup;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
					ServerOK(c->event, link);
				}
				else if(conn->socksType == connDCC)
				{
					DCCFailed(&conn, "\p");
				}
			}
			return true;
		}
		if(conn->connectStage == csOpeningConnection)
		{
			if(c->event == C_Established)
			{
				conn->ip = c->value;
				
				if(conn->connType == connSOCKS || conn->connType == connIRC)
				{
					if(conn->connType == connIRC || conn->socksType == connIRC)
					{
						if(c->value == -1)
							LinkFailedConnection(link);
						else
						{
							LinkSuccessfulConnection(link, conn->connType == connIRC);
							if(conn->connType == connSOCKS)
								conn->InputFunc(c, conn);
						}
					}
					else if(conn->socksType == connDCC)
						conn->InputFunc(c, conn);
				}
			}
			else
			{
				if(conn->socksType == connIRC)
				{
					p.connectStage = conn->connectStage=csConnectFailed;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
					ServerOK(c->event, link);
				}
				else if(conn->socksType == connDCC)
				{
					DCCFailed(&conn, "\p");
				}
			}
			return true;
		}
	}
	
	return false;
}

static pascal void doTCPEvent(CEPtr c)
{
	connectionPtr conn;
	
	conn=findConnectionSock(c->connection);
	if(conn)
		if(!processOutgoingConnectionEvent(c, conn))
			conn->InputFunc(c, conn);
}

static pascal char doDialogEvent(EventRecord *e)
{
	WindowPtr fw;
	DialogPtr p;
	short theItem;
	pluginDlgInfoPtr pl;
	
	if((e->what==keyDown) && ((e->message & 255)==3) || ((e->message & 255)==13))
	{	//pressing return or enter in dlg
		fw=FrontNonFloater();
		e->message=(long)fw;
		
		pl=(pluginDlgInfoPtr)GetWRefCon(fw);
		if(pl && (pl->magic==PLUGIN_MAGIC))
			pluginHitDialog(GetDialogFromWindow(fw), pl, 1);
		
		return 1;
	}
	else if((e->what==keyDown) && ((e->message&255)==27))
	{	//pressing esc in dlg
		fw=FrontNonFloater();
		e->message=(long)fw;
		
		pl=(pluginDlgInfoPtr)GetWRefCon(fw);
		if(pl && (pl->magic==PLUGIN_MAGIC))
			pluginHitDialog(GetDialogFromWindow(fw), pl, 2);
		
		return 1;
	}
	else if(DialogSelect(e, &p, &theItem))
	{
		e->message=(long)p;
		
		pl=(pluginDlgInfoPtr)GetWRefCon(GetDialogWindow(p));
		if(pl && (pl->magic==PLUGIN_MAGIC))
			pluginHitDialog(p, pl, theItem);
		return 1;
	}
	else 
		return (e->what == updateEvt) || (e->what !=0);
}

static pascal void ApplEvents(EventRecord *e)
{
	long l;

	if((e->what==keyDown) && (e->modifiers & cmdKey))
	{
		char c= e->message & 0xFF;
		if(((e->modifiers&(controlKey|cmdKey|shiftKey|optionKey)) == (controlKey|cmdKey)) && c>='0' && c<='9') //hack for cmd-ctrl-# for 8.5
		{
			l = c - '0';
			if(!l)
				l=10;
			FlashMenuBar(shortcutsMenu);
			ShortcutsMenu(22 + l);
			FlashMenuBar(shortcutsMenu);
		}
		else
		{
			l = MenuEvent(e);

			if(!l)
				Key(e, 1);
			else
				DoMenuEvent(l, e);
		}
		return;
	}
	else if(IsDialogEvent(e))
		if(doDialogEvent(e))
			return;
	
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
		
		case osEvt:
			if(*(char*)&e->message == suspendResumeMessage)
				DoSuspendResumeEvent(e);
			else if(*(char*)&e->message == mouseMovedMessage)
				DoMouseMovedEvent(e);
			break;
		
		case kHighLevelEvent:
			AEProcessAppleEvent(e);
			break;
			
		case autoKey:
			if(!(e->modifiers & cmdKey))
		case keyDown:
				Key(e, 0);
			break;
			
		case mouseDown:
			doMouseDown(e);
			break;
		
		case mouseUp:
			doMouseUp(e);
			break;
	}
}

pascal void doNetworkCheck(void)
{
	connectionEventRecord connEvt;
	if(GetConnectionEvent(&connEvt))
		doTCPEvent(&connEvt);
}

pascal void ApplRun(void)
{
	EventRecord e;
	connectionEventRecord connEvt;
	int x;
	
	while(!QuitRequest)
	{
		WaitNextEvent(-1, &e, 1, mouseRgn);
		GetDateTime(&now);
		if(e.what==nullEvent)
		{
			x=-8;
			while(GetConnectionEvent(&connEvt))
			{
				doTCPEvent(&connEvt);
				if(!++x)
					break;
			}
			
			if(x >= -2) //lots of data...check memory to help protect from being flooded off
				CheckMem();
		}
		ApplEvents(&e);
	}
	
	ApplExit();
}

static pascal long GrowZoneProc(long needed)
{
#pragma unused(needed)
	char ret  = CheckMem();
	
	return ret;
}

pascal void ApplInit(void)
{
	mouseRgn=NewRgn();
	SetRectRgn(mouseRgn, -32767, -32767, 32766, 32766);
	
	MenuInit();

	SetGrowZone(NewGrowZoneProc(GrowZoneProc));

	FlushEvents(everyEvent, 0);
	
	ActivateWindowProcPtr=&WindowActivate;
	scb=NewSndCallBackProc(AsyncSoundCallback);
}

static pascal void ApplExit(void)
{
	runAllPlugins(pQuitMessage, 0);
	
	ConnStopNetworking();
	
	StopIC();
	
	RemoveDragHandlers();
	UnregisterAppearanceClient();
	
	if(hasNav)
		NavUnload();
	
	#if !TARGET_CARBON
	DisposeAllSmartScrolls();
	#endif
	ExitToShell();
}