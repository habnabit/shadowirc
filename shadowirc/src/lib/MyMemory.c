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

	Note: This code based on code by Peter N. Lewis.
*/

#include "MyMemory.h"

//#if !__POWERPC__
void MZero(void* p, long size)
{
	if(p && size>0)
	{
		for(;size;size--)
			*(((char*)p)++) = 0;
	}
}
/*
#else
asm pascal void MZero(register void* p, register long size)
{
#pragma unused(p, size)
	cmplwi		r3, 0		//is r3 nil?
	beqlr							//return if equal
	cmpwi		r4, 0		//Is size <=0 ?
	blelr							//return if true
	li				r5, 0		//zero r5
//	beqlr		//huh???????????
	rlwinm.		r0, r4, 29, 3, 31	//right-shift r4 by 3, copy to r0, clear high-order bits, and set cr
	beq			@2			//skip to @2 if result is zero
	mtctr		r0				//copy r0 to counter

@0:	//clear eight bytes
	stw			r5, 0(r3)		//copy zero to r3
	stw			r5, 4(r3)		//copy zero to r3+4
	addi			r3, r3, 8		//r3 += 8
	bdnz			@0				//continue loop
	andi.			r4, r4, 7		//size &= 7
	bclr			12, 2			//return if size == 0
@2:
	mtctr		r4					//set counter

@1:
	stb			r5, 0(r3)		//write one byte
	addi			r3, r3, 1		//r3++
	bdnz			@1				//continue loop
	blr								//return
}
#endif
*/

void MFill(void* p, long size, short val)
{
	if(p && size>0)
	{
		for(;size;size--)
			*(((char*)p)++) = val;
	}
}

void MFillLong(void* p, long size, long val)
{
	if(p)
	{
		while(size>3)
		{
			*(long*)p = val;
			((long*)p)++;
			size-=4;
		}
	}
}
