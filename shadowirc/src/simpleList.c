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