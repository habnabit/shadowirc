/*
	ShadowIRC Userlist
	Copyright (C) 1997-2002 John Bafford
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

#define linkfor(list, init) for((list) = (init); (list); (list)=(list)->next)

#define enableinline 0

#if enableinline
	#define INLINE inline
#else
	#define INLINE
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
	ControlRef browser;
	Rect uwinSize;
	channelPtr ch;
	
	MWPtr mw;
	mwPanePtr pane;
	
	short nickListWidth;
	short sort;
	char sortReverse;
	char rightSide;
	char updateList;
	long lastUpdate;
} UserListInstance, *ULI;

enum {
	kTopOffset = 1 + 14,
	kBottomOffset = 2,
	kLeftOffset = 4,
	kRightOffset = 14 + 4,
	
	kInWindowBorder = 5,
	
	kMinimumPaneWidth = kInWindowBorder, //= 20,
	
	kULMininumWidth = 80,
	kULMinimumHeight = 80
};

enum {
	kIdleNormal = 20,
	kIdleFast = 4
};

#endif