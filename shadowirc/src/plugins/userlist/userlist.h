/*
	ShadowIRC Userlist
	Copyright (C) 1997-2000 John Bafford
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

#ifndef _userlist_
#define _userlist_

#ifndef __ShadowIRC_Headers__
#include "ShadowIRC.h"
#endif

#pragma internal on

#define scrollbarWidth 16

#define enableinline 0

#if enableinline
	#define INLINE inline
#else
	#define INLINE pascal
#endif

typedef struct MyList {
	UserListPtr u;
} MyList, *MyListPtr;

typedef struct UserListBegin {
	channelPtr ch;
	long num;
} UserListBegin;

typedef struct UserList {
	channelPtr ch;
	long num;
	MyList u[1];
} ulRec, *ulPtr, **ulHand;

#if __POWERPC__
	#define APPEARANCE11 hasAppearance11
#else
	#define APPEARANCE11 0
#endif

enum ulType {
	ulGlobal = 'ulGl',
	ulMessageWindow = 'ulIW',

	kUserlistPane = ulMessageWindow,

	ulWindowRefCon = ulGlobal
};

typedef struct UserListInstance {
	long ulType;
	WindowPtr uwin;
	ControlHandle bar;
	Rect uwinSize;
	ulHand users;
	long visLines;
	long lastClick;
	long lastClickTime;
	
	MWPtr mw;
	mwPanePtr pane;
	
	short nickListWidth;
	short sort;
	char sortReverse;
	char rightSide;
	char scrollbarLeft;
	char updateList;
	long lastUpdate;
} UserListInstance, *ULI;

enum {
	kShitSort = -1
};

enum {
	kHeaderSize  = 14,
	kHeaderText = 10,
	
	kTopOffset = 1 + kHeaderSize,
	kBottomOffset = 2,
	kLeftOffset = 4,
	kRightOffset = 14 + 4,
	
	kInWindowBorder = 5,
	
	kMinimumPaneWidth = kInWindowBorder, //= 20,
	
	kULMininumWidth = 64,
	kULMinimumHeight = 64
};

enum {
	kIdleNormal = 20,
	kIdleFast = 4
};

enum {
	cMedGrey = 44395,
	cMDkGrey = 35939
};

extern ShadowIRCDataRecord* sidr;
extern prefsPtr mainPrefs;
extern RGBColor *shadowircColors;
extern char hasAppearance, hasAppearance11;
extern short genevaNum;
extern int line;

extern const RGBColor white, black, MedGrey, MDkGrey;

#if __POWERPC__
	#define PPC 1
#else
	#define PPC 0
#endif

#define WInvalRect(w, r) {if(PPC && hasAppearance11)InvalWindowRect(w, r);else{GrafPtr g;GetPort(&g);if(g==w)InvalRect(r);else{SetPort(w);InvalRect(r);SetPort(g);}}}
#define WInvalRgn(w, r) {if(PPC && hasAppearance11)InvalWindowRgn(w, r);else{GrafPtr g;GetPort(&g);if(g==w)InvalRgn(r);else{SetPort(w);InvalRgn(r);SetPort(g);}}}
#define WValidRect(w, r) {if(PPC && hasAppearance11)ValidWindowRect(w, r);else{GrafPtr g;GetPort(&g);if(g==w)ValidRect(r);else{SetPort(w);ValidRect(r);SetPort(g);}}}
#define WValidRgn(w, r) {if(PPC && hasAppearance11)ValidWindowRgn(w, r);else{GrafPtr g;GetPort(&g);if(g==w)ValidRgn(r);else{SetPort(w);ValidRgn(r);SetPort(g);}}}

#endif