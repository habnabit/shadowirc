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

#ifndef _LongStrings
#define maxLSlen 600

#if TARGET_CARBON
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#endif

typedef struct LongString
{
	short len;
//MISALIGNED 2
	unsigned char data[maxLSlen + 1];
} LongString;

#ifdef __cplusplus
extern "C" {
#endif

pascal void LSDupe(const LongString * const l1, LongString *l2);

#pragma lib_export on
#pragma export on

pascal char LSCmp(const LongString *ls1, const LongString *ls2);
pascal char LSCaseCmp(const LongString *ls1, const LongString *ls2);
pascal void LSDelete(LongString *ls, short firstpos, short lastpos);
pascal void LSCopy(const LongString *ls, short firstpos, short len, LongString *out);
pascal void LSCopyString(const LongString *ls, short firstpos, short len, StringPtr str);
pascal short LSPosCustom(ConstStr255Param s, const LongString *ls, short start);
pascal short LSPos(ConstStr255Param s, const LongString *ls);
pascal short LSPosCaseCustom(ConstStr255Param s, const LongString *ls, short start);
pascal short LSPosCase(ConstStr255Param s, const LongString *ls);
pascal void LSNextArg(LongString *ls, StringPtr arg);
pascal void LSNextArgND(LongString *ls, StringPtr arg);
pascal short LSPosChar(short c, const LongString *ls);
pascal void LSStrLS(ConstStr255Param s, LongString *ls);
pascal void LSConcatLSAndStr(const LongString *ls, ConstStr255Param s, LongString *out);
pascal void LSConcatStrAndLS(ConstStr255Param s, const LongString *ls, LongString *out);
pascal void LSConcatStrAndStr(ConstStr255Param s1, ConstStr255Param s2, LongString *out);
pascal void LSConcatLSAndLS(const LongString *ls, const LongString *ls2, LongString *out);
pascal void LSConcatLSAndStrAndLS(const LongString *ls1, ConstStr255Param s, const LongString *ls2, LongString *out);
pascal void LSConcatLSAndStrAndStr(const LongString *ls1, ConstStr255Param s, ConstStr255Param s2, LongString *out);
pascal void LSInsertStr(ConstStr255Param s, short insat, LongString *ls);
pascal char LSInsertChar(char c, short p, LongString *out);
pascal char LSInsertShort(short s, short p, LongString *out);
pascal char LSInsertLong(long l, short p, LongString *out);
pascal void LSConcatStrAndLSAndStr(ConstStr255Param s1, const LongString *ls, ConstStr255Param s2, LongString *out);
pascal void LSConcatStrAndStrAndStr(ConstStr255Param s1, ConstStr255Param s2, ConstStr255Param s3, LongString *out);
void LSStrCat(short n,LongString *out,...);
pascal void LSGetIndString(LongString *ls, short id, short num);
pascal OSErr LSParamString(LongString *ls, ConstStr255Param str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3);
pascal OSErr LSParamLS(LongString *ls, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3);

#pragma export off
#pragma lib_export off

#ifdef __cplusplus
}
#endif


#pragma internal on
pascal void LSNextArgIRC(LongString *ls, StringPtr arg);
pascal void LSStrCat4(LongString *out, ConstStr255Param s1, ConstStr255Param s2, ConstStr255Param s3, ConstStr255Param s4);

#define LSAppend1(ls,ch){(ls).data[++((ls).len)]=ch;}
#define LSAppend2(ls,sh){(ls).len+=2;*(short*)&(ls).data[(ls).len-1]=sh;}
#define LSAppend3(ls,lo){(ls).len+=3;*(long*)&(ls).data[(ls).len-2]=lo;}
#define LSAppend4(ls,lo){(ls).len+=4;*(long*)&(ls).data[(ls).len-3]=lo;}

#define LSMakeStr(ls) {if((ls).len>=255) (ls).data[0]=255; else (ls).data[0]=(ls).len;}

#pragma internal reset

#define _LongStrings
#endif