/*
 *	WELowLevelEditing.c
 *
 *	WASTE PROJECT
 *  Low-Level Editing Routines
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

pascal Boolean _WEIsWordRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	// _WEIsWordRange returns true if the specified range is a word range,
	// i.e. if it would be possible to select it by double-clicking and (optionally) dragging.

	SInt32 wordStart, wordEnd;

	// determine if rangeStart is at the beginning of a word
	WEFindWord(rangeStart, kLeadingEdge, &wordStart, &wordEnd, hWE);
	if (rangeStart == wordStart)
	{

		// determine if rangeEnd is at the end of a word
		WEFindWord(rangeEnd, kTrailingEdge, &wordStart, &wordEnd, hWE);
		return (rangeEnd == wordEnd);
	}
	return false;
}

pascal Boolean _WEIsPunct(SInt32 offset, WEHandle hWE)
{
	SInt16 cType;

	cType = WECharType(offset, hWE);
	if ((cType & smcTypeMask) == smCharPunct)
	{
		cType &= smcClassMask;
		if ((cType == smPunctNormal) || (cType == smPunctBlank))
		{
			return true;
		}
	}
	return false;
}

pascal void _WEIntelligentCut(SInt32 *rangeStart, SInt32 *rangeEnd, WEHandle hWE)
{

	// _WEIntelligentCut is called by other WASTE routines to determine the actual
	// range to be deleted when weFIntCutAndPaste is enabled.
	// On entry, rangeStart and rangeEnd specify the selection range visible to the user.
	// On exit, rangeStart and rangeEnd specify the actual range to be removed.

	// do nothing if the intelligent cut-and-paste feature is disabled
	if (!BTST((*hWE)->features, weFIntCutAndPaste))
	{
		return;
	}

	// intelling cut-&-paste rules should be applied only to word ranges...
	if (!_WEIsWordRange(*rangeStart, *rangeEnd, hWE))
	{
		return;
	}

	// ...without punctuation characters at the beginning or end
	if (_WEIsPunct(*rangeStart, hWE))
	{
		return;
	}
	if (_WEIsPunct(*rangeEnd - 1, hWE))
	{
		return;
	}

	// if the character preceding the selection range is a space, discard it
	if (WEGetChar(*rangeStart - 1, hWE) == kSpace)
	{
		(*rangeStart)--;
	}
	// else, if the character following the selection range is a space, discard it
	else if (WEGetChar(*rangeEnd, hWE) == kSpace)
	{
		(*rangeEnd)++;
	}
}

pascal SInt16 _WEIntelligentPaste(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	SInt16 retval;

	// _WEIntelligentPaste is called by other WASTE routines to determine whether
	// an additional space character should be added (before or after) after inserting
	// new text (usually from the Clipboard or from a drag).

	retval = weDontAddSpaces;

	// do nothing unless the intelligent cut-and-paste feature is enabled
	if (!BTST((*hWE)->features, weFIntCutAndPaste))
	{
		return retval;
	}

	// extra spaces will be added only if the pasted text looks like a word range,
	// without punctuation characters at the beginning or at the end
	if (_WEIsPunct(rangeStart, hWE))
	{
		return retval;
	}
	if (_WEIsPunct(rangeEnd - 1, hWE))
	{
		return retval;
	}

	// if the character on the left of the pasted text is a punctuation character
	// and the character on the right isn't,  add a space on the right, and vice versa
	if (_WEIsPunct(rangeStart - 1, hWE))
	{
		if (!_WEIsPunct(rangeEnd, hWE))
		{
			retval = weAddSpaceOnRightSide;
		}
	}
	else if (_WEIsPunct(rangeEnd, hWE))
	{
		retval = weAddSpaceOnLeftSide;
	}

	return retval;
}

pascal OSErr _WEInsertRun(SInt32 runIndex, SInt32 offset, SInt32 styleIndex, WEPtr pWE)
{

	// Insert a new entry in the style run array, at the specified runIndex position.
	// The new entry consists of the pair <offset, styleIndex>.

	WERunArrayEntry entry;
	OSErr err;

	// prepare the entry record to be inserted in the array
	entry.runStart = offset;
	entry.styleIndex = styleIndex;

	// do the insertion
	if ((err = _WESplice((Handle) pWE->hRuns, &entry, sizeof(entry), (runIndex + 1) * sizeof(entry))) != noErr)
	{
		return err;
	}

	// increment style run count
	pWE->nRuns++;

	// increment the reference count field of the style table entry
	// referenced by the newly inserted style run
	(*pWE->hStyles)[styleIndex].refCount++;

	return noErr;
}

pascal void _WERemoveRun(SInt32 runIndex, WEPtr pWE)
{
	WEStyleTableEntry *pStyle;

	// get a pointer to the style table entry referenced by the style run
	pStyle = *pWE->hStyles + (*pWE->hRuns)[runIndex].styleIndex;

	// decrement the reference count field of the style table entry
	// referenced by the style run to be removed
	pStyle->refCount--;

#if WASTE_OBJECTS
	// dispose of the embedded object (if any)
	if (pStyle->info.runStyle.tsObject != nil)
	{
		_WEFreeObject(pStyle->info.runStyle.tsObject);
	}
#endif

	// remove a "slot" from the run array
	_WESplice((Handle) pWE->hRuns, nil, - sizeof(WERunArrayEntry), runIndex * sizeof(WERunArrayEntry));

	// decrement style run count
	pWE->nRuns--;
}

pascal void _WEChangeRun(SInt32 runIndex, SInt32 newStyleIndex, Boolean keepOld, WEPtr pWE)
{
#if !WASTE_OBJECTS
	#pragma unused(keepOld)
#endif
	// change the styleIndex field of the specified entry of the style run array

	SInt32 oldStyleIndex;
	WEStyleTableEntry *oldStyle, *newStyle;

	// do the change
	oldStyleIndex = (*pWE->hRuns)[runIndex].styleIndex;
	(*pWE->hRuns)[runIndex].styleIndex = newStyleIndex;

	// get pointers to old and new style table elements
	oldStyle = *pWE->hStyles + oldStyleIndex;
	newStyle = *pWE->hStyles + newStyleIndex;

	// increment the reference count field of the new style table entry
	newStyle->refCount++;

	// decrement the reference count field of the old style table entry
	oldStyle->refCount--;

#if WASTE_OBJECTS
	// dispose of the embedded object (if any) unless it is again referenced in the new style
	if (!keepOld)
	{
		WEObjectDescHandle oldObject = oldStyle->info.runStyle.tsObject;

		if ((oldObject != nil) && (oldObject != newStyle->info.runStyle.tsObject))
		{
			_WEFreeObject(oldObject);
		}
	}
#endif
}

pascal OSErr _WENewStyle(const WETextStyle *ts, SInt32 *styleIndex, WEPtr pWE)
{
	// given the specified WETextStyle record, find the corresponding entry
	// in the style table (create a new entry if necessary), and return its index

	WEStyleTableEntry *pEntry;
	WEStyleTableEntry entry;
	SInt32 index;
	SInt32 unusedIndex;
	OSErr err;

	// see if the given style already exists in the style table
	// while scanning the table, also remember the position of the first unused style, if any
	unusedIndex = -1;
	pEntry = *pWE->hStyles;
	for ( index = 0 ; index < pWE->nStyles ; index++ )
	{
		// check for entries which aren't referenced and can be recycled
		if (pEntry->refCount == 0)
		{
			unusedIndex = index;
		}

		// perform a bitwise comparison between the current entry and the specified style
		// (actually, we ignore metrics information)
		else if (_WEBlockCmp(&pEntry->info.runStyle, ts, sizeof(WETextStyle)))
		{
			// found: style already present
			*styleIndex = index;
			return noErr;
		}
		pEntry++;
	} // for

	// the specified style doesn't exist in the style table
	// since this is a new entry, we have to calculate font metrics information
	entry.info.runStyle = *ts;
	_WEFillFontInfo(pWE->port, &entry.info);

	// see if we can recycle an unused entry
	if (unusedIndex >= 0)
	{
		index = unusedIndex;
		(*pWE->hStyles)[index].info = entry.info;
	}
	else
	{
		// no reusable entry: we have to append a new entry at the end of the table
		entry.refCount = 0;
		if ((err = _WESplice((Handle) pWE->hStyles, &entry, sizeof(entry), -1)) != noErr)
		{
			return err;
		}

		// update style count in the WE record
		pWE->nStyles++;
	}

	// return the index to the new entry
	*styleIndex = index;
	return noErr;
}

pascal OSErr _WERedraw(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	WEPtr pWE = *hWE;		// assume WE record is already locked
	WELineRec *pLines;
	SInt32 startLine, endLine;
	SInt32 oldTextHeight, newTextHeight;
	LongRect r;
	Rect viewRect, updateRect;
	RgnHandle saveClip, auxRgn;
	GrafPtr savePort;
	OSErr err;

	// do nothing if recalculation has been inhibited
	if (!BTST(pWE->features, weFInhibitRecal))
	{
		// hide the caret
		if (BTST(pWE->flags, weFCaretVisible))
		{
			_WEBlinkCaret(hWE);
		}

		// remember total text height
		oldTextHeight = pWE->destRect.bottom - pWE->destRect.top;

		// find line range affected by modification
		startLine = WEOffsetToLine(rangeStart, hWE);
		endLine = WEOffsetToLine(rangeEnd, hWE);

		// recalculate line breaks starting from startLine
		if ((err = _WERecalBreaks(&startLine, &endLine, hWE)) != noErr)
		{
			goto cleanup;
		}

		// recalculate slops
		_WERecalSlops(startLine, endLine, hWE);

		// do nothing if redrawing has been inhibited
		if (!BTST(pWE->features, weFInhibitRedraw))
		{
			// calculate new total text height
			newTextHeight = pWE->destRect.bottom - pWE->destRect.top;

			// calculate the rectangle to redraw (in long coordinates)
			r.left = -SHRT_MAX;
			r.right = SHRT_MAX;
			pLines = *pWE->hLines;
			r.top = pLines[startLine].lineOrigin;

			// if total text height hasn't changed, it's enough to redraw lines up to endLine
			// otherwise we must redraw all lines from startLine on
			if ((newTextHeight == oldTextHeight) && (endLine < pWE->nLines - 1))
			{
				r.bottom = pLines[endLine + 1].lineOrigin;
			}
			else if (newTextHeight < oldTextHeight)
			{
				r.bottom = oldTextHeight;
			}
			else
			{
				r.bottom = newTextHeight;
			}

			WEOffsetLongRect(&r, 0, pWE->destRect.top);

			// calculate the intersection between this rectangle and the view rectangle
			WELongRectToRect(&r, &updateRect);
			WELongRectToRect(&pWE->viewRect, &viewRect);

			if (SectRect(&updateRect, &viewRect, &updateRect))
			{
				// set up the port and the clip region
				GetPort(&savePort);
				SetPort(pWE->port);

				// restrict the clip region to updateRect
				saveClip = NewRgn();
				GetClip(saveClip);
				auxRgn = NewRgn();
				RectRgn(auxRgn, &updateRect);
				SectRgn(saveClip, auxRgn, auxRgn);
				SetClip(auxRgn);
				DisposeRgn(auxRgn);

				// we only really need to redraw the visible lines
				startLine = _WEPixelToLine(updateRect.top - pWE->destRect.top, hWE);
				endLine = _WEPixelToLine(updateRect.bottom - pWE->destRect.top - 1, hWE);

				// redraw the lines (pass true in the doErase parameter)
				_WEDrawLines(startLine, endLine, true, hWE);

				// erase the portion of the update rectangle below the last line (if any)
				pLines = *pWE->hLines;
				updateRect.top = pWE->destRect.top + pLines[endLine + 1].lineOrigin;
				if (updateRect.top < updateRect.bottom)
				{
					CallWEEraseProc(&updateRect, hWE, pWE->eraseHook);
				}

				// restore the clip region
				SetClip(saveClip);
				DisposeRgn(saveClip);

				// restore the port
				SetPort(savePort);

				// redraw the caret or the selection range
				if (pWE->selStart < pWE->selEnd)
				{
					_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);
				}
				else
				{
					_WEBlinkCaret(hWE);
				}
			} // if SectRect

			// scroll the selection range into view
			WESelView(hWE);

		} // if redraw not inhibited
	} // if recal not inhibited

	// clear result code
	err = noErr;

cleanup:
	// return result code
	return err;
}

pascal OSErr WECalText(WEHandle hWE)
{
	Boolean saveWELock;
	OSErr err;

	// lock WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);

#if WASTE_WECALTEXT_DOES_REDRAW

	// recalculate line breaks & slops and redraw the text
	err = _WERedraw(0, LONG_MAX, hWE);

#else

	{
		SInt32 startLine = 0;
		SInt32 endLine = LONG_MAX;

		// recalculate line breaks & slops without redrawing anything
		if ((err = _WERecalBreaks(&startLine, &endLine, hWE)) == noErr)
		{
			_WERecalSlops(startLine, endLine, hWE);
		}
	}

#endif

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

pascal OSErr _WESetStyleRange(SInt32 rangeStart, SInt32 rangeEnd, WEStyleMode mode, const WETextStyle *ts, WEHandle hWE)
{
	// alter the style attributes of the specified text range according to ts and mode

	WEPtr pWE = *hWE;					// assume WE record is already locked
	WERunArrayHandle hRuns = pWE->hRuns;
	SInt32 offset;
	SInt32 runIndex;
	SInt32 oldStyleIndex, newStyleIndex;
	WERunInfo runInfo;
	Style continuousStyles;
	OSErr err;

    WEASSERT(((rangeStart < rangeEnd) || ((rangeStart == 0) && (rangeEnd == 0) && (pWE->textLength == 0))), "\pBad style range");

	// if mode contains weDoToggleFace, we need to determine which Quickdraw styles
	// are continuous over the specified text range: those styles must be turned off
	if (BTST(mode, kModeToggleFace))
	{
		WEStyleMode temp = weDoFace;
		_WEContinuousStyleRange(rangeStart, rangeEnd, &temp, &runInfo.runAttrs.runStyle, hWE);
		continuousStyles = runInfo.runAttrs.runStyle.tsFace;
	}
	else
	{
		continuousStyles = normal;
	}

	// find the index to the first style run in the specified range
	offset = rangeStart;
	runIndex = WEOffsetToRun(offset, hWE);

	// run thru all the style runs that encompass the selection range
	do
	{
		// find style index for this run and retrieve corresponding style attributes
		oldStyleIndex = (*hRuns)[runIndex].styleIndex;
		_WEGetIndStyle(runIndex, &runInfo, hWE);

		// _WEGetIndStyle returns textLength + 1 in runInfo.runEnd for the last style run:
		// correct this anomaly (which is useful for other purposes, anyway)
		if (runInfo.runEnd > pWE->textLength)
		{
			runInfo.runEnd = pWE->textLength;
		}

		// apply changes to existing style attributes as requested
		_WECopyStyle(ts, &runInfo.runAttrs.runStyle, continuousStyles, mode);

		// the high bit of tsFlags must be set if the font
		// belongs to a right-to-left script system
		if (BTST(pWE->flags, weFBidirectional))
		{
			if (GetScriptVariable(FontToScript(runInfo.runAttrs.runStyle.tsFont), smScriptRight) != 0)
			{
				BSET(runInfo.runAttrs.runStyle.tsFlags, tsRightToLeft);
			}
		}

		// get a style index for the new text style
		if ((err = _WENewStyle(&runInfo.runAttrs.runStyle, &newStyleIndex, pWE)) != noErr)
		{
			goto cleanup;
		}

		// if offset falls on a style boundary and this style run has become identical
		// to the previous one, merge the two runs together
		if ((offset == runInfo.runStart) && (runIndex > 0) &&
			((*hRuns)[runIndex - 1].styleIndex == newStyleIndex))
		{
			_WERemoveRun(runIndex, pWE);
			runIndex--;
		}

		// style index changed?
		if (oldStyleIndex != newStyleIndex)
		{
			// if offset is in the middle of a style run, insert a new style run in the run array
			if (offset > runInfo.runStart)
			{
				if ((err = _WEInsertRun(runIndex, offset, newStyleIndex, pWE)) != noErr)
				{
					goto cleanup;
				}
				runIndex++;
			}
			else
			{
				// otherwise just change the styleIndex field of the current style run entry
				_WEChangeRun(runIndex, newStyleIndex, (rangeEnd < runInfo.runEnd), pWE);
			}

			// if specified range ends in the middle of a style run, insert yet another entry
			if (rangeEnd < runInfo.runEnd)
			{
				if ((err = _WEInsertRun(runIndex, rangeEnd, oldStyleIndex, pWE)) != noErr)
				{
					goto cleanup;
				}
			}
		} // if oldStyle != newStyle

		// go to next style run
		runIndex++;
		offset = runInfo.runEnd;

	} while (offset < rangeEnd);

	// if the last style run ends exactly at the end of the specified range,
	// see if we can merge it with the following style run
	if ((offset == rangeEnd) && (runIndex < pWE->nRuns) &&
		((*hRuns)[runIndex].styleIndex == newStyleIndex))
	{
		_WERemoveRun(runIndex, pWE);
	}

	// clear result code
	err = noErr;

cleanup:
	// return result code
	return err;
}

pascal OSErr _WEApplyStyleScrap(SInt32 rangeStart, SInt32 rangeEnd, myStScrpHandle styleScrap, WEHandle hWE)
{
	// apply the given style scrap to the specified text range

	TEStyleScrapElement *pEntry;
	Size styleScrapSize;
	SInt32 runStart, runEnd;
	SInt32 entryCount, index;
	WETextStyle ts;
	OSErr err;

	// sanity check: make sure the style scrap handle is a reasonable size
	err = weCorruptDataErr;
	styleScrapSize = GetHandleSize((Handle) styleScrap) - sizeof(SInt16);
	if (styleScrapSize < 0)
	{
		goto cleanup;		// too short: header is missing
	}

	// calculate entry count based on handle size
	entryCount = styleScrapSize / sizeof(myScrpSTElement);
	if (styleScrapSize != entryCount * sizeof(myScrpSTElement))
	{
		goto cleanup;		// not an integral number of entries
	}

	// the calculated entry count must match scrpNStyles,
	// unless it's larger than 32,766
	if (entryCount < SHRT_MAX)
	{
		if (entryCount != (*styleScrap)->scrpNStyles)
		{
			goto cleanup;	// invalid entry count
		}
	}

	// loop through each entry of the style scrap
	for ( index = 0; index < entryCount; index++ )
	{
		// get a pointer to the current scrap entry
		pEntry = (TEStyleScrapElement *) ((*styleScrap)->scrpStyleTab + index);

		// calculate text run to which this entry is to be applied
		runStart = rangeStart + pEntry->scrpStartChar;
		if (index < entryCount - 1)
		{
			runEnd = rangeStart + pEntry[1].scrpStartChar;
		}
		else
		{
			runEnd = rangeEnd;
		}

		// perform some range checking
		if (runEnd > rangeEnd)
		{
			runEnd = rangeEnd;
		}
		if (runStart >= runEnd)
		{
			continue;
		}

		// copy style to a local variable in case memory moves
		* (myTextStyle *) &ts = pEntry->scrpTEAttrs.runTEStyle;

		// apply the specified style to the range
		if ((err = _WESetStyleRange(runStart, runEnd, weDoAll + weDoReplaceFace, &ts, hWE)) != noErr)
		{
			goto cleanup;
		}
	}

	// clear result code
	err = noErr;

cleanup:
	// return result code
	return err;
}

#if WASTE_OBJECTS

pascal OSErr _WEApplySoup(SInt32 offset, Handle hSoup, WEHandle hWE)
{
	WESoup soup;
	Ptr pSoup, pSoupEnd;
	WETextStyle ts;
	Handle hObjectData;
	SInt32 objectOffset;
	Boolean saveWELock;
	OSErr err;

	BLOCK_CLR(ts);
	hObjectData = nil;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);

	// lock the soup in high heap
	HLockHi(hSoup);
	pSoup = *hSoup;
	pSoupEnd = pSoup + GetHandleSize(hSoup);

	// loop through each object descriptor in the soup
	while (pSoup < pSoupEnd)
	{
		// Object descriptors may be aligned to odd addresses (duh!)
		// this isn't a problem on 68020+ and PPC CPUs, but causes
		// a fatal address error on the 68000.  To avoid this, we
		// copy the descriptor to the stack with BlockMoveData()
		// before trying to access its fields.
		BlockMoveData(pSoup, &soup, sizeof(soup));

		// if soupDataSize is negative, this soup is a special type that we won't handle here
		if (soup.soupDataSize < 0)
		{
			continue;
		}

		// create a new relocatable block the hold the object data
		if ((err = _WEAllocate(soup.soupDataSize, kAllocTemp, &hObjectData)) != noErr)
		{
			goto cleanup;
		}

		// copy the object data to this block
		BlockMoveData(pSoup + sizeof(soup), *hObjectData, soup.soupDataSize);

		// create a new object out of the tagged data
		if ((err = _WENewObject(soup.soupType, hObjectData, hWE, &ts.tsObject)) != noErr)
		{
			goto cleanup;
		}

		// if there was no new handler for this object, use the object size stored in the soup
		if ((*ts.tsObject)->objectTable == nil)
		{
			(*ts.tsObject)->objectSize = soup.soupSize;
		}

		// record a reference to the object descriptor in the style table
		objectOffset = soup.soupOffset + offset;
		err = _WESetStyleRange(objectOffset, objectOffset + 1, weDoObject, &ts, hWE);
		hObjectData = nil;
		ts.tsObject = nil;
		if (err != noErr)
		{
			goto cleanup;
		}

		// advance soup pointer
		pSoup += sizeof(soup) + soup.soupDataSize;

	} // while

	// clear result code
	err = noErr;

cleanup:
	// clean up
	HUnlock(hSoup);
	_WEForgetHandle((Handle *) &ts.tsObject);
	_WEForgetHandle(&hObjectData);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

#endif	// WASTE_OBJECTS

pascal void _WEBumpRunStart(SInt32 runIndex, SInt32 deltaRunStart, WEPtr pWE)
{
	// add deltaLineStart to the lineStart field of all line records
	// starting from lineIndex

	WERunArrayEntry *pRun = *pWE->hRuns + runIndex;
	SInt32 nRuns = pWE->nRuns;

	// loop through the style run array adjusting the runStart fields
	for ( ; runIndex <= nRuns; runIndex++ )
	{
		pRun->runStart += deltaRunStart;
		pRun++;
	}
}

pascal void _WERemoveRunRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	// the range of text between rangeStart and rangeEnd is being deleted
	// update the style run array (and the style table) accordingly

	WEPtr pWE = *hWE;	// assume WE record is already locked
	WERunArrayEntry *pRun;
	SInt32 startRun, endRun;

	// find the index to the first and last style runs in the specified range
	startRun = WEOffsetToRun(rangeStart, hWE);
	endRun = WEOffsetToRun(rangeEnd, hWE) - 1;

	// remove all style runs between startRun and endRun
	for ( ; endRun > startRun; endRun-- )
	{
		_WERemoveRun(endRun, pWE);
	}

	// move back all subsequent style runs
	_WEBumpRunStart(startRun + 1, rangeStart - rangeEnd, pWE);

	if ((endRun == startRun) && (endRun < pWE->nRuns - 1))
	{
		pRun = *pWE->hRuns + endRun;
		pRun[1].runStart = rangeStart;
	}

	// remove the first style run if is has become zero length
	pRun = *pWE->hRuns + startRun;
	if (pRun[0].runStart == pRun[1].runStart)
	{
		_WERemoveRun(startRun, pWE);
		startRun--;
	}

	// merge the first and last runs if they have the same style index
	if (startRun >= 0)
	{
		pRun = *pWE->hRuns + startRun;
		if (pRun[0].styleIndex == pRun[1].styleIndex)
		{
			_WERemoveRun(startRun + 1, pWE);
		}
	}
}

pascal void _WEBumpLineStart(SInt32 lineIndex, SInt32 deltaLineStart, WEPtr pWE)
{
	// add deltaLineStart to the lineStart field of all line records
	// starting from lineIndex

	WELineRec *pLine = *pWE->hLines + lineIndex;
	SInt32 nLines = pWE->nLines;

	// loop through the line array adjusting the lineStart fields
	for ( ; lineIndex <= nLines; lineIndex++ )
	{
		pLine->lineStart += deltaLineStart;
		pLine++;
	}
}

pascal void _WERemoveLineRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	// the range of text between rangeStart and rangeEnd is being deleted
	// update the line array accordingly

	WEPtr pWE = *hWE;	// assume WE record is already locked
	SInt32 startLine, nLines;

	// remove all line records between rangeStart and rangeEnd
	startLine = WEOffsetToLine(rangeStart, hWE) + 1;
	nLines = WEOffsetToLine(rangeEnd, hWE) - startLine + 1;

	_WESplice((Handle) pWE->hLines, nil, - (nLines * sizeof(WELineRec)), startLine * sizeof(WELineRec));
	pWE->nLines -= nLines;

	// update the lineStart field of all the line records that follow
	_WEBumpLineStart(startLine, rangeStart - rangeEnd, pWE);
}

pascal OSErr _WEDeleteRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	// used internally to delete a text range
	WEPtr pWE = *hWE;	// assume WE record is already locked
	WERunInfo runInfo;
	SInt32 delta;
	OSErr err = noErr;

	if (rangeEnd > pWE->textLength)
	{
		rangeEnd = pWE->textLength;
	}

	// do nothing if the specified range is empty
	if ((delta = (rangeStart - rangeEnd)) == 0)
	{
		goto cleanup;
	}

	// save the first style in the specified range in nullStyle
	WEGetRunInfo(rangeStart, &runInfo, hWE);
	pWE->nullStyle = runInfo.runAttrs;
	BSET(pWE->flags, weFUseNullStyle);

#if WASTE_OBJECTS
	// special case: if we're deleting up to the end of the text, see whether
	// there's an embedded object at the very end and remove it
	if (rangeEnd == pWE->textLength)
	{
		WEGetRunInfo(rangeEnd - 1, &runInfo, hWE);
		if (runInfo.runAttrs.runStyle.tsObject != nil)
		{
			runInfo.runAttrs.runStyle.tsObject = nil;
			if ((err = _WESetStyleRange(rangeEnd - 1, rangeEnd, weDoObject, &runInfo.runAttrs.runStyle, hWE)) != noErr)
			{
				goto cleanup;
			}
		}
	}
#endif

	// remove all line records between rangeStart and rangeEnd
	_WERemoveLineRange(rangeStart, rangeEnd, hWE);

	// remove all style runs between rangeStart and rangeEnd
	_WERemoveRunRange(rangeStart, rangeEnd, hWE);

	// remove the text
	if ((err = _WESplice(pWE->hText, nil, delta, rangeStart)) != noErr)
	{
		goto cleanup;
	}

	// update textLength field
	pWE->textLength += delta;

	// we modified the text, so the anchor range (if any) is no longer valid
	pWE->clickCount = 0;

	// call the flux callback, if any
	if (pWE->fluxProc != nil)
	{
		CallWEFluxProc(rangeStart, delta, hWE, pWE->fluxProc);
	}

cleanup:
	// return result code
	return err;
}

pascal OSErr _WEInsertText(SInt32 offset, Ptr textPtr, SInt32 textLength, WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	WEStyleMode mode;
	OSErr err = noErr;

	// do nothing if textLength is zero or negative
	if (textLength <= 0)
	{
		goto cleanup;
	}

	// insert the text
	if ((err = _WESplice(pWE->hText, textPtr, textLength, offset)) != noErr)
	{
		goto cleanup;
	}

	// update the lineStart fields of all lines following the insertion point
	_WEBumpLineStart(WEOffsetToLine(offset, hWE) + 1, textLength, pWE);

	// update the runStart fields of all style runs following the insertion point
	_WEBumpRunStart(WEOffsetToRun(offset - 1, hWE) + 1, textLength, pWE);

	// update the textLength field
	pWE->textLength += textLength;

	// we modified the text, so the anchor range (if any) is no longer valid
	pWE->clickCount = 0;

	// make sure the newly inserted text doesn't reference any embedded object
#if WASTE_OBJECTS
	pWE->nullStyle.runStyle.tsObject = nil;
#endif
	mode = weDoObject;

	// if there is a valid null style, apply it to the newly inserted text
	if (BTST(pWE->flags, weFUseNullStyle))
	{
		mode += (weDoAll + weDoReplaceFace);
	}

	if ((err = _WESetStyleRange(offset, offset + textLength, mode, &pWE->nullStyle.runStyle, hWE)) != noErr)
	{
		goto cleanup;
	}

	// call the flux callback, if any
	if (pWE->fluxProc != nil)
	{
		CallWEFluxProc(offset, textLength, hWE, pWE->fluxProc);
	}

cleanup:
	return err;
}
