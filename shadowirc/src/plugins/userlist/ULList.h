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

#ifndef _ULList_
#define _ULList_

#ifndef __ShadowIRC_Headers__
#include "headers.h"
#endif

extern CIconHandle gSortForwardIcon, gSortReverseIcon;

GrafPtr ULISetupDrawing(ULI ul);
void DrawWindowHeader(Rect *r, long state);

#if enableinline
INLINE void ULIFinishDrawing(ULI ul, GrafPtr gp) {
	#pragma unused(ul)
	SetOrigin(0,0); SetPort(gp);
}
#else
INLINE void ULIFinishDrawing(ULI ul, GrafPtr gp);
#endif

void ListSetWTitle(ULI ul);

void ListHeader(ULI ul);
void ListDrawUser(ULI ul, int num, short po);
char ListCanDraw(ULI ul, long num);
void ListDrawOne(ULI ul, long num);
void ListDrawOneUser(ULI ul, UserListPtr u);
void ListDraw(ULI ul);
void ListGenerate(ULI ul, channelPtr ch);
char ListSortPart(ULI ul, MyList *one, MyList *two);
void ListSort(ULI ul, char locked);
void ListTrash(ULI ul);
void ListAdd(ULI ul, UserListPtr u);
void ListDel(ULI ul, UserListPtr u);
char ListDelName(ULI ul, Str255 nick);
long ListFind(ULI ul, UserListPtr u);
char ListResortInd(ULI ul, int num);


#endif