/*
 *	WEDrawing.c
 *
 *	WASTE PROJECT
 *  Drawing routines and other basic support functions
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */

#include "WASTEIntf.h"

#if WASTE_OBJECTS

pascal OSErr WEGetSelectedObject(WEObjectDescHandle *hObjectDesc, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	WERunInfo runInfo;

	// assume current selection is not an embedded object
	*hObjectDesc = nil;

	// check selection range
	if (pWE->selEnd - pWE->selStart == 1)
	{
		// check run info
		WEGetRunInfo(pWE->selStart, &runInfo, hWE);
		if ((*hObjectDesc = runInfo.runAttrs.runStyle.tsObject) != nil)
		{
			return noErr;
		}
	}
	return weObjectNotFoundErr;
}

pascal SInt32 WEFindNextObject(SInt32 offset, WEObjectDescHandle *hObjectDesc, WEHandle hWE)
{
	WEPtr pWE = *hWE;
	WEStyleTableEntry *pStyles = *pWE->hStyles;
	WEObjectDescHandle obj;
	WERunArrayEntry *pRun;

	*hObjectDesc = nil;

	// do nothing if offset is already at the end of the text
	if (offset >= pWE->textLength - 1)
	{
		return kInvalidOffset;
	}

	// get a pointer to the run array entry immediately following offset
	pRun = *pWE->hRuns + WEOffsetToRun(offset + 1, hWE);

	// perform a linear scan of the run array looking for a run whose
	// corresponding style table entry points to an embedded object;
	// the search will stop anyway because the last run array entry has styleIndex = -1
	while (pRun->styleIndex >= 0)
	{
		if ((obj = pStyles[pRun->styleIndex].info.runStyle.tsObject) != nil)
		{
			*hObjectDesc = obj;
			return pRun->runStart;
		}
		pRun++;
	}

	return kInvalidOffset;

}

#endif  // WASTE_OBJECTS

pascal void _WEContinuousStyleRange(SInt32 rangeStart, SInt32 rangeEnd, WEStyleMode *mode, WETextStyle *ts, WEHandle hWE)
{
	// find out which style attributes are continous over the specified text range
	// on entry, *mode specifies which attributes are to be checked
	// on exit, *mode specifies the continuous attributes, also copied to ts

	SInt32 runIndex;
	WEStyleMode outMode;
	WERunInfo runInfo;

	outMode = *mode;

	// get style info at the beginning of the specified range
	runIndex = WEOffsetToRun(rangeStart, hWE);
	_WEGetIndStyle(runIndex, &runInfo, hWE);

	// copy the specified fields to ts
	_WECopyStyle(&runInfo.runAttrs.runStyle, ts, normal, (outMode & weDoAll) | weDoReplaceFace);

	// loop through style runs across the current selection range
	// if we determine that all specified attributes are discontinuous, we exit prematurely
	do
	{
		_WEGetIndStyle(runIndex, &runInfo, hWE);

		// determine which attributes have changed, if any
		if (outMode & weDoFont)
		{
			if (runInfo.runAttrs.runStyle.tsFont != ts->tsFont)
			{
				outMode &= (~weDoFont);
			}
		}
		if (outMode & weDoFace)
		{
			if (runInfo.runAttrs.runStyle.tsFace != ts->tsFace)
			{
				ts->tsFace &= runInfo.runAttrs.runStyle.tsFace;
				if (ts->tsFace == 0)
				{
					outMode &= (~weDoFace);
				}
			}
		}
		if (outMode & weDoFaceMask)
		{
			ts->tsFlags |= runInfo.runAttrs.runStyle.tsFace;
		}
		if (outMode & weDoSize)
		{
			if (runInfo.runAttrs.runStyle.tsSize != ts->tsSize)
			{
				outMode &= (~weDoSize);
			}
		}
		if (outMode & weDoColor)
		{
			if (!_WEBlockCmp(&runInfo.runAttrs.runStyle.tsColor, &ts->tsColor, sizeof(RGBColor)))
			{
				outMode &= (~weDoColor);
			}
		}

		runIndex++;
	} while ((outMode != 0) && (runInfo.runEnd < rangeEnd));

	*mode = outMode;
}

pascal void _WESynchNullStyle(WEHandle hWE)
{
	// This routine fills the nullStyle field of the WE record with valid information
	// and makes sure that the null style font belongs to the keyboard script.

	WEPtr pWE = *hWE;	// assume WE record is already locked
	SInt32 runIndex;
	WERunInfo runInfo;
#if !WASTE_NO_SYNCH
	ScriptCode keyboardScript;
	SInt16 fontID;
#endif

	// find the run index of the style run preceding the insertion point
	runIndex = WEOffsetToRun(pWE->selStart - 1, hWE);

	// if the nullStyle record is marked as invalid, fill it with the style attributes
	// associated with the character preceding the insertion point, and mark it as valid
	if (!BTST(pWE->flags, weFUseNullStyle))
	{
		_WEGetIndStyle(runIndex, &runInfo, hWE);
		pWE->nullStyle = runInfo.runAttrs;
		BSET(pWE->flags, weFUseNullStyle);
	}

#if !WASTE_NO_SYNCH
	// if only the Roman script is installed, we're finished
	if (!BTST(pWE->flags, weFNonRoman))
	{
		return;
	}

	// *** FONT / KEYBOARD SYNCHRONIZATION ***
	// get the keyboard script
	keyboardScript = GetScriptManagerVariable(smKeyScript);

	// find out what font will be used for the next character typed
	fontID = pWE->nullStyle.runStyle.tsFont;

	// do nothing if the font script is the same as the keyboard script
	if (FontToScript(fontID) == keyboardScript)
	{
		return;
	}

	// scan style runs starting from the insertion point backwards,
	// looking for the first font belonging to the keyboard script
	do
	{
		_WEGetIndStyle(runIndex, &runInfo, hWE);
		fontID = runInfo.runAttrs.runStyle.tsFont;
		if (FontToScript(fontID) == keyboardScript)
		{
			break;
		}
		runIndex--;
	} while (runIndex>=0);

	// if no font was ever used for the keyboard script, default to the
	// application font for the script
	if (runIndex < 0)
	{
		fontID = GetScriptVariable(keyboardScript, smScriptAppFond);
	}

	// change the font in the null style record
	pWE->nullStyle.runStyle.tsFont = fontID;
#endif
}

pascal Boolean WEContinuousStyle(WEStyleMode *mode, TextStyle *ts, WEHandle hWE)
{
	// find out which style attributes are continous over the selection range
	// on entry, the mode bitmap specifies which attributes are to be checked
	// on exit, the mode bitmap specifies the continuous attributes, also copied to ts
	// return true if all specified attributes are continuous

	WEPtr pWE;
	WEStyleMode oldMode;
	Boolean continuousStyle;
	Boolean saveWELock;

	// lock the WE record
	pWE = *hWE;
	saveWELock = _WESetHandleLock((Handle) hWE, true);

	// mask out unused bits in *mode
	*mode &= (weDoAll | weDoFaceMask);

	// two rather different paths are taken depending on whether
	// the selection range is empty or not
	if (pWE->selStart == pWE->selEnd)
	{
		// if the selection range is empty, always return true and set ts
		// from the nullStyle record, after having validated it
		continuousStyle = true;
		_WESynchNullStyle(hWE);
		_WECopyStyle(&pWE->nullStyle.runStyle, (WETextStyle *) ts, normal, (*mode & weDoAll) | weDoReplaceFace);
	}
	else
	{
		// otherwise get the continuous style attributes over the selection range
		oldMode = *mode;
		_WEContinuousStyleRange(pWE->selStart, pWE->selEnd, mode, (WETextStyle *) ts, hWE);

		// return true if mode hasn't changed
		continuousStyle = (oldMode == *mode);
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	return continuousStyle;
}

static pascal Boolean GetSegmentDirection(SInt16 segmentIndex, const struct FormatOrderData *data)
{
	return _WEGetIndDirection(data->firstRun + segmentIndex, data->hWE);
}

pascal void _WESegmentLoop(SInt32 firstLine, SInt32 lastLine, WESegmentLoopProcPtr callback, void *callbackData, WEHandle hWE)
{
	// For each style segment on every line in the specified range, set up
	// text attributes in the port and call the callback.
	// the WE record must be already locked

	WEPtr pWE = *hWE;
	WELineRec *pLine;
	Ptr pText;
	SInt32 lineIndex;
	SInt32 firstRun;
	SInt32 segmentIndex, segmentCount;
	SInt32 runIndex, currentRunIndex;
	SInt32 lineStart, lineEnd, segmentStart, segmentEnd;
	JustStyleCode styleRunPosition;
	WERunInfo runInfo;
	SInt16 autoOrdering[ kAutoOrderingSize ];
	Handle hTempOrdering = nil;
	SInt16 *pOrdering = nil;
	struct FormatOrderData formatOrderData;
	Boolean isRightToLeft;
	Boolean saveLineLock;
	Boolean saveTextLock;
	QDEnvironment saveEnvironment;

	static StyleRunDirectionUPP directionProc = nil;

	// create a routine descriptor for GetSegmentDirection, if we haven't already
	if (directionProc == nil)
	{
		directionProc = NewStyleRunDirectionUPP((StyleRunDirectionProcPtr) &GetSegmentDirection);
	}

	// is right-to-left the dominant line direction?
	isRightToLeft = IsRightToLeft(pWE->direction);

	// save the Quickdraw environment
	_WESaveQDEnvironment(pWE->port, BTST(pWE->flags, weFHasColorQD) ? true : false, &saveEnvironment);

	// make sure firstLine and lastLine are within the allowed range
	lineIndex = pWE->nLines - 1;
	firstLine = _WEPinInRange(firstLine, 0, lineIndex);
	lastLine = _WEPinInRange(lastLine, 0, lineIndex);

	// lock the line array
	saveLineLock = _WESetHandleLock((Handle) pWE->hLines, true);
	pLine = *pWE->hLines + firstLine;

	// lock the text
	saveTextLock = _WESetHandleLock(pWE->hText, true);
	pText = *pWE->hText;

	currentRunIndex = -1;

	// loop thru the specified lines
	for ( lineIndex = firstLine; lineIndex <= lastLine; lineIndex++ )
	{
		// get line start and line end
		lineStart = pLine[0].lineStart;
		lineEnd = pLine[1].lineStart;

		// calculate the index to the first style run on the line
		firstRun = WEOffsetToRun(lineStart, hWE);

		// calculate the number of segments on this line
		segmentCount = WEOffsetToRun(lineEnd - 1, hWE) - firstRun + 1;

		if (BTST(pWE->flags, weFBidirectional))
		{
			// SPECIAL PROCESSING FOR BIDIRECTIONAL SCRIPTS
			// we need an array of segmentCount entries for GetFormatOrder
			// in most cases, stack storage will be sufficient
			if (segmentCount <= kAutoOrderingSize)
			{
				pOrdering = autoOrdering;
			}
			else
			{
				// in rare cases the ordering array must be allocated dynamically
				_WEForgetHandle(&hTempOrdering);
				if (_WEAllocate(segmentCount * sizeof(SInt16), kAllocTemp, &hTempOrdering) != noErr)
				{
					// couldn't allocate memory: emergency exit
					break;
				}
				HLock(hTempOrdering);
				pOrdering = (SInt16 *) *hTempOrdering;
			}

			// obtain the correct display order for the segments
			if (segmentCount > 1)
			{
				formatOrderData.firstRun = firstRun;
				formatOrderData.hWE = hWE;
				GetFormatOrder((FormatOrderPtr) pOrdering, 0, segmentCount - 1, isRightToLeft, directionProc, (Ptr) &formatOrderData);
			}
			else
			{
				pOrdering[0] = 0;
			}
		}

		// loop thru each segment on this line
		for ( segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++ )
		{
			if (BTST(pWE->flags, weFBidirectional))
			{
				runIndex = pOrdering[ segmentIndex ];
			}
			else
			{
				runIndex = segmentIndex;
			}
			runIndex += firstRun;

			if (currentRunIndex != runIndex)
			{
				// get style run information for the current style run
				_WEGetIndStyle(runIndex, &runInfo, hWE);

				// set new text attributes
				TextFont(runInfo.runAttrs.runStyle.tsFont);
				TextFace(runInfo.runAttrs.runStyle.tsFace);
				TextSize(runInfo.runAttrs.runStyle.tsSize);

				// remember current run index
				currentRunIndex = runIndex;
			}

			// determine segment boundaries
			segmentStart = (runInfo.runStart <= lineStart) ? lineStart : runInfo.runStart;
			segmentEnd = (runInfo.runEnd >= lineEnd) ? lineEnd : runInfo.runEnd;

			// determine the relative position of this segment on the line
			styleRunPosition = 0;		// onlyStyleRun
			if (segmentIndex < segmentCount - 1)
			{
				styleRunPosition += 1;	// leftStyleRun or middleStyleRun
			}
			if (segmentIndex > 0)
			{
				styleRunPosition += 2;	// rightStyleRun or middleStyleRun
			}

			// do the callback
			if (callback(pLine, &runInfo.runAttrs, pText + segmentStart, segmentStart, segmentEnd - segmentStart, styleRunPosition, hWE, callbackData))
			{
				break;
			}
		};
		pLine++;
	}

	// unlock the text
	_WESetHandleLock(pWE->hText, saveTextLock);

	// unlock the line array
	_WESetHandleLock((Handle) pWE->hLines, saveLineLock);

	// restore the Quickdraw environment
	_WERestoreQDEnvironment(&saveEnvironment);

	// dispose of any temporary storage
	_WEForgetHandle(&hTempOrdering);
}

pascal void _WEDrawTSMHilite(Rect *segmentRect, UInt8 tsFlags)
{
	SInt16 underlineHeight;
	RGBColor background, foreground, saveForeground;
	Boolean isColorPort;
	Boolean usingTrueGray;
	Pattern	pattern;

	isColorPort = IsPortColor(GetQDGlobalsThePort());
	usingTrueGray = false;

	// by default, the pen pattern is solid
	PenPat(GetQDGlobalsBlack(&pattern));

	// if we're drawing in color, set the foreground color
	if (isColorPort)
	{
		// save foreground color
		GetForeColor(&saveForeground);

		// by default, the foreground color is black
		foreground.red = 0;
		foreground.green = 0;
		foreground.blue = 0;

		// if we're underlining raw (unconverted) text, see if a "true gray" is available
		if (!BTST(tsFlags, tsTSMConverted))
		{
			GetBackColor(&background);
			usingTrueGray = GetGray(GetGDevice(), &background, &foreground);
		} // if raw text

		// set the foreground color
		RGBForeColor(&foreground);
	} // if color graf port

	// if we're underlining raw (unconverted) text and no true gray is available,
	// simulate gray with a 50% pattern
	if (!BTST(tsFlags, tsTSMConverted))
	{
		if (!usingTrueGray)
		{
			PenPat(GetQDGlobalsGray(&pattern));
		}
	}
	// use a 2-pixel tall underline if text is "selected", else use a 1-pixel tall underline
	underlineHeight = BTST(tsFlags, tsTSMSelected) ? 2 : 1;

	// segmentRect becomes the rectangle to paint
	InsetRect(segmentRect, 1, 0);
	segmentRect->top = segmentRect->bottom - underlineHeight;

	// draw the underline
	PaintRect(segmentRect);

	// restore the foreground color
	if (isColorPort)
	{
		RGBForeColor(&saveForeground);
	}
}

static Boolean SLDraw
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
#pragma unused(segmentStart)
	struct SLDrawData *cd = (struct SLDrawData *) callbackData;
	WEPtr pWE = *hWE;
	Fixed slop;
	Rect segmentRect;
	RGBColor rgbTemp1, rgbTemp2;

	// is this the first segment on this line?
	if (IS_LEFTMOST_RUN(styleRunPosition))
	{
		// calculate the line rectangle (the rectangle which completely encloses the current line)
		cd->lineRect.left = pWE->destRect.left;
		cd->lineRect.right = pWE->destRect.right;
		cd->lineRect.top = pWE->destRect.top + pLine[0].lineOrigin;
		cd->lineRect.bottom = pWE->destRect.top + pLine[1].lineOrigin;

		// calculate the visible portion of this rectangle
		// we do this by intersecting the line rectangle with the view rectangle
		GetRegionBounds(pWE->viewRgn, &cd->drawRect);
		SectRect(&cd->lineRect, &cd->drawRect, &cd->drawRect);

		if (cd->usingOffscreen)
		{
			// calculate the boundary rectangle for the offscreen buffer
			// this is simply drawRect converted to global coordinates
			cd->bounds = cd->drawRect;
			LocalToGlobal((Point *) &cd->bounds.top);
			LocalToGlobal((Point *) &cd->bounds.bottom);

			// update the offscreen graphics world for the new bounds (this could fail)
			cd->drawingOffscreen = false;
			if (! (UpdateGWorld((GWorldPtr *) &pWE->offscreenPort, 0, &cd->bounds, nil, nil, 0) & gwFlagErr))
			{
				// NOTE: when running on a 68000 machine with the original Quickdraw,
				// a GWorld is just an extended GrafPort, and GetGWorldPixMap actually
				// returns a handle to a _copy_ of the GrafPort portBits (a BitMap, not a PixMap).
				// An important side-effect of this is that when we call SetOrigin,
				// only the original portBits is offset, not the copy.
				// get the pixel map associated with the offscreen graphics world
				cd->offscreenPixels = GetGWorldPixMap((GWorldPtr) pWE->offscreenPort);

				// lock it down
				if (LockPixels(cd->offscreenPixels))
				{
					// offscreen pixel buffer allocation was successful
					cd->drawingOffscreen = true;

					// get background color of onscreen port
					GetBackColor(&rgbTemp1);

					// switch graphics world
					SetGWorld((GWorldPtr) pWE->offscreenPort, nil);

					// synchronize the coordinate system of the offscreen port with that of the screen port
					SetOrigin(cd->drawRect.left, cd->drawRect.top);

					// synchronize the background color
					RGBBackColor(&rgbTemp1);

					// reset the offscreen clip region
					ClipRect(&cd->drawRect);
				}
			} // if pixel buffer allocation was successful
		} // if usingOffscreen

		// if doErase is true, erase the drawable area before drawing text
		if (cd->doErase)
		{
			CallWEEraseProc(&cd->drawRect, hWE, pWE->eraseHook);
		}

		// position the pen
		MoveTo(cd->lineRect.left + _WECalcPenIndent(pLine, pWE->alignment, pWE->direction), cd->lineRect.top + pLine->lineAscent);
	} // if first segment on line

	// if drawingOffscreen, switch thePort to the offscreen port
	// and synchronize text attributes
	if (cd->drawingOffscreen)
	{
		SetPort(pWE->offscreenPort);
		TextFont(pAttrs->runStyle.tsFont);
		TextFace(pAttrs->runStyle.tsFace);
		TextSize(pAttrs->runStyle.tsSize);
	} // if drawingOffscreen

	// get horizontal coordinate of the pen before drawing the segment
	GetPen((Point *) &segmentRect.top);

	// set the foreground color
	if (cd->usingColor)
	{
		//RGBForeColor(&pAttrs->runStyle.tsColor);
		//ShadowIRC modifications: for inverse and background color styles.
		int x;
		
		GetBackColor(&rgbTemp1); //must do anyways
		if((x = pAttrs->runStyle.extraFlags & (kInverse | kBackColor)) != 0)
		{
			if(x == (kInverse | kBackColor))
			{
				TextMode(srcCopy);
				RGBForeColor(&pAttrs->runStyle.tsBackColor);
				RGBBackColor(&pAttrs->runStyle.tsColor);
			}
			else if(x == kInverse)
			{
				TextMode(notSrcBic);
				RGBBackColor(&pAttrs->runStyle.tsColor);
				RGBForeColor(&rgbTemp1);
			}
			else if(x == kBackColor)
			{
				TextMode(srcCopy);
				RGBForeColor(&pAttrs->runStyle.tsColor);
				RGBBackColor(&pAttrs->runStyle.tsBackColor);
			}
		}
		else
			RGBForeColor(&pAttrs->runStyle.tsColor);
	}

#if WASTE_OBJECTS
	if (pAttrs->runStyle.tsObject != nil)
	{
		_WEDrawObject(pAttrs->runStyle.tsObject);
	}
	else
#endif
	{
		slop = 0;

		// calculate the "slop" (extra space) for this text segment (justified text only)
		if (pWE->alignment == weJustify)
		{
			// if this is the last segment on the line, strip trailing spaces
			if (IS_RIGHTMOST_RUN(styleRunPosition))
			{
				segmentLength = VisibleLength(pSegment, segmentLength);
			}
			// calculate how much extra space is to be applied to this text segment
			slop = FixMul(PortionLine(pSegment, segmentLength, styleRunPosition,
					kOneToOneScaling, kOneToOneScaling), pLine->lineJustAmount);

		} // if (alignment == weJustify)

		// draw the segment
		CallWEDrawTextProc(pSegment, segmentLength, slop, styleRunPosition, hWE, pWE->drawTextHook);
	}

	// get horizontal coordinate of the pen after drawing the segment
	GetPen((Point *) &segmentRect.bottom);
	segmentRect.bottom = cd->lineRect.bottom;

	// if this segment is in the TSM area, underline it in the appropriate way
	if (BTST(pAttrs->runStyle.tsFlags, tsTSMHilite))
	{
		_WEDrawTSMHilite(&segmentRect, pAttrs->runStyle.tsFlags);
	}

	if(pAttrs->runStyle.extraFlags & (kInverse | kBackColor))
	{
		TextMode(srcOr);
		RGBBackColor(&rgbTemp1);
	}
	
	if (cd->drawingOffscreen)
	{
		if (IS_RIGHTMOST_RUN(styleRunPosition))
		{
			// after drawing offscreen the last segment,
			// prepare to copy the offscreen buffer to video RAM

			// first set the graphics world to the screen port
			SetGWorld((CGrafPtr) cd->screenPort, cd->screenDevice);

			// before calling CopyBits, set the foreground color to black
			// and the background to white to avoid colorization
			if (cd->usingColor)
			{
				//ShadowIRC: comment out this get since we do it above
				//GetBackColor(&rgbTemp1);
				rgbTemp2.red = 0xFFFF;
				rgbTemp2.green = 0xFFFF;
				rgbTemp2.blue = 0xFFFF;
				RGBBackColor(&rgbTemp2);
				rgbTemp2.red = 0;
				rgbTemp2.green = 0;
				rgbTemp2.blue = 0;
				RGBForeColor(&rgbTemp2);
			}

			// copy the offscreen image of the [visible portion of the] line to the screen
			CopyBits(GetPortBitMapForCopyBits(pWE->offscreenPort), 
					 GetPortBitMapForCopyBits(cd->screenPort),
					 &cd->drawRect, &cd->drawRect, srcCopy, nil);

			// restore the original background color in the onscreen port
			if (cd->usingColor)
			{
				RGBBackColor(&rgbTemp1);
			}

			// restore the original offscreen coordinate system and unlock the pixel image
			SetPort(pWE->offscreenPort);
			SetOrigin(0, 0);
			if (cd->usingColor)
			{
				RGBForeColor(&rgbTemp2);
			}
			UnlockPixels(cd->offscreenPixels);

		} // if last segment

		// restore the screen port for _WESegmentLoop
		SetPort(cd->screenPort);
	} // if drawingOffscreen

	return false;	// keep looping
}

pascal void _WEDrawLines (SInt32 firstLine, SInt32 lastLine, Boolean doErase, WEHandle hWE)
{
	// draw the specified range of lines
	// we can safely assume that the WE record is already locked
	// and the port is already set the pWE->port

	WEPtr pWE = *hWE;			// assume WE record is locked
	struct SLDrawData cd;		// block of data we'll pass to SLDraw
	RgnHandle visRgn;
	Boolean ret;

	BLOCK_CLR(cd);
	cd.doErase = doErase;
	cd.usingColor = (BTST(pWE->flags, weFHasColorQD) && !BTST(pWE->features, weFInhibitColor)) ? true : false;

	// do nothing if our graphics port is not visible
	ret = false;
	
	if ((visRgn = NewRgn()))
	{
		GetPortVisibleRegion(pWE->port, visRgn);
		if (EmptyRgn(visRgn))
			ret = true;
		DisposeRgn(visRgn);
		
		if(ret)
			return;
	}

	// save graphics world
	GetGWorld((GWorldPtr *) &cd.screenPort, &cd.screenDevice);

	// If doErase is true, we're drawing over old text, so we must erase each line
	// before redrawing it.  But if the weFDrawOffscreen feature is enabled, we draw
	// the entire line offscreen and  we copy the image right over the old line,
	// without erasing it, thus achieving a very smooth drawing effect.

	if ((doErase) && BTST(pWE->features, weFDrawOffscreen))
	{
		// has an offscreen world already been allocated?
		if (pWE->offscreenPort == nil)
		{
			// nope,  create one; its bounds are set initially to an arbitrary rectangle
			SetRect(&cd.bounds, 0, 0, 1, 1);

			// if NewGWorld fails, it will set pWE->offscreenPort to nil
			NewGWorld((GWorldPtr *) &pWE->offscreenPort, 0, &cd.bounds, nil, nil, pixPurge + noNewDevice + useTempMem);
		}
		cd.usingOffscreen = (pWE->offscreenPort != nil);
	}

	_WESegmentLoop(firstLine, lastLine, SLDraw, &cd, hWE);

	// restore graphics world
	SetGWorld((GWorldPtr) cd.screenPort, cd.screenDevice);
}

pascal SInt16 _WECalcPenIndent(const WELineRec *pLine, WEAlignment alignment, WEDirection direction)
{
	SInt16 retval = 0;

	switch (alignment)
	{
		case weFlushLeft:
		{
			break;
		}

		case weFlushRight:
		{
			retval = pLine->lineSlop;
			break;
		}

		case weCenter:
		{
			retval = pLine->lineSlop / 2;
			break;
		}

		case weJustify:
		{
			if (pLine->lineJustAmount > 0)
			{
				break;
			}
			// deliberate fall through to default case for
			// last line of justified paragraph
		}

		default:
		{
			if (IsRightToLeft(direction))
			{
				retval = pLine->lineSlop;
			}
			break;
		}
	}

	return retval;
}

pascal void _WESaveQDEnvironment(GrafPtr port, Boolean saveColor, QDEnvironment *environment)
{
	GetPort(&environment->envPort);
	SetPort(port);
	GetPenState(&environment->envPen);
	PenNormal();
	
	environment->envStyle.tsFont = GetPortTextFont(port);
	environment->envStyle.tsFace = GetPortTextFace(port);
	environment->envStyle.tsSize = GetPortTextSize(port);
	environment->envMode = GetPortTextMode(port);
	environment->envStyle.tsFlags = saveColor;		// remember if color was saved
	if (saveColor)
	{
		GetForeColor(&environment->envStyle.tsColor);
//ShadowIRC: Save background color
		GetBackColor(&environment->envStyle.tsBackColor);
	}
	TextMode(srcOr);
}

pascal void _WERestoreQDEnvironment(const QDEnvironment *environment)
{
	SetPenState(&environment->envPen);
	TextFont(environment->envStyle.tsFont);
	TextFace(environment->envStyle.tsFace);
	TextSize(environment->envStyle.tsSize);
	TextMode(environment->envMode);
	if (environment->envStyle.tsFlags)
	{
		RGBForeColor(&environment->envStyle.tsColor);
//ShadowIRC: Restore background color
		RGBBackColor(&environment->envStyle.tsBackColor);
	}
	SetPort(environment->envPort);
}

pascal void _WEFillFontInfo (GrafPtr port, WERunAttributes *targetStyle)
{
	// given a WERunAttributes record, fill in the runHeight, runAscent fields etc.
	FontInfo fInfo;
	QDEnvironment saveEnvironment;

	_WESaveQDEnvironment(port, false, &saveEnvironment);

	// we don't want a zero font size; although Quickdraw accepts zero to mean
	// the default font size, it can cause trouble to us when we do calculations
	if (targetStyle->runStyle.tsSize == 0)
	{
		targetStyle->runStyle.tsSize = 12;
	}

	// set the text attributes
	TextFont(targetStyle->runStyle.tsFont);
	TextSize(targetStyle->runStyle.tsSize);
	TextFace(targetStyle->runStyle.tsFace);
	GetFontInfo(&fInfo);
	targetStyle->runHeight = fInfo.ascent + fInfo.descent + fInfo.leading;
	targetStyle->runAscent = fInfo.ascent;

	_WERestoreQDEnvironment(&saveEnvironment);
}

pascal void _WECopyStyle(const WETextStyle *sourceStyle, WETextStyle *targetStyle,
				Style offStyles, WEStyleMode mode)
{
	// Copy some or all of the attributes composing sourceStyle to targetStyle.
	// The mode parameter determines which attributes are to be copied and how.
	// If mode contains weDoToggleFace,  offStyles indicates which
	// Quickdraw styles are to be turned off.

	// COPY FONT
	if (mode & weDoFont)
	{
			targetStyle->tsFont = sourceStyle->tsFont;
#if WASTE_RESOLVE_FONT_DESIGNATORS
			if (targetStyle->tsFont == systemFont)
			{
				targetStyle->tsFont = GetSysFont();
			}
			if (targetStyle->tsFont == applFont)
			{
				targetStyle->tsFont = GetAppFont();
			}
#endif
	}

	// COPY SIZE
	if (mode & (weDoSize | weDoAddSize))
	{
		// copy size to a long variable to avoid integer overflows when doing additions
		SInt32 longSize = sourceStyle->tsSize;

		// zero really means 12
		if (longSize == 0)
		{
			longSize = 12;
		}

		// if kModeAddSize is set, the source size is added to the target size,
		// otherwise the source size replaces the target size outright
		if (mode & weDoAddSize)
		{
			longSize += targetStyle->tsSize;
		}
		// range-check the resulting size
		longSize = _WEPinInRange(longSize, kMinFontSize, kMaxFontSize);
		targetStyle->tsSize = longSize;
	} // if alter size

	// COPY FACE
	if (mode & (weDoFace | weDoFaceMask))
	{
		Style sourceFace = sourceStyle->tsFace;
		Style targetFace = targetStyle->tsFace;

		if (mode & weDoFaceMask)
		{
			// USE MASK
			// if kModeFaceMask is set, copy the Quickdraw styles (tsFace field)
			// using the tsFlags field as a mask specifying which bits in the tsFace
			// field are to be copied.
			Style sourceMask = sourceStyle->tsFlags;
			targetFace = (sourceFace & sourceMask) | (targetFace & (~sourceMask));
		}
		else
		{
			// IGNORE MASK
			// sourceFace replaces targetFace outright if one or both of these conditions hold:
			// 1. sourceFace is zero (= empty set = plain text)
			// 2. the kModeReplaceFace bit is set

			if ((sourceFace == normal) || (mode & weDoReplaceFace))
			{
				targetFace = sourceFace;
			}
			else
			{
				// Otherwise sourceFace is interpreted as a bitmap indicating
				// which styles are to be altered -- all other styles are left intact.
				// What exactly happens to the styles indicated in sourceFace
				// depends on whether the kModeToggleFace bit is set or clear.

				// if kModeToggleFace is set, turn a style off if it's set in offStyles,
				// else turn it on
				if (mode & weDoToggleFace)
				{
					targetFace = (sourceFace ^ offStyles) | (targetFace & (~sourceFace));
				}
				else
				{
					// if kModeToggleFace is clear, turn on the styles specified in sourceStyle
					targetFace |= sourceFace;
				}
			}
		}

		// the condense and extend attributes are mutually exclusive: if one is set
		// in sourceFace, remove it from targetFace
		if (sourceFace & condense)
		{
			targetFace &= (~extend);
		}
		if (sourceFace & extend)
		{
			targetFace &= (~condense);
		}

		targetStyle->tsFace = targetFace;
	}

	// COPY COLOR
	if (mode & weDoColor)
	{
		targetStyle->tsColor = sourceStyle->tsColor;
		//ShadowIRC: Save background color
		targetStyle->tsBackColor = sourceStyle->tsBackColor;
	}

#if WASTE_OBJECTS
	// if kModeObject is set, copy object descriptor
	if (mode & weDoObject)
	{
		targetStyle->tsObject = sourceStyle->tsObject;
	}
#endif

	//ShadowIRC: Save extra flags
	targetStyle->extraFlags = sourceStyle->extraFlags;
	// always clear targetStyle->tsFlags by default
	targetStyle->tsFlags = 0;

	// if kModeFlags is set, copy the tsFlags field
	if (mode & weDoFlags)
	{
		targetStyle->tsFlags = sourceStyle->tsFlags;
	}
}

pascal Boolean _WEOffsetInRange(SInt32 offset, WEEdge edge, SInt32 rangeStart, SInt32 rangeEnd)
{
	// return true if the position specified by the pair < offset, edge >
	// is within the specified range

	// if edge is kTrailingEdge, offset really refers to the preceding character
	if (edge == kTrailingEdge)
	{
		offset--;
	}
	// return true iff offset is within the specified range
	return ((offset >= rangeStart) && (offset < rangeEnd));
}
