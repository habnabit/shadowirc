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

#include <Appearance.h>
#include <balloons.h>
#include <Sound.h>
#include <Navigation.h>
#include "WASTE.h"
#include <ControlDefinitions.h>

#include "AppearanceHelp.h"
#include "SmartScrollAPI.h"

#include "LongStrings.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "MWPanes.h"
#include "WindowList.h"
#include "connections.h"
#include "IRCChannels.h"
#include "IRCCommands.h"
#include "InputLine.h"
#include "channels.h"
#include "TCP.h"
#include "DCC.h"
#include "plugins.h"
#include "IRCAux.h"
#include "filesMan.h"
#include "utils.h"
#include "IRCInput.h"
#include "IRCInputPrivate.h"
#include "Inline.h"
#include "Floaters.h"
#include "IRCPreferences.h"
#include "Shortcuts.h"
#include "ApplBase.h"
#include "CMenus.h"
#include "InetConfig.h"
#include "TextWindows.h"
#include "DragDrop.h"
#include "Notify.h"
#include "TextManip.h"
#include "CalcCRC.h"
#include "CommandsMenu.h"

#pragma internal on
static pascal void DoAbout(void);
static pascal void WindowActivate(WindowPtr window, char activate);
static pascal void UpdateWindowPosition(WindowPtr win);
static pascal void floatingWindowClick(EventRecord *e);
inline void menuFonts(short item);
static pascal void MenuAppleURL(short item);
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
static pascal void cascade2(WindowPtr w);
static pascal void DoCascadeWindows(void);
static pascal void DoTileWindows(void);
static pascal void DoCycleCommand(char next);
static pascal void DoFind2(void);
static pascal void DoFind(char again);
static pascal void FinishStaleConnections(void);
static pascal void CloseFrontWindow(void);
static pascal void doMenuClick(const EventRecord *e);
static pascal void MenuFILE(int item);
static pascal void doMenuEvent(long menuitem, const EventRecord *e);

WindowPtr ContextWindow = 0;

static pascal void AsyncSoundCallback(SndChannelPtr theSoundChannel, SndCommand *infoRecord);

MenuHandle gAppleMenu, gEditMenu, gShortcutsMenu, gWindowMenu;
static MenuHandle gFileMenu, gFontsMenu;

static short applMItems;
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
UserItemUPP AboutDlgVersionFilter;

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


#pragma internal on

#pragma dont_inline off
inline char inupc(char c);
inline char inupc(char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

static pascal char _strncmp(const char *s1, const char *s2, unsigned long len)
{
	int x;
	
	for(x=0; x < len; x++, s1++, s2++)
		if(*s1 != *s2)
			return 0;
	
	return 1;
}

static pascal char _strncmpcase(const char *s1, const char *s2, unsigned long len)
{
	int x;
	
	for(x=0; x < len; x++, s1++, s2++)
		if(inupc(*s1) != inupc(*s2))
			return 0;
	
	return 1;
}

static pascal long FindText(Handle t, long start, Str255 searchFor, char caseSen, char reverse)
{
	long x;
	char b;
	long max;
	char* p;
	long mag;
	
	max = GetHandleSize(t);
	HLock(t);
	p = *t;
	
	if(reverse)
		mag = -1;
	else
		mag = 1;
	
	if(reverse && start + searchFor[0] > max)
		x = max - searchFor[0] - 1;
	else
		x = start;
	
	for(; x>=0 && x < max; x += mag)
	{
		if(caseSen)
			b = _strncmp(&p[x], (char*)&searchFor[1], searchFor[0]);
		else
			b = _strncmpcase(&p[x], (char*)&searchFor[1], searchFor[0]);
		
		if(b)
			return x;
	}
	HUnlock(t);

	return -1;
}

#pragma dont_inline reset

static struct FindInformation {
	Str255 searchFor;
	char caseSen;
	char reverse;
} find = {"\p", 0, 1};

static pascal void DoFind2(void)
{
	Handle t;
	long s0, s1;
	long found;
	
	if(find.searchFor[0] && MWActive)
	{
		t=WEGetText(MWActive->we);
		WEGetSelection(&s0, &s1, MWActive->we);
		if(s0==s1)
		{
			if(find.reverse)
				s0 = GetHandleSize(t) + 1;
			else
				s0=-1;
		}
		if(t)
		{
			if(find.reverse)
				s0--;
			else
				s0++;
			found=FindText(t, s0, find.searchFor, find.caseSen, find.reverse);
			if(found>=0)
			{
				WESetSelection(found, found+find.searchFor[0], MWActive->we);
				WESelView(MWActive->we);
			}
			else
				SysBeep(0);
		}
	}
}

static pascal void DoFind(char again)
{
	DialogPtr d;
	short i;
	
	if(again && find.searchFor[0])
		DoFind2();
	else //throw a dialog
	{
		d=GetNewDialog(135, 0, (WindowPtr)-1);
		setCheckBox(d, 5, find.caseSen);
		setCheckBox(d, 6, find.reverse);
		SetText(d, 4, find.searchFor);
		SelectDialogItemText(d, 4, 0, 255);

		SetupModalDialog(d, 1, 2);
		do {
			ModalDialog(StdDlgFilter, &i);
			
			if(i==5 || i==6)
				setCheckBox(d, i, !getCheckBox(d, i));
		} while((i!=1) && (i!=2));
		
		if(i==1)
		{
			find.caseSen=getCheckBox(d, 5);
			find.reverse=getCheckBox(d, 6);
			GetText(d, 4, find.searchFor);
		}

		DisposeDialog(d);
		FinishModalDialog();
		
		if(i==1)
			DoFind2();
	}
}

#pragma mark -

pascal char doQuit(char force, LongString *reason)
{
	short i;
	linkPtr link;
	int save;
	
	//Force used to be if the program was quitting for unregistered timeout.
	
	if(TWCloseAll(force))
	{
		if(mainPrefs->quitAction == qaAutoSave)
			save = 1;
		else
		{
			int sel = 0;

			save = 0;
			if(!force && mainPrefs->quitAction == qaConfirm)
				sel = 1;

			if(sel)
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
			{
				LongString ls2;
				
				if(force)
				{
					LSStrLS("\pThis unregistered copy of ShadowIRC has expired.", &ls2);
					DoSignoff(link, &ls2);
				}
				else
					DoSignoff(link, reason);
			}
		
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
			if(link->serverStatus==S_CONN)
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
					if(TWClose(mw, true, false))
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
	lastFmem=fm;
	lastCheck=TickCount();
	return purgedAnything;
}

static pascal void UpdateWindowPosition(WindowPtr win)
{
	MWPtr mw;
	Rect r;
	pUIWindowMoveDataRec p;
	
	if(win==inputLine.w)
	{
		r=WGetBBox(win);
		if(r.bottom-r.top>32)
			mainPrefs->inputLoc=r;
	}
	else
	{
		mw=MWFromWindow(win);
		if(mw)
		{
			if(mw->winType == conWin)
			{
				r=WGetBBox(win);
				if(r.bottom-r.top>=64)
					mainPrefs->consoleLoc=r;
			}
			else if(mw->winType==chanWin)
			{
				linkPrefsPtr lp=mw->link->linkPrefs;
				
				r=WGetBBox(win);
				if(r.bottom-r.top>=64)
					lp->windowLoc[mw->channelWindowNumber]=r;
			}
			else if(mw->pluginRef)
			{
				p.w=mw->w;
				p.newpos=WGetBBox(win);
				runIndPlugin(mw->pluginRef, pUIWindowMoveMessage, &p);
			}
		}
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

static pascal void DoCycleCommand(char next)
{
	MWPtr activeW, mw;
	
	if(!FrontNonFloater()) //do nothing if no front window.
		return;
	
	activeW=MWActive;
	if(!activeW) //no active mw. find one
	{
		return;
	}
	
	if(next)
	{
		if(!activeW->prev)
			mw=consoleWin;
		else
			mw=activeW->prev;
		
		//window to activate is in mw, and w is correct
		while(!IsVisible(mw->w))
		{
			mw=mw->prev;
			
			if(mw==activeW)
				break;
			else if(!mw)
				mw=consoleWin;
		}
		MWActive=mw;
		WSelect(mw->w);
	}
	else //exact same as above, except prev instead of next
	{
		if(!activeW->next)
			mw = mwl;
		else
			mw=activeW->next;
		
		//window to activate is in mw, and w is correct
		while(!IsVisible(mw->w))
		{
			mw=mw->next;
			
			if(mw==activeW)
				break;
			else if(!mw)
				mw = mwl;
		}
		MWActive=mw;
		WSelect(mw->w);
	}
}

pascal void WindowClose(WindowPtr wp)
{
	long i;
	
	if(wp)
	{
	i=GetWRefCon(wp);
	if(i)
	{
		if(((MWPtr)i)->magic==MW_MAGIC)
				MWPart((MWPtr)i);
		else if(((pluginDlgInfoPtr)i)->magic==PLUGIN_MAGIC)
			pluginCloseWindow((WindowPtr)wp, (pluginDlgInfoPtr)i);
	}
}
}

static pascal void CloseFrontWindow(void)
{
	WindowClose(FrontNonFloater());
}

static pascal void cascade2(WindowPtr w)
{
	WindowPtr w2;
	MWPtr mw;
	Rect r;
	
	w2=GetNextWindow(w);
	if(w2)
		cascade2(w2);
	
	mw=MWFromWindow(w);
	if(mw)
	{
		MWNewPosition(&r);
		MWSetDimen(mw, r.left, r.top, r.right-r.left, r.bottom-r.top);
		UpdateWindowPosition(mw->w);
		MWNewPosition(&r); //twice to give some more space
	}
}

static pascal void DoCascadeWindows(void)
{
	WindowPtr w;
	
	SetRect(&cornerstone, 5, GetMBarHeight() + 16, 425, 340);
	w=FrontNonFloater();
	
	if(w)
		cascade2(w);
}

static pascal void DoTileWindows(void)
{
	WindowPtr w = FrontNonFloater();
	WindowPtr w2=w;
	int cnt = 0;
	int height;
	short windowTopHeight, windowBotHeight;
	short windowLeftWid, windowRightWid;
	
	if(w)
	{
		RgnHandle cr, sr;
		RectPtr crr, srr;
		
		if(hasAppearance11)
		{
			cr = NewRgn();
			sr = NewRgn();
			
			GetWindowRegion(w, kWindowContentRgn, cr);
			GetWindowRegion(w, kWindowStructureRgn, sr);
			crr = GetRegionBounds(cr, 0);
			srr = GetRegionBounds(sr, 0);
		}
#if !TARGET_CARBON
		else
		{
			WindowPeek wp = (WindowPeek)w;

			crr = &(**wp->contRgn).rgnBBox;
			srr = &(**wp->strucRgn).rgnBBox;
		}
#endif

		windowTopHeight = crr->top - srr->top;
		windowBotHeight = srr->bottom - crr->bottom;

		windowLeftWid = crr->left - srr->left;
		windowRightWid = srr->right - crr->right;

		if(hasAppearance11)
		{
			DisposeRgn(cr);
			DisposeRgn(sr);
		}
	}
	
	while(w)
	{
		if(IsVisible(w) && MWFromWindow(w))
			cnt++;
		
		w = GetNextWindow(w);
	}
	
	if(cnt>0)
	{
		short totHeight;
		Rect r;
		short mbarHeight = GetMBarHeight();
		Rect sb;
#if TARGET_CARBON
		sb = GetQDGlobalsScreenBits(0)->bounds;
#else
		sb=qd.screenBits.bounds;
#endif

		SetRect(&sb, -sb.left, -sb.top + mbarHeight + windowTopHeight, sb.right - sb.left, sb.bottom - sb.top - 35);
		totHeight = sb.bottom - sb.top;
		height = (totHeight / cnt) - windowTopHeight - windowBotHeight;
		
		r.left = windowLeftWid;
		r.right = sb.right - windowRightWid;
		r.top = sb.top;
		r.bottom = r.top + height;
		
		w = w2;
		while(w)
		{
			if(IsVisible(w))
			{
				MWPtr mw = MWFromWindow(w);
				if(mw)
				{
					MWSetDimen(mw, r.left, r.top, r.right-r.left, r.bottom-r.top);
					UpdateWindowPosition(mw->w);
					r.top+=height + windowTopHeight + windowBotHeight;
					r.bottom+=height + windowTopHeight + windowBotHeight;
				}
			}
			
			w = GetNextWindow(w);
		}
	}
}

static pascal void DoAbout(void)
{
	DialogPtr d;
	short i;
	Rect itemRect;
	Handle itemHandle;
	short itemType;
	
	d = GetNewDialog(128, 0, (WindowPtr)-1);
	SetDialogDefaultItem(d, 1);
	SetDlogFont(d);
	EnterModalDialog();
	ShowWindow(GetDialogWindow(d));

	GetDialogItem(d, 3, &itemType, &itemHandle, &itemRect);
	SetDialogItem(d, 3, itemType, (Handle)AboutDlgVersionFilter, &itemRect);
	GetDialogItem(d, 4, &itemType, &itemHandle, &itemRect);
	SetDialogItem(d, 4, itemType, (Handle)AboutDlgVersionFilter, &itemRect);
	GetDialogItem(d, 5, &itemType, &itemHandle, &itemRect);
	SetDialogItem(d, 5, itemType, (Handle)AboutDlgVersionFilter, &itemRect);

	do {
		ModalDialog(StdDlgFilter, &i);
	} while(!i);

	DisposeDialog(d);
	ExitModalDialog();
}

#pragma mark -

static pascal void MenuFILE(int item)
{
	switch(item)
	{
		case 2:
			NewTextWin();
			break;
		
		case 3:
			TWSelect();
			break;
			
		case 4:
			CloseFrontWindow();
			break;
		
		case 6:
			if(MWActive && MWActive->winType == textWin)
				TWSave(MWActive, false);
			else
				writeAllFiles();
			break;

		case 9:
			doQuit(false, 0);
			break;
	}
}

pascal void menuEdit(short item)
{
	char mwFront, otherFront;
	TEHandle te;
	WEReference we;
	long s0, s1;
	WindowPtr w = FrontNonFloater();
	
	if(item == 12)
	{
		OpenPreferencesWindow(-1);
		return;
	}
	
	if(w && IsDialog(w))
	{
		DialogPtr d = GetDialogFromWindow(w);
		
		if(item == 3)
		{
			DialogCut(d);
			ZeroScrap();
			TEToScrap();
		}
		else if(item == 4)
		{
			DialogCopy(d);
			ZeroScrap();
			TEToScrap();
		}
		else if(item==5)
			DialogPaste(d);
		else if(item==6) //clear
			DialogDelete(d);
		else if(item==7) //select all
		{
			te = GetDialogTextEditHandle(d);
			if(te)
				SelectDialogItemText(d, GetDialogKeyboardFocusItem(d), 0, 32766);
		}
		return;
	}
	
	if(MWActive)
	{
		mwFront = 1;
		otherFront = 0;
	}
	else
	{
		mwFront = 0;
		otherFront = !iwFront;
	}

	te=0;
	we=0;
	if(iwFront)
		we=ILGetWE();
	else if(mwFront)
		we=MWActive->we;
	else if(otherFront) //this was a dangerous assumption.
	{
		if(ContextWindow) //this isn't one of my windows. Is it a plugin's?
		{
			s0 = GetWRefCon(ContextWindow);
			if(s0 && ((pluginDlgInfoPtr)s0)->magic == PLUGIN_MAGIC)
			{
				plugsPtr plug = GetPluginWindowOwner(ContextWindow);
				pMenuItemSelectedData p;
				
				p.contextWindow = ContextWindow;
				p.menuID = 258;
				p.itemID = item;
				
				runIndPlugin(plug, pMenuItemSelectedMessage, &p);
			}
			
			return; //don't know what to do.
		}
	}
	
	if(te || we)
	{
		switch(item)
		{
			case 1:
				if(we)
				{
					if(MWActive && MWActive->winType == textWin)
						WEUndo(MWActive->we);
					else if(!noFloatingInput)
						WEUndo(ILGetWE());
				}
				break;
				
			case 3: //cut
				if(iwFront)
					WECut(ILGetWE());
				else if(mwFront && MWActive->winType==textWin)
					WECut(we);
				else if(otherFront)
				{
					TECut(te);
					ZeroScrap();
					TEToScrap();
				}
				break;
			
			case 4: //copy
				if(otherFront)
				{
					TECopy(te);
					ZeroScrap();
					TEToScrap();
				}
				else
					WECopy(we);
				break;
			
			case 5: //paste
				if(mwFront || iwFront)
				{
					if(!iwFront && MWActive->winType == textWin)
						WEPaste(MWActive->we);
					else
					{
						WEReference il = ILGetWE();
						
						if(il)
						{
							WEPaste(il);
							processPaste(MWActive, false);
						}
					}
				}
				else if(otherFront)
					TEPaste(te);
				break;
			
			case 6: //clear
				if(otherFront)
					TEDelete(te);
				else
				{
					WEGetSelection(&s0, &s1, we);
					if(s0==s1)
					{
						WEDeactivate(we);
						WESetSelection(0, 0x7FFFFF, we);
					}
					WEDelete(we);
					WEActivate(we);
				}
				break;
			
			case 7: //select all
				if(te)
					TESetSelect(0, 32767, te);
				else if(we)
					WESetSelection(0, 0x7FFFFFF, we);
				break;
			
			case 9:	
				DoFind(false);
				break;
			case 10:
				DoFind(true);
				break;
		}
	}
}

pascal void wmenu(const short item)
{
	if(item == wConsoleItem)
	{
		if(FrontNonFloater() == consoleWin->w && IsVisible(consoleWin->w))
		{
			WHide(consoleWin->w);
			mainPrefs->consoleOpen=0;
		}
		else
		{
			 if(!IsVisible(consoleWin->w)) //not front and visible
			 	WShow(consoleWin->w);
			WSelect(consoleWin->w);
			mainPrefs->consoleOpen=1;
		}
	}
	else //check for service items
	{
		if(wMenuServices && item <= wLastServiceItem)
		{
			pServiceWindowMenuData dp;
			
			wMSDataPtr service = &(**wMenuServices).service[item - wFirstServiceItem];
			dp.key = service->key;
			dp.menuItem = item;
			runIndPlugin(service->pluginRef, pServiceWindowMenu, &dp);
		}
		else if(item == wNextWindowItem)
			DoCycleCommand(true);
		else if(item == wPrevWindowItem)
			DoCycleCommand(false);
		else if(item == wCascadeItem)
			DoCascadeWindows();
		else if(item == wTileItem)
			DoTileWindows();
		else
		{
			if(item<windowsStart2)
			{
				//NOW Menus menu selection workaround
				//What should this be normally?
				if(item-windowsStart <= wmItems.channelsLength)
					WSelect((**wmItems.channels).mw[item-windowsStart-1]->w);
			}
			else
			{
				if(item-windowsStart2 <= wmItems.restLength)
					WSelect((**wmItems.rest).mw[item-windowsStart2-1]->w);
			}
		}	
	}
}

inline void menuFonts(short item)
{
	Str255 s;
	short i;
	long l;
	
	GetMenuItemText(gFontsMenu, item, s);
	if(item>fontsBegin)
	{
		if(MWActive)
		{
			GetFNum(s, &i);
			MWSetFontSize(MWActive, i, -1);
			WEActivate(MWActive->we);
		}
	}
	else
	{
		StringToNum(s, &l);
		if(MWActive)
		{
			MWSetFontSize(MWActive, -1, l);
			WEActivate(MWActive->we);
		}
	}
}

pascal void MenuConnectionList(short item)
{
	short i;
	Str255 s;
	linkPtr link;
	
	if(item)
	{
		if(!allowConnections)
		{
			i=InitConnections();
			if(i)
			{
				NumToString(i, s);
				ParamText(s, "\p", "\p", "\p");
				Alert(129, 0);
				allowConnections=0;
				return;
			}
			else
				allowConnections=1;
		}
		
		link = firstLink;
		while(--item)
			link = link->next;
		
		if(OpenConnection(link))
		{
			InvalTarget(&CurrentTarget);
			CurrentTarget.bad=0;
			CurrentTarget.link=link;
		}
	}	
}

pascal void MenuSignoffConnectionList(short item)
{
	if(item)
	{
		linkPtr link = firstLink;
		
		while(--item)
			link = link->next;
		
		if(link->serverStatus==S_CONN)
			DoSignoff(link, 0);
		
		if(link->identConn)
			deleteConnection(&link->identConn);

		if(CurrentTarget.link == link)
			InvalTarget(&CurrentTarget);
		UpdateStatusLine();
	}
}

static pascal void MenuAppleURL(short item)
{
	Str255 url;
	
	if(internetConfig)
	{
		//get url
		GetIntString(url, spAppleURL, item * 2);
		
		if(OpenURL(url))
		{
			//error
		}
	}
	else
	{
	}
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

					upp = NewControlActionProc(MWVScrollTrack);
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

static pascal void doMenuEvent(long menuitem, const EventRecord *e)
{
	int menuNum, itemNum;
	
	menuNum = menuitem >> 16;
	itemNum = menuitem & 0x0000FFFF;
	
	if(menuNum)
	{
		switch(menuNum)
		{
			case appleMenu:
				if(itemNum==1)
					DoAbout();
			#if !TARGET_CARBON
				else
					if(itemNum>applMItems)
					{
						Str255 s;
						
						GetMenuItemText(gAppleMenu, itemNum, s);
						OpenDeskAcc(s);
					}
			#endif
				break;
			
			case FileMenu:
				MenuFILE(itemNum);
				break;
			case EditMenu:
				menuEdit(itemNum);
				break;
			case windowMenu:
				wmenu(itemNum);
				break;
			case shortcutsMenu:
				ShortcutsMenu(itemNum);
				break;
			case commandsMenu:
				menuCommands(itemNum, e->modifiers);
				break;
			case fontsMenu:
				menuFonts(itemNum);
				break;
			
			case ConnectionListMenu:
				MenuConnectionList(itemNum);
				break;
			case SignoffConnectionListMenu:
				MenuSignoffConnectionList(itemNum);
				break;
			
			case AppleURLMenu:
				MenuAppleURL(itemNum);
				break;
			
		#if !TARGET_CARBON
			case kHMHelpMenuID:
				ShowHelp(itemNum - defaultHelpItems);
				break;
		#endif
		}
	}
	HiliteMenu(0);
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

static pascal void doMenuClick(const EventRecord *e)
{
	static previousFontCheck = 0;
	static previousSizeCheck = 0;
	Str255 s, s2;
	MenuHandle m;
	int x, y;
	long l;
	WindowPtr w;
	
	//Update file menu
	SetMenuItemText(gFileMenu, 6, GetIntStringPtr(spCM, smSavePreferences + (MWActive && MWActive->winType == textWin)));
	
	//Update edit menu
	if(MWActive)
	{
		EnableMenuItem(gEditMenu, 9);
		EnableMenuItem(gEditMenu, 10);
	}
	else
	{
		DisableMenuItem(gEditMenu, 9);
		DisableMenuItem(gEditMenu, 10);
	}
	
	//Update fonts menu, but only if we have one..
	if(MWActive && gFontsMenu)
	{
		m = gFontsMenu;
		
		if(previousSizeCheck)
			CheckMenuItem(m, previousSizeCheck, false);
		
		for(x=1;x<fontsBegin;x++)
		{
			GetMenuItemText(m, x, s);
			StringToNum(s, &l);
			if(l == MWActive->size)
			{
				CheckMenuItem(m, x, true);
				previousSizeCheck = x;
				break;
			}
		}
		
		if(previousFontCheck)
			CheckMenuItem(m, previousFontCheck, false);
		
		GetFontName(MWActive->font, s);
		y=CountMenuItems(m);
		for(x=fontsBegin+1;x<=y;x++)
		{
			GetMenuItemText(m, x, s2);
			if(pstrcmp(s, s2))
			{
				previousFontCheck = x;
				CheckMenuItem(m, x, true);
				break;
			}
		}
	}
	
	//Update window menu
	if((w = FrontNonFloater())!=0 && (w=GetNextWindow(w))!=0 && IsVisible(w))
	{
		EnableMenuItem(gWindowMenu, wPrevWindowItem);
		EnableMenuItem(gWindowMenu, wNextWindowItem);
	}
	else
	{
		DisableMenuItem(gWindowMenu, wPrevWindowItem);
		DisableMenuItem(gWindowMenu, wNextWindowItem);
	}
	CheckMenuItem(gWindowMenu, wConsoleItem, IsVisible(consoleWin->w));
	doMenuEvent(MenuSelect(e->where), e);
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
			doMenuClick(e);
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

#if __POWERPC__
					case inProxyIcon:
						inProxyHandler(e);
						break;
#endif					
				}
			}
	}
}

static pascal void doTCPEvent(CEPtr c)
{
	connectionPtr conn;
	
	conn=findConnectionSock(c->connection);
	if(conn)
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
						FindAddress(&conn->private_socket, link->conn->name);
						
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
				return;
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
							if(connection3(link, conn->connType == connIRC))
								if(conn->connType == connSOCKS)
									conn->InputFunc(c, conn);
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
				return;
			}
		}
		
		conn->InputFunc(c, conn);
	}
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
				doMenuEvent(l, e);
		}
		return;
	}
	else if(IsDialogEvent(e))
		if(doDialogEvent(e))
			return;
	
	switch(e->what)
	{
		case nullEvent:
			if(gSoundIsFinished)
				AsyncSoundCheck();

			if(!inBackground)
			{
				if(MWActive && MWActive->winType == textWin && !iwFront)
					WEIdle(0, MWActive->we);
				else
					if(!inputLine.lock)
						WEIdle(0, ILGetWE());

				checkCursorFocus(e);
				CMSetCursor();
			}
			idlePlugins(e);
			CheckMem();
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

	WaitNextEvent(-1, &e, 1, mouseRgn);
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

static pascal long GrowZoneProc(long needed)
{
#pragma unused(needed)
#if !__POWERPC__
	long oldA5 = SetCurrentA5();
#endif
	char ret  = CheckMem();
	
#if !__POWERPC__
	SetA5(oldA5);
#endif
	return ret;
}

inline void FontsMenuInit(void);
inline void FontsMenuInit(void)
{
	if(mainPrefs->noFontsMenu)
	{
		DeleteMenu(fontsMenu);
		gFontsMenu = 0;
	}
	else
	{
		MenuHandle mh = GetMenuHandle(fontsMenu);
		gFontsMenu = mh;
		fontsBegin=CountMenuItems(mh);
		AppendResMenu(mh, 'FONT');
	}
}

pascal void ApplInit(void)
{
	MenuHandle m;
	
	mouseRgn=NewRgn();
	SetRectRgn(mouseRgn, -32767, -32767, 32766, 32766);
	
	SetMenuBar(GetNewMBar(menuBar));
	gAppleMenu = m = GetMenuHandle(appleMenu);
	applMItems=CountMenuItems(m);
#if !TARGET_CARBON
	AppendResMenu(m, 'DRVR');
#endif
	
	gEditMenu = GetMenuHandle(EditMenu);
	gFileMenu = GetMenuHandle(FileMenu);
	FontsMenuInit();
	gShortcutsMenu = GetMenuHandle(shortcutsMenu);
	gWindowMenu = GetMenuHandle(windowMenu);

	SetGrowZone(NewGrowZoneProc(GrowZoneProc));

	FlushEvents(everyEvent, 0);
	
	ActivateWindowProcPtr=&WindowActivate;
	scb=NewSndCallBackProc(AsyncSoundCallback);
}

static pascal void FinishStaleConnections(void)
{
	connectionPtr conn, conn2;
	int x=0, y=0, z=0;
	LongString ls;
	Str255 s1;
	
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
	
	if(debugOn)
	{
		NumToString(x, s1);
		LSConcatStrAndStr(s1, "\p connections", &ls);
		LineMsg(&ls);

		NumToString(y, s1);
		LSConcatStrAndStr(s1, "\p stale connections", &ls);
		LineMsg(&ls);

		NumToString(z, s1);
		LSConcatStrAndStr(s1, "\p IRC conections with no channels", &ls);
		LineMsg(&ls);
	}

	if(fConn) //Now, if there are still stale connections left, let them time out.
	{
		DialogPtr d = GetNewDialog(140, 0, (WindowPtr)-1);
		long time = now;
		
		while(fConn) //while there's still a (stale) connection
		{
			connectionEventRecord connEvt;
			EventRecord e;
			
			WaitNextEvent(everyEvent, &e, 1, mouseRgn);
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
			
			if(time - now> 5 && fConn)
			{
				ShowWindow(GetDialogWindow(d));
				DrawDialog(d);
			}
		}
		
		DisposeDialog(d);
	}
}

pascal void ApplExit(void)
{
	runAllPlugins(pQuitMessage, 0);
	
	if(allowConnections)
	{
		FinishStaleConnections();
		FinishEverything();
	}
	
	StopIC();
	
	RemoveDragHandlers();
	UnregisterAppearanceClient();
	
	#if __POWERPC__
	if(hasNav)
		NavUnload();
	#endif
	DisposeAllSmartScrolls();
	ExitToShell();
}