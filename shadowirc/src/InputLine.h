/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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

#include "InputControl.h"

#ifndef __INPUTLINE__
#define CONST const
#else
#define CONST
#endif

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

enum statuslineFlags {
	kUmodeFlagsOn = 1 << 0,
	kUmodeFlagsPlus = 1 << 1,
	kBoldedPopups = 1 << 2
};

typedef struct InputWindowData {
	CONST WindowPtr w;
	CONST inputAreaDataPtr inputData;
	CONST ControlRef statusControl;
	
	CONST short statusLinePos, statusLineHeight;
	
	long statuslineFlags;
	
	//END OF PLUGINS API
	
	CONST FontInfo fi;
	CONST short fontnum, fontsize;
	CONST char lock;
} InputWindowData, *InputWindowDataPtr;

extern char noFloatingInput;

#undef CONST

void IWLock(void);
void IWUnlock(void);

void OpenInputLine(void);
InputWindowDataPtr GetInputWindowData(void);

void UpdateStatusLine(void);

inputAreaDataPtr ILGetInputDataFromMW(MWPtr mw);

#endif
