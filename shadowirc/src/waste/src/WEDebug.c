/*
 *	WEDebug.c
 *
 *	WASTE PROJECT
 *  Debugging Routines
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

#if WASTE_DEBUG

pascal void _WESanityCheck(WEHandle hWE)
{
	// _WESanityCheck performs several checks on two key data structures:
	// the run array and the style table, verifying a number of assertions.
	// This routine made it possible to identify many subtle bugs during development.

	WEPtr pWE = *hWE;				// we aren't going to move memory
	SInt32 runIndex;
	SInt32 styleIndex;
	WERunArrayEntry *pRun;
	WEStyleTableEntry *pStyle;

	// check the consistency of the run array
	pRun = *pWE->hRuns;

	// first runStart must be zero
	WEASSERT(pRun->runStart == 0, "\pFirst run array entry is bad");

	for ( runIndex = 0; runIndex < pWE->nRuns; runIndex++ )
	{
		// all runs must be at least one character long
		WEASSERT((pRun[1].runStart - pRun[0].runStart) > 0, "\pRun length less than one");

		// no two consecutive runs may reference the same style
		WEASSERT(pRun[1].styleIndex != pRun[0].styleIndex, "\pSpurious run boundary");

		// all run array elements (except the last dummy entry) must reference an existing style
		WEASSERT((pRun->styleIndex >= 0) && (pRun->styleIndex < pWE->nStyles), "\pInvalid style index");
		pRun++;
	}

	// last (dummy) runStart must be textLength + 1 and styleIndex must be -1
	WEASSERT((pRun->runStart == pWE->textLength + 1) && (pRun->styleIndex == -1),
				"\pLast run array entry is bad");

	pStyle = *pWE->hStyles;
	for ( styleIndex = 0; styleIndex < pWE->nStyles; styleIndex++ )
	{
		SInt32 refCount;

		// the number of runs referencing each style in the style table
		// must match the style reference count
		refCount = 0;
		pRun = *pWE->hRuns;
		for ( runIndex = 0; runIndex < pWE->nRuns; runIndex++ )
		{
			if (pRun->styleIndex == styleIndex)
			{
				refCount++;
			}
			pRun++;
		}
		WEASSERT(pStyle->refCount == refCount, "\pBad style reference count");

		// there may not be two identical entries in the style table (except for unused entries)
		if (pStyle->refCount > 0)
		{
			WEStyleTableEntry *pStyle2;
			SInt32 styleIndex2;

			pStyle2 = pStyle;
			for ( styleIndex2 = styleIndex + 1; styleIndex2 < pWE->nStyles - 1; styleIndex2++ )
			{
				pStyle2++;
				if (pStyle2->refCount > 0)
				{
					WEASSERT(!_WEBlockCmp(&pStyle->info, &pStyle2->info, sizeof(WERunAttributes)),
							"\pDuplicate entry in style table");
				}
			}
		}
		pStyle++;
	}
}

#endif  // WASTE_DEBUG
