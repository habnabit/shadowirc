/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.com

	Parts updated for Carbon by Sean McGovern <smcgovern@users.sourceforge.net>

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

#include "LongStrings.h"
#include "StringList.h"
#include "AppearanceHelp.h"
#include "MsgWindows.h"
#include "MWPanes.h"
#include "WindowList.h"
#include "channels.h"
#include "IRCGlobals.h"
#include "connections.h"
#include "IRCChannels.h"
#include "DCC.h"
#include "utils.h"
#include "IRCAux.h"
#include "IRCCommands.h"
#include "Inline.h"
#include "Floaters.h"
#include "FilesMan.h"
#include "Inputline.h"
#include "plugins.h"
#include "IRCInput.h"
#include "CMenus.h"
#include "TextWindows.h"
#include "ApplBase.h"
#include "TextManip.h"
#include "MenuCommands.h"

static void MWWidgetInternalDraw(mwWidgetPtr o, char winActive);
inline void DrawTopic(mwWidgetPtr o, char winActive);
static void DrawChannelModes(mwWidgetPtr o, char winActive);
static void MWStatusLineClickInternal(mwWidgetPtr o, Point p);
inline char TrackTopicMouse(const Rect *cr);
static void DoTopicWidget(mwWidgetPtr o);
static void DoModesWidget(mwWidgetPtr o, Point p);
static void StandardIconWidget(mwWidgetPtr o, CIconHandle icon[]);
static pascal OSStatus TopicWidgetDialogEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData);
static void TopicWindowSet(WindowRef dlgWindow, channelPtr ch);

target CurrentTarget = {0, 0, 0, 0, 1, 0, 0};

static MWPtr hel=0;
static short curHelpItem = 0;

static CIconHandle giCMWidgetIcon[4], giHelpWidgetIcon[4], giLinkWidgetIcon[4];

inline void linkWinAdd(MWPtr win, int i);
inline int linkWinNull(linkPtr link);
inline void linkWinDel(MWPtr win);

static char TrackCSMouse(Point p, const Rect *cr, short c, char modePlus);
static void RollCSMouseCall(short l, char mc, char cl, Rect *cr, Point p, LongString *ls, char *b);

#define kNibChannel CFSTR("channel")
#define kNibWinChannelTopic CFSTR("Channel Topic")

enum {
	kSIRCTopicOKControlID = 1,
	kSIRCTopicCancelControlID,
	kSIRCTopicTextControlID = 4,
	kSIRCTopicSetControlID,
	kSIRCTopicOpsControlID
};

enum {
	kHICommandTopicText = 'Topc'
};

typedef struct TopicWindowInfo {
	channelPtr ch;
	long time;
	short topicLen;
	char hadOps;
} TopicWindowInfo, *TopicWindowInfoPtr;

enum {
	kTopicWindowDataProperty = 'TOPC',
	kTopicWindowProperty = 'TOPW'
};


inline void linkWinAdd(MWPtr win, int i)
{
	linkPtr link = win->link;
	
	if(!link->windows[i])
		link->windows[i] = win;
}

inline int linkWinNull(linkPtr link)
{
	int x;

	for(x=0;x<maxLinks;x++)
		if(!link->windows[x])
			return x;
	
	return maxLinks-1;
}

inline void linkWinDel(MWPtr win)
{
	int x;
	linkPtr link = win->link;
	
	for(x=0;x<maxLinks;x++)
		if(win == link->windows[x])
		{
			link->windows[x]=0;
			return;
		}
}

#pragma mark -

//Stick the init here since it's only used here.

enum iconIDs {
	kIconWidgetUp = 0,
	kIconWidgetDisabled = 1,
	kIconWidgetDown = 2,
	kIconWidgetDownDisabled = 3,
	
	kiCMWidgetUp = 128,
	kiCMWidgetDisabled = 129,
	kiCMWidgetDown = 130,
	
	kiHelpWidgetUp = 132,
	kiHelpWidgetDisabled = 133,
	kiHelpWidgetDown = 134,

	kiLinkWidgetUp = 136,
	kiLinkWidgetDisabled = 137,
	kiLinkWidgetDown = 138
};

void GetIcons(void)
{
	giCMWidgetIcon[kIconWidgetUp] = GetCIcon(kiCMWidgetUp);
	giCMWidgetIcon[kIconWidgetDownDisabled] = giCMWidgetIcon[kIconWidgetDisabled] = GetCIcon(kiCMWidgetDisabled);
	giCMWidgetIcon[kIconWidgetDown] = GetCIcon(kiCMWidgetDown);

	giHelpWidgetIcon[kIconWidgetUp] = GetCIcon(kiHelpWidgetUp);
	giHelpWidgetIcon[kIconWidgetDownDisabled] = giHelpWidgetIcon[kIconWidgetDisabled] = GetCIcon(kiHelpWidgetDisabled);
	giHelpWidgetIcon[kIconWidgetDown] = GetCIcon(kiHelpWidgetDown);

	giLinkWidgetIcon[kIconWidgetUp] = GetCIcon(kiLinkWidgetUp);
	giLinkWidgetIcon[kIconWidgetDownDisabled] = giLinkWidgetIcon[kIconWidgetDisabled] = GetCIcon(kiLinkWidgetDisabled);
	giLinkWidgetIcon[kIconWidgetDown] = GetCIcon(kiLinkWidgetDown);
}

#pragma mark -

void InvalTarget(targetPtr targ)
{
	targ->type=targInvalid;
	
	targ->mw=0;
//	targ->link=0;
	targ->inactive=0;
	targ->chan=0;
	targ->dcc=0;
	targ->bad=1;
}

void SetTarget(MWPtr w, targetPtr targ)
{
	targ->mw=w;

	targ->type=w->winType;
	targ->inactive=w->inactive;
	targ->bad=0;
	
	switch(targ->type)
	{
		case targConsole:
		case targHelp:
		case targText:
			targ->link = CurrentTarget.link;
			if(targ->link == 0)
				targ->link = firstLink;
			targ->bad=1;
			targ->chan=0;
			targ->dcc=0;
			break;
			
		case targPlugin:
			targ->chan=0;
			targ->dcc=0;
			targ->link = w->link;
			break;
		
		case targChannel:
			targ->chan=MWGetChannel(w);
			targ->link=w->link;
			targ->dcc=0;
			break;
		
		case targQuery:
			targ->chan=0;
			targ->dcc=0;
			targ->link=w->link;
			break;
			
		case targDCC:
			targ->dcc=MWGetDCC(w);
			targ->link=w->link;
			targ->chan=0;
			break;
	}
	
	if(!targ->bad)
		targ->bad=targ->inactive;
}

static char TrackCSMouse(Point p, const Rect *cr, short c, char modePlus)
{
	char b=0, pir;
	Point m;
	RGBColor saved;
	MouseTrackingResult trackingResult;
	
	GetForeColor(&saved);
	
	GetMouse(&m);
	do
	{
		pir = PtInRect(m, cr);
		if(pir != b)
		{
			b = pir;
			EraseRect(cr);
			
			if(b) //was depressed
			{
				RGBForeColor(&black);
				MoveTo(p.h +1, p.v +1);
			}
			else
			{
				if(!modePlus)
					RGBForeColor(&white);
				else
					RGBForeColor(&HalfGrey);
				MoveTo(p.h +1, p.v +1);
				DrawChar(c);

				RGBForeColor(&black);
				MoveTo(p.h, p.v);
			}
			
			DrawChar(c);
		}
		TrackMouseLocation(NULL, &m, &trackingResult);
	} while(trackingResult != kMouseTrackingMouseReleased);
	
	EraseRect(cr);
	//released
	if(!modePlus)
		RGBForeColor(&white);
	else
		RGBForeColor(&HalfGrey);
	MoveTo(p.h +1, p.v +1);
	DrawChar(c);

	RGBForeColor(&black);
	MoveTo(p.h, p.v);
	DrawChar(c);
	
	RGBForeColor(&saved);
	return b;
}

static void RollCSMouseCall(short l, char mc, char cl, Rect *cr, Point p, LongString *ls, char *b)
{
	p.h=cr->left=l;
	cr->right= p.h + CharWidth(cl) + 1;
	*b=TrackCSMouse(p, cr, cl, mc);
	if(*b)
	{
		LSAppend1(*ls, ' ');
		if(mc)
			LSAppend1(*ls, '-')
		else
			LSAppend1(*ls, '+');
		LSAppend1(*ls, cl + 0x20);
	}
}

inline char TrackTopicMouse(const Rect *cr)
{
	char b=0, pir;
	Point m;
	MouseTrackingResult trackingResult;
	
	GetMouse(&m);
	do
	{
		pir = PtInRect(m, cr);
		if(pir != b)
		{
			b = pir;
			InvertRect(cr);
		}
		TrackMouseLocation(NULL, &m, &trackingResult);
	} while(trackingResult != kMouseTrackingMouseReleased);
	
	if(b)
		InvertRect(cr);
	return b;
}

#pragma mark -

static TopicWindowInfoPtr NewTopicWindowInfo(WindowPtr parent, WindowPtr sheet, channelPtr ch)
{
	TopicWindowInfoPtr twi = (TopicWindowInfoPtr)NewPtrClear(sizeof(TopicWindowInfo));
	
	SetWindowProperty(sheet, kApplicationSignature, kTopicWindowDataProperty, sizeof(twi), &twi);
	SetWindowProperty(parent, kApplicationSignature, kTopicWindowProperty, sizeof(WindowPtr), &sheet);
	
	twi->ch = ch;

	if(ch->hasOps || ch->hasHalfOps || !ch->modes[modeT])
		twi->hadOps = true;
	else
		twi->hadOps = false;
	
	return twi;
}

static TopicWindowInfoPtr GetTopicWindowInfo(WindowPtr win)
{
	TopicWindowInfoPtr twi;
	UInt32 actualSize;
	
	GetWindowProperty(win, kApplicationSignature, kTopicWindowDataProperty, sizeof(twi), &actualSize, (void*)&twi);
	
	return twi;
}

static void DeleteTopicWindowInfo(WindowPtr parent, WindowPtr sheet)
{
	TopicWindowInfoPtr twi = GetTopicWindowInfo(sheet);
	
	RemoveWindowProperty(sheet, kApplicationSignature, kTopicWindowDataProperty);
	RemoveWindowProperty(parent, kApplicationSignature, kTopicWindowProperty);
	
	DisposePtr((void*)twi);
}

void ChCloseTopicWindow(channelPtr ch)
{
	WindowPtr parent = ch->window->w;
	WindowPtr sheet;
	UInt32 actualSize;
	OSStatus status;
	
	status = GetWindowProperty(parent, kApplicationSignature, kTopicWindowProperty, sizeof(WindowPtr), &actualSize, (void*)&sheet);
	
	if(!status && sheet)
	{
		HideSheetWindow(sheet);
		DeleteTopicWindowInfo(parent, sheet);
		DisposeWindow(sheet);
	}
}

#pragma mark -

static void _TopicWindowLengthDisplay(WindowRef dlgWindow, channelPtr ch)
{
	ControlRef topicOpsControl = NULL;
	ControlID topicOpsControlID = { kApplicationSignature, kSIRCTopicOpsControlID };
	int maxLen = HTFindNumericDefault(ch->link->serverOptions, "\pTOPICLEN", 0);
	TopicWindowInfoPtr twi = GetTopicWindowInfo(dlgWindow);
	
	GetControlByID(dlgWindow, &topicOpsControlID, &topicOpsControl);

	if(!maxLen)
	{
		SetControlData(topicOpsControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), CFSTR(""));
		DrawOneControl(topicOpsControl);
	}
	else
	{
		CFStringRef theString;
		LongString ls;
		Str255 st, st2;
		
		GetControlData(topicOpsControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString, NULL);

		CFStringGetPascalString(theString, st, sizeof(Str255), CFStringGetSystemEncoding());
		twi->topicLen = st[0];
		NumToString(st[0], st);
		NumToString(maxLen, st2);

		LSParamString(&ls, GetIntStringPtr(spInfo, sTopicLength), st, st2, 0, 0);
		LSMakeStr(ls);

		theString = CFStringCreateWithPascalString(NULL, ls.data,kCFStringEncodingMacRoman);
		SetControlData(topicOpsControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString);
		CFRelease(theString);
		DrawOneControl(topicOpsControl);
	}
}

static void TopicWindowSet(WindowRef dlgWindow, channelPtr ch)
{
	ControlRef topicSetControl = NULL, topicTextControl = NULL, topicOpsControl = NULL, topicOKControl = NULL, topicCancelControl = NULL;
	ControlID topicSetControlID = { kApplicationSignature, kSIRCTopicSetControlID };
	ControlID topicTextControlID = { kApplicationSignature, kSIRCTopicTextControlID };
	ControlID topicOpsControlID = { kApplicationSignature, kSIRCTopicOpsControlID };
	ControlID topicOKControlID = { kApplicationSignature, kSIRCTopicOKControlID };
	ControlID topicCancelControlID = { kApplicationSignature, kSIRCTopicCancelControlID };
	CFStringRef theString;
	Str255 s1, s2;		/* can these eventually go byebye? */
	TopicWindowInfoPtr twi = GetTopicWindowInfo(dlgWindow);
	
	GetControlByID(dlgWindow, &topicSetControlID, &topicSetControl);
	GetControlByID(dlgWindow, &topicTextControlID, &topicTextControl);
	GetControlByID(dlgWindow, &topicOpsControlID, &topicOpsControl);
	GetControlByID(dlgWindow, &topicOKControlID, &topicOKControl);
	GetControlByID(dlgWindow, &topicCancelControlID, &topicCancelControl);

	twi->time = ch->topicSetOn;
	if(ch->topicSetOn) //if there's a time the topic was set
	{
		DateString(ch->topicSetOn, longDate, s1, 0);
		TimeString(ch->topicSetOn, true, s2, 0);
		ParamText(ch->chName, ch->topicSetBy, s1, s2);
	}
	else
	{
		if(!ch->topic[0]) //if there is a topic set
		{
			ParamText(ch->chName, "\p", "\p", "\p");
			theString = CFStringCreateWithPascalString(NULL, GetIntStringPtr(spError, sNoTopic), kCFStringEncodingMacRoman);
			SetControlData(topicSetControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString);
			CFRelease(theString);
			DrawOneControl(topicSetControl);
		}
		else
			theString = CFSTR("");
		
		SetControlData(topicSetControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString);
		DrawOneControl(topicSetControl);
	}
	
	theString = CFStringCreateWithPascalString(NULL, ch->topic, kCFStringEncodingMacRoman);
	SetControlData(topicTextControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString);
	DrawOneControl(topicTextControl);

	if(twi->hadOps)
		_TopicWindowLengthDisplay(dlgWindow, twi->ch);
	else
	{
		theString = CFStringCreateWithPascalString(NULL, GetIntStringPtr(spError, sNoOpsCantTopic), kCFStringEncodingMacRoman);
		SetControlData(topicOpsControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString);
		CFRelease(theString);
		DrawOneControl(topicOpsControl);
		DeactivateControl(topicOKControl);
	}
	
	SetWindowCancelButton(dlgWindow, topicCancelControl);
	SetWindowDefaultButton(dlgWindow, topicCancelControl);
}

static pascal OSStatus TopicWidgetDialogEventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	OSStatus result = eventNotHandledErr;
	WindowRef sheet = NULL;
	ControlRef control = NULL, topicTextControl = NULL, topicOKControl = NULL;
	ControlID topicTextControlID = { kApplicationSignature, kSIRCTopicTextControlID };
	ControlID topicOKControlID = { kApplicationSignature, kSIRCTopicOKControlID };
	CFStringRef theString;
	Str255 s1;
	LongString ls;	/* BLECH! */
	channelPtr ch;
	UInt32 cmd;
	TopicWindowInfoPtr twi;
	
	GetEventParameter(event, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &control);
	GetControlCommandID(control, &cmd);

	sheet =(WindowRef)userData;
	
	twi = GetTopicWindowInfo(sheet);
	ch = twi->ch;

	switch(cmd)
	{
		case kHICommandTopicText:		// find out if there's a way to see if text has been typed or not
			if(twi->hadOps)
			{
				GetControlByID(sheet, &topicOKControlID, &topicOKControl);
				SetWindowDefaultButton(sheet, topicOKControl);
			}
			break;
		
		case kHICommandOK:
			GetControlByID(sheet, &topicTextControlID, &topicTextControl);
			GetControlData(topicTextControl, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &theString, NULL);
			CFStringGetPascalString(theString, s1, sizeof(Str255), CFStringGetSystemEncoding());
			if(s1[0])
			{
				LSStrCat4(&ls,"\pTOPIC ", ch->chName, "\p :", s1);
				SendCommand(ch->link, &ls);
			}

		case kHICommandCancel:
			ChCloseTopicWindow(ch);
			break;
	}
	
	return result;
}

void ChTopicWindow(channelPtr ch)
{
	IBNibRef mainNibRef;
	WindowRef channelTopicSheet = NULL;
	static EventHandlerUPP ctUPP = NULL;
	const EventTypeSpec ctSpec = { kEventClassControl, kEventControlHit };
	OSStatus status;
	WindowPtr parent;
	
	if(!ctUPP)
		ctUPP = NewEventHandlerUPP(TopicWidgetDialogEventHandler);
	
	status = CreateNibReference(kNibChannel, &mainNibRef);
	require_noerr(status, CantFindDialogNib);

	status = CreateWindowFromNib(mainNibRef, kNibWinChannelTopic, &channelTopicSheet);
	require_noerr(status, CantCreateDialogWindow);

	DisposeNibReference(mainNibRef);

	status = InstallWindowEventHandler(channelTopicSheet, ctUPP, 1, &ctSpec,(void *)channelTopicSheet, NULL);
	require_noerr(status, CantInstallDialogHandler);
	
	parent = ch->window->w;
	
	NewTopicWindowInfo(parent, channelTopicSheet, ch);
	TopicWindowSet(channelTopicSheet, ch);

	ShowSheetWindow(channelTopicSheet, parent);
	SelectWindow(channelTopicSheet);

CantFindDialogNib:
CantCreateDialogWindow:
CantInstallDialogHandler:
	return;
}

static void DoTopicWidget(mwWidgetPtr o)
{
	Rect r;
	channelPtr ch = MWGetChannel(o->mw);
	
	if(ch && ch->chName[1]!='+')
	{
		r=o->drawArea;
		r.top++;
		r.left--;
		r.right+=2;
		if(TrackTopicMouse(&r))
			ChTopicWindow(ch);
	}
}

void DoModeLWindow(channelPtr ch, LongString *ls)
{
	char b;
	DialogPtr d;
	short i;
	Str255 s;
	
	EnterModalDialog();
	d=GetNewDialog(10001, 0,(WindowPtr)-1);
	NumToString(ch->limit, s);
	SetText(d, 4, s);
	ParamText(ch->chName, "\p", "\p", "\p");
	SelectDialogItemText(d, 4, 0, 255);
	SetupModalDialog(d, 1, 2);
	b=0;
	do
	{
		ModalDialog(StdDlgFilter, &i);
		if((i==1) || (i==2))
			b=1;
	} while(!b);
	
	b=0;
	if(i==1)
	{
		GetText(d, 4, s);
		if(s[0])
		{
			long l;
			StringToNum(s, &l);
			if(l>0)
			{
				LSAppend1(*ls, ' ');
				LSConcatLSAndStr(ls, s, ls);
				b=1;
			}
		}
	}
	else //canceled
		ls->len = 0;
	DisposeDialog(d);
	FinishModalDialog();
}

void DoModeKWindow(channelPtr ch, LongString *ls)
{
	char b;
	DialogPtr d;
	short i;
	Str255 s;
	
	b=0;
	EnterModalDialog();
	d=GetNewDialog(10002, 0, (WindowPtr)-1);
	SetText(d, 4, ch->key);
	ParamText(ch->chName, "\p", "\p", "\p");
	SelectDialogItemText(d, 4, 0, 255);
	SetupModalDialog(d, 1, 2);
	do
	{
		ModalDialog(StdDlgFilter, &i);
		if((i==1) || (i==2))
			b=1;
	} while(!b);
	
	b=0;
	if(i==1)
	{
		GetText(d, 4, s);
		if(s[0])
		{
			LSAppend1(*ls, ' ');
			LSConcatLSAndStr(ls, s, ls);
			b=1;
		}
	}
	DisposeDialog(d);
	FinishModalDialog();
}

static const char modelist[] = "TNIMPSLK";
static void DoModesWidget(mwWidgetPtr o, Point p)
{
	LongString ls;
	Rect cr;
	char b;
	MWPtr mw = o->mw;
	channelPtr ch = MWGetChannel(mw);
	linkPtr link = ch->link;
	int x;
	short off = p.h - o->drawArea.left;
	short *modePt = (short*)o->data;
	char *themode = ch->modes;
	
	cr.bottom = o->drawArea.bottom - 3;
	cr.top = o->drawArea.top + 2;
	
	//Find where the user clicked
	LSConcatStrAndStr("\pMODE ", ch->chName, &ls);
	if(ch->hasOps && p.v < o->drawArea.bottom -2 && p.v > o->drawArea.top + 2 &&
		off > 3 && o->drawArea.right - p.h > 3) //then they clicked in the area, AND they ahve ops
	{
		//Now, we need to find what widget they clicked on. Since we have
		//left edges, we start from the right and tunnel back
		for(x=7;x>=0;x--)
			if(p.h >= modePt[x])
			{
				TextSize(10);
				TextFace(0);
				p.v = o->drawArea.bottom - 3;
				RollCSMouseCall(modePt[x], themode[x], modelist[x], &cr, p, &ls, &b);
				if(b)
				{
					if(x == 7) // mode k
					{
						if(!themode[7]) //we are -k
							DoModeKWindow(ch, &ls);
						else //we are +k
						{
							//tack on ' password' to ls
							LSAppend1(ls, ' ');
							LSConcatLSAndStr(&ls, ch->key, &ls);
						}
					}
					else if(x == 6 && !themode[6]) // mode l
						DoModeLWindow(ch, &ls);
					
					if(ls.len)
						HandleCommand(link, &ls);
				}
				break;
			}
	}
}

static void StandardIconWidget(mwWidgetPtr o, CIconHandle icon[])
{
	Rect r;
	
	if(o)
	{
		MoveTo(o->drawArea.right, o->drawArea.bottom-15);
		LineTo(o->drawArea.right, o->drawArea.bottom);

		r.top = o->drawArea.top + 2;
		r.left = o->drawArea.left;
		r.bottom = o->drawArea.bottom - 1;
		r.right = o->drawArea.right - 1;
		
		if(IsWindowHilited(o->mw->w))
			PlotCIcon(&r, icon[0]);
		else
			PlotCIcon(&r, icon[1]);
	}
}

static void MWStatusLineClickInternal(mwWidgetPtr o, Point p)
{
	MWPtr mw;
	short i;
	
	mw = o->mw;
	switch(o->type)
	{
		case mwTopicWidget:
			if(mw->winType == textWin)
				TWStatusClick(o, p);
			else
				DoTopicWidget(o);
			break;
		
		case mwModesWidget:
			DoModesWidget(o, p);
			break;

		case mwCMWidget:
			LocalToGlobal(&p);
			p.h+=2;

			StandardIconWidget(o, &giCMWidgetIcon[kIconWidgetDown]);
			
			DetermineCM(mw->w, p, false);
			if(mw->magic==MW_MAGIC)
				StandardIconWidget(o, giCMWidgetIcon);
			break;
			
		case mwHelpWidget:
			LocalToGlobal(&p);

			StandardIconWidget(o, &giHelpWidgetIcon[kIconWidgetDown]);
			PopUpMenuSelect(menuHelpWidget, p.v, p.h, curHelpItem);
			StandardIconWidget(o, giHelpWidgetIcon);
			break;
		
		case mwLinkWidget:
			ConnectionMenuHilites();
			if(CurrentTarget.link)
				i=CurrentTarget.link->linkNum - 1;
			else
				i=0;
			
			LocalToGlobal(&p);
			StandardIconWidget(o, &giLinkWidgetIcon[kIconWidgetDown]);
			PopUpMenuSelect(menuSelectConnectionList, p.v, p.h, i);
			
			DrawMWinStatus(mw);
			StandardIconWidget(o, giLinkWidgetIcon);
			break;
	}
}

void MWStatusClick(MWPtr mw, Point p)
{
	mwWidgetPtr o;
	pMWWidgetClickData pp;
	GrafPtr gp;
	
	GetPort(&gp);
	SetPortWindowPort(mw->w);
	TextFont(3); //get the real geneva font number
	TextSize(9);
	TextFace(0);
	
	linkfor(o, mw->widgetList)
		if(PtInRect(p, &o->drawArea))
		{
			if(o->creator==mwShadowIRCWidget && !o->pluginRef)
				MWStatusLineClickInternal(o, p);
			else
			{
				pp.widget=o;
				pp.where=p;
				runIndPlugin(o->pluginRef, pMWWidgetClickMessage, &pp);
			}
			break;
		}

	SetPort(gp);
}

static void DrawChannelModes(mwWidgetPtr o, char winActive)
{
	channelPtr ch = MWGetChannel(o->mw);
	RGBColor frontRGB;
	RGBColor rgb;
	Point p;
	char *modeflags;
	int x;
	Point pt;
	char clr;

	if(ch)
	{
		short *modeStart = ((mwModesWidgetDataPtr)o->data)->modeStart;

		GetForeColor(&frontRGB);
		modeflags = ch->modes;
		
		GetPen(&p);
		
		//The right border
		MoveTo(o->drawArea.right, o->drawArea.bottom-15);
		LineTo(o->drawArea.right, o->drawArea.bottom);

		p.h+=2;
		MoveTo(p.h+1, p.v+1);
		
		TextSize(10);
		rgb = white;
		if(!ch->hasOps || !winActive)
			LightenColor(&rgb);
		RGBForeColor(&rgb);
		clr = 0;
		
		GetPen(&pt);
		for(x=0;modelist[x];x++)
		{
			if(modeflags[x] != clr)
			{
				clr = modeflags[x];
				if(clr) //mode up, 50% grey bg
					rgb = HalfGrey;
				else
					rgb = white;
					
				if(!ch->hasOps || !winActive)
					LightenColor(&rgb);
				RGBForeColor(&rgb);
			}
			DrawChar(modelist[x]);

			modeStart[x] = pt.h;
			GetPen(&pt);
			MoveTo(pt.h + 1, pt.v);
		}
		
		rgb.red=rgb.green=rgb.blue=0;
		if(!ch->hasOps || !winActive)
			LightenColor(&rgb);
		RGBForeColor(&rgb);
		
		MoveTo(p.h, p.v);
		
		for(x=0;modelist[x];x++)
		{
			DrawChar(modelist[x]);
			GetPen(&pt);
			MoveTo(pt.h + 1, pt.v);
		}

		RGBForeColor(&frontRGB);
	}
	else //not channel
	{
		MWPtr mw = o->mw;
		//kill the widget
		MWDestroyWidget(o);
		DrawMWinStatus(mw);
	}
}

inline void DrawTopic(mwWidgetPtr o, char winActive)
{
	Str255 st;
	Point p2;
	Rect r;
	int x;
	RGBColor rgb, rgb2, back;
	int c;
	MWPtr mw = o->mw;
	channelPtr ch = MWGetChannel(mw);
	ConstStringPtr stp;
	
	if(ch)
	{
		if(ch->topic[0])
		{
			char invSty, inv;
			Style s, s2;

			GetForeColor(&rgb);
			rgb2 = rgb;
			GetBackColor(&back);
			r=o->drawArea;

			stp = GetIntStringPtr(spInfo, sTopicStr);
			TextFace(bold);
			x=StringWidth(stp);
			if(x > o->givenWidth - 2)
			{
				TextFace(0);
				return;
			}
			else
				DrawString(stp);
			TextFace(0);
			GetPen(&p2);
			st[0]=0;

			inv = 0;
			s = 0;
			for(x=1;x<=ch->topic[0];x++)
			{
				switch(ch->topic[x])
				{
					case 3: //color
						if(st[0])
						{
							if(inv)
							{
								RGBForeColor(&back);
								RGBBackColor(&rgb2);
								TextMode(notSrcBic);
							}
							else
							{
								RGBForeColor(&rgb2);
								RGBBackColor(&back);
							}
							GetPen(&p2);
							c = TruncString(r.right-p2.h, st, truncEnd);
							DrawString(st);
							
							if(inv)
								TextMode(srcOr);

							if(c) //then we truncated, so we stop.
								return;
							st[0]=0;
						}
						if(mw->colorMethod==cmIrcle)
						{
							if((x+1 <= ch->topic[0]) && (ch->topic[x+1] != 3))
							{
								x++;
								c=ch->topic[x]-'0';
								//Can't have color less than zero...could crash.
								if((c >= 0 && c<kMaxMacColors) && !mainPrefs->disableColor)
								{
									rgb2 = macColors[c];
									if(!winActive)
										LightenColor(&rgb2);
								}
							}
						}
						else if(mw->colorMethod==cmMIRC)
						{
							
						}
						break;
					
					case C_BOLD: //bold
					case C_INVERSE: //invese
					case C_UNDERLINE: //underline
					case C_NORMAL: //normal
						if(st[0])
						{
							if(inv)
							{
								RGBForeColor(&back);
								RGBBackColor(&rgb2);
								TextMode(notSrcBic);
							}
							else
							{
								RGBForeColor(&rgb2);
								RGBBackColor(&back);
							}
							
							GetPen(&p2);
							c = TruncString(r.right-p2.h, st, truncEnd);
							DrawString(st);
							if(inv)
								TextMode(srcOr);

							if(c) //then we truncated, so we stop.
							{
								RGBForeColor(&rgb);
								RGBBackColor(&back);
								return;
							}
							st[0]=0;
						}
						
						if(!mainPrefs->disableStyles)
						{
							invSty = false;
							switch(ch->topic[x])
							{
								case C_BOLD:
									s2=bold;
									break;
								case C_INVERSE:
									inv=!inv;
									invSty = true;
									break;
								case C_UNDERLINE:
									s2=underline;
									break;
								case C_NORMAL:
									s2=-1;
									RGBForeColor(&rgb);
									rgb2 = rgb;
									RGBBackColor(&back);
									inv = false;
									break;
							}
							
							if(!invSty)
							{
								if(s2==(Style)-1)
									s=0;
								else
									s^=s2;
								TextFace(s);
							}
						}
						break;
					
					default:
						SAppend1(st, ch->topic[x]);
				}
			}

			if(inv)
			{
				RGBForeColor(&back);
				RGBBackColor(&rgb2);
				TextMode(notSrcBic);
			}
			else
			{
				RGBForeColor(&rgb2);
				RGBBackColor(&back);
			}
			
			GetPen(&p2);
			TruncString(r.right-p2.h, st, truncEnd);
			DrawString(st);

			if(inv)
				TextMode(srcOr);

			if(ch->topicSetBy[0])
			{
				RGBForeColor(&rgb);
				TextFace(bold);
				GetPen(&p2);
				stp = GetIntStringPtr(spInfo, sTopicSetBy);
				if(StringWidth(stp) < r.right-p2.h)
				{
					DrawString(stp);
					TextFace(0);
					pstrcpy(ch->topicSetBy, st);
					GetPen(&p2);
					TruncString(r.right-p2.h, st, truncEnd);
					DrawString(st);
				}
			}
		}
		else
		{
			if(ch->chName[1] == '+') //modeless
				GetIntString(st, spTopic, sModelessChannel);
			else
				GetIntString(st, spInfo, sNoTopicSet);
			x = StringWidth(st);
			c=CharWidth(')');
			if(x > o->givenWidth - 2)
			{
				st[0]--;
				TruncString((-2) + o->givenWidth -2 -c, st, truncEnd);
				SAppend1(st, ')');
			}

			if(o->givenWidth - 2 > 2*c)
				DrawString(st);
		}
		
		RGBBackColor(&back);
	}
	else //inactive channel
	{
		TextFace(bold);
		DrawString(GetIntStringPtr(spInfo, sInactiveChannel));
		TextFace(0);
	}
}

static void DrawTopicWidget(mwWidgetPtr o, char winActive)
{
	MWPtr mw = o->mw;
	ConstStringPtr s;
	
	MoveTo(o->drawArea.left + 2, o->drawArea.bottom-3);
	TextFace(bold);
	switch(mw->winType)
	{
		case conWin:
			DrawString(GetIntStringPtr(spTopic, sConsole));
			TextFace(0);
			if(CurrentTarget.link)
			{
				linkPtr link = CurrentTarget.link;
				
				DrawString("\p: ");
				
				if(link->serverStatus == S_CONN)
					if(link->connectStage == csOnline && link->CurrentNick[0])
					{
						TextFace(bold);
						DrawString(link->CurrentNick);
						TextFace(0);
						if(link->awayString)
							DrawString(GetIntStringPtr(spInfo, sAway));
						DrawString("\p on ");
					}
				
				TextFace(bold);
				DrawString(link->linkPrefs->linkName);
				TextFace(0);
				if(link->serverStatus==S_CONN && link->CurrentServer[0])
				{
					DrawString("\p (");
					DrawString(link->CurrentServer);
					DrawChar(')');
				}
			}
			break;
		
		case helpWin:
			DrawString(GetIntStringPtr(spTopic, sHelp));
			TextFace(0);
			DrawString(GetIntStringPtr(spHelp, curHelpItem));
			break;
		
		case queryWin:
			DrawString(GetIntStringPtr(spTopic, sQuery));
			if(mw->link)
			{
				linkPtr link = mw->link;
				
				TextFace(0);
				DrawString("\p: ");
				DrawString(link->linkPrefs->linkName);
				if(link->serverStatus==S_CONN)
				{
					DrawString("\p (");
					DrawString(link->CurrentServer);
					DrawChar(')');
				}
			}
			break;
		
		case dccWin:
			if(!mw->inactive)
				s = GetIntStringPtr(spTopic, sDCCChat);
			else
				s = GetIntStringPtr(spTopic, sInactiveDCCChat);

			DrawString(s);
			break;
		
		case pluginWin:
		{
			pUIMWInfoRefreshData p;
			//send the plugin a message asking if they want us to display text, or give them the opportunity
			//to do it on their own
			
			p.mw=mw;
			p.displayStr[0]=0;
			p.active=winActive;
			p.rect=o->drawArea;
			runIndPlugin(mw->pluginRef, pUIMWInfoRefresh, &p);
			if(p.displayStr[0])
				DrawString(p.displayStr);
			break;
		}
		
		case chanWin:
			DrawTopic(o, winActive);
			break;
		
		case textWin:
			TWStatus(o, winActive);
			break;
	}
}

static void MWWidgetInternalDraw(mwWidgetPtr o, char winActive)
{
	switch(o->type)
	{
		case mwTopicWidget:
			DrawTopicWidget(o, winActive);
			break;
		
		case mwModesWidget:
			DrawChannelModes(o, winActive);
			break;
		
		case mwCMWidget:
			StandardIconWidget(o, giCMWidgetIcon);
			break;
		
		case mwHelpWidget:
			StandardIconWidget(o, giHelpWidgetIcon);
			break;
		
		case mwLinkWidget:
			StandardIconWidget(o, giLinkWidgetIcon);
			break;
	}
}

void DrawMWinStatus(MWPtr mw)
{
	GrafPtr gp;
	Rect r, r2;
	char winActive;
	mwPanePtr p;
	mwWidgetPtr o;
	pMWWidgetUpdateData pp;
	RgnHandle clip;
	char b;
	int themeTextColor;
	
	if(!mw || !mw->w || !IsWindowVisible(mw->w))
		return;
	
	if(++mw->drawingStatus > 1)
		return;
	
	GetPort(&gp);
	SetPortWindowPort(mw->w);
	winActive = IsWindowHilited(mw->w);

	p = mw->widgetsPane;
restart:
	r = p->drawArea;

	WGetBBox(mw->w, &r2);
	GlobalToLocal((Point*)&r2.bottom);

	r.top=r.bottom-15;
	r.left = -1;
	
	
	if(r2.bottom == r.bottom)
		 b = 1;
	else
	{
		b = 0;
		r.bottom++;
	}
	
	if(b)
		r.bottom++;
	r.right++;
	
	if(winActive)
	{
		DrawThemePlacard(&r, kThemeStateActive);
		themeTextColor = kThemeTextColorPlacardActive;
	}
	else
	{
		DrawThemePlacard(&r, kThemeStateInactive);
		themeTextColor = kThemeTextColorPlacardInactive;
	}
	
	TextFont(3);

	clip = NewRgn();
	GetClip(clip);

	//The window is completely set up now. Run through its widgets.
	linkfor(o, mw->widgetList)
	{
		Rect cr;

		cr = o->drawArea;
		cr.top++;
		cr.left--;
		cr.right++;
		ClipRect(&cr);
		
		TextSize(9);
		TextFace(0);
		SetTextColor(themeTextColor);
		MoveTo(o->drawArea.left, o->drawArea.bottom-3);
		
		if(o->creator==mwShadowIRCWidget && !o->pluginRef)
		{
			MWWidgetInternalDraw(o, winActive);
		}
		else
		{
			pp.widget=o;
			pp.winActive = winActive;
			runIndPlugin(o->pluginRef, pMWWidgetUpdateMessage, &pp);
		}
		
		if(mw->drawingStatus>1)
		{
			mw->drawingStatus = 1;
			SetClip(clip);
			DisposeRgn(clip);
			goto restart;
		}
	}

	SetClip(clip);
	DisposeRgn(clip);

	RGBForeColor(&black);
	if(!mw->protect)
		RGBBackColor(&shadowircColors[sicWindowBG]);
	else
		RGBBackColor(&white);

	SetPort(gp);
	mw->drawingStatus = 0;
}

#pragma mark -

void Message(const LongString *msg)
{
	MWPtr m;
	WindowPtr w;
	
	if(MWActive)
		m=MWActive;
	else
		m=MWLast;
	
	if(m && m->protect)
	{
		w=m->w;
		while(w)
		{
			w=GetNextWindow(w);
			m=MWFromWindow(w);
			if(m && !m->protect)
				break;
		}
	}
	
	if(!m)
		m=consoleWin;
	
	MWMessage(m, msg);
}

MWPtr ChannelWindow(linkPtr link, ConstStr255Param ch)
{
	MWPtr mw;
	channelPtr c=ChFind(ch, link);
	
	if(c && c->window)
		return c->window;
	
	//didn't find a chnnel. So look for a query window.
	mw = MWFindQuery(ch, link);
	
	if(!mw) //look for a dcc now...
		if(IsDCCName(ch))
			mw = MWFindDCC(ch, link);
	
	//if we didn't get a message window, then we need to decide where it goes...
	if(mw)
		return mw;
	else
	{
		//if it's not a channel it's query...
		if(!IsChannel(ch) && mainPrefs->privmsgsToConsole)
			return consoleWin;
		else //I dunno...front window sounds good to me
		{
			if(CurrentTarget.mw && !CurrentTarget.mw->protect)
				return CurrentTarget.mw;
			else
				return consoleWin;
		}
	}
}

void ChMsg(channelPtr ch, const LongString *msg)
{
	if(ch && ch->window)
		MWMessage(ch->window, msg);
	else
		MWMessage(consoleWin, msg);
}

void ChannelMsg(linkPtr link, ConstStr255Param ch, const LongString *msg)
{
	MWMessage(ChannelWindow(link, ch), msg);
}

MWPtr ChJoin(channelPtr ch)
{
	MWPtr w;
	long i;
	LongString ls;
	linkPtr link=ch->link;
	mwWidgetPtr o;
	char createdNewWindow = 0;
	
	w=ch->window;
	
	if(!w)
		w = MWFindChannel(ch);
	
	if(!w)
	{
		i=linkWinNull(link);
		
		LSConcatStrAndStrAndStr(ch->chName, "\p: ", link->linkPrefs->linkName, &ls);
		LSMakeStr(ls);
		w=MWNew(ls.data, chanWin, link, i);
		if(w)
		{
			wmAdd(w);
			ch->window=w;
			MWSetChannel(w, ch);

			linkWinAdd(w, i);

			createdNewWindow = true;

			if(mainPrefs->autoLogging)
				MWStartLogging(w);
		}
	}
	else
	{
		LSConcatStrAndStrAndStr(ch->chName, "\p: ", link->linkPrefs->linkName, &ls);
		LSMakeStr(ls);
		
		wmChange(w, ls.data);
		MWSetChannel(w, ch);
		ch->window=w;
		w->inactive=0;
		
		//Only need to fix the target if the window already exited.
		if(CurrentTarget.bad && ActiveNonFloatingWindow() == w->w)
		{
			SetTarget(w, &CurrentTarget);
			UpdateStatusLine();
		}
	}

	if(ch->chName[1]!= '+' && !mainPrefs->noModesWidget) //don't add modes widget if modeless channel
	{
		o = MWNewWidget(w, mwModesWidget, mwAlignLeft, mwModesWidgetWidth);
		if(o)
			o->data = (long)NewPtrClear(sizeof(mwModesWidgetData));
	}

	if(createdNewWindow)
	{
		if(mainPrefs->dontActivateNewWindowsIfInputlineText)
		{
			GetInputLine(&ls);
			if(!ls.len)
				WSelect(w->w);
			else
				ShowWindow(w->w);
		}
		else
			WSelect(w->w);
	}
	
	return w;
}

void DoJoinSelWin(MWPtr w)
{
	if(MWActive && MWActive->protect)
		ShowWindow(w->w);
	else if(mainPrefs->dontActivateNewWindowsIfInputlineText)
	{
		LongString ls;

		GetInputLine(&ls);
		if(!ls.len)
			WSelect(w->w);
		else
			ShowWindow(w->w);
	}
	else
		WSelect(w->w);
}

MWPtr DoJoinQuery(ConstStr255Param q, linkPtr link)
{
	MWPtr mw = MWFindQuery(q, link);
	
	if(mw)
	{
		mw->inactive = 0;
		MWSetNameQuery(mw, q, link);
	}
	else //make a new query window
	{
		mw=MWNewQuery(q, link);
		if(mw)
		{
			if(mainPrefs->autoLogQueries)
				MWStartLogging(mw);

			wmAdd(mw);
			DoJoinSelWin(mw);
		}
	}
	
	return mw;
}

void ChPart(MWPtr w)
{
	channelPtr cc;
	
	linkWinDel(w);
	cc = MWGetChannel(w);
	
	if(cc)
		ChCloseTopicWindow(cc);
	
	MWDelete(w);
	
	if(cc)
	{
		cc->window=0;
		ChDestroy(cc);
	}
	
	if(cc == CurrentTarget.chan)
		InvalTarget(&CurrentTarget);
}

void MWPart(MWPtr mw)
{
	pUIWindowCloseDataRec p;
	LongString ls;
	
	if(mw)
	{
		switch(mw->winType)
		{
			case conWin:
				HideWindow(mw->w);
				SendBehind(mw->w, 0);
				mainPrefs->consoleOpen=0;
				break;
			
			case chanWin:
			{
				channelPtr ch = MWGetChannel(mw);
				
				if(ch)
				{
					LSConcatStrAndStr("\pPART ", ch->chName, &ls);
					SendCommand(mw->link, &ls);
				}
				else
					ChPart(mw);
				break;
			}
			
			case queryWin:
				MWDelete(mw);
				break;
			
			case helpWin:
				CloseHelp();
				break;
			
			case dccWin:
				DCCWindowClose(mw);
				break;
			
			case pluginWin:
				p.w=mw->w;
				p.mw=mw;
				runIndPlugin(mw->pluginRef, pUIWindowCloseMessage, &p); //pUIWindowClose
				break;
			
			case textWin:
				TWClose(mw, false);
				break;
		}
	}
	
	if(!MWActive)
		InvalTarget(&CurrentTarget);
}

#pragma mark -

void CloseHelp(void)
{
	MWDelete(hel);
	hel=0;
	curHelpItem = 0;
}

void ShowHelp(short helpID)
{
	Handle ht, hs;
	pHelpMenuData p;
	Str255 title;
	
	if(!hel)
	{
		hel=MWNew("\pHelp", helpWin, 0, 0);
		if(!hel)
			return;
		MWNewWidget(hel, mwHelpWidget, mwForceLeft, 17);
	}
	
	if(helpID != curHelpItem)
	{
		curHelpItem = helpID;
		
		if(helpID >= (normHelpMenuItems - defaultHelpItems)) //plugin text
		{
			helpID -= defaultHelpItems;
			p.reference = helpID;
			p.text = p.style = 0;
			runIndPlugin((**hmiList).list[helpID].pluginRef, pHelpMenuMessage, &p);
			ht = p.text;
			hs = p.style;
			
			pstrcpy((**hmiList).list[helpID].name, title);
		}
		else //my text
		{
			ht=GetResource('TEXT', 127 + helpID);
			hs=GetResource('styl', 127 + helpID);
			
			GetIntString(title, spHelp, helpID);
		}
		if(ht && hs)
		{
			pstrcat("\pHelp: ", title, title);
			SetWTitle(hel->w, title);
			
			WEDeactivate(hel->we);
			WESetSelection(0, 0x7FFFFFFF, hel->we);
			HLock(ht);
			HLock(hs);
			WEInsert(*ht, GetHandleSize(ht), (StScrpHandle)hs, 0, hel->we);
			WESetSelection(0, 0, hel->we);
			ReleaseResource(ht);
			ReleaseResource(hs);
			WEActivate(hel->we);
		}
	}

	WSelect(hel->w);
	DrawMWinStatus(hel);
}
