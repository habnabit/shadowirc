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

#ifndef _InputLine_
#define _InputLine_

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
	
	iwStatusLine = 'stat'
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

typedef struct inputAreaData inputAreaData, *inputAreaDataPtr;

typedef struct inputLineRec {
	CONST WindowPtr w;
	CONST inputAreaDataPtr inputData;
	CONST FontInfo fi;
	CONST short fontnum, fontsize;
	
	CONST iwWidgetPtr objectList;
	CONST iwWidgetPtr status;

	CONST short statusLinePos, statusLineHeight;
	
	char statuslineFlags;
	CONST char lock;
} inputLineRec, *inputLinePtr;

extern inputLineRec inputLine;
extern char noFloatingInput;

#undef CONST


#ifdef _MsgWindows_
void ILAddHistory(MWPtr mw, LongString *line);

inputAreaDataPtr ILGetInputDataFromMW(MWPtr mw);
#endif

void OpenInputLine(void);
void StatusLineClick(Point where, short modifiers);

void IADDispose(inputAreaDataPtr iad);

char IADGetText(inputAreaDataPtr iad, LongString *ls);
char IADSetText(inputAreaDataPtr iad, LongString *ls);

void IADSetCursorSelection(inputAreaDataPtr iad, long start, long finish);
void IADGetCursorSelection(inputAreaDataPtr iad, long *start, long *finish);

void RecallLineUp(inputAreaDataPtr iad);
void RecallLineDown(inputAreaDataPtr iad);


void IWLock(void);
void IWUnlock(void);

pascal iwWidgetPtr IWNewWidget(long type, short align, short width);
pascal void IWRecalculateRects(void);
pascal void UpdateStatusLine(void);
pascal long IWPopUpMenu(Point p, MenuHandle m, long curItem);
pascal char IWDeleteWidget(iwWidgetPtr o);
pascal short IWOverride(long type, iwWidgetPtr *object);

#ifdef _MsgWindows_
#ifdef _WASTE_
inputAreaDataPtr IADNew(WEReference il);
WEReference ILGetWEFromMW(MWPtr mw);
WEReference IADGetWE(inputAreaDataPtr iad);
#endif
#endif

#endif
