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

#if TARGET_CARBON
#include <Memory.h>
#include <TextUtils.h>
#endif

#include "LongStrings.h"
#include <stdarg.h>

#pragma dont_inline off

#pragma optimization_level 4

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

#if !__POWERPC__
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
#else
asm pascal char LSCmp(register const LongString *ls1, register const LongString *ls2)
{
	#pragma unused(ls1, ls2)
	lhz	r5, 0(r3)		//Get the byte counts of both strings
	lhz	r6, 0(r4)

	cmpw	r5, r6		//If the two lengths aren't the same, fail immediately
	beq		@more
@fail:
	li			r3, 0
	blr
	
@more:
	cmpwi	r5, 1		//If the length is less than one succeed.
	blt		@succ
	
	//Test the first byte of each string.
	lbz		r6, 3(r3)
	lbz		r7, 3(r4)
	
	cmpw	r6, r7	//If not the same, fail
	bne		@fail
	
	subi		r5, r5, 1	//Decrement length to check by one
	addi		r3, r3, 4	//Increment strings to ls->data[2]
	addi		r4, r4, 4

	//If the length is <= 12, we can complete the comparison in one pass,
	//using r6..r8 and r10..r12 as scratch registers
	cmpwi   r5, 12
	ble+    @lessThanTwelveBytes

@twelveOrMoreBytes:
	// OK...more than 1 bytes to compare.  We'll compare 12 bytes and then see if we can finish.

	lwz		r6, 0(r3)
	lwz		r7, 4(r3)
	lwz		r8, 8(r3)

	lwz		r10, 0(r4)
	lwz		r11, 4(r4)
	lwz		r12, 8(r4)

	subi		r5, r5, 12		//Decrement size
	addi		r3, r3, 12		//Increment strings by 12
	addi		r4, r4, 12

	//Compare each of the register pairs R6/R10, R7/R11, R8/R12.  If any differs, exit.
	cmp		cr1, 0, r6, r10
	cmp		cr2, 0, r7, r11
	cmp		cr3, 0, r8, r12

	bne		cr1, @fail
	bne		cr2, @fail
	bne		cr3, @fail

	cmpwi	r0, 12		//If we have more than 12, go back up and do the next 16. Else, continue...
	bge		@twelveOrMoreBytes

@lessThanTwelveBytes:
	mtcrf	1, r5		//set CR7 with 8/4/2/1 testers
	bf			28, @test4	//if less than 8 left, skip
	
	lwz		r6, 0(r3)
	lwz		r7, 4(r3)
	
	lwz		r10, 0(r4)
	lwz		r11, 4(r4)
	
	addi		r3, r3, 8
	addi		r4, r4, 8

	cmp		cr1, 0, r6, r10;
	cmp		cr2, 0, r7, r11
	
	bne		cr1, @fail
	bne		cr2, @fail

@test4:
	bf			29, @test2	//if less than 4 left, skip

	lwz		r6, 0(r3)
	lwz		r10, 0(r4)

	addi		r3, r3, 4
	addi		r4, r4, 4

	cmp		cr1, 0, r6, r10;
	bne		cr1, @fail

@test2:
	bf			30, @test1	//if less than 2 left, skip

	lhz		r6, 0(r3)
	lhz		r10, 0(r4)

	addi		r3, r3, 2
	addi		r4, r4, 2

	cmp		cr1, 0, r6, r10;
	bne		cr1, @fail

@test1:
	bf			31, @succ		//if less than 1 left, skip
	
	lbz		r6, 0(r3)
	lbz		r10, 0(r4)
	
	cmp		cr1, 0, r6, r10;
	bne		cr1, @fail

/*
	// We complete the comparison here, comparing between 1 and 11 bytes. If r5 is zero, we succeed immediately.
	cmpwi	r5, 0
	beq		@succ
	
	mr		r0,r5				//Copy length to r0
	mtxer	r0						//Copy the length to XER register.

	lswx		r5, r0, r3			//Load the strings into R5..R7 and R9..R11.  
	lswx		r9, r0, r4

	cmp		cr0, 0, r5, r9	//Compare R5 against R9; if they differ, fail. Succed If r0 is <= 4.
	cmpi		cr1, 0, r0, 4

	bne		cr0, @fail
	ble		cr1, @succ

	cmp		cr0, 0, r6, r10		//Compare R6 against R10; fail if different.  Succeed if r0 <= 8.
	cmpi		cr1, 0, r0, 8

	bne		cr0, @fail
	ble		cr1, @succ

	cmpw	r7, r11		//Compare R7 against R11; fail if different.  succeed if same.
	bne		@fail
*/
		
@succ:
	li		 r3, 1
	blr
}
#endif

inline char inupc(char c);
inline char inupc(char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

#if !__POWERPC__
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
#else
asm pascal char LSCaseCmp(register const LongString *ls1, register const LongString *ls2)
{
	#pragma unused(ls1, ls2)
	lhz		r5, 0(r3)	//Get lengths
	lhz		r6, 0(r4)
	mtctr	r5
	cmpw	r5, r6		//s1[0] == s2[0]
	beq		@more		//if equal, branch

@notEqual:
	li			r3, 0		//else return 0
	blr						
	
@more
	lbz		r5, 3(r3)	//read next character and increment strings
	addi		r3, r3, 3
	lbz		r6, 3(r4)
	addi		r4, r4, 3
@loop:
	cmplwi	r5, 'a'		//See if first char is lowercase
	blt		@nextA
	cmplwi	r5, 'z'
	bgt		@nextA
	
	subi		r5, r5, 32	//If it is, uppercase

@nextA:
	cmplwi	r6, 'a'			//See if second char is lowercase
	blt		@nextB
	cmplwi	r6, 'z'
	bgt		@nextB
	
	subi		r6, r6, 32	//If it is, uppercase

@nextB:
	cmpw	r5, r6			//Compare two strings
	bne		@notEqual		//If not equal, need to return zero
	bdz		@finish			//decrement counter if zero, finish, otherwise, continue
	
	lbz		r5, 1(r3)	//read next character and increment strings
	addi		r3, r3, 1
	lbz		r6, 1(r4)
	addi		r4, r4, 1
	b			@loop

@finish:	
	li		r3, 1
	blr
}
#endif

pascal void LSDelete(LongString *ls, short firstpos, short lastpos)
{
	if(firstpos>0 && lastpos >= firstpos)
	{
		if(lastpos>ls->len)
			lastpos=ls->len;
		
		BlockMoveData(&ls->data[lastpos+1], &ls->data[firstpos], ls->len - firstpos);
		ls->len+=(-1)-lastpos+firstpos;
	}
}

pascal void LSCopy(const LongString *ls, short firstpos, short len, LongString *out)
{
	int i;
	
	if(firstpos>0)
	{
		if(len>(ls->len - firstpos+1))
			i=(ls->len-firstpos+1);
		else
			i=len;
		
		BlockMoveData(&ls->data[firstpos], &out->data[1], i);
		out->len=i;
	}
	else
		if(out!=ls)
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

pascal short LSPosCustom(ConstStr255Param s, const LongString *ls, short start)
{
	int i=s[0], max=ls->len-i+1;
	int x, x2, x3;
	const unsigned char* lsd = ls->data;
	char b;
	
	for(x=start;x<=max;x++)
	{
		b = 1;
		
		for(x2=1, x3 = x; b && (x2<=i);x2++, x3++)
			b=(s[x2]==lsd[x3]);
		
		if(b)
			return x;
	}
	return 0;
}

pascal short LSPos(ConstStr255Param s, const LongString *ls)
{
	int i=s[0], max=ls->len-i+1;
	int x, x2, x3;
	const unsigned char* lsd = ls->data;
	char b;
	
	for(x=1;x<=max;x++)
	{
		b = 1;
		
		for(x2=1, x3 = x; b && (x2<=i);x2++, x3++)
			b=(s[x2]==lsd[x3]);
		
		if(b)
			return x;
	}
	return 0;
}

pascal short LSPosCaseCustom(ConstStr255Param s, const LongString *ls, short start)
{
	int i=s[0], max=ls->len-i+1;
	int x, x2, x3;
	const unsigned char* lsd = ls->data;
	char b;
	
	for(x=start;x<=max;x++)
	{
		b = 1;
		
		for(x2=1, x3 = x; b && (x2<=i);x2++, x3++)
			b=(inupc(s[x2])==inupc(lsd[x3]));
		
		if(b)
			return x;
	}
	return 0;
}

pascal short LSPosCase(ConstStr255Param s, const LongString *ls)
{
	int i=s[0], max=ls->len-i+1;
	int x, x2, x3;
	const unsigned char* lsd = ls->data;
	char b;
	
	for(x=1;x<=max;x++)
	{
		b = 1;
		
		for(x2=1, x3 = x; b && (x2<=i);x2++, x3++)
			b=(inupc(s[x2])==inupc(lsd[x3]));
		
		if(b)
			return x;
	}
	return 0;
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

#if !__POWERPC__
pascal short LSPosChar(short c, const LongString *ls)
{
	int i=1;
	
	for(;i<=ls->len;i++)
		if(ls->data[i]==(char)c)
			return i;
	
	//else
	return 0;
}
#else
asm pascal short LSPosChar(register short c, register const LongString *ls)
{
	#pragma unused(c, ls)
	lhz		r0, 0(r4)				//copy length to r0
	rlwinm	r5, r3, 0, 24, 31	//copy low-8 of r3 to r5
	
	cmpwi	r0, 1					//If length < 1, return false
	blt 		@notFound	
	
	li			r3, 1					//start at pos 1 in r3
	mtctr	r0							//Set counter to length
	
	addi		r4, r4, 3				//move to ls->data[1] = sizeof(short) + sizeof(char)
@loop
	lbz		r0, 0(r4)				//Load character
	cmplw	r5, r0					//compare character
	beqlr									//If they match, return
	
	addi		r3, r3, 1				//Increment the count
	addi		r4, r4, 1				//increment to next position in string
	bdnz		@loop
	
@notFound:	
	li			r3, 0					//return 0
	blr
}
#endif

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
	BlockMoveData(&s[1], &out->data[1], x);					// This way, we can allow out = ls.
	out->len=i+x;
}

pascal void LSConcatStrAndStr(ConstStr255Param s1, ConstStr255Param s2, LongString *out)
{
	short x=s1[0], y=s2[0];
	
	out->len=x+y;
	BlockMoveData(&s1[1], &out->data[1], x);
	BlockMoveData(&s2[1], &out->data[x+1], y);
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
	BlockMoveData(&s1[1], &out->data[1], i);
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

	BlockMoveData(&s1[1], &out->data[1], x);
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

	BlockMoveData(&s1[1], &out->data[1], x);
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
	
	va_start(args,n);
	
	if(n<1)
	{
		out->len=0;
		return;
	}
	
	len=0;
	va_arg(args, StringPtr);
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