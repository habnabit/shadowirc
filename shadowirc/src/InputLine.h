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

#ifndef _InputLine_
#define _InputLine_

#define MAXLINE 500
#define MAXHIST 4096

#ifndef __INPUTLINE__
#define CONST const
#else
#define CONST
#endif

enum {
	kInputField = 'INPT'
};

enum iwAlign {
	iwLeft=0,
	iwRight=1
};

enum iwOverrideErrors {
	iwOverrideNoErr = 0,			//IWOverride() successful
	iwOverrideNotFound,				//No such ShadowIRC inputwindow object found
	iwOverridePluginObject,		//The object was a plugin object. You can't have it.
	iwOverrideAlready				//That ShadowIRC object is already overridden.
};

enum iwDefaultObjects {
	iwShadowIRCObject = 'SIRC',
	iwPluginObject = 'PLUG',
	
	iwStatusLine = 'stat',
	iwMemory = 'memd'
};

typedef struct iwStatusObjectRec {
	short awayWidth;
	short istalkingwithWidth;
	short onWidth;
	short maxLineWid;
	short closeParenWid;
	
	short nickStart, nickEnd;
	short targStart, targEnd;
	short servStart, servEnd;
} iwStatusObjectRec, *iwStatusObjectPtr;

typedef struct iwWidgetRec iwWidgetRec, *iwWidgetPtr;
struct iwWidgetRec {
	CONST iwWidgetPtr next;	//Next object
	CONST iwWidgetPtr prev;	//Previous object
	CONST long creator;
	CONST long type;
	CONST long data;
	CONST Rect drawArea;
	CONST short askedWidth;
	CONST short givenWidth;
	CONST short align;
	const short unused;
	CONST struct plugsRec* pluginRef;
};

enum {
	kILMDBytes, kILMDKBytes,
	kILMDmax=1
};

enum statuslineFlags {
	kUmodeFlagsOn = 1 << 0,
	kUmodeFlagsPlus = 1 << 1,
	kBoldedPopups = 1 << 2
};

typedef struct inputLineRec {
	CONST WindowPtr w;
	CONST CharsHandle _hist;
	CONST FontInfo fi;
	CONST short fontnum, fontsize;
	
	CONST iwWidgetPtr objectList;
	CONST iwWidgetPtr status, memory;

#ifndef _WASTE_
	CONST Ptr _il;
#else
	CONST WEReference _il;
#endif

	CONST short statusLinePos, statusLineHeight;
	short _hpos;
	
	char statuslineFlags;
	CONST char lock;
} inputLineRec, *inputLinePtr;

#pragma internal on
extern inputLineRec inputLine;

#ifdef _MsgWindows_
pascal void ILAddHistory(MWPtr mw, LongString *line);
#endif

pascal void OpenInputLine(void);
pascal void StatusLineClick(Point where, short modifiers, long when);
pascal void IWGrow(const EventRecord *e);

#ifdef _WASTE_
pascal char ILWEIsInput(WEReference we);
pascal WEReference ILGetWE(void);
#endif

#pragma internal reset

pascal void IWLock();
pascal void IWUnlock();

#pragma lib_export on
#pragma export on
pascal void SetInputLineCursorSelection(long start, long finish);
pascal void GetInputLineCursorSelection(long *start, long *finish);
pascal void SetInputLine(LongString *ls);
pascal void GetInputLine(LongString *line);

pascal iwWidgetPtr IWNewWidget(long type, short align, short width);
pascal void IWRecalculateRects(void);
pascal void UpdateStatusLine(void);
pascal long IWPopUpMenu(Point p, MenuHandle m, long curItem);
pascal char IWDeleteWidget(iwWidgetPtr o);
pascal short IWOverride(long type, iwWidgetPtr *object);

#ifdef _MsgWindows_
#ifdef _WASTE_
pascal char ILSetLine(MWPtr mw, LongString *ls);
pascal char ILGetLine(MWPtr mw, LongString *ls);
#endif
#endif
#pragma export off
#pragma lib_export off

#undef CONST

pascal void RecallLineUp(void);
pascal void RecallLineDown(void);

#ifdef _MsgWindows_
#ifdef _WASTE_
pascal WEReference _ILGetWE(MWPtr mw);
#endif
pascal char ILInsertLine(MWPtr mw, LongString *ls, char select);
#endif

pascal CharsHandle ILGetHist(void);
pascal void ILSetHpos(long hp);
pascal long ILGetHpos(void);

extern char noFloatingInput;

#endif