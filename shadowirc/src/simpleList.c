/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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

#include "simpleList.h"

//For CopyPascalStringToC -- eeww
#include <Carbon/Carbon.h>

#include <stdlib.h>
#include <string.h>

simpleListPtr NewSimpleList(int (*compareFn)(const void *, const void *))
{
	simpleListPtr sl = malloc(sizeof(simpleList));
	
	sl->eltCount = 0;
	sl->compareFn = compareFn;
	sl->elts = 0;
	
	return sl;
}

void FreeSimpleList(simpleListPtr sl)
{
	if(sl->elts)
		free(sl->elts);
	
	sl->eltCount = 0;
	sl->compareFn = 0;
	sl->elts = malloc(0);
	
	free(sl);
}

void SLAddItem(simpleListPtr sl, void* item)
{
	sl->eltCount++;
	sl->elts = realloc(sl->elts, sl->eltCount * sizeof(void*));
	sl->elts[sl->eltCount - 1] = item;
}

void SLSort(simpleListPtr sl)
{
	qsort(sl->elts, sl->eltCount, sizeof(void*), sl->compareFn);
}

//-

int SLPstrCaseCompare(const void* a, const void* b)
{
	unsigned char s1[256], s2[256];
	
	CopyPascalStringToC(a, s1);
	CopyPascalStringToC(b, s2);
	
	return strcmp(s1, s2);
}