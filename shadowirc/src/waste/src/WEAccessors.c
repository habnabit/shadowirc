/*
 *	WEAccessors.c
 *
 *	WASTE PROJECT
 *  Accessors
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

pascal SInt32 WEOffsetToLine (SInt32 offset, WEHandle hWE)
{
	// given a byte offset into the text, find the corresponding line index

	WEPtr pWE = *hWE;
	WELineRec *pLines = *pWE->hLines;
	SInt32 minIndex = 0;
	SInt32 maxIndex = pWE->nLines;
	SInt32 index = 0;

	// binary-search the line array

	while (minIndex < maxIndex)
	{
		index = (minIndex + maxIndex) >> 1;
		if (offset >= pLines[index].lineStart)
		{
			if (offset < pLines[index + 1].lineStart)
			{
				break;
			}
			else
			{
				minIndex = index + 1;
			}
		}
		else
		{
			maxIndex = index;
		}
	}

	return index;
}

pascal SInt32 WECountLines(WEHandle hWE)
{
	return (*hWE)->nLines;
}

pascal void WEGetLineRange(SInt32 lineIndex, SInt32 *lineStart, SInt32 *lineEnd, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	WELineRec *pLine;

	pLine = *pWE->hLines + _WEPinInRange(lineIndex, 0, pWE->nLines - 1);
	if (lineStart != nil)
	{
		*lineStart = pLine[0].lineStart;
	}
	if (lineEnd != nil)
	{
		*lineEnd = pLine[1].lineStart;
	}
}

pascal SInt32 WEOffsetToRun (SInt32 offset, WEHandle hWE)
{
	// given a byte offset into the text, find the corresponding style run index

	WEPtr pWE = *hWE;
	WERunArrayEntry *pRuns = *pWE->hRuns;
	SInt32 minIndex = 0;
	SInt32 maxIndex = pWE->nRuns;
	SInt32 index = 0;

	// binary-search the style run array

	while (minIndex < maxIndex)
	{
		index = (minIndex + maxIndex) >> 1;
		if (offset >= pRuns[index].runStart)
		{
			if (offset < pRuns[index + 1].runStart)
			{
				break;
			}
			else
			{
				minIndex = index + 1;
			}
		}
		else
		{
			maxIndex = index;
		}
	}

	return index;
}

pascal SInt32 WECountRuns(WEHandle hWE)
{
	return (*hWE)->nRuns;
}

pascal void WEGetRunRange(SInt32 runIndex, SInt32 *runStart, SInt32 *runEnd, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	WERunArrayEntry *pRun;

	pRun = *pWE->hRuns + _WEPinInRange(runIndex, 0, pWE->nRuns - 1);
	if (runStart != nil)
	{
		*runStart = pRun[0].runStart;
	}
	if (runEnd != nil)
	{
		*runEnd = pRun[1].runStart;
		if (*runEnd > pWE->textLength)
		{
			*runEnd = pWE->textLength;
		}
	}
}

pascal SInt32 _WEPixelToLine(SInt32 vOffset, WEHandle hWE)
{
	// given a vertical pixel offset in local coordinates,
	// find the corresponding line index

	WEPtr pWE = *hWE;
	WELineRec *pLines = *pWE->hLines;
	SInt32 minIndex = 0;
	SInt32 maxIndex = pWE->nLines;
	SInt32 index = 0;

	// binary-search the line array

	while (minIndex < maxIndex)
	{
		index = (minIndex + maxIndex) >> 1;
		if (vOffset >= pLines[index].lineOrigin)
		{
			if (vOffset < pLines[index + 1].lineOrigin)
			{
				break;
			}
			else
			{
				minIndex = index + 1;
			}
		}
		else
		{
			maxIndex = index;
		}
	}

	return index;
}

pascal void _WEGetIndStyle(SInt32 runIndex, WERunInfo *info, WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	WERunArrayEntry *pRun;

	// get a pointer to the specified run array entry
	pRun = *pWE->hRuns + runIndex;

	// fill in the runStart and runEnd fields from the style run array
	info->runStart = pRun[0].runStart;
	info->runEnd = pRun[1].runStart;

	// copy the style information from the appropriate entry in the style table
	info->runAttrs = (*pWE->hStyles)[pRun->styleIndex].info;
}

pascal Boolean _WEGetIndDirection(SInt32 runIndex, WEHandle hWE)
{
	WEPtr pWE = *hWE;

	// the high bit of tsFlags is set if the corresponding run is right-to-left
	return BTST((*pWE->hStyles + (*pWE->hRuns + runIndex)->styleIndex)->info.runStyle.tsFlags, tsRightToLeft) ? true : false;
}

pascal Boolean WEGetRunDirection(SInt32 offset, WEHandle hWE)
{
	WEPtr pWE = *hWE;

	//	if offset is out of range, return the primary line direction
	if ((offset < 0) || (offset >= pWE->textLength))
	{
		return IsRightToLeft(pWE->direction);
	}

	return _WEGetIndDirection(WEOffsetToRun(offset, hWE), hWE);
}

pascal void WEGetRunInfo(SInt32 offset, WERunInfo *info, WEHandle hWE)
{
	_WEGetIndStyle(WEOffsetToRun(offset, hWE), info, hWE);
}

pascal SInt32 WEGetHeight(SInt32 startLine, SInt32 endLine, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	WELineRec *pLines = *pWE->hLines;
	SInt32 nLines = pWE->nLines;

	startLine = _WEPinInRange(startLine, 0, nLines);
	endLine = _WEPinInRange(endLine, 0, nLines);
	_WEReorder(&startLine, &endLine);
	return pLines[endLine].lineOrigin - pLines[startLine].lineOrigin;
}

pascal Handle WEGetText(WEHandle hWE)
{
	return (*hWE)->hText;
}

pascal SInt16 WEGetChar(SInt32 offset, WEHandle hWE)
{
	WEPtr pWE = *hWE;

	// sanity check: make sure offset is withing allowed bounds
	if ((offset < 0) || (offset >= pWE->textLength))
	{
		return 0;
	}

	// get the specified character (actually, byte)
	return * (UInt8 *) (*pWE->hText + offset);
}

pascal WEAlignment WEGetAlignment(WEHandle hWE)
{
	return (*hWE)->alignment;
}

pascal WEDirection WEGetDirection(WEHandle hWE)
{
	return (*hWE)->direction;
}

pascal void WEGetSelection(SInt32 *selStart, SInt32 *selEnd, WEHandle hWE)
{
	WEPtr pWE = *hWE;

	*selStart = pWE->selStart;
	*selEnd = pWE->selEnd;
}

pascal void WESetDestRect(const LongRect *destRect, WEHandle hWE)
{
	(*hWE)->destRect = *destRect;
}

pascal void WEGetDestRect(LongRect *destRect, WEHandle hWE)
{
	*destRect = (*hWE)->destRect;
}

pascal void WESetViewRect(const LongRect *viewRect, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	Rect r;

	pWE->viewRect = *viewRect;

	// keep the viewRgn in sync with the view rectangle
	WELongRectToRect(viewRect, &r);
	RectRgn(pWE->viewRgn, &r);
}

pascal void WEGetViewRect(LongRect *viewRect, WEHandle hWE)
{
	*viewRect = (*hWE)->viewRect;
}

pascal SInt32 WEGetTextLength(WEHandle hWE)
{
	return (*hWE)->textLength;
}

pascal SInt16 WEFeatureFlag(SInt16 feature, SInt16 action, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	SInt16 status;

	// get current status of the specified feature
	status = BTST(pWE->features, feature) ? weBitSet : weBitClear;

	// if action is weBitToggle, invert flag
	if (action == weBitToggle)
	{
		action = 1 - status;
	}

	// reset flag according to action
	if (action == weBitClear)
	{
		BCLR(pWE->features, feature);
	}
	else if (action == weBitSet)
	{
		BSET(pWE->features, feature);
	}

	// return old status
	return status;
}

pascal UInt32 WEVersion(void)
{
	return WASTE_VERSION ;
}
