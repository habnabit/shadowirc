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

#ifndef _ULList_
#define _ULList_

#ifndef __ShadowIRC_Headers__
#include "headers.h"
#endif

#pragma internal on

//extern CIconHandle gSortForwardIcon, gSortReverseIcon;
extern CIconHandle gSortForwardIcon, gSortReverseIcon;

pascal GrafPtr ULISetupDrawing(ULI ul);
pascal void DrawWindowHeader(Rect *r, long state);

#if enableinline
INLINE void ULIFinishDrawing(ULI ul, GrafPtr gp) {
	#pragma unused(ul)
	SetOrigin(0,0); SetPort(gp);
}
#else
INLINE void ULIFinishDrawing(ULI ul, GrafPtr gp);
#endif

pascal void ListSetWTitle(ULI ul);

pascal void ListHeader(ULI ul);
pascal void ListDrawUser(ULI ul, int num, short po);
pascal char ListCanDraw(ULI ul, long num);
pascal void ListDrawOne(ULI ul, long num);
pascal void ListDrawOneUser(ULI ul, UserListPtr u);
pascal void ListDraw(ULI ul);
pascal void ListGenerate(ULI ul, channelPtr ch);
pascal char ListSortPart(ULI ul, MyList *one, MyList *two);
pascal void ListSort(ULI ul, char locked);
pascal void ListTrash(ULI ul);
pascal void ListAdd(ULI ul, UserListPtr u);
pascal void ListDel(ULI ul, UserListPtr u);
pascal char ListDelName(ULI ul, Str255 nick);
pascal long ListFind(ULI ul, UserListPtr u);
pascal char ListResortInd(ULI ul, int num);


#endif