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

#ifndef _WindowList_
#define _WindowList_

#define windowsStart2 (windowsStart + 1 + wmItems.channelsLength)

#ifndef WINDOWLIST
#define CONST const
#else
#define CONST
#endif

typedef struct wmList {
	CONST MWPtr mw[1];
} wmList, *wmListPtr, **wmListHand;

typedef struct {
	CONST short channelsLength;
	CONST short restLength;
	
	CONST wmListHand channels;
	CONST wmListHand rest;
} wmItemsData;

extern wmItemsData wmItems;

#pragma internal on
pascal void wmItemsInit(void);
pascal void wmChange(MWPtr mw, ConstStr255Param newName);
pascal void wmAdd(MWPtr mw);
pascal void wmDelete(MWPtr mw);
#pragma internal reset

#pragma mark -

pascal MWPtr MWFindQuery(ConstStr255Param s, linkPtr link);
pascal MWPtr MWFindDCC(ConstStr255Param s, linkPtr link);
pascal MWPtr MWFindChannel(channelPtr ch);


#pragma lib_export on
#pragma export on

pascal MWPtr findWLName(ConstStr255Param s, linkPtr link);

#pragma export off
#pragma lib_export off

#undef CONST

#endif