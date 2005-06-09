/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2005 John Bafford
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

#include <stdarg.h>

#include "LongStrings.h"
#include "Inline.h"

pascal void LSDupe(const LongString * const l1, LongString *l2)
{
	int len;
	
	if(l1->len > maxLSlen)
		len = maxLSlen;
	else
		len = l1->len;
	
	len += sizeof(short) + 1;
	BlockMoveData(l1, l2, len);
}

pascal char LSCmp(const LongString *ls1, const LongString *ls2)
{
	int x, y;
	
	if(ls1==ls2)
		return 1;
	
	if(ls1->len != ls2->len)
		return 0;
	else
	{
		y=ls1->len;
		for(x=1;x<=y;x++)
			if(ls1->data[x] != ls2->data[x])
				return 0;
		
		return 1;
	}
}

static inline char inupc(char c);
static inline char inupc(char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

pascal char LSCaseCmp(const LongString *ls1, const LongString *ls2)
{
	int x, y;
	
	if(ls1==ls2)
		return 1;
	
	if(ls1->len != ls2->len)
		return 0;
	else
	{
		y=ls1->len;
		for(x=1;x<=y;x++)
			if(inupc(ls1->data[x]) != inupc(ls2->data[x]))
				return 0;
		
		return 1;
	}
}

pascal void LSDelete(LongString *ls, short firstpos, short lastpos)
{
	if(firstpos>0 && lastpos >= firstpos)
	{
		if(lastpos>ls->len)
			lastpos=ls->len;
		
		BlockMoveData(&ls->data[lastpos+1], &ls->data[firstpos], ls->len - lastpos);
		ls->len+=(-1)-lastpos+firstpos;
	}
}

pascal void LSCopy(const LongString *ls, short firstpos, short len, LongString *out)
{
	if(firstpos > ls->len)
		out->len = 0;
	else if(firstpos > 0)
	{
		short maxLen = ls->len - firstpos + 1;
		
		if(len > maxLen)
			len = maxLen;
		
		BlockMoveData(&ls->data[firstpos], &out->data[1], len);
		out->len = len;
	}
	else
		if(out != ls)
			LSDupe(ls, out);
}

pascal void LSCopyString(const LongString *ls, short firstpos, short len, StringPtr str)
{
	if(firstpos>0 && len >= 1)
	{
		if(len>255)
			len=255;
		
		if((-1) + len + firstpos > ls->len)
			len = ls->len - firstpos + 1;
		
		BlockMoveData(&ls->data[firstpos], &str[1],len);
		str[0]=(char)len;
	}
	else
		str[0]=0;
}

short LSPosCharInRange(int ch, const LongString *ls, short start, short length)
{
	int cur, stop;
	
	//If the length is negative, we want the length characters preceeding start. (WHY?)
	if(length < 0)
	{
		start += length; //remember, length is negative, so this is subtraction.
		length = -length;
	}
	
	//if the start index is outside the string's own bounds, or if the length is zero, bail with no work.
	if(start > ls->len || start < 1 || length == 0)
		return 0;
	
	stop = start + length - 1;
	if(stop > ls->len)
		stop = ls->len;
	
	for(cur = start; cur <= stop; cur++)
		if(ls->data[cur] == ch)
			return cur;
	
	return 0;
}

pascal short LSPosCustom(ConstStr255Param s, const LongString *ls, short start)
{
	int i = s[0], max = ls->len - i + 1;
	int x, x2, x3;
	const unsigned char* lsd = ls->data;
	char b;
	int step, stop;
	
	if(start >= 0)
	{
		step = 1;
		stop = max + 1;
	}
	else
	{
		step = -1;
		start = ls->len + start + 1;
		if(start > max)
			start = max;
		stop = 1;
	}
	
	for(x = start; x != stop; x+= step)
	{
		b = 1;
		
		for(x2=1, x3 = x; b && (x2 <= i); x2++, x3++)
			b = (s[x2] == lsd[x3]);
		
		if(b)
			return x;
	}
	return 0;
}

pascal short LSPos(ConstStr255Param s, const LongString *ls)
{
	return LSPosCustom(s, ls, 1);
}

pascal short LSPosCaseCustom(ConstStr255Param s, const LongString *ls, short start)
{
	int i = s[0], max = ls->len - i + 1;
	int x, x2, x3;
	const unsigned char* lsd = ls->data;
	char b;
	int step, stop;
	
	if(start >= 0)
	{
		step = 1;
		stop = max + 1;
	}
	else
	{
		step = -1;
		start = ls->len + start + 1;
		if(start > max)
			start = max;
		stop = 1;
	}
	
	for(x = start; x != stop; x+= step)
	{
		b = 1;
		
		for(x2=1, x3 = x; b && (x2 <= i); x2++, x3++)
			b = (inupc(s[x2]) == inupc(lsd[x3]));
		
		if(b)
			return x;
	}
	return 0;
}

pascal short LSPosCase(ConstStr255Param s, const LongString *ls)
{
	return LSPosCaseCustom(s, ls, 1);
}

pascal void LSNextArgIRC(LongString *ls, StringPtr arg)
{
	long i;
	
	i=LSPosChar(' ', ls);
	if(i)
	{
		if(arg)
			LSCopyString(ls,1,i-1, arg);
		while(i<ls->len)
		{
			if((ls->data[i+1]!=':')&&(ls->data[i+1]!=' '))
				break;
			i++;
		}
		LSDelete(ls,1,i);
	}
	else
	{
		if(arg)
			LSCopyString(ls,1,ls->len, arg);
		ls->len=0;
	}
}

pascal void LSNextArg(LongString *ls, StringPtr arg)
{
	int i;
	
	i=LSPosChar(' ', ls);
	if(i)
	{
		if(arg)
			LSCopyString(ls,1,i-1, arg);
		while(i<ls->len)
		{
			if(ls->data[i+1]!=' ')
				break;
			i++;
		}
		LSDelete(ls,1,i);
	}
	else
	{
		if(arg)
			LSCopyString(ls,1,ls->len, arg);
		ls->len=0;
	}
}

pascal void LSNextArgND(LongString *ls, StringPtr arg)
{
	int i;
	
	i=LSPosChar(' ', ls);
	if(i)
		LSCopyString(ls,1,i-1, arg);
	else
		LSCopyString(ls,1,ls->len, arg);
}

pascal short LSPosChar(short c, const LongString *ls)
{
	int i=1;
	
	for(;i<=ls->len;i++)
		if(ls->data[i]==(char)c)
			return i;
	
	//else
	return 0;
}

pascal void LSStrLS(ConstStr255Param s, LongString *ls)
{
	ls->len=s[0];
	BlockMoveData(s, ls->data, ls->len+1);
}

#pragma mark -

pascal void LSConcatLSAndStr(const LongString *ls, ConstStr255Param s, LongString *out)
{
	long x=s[0], i=maxLSlen - ls->len;
	
	if(ls!=out)
		LSDupe(ls,out);
	if(i>x)
		i=x;
	
	BlockMoveData(&s[1], &out->data[ls->len+1], i);
	out->len+=i;
}

pascal void LSConcatStrAndLS(ConstStr255Param s, const LongString *ls, LongString *out)
{
	short x=s[0], i=maxLSlen-x;
	
	if(i>ls->len)
		i=ls->len;
	
	BlockMoveData(&ls->data[1], &out->data[x+1], i);	// Move the longstring down FIRST, THEN move the string in.
	BlockMoveData(s, out->data, x+1);					// This way, we can allow out = ls.
	out->len=i+x;
}

pascal void LSConcatStrAndStr(ConstStr255Param s1, ConstStr255Param s2, LongString *out)
{
	short x=s1[0], y=s2[0];
	
	out->len=x+y;
	BlockMoveData(s2, &out->data[x], y+1);
	BlockMoveData(s1, out->data, x+1);
}

pascal void LSConcatStrAndStrAndLS(ConstStr255Param s1, ConstStr255Param s2, const LongString *ls, LongString *out)
{
	short x=s1[0], y=s2[0];
	int i = maxLSlen - (x + y);
	
	if(i > ls->len)
		i = ls->len;
	BlockMoveData(ls->data, &out->data[x+y], i+1);
	out->len = x + y + i;

	BlockMoveData(s2, &out->data[x], y+1);
	BlockMoveData(s1, out->data, x+1);
}

pascal void LSConcatLSAndLS(const LongString *ls, const LongString *ls2, LongString *out)
{
	int i=maxLSlen-ls->len;
	short j;
	short k = ls->len;

	if(i>ls2->len)
		j=ls2->len;
	else
		j=i;
	
	BlockMoveData(&ls2->data[1], &out->data[k+1], j);
	if(out!=ls)
		BlockMoveData(ls->data, out->data, k+1);
	out->len = k + j;
}

#pragma mark -
pascal void LSConcatLSAndStrAndLS(const LongString *ls1, ConstStr255Param s, const LongString *ls2, LongString *out)
{
	LongString tls;
	int x = s[0];
	int i = maxLSlen-ls1->len;
	
	if(ls2 == out)
	{
		LSDupe(ls2, &tls);
		ls2 = &tls;
	}
	if(ls1 != out)
		LSDupe(ls1, out);
	
	//Only copy the part of the string that will fit
	if(i > x)
		i = x;
	if(i > 0)
	{
		BlockMoveData(&s[1], &out->data[out->len+1], i);
		out->len += i;
	}
	
	//Only copy the part of ls2 that will fit
	i = maxLSlen - out->len;
	if(i > ls2->len)
		i = ls2->len;
	
	if(i>0)
	{
		BlockMoveData(&ls2->data[1], &out->data[out->len+1], i);
		out->len+=i;
	}
}

pascal void LSConcatLSAndStrAndStr(const LongString *ls1, ConstStr255Param s, ConstStr255Param s2, LongString *out)
{
	char n=0;
	long x=s[0], i=maxLSlen-ls1->len;
	
	if(ls1!=out)
		LSDupe(ls1, out);
	
	if(i>x)
		i=x;
	if(i>0)
	{
		BlockMoveData(&s[1], &out->data[out->len+1], i);
		out->len+=i;
	}
	
	i=maxLSlen-out->len;
	if(n)
	{
		if(i>out->len)
			i=out->len;
	}
	else
		if(i>s2[0])
			i=s2[0];
	
	if(i>0)
	{
		BlockMoveData(&s2[1], &out->data[out->len+1], i);
		out->len+=i;
	}
}

pascal void LSInsertStr(ConstStr255Param s, short insat, LongString *ls)
{
	LongString l2;

	if(insat>ls->len)
		LSConcatLSAndStr(ls,s, ls);
	else if(insat<=1)
		LSConcatStrAndLS(s,ls, ls);
	else
	{
		LSCopy(ls,insat+1,maxLSlen, &l2);
		ls->len = insat;
		LSConcatLSAndStrAndLS(ls,s,&l2, ls);
	}
}

pascal char LSInsertChar(char c, short p, LongString *out)
{
	if(out->len < maxLSlen - 1)
	{
		BlockMoveData(&out->data[p], &out->data[p+1], out->len +1 - p);
		out->data[p]=c;
		out->len++;
		return 1;
	}
	else
		return 0;
}

pascal char LSInsertShort(short s, short p, LongString *out)
{
	if(out->len < maxLSlen - 2)
	{
		BlockMoveData(&out->data[p], &out->data[p+2], out->len +1 - p);
		*(short*)&out->data[p]=s;
		out->len+=2;
		return 1;
	}
	else
		return 0;
}

pascal char LSInsertLong(long l, short p, LongString *out)
{
	if(out->len < maxLSlen - 4)
	{
		BlockMoveData(&out->data[p], &out->data[p+4], out->len +1 - p);
		*(long*)&out->data[p]=l;
		out->len+=4;
		return 1;
	}
	else
		return 0;
}

pascal void LSConcatStrAndLSAndStr(ConstStr255Param s1, const LongString *ls, ConstStr255Param s2, LongString *out)
{
	long i=s1[0], x=s2[0];
	long n;

	n=maxLSlen-i;
	if(n>ls->len)
		n=ls->len;
	
	BlockMoveData(&ls->data[1], &out->data[i+1], n);
	BlockMoveData(s1, out->data, i+1);
	out->len=i+n;
	
	i=maxLSlen-out->len;
	if(i>x)
		i=x;
	
	BlockMoveData(&s2[1], &out->data[out->len+1], i);
	out->len+=i;
}

pascal void LSConcatStrAndStrAndStr(ConstStr255Param s1, ConstStr255Param s2, ConstStr255Param s3, LongString *out)
{
	long x=s1[0], y=s2[0], z=s3[0], a=x+y, i=maxLSlen-a;

	BlockMoveData(s1, out->data, x+1);
	BlockMoveData(&s2[1], &out->data[x+1], y);
	
	i=maxLSlen-a;
	if (i>z)
		i=z;
	BlockMoveData(&s3[1], &out->data[a+1], i);
	out->len=a+i;
}

pascal void LSStrCat4(LongString *out, ConstStr255Param s1, ConstStr255Param s2, ConstStr255Param s3, ConstStr255Param s4)
{
	long x=s1[0], y=s2[0], a=x+y, i;

	BlockMoveData(s1, out->data, x+1);
	BlockMoveData(&s2[1], &out->data[x+1], y);
	
	x = s3[0];
	i=maxLSlen-a;
	if (i>x)
		i=x;
	BlockMoveData(&s3[1], &out->data[a+1], i);
	
	a+=i;
	//Ok, everything's set up for the first three...
	x=s4[0];
	i = maxLSlen -a;
	if(i>x)
		i=x;
	BlockMoveData(&s4[1], &out->data[a+1], i);
	out->len=a+i;
}

void LSStrCat(short n,LongString *out,...)
{
	va_list args;
	long len;
	StringPtr cur;
	Ptr dest;
	
	va_start(args,out);
	
	if(n<1)
	{
		out->len=0;
		return;
	}
	
	len=0;
	for(;n;n--)
	{
		cur=va_arg(args, StringPtr);
		dest=(char*)&(out->data[len+1]);
		if(len+cur[0]>maxLSlen)
		{
			BlockMoveData(&cur[1], dest, maxLSlen-len);
			len=maxLSlen;
			break;
		}
		else
		{
			BlockMoveData(&cur[1], dest, cur[0]);
			len+=cur[0];
		}
	}
	
	out->len=len;
	va_end(args);
}

pascal void LSGetIndString(LongString *ls, short id, short num)
{
	GetIndString(ls->data, id, num);
	ls->len = ls->data[0];
}

pascal OSErr LSParamString(LongString *ls, ConstStr255Param str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3)
{
	long len, x, max;
	
	max = str[0];
	len = 1;
	for(x = 1; x <= max; x++)
	{
		if(len >= maxLSlen)
			break;
		
		if(str[x] != '^' || x >= max) //or if last character
			ls->data[len++] = str[x];
		else
		{
			ls->len = len-1;
			switch(str[x+1])
			{
				case '0':
					if(r0)
					{
						LSConcatLSAndStr(ls, r0, ls);
						len += r0[0];
					}
					x++;
					break;
				
				case '1':
					if(r1)
					{
						LSConcatLSAndStr(ls, r1, ls);
						len += r1[0];
					}
					x++;
					break;
				
				case '2':
					if(r2)
					{
						LSConcatLSAndStr(ls, r2, ls);
						len += r2[0];
					}
					x++;
					break;
				
				case '3':
					if(r3)
					{
						LSConcatLSAndStr(ls, r3, ls);
						len += r3[0];
					}
					x++;
					break;
				
				default: //the ^ and the char after it
					ls->data[len++] = str[x-1];
				case '^': //just the ^
					ls->data[len++] = str[x++];
					break;
			}
		}
	}
	ls->len = len-1;
	
	return 0;
}

pascal OSErr LSParamLS(LongString *ls, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3)
{
	Str255 str;
	
	LSCopyString(ls, 1, 255, str);
	LSParamString(ls, str, r0, r1, r2, r3);
	return 0;
}

CFStringRef LSCreateCFString(LongString *ls)
{
	return CFStringCreateWithBytes(NULL, &ls->data[1], ls->len, kCFStringEncodingMacRoman, false);
}

