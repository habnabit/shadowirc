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

#ifndef _utils_
#define _utils_

#include <Carbon/Carbon.h>

#ifndef _LongStrings
#include "LongStrings.h"
#endif


void * safe_malloc (size_t size);
pascal char upc(unsigned char c);
pascal void ucase(StringPtr str);
pascal void pdelete(StringPtr s, short start, short len);
pascal void pinsert(ConstStr255Param ins, StringPtr s, short insat);
pascal char pstrcmp(ConstStr255Param s1, ConstStr255Param s2);
pascal char maskMatch(ConstStr255Param s, ConstStr255Param mask);
pascal unsigned short countChar(short c, const ConstStr255Param s);
pascal unsigned short pos(short c, ConstStr255Param s);
pascal unsigned short revPos(short c, ConstStr255Param s);
pascal char isNumber(ConstStr255Param s);
pascal long IPStringToLong(ConstStr255Param s);
pascal char isIPNumber(ConstStr255Param s);
pascal void GetText(DialogPtr dlg, short id, StringPtr into);
pascal void SetText(DialogPtr dlg, short id, ConstStr255Param text);
pascal void setCheckBox(DialogPtr dlg, short item, char up);
pascal void setButtonEnable(DialogPtr dlg, short item, char on);
pascal char getCheckBox(DialogPtr dlg, short item);
pascal ControlHandle GetControlHandle(DialogPtr d, short i);
pascal char getFirstSelectedCell(ListHandle l, Cell *c);
pascal void selectOneCell(ListHandle l, Cell c);
pascal void makeCellVisible(ListHandle theList, Cell theCell);
pascal void drawListBorder(ListHandle myList);
pascal void SetDlogFont(DialogPtr d);
pascal void makeMask(StringPtr uah, StringPtr mask);
pascal void padEnd(ConstStr255Param s, short num, StringPtr out);
pascal void padBegin(ConstStr255Param s, short num, StringPtr out);
pascal char pstrcasecmp(ConstStr255Param s1, ConstStr255Param s2);
pascal char pstrcasecmp2(ConstStr255Param s1, ConstStr255Param s2);
pascal OSErr ParamString(StringPtr str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3);
pascal void SecsToHMS(long secs, LongString *ls);
pascal void LightenColor(RGBColor *rgb);
pascal unsigned char StandardDialogFilter(DialogPtr d, EventRecord *e, short *item);


pascal void pstrcpymax(ConstStringPtr src, StringPtr dest, char max);
pascal OSErr CopyResource(ResType type, short id, short src, short des);
pascal void pstrcat(ConstStr255Param src1, ConstStr255Param src2, StringPtr des);

pascal void NextArg(StringPtr from, StringPtr arg);

pascal void DeleteResourceType(short resFile, ResType type);

pascal void SetupModalDialog(DialogPtr d, short ok, short cancel);
pascal void FinishModalDialog(void);

pascal void pstrcpyucase(ConstStr255Param src, Str255 dest);
pascal void myCopy(ConstStr255Param s, short first, short last, StringPtr out);
pascal char rectcmp(const Rect *r1, const Rect *r2);
pascal void ulong64val(ConstStr255Param s, unsigned long long *out);

#ifdef _stringlist_
pascal void GetIntString(StringPtr the_string, STRnPtr list, short elt);
pascal ConstStr255Param GetIntStringPtr(STRnPtr list, short elt);
pascal void LSGetIntString(LongString *ls, STRnPtr list, short elt);
#endif

pascal StringPtr NewPString(const unsigned char *pstr);

#define SAppend1(s,ch){(s)[++((s)[0])]=ch;}
#define SAppend2(s,sh){(s)[0]+=2;*(short*)&(s)[(s)[0]-1]=sh;}
#define SAppend3(s,lo){(s)[0]+=3;*(long*)&(s)[(s)[0]-2]=lo;}
#define SAppend4(s,lo){(s)[0]+=4;*(long*)&(s)[(s)[0]-3]=lo;}

#define ZeroRect(r) *(long*)&(r).top = *(long*)&(r).bottom = 0;
#endif