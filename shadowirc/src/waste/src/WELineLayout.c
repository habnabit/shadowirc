/*
 *	WELineLayout.c
 *
 *	WASTE PROJECT
 *  Line Layout, Getting and Setting Variables, etc.
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

INLINE pascal void _WERemoveLine(SInt32 lineIndex, WEPtr pWE)
{
	// remove the line
	_WESplice((Handle) pWE->hLines, nil, - sizeof(WELineRec), lineIndex * sizeof(WELineRec));

	// decrement line count
	pWE->nLines--;
}

pascal OSErr _WEInsertLine(SInt32 lineIndex, const WELineRec *pLine, WEPtr pWE)
{
	// insert the specified entry in the line array
	OSErr err;

	// do the insertion
	if ((err = _WESplice((Handle) pWE->hLines, pLine, sizeof(WELineRec), lineIndex * sizeof(WELineRec))) != noErr)
	{
		return err;
	}

	// increment line count
	pWE->nLines++;
	return noErr;
}

pascal void _WEBumpOrigin(SInt32 lineIndex, SInt32 deltaOrigin, WEPtr pWE)
{
	WELineRec *pLine = *pWE->hLines + lineIndex;
	SInt32 nLines = pWE->nLines;

	// loop through the line run array adjusting the lineOrigin fields
	for ( ; lineIndex <= nLines; lineIndex++ )
	{
		pLine->lineOrigin += deltaOrigin;
		pLine++;
	}
}

pascal SInt32 _WEFindLineBreak(SInt32 lineStart, WEHandle hWE)
{
	// Find where to break the line beginning at lineStart
	// the current graphics port must be already set up correctly

	WEPtr pWE = *hWE;	// assume WE record is already locked
	Ptr pText;
	SInt32 offset, breakOffset;
	SInt32 textLength;
	SInt32 remainingLength;
	SInt32 segmentStart, segmentEnd;
	SInt32 runIndex;
	WERunInfo runInfo;
	Fixed pixelWidth;
	ScriptCode script, previousScript;
	Boolean isBreak = false;

	offset = lineStart;
	pText = *pWE->hText + offset;
	remainingLength = pWE->textLength - offset;

	// find the style run index corresponding to the first segment on this line
	runIndex = WEOffsetToRun(offset, hWE);

	// initialize pixelWidth to the width of the destination rectangle, as a Fixed quantity
	pixelWidth = BSL((pWE->destRect.right - pWE->destRect.left), 16);

	// STYLE SEGMENT LOOP
	do
	{

		// get style run information for the current style run
		_WEGetIndStyle(runIndex, &runInfo, hWE);
		runIndex++;

		// set text attributes in the graphics port
		TextFont(runInfo.runAttrs.runStyle.tsFont);
		TextFace(runInfo.runAttrs.runStyle.tsFace);
		TextSize(runInfo.runAttrs.runStyle.tsSize);

		// if we're handling multiscript text, keep track of script boundaries
		if (BTST(pWE->flags, weFNonRoman))
		{
			// what is the script for this segment?
			script = FontToScript(runInfo.runAttrs.runStyle.tsFont);

			// have we crossed a script run boundary in the middle of a line?
			if ((runInfo.runStart > offset) && (script != previousScript))
			{
				// leave behind the all previous segments on this line
				offset = runInfo.runStart;
				pText = *pWE->hText + offset;
				remainingLength = pWE->textLength - offset;
			}
			previousScript = script;
		} // if non-Roman

		// we'll pass textLength as the second parameter to the line break hook
		// although this parameter is declared as a long, StyledLineBreak uses only
		// the low word, so make sure it doesn't trespass the 32,767 byte threshold!
		textLength = _WEPinInRange(remainingLength, 0, SHRT_MAX);

		// calculate segmentStart and segmentEnd relative to offset
		segmentStart = _WEPinInRange(runInfo.runStart - offset, 0, textLength);
		segmentEnd = _WEPinInRange(runInfo.runEnd - offset, 0, textLength);

		// set breakOffset to a non-zero value for the first script run on the line,
		// set it to zero for all subsequent script runs
		breakOffset = (offset == lineStart);

#if WASTE_OBJECTS
		if (runInfo.runAttrs.runStyle.tsObject != nil)
		{
			// EMBEDDED OBJECT
			// subtract object width from pixelWidth
			pixelWidth -= BSL((*runInfo.runAttrs.runStyle.tsObject)->objectSize.h, 16);

			// stop looping if pixelWidth has gone negative
			isBreak = (pixelWidth < 0);
			breakOffset = isBreak ? segmentStart : segmentEnd;
		}
		else
#endif
		{
			// REGULAR TEXT
			isBreak = (CallWELineBreakProc(pText, textLength, segmentStart, segmentEnd,
				&pixelWidth, &breakOffset, hWE, pWE->lineBreakHook) != smBreakOverflow);
		}

		// break the line anyway when we reach the end of the text
		if ((segmentEnd >= remainingLength) || (runIndex >= pWE->nRuns))
		{
			isBreak = true;
		}
	} while (!isBreak);

	// return the offset from lineStart to the break point
	return (offset - lineStart) + breakOffset;
}

pascal void _WECalcHeights(SInt32 rangeStart, SInt32 rangeEnd, SInt16 *lineAscent, SInt16 *lineDescent,
		WEHandle hWE)
{
	// Find the maximum ascent and descent values between rangeStart and rangeEnd
	// the WE record must be already locked
	// the current graphics port must be already set up correctly

	SInt32 runIndex;
	WERunInfo runInfo;
	SInt16 runAscent, runDescent;

	*lineAscent = 1;
	*lineDescent = 0;

	// find the style run index corresponding to the first segment on this line
	runIndex = WEOffsetToRun(rangeStart, hWE);

	// STYLE SEGMENT LOOP
	do
	{
		// get style run information for the current style run
		_WEGetIndStyle(runIndex, &runInfo, hWE);
		runIndex++;

		// calculate ascent and descent (actually, descent + leading) values for this style run

#if WASTE_OBJECTS
		if (runInfo.runAttrs.runStyle.tsObject != nil)
		{
			// EMBEDDED OBJECT
			runAscent = (*runInfo.runAttrs.runStyle.tsObject)->objectSize.v;
			runDescent = 0;
		}
		else
#endif
		{
			// REGULAR TEXT
			runAscent = runInfo.runAttrs.runAscent;
			runDescent = runInfo.runAttrs.runHeight - runAscent;
		}

		// save the maximum values in lineAscent and lineDescent
		if (runAscent > *lineAscent)
		{
			*lineAscent = runAscent;
		}

		if (runDescent > *lineDescent)
		{
			*lineDescent = runDescent;
		}

		// keep looping until we reach rangeEnd
	} while (runInfo.runEnd < rangeEnd);
}

pascal OSErr _WERecalBreaks(SInt32 *startLine, SInt32 *endLine, WEHandle hWE)
{
	// Recalculates line breaks, line heights and ascents for all the text or for a portion of it.
	// On entry, startLine and endLine define a range of lines to recalculate.
	// On exit, startLine to endLine defines the range of lines actually recalculated
	// the WE record must already be locked

	WEPtr pWE = *hWE;
	WELineRec *pLine;
	WELineRec lineInfo, oldLineInfo;
	SInt32 lineIndex;
	SInt32 recalThreshold;
	SInt32 lineOffset;
	SInt16 lineAscent, lineDescent;
	SInt32 textHeight;
	Boolean saveTextLock;
	QDEnvironment saveEnvironment;
	OSErr err = noErr;

	// lock the text
	saveTextLock = _WESetHandleLock(pWE->hText, true);

	// find the character offset that must be necessarily reached before we can
	// even consider the possibility of stopping the recalculation process
	// this offset, recalThreshold, is the last character on endLine _before_ recalculation
	lineIndex = _WEPinInRange(*endLine, 0, pWE->nLines - 1);
	recalThreshold = (*pWE->hLines)[lineIndex + 1].lineStart;

	// we start recalculating line breaks from the line actually _preceding_ startLine,
	// since editing startLine may cause part of its text to fit on the preceding line
	lineIndex = _WEPinInRange(*startLine - 1, 0, pWE->nLines - 1);

	// find where in the text recalculation should begin
	lineInfo = (*pWE->hLines)[lineIndex];

	// save the Quickdraw environment
	_WESaveQDEnvironment(pWE->port, false, &saveEnvironment);

	// MAIN LINE BREAKING LOOP
	do
	{
		// find where to break the current line
		lineOffset = _WEFindLineBreak(lineInfo.lineStart, hWE);

		// make sure we advance at least by one character (unless we reached the end of text)
		if ((lineOffset <= 0) && (lineInfo.lineStart < pWE->textLength))
		{
			lineOffset = 1;
		}

		// calculate ascent and descent values for this line
		_WECalcHeights(lineInfo.lineStart, lineInfo.lineStart + lineOffset, &lineAscent, &lineDescent, hWE);

		// save the maximum line ascent for this line in the line array
		pLine = *pWE->hLines + lineIndex;
		pLine->lineAscent = lineAscent;

		// increment counters (go to the next line array entry)
		lineIndex++;
		lineInfo.lineStart += lineOffset;
		lineInfo.lineOrigin += (lineAscent + lineDescent);
		pLine++;

		// compare the newly calculated line start with the old value
		// if the new line start comes before the old line start, insert a new element
		oldLineInfo = *pLine;
		if ((lineIndex > pWE->nLines) || (lineInfo.lineStart < oldLineInfo.lineStart))
		{
			if ((err = _WEInsertLine(lineIndex, &lineInfo, pWE)) != noErr)
			{
				goto cleanup;
			}
		}
		else
		{
			// overwrite the old element
			pLine->lineStart = lineInfo.lineStart;
			pLine->lineOrigin = lineInfo.lineOrigin;

			// remove all further elements which have a lineStart field
			// less than or equal to the current one
			while((lineIndex < pWE->nLines) && (lineInfo.lineStart >= (pLine + 1)->lineStart))
				_WERemoveLine(lineIndex + 1, pWE);

			// if the new line start is the same as the old one...
			if (lineInfo.lineStart == oldLineInfo.lineStart)
			{
				// ...and recalThreshold has been reached, we can stop recalculating line breaks
				if (lineInfo.lineStart >= recalThreshold)
				{
					// although line breaks need not be changed from lineIndex on,
					// the lineOrigin fields may need to be changed
					if (lineInfo.lineOrigin != oldLineInfo.lineOrigin)
					{
						_WEBumpOrigin(lineIndex + 1, lineInfo.lineOrigin - oldLineInfo.lineOrigin, pWE);
					}

					// exit from the line breaking loop
					goto cleanup;
				}
			}
			else
			{
				// otherwise, the new line start comes after the old line start...
				// if the current line is the one preceding startLine, warn our caller about this
				if ((lineIndex > 0) && (lineIndex == *startLine))
				{
					*startLine = lineIndex - 1;
				}
			}
		}
	} while(lineInfo.lineStart < pWE->textLength);

cleanup:
	// calculate total text height
	textHeight = WEGetHeight(0, pWE->nLines, hWE);

	// quirk: if the last character in the text is a carriage return, the caret appears
	// below the last line, so in this case we need to add the extra height to textHeight
	if (WEGetChar(pWE->textLength - 1, hWE) == kEOL)
	{
		textHeight += WEGetHeight(pWE->nLines - 1, pWE->nLines, hWE);
	}

	// if total text height has changed, remember to call the scroll callback later
	if (textHeight != pWE->destRect.bottom - pWE->destRect.top)
	{
		BSET(pWE->flags, weFDestRectChanged);
	}

	// set destRect.bottom to destRect.top + total text height
	pWE->destRect.bottom = pWE->destRect.top + textHeight;

	// return through endLine the index of the last line affected by recalculation
	*endLine = lineIndex - 1;

	// make sure startLine isn't greater than endLine
	if (*startLine > *endLine)
	{
		*startLine = *endLine;
	}

	// unlock the text
	_WESetHandleLock(pWE->hText, saveTextLock);

	// restore the Quickdraw environment
	_WERestoreQDEnvironment(&saveEnvironment);

	// return result code
	return err;
}

static Boolean SLCalcSlop
	(
		WELineRec *pLine,
		const WERunAttributes *pAttrs,
		Ptr pSegment,
		SInt32 segmentStart,
		SInt32 segmentLength,
		JustStyleCode styleRunPosition,
		WEHandle hWE,
		void *callbackData
	)
{
#if !WASTE_OBJECTS
	#pragma unused(pAttrs)
#endif
	struct SLCalcSlopData *cd = (struct SLCalcSlopData *) callbackData;
	SInt16 segmentWidth;
	Fixed segmentProportion;
	Boolean isEndOfLine;

	// see if this text segment ends with a carriage return, or if we've reached the
	// end of the text (in which case we don't want any justification to take place)
	isEndOfLine = (segmentStart + segmentLength >= (*hWE)->textLength) ||
				  (pSegment [ segmentLength - 1 ] == kEOL);

	// if this is the first segment on the line, reset line totals
	if (IS_LEFTMOST_RUN(styleRunPosition))
	{
		cd->totalSlop = cd->lineWidth;
		cd->totalProportion = 0;
	}

#if WASTE_OBJECTS
	if (pAttrs->runStyle.tsObject != nil)
	{
		// EMBEDDED OBJECT
		// segment width is just object width; no extra space can be applied for justification
		segmentWidth = (*pAttrs->runStyle.tsObject)->objectSize.h;
		segmentProportion = 0;
	}
	else
#endif
	{
		// REGULAR TEXT

		// if this is the last segment on the line, strip trailing spaces
		if (IS_RIGHTMOST_RUN(styleRunPosition))
		{
			segmentLength = VisibleLength(pSegment, segmentLength);
		}

		// measure this segment
		segmentWidth = TextWidth(pSegment, 0, segmentLength);

		// calculate the proportion of extra space to apply to this text segment
		segmentProportion = PortionLine(pSegment, segmentLength,
				styleRunPosition, kOneToOneScaling, kOneToOneScaling);
	}

	// keep track of line totals
	cd->totalSlop -= segmentWidth;
	cd->totalProportion += segmentProportion;

	// if this is the last segment on the line, save values in the line array
	if (IS_RIGHTMOST_RUN(styleRunPosition))
	{
		// make sure slop is non-negative
		if (cd->totalSlop < 0)
		{
			cd->totalSlop = 0;
		}
		pLine->lineSlop = cd->totalSlop;
		pLine->lineJustAmount = isEndOfLine ? 0 : FixDiv(BSL(cd->totalSlop, 16), cd->totalProportion);
	}
	return false;	// keep looping
}

pascal void _WERecalSlops(SInt32 firstLine, SInt32 lastLine, WEHandle hWE)
{
	// Calculates the lineSlop and lineJustAmount fields
	// of the line array for the specified lines

	WEPtr pWE = *hWE;
	struct SLCalcSlopData cd;

	// we only need to bother if the user isn't using left justification
	if (pWE->alignment == weFlushLeft)
	{
		return;
	}

	// calculate slop and normalized slop proportion for all lines
	cd.lineWidth = pWE->destRect.right - pWE->destRect.left;
	_WESegmentLoop(firstLine, lastLine, SLCalcSlop, &cd, hWE);
}

pascal void WESetAlignment(WEAlignment alignment, WEHandle hWE)
{
	WEPtr pWE;
	WEAlignment oldAlignment;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;
	oldAlignment = pWE->alignment;

	if ((alignment >= weFlushLeft) && (alignment <= weJustify) && (alignment != oldAlignment))
	{
		// hide the caret if it's showing
		if (BTST(pWE->flags, weFCaretVisible))
		{
			_WEBlinkCaret(hWE);
		}

		// change the alignment
		pWE->alignment = alignment;

		if (! BTST(pWE->features, weFInhibitRecal))
		{
			// if the text was left-aligned, then we haven't been bothering till now,
			// so we have to recalc the whole document
			if (oldAlignment == weFlushLeft)
			{
				_WERecalSlops(0, pWE->nLines - 1, hWE);
			}

			if (! BTST(pWE->features, weFInhibitRedraw))
			{
				// redraw the view rectangle
				WEUpdate(nil, hWE);
			}
		}
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

pascal void WESetDirection(WEDirection direction, WEHandle hWE)
{
	WEPtr pWE;
	WEDirection oldDirection;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;
	oldDirection = pWE->direction;

	if ((direction >= weDirRightToLeft) && (direction <= weDirDefault) && (direction != oldDirection))
	{
		// hide the caret if it's showing
		if (BTST(pWE->flags, weFCaretVisible))
		{
			_WEBlinkCaret(hWE);
		}

		// change the direction
		pWE->direction = direction;

		if (! (pWE->features & ((1L << weFInhibitRecal) | (1L << weFInhibitRedraw))))
		{
			// redraw the view rectangle
			WEUpdate(nil, hWE);
		}
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}
