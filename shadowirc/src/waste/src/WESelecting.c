/*
 *	WESelecting.c
 *
 *	WASTE PROJECT
 *  Drawing Selections, Activating, Updating, Scrolling, etc.
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

// values for _WEArrowOffset action parameter:

enum
{
	// plain arrow keys
	kGoLeft			=	0,
	kGoRight		=	1,
	kGoUp			=	2,
	kGoDown			=	3,

	// modifiers
	kOption			=	4,
	kCommand		=	8,

	// option + arrow combos
	kGoWordStart	=	kGoLeft + kOption,
	kGoWordEnd		=	kGoRight + kOption,
	kGoPageStart	=	kGoUp + kOption,
	kGoPageEnd		=	kGoDown + kOption,

	// command + arrow combos
	kGoLineStart	=	kGoLeft + kCommand,
	kGoLineEnd		=	kGoRight + kCommand,
	kGoTextStart	=	kGoUp + kCommand,
	kGoTextEnd		=	kGoDown + kCommand
};

INLINE pascal void _WEClearHiliteBit(void)
{
	LMSetHiliteMode(LMGetHiliteMode() & 0x7F);
}

static Boolean SLPixelToChar
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
	struct SLPixelToCharData *cd = (struct SLPixelToCharData *) callbackData;
	WEPtr pWE = *hWE;
	Fixed slop;
	SInt16 cType;
	Fixed oldWidth;
#if WASTE_OBJECTS
	Fixed objectWidth;
	Fixed subWidth;
#endif

	// if this is the first style run on the line, subtract pen indent from pixelWidth
	if (IS_LEFTMOST_RUN(styleRunPosition))
	{
		cd->pixelWidth -= BSL(_WECalcPenIndent(pLine, pWE->alignment, pWE->direction), 16);
	}

	// if pixelWidth is gone negative already, the point is on the
	// _leading_ edge (NOTE: was trailing in WASTE 1.2) of first glyph
	if (cd->pixelWidth <= 0)
	{
		cd->offset = segmentStart;
		cd->edge = kLeadingEdge;
		return true;	// stop looping
	}

	oldWidth = cd->pixelWidth;

#if WASTE_OBJECTS
	if (pAttrs->runStyle.tsObject != nil)
	{

		// EMBEDDED OBJECT
		// calculate object width as Fixed
		objectWidth = BSL((*pAttrs->runStyle.tsObject)->objectSize.h, 16);

		// subtract object width from pixelWidth
		cd->pixelWidth -= objectWidth;

#if WASTE_OBJECTS_ARE_GLYPHS

		// find out whether the point is in the leftmost half of the object,
		// in the rightmost half or past the object
		subWidth = objectWidth >> 1;	// divide by two
		if (cd->pixelWidth + subWidth < 0)
		{
			cd->offset = segmentStart;
			cd->edge = kLeadingEdge;		// point is in leftmost half of object
		}
		else
		{
			cd->offset= segmentStart + 1;
			if (cd->pixelWidth < 0)
			{
				cd->edge = kTrailingEdge;
			}	// point is in rightmost half of object
			else
			{
				cd->edge = kLeadingEdge;	// point is past object
			}
		}
#else

		// find out whether the point is in the leftmost quarter of the object,
		// in the middle half, in the rightmost quarter or past the object
		subWidth = objectWidth >> 2;	// divide by four
		if (cd->pixelWidth + subWidth < 0)
		{
			cd->offset = segmentStart;
			if (cd->pixelWidth + objectWidth < subWidth)
			{
				cd->edge = kLeadingEdge;	// point is in leftmost quarter of object
			}
			else
			{
				cd->edge = kObjectEdge;		// point is in middle half of object
			}
		}
		else
		{
			cd->offset = segmentStart + 1;
			if (cd->pixelWidth < 0)
			{
				cd->edge = kTrailingEdge;	// point is in rightmost quarter of object
			}
			else
			{
				cd->edge = kLeadingEdge;		// point is past object
			}
		}
#endif // WASTE_OBJECTS_ARE_GLYPHS
	}
	else
#endif // WASTE_OBJECTS
	{

		// REGULAR TEXT

		// if this is the last segment on the line, strip the last blank character (if any),
		// unless it is the last non-CR character in the whole text stream
		if (IS_RIGHTMOST_RUN(styleRunPosition))
		{
			if ((segmentStart + segmentLength < pWE->textLength) ||
				pSegment[segmentLength - 1] == kEOL)
			{
				cType = CallWECharTypeProc(pSegment, segmentLength - 1, FontScript(),
					 hWE, pWE->charTypeHook);
				if ((cType & (smcTypeMask + smcClassMask)) == smCharPunct + smPunctBlank)
				{
					segmentLength -= ((cType & smcDoubleMask) ? 2 : 1);
				}
			}
		}

		// calculate slop for this text segment (justified text only)
		if (pWE->alignment == weJustify)
		{
			slop = FixMul(PortionLine(pSegment, segmentLength, styleRunPosition,
					kOneToOneScaling, kOneToOneScaling), pLine->lineJustAmount);
		}
		else
		{
			slop = 0;
		}

		// call PixelToChar hook for this segment
		cd->offset = segmentStart + CallWEPixelToCharProc(pSegment, segmentLength,
				slop, &cd->pixelWidth, &cd->edge, styleRunPosition, cd->hPos,
				hWE, pWE->pixelToCharHook);
	}

	// increment hPos by change in pixelWidth
	cd->hPos += (oldWidth - cd->pixelWidth);

	// if pixelWidth has gone negative, we're finished; otherwise go to next run
	return (cd->pixelWidth <= 0);
}

pascal SInt32 WEGetOffset(const LongPt *thePoint, WEEdge *edge, WEHandle hWE)
{
	// given a long point in local coordinates,
	// find the text offset corresponding to the nearest glyph

	WEPtr pWE;
	SInt32 lineIndex;
	LongPt tempPoint;
	struct SLPixelToCharData cd;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// tempPoint is thePoint, relative to the top left corner of the dest rect
	tempPoint.v = thePoint->v - pWE->destRect.top;
	tempPoint.h = thePoint->h - pWE->destRect.left;

	// if the point is above the destination rect, return zero
	if (tempPoint.v < 0)
	{
		cd.offset = 0;
		cd.edge = kTrailingEdge;
	}
	else
	{
		// if the point is below the last line, return last char offset
		if (tempPoint.v >= WEGetHeight(0, LONG_MAX, hWE))
		{
			cd.offset = pWE->textLength;
			cd.edge = kLeadingEdge;
		}
		else
		{
			// find the line index corresponding to the vertical pixel offset
			lineIndex = _WEPixelToLine(tempPoint.v, hWE);

			// express the horizontal pixel offset as a Fixed value
			cd.pixelWidth = BSL(tempPoint.h, 16);

			// walk through the segments on this line calling PixelToChar
			cd.hPos = 0;
			cd.offset = 0;
			_WESegmentLoop(lineIndex, lineIndex, SLPixelToChar, &cd, hWE);
		}
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return offset/edge pair
	if (edge != nil)
	{
		*edge = cd.edge;
	}
	return cd.offset;
}

static Boolean SLCharToPixel
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
	struct SLCharToPixelData *cd = (struct SLCharToPixelData *) callbackData;
	WEPtr pWE = *hWE;
	SInt32 offset;
	Fixed slop;
	SInt16 direction = cd->direction;
	SInt16 width;
	Boolean isInSegment;

	// calculate offset relative to beginning of segment
	offset = cd->offset - segmentStart;

	// is offset within this segment?
	isInSegment = ((offset >= 0) && (offset < segmentLength));

	// if this is the first style run on the line, add pen indent to thePoint.h
	if (IS_LEFTMOST_RUN(styleRunPosition))
	{
		cd->thePoint->h += _WECalcPenIndent(pLine, pWE->alignment, pWE->direction);
	}

#if WASTE_OBJECTS
	if (pAttrs->runStyle.tsObject != nil)
	{

		// EMBEDDED OBJECT
		width = isInSegment ? 0 : (*pAttrs->runStyle.tsObject)->objectSize.h;
	}
	else
#endif
	{
		// REGULAR TEXT
		slop = 0;

		// calculate slop for this text segment (justified text only)
		if (pWE->alignment == weJustify)
		{
			slop = FixMul(PortionLine(pSegment, segmentLength, styleRunPosition,
				kOneToOneScaling, kOneToOneScaling), pLine->lineJustAmount);
		}

		if (BTST(pWE->flags, weFBidirectional))
		{
			//	SPECIAL TREATMENT FOR BIDIRECTIONAL SCRIPTS
			Boolean	segmentDir = (BTST(pAttrs->runStyle.tsFlags, tsRightToLeft) != 0);

			if ((offset < 0) || (offset > segmentLength))
			{
				// we want the total width of this segment, so set offset and direction appropriately
				if (segmentDir)
				{
					offset = 0;
					direction = rightCaret;
				}
				else
				{
					offset = segmentLength;
					direction = leftCaret;
				}
			}
			else if (cd->rightEdge)
			{
				// we're drawing a cursor at the right edge of the line, so ensure we measure this whole segment
				isInSegment = false;
				if (cd->lineDir && !segmentDir)
				{
					offset = segmentLength;
					direction = leftCaret;
				}
				else if (!cd->lineDir && segmentDir)
				{
					offset = 0;
					direction = rightCaret;
				}
			}
			else if ((offset == 0) || (offset == segmentLength))
			{
				isInSegment = false;
				// we're at a style run boundary
				if (((direction == leftCaret) && (! segmentDir)) || ((direction == rightCaret) && segmentDir))
				{
					// requested caret direction matches style run direction, so we can exit the loop
					isInSegment = true;
				}
				else
				{
					// need caret for direction opposite to this run, so need to check for direction run boundary;
					// determine whether preceding and following runs are R-L
					Boolean prevDir;
					Boolean nextDir;

					//	note that if segmentStart is 0, WEGetRunDirection
					//	will return the primary line direction
					prevDir = WEGetRunDirection(segmentStart - 1, hWE);

					//	again, if segmentStart + segmentLength == pWE->textLength,
					//	WEGetRunDirection will do the Right Thing
					nextDir = WEGetRunDirection(segmentStart + segmentLength, hWE);

					if ((offset == 0) && (prevDir != segmentDir))
					{
						if (cd->lineDir)
						{
							if (segmentDir)
							{
								//offset = 0;	// already known to be zero
								direction = rightCaret;
							}
							else
							{
								offset = segmentLength;
								direction = leftCaret;
							}
						}
					}
					else if ((offset == segmentLength) && (nextDir != segmentDir))
					{
						if (!cd->lineDir)
						{
							if (segmentDir)
							{
								offset = 0;
								direction = rightCaret;
							}
							else
							{
								//offset = segmentLength;
								direction = leftCaret;
							}
						}
					}
					else
					{
						// not a direction boundary, so force caret dir to match style run dir, and exit loop
						direction = segmentDir ? rightCaret : leftCaret;
						isInSegment = true;
					}
				}
			}
		} // end of special treatment for bidirectional scripts

		// call CharToPixel to get width of segment up to specified offset
		width = CallWECharToPixelProc(pSegment, segmentLength,
			slop, offset, direction, styleRunPosition, cd->thePoint->h,
			hWE, pWE->charToPixelHook);
	}

	// advance thePoint.h by the width of this segment
	cd->thePoint->h += width;

	// drop out of loop when we reach offset
	return isInSegment;
}

pascal void WEGetPoint(SInt32 offset, SInt16 direction, LongPt *thePoint, SInt16 *lineHeight, WEHandle hWE)
{
	// given a byte offset into the text, find the corresponding glyph position
	// this routine is useful for highlighting the text and for positioning the caret

	WEPtr pWE;
	WELineRec *pLine;
	SInt32 lineIndex;
	SInt32 height;
	struct SLCharToPixelData cd;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// the base point is the top left corner of the destination rectangle
	*thePoint = * (LongPt *) &pWE->destRect;

	// first of all find the line on which the glyph lies
	lineIndex = WEOffsetToLine(offset, hWE);

	// calculate the vertical coordinate and the line height
	pLine = *pWE->hLines + lineIndex;
	thePoint->v += pLine->lineOrigin;
	height = pLine[1].lineOrigin - pLine[0].lineOrigin;

	if ((offset == pWE->textLength) && (WEGetChar(offset - 1, hWE) == kEOL))
	{
		// SPECIAL CASE: if offset is past the last character and
		// the last character is a carriage return, return a point below the last line
		WELineRec dummyLine;

		dummyLine.lineSlop = pWE->destRect.right - pWE->destRect.left;
		dummyLine.lineJustAmount = 0;

		thePoint->v += height;
		thePoint->h += _WECalcPenIndent(&dummyLine, pWE->alignment, pWE->direction);
	}
	else
	{
// ¥¥¥Ênew version of this: better boundary behavior
		SInt32	lineStart, lineEnd;
		Boolean	lineDir;

		// determine the dominant line direction
		lineDir = IsRightToLeft(pWE->direction);

		// simplify direction so it's either leftCaret or rightCaret
		if (direction == hilite)
		{
			direction = (lineDir ? rightCaret : leftCaret);
		}

		// find ends of line so we can special-case them
		WEGetLineRange(lineIndex, &lineStart, &lineEnd, hWE);
		if (lineEnd < pWE->textLength || WEGetChar(lineEnd - 1, hWE) == kEOL)
		{
			--lineEnd;
		}

		if ((offset == lineStart && !lineDir && direction == leftCaret)
			|| (offset == lineEnd && lineDir && direction == rightCaret)
		   )
		{
			// SPECIAL CASE: if caret should be at left end of line, don't call style run loop
			thePoint->h += _WECalcPenIndent(pLine, pWE->alignment, pWE->direction);
		}
		else
		{
			cd.lineDir = lineDir;
			// Inform SLCharToPixel if we want the right edge of the line
			cd.rightEdge = ((offset == lineEnd && !lineDir && direction == leftCaret) ||
							(offset == lineStart && lineDir && direction == rightCaret));
			cd.offset = offset;
			cd.thePoint = thePoint;
			cd.direction = direction;

			// to get the horizontal coordinate, walk through the style runs on this line
			_WESegmentLoop(lineIndex, lineIndex, SLCharToPixel, &cd, hWE);
		}

// ¥¥¥Êreplaced with code above
/*
		cd.offset = offset;
		cd.thePoint = thePoint;
		cd.direction = direction;

		// to get the horizontal coordinate, walk through the style runs on this line
		_WESegmentLoop(lineIndex, lineIndex, SLCharToPixel, &cd, hWE);
*/
	}

	// pin the horizontal coordinate to the destination rectangle
	thePoint->h = _WEPinInRange(thePoint->h, pWE->destRect.left, pWE->destRect.right);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// copy line height
	if (lineHeight != nil)
	{
		*lineHeight = height;
	}
}

pascal void WEFindLine(SInt32 offset, WEEdge edge, SInt32 *lineStart, SInt32 *lineEnd, WEHandle hWE)
{
#pragma unused(edge)
	WEPtr pWE = *hWE;
	WELineRec *pLine;

	pLine = *pWE->hLines + WEOffsetToLine(offset, hWE);
	if (lineStart != nil)
	{
		*lineStart = pLine[0].lineStart;
	}
	if (lineEnd != nil)
	{
		*lineEnd = pLine[1].lineStart;
	}
}

pascal void WEFindParagraph(SInt32 offset, WEEdge edge, SInt32 *paragraphStart, SInt32 *paragraphEnd, WEHandle hWE)
{
#pragma unused(edge)
	WEPtr pWE = * hWE ;
	Ptr pText = * pWE -> hText ;
	SInt32 textLength = pWE -> textLength ;
	WELineRec * pStartLine = * pWE -> hLines + WEOffsetToLine ( offset, hWE ) ;
	WELineRec * pLine ;

	//	scan backwards the line array, looking for a line ending with a CR
	for (
			pLine = pStartLine ;
			( pLine -> lineStart > 0 ) && ( pText [ pLine -> lineStart - 1 ] != kEOL ) ;
			pLine --
		) ;
	if ( paragraphStart != nil )
	{
		* paragraphStart = pLine -> lineStart ;
	}

	//	now scan forward
	for (
			pLine = pStartLine + 1 ;
			( pLine -> lineStart < textLength ) && ( pText [ pLine -> lineStart - 1 ] != kEOL ) ;
			pLine ++
		) ;
	if ( paragraphEnd != nil )
	{
		* paragraphEnd = pLine -> lineStart ;
	}
}

pascal ScriptCode _WEGetContext(SInt32 offset, SInt32 *contextStart, SInt32 *contextEnd,
						WEHandle hWE)
{
	// This function finds a range of characters ("context"), all belonging to the same script
	// and centered around the specified offset.
	// The function result is the ID of a font belonging to this script.
	// Ideally, the context should consist of a whole script run, but in practice the returned
	// context can be narrower, for performance and other reasons (see below)

	SInt32 index, saveIndex, saveRunEnd;
	WERunInfo runInfo;
	ScriptCode script1, script2;
	SInt16 retval;

	if (BTST((*hWE)->flags, weFNonRoman))
	{
		// if more than one script is installed, limit the search of
		// script run boundaries to a single paragraph, for speed's sake
		WEFindParagraph(offset, kLeadingEdge, contextStart, contextEnd, hWE);

		// find the style run the specified offset is in
		index = WEOffsetToRun(offset, hWE);
		_WEGetIndStyle(index, &runInfo, hWE);

		// find the script code associated with this style run
		script1 = FontToScript(runInfo.runAttrs.runStyle.tsFont);

		// the script code is returned as function result
		retval = script1;

		// save index and runInfo.runEnd for the second while loop
		saveIndex = index;
		saveRunEnd = runInfo.runEnd;

		// walk backwards across style runs preceding offset, looking for a script run boundary
		while (runInfo.runStart > *contextStart)
		{
			index--;
			_WEGetIndStyle(index, &runInfo, hWE);
			script2 = FontToScript(runInfo.runAttrs.runStyle.tsFont);
			if (script1 != script2)
			{
				*contextStart = runInfo.runEnd;
				break;
			}
		}

		// restore index and runInfo.runEnd
		index = saveIndex;
		runInfo.runEnd = saveRunEnd;

		// walk forward across style runs following offset, looking for a script run boundary
		while (runInfo.runEnd < *contextEnd)
		{
			index++;
			_WEGetIndStyle(index, &runInfo, hWE);
			script2 = FontToScript(runInfo.runAttrs.runStyle.tsFont);
			if (script1 != script2)
			{
				*contextEnd = runInfo.runStart;
				break;
			}
		}
	}
	else
	{
		// only the Roman script is enabled: the whole text constitutes one script run
		retval = smRoman;
		*contextStart = 0;
		*contextEnd = (*hWE)->textLength;
	}

	// make sure the range identified by contextStart/contextEnd is contained within
	// the 32K byte range centered around the specified offset
	// the reason for this is that many Script Manager routines (e.g. FindWord and CharByte)
	// only accept 16-bit offsets, rather than 32-bit offsets

	*contextStart = _WEPinInRange(*contextStart, offset - (SHRT_MAX / 2), offset);
	*contextEnd = _WEPinInRange(*contextEnd, offset, offset + (SHRT_MAX / 2));

	return retval;
}

pascal ScriptCode _WEGetRestrictedContext(SInt32 offset, SInt32 *contextStart, SInt32 *contextEnd,
						WEHandle hWE)
{
	// This function finds a range of characters ("context"), all belonging to the same script
	// and centered around the specified offset.
	// This function returns a script run subrange and is more efficient than
	// _WEGetContext because it doesn't try to find the script boundaries accurately.

	WERunInfo runInfo;

	// just find the style run the specified offset is in
	WEGetRunInfo(offset, &runInfo, hWE);
	*contextStart = runInfo.runStart;
	*contextEnd = runInfo.runEnd;

	// make sure the range identified by contextStart/contextEnd is contained within
	// the 32K byte range centered around the specified offset
	// the reason for this is that many Script Manager routines (e.g. FindWord and CharByte)
	// only accept 16-bit offsets, rather than 32-bit offsets

	*contextStart = _WEPinInRange(*contextStart, offset - (SHRT_MAX / 2), offset);
	*contextEnd = _WEPinInRange(*contextEnd, offset, offset + (SHRT_MAX / 2));

	return FontToScript(runInfo.runAttrs.runStyle.tsFont);
}

pascal void WEFindWord(SInt32 offset, WEEdge edge, SInt32 *wordStart, SInt32 *wordEnd, WEHandle hWE)
{
	WEPtr pWE;
	ScriptCode script;
	SInt32 contextStart, contextEnd;
	Handle hText;
	OffsetTable wordBreaks;
	Boolean saveTextLock, saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// find a script context containing the specified offset
	// (words cannot straddle script boundaries)
	script = _WEGetContext(offset, &contextStart, &contextEnd, hWE);

	// lock the text
	hText = pWE->hText;
	saveTextLock = _WESetHandleLock(hText, true);

	// call the word break hook
	CallWEWordBreakProc(*hText + contextStart, contextEnd - contextStart,
		offset - contextStart, edge, wordBreaks, script, hWE, pWE->wordBreakHook);

	// unlock the text
	_WESetHandleLock(hText, saveTextLock);

	// calculate wordStart and wordEnd relative to the beginning of the text
	if (wordStart != nil)
	{
		*wordStart = contextStart + wordBreaks[0].offFirst;
	}
	if (wordEnd != nil)
	{
		*wordEnd = contextStart + wordBreaks[0].offSecond;
	}
}

pascal SInt16 WECharByte(SInt32 offset, WEHandle hWE)
{
	WEPtr pWE;
	Handle hText;
	SInt32 contextStart, contextEnd;
	ScriptCode script;
	Boolean saveWELock, saveTextLock;
	SInt16 retval = smSingleByte;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// do nothing unless there is at least one double-byte script system installed
	// and make sure offset is within allowed bounds
	if (BTST(pWE->flags, weFDoubleByte))
	{
		if ((offset >= 0) && (offset < pWE->textLength))
		{

			// find a script context containing the specified offset
			script = _WEGetRestrictedContext(offset, &contextStart, &contextEnd, hWE);

			// lock the text
			hText = pWE->hText;
			saveTextLock = _WESetHandleLock(hText, true);

			// pass the CharByte hook a pointer to the beginning of the style run
			retval = CallWECharByteProc(*hText + contextStart,
				offset - contextStart, script, hWE, pWE->charByteHook);

			// unlock the text
			_WESetHandleLock(hText, saveTextLock);
		}
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	return retval;
}

pascal SInt16 WECharType(SInt32 offset, WEHandle hWE)
{
	WEPtr pWE;
	Handle hText;
	SInt32 contextStart, contextEnd;
	ScriptCode script;
	Boolean saveWELock, saveTextLock;
	SInt16 retval = 0;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// make sure offset is within allowed bounds
	if ((offset >= 0) && (offset < pWE->textLength))
	{

		// find a script context containing the specified offset
		script = _WEGetRestrictedContext(offset, &contextStart, &contextEnd, hWE);

		// lock the text
		hText = pWE->hText;
		saveTextLock = _WESetHandleLock(hText, true);

		// pass the CharType hook a pointer to the beginning of the style run
		retval = CallWECharTypeProc(*hText + contextStart,
			offset - contextStart, script, hWE, pWE->charTypeHook);

		// unlock the text
		_WESetHandleLock(hText, saveTextLock);
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	return retval;
}

pascal void _WEGetCaretRect(SInt32 offset, SInt16 direction, Rect *caretRect, WEHandle hWE)
{
	LongPt thePoint;
	SInt16 caretHeight;

	WEGetPoint(offset, direction, &thePoint, &caretHeight, hWE);
	WELongPointToPoint(&thePoint, (Point *) &caretRect->top);
	if (caretRect->left > (*hWE)->destRect.left)
	{
		caretRect->left--;
	}

	caretRect->bottom = caretRect->top + caretHeight;
	caretRect->right = caretRect->left + kCaretWidth;
}

pascal void _WEDrawCaret(SInt32 offset, SInt16 direction, Boolean useDualCaret, WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	Rect caretRect;
	GrafPtr savePort;
	RgnHandle saveClip, auxRgn;

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// save the clip region
	saveClip = NewRgn();
	GetClip(saveClip);

	// restrict the clip region to the view region
	auxRgn = NewRgn();
	SectRgn(saveClip, pWE->viewRgn, auxRgn);
	SetClip(auxRgn);
	DisposeRgn(auxRgn);

	// calculate caret rectangle for the primary caret
	_WEGetCaretRect(offset, direction, &caretRect, hWE);

	// should we use a dual caret?
	if (useDualCaret)
	{
		// draw the primary caret
		caretRect.bottom = (caretRect.top + caretRect.bottom) >> 1;
		InvertRect(&caretRect);

		// calculate the caret rectangle for the secondary caret
		_WEGetCaretRect(offset, (leftCaret + rightCaret) - direction, &caretRect, hWE);
		caretRect.top = (caretRect.top + caretRect.bottom) >> 1;
	}

	// draw the caret (either a single caret, or the secondary caret)
	InvertRect(&caretRect);

	// restore the clip region
	SetClip(saveClip);
	DisposeRgn(saveClip);

	// restore the port
	SetPort(savePort);
}

pascal void _WEBlinkCaret(WEHandle hWE)
{
	WEPtr pWE = *hWE;				// assume WE record is already locked
	SInt16 direction = hilite;
	Boolean useDualCaret = false;

	// do nothing if we're not active
	if (!BTST(pWE->flags, weFActive))
	{
		return;
	}

#if WASTE_NO_RO_CARET
	if (BTST(pWE->features, weFReadOnly) && !BTST(pWE->flags, weFCaretVisible))
	{
		return;
	}
#endif

	if (BTST(pWE->flags, weFBidirectional))
	{
		// SPECIAL PROCESSING FOR BIDIRECTIONAL SCRIPTS
		// check if we should use a dual caret
		if (BTST(GetScriptManagerVariable(smGenFlags), smfDualCaret))
		{
			//	DUAL CARET
			useDualCaret = true;

			//	primary caret position depends on primary line direction
			direction = IsRightToLeft(pWE->direction) ? rightCaret : leftCaret;
		}
		else
		{
			//	SINGLE (JUMPING) CARET
			//	caret position depends on keyscript direction
			//	when erasing, though, use previously used direction
			//	in case keyscript changed in the meantime
			if (BTST(pWE->flags, weFCaretVisible))
			{
				direction = BTST(pWE->flags, weFCaretRight) ? rightCaret : leftCaret;
			}
			else
			{
				if (GetScriptVariable(GetScriptManagerVariable(smKeyScript), smScriptRight))
				{
					direction = rightCaret;
					BSET(pWE->flags, weFCaretRight);
				}
				else
				{
					direction = leftCaret;
					BCLR(pWE->flags, weFCaretRight);
				}
			}
		}
	}

	// redraw the caret, in XOR mode
	_WEDrawCaret(pWE->selStart, direction, useDualCaret, hWE);

	// keep track of the current caret visibility status
	BCHG(pWE->flags, weFCaretVisible);	// invert flag

	// update caretTime
	pWE->caretTime = TickCount();
}

static Boolean SLCollectHiliteRgn
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
// Get the hilite rgn for the current run, or the portion thereof within the hilite range;
// this just does FrameRect for the relevant area(s) -- caller is assumed to have a region open
{
	WEPtr pWE = *hWE;
	struct SLCollectHiliteRgnData *cd = (struct SLCollectHiliteRgnData *) callbackData;
	SInt16 lineHeight;
	SInt32 segmentEnd;
	SInt32 firstOffset, secondOffset;
	OffsetTable	offsets;
	SInt16 i;
	Rect r;
	Fixed slop;
	SInt16 width;
	SInt32 start, end, lineOrigin;
	WEDirection direction;
	SInt32 offset;

	// determine the line direction
	direction = pWE->direction;
	if (direction == weDirDefault)
	{
		direction = GetSysDirection();
	}

	// find the line origin & height
	lineOrigin = pWE->destRect.top + pLine->lineOrigin;
	lineHeight = pLine[1].lineOrigin - pLine[0].lineOrigin;

	// initialize cd->hPos on first segment of the line
	if (IS_LEFTMOST_RUN(styleRunPosition))
	{
		cd->hPos = pWE->destRect.left + _WECalcPenIndent(pLine, pWE->alignment, direction);

		// if selection starts before beginning of line AND direction is LR,
		// OR selection ends after end of line AND direction is RL,
		// then we need to include the area to the left of the line in the hilite

		if ( ((cd->rangeStart < pLine[0].lineStart) && (direction == weDirLeftToRight))
		  || ((cd->rangeEnd >= pLine[1].lineStart) && (direction == weDirRightToLeft)) )
		{
			// frame the rect to the left of the actual text
			SetRect(&r, pWE->destRect.left, lineOrigin, cd->hPos, lineOrigin + lineHeight);
			FrameRect(&r);
		}
	}

	// calculate segment end
	segmentEnd = segmentStart + segmentLength;

#if WASTE_OBJECTS
	if (pAttrs->runStyle.tsObject != nil)
	{
		// EMBEDDED OBJECT

		// get object width
		width = (*pAttrs->runStyle.tsObject)->objectSize.h;

		// if this object is hilited, add its rect to the region
		if (cd->rangeStart <= segmentStart && cd->rangeEnd >= segmentEnd)
		{
			SetRect(&r, cd->hPos, lineOrigin, cd->hPos + width, lineOrigin + lineHeight);
			FrameRect(&r);
		}
	}
	else
#endif
	{
		// REGULAR TEXT

		// calculate slop value if justifying
		if (pWE->alignment == weJustify)
		{
			slop = FixMul(PortionLine(pSegment, segmentLength, styleRunPosition, kOneToOneScaling, kOneToOneScaling), pLine->lineJustAmount);
		}
		else
		{
			slop = 0;
		}

		// see whether this run includes any hilited text
		if (cd->rangeEnd > segmentStart && cd->rangeStart < segmentEnd)
		{
			if (segmentStart < cd->rangeStart)
			{
				firstOffset = cd->rangeStart - segmentStart;
			}
			else
			{
				firstOffset = 0;
			}

			if (segmentEnd >= cd->rangeEnd)
			{
				secondOffset = cd->rangeEnd - segmentStart;
			}
			else
			{
				secondOffset = segmentLength;
			}

			HiliteText(pSegment, segmentLength, firstOffset, secondOffset, offsets);

			for ( i = 0; i <= 2; ++i )
			{	// for each pair of offsets...
				if (offsets[i].offFirst != offsets[i].offSecond)
				{	// if offset pair is not empty...
					// calculate horizontal coordinates of start and end of hilite area
					start = CallWECharToPixelProc(pSegment, segmentLength,
						slop, offsets[i].offFirst, hilite, styleRunPosition, cd->hPos,
						hWE, pWE->charToPixelHook);
					end = CallWECharToPixelProc(pSegment, segmentLength,
						slop, offsets[i].offSecond, hilite, styleRunPosition, cd->hPos,
						hWE, pWE->charToPixelHook);
					// frame the rectangle to be hilited
					SetRect(&r, cd->hPos + start, lineOrigin, cd->hPos + end, lineOrigin + lineHeight);
					FrameRect(&r);
				}	// if offset pair is not empty
			}	// for each pair of offsets
		}

		// calculate the width of this segment
		if (BTST(pAttrs->runStyle.tsFlags, tsRightToLeft) == 0)
		{
			i = leftCaret;
			offset = segmentLength;
		}
		else
		{
			i = rightCaret;
			offset = 0;
		}
		width = CallWECharToPixelProc(pSegment, segmentLength, slop,
			offset, i, styleRunPosition, cd->hPos, hWE, pWE->charToPixelHook);
	}

	// update cd->hPos to account for the width of the segment
	cd->hPos += width;

	if (IS_RIGHTMOST_RUN(styleRunPosition))
	{
		// if selection ends after end of line AND direction is LR,
		// OR selection begins before beginning of line AND direction is RL,
		// then we need to include the area to the right of the line in the hilite

		if ( (cd->rangeEnd >= pLine[1].lineStart && direction == weDirLeftToRight)
		  || (cd->rangeStart < pLine->lineStart && direction == weDirRightToLeft) )
		{
			// frame the rect to the right of the text
			SetRect(&r, cd->hPos, lineOrigin, pWE->destRect.right, lineOrigin + lineHeight);
			FrameRect(&r);
		}
	}

	return false;	// don't break out of the segment loop
}

pascal RgnHandle WEGetHiliteRgn(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	// returns the hilite region corresponding to the specified range
	// the caller is responsible for disposing of the returned region
	// when it's finished with it

	WEPtr pWE;
	RgnHandle hiliteRgn;
	LongRect selRect;
	LongPt firstPoint, lastPoint;
	SInt32 lineIndex;
	SInt16 firstLineHeight, lastLineHeight;
	Rect r;
	GrafPtr savePort;
	Boolean saveWELock;
	Boolean bidirectional;
	struct SLCollectHiliteRgnData callbackData;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// make sure rangeStart comes before rangeEnd
	_WEReorder(&rangeStart, &rangeEnd);

	// calculate pixel location corresponding to rangeStart
	WEGetPoint(rangeStart, hilite, &firstPoint, &firstLineHeight, hWE);

	// calculate pixel location corresponding to rangeEnd
	WEGetPoint(rangeEnd, hilite, &lastPoint, &lastLineHeight, hWE);

	// open a region: rects to be hilited will be accumulated in this
	OpenRgn();

	// any bidirectional scripts installed?
	bidirectional = (BTST(pWE->flags, weFBidirectional) != 0);
	if (bidirectional)
	{
		callbackData.rangeStart = rangeStart;
		callbackData.rangeEnd = rangeEnd;
	}

	if (firstPoint.v == lastPoint.v)
	{
		// selection range encompasses only one line
		// for bidirectional text, we need to loop through the style runs
		if (bidirectional)
		{
			lineIndex = WEOffsetToLine(rangeStart, hWE);
			_WESegmentLoop(lineIndex, lineIndex, SLCollectHiliteRgn, &callbackData, hWE);
		}
		else
		{
			WESetLongRect(&selRect, firstPoint.h, firstPoint.v, lastPoint.h, lastPoint.v + lastLineHeight);
			WELongRectToRect(&selRect, &r);
			FrameRect(&r);
		}
	}
	else
	{
		// selection range encompasses more than one line
		// hilite the first line
		if (bidirectional)
		{
			lineIndex = WEOffsetToLine(rangeStart, hWE);
			_WESegmentLoop(lineIndex, lineIndex, SLCollectHiliteRgn, &callbackData, hWE);
		}
		else
		{
			WESetLongRect(&selRect, firstPoint.h, firstPoint.v, pWE->destRect.right, firstPoint.v + firstLineHeight);
			WELongRectToRect(&selRect, &r);
			FrameRect(&r);
		}

		// any lines between the first and the last one?
		if (firstPoint.v + firstLineHeight < lastPoint.v)
		{
			// hilite all the lines in-between
			WESetLongRect(&selRect, pWE->destRect.left, firstPoint.v + firstLineHeight, pWE->destRect.right, lastPoint.v);
			WELongRectToRect(&selRect, &r);
			FrameRect(&r);
		}

		// hilite the last line
		if (bidirectional)
		{
			// ??? shouldn't this be WEOffsetToLine(rangeEnd - 1, hWE) ??? -- marco
			// ¥¥¥Êyes, I think you're right, but I haven't tested it thoroughly -- jonathan
			lineIndex = WEOffsetToLine(rangeEnd, hWE);

			// skip this if it's an empty last line of the document
			if ((rangeEnd < pWE->textLength) || (WEGetChar(rangeEnd - 1, hWE) != kEOL))
			{
				_WESegmentLoop(lineIndex, lineIndex, SLCollectHiliteRgn, &callbackData, hWE);
			}
		}
		else
		{
			WESetLongRect(&selRect, pWE->destRect.left, lastPoint.v, lastPoint.h, lastPoint.v + lastLineHeight);
			WELongRectToRect(&selRect, &r);
			FrameRect(&r);
		}
	}

	// copy the accumulated region into a new region
	hiliteRgn = NewRgn();
	CloseRgn(hiliteRgn);

	// restrict this region to the view region
	SectRgn(hiliteRgn, pWE->viewRgn, hiliteRgn);

	// restore the port
	SetPort(savePort);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return the hilite region
	return hiliteRgn;
}

pascal void _WEHiliteRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE)
{
	WEPtr pWE;
	RgnHandle saveClip, auxRgn, hiliteRgn;
	PenState savePen;
	GrafPtr savePort;

	// the WE record must be already locked
	pWE = *hWE;

	// do nothing if the specified range is empty
	if (rangeStart == rangeEnd)
	{
		return;
	}

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// create auxiliary regions
	saveClip = NewRgn();
	auxRgn = NewRgn();

	// restrict the clip region to the view rectangle
	GetClip(saveClip);
	SectRgn(saveClip, pWE->viewRgn, auxRgn);
	SetClip(auxRgn);

	// get the hilite region corresponding to the specified range
	hiliteRgn = WEGetHiliteRgn(rangeStart, rangeEnd, hWE);

	// hilite the region or frame it, depending on the setting of the active flag
	if (BTST(pWE->flags, weFActive))
	{
		_WEClearHiliteBit();
		InvertRgn(hiliteRgn);
	}
	else if (BTST(pWE->features, weFOutlineHilite))
	{
		GetPenState(&savePen);
		PenNormal();
		PenMode(patXor);
		_WEClearHiliteBit();
		FrameRgn(hiliteRgn);
		SetPenState(&savePen);
	}

	// restore the clip region
	SetClip(saveClip);

	// dispose of all regions
	DisposeRgn(saveClip);
	DisposeRgn(auxRgn);
	DisposeRgn(hiliteRgn);

	// restore the port
	SetPort(savePort);
}

pascal void WESetSelection(SInt32 selStart, SInt32 selEnd, WEHandle hWE)
{
	WEPtr pWE;
	SInt32 oldSelStart, oldSelEnd;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// range-check parameters
	selStart = _WEPinInRange(selStart, 0, pWE->textLength);
	selEnd = _WEPinInRange(selEnd, 0, pWE->textLength);

	// set the weFAnchorIsEnd bit if selStart > selEnd,  reorder the endpoints
	if (selStart > selEnd)
	{
		BSET(pWE->flags, weFAnchorIsEnd);
		_WEReorder(&selStart, &selEnd);
	}
	else
	{
		BCLR(pWE->flags, weFAnchorIsEnd);
	}

	// get old selection range
	oldSelStart = pWE->selStart;
	oldSelEnd = pWE->selEnd;

	// selection changed?
	if ((oldSelStart != selStart) || (oldSelEnd != selEnd))
	{
		// invalid the null style
		BCLR(pWE->flags, weFUseNullStyle);

		// hide the caret if it's showing
		if (BTST(pWE->flags, weFCaretVisible))
		{
			_WEBlinkCaret(hWE);
		}

		// set new selection range
		pWE->selStart = selStart;
		pWE->selEnd = selEnd;

		// skip this section if either recalc or redraw has been inhibited
		if (! (pWE->features & ((1L << weFInhibitRecal) | (1L << weFInhibitRedraw))))
		{
			// if we're active, invert the exclusive-OR between the old range and the new range.
			// if we're inactive, this optimization can't be used because of outline highlighting.
			if (BTST(pWE->flags, weFActive))
			{
				_WEReorder(&oldSelStart, &selStart);
				_WEReorder(&oldSelEnd, &selEnd);
				_WEReorder(&oldSelEnd, &selStart);
			}

			_WEHiliteRange(oldSelStart, oldSelEnd, hWE);
			_WEHiliteRange(selStart, selEnd, hWE);

			if (!BTST(pWE->flags, weFMouseTracking))
			{
				// redraw the caret immediately, if the selection range is empty
				if (pWE->selStart == pWE->selEnd)
				{
					_WEBlinkCaret(hWE);
				}

				// clear clickCount, unless we're tracking the mouse
				pWE->clickCount = 0;

				// scroll the selection into view, unless we're tracking the mouse
				WESelView(hWE);

			}
		} // if redrawing not inhibited
	} // if selection changed

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

static Boolean SLCrossDirectionBoundary
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
#pragma unused(pLine, pAttrs, pSegment, hWE)
	struct SLCrossDirectionBoundaryData *cd = (struct SLCrossDirectionBoundaryData *) callbackData;
	Boolean isOldSegment = false;

	// have we reached the segment that contains the old offset?
	isOldSegment = (cd->oldOffset == segmentStart + segmentLength);

	if (isOldSegment)
	{
		if (cd->movingRight)
		{
			cd->isDone = true;	//	exit on next iteration
			return false;
		}
		else
		{
			if (IS_LEFTMOST_RUN(styleRunPosition))
			{
				cd->newOffset = segmentStart;
			}
			return true;		//	exit now
		}
	}

	cd->newOffset = segmentStart + segmentLength;

	if ((! isOldSegment) && (cd->isDone))
	{
		//	we're in the segment to the right of old segment
		return true;
	}

	return false;	//	keep looping
}

pascal SInt32 _WECrossDirectionBoundary(SInt32 offset, Boolean movingRight, WEHandle hWE)
{
	struct SLCrossDirectionBoundaryData cd;
	SInt32 lineIndex = WEOffsetToLine(offset, hWE);

	cd.oldOffset = cd.newOffset = offset;
	cd.movingRight = movingRight;
	cd.isDone = false;
	_WESegmentLoop(lineIndex, lineIndex, SLCrossDirectionBoundary, &cd, hWE);
	return cd.newOffset;
}

pascal SInt32 _WEArrowOffset(SInt16 action, SInt32 offset, WEHandle hWE)
{
	// given an action code (corresponding to a modifiers + arrow key combo)
	// and an offset into the text, find the offset of the new caret position

	WEPtr pWE = *hWE;	//	assume the WE record is already locked
	LongPt thePoint;
	SInt32 rangeStart, rangeEnd;
	SInt32 oldOffset;
	SInt32 scrollAmount;
	SInt16 lineHeight;
	SInt16 arrow;
	Boolean prevDir, nextDir;
	SInt16 cType;

	//	extract arrow portion from action parameter
	arrow = action & 0x0003;

	if (BTST(pWE->flags, weFBidirectional) && (arrow <= kGoRight))
	{
		//	bidirectional pre-processing for left/right arrow keys
		prevDir = WEGetRunDirection(offset - 1, hWE);
		nextDir = WEGetRunDirection(offset, hWE);

		//	determine whether offset is at a direction boundary
		if (prevDir != nextDir)
		{
			//	offset at direction boundary:
			//	determine whether we should cross the boundary
			if (prevDir != (arrow != kGoLeft))
			{
				offset = _WECrossDirectionBoundary(offset, (arrow != kGoLeft), hWE);
			}
		} // at direction boundary

		if ((arrow == kGoLeft) ? prevDir : nextDir)
		{
			//	insertion point is within a RL run: invert left/right keys
			arrow = (kGoLeft + kGoRight) - arrow;
			action = (action & ~0x0003) | arrow;
		}
	}	//	bidirectional pre-processing

	switch (action)
	{
		case kGoLeft:
		{
			if (offset > 0)
			{
				offset--;
				if (WECharByte(offset, hWE) != smSingleByte)
				{
					offset--;
				}
			}
			break;
		}

		case kGoRight:
		{
			if (offset < pWE->textLength)
			{
				if (WECharByte(offset, hWE) != smSingleByte)
				{
					offset++;
				}
				offset++;
			}
			break;
		}

		case kGoUp:
		{
			WEGetPoint(offset, hilite, &thePoint, nil, hWE);
			thePoint.v--;
			offset = WEGetOffset(&thePoint, nil, hWE);
			break;
		}

		case kGoDown:
		{
			WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);
			thePoint.v += lineHeight;
			offset = WEGetOffset(&thePoint, nil, hWE);
			break;
		}

		case kGoWordStart:
		{
			while (true)
			{
				WEFindWord(offset, kTrailingEdge, &rangeStart, &rangeEnd, hWE);
				offset = rangeStart;

				// If the found range is empty, get outta here.  (Most
				// likely this means that we have reached the beginning
				// of the text.)
				if (rangeStart == rangeEnd)
				{
					break;
				}

				cType = WECharType(rangeStart, hWE);

				// If the char is punctuation (other than a number),
				// it's not really a word, so keep looping.  Otherwise
				// we're done.
				if (((cType & smcTypeMask) != smCharPunct) ||
					((cType & smcClassMask) == smPunctNumber))
				{
					break;
				}
			}
			break;
		}

		case kGoWordEnd:
		{
			while (true)
			{
				WEFindWord(offset, kLeadingEdge, &rangeStart, &rangeEnd, hWE);
				offset = rangeEnd;

				// If the found range is empty, get outta here.
				// (Most likely this means that we have reached
				// the end of the text.)
				if (rangeStart == rangeEnd)
				{
					break;
				}

				// `rangeEnd - 1' may point in the middle of a two-byte
				// character; that's ok, CharType can deal with that.
				cType = WECharType(rangeEnd - 1, hWE);

				// If the char is punctuation (other than a number),
				// it's not really a word, so keep looping.  Otherwise
				// we're done.
				if (((cType & smcTypeMask) != smCharPunct) ||
					((cType & smcClassMask) == smPunctNumber))
				{
					break;
				}
			}
			break;
		}

		case kGoLineStart:
		{
			WEFindLine(offset, kLeadingEdge, &rangeStart, &rangeEnd, hWE);
			offset = rangeStart;
			break;
		}

		case kGoLineEnd:
		{
			WEFindLine(offset, kTrailingEdge, &rangeStart, &rangeEnd, hWE);
			offset = rangeEnd;
			if (offset < pWE->textLength)
			{
				offset--;
				if (WECharByte(offset, hWE) != smSingleByte)
				{
					offset--;
				}
			}
			break;
		}

		case kGoPageStart:
		{
			oldOffset = offset;

			//	get the offset corresponding to the top of the page
			thePoint.v = pWE->viewRect.top;
			thePoint.h = pWE->destRect.left;
			offset = WEGetOffset(&thePoint, nil, hWE);

			//	if the top line isn't completely visible, pick the line below
			WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);
			if (thePoint.v < pWE->viewRect.top)
			{
				thePoint.v += lineHeight;
				offset = WEGetOffset(&thePoint, nil, hWE);
			}

			//	if we're already at the top of this page, scroll up a page
			if (offset == oldOffset)
			{
				scrollAmount = pWE->viewRect.bottom - pWE->viewRect.top - lineHeight;

				//	get the offset corresponding to the top of the previous page
				thePoint.v = pWE->viewRect.top - scrollAmount;
				thePoint.h = pWE->destRect.left;
				offset = WEGetOffset(&thePoint, nil, hWE);

				//	if the top line isn't completely visible, pick the line below
				WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);
				if (thePoint.v < pWE->viewRect.top - scrollAmount)
				{
					thePoint.v += lineHeight;
					scrollAmount = pWE->viewRect.top - thePoint.v;
					offset = WEGetOffset(&thePoint, nil, hWE);
				}
			}
			break;
		}

		case kGoPageEnd:
		{
			oldOffset = offset ;

			//	get the offset corresponding to the bottom of the page
			thePoint.v = pWE->viewRect.bottom;
			thePoint.h = pWE->destRect.left;
			offset = WEGetOffset(&thePoint, nil, hWE);

			//	if the bottom line isn't completely visible, pick the line above
			WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);
			if (thePoint.v + lineHeight > pWE->viewRect.bottom)
			{
				thePoint.v -= lineHeight;
				offset = WEGetOffset(&thePoint, nil, hWE);
			}

			//	if we're already at the bottom of this page, scroll down a page
			if (offset == oldOffset)
			{
				scrollAmount = pWE->viewRect.bottom - pWE->viewRect.top - lineHeight;

				//	get the offset corresponding to the bottom of the next page
				thePoint.v = pWE->viewRect.bottom + scrollAmount;
				thePoint.h = pWE->destRect.left;
				offset = WEGetOffset(&thePoint, nil, hWE);

				//	if the bottom line isn't completely visible, pick the line above
				WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);
				if (thePoint.v + lineHeight > pWE->viewRect.bottom + scrollAmount)
				{
					thePoint.v -= lineHeight;
					scrollAmount = pWE->viewRect.top - thePoint.v;
					offset = WEGetOffset(&thePoint, nil, hWE);
				}
			}
			break;
		}

		case kGoTextStart:
		{
			offset = 0;
			break;
		}

		case kGoTextEnd:
		{
			offset = pWE->textLength;
			break;
		}
	}

	if (BTST(pWE->flags, weFBidirectional) && (arrow <= kGoRight))
	{
		//	bidirectional post-processing for left/right arrow keys
		prevDir = WEGetRunDirection(offset - 1, hWE);
		nextDir = WEGetRunDirection(offset, hWE);

		//	determine whether offset is at a direction boundary
		//	and whether we should cross it
		if ((prevDir != nextDir) && (prevDir != (arrow == kGoLeft)))
		{
			offset = _WECrossDirectionBoundary(offset, (arrow == kGoLeft), hWE);
		}
	} // bidirectional post-processing

	return offset;
}

pascal void _WEDoArrowKey (SInt16 arrow, EventModifiers modifiers, WEHandle hWE)
{
	// this routine is called by WEKey to handle arrow keys

	WEPtr pWE = *hWE;	// assume the WE record is already locked
	SInt16 action;
	SInt32 selStart, selEnd;
	SInt32 caretLoc, anchor;

	// calculate the "action" parameter for _WEArrowOffset from arrow and modifiers
	action = arrow - kArrowLeft;			// possible range: 0..3
	if (modifiers & optionKey)
	{
		action += kOption;
	}
	if (modifiers & cmdKey)
	{
		action += kCommand;
	}

	// get selection range
	selStart = pWE->selStart;
	selEnd = pWE->selEnd;

	if ((modifiers & shiftKey) == 0)
	{
		// if selection range isn't empty, collapse it to one of the endpoints
		if (selStart < selEnd)
		{
			if ((arrow == kArrowLeft) || (arrow == kArrowUp))
			{
				caretLoc = selStart;
			}
			else
			{
				caretLoc = selEnd;
			}
		}
		else
		{
			// otherwise move the insertion point
			caretLoc = _WEArrowOffset(action, selStart, hWE);
		}

		// set anchor to caretLoc, so new selection will be empty
		anchor = caretLoc;
	}
	else
	{
		// shift key was held down: extend the selection rather than replacing it
		// find out which selection boundary is the anchor and which is the free endpoint
		if (BTST(pWE->flags, weFAnchorIsEnd))
		{
			anchor = selEnd;
			caretLoc = selStart;
		}
		else
		{
			anchor = selStart;
			caretLoc = selEnd;
		}

		// move the free endpoint
		caretLoc = _WEArrowOffset(action, caretLoc, hWE);
	}

	// select the new selection
	WESetSelection(anchor, caretLoc, hWE);
}

pascal Boolean WEAdjustCursor(Point mouseLoc, RgnHandle mouseRgn, WEHandle hWE)
{
	// Call WEAdjustCursor to set the cursor shape when the mouse is in the view rectangle.
	// MouseRgn should be either a valid region handle or nil.
	// If mouseRgn is supplied (i.e., if it's not nil), it is intersected with a region
	// in global coordinates within which the cursor is to retain its shape.
	// WEAdjustCursor returns true if the cursor has been set.
	// Your application should set the cursor only if WEAdjustCursor returns false.

	WEPtr pWE;
	RgnHandle auxRgn;
	Point portDelta;
	GrafPtr savePort;
	CursHandle hCursor;
	SInt16 cursorID;
	Boolean saveWELock;
	Boolean adjustCursor;

	adjustCursor = false;
	cursorID = iBeamCursor;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// calculate delta between the local coordinate system and the global one
	portDelta.v = 0;
	portDelta.h = 0;
	LocalToGlobal(&portDelta);

	// calculate the visible portion of the view rectangle, in global coordinates
	auxRgn = NewRgn();
	CopyRgn(pWE->viewRgn, auxRgn);
	SectRgn(auxRgn, pWE->port->visRgn, auxRgn);
	OffsetRgn(auxRgn, portDelta.h, portDelta.v);

	if (PtInRgn(mouseLoc, auxRgn))
	{
		// mouse is within view rectangle: it's up to us to set the cursor
		adjustCursor = true;

#if WASTE_DRAG_AND_DROP
		// if drag-and-drop is enabled, see if the mouse is within current selection
		if (BTST(pWE->flags, weFHasDragManager) && BTST(pWE->features, weFDragAndDrop))
		{
			if (pWE->selStart < pWE->selEnd)
			{
				RgnHandle hiliteRgn ;

				// get current hilite region in global coordinates
				hiliteRgn = WEGetHiliteRgn(pWE->selStart, pWE->selEnd, hWE);
				OffsetRgn(hiliteRgn, portDelta.h, portDelta.v);

				// if mouse is within selection, set cursor to an arrow, else to an I-beam
				// (actually, we still use an I-beam if less than GetDblTime() ticks have
				// elapsed since the last mouse click, so that the cursor doesn't turn into an
				// arrow while triple-clicking + dragging a range of lines)

				if (PtInRgn(mouseLoc, hiliteRgn) && ((TickCount() > pWE->clickTime + GetDblTime()) ||
					(pWE->clickEdge == kObjectEdge)))
				{
					cursorID = 0;				// use arrow cursor
					CopyRgn(hiliteRgn, auxRgn);
				}
				else
				{
					DiffRgn(auxRgn, hiliteRgn, auxRgn);
				}

				// dispose of the hilite region
				DisposeRgn(hiliteRgn);

			} // if drag-and-drop is enabled
		}
#endif	// WASTE_DRAG_AND_DROP

		// set the cursor
		if (cursorID == 0)
		{
			SetCursor(&qd.arrow);
		}
		else
		{
			hCursor = GetCursor(cursorID);
			if ((hCursor != nil) && (*hCursor != nil))
			{
				SetCursor(*hCursor);
			}
		}

		// set mouseRgn, if provided
		if (mouseRgn != nil)
		{
			SectRgn(mouseRgn, auxRgn, mouseRgn);
		}
	}
	else
	{
		// mouse is outside view rectangle: don't set the cursor; subtract viewRgn from mouseRgn
		if (mouseRgn != nil)
		{
			DiffRgn(mouseRgn, auxRgn, mouseRgn);
		}
	}
	// dispose of the temporary region
	DisposeRgn(auxRgn);

	// restore the port
	SetPort(savePort);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	return adjustCursor;
}

pascal void WEIdle(UInt32 *maxSleep, WEHandle hWE)
{
	WEPtr pWE;
	UInt32 currentTime, blinkTime, sleep;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

#if WASTE_DEBUG
		_WESanityCheck(hWE);
#endif

	// the caret blinks only if we're active and the selection point is empty
	if (BTST(pWE->flags, weFActive) && (pWE->selStart == pWE->selEnd))
	{
		// get current time
		currentTime = TickCount();

		// calculate when the caret should be blinked again
		blinkTime = pWE->caretTime + GetCaretTime();

		if (currentTime < blinkTime)
		{
			sleep = blinkTime - currentTime;
		}
		else
		{
			_WEBlinkCaret(hWE);
			sleep = GetCaretTime();
		}
	}
	else
	{
		// if we don't need to blink the caret, we can sleep forever
		sleep = LONG_MAX;
	}

	// return sleepTime to the caller if maxSleep isn't nil
	if (maxSleep != nil)
	{
		*maxSleep = sleep;
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

pascal void WEUpdate(RgnHandle updateRgn, WEHandle hWE)
{
	WEPtr pWE;
	LongRect updateRect;
	Rect r;
	RgnHandle saveClip, auxRgn;
	GrafPtr savePort;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// save the clip region
	saveClip = NewRgn();
	GetClip(saveClip);

	// clip to the insersection between updateRgn and the view rectangle
	// (updateRgn may be nil; in this case, just clip to the view rectangle)
	auxRgn = NewRgn();
	SectRgn(saveClip, pWE->viewRgn, auxRgn);
	if (updateRgn != nil)
	{
		SectRgn(auxRgn, updateRgn, auxRgn);
	}
	SetClip(auxRgn);

	if (!EmptyRgn(auxRgn))
	{
		// calculate the rectangle to update
		r = (*auxRgn)->rgnBBox;
		WERectToLongRect(&r, &updateRect);

		// find out which lines need to be redrawn and draw them
		// if updateRgn is nil, erase each line rectangle before redrawing
		_WEDrawLines( _WEPixelToLine(updateRect.top - pWE->destRect.top, hWE),
					  _WEPixelToLine((updateRect.bottom - 1) - pWE->destRect.top, hWE),
					  (updateRgn == nil), hWE);

		// erase the portion of the update rectangle below the last line (if any)
		updateRect.top = pWE->destRect.top + (*pWE->hLines)[pWE->nLines].lineOrigin;
		if (updateRect.top < updateRect.bottom)
		{
			WELongRectToRect(&updateRect, &r);
			CallWEEraseProc(&r, hWE, pWE->eraseHook);
		}

		// hilite the selection range or draw the caret (only if active)
		if (pWE->selStart < pWE->selEnd)
		{
			_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);
		}
		else if (BTST(pWE->flags, weFCaretVisible))
		{
			_WEBlinkCaret(hWE);
			BSET(pWE->flags, weFCaretVisible);
		}
	}

	DisposeRgn(auxRgn);

	// restore the clip region
	SetClip(saveClip);
	DisposeRgn(saveClip);

	// restore the port
	SetPort(savePort);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

pascal void WEDeactivate(WEHandle hWE)
{
	WEPtr pWE;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// do nothing if we are already inactive
	if (BTST(pWE->flags, weFActive))
	{

		// hide the selection range or the caret
		_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);
		if (BTST(pWE->flags, weFCaretVisible))
		{
			_WEBlinkCaret(hWE);
		}

		// clear the active flag
		BCLR(pWE->flags, weFActive);

		// frame the selection
		_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);

		// dispose of the offscreen graphics world, if any
		if (pWE->offscreenPort != nil)
		{
			DisposeGWorld((GWorldPtr)(pWE->offscreenPort));
			pWE->offscreenPort = nil;
		}

#if WASTE_TSM_SUPPORT
		// notify Text Services
		if (pWE->tsmReference != nil)
		{
			DeactivateTSMDocument(pWE->tsmReference);
		}
#endif
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

pascal void WEActivate(WEHandle hWE)
{
	WEPtr pWE;
	Boolean saveWELock;

	if (WEIsActive(hWE)) return;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// remove the selection frame
	_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);

	// set the active flag
	BSET(pWE->flags, weFActive);

	// show the selection range
	_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);

#if WASTE_TSM_SUPPORT
	// notify Text Services
	if (pWE->tsmReference != nil)
	{
		ActivateTSMDocument(pWE->tsmReference);
	}
#endif

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

pascal Boolean WEIsActive(WEHandle hWE)
{
	// return true iff the specified WE instance is currently active
	return BTST((*hWE)->flags, weFActive) ? true : false;
}

pascal void WEScroll(SInt32 hOffset, SInt32 vOffset, WEHandle hWE)
{
	WEPtr pWE;
	Rect viewRect;
	GrafPtr savePort;
	Boolean hideOutline, saveWELock;

	// do nothing if both scroll offsets are zero
	if ((hOffset == 0) && (vOffset == 0))
	{
		return;
	}

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// get view rect in short coordinates
	viewRect = (*pWE->viewRgn)->rgnBBox;

	// hide the caret if it's showing
	if (BTST(pWE->flags, weFCaretVisible))
	{
		_WEBlinkCaret(hWE);
	}

	// if we're inactive and outline highlighting is on, we have to temporarily
	// hide the selection outline while scrolling to avoid a cosmetic bug
	hideOutline = false;
	if (!BTST(pWE->flags, weFActive))
	{
		if (BTST(pWE->features, weFOutlineHilite))
		{
			hideOutline = true;
			_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);
			BCLR(pWE->features, weFOutlineHilite);
		}
	}

	// offset the destination rectangle by the specified amount
	WEOffsetLongRect(&pWE->destRect, hOffset, vOffset);

	// scroll the view rectangle
	// we use ScrollRect unless the whole text is to be redrawn anyway
	// notice that ScrollRect takes short (16-bit) offset parameters,
	// while WEScroll deals with long (32-bit) quantities
	if ((ABS(hOffset) < (viewRect.right - viewRect.left)) && (ABS(vOffset) < (viewRect.bottom - viewRect.top)))
	{
		RgnHandle updateRgn = NewRgn();

		// ScrollRect will set updateRgn to the region to redraw
		ScrollRect(&viewRect, (SInt16) hOffset, (SInt16) vOffset, updateRgn);

		// redraw the exposed region
		WEUpdate(updateRgn, hWE);
		DisposeRgn(updateRgn);
	}
	else
	{
		// redraw the whole text
		WEUpdate(nil, hWE);
	}

	// redraw the selection outline, if hidden
	if (hideOutline)
	{
		BSET(pWE->features, weFOutlineHilite);
		_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);
	}

	// restore the port
	SetPort(savePort);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

pascal void WEPinScroll(SInt32 hOffset, SInt32 vOffset, WEHandle hWE)
{
	WEPtr pWE = *hWE;	// we ain't gonna move memory
	SInt32 delta;

	if (vOffset > 0)
	{
		delta = pWE->viewRect.top - pWE->destRect.top;

		// if top of the destRect would be moved below top of the viewRect,
		// pin it to top of the viewRect
		if (vOffset > delta)
		{
			vOffset = delta;
		}
	}
	else if (vOffset < 0)
	{
		delta = pWE->viewRect.bottom - pWE->destRect.bottom;

		// if bottom of the destRect would be moved above bottom of the viewRect,
		// pin it to bottom of viewRect
		if (vOffset < delta)
		{
			vOffset = delta;
		}
	}

	WEScroll(hOffset, vOffset, hWE);
}

pascal Boolean _WEScrollIntoView (SInt32 offset, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	LongPt thePoint;
	SInt16 lineHeight;
	SInt32 hScroll, vScroll, temp;
	Boolean retval = false;

	// do nothing if automatic scrolling is disabled
	if (BTST(pWE->features, weFAutoScroll))
	{

		// find the selection point
		WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);

		// assume no scrolling is needed
		vScroll = 0;
		hScroll = 0;

		// determine if we need to scroll up
		temp = pWE->viewRect.top - thePoint.v;
		if (temp > 0)
		{
			vScroll = temp;	// scroll up just enough to make the top line flush
		}

		//	determine if we need to scroll down
		temp = pWE->viewRect.bottom - (thePoint.v + lineHeight);
		if (temp < 0)
		{
			vScroll = temp;	// scroll down just enough to make the bottom line flush
		}

		if (vScroll != 0)
		{
			// we'd like to superimpose the bottom margins of the dest/view rects, if possible
			temp = pWE->viewRect.bottom - pWE->destRect.bottom;
			if (temp > vScroll)
			{
				vScroll = temp;
			}
			// but we also have to make sure the dest top isn't scrolled below the view top
			temp = pWE->viewRect.top - pWE->destRect.top;
			if (temp < vScroll)
			{
				vScroll = temp;
			}
		}

		// determine if we need to scroll horizontally
		if ((thePoint.h - 1 < pWE->viewRect.left) || (thePoint.h >= pWE->viewRect.right))
		{
			// calculate the amount of horizontal scrolling needed to center the selection into view
			hScroll = ((pWE->viewRect.left + pWE->viewRect.right) >> 1) - thePoint.h;

			// we'd like to superimpose the right margins of the dest/view rects, if possible
			temp = pWE->viewRect.right - pWE->destRect.right;
			if (temp > hScroll)
			{
				hScroll = temp;
			}

			// but we also have to make sure the dest left isn't scrolled to the right of the view left
			temp = pWE->viewRect.left - pWE->destRect.left;
			if (temp < hScroll)
			{
				hScroll = temp;
			}
		}

		// scroll the text if necessary
		if ((vScroll != 0) || (hScroll != 0))
		{
			retval = true;
			WEScroll(hScroll, vScroll, hWE);
			BSET(pWE->flags, weFDestRectChanged);
		}
	}

	// notify our client of changes to the destination rectangle
	if (BTST(pWE->flags, weFDestRectChanged))
	{
		if (pWE->scrollProc != nil)
		{
			CallWEScrollProc(hWE, pWE->scrollProc);
		}
		BCLR(pWE->flags, weFDestRectChanged);
	}

	return retval;
}

pascal void WESelView(WEHandle hWE)
{
	WEPtr pWE;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// scroll the free endpoint of the selection into view
	_WEScrollIntoView(BTST(pWE->flags, weFAnchorIsEnd) ? pWE->selStart : pWE->selEnd, hWE);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}
