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

#include <Carbon/Carbon.h>

#define linkfor(list, init) for((list) = (init); (list); (list)=(list)->next)

//inline void pstrcpy(ConstStringPtr f, StringPtr dest) {BlockMoveData(f, dest, f[0]+1);}
#define pstrcpy(src, dest) BlockMoveData(src, dest, (src)[0] + 1)

#define SetRGBColor(color, r, g, b) ((color).red = r, (color).green = g, (color).blue = b)

extern pascal char inupc(char c);
extern void DebugLS(char *str);

#define LineMsg(ls) MWMessage(consoleWin, ls)

#define pstrcmp3(str, l) (*(long*)str==0x03000000+l)
#define pstrcmp4(str, l, c) (*(long*)str==0x04000000+l && str[4]==c)
#define pstrcmp5(str, l, s) (*(long*)str==0x05000000+l && *(short*)&str[4]==s)
#define pstrcmp6(str, l, s, c) (*(long*)str==0x06000000+l && *(short*)&str[4]==s && str[6]==c)
#define pstrcmp7(str, l, l2) (*(long*)str==0x07000000+l && *(long*)&str[4]==l2)
#define pstrcmp8(str, l, l2, c) (*(long*)str==0x08000000+l && *(long*)&str[4]==l2 && str[8]==c)
#define pstrcmp9(str, l, l2, s) (*(long*)str==0x09000000+l && *(long*)&str[4]==l2 && *(short*)&str[8]==s)

#define pstrcmpX(str, l, l2) (*(long*)str == (((long)l << 24L) + l2))

