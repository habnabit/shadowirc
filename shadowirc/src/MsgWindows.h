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

#ifndef _MsgWindows_
#define _MsgWindows_

#ifndef _MasterTypes_
#include "MasterTypes.h"
#endif

#include <Carbon/Carbon.h>

#define MW_MAGIC 'MWIN'

#ifndef MSGWINDOWS
	#define CONST const
#else
	#define CONST
#endif

typedef struct MessageWindow *MWPtr;

typedef struct mwWidgetRec mwWidgetRec, *mwWidgetPtr;
struct mwWidgetRec {
	CONST mwWidgetPtr next;	//Next object
	CONST mwWidgetPtr prev;	//Previous object
	CONST MWPtr mw;
	CONST long creator;
	CONST long type;
	long data;
	CONST Rect drawArea;
	short askedWidth;
	CONST short givenWidth;
	struct plugsRec* CONST pluginRef;
	CONST short align;
};

typedef struct mwPaneRec mwPaneRec, *mwPanePtr;
struct mwPaneRec {
	CONST mwPanePtr next, prev;
	CONST MWPtr mw;
	CONST long creator;
	CONST long type;
	long data;
	CONST Rect drawArea;
	short askedHeight, askedWidth;
	CONST short givenHeight, givenWidth;
	CONST short align;
	const short unused;
	struct plugsRec* CONST pluginRef;
};

enum mwTypes {
	invalWin = -1,
	conWin=0,
	chanWin,
	queryWin,
	pluginWin,
	helpWin,
	dccWin
};

/***
	WASTE 1.3 Modification Compatibility
***/

enum extraFlags {
	kBackColor = 1 << 0,
	kInverse = 1 << 1
};

typedef struct myScrpSTElement {
	long 							scrpStartChar;				/*starting character position*/
	short 							scrpHeight;
	short 							scrpAscent;
	short 							scrpFont;
	StyleField 						scrpFace;					/*unpacked byte*/
	short 							scrpSize;
	RGBColor 						scrpColor;
	RGBColor						scrpBackColor;
	long extraFlags;
} myScrpSTElement, *myScrpSTElementPtr;

typedef myScrpSTElement		myScrpSTTable[1601];

typedef struct myStScrpRec {
	short 							scrpNStyles;				/*number of styles in scrap*/
	myScrpSTTable 			scrpStyleTab;				/*table of styles for scrap*/
} myStScrpRec, *myStScrpPtr, **myStScrpHandle;

/***
	END WASTE 1.3 Modification Compatibility
***/

typedef union MWWindowData {
	struct Channel* chan;
	connectionPtr dcc;
	
	
	void* data;
} MWWindowData;

typedef const struct OpaqueMWWindowDataStruct *OpaqueMWWindowData;

typedef struct MessageWindow MessageWindow;
struct MessageWindow {
	CONST OSType magic;			//'MWIN'. DO NOT EVER CHANGE THIS.
	CONST WindowPtr w;			//The window.
	const short unused;
	CONST short winType;			//The type of window. (What to do if a user tries to close it.)
	CONST ControlHandle vscr;	//The vertical scroll bar.

#ifndef _WASTE_
	CONST Ptr we;
#else
	CONST WEReference we;
#endif

	CONST short vislines;			//Number of visible lines
	CONST short scrpHeight;		//height of one line.
	CONST short font,size;
	CONST myStScrpHandle sty;
	CONST short logRefNum;
	short colorMethod;
	CONST linkPtr link;
	CONST mwWidgetPtr widgetList;
	CONST mwPanePtr paneList;
	long refCon;

	MWPtr next, prev;

	struct plugsRec* CONST pluginRef;
	
//Reserved. Not in API
	Str255 name;
#ifdef MSGWINDOWS
	MWWindowData mwdata;
#else
	const OpaqueMWWindowData data;
#endif

	CONST mwPanePtr textPane, widgetsPane;

	CONST CharsHandle hist;
#ifndef _WASTE_
	CONST Ptr il;
#else
	CONST WEReference il;
#endif
	short hpos;
	
	char inactive;
	CONST unsigned char channelWindowNumber;
	CONST char protect;
	unsigned char drawingStatus;
};

enum{
	C_BOLD=2,
	C_INVERSE=22,
	C_UNDERLINE=31,
	C_NORMAL=14,
	C_COLOR=3,
	C_BEEP=7,
	C_SIColor=8
};


MWPtr GetFrontMW(void);
MWPtr GetActiveMW(void);

pascal char MWValid(MWPtr mw);
pascal MWPtr NewPluginMWindow(ConstStr255Param title);
void MWSetDimen(MWPtr win, short left, short top, short width, short height);
pascal void MWSetFontSize(MWPtr win, short font, short size);
pascal void MWDelete(MWPtr w);

pascal struct Channel* MWGetChannel(MWPtr mw);
pascal ConstStringPtr MWGetName(MWPtr mw, StringPtr s);
pascal connectionPtr MWGetDCC(MWPtr mw);

#ifdef _LongStrings
pascal void MWMessage(MWPtr, const LongString *ls);
#endif

pascal MWPtr MWFromWindow(WindowPtr w);

void MWHitContent(MWPtr mw, EventRecord *e);

extern MWPtr mwl;

pascal char MWSetChannel(MWPtr mw, struct Channel* ch);
pascal char MWSetDCC(MWPtr mw, connectionPtr dcc, Str255 name);
pascal void MWSetNameQuery(MWPtr mw, ConstStr255Param from, linkPtr link);

extern MWPtr consoleWin;
extern Rect cornerstone;

enum {
	kMaxMacColors = 28,
	kMaxMircColors = 16
};

extern const RGBColor macColors[kMaxMacColors];
extern const RGBColor mircColors[kMaxMircColors];

pascal void MWPage(MWPtr mw, char up);
pascal void MWScroll(MWPtr mw, long delta);

pascal void MWNewPosition(Rect *windowSize);

pascal MWPtr MWNewQuery(ConstStr255Param name, linkPtr link);
pascal MWPtr MWNew(ConstStr255Param title,short DoWhenDone, linkPtr link, long mwinRefCon);

pascal void InitMsgWindows(void);

pascal void MWUnquote(MWPtr mw);
pascal void MWQuote(MWPtr mw);

pascal OSErr MWLogToFSp(MWPtr mw, const FSSpec *fs, long s0, long s1);
pascal void MWLogToFile(MWPtr mw, long s0, long s1);
pascal void MWStopLogging(MWPtr mw);
pascal void MWStartLogging(MWPtr w);

pascal void MWReposition(MWPtr win);

#undef CONST
#endif