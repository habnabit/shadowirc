/*
 *	WEBirthDeath.c
 *
 *	WASTE PROJECT
 *  Creation and Destruction, Standard Procs, etc.
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

#if GENERATINGCFM
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif
#endif

const Point kOneToOneScaling = { 1, 1 };

// static variables

static WEDrawTextUPP		_weStdDrawTextProc = nil;
static WEPixelToCharUPP		_weStdPixelToCharProc = nil;
static WECharToPixelUPP		_weStdCharToPixelProc = nil;
static WELineBreakUPP		_weStdLineBreakProc = nil;
static WEWordBreakUPP		_weStdWordBreakProc = nil;
static WECharByteUPP		_weStdCharByteProc = nil;
static WECharTypeUPP		_weStdCharTypeProc = nil;
static WEClickLoopUPP		_weStdClickLoopProc = nil;
#if WASTE_DRAG_AND_DROP
static WEHiliteDropAreaUPP	_weStdHiliteDropAreaProc = nil;
#endif
static WEEraseUPP			_weStdEraseProc = nil;

static pascal void _WEStdDrawText(Ptr pText, SInt32 textLength, Fixed slop,
				JustStyleCode styleRunPosition, WEHandle hWE)
{
#pragma unused(hWE)
	DrawJustified(pText, textLength, slop, styleRunPosition,
		  kOneToOneScaling, kOneToOneScaling);
}

static pascal SInt32 _WEStdPixelToChar(Ptr pText, SInt32 textLength, Fixed slop,
				Fixed *width, WEEdge *edge, JustStyleCode styleRunPosition,
				Fixed hPos, WEHandle hWE)
{
#pragma unused(hPos, hWE)
	Fixed lastWidth;
	SInt32 offset;

	lastWidth = *width;
	offset = PixelToChar(pText, textLength, slop, lastWidth, (Boolean *) edge,
		width, styleRunPosition, kOneToOneScaling, kOneToOneScaling);

	// round width to nearest integer value
	// (this is supposed to fix an incompatibility with the WorldScript Power Adapter)
	*width = (*width + 0x00008000) & 0xFFFF0000;

	return offset;
}

static pascal SInt16 _WEStdCharToPixel(Ptr pText, SInt32 textLength, Fixed slop,
				SInt32 offset, SInt16 direction, JustStyleCode styleRunPosition,
				SInt16 hPos, WEHandle hWE)
{
#pragma unused(hPos, hWE)
	return CharToPixel(pText, textLength, slop, offset, direction,
			styleRunPosition, kOneToOneScaling, kOneToOneScaling);
}

static pascal StyledLineBreakCode _WERomanLineBreak
	(
		Ptr pText,
		SInt32 textLength,
		SInt32 textStart,
		SInt32 textEnd,
		Fixed * textWidth,
		SInt32 * textOffset,
		WEHandle we
	)
{
#pragma unused(we)

	SInt32 offset = 0, rawOffset ;
	StyledLineBreakCode breakCode = smBreakOverflow ;
	WEEdge edge ;

	//	do nothing if we were called for an empty line
	if ( textLength == 0 )
	{
		goto exit ;
	}

	//	first look for CRs
	for ( offset = textStart ; offset < textEnd ; offset ++ )
	{
		if ( pText [ offset ] == kEOL )
		{
			//	if a CR is found, ignore everything that follows and be sure to
			//	eventually break the line
			textEnd = offset + 1 ;
			breakCode = smBreakWord ;
			break ;
		}
	}

	//	find "raw" break offset
	rawOffset = textStart + PixelToChar ( pText + textStart, textEnd - textStart, 0,
			* textWidth, ( Boolean * ) & edge, textWidth,
			onlyStyleRun, kOneToOneScaling, kOneToOneScaling ) ;
	if ( edge == kTrailingEdge ) rawOffset -- ;

	//	if textWidth has gone negative, we need to break now
	if ( * textWidth < 0 )
	{
		//	find a suitable break point
		for ( offset = rawOffset ; offset > 0 ; offset -- )
		{
			//	look for a breaking character (everything whose ASCII code <= 0x20)
			if ( ( unsigned char ) pText [ offset - 1 ] <= ' ' )
			{
				if ( offset == rawOffset )
				{
					//	if rawOffset points to a breaking character already,
					//	skip any following blanks
					for ( ; ( offset < textEnd ) && ( ( unsigned char ) pText [ offset ] <= ' ' ) ; offset ++ )
						;
				}
				breakCode = smBreakWord ;
				goto exit ;
			}
		}

		//	we need to break but were unable to find a proper breaking character
		//	as a last resort, break the line on a non-breaking character
		offset = rawOffset ;
		breakCode = smBreakChar ;
	}
	else
	{
		//	we'll break this line later
		offset = textEnd ;
	}


exit :
	* textOffset = offset ;
	return breakCode ;
}

static pascal StyledLineBreakCode _WEStdLineBreak(Ptr pText, SInt32 textLength,
				SInt32 textStart, SInt32 textEnd, Fixed *textWidth,
				SInt32 *textOffset, WEHandle hWE)
{
#pragma unused(hWE)
	return StyledLineBreak(pText, textLength, textStart, textEnd, 0, textWidth,
				textOffset);
}

static pascal void _WEStdWordBreak(Ptr pText, SInt16 textLength, SInt16 offset,
				WEEdge edge, OffsetTable breakOffsets, ScriptCode script,
				WEHandle hWE)
{
#pragma unused(hWE)
	FindWordBreaks(pText, textLength, offset, (Boolean) edge, nil, breakOffsets, script);
}

static pascal SInt16 _WEStdCharByte(Ptr pText, SInt16 textOffset, ScriptCode script,
				WEHandle hWE)
{
#pragma unused(hWE)
	return CharacterByteType(pText, textOffset, script);
}

static pascal SInt16 _WEStdCharType(Ptr pText, SInt16 textOffset, ScriptCode script,
				WEHandle hWE)
{
#pragma unused(hWE)
	return CharacterType(pText, textOffset, script);
}

//	_WEScriptToFont, _WEOldWordBreak, _WEOldCharByte and _WEOldCharType
//	are only needed to support version 7.0 / 7.0.1 of MacOS;
//	CFM-based code (both PowerPC and CFM68K) doesn't need these routines
//	as the Code Fragment Manager requires at least MacOS 7.1.2
//	(on the PowerPC) or MacOS 7.1 (for CFM68K)

#if ! (SystemSevenFiveOrLater || GENERATINGCFM)

pascal SInt16 _WEScriptToFont(ScriptCode script)
{
	// given an explicit script code, return the first font ID in the corresponding range
	// for an explanation of the formula given below, see IM: Text, page B-8

	if (script == smRoman)
	{
		return 2;
	}
	else if ((script > smRoman) && (script <= smUninterp))
	{
		return (0x3E00 + 0x200 * script);
	}
	else
	{
		return systemFont;	// unknown script code (?)
	}
}

static pascal void _WEOldWordBreak(Ptr pText, SInt16 textLength, SInt16 offset,
				WEEdge edge, OffsetTable breakOffsets, ScriptCode script,
				WEHandle hWE)
{
	GrafPtr savePort, tempPort;
	SInt16 saveFont;

	// the old (now obsolete) FindWord routine gets an implicit script parameter through
	// the current graphics port txFont field, so first of all we must have a valid port
	GetPort(&savePort);
	tempPort = (*hWE)->port;
	SetPort(tempPort);

	// then set the txFont field to a font number in the specified script range
	saveFont = tempPort->txFont;
	TextFont(_WEScriptToFont(script));

	// call _FindWord
	FindWord(pText, textLength, offset, (Boolean)edge, nil, breakOffsets);

	// restore font and port
	TextFont(saveFont);
	SetPort(savePort);

}

static pascal SInt16 _WEOldCharByte(Ptr pText, SInt16 textOffset, ScriptCode script,
				WEHandle hWE)
{
	GrafPtr savePort, tempPort;
	SInt16 saveFont;
	SInt16 retVal;

	// the old (now obsolete) CharByte routine gets an implicit script parameter through
	// the current graphics port txFont field, so first of all we must have a valid port
	GetPort(&savePort);
	tempPort = (*hWE)->port;
	SetPort(tempPort);

	// then set the txFont field to a font number in the specified script range
	saveFont = tempPort->txFont;
	TextFont(_WEScriptToFont(script));

	// call _CharByte
	retVal = CharByte(pText, textOffset);

	// restore font and port
	TextFont(saveFont);
	SetPort(savePort);

	return retVal;
}

static pascal SInt16 _WEOldCharType(Ptr pText, SInt16 textOffset, ScriptCode script,
				WEHandle hWE)
{
	GrafPtr savePort, tempPort;
	SInt16 saveFont;
	SInt16 retVal;

	// the old (now obsolete) CharType routine gets an implicit script parameter through
	// the current graphics port txFont field, so first of all we must have a valid port
	GetPort(&savePort);
	tempPort = (*hWE)->port;
	SetPort(tempPort);

	// then set the txFont field to a font number in the specified script range
	saveFont = tempPort->txFont;
	TextFont(_WEScriptToFont(script));

	// call _CharType
	retVal = CharType(pText, textOffset);

	// restore font and port
	TextFont(saveFont);
	SetPort(savePort);

	return retVal;

}

#endif

static pascal Boolean _WEStdClickLoop(WEHandle hWE)
{
	WEPtr pWE = *hWE;		// assume WE record is already locked
	Point mouseLoc;
	SInt32 currentOffset;
	SInt32 maxOffset;
	SInt32 vDelta = 0;
	SInt32 hDelta = 0;

	// do nothing if auto-scroll is disabled or if we're inactive
	if (!BTST(pWE->features, weFAutoScroll) || !BTST(pWE->flags, weFActive))
	{
		return true;
	}

	// get current mouse location, in local coords
	// we can safely assume the graphics port is set up correctly
	GetMouse(&mouseLoc);

	// HANDLE VERTICAL AUTOSCROLL
	currentOffset = pWE->viewRect.top - pWE->destRect.top;
	maxOffset = (pWE->destRect.bottom - pWE->destRect.top) - (pWE->viewRect.bottom - pWE->viewRect.top);

	// is the mouse below the view rect?
	if (mouseLoc.v > pWE->viewRect.bottom)
	{
		// is there anything hidden below the view rect?
		if (currentOffset < maxOffset)
		{
			// then scroll down: calculate the scroll delta
			vDelta = pWE->viewRect.bottom - mouseLoc.v;

			// pin the new vertical offset to the bottom of the dest rectangle
			if (vDelta < (currentOffset - maxOffset))
			{
				vDelta = currentOffset - maxOffset;
			}

			// never scroll by more than kMaxScrollDelta pixels
			if (vDelta < -kMaxScrollDelta)
			{
				vDelta = -kMaxScrollDelta;
			}
		}
	}

	// is the mouse above the view rect?
	else if (mouseLoc.v < pWE->viewRect.top)
	{
		// is there anything hidden above the view rect?
		if (currentOffset > 0)
		{
			// then scroll up: calculate the scroll delta
			vDelta = pWE->viewRect.top - mouseLoc.v;

			// pin the new vertical offset to the top of the dest rectangle
			if (vDelta > currentOffset)
			{
				vDelta = currentOffset;
			}

			// never scroll by more than kMaxScrollDelta pixels
			if (vDelta > kMaxScrollDelta)
			{
				vDelta = kMaxScrollDelta;
			}
		}
	}

	// HANDLE HORIZONTAL AUTOSCROLL
	currentOffset = pWE->viewRect.left - pWE->destRect.left;
	maxOffset = (pWE->destRect.right - pWE->destRect.left) - (pWE->viewRect.right - pWE->viewRect.left);

	// is the mouse to the right of the view rect?
	if (mouseLoc.h > pWE->viewRect.right)
	{
		// is there anything hidden to the right of the view rect?
		if (currentOffset < maxOffset)
		{
			// then scroll right: calculate the scroll delta
			hDelta = pWE->viewRect.right - mouseLoc.h;

			// pin the new vertical offset to the rightmost edge
			// of the dest rectangle
			if (hDelta < (currentOffset - maxOffset))
			{
				hDelta = currentOffset - maxOffset;
			}

			// never scroll by more than kMaxScrollDelta pixels
			if (hDelta < -kMaxScrollDelta)
			{
				hDelta = -kMaxScrollDelta;
			}
		}
	}

	// is the mouse to the left of the view rect?
	else if (mouseLoc.h < pWE->viewRect.left)
	{
		// is there anything hidden to the left of the view rect?
		if (currentOffset > 0)
		{
			// then scroll up: calculate the scroll delta
			hDelta = pWE->viewRect.left - mouseLoc.h;

			// pin the new horizontal offset to the leftmost edge
			// of the dest rectangle
			if (hDelta > currentOffset)
			{
				hDelta = currentOffset;
			}

			// never scroll by more than kMaxScrollDelta pixels
			if (hDelta > kMaxScrollDelta)
			{
				hDelta = kMaxScrollDelta;
			}
		}
	}

	if ((vDelta != 0) || (hDelta != 0))
	{
		// do the scroll
		WEScroll(hDelta, vDelta, hWE);

		// notify our client we have scrolled the text
		if (pWE->scrollProc != nil)
		{
			CallWEScrollProc(hWE, pWE->scrollProc);
		}
	}

	return true;
}

#if WASTE_DRAG_AND_DROP
static pascal OSErr _WEStdHiliteDropArea(DragReference drag, Boolean hiliteFlag, WEHandle hWE)
{
	WEPtr pWE = *hWE;		// assume WE record is already locked
	RgnHandle tmpRgn;
	OSErr err;

	if (hiliteFlag)
	{
		tmpRgn = NewRgn();
		CopyRgn(pWE->viewRgn, tmpRgn);
		InsetRgn(tmpRgn, -kTextMargin, -kTextMargin);
		err = ShowDragHilite(drag, tmpRgn, true);
		DisposeRgn(tmpRgn);
	}
	else
	{
		err = HideDragHilite(drag);
	}
	return err;
}
#endif	// WASTE_DRAG_AND_DROP

static pascal void _WEStdErase(const Rect *area, WEHandle hWE)
{
#pragma unused(hWE)
	EraseRect(area);
}

#if WASTE_TSM_SUPPORT
pascal OSErr _WERegisterWithTSM(WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	InterfaceTypeList typeList;
	OSErr err;

	// do nothing if the Text Services Manager isn't available
	if (BTST(pWE->flags, weFHasTextServices))
	{
		typeList[0] = kTextService;
		if ((err = NewTSMDocument(1, typeList, &pWE->tsmReference, (SInt32) hWE)) != noErr)
		{
			// we don't consider it an error if our client application isn't TSM-aware
			if (err != tsmNeverRegisteredErr)
			{
				goto cleanup;
			}
		}
	}

	// clear result code
	err = noErr;

cleanup:
	// return result code
	return err;
}
#endif

pascal void _WESetStandardHooks(WEHandle hWE)
{
	WEPtr pWE;

	// the first time we're called, create routine descriptors
	if (_weStdDrawTextProc == nil)
	{
		_weStdDrawTextProc = NewWEDrawTextProc(_WEStdDrawText);
		_weStdPixelToCharProc = NewWEPixelToCharProc(_WEStdPixelToChar);
		_weStdCharToPixelProc = NewWECharToPixelProc(_WEStdCharToPixel);

		if (GetScriptManagerVariable(smEnabled) > 1)
		{
			//	one or more non-Roman scripts installed: use WorldScript routine
			_weStdLineBreakProc = NewWELineBreakProc(_WEStdLineBreak);
		}
		else
		{
			//	Roman-only case: use our own TextEdit-like line breaking routine
			_weStdLineBreakProc = NewWELineBreakProc(_WERomanLineBreak);
		}

#if ! (SystemSevenFiveOrLater || GENERATINGCFM)

		if (GetScriptManagerVariable(smVersion) < 0x0710)
		{
			// pre-7.1 version of the Script Manager: must use old hooks
			_weStdWordBreakProc = NewWEWordBreakProc(_WEOldWordBreak);
			_weStdCharByteProc = NewWECharByteProc(_WEOldCharByte);
			_weStdCharTypeProc = NewWECharTypeProc(_WEOldCharType);
		}
		else
#endif
		{
			// Script Manager version 7.1 or newer
			_weStdWordBreakProc = NewWEWordBreakProc(_WEStdWordBreak);
			_weStdCharByteProc = NewWECharByteProc(_WEStdCharByte);
			_weStdCharTypeProc = NewWECharTypeProc(_WEStdCharType);
		}

		_weStdClickLoopProc = NewWEClickLoopProc(_WEStdClickLoop);
#if WASTE_DRAG_AND_DROP
		_weStdHiliteDropAreaProc = NewWEHiliteDropAreaProc(_WEStdHiliteDropArea);
#endif
		_weStdEraseProc = NewWEEraseProc(_WEStdErase);

	} // if called for the first time

	// replace null hook fields with the addresses of the standard hooks

	pWE = *hWE;

	if (pWE->drawTextHook == nil)
	{
		pWE->drawTextHook = _weStdDrawTextProc;
	}
	if (pWE->pixelToCharHook == nil)
	{
		pWE->pixelToCharHook = _weStdPixelToCharProc;
	}
	if (pWE->charToPixelHook == nil)
	{
		pWE->charToPixelHook = _weStdCharToPixelProc;
	}
	if (pWE->lineBreakHook == nil)
	{
		pWE->lineBreakHook = _weStdLineBreakProc;
	}
	if (pWE->wordBreakHook == nil)
	{
		pWE->wordBreakHook = _weStdWordBreakProc;
	}
	if (pWE->charByteHook == nil)
	{
		pWE->charByteHook = _weStdCharByteProc;
	}
	if (pWE->charTypeHook == nil)
	{
		pWE->charTypeHook = _weStdCharTypeProc;
	}
	if (pWE->clickLoop == nil)
	{
		pWE->clickLoop = _weStdClickLoopProc;
	}
#if WASTE_DRAG_AND_DROP
	if (pWE->hiliteDropAreaHook == nil)
	{
		pWE->hiliteDropAreaHook = _weStdHiliteDropAreaProc;
	}
#endif
	if (pWE->eraseHook == nil)
	{
		pWE->eraseHook = _weStdEraseProc;
	}
}

pascal OSErr WENew(const LongRect *destRect, const LongRect *viewRect, UInt32 features, WEHandle *hWE)
{
	WEPtr pWE = nil;
	UInt32 allocFlags = kAllocClear;
	SInt32 response;
	Rect r;
	WERunAttributes attributes;
	OSErr err;

	// allocate the WE record
	if ((err = _WEAllocate(sizeof(WERec), allocFlags, (Handle *)hWE)) != noErr)
	{
		goto cleanup;
	}

	// lock it down
	HLock((Handle)*hWE);
	pWE = **hWE;

	// get active port
	GetPort(&pWE->port);

	// determine whether temporary memory should be used for data structures
	if (BTST(features, weFUseTempMem))
	{
		allocFlags += kAllocTemp;
	}

	// allocate the text handle (initially empty)
	if ((err = _WEAllocate(0, allocFlags, (Handle *)&pWE->hText)) != noErr)
	{
		goto cleanup;
	}

	// allocate the line array
	if ((err = _WEAllocate(2 * sizeof(WELineRec), allocFlags, (Handle *)&pWE->hLines)) != noErr)
	{
		goto cleanup;
	}

	// allocate the style table
	if ((err = _WEAllocate(sizeof(WEStyleTableEntry), allocFlags, (Handle *)&pWE->hStyles)) != noErr)
	{
		goto cleanup;
	}

	// allocate the run array
	if ((err = _WEAllocate(2 * sizeof(WERunArrayEntry), allocFlags, (Handle *)&pWE->hRuns)) != noErr)
	{
		goto cleanup;
	}

	// allocate the user info array
	if ((err = _WEAllocate(sizeof(WEUserInfoEntry), allocFlags, &pWE->hUserInfo)) != noErr)
	{
		goto cleanup;
	}

	// check for the presence of various system software features
	// determine whether Color Quickdraw is available
	if ((Gestalt(gestaltQuickdrawVersion, &response) == noErr) && (response >= gestalt8BitQD))
	{
		BSET(pWE->flags, weFHasColorQD);
	}

#if WASTE_DRAG_AND_DROP
	// determine whether the Drag Manager is available
	if ((Gestalt(gestaltDragMgrAttr, &response) == noErr) && BTST(response, gestaltDragMgrPresent))
	{
#if GENERATINGCFM
		if ((UInt32) NewDrag != kUnresolvedCFragSymbolAddress)
#endif
			BSET(pWE->flags, weFHasDragManager);

#if WASTE_TRANSLUCENT_DRAGS
		// determine whether translucent drags are available
		if (BTST(response, 3))
		{
#if GENERATINGCFM
			if ((UInt32) SetDragImage != kUnresolvedCFragSymbolAddress)
#endif
				BSET(pWE->flags, weFHasTranslucentDrags);
		}
#endif	// WASTE_TRANSLUCENT_DRAGS
	}
#endif	// WASTE_DRAG_AND_DROP

#if WASTE_TSM_SUPPORT
	// determine whether the Text Services manager is available
	if (Gestalt(gestaltTSMgrVersion, &response) == noErr)
	{
		BSET(pWE->flags, weFHasTextServices);
	}
#endif

	// determine if there are any non-Roman scripts enabled
	if (GetScriptManagerVariable(smEnabled) > 1)
	{
		BSET(pWE->flags, weFNonRoman);

		// determine whether a double-byte script is installed
		if (GetScriptManagerVariable(smDoubleByte) != 0)
		{
#if GENERATING68K
			BSET(pWE->flags, weFDoubleByte);	// the WorldScript Power Adapter breaks this :-(
#else
			ScriptCode script;
			for ( script = smRoman; script <= smKlingon; script++ )
			{
				if (GetScriptVariable(script, smEnabled) &&
				! BTST(GetScriptVariable(script, smScriptFlags), smsfSingByte))
				{
					BSET(pWE->flags, weFDoubleByte);
					break;
				}
			}
#endif
		}

		// determine whether a bidirectional script is installed
		if (GetScriptManagerVariable(smBidirect) != 0)
		{
			BSET(pWE->flags, weFBidirectional);
		}
	}

	// initialize miscellaneous fields of the WE record
	pWE->nLines = 1;
	pWE->nStyles = 1;
	pWE->nRuns = 1;
	pWE->viewRect = *viewRect;
	pWE->destRect = *destRect;
	pWE->direction = weDirDefault;
	pWE->features = features;
#if WASTE_TSM_SUPPORT
	pWE->tsmAreaStart = kInvalidOffset;
	pWE->tsmAreaEnd = kInvalidOffset;
#endif
#if WASTE_DRAG_AND_DROP
	pWE->dragCaretOffset = kInvalidOffset;
#endif

	// initialize hook fields with the addresses of the standard hooks
	_WESetStandardHooks(*hWE);

	// create a region to hold the view rectangle
	pWE->viewRgn = NewRgn();
	WELongRectToRect(viewRect, &r);
	RectRgn(pWE->viewRgn, &r);

	// initialize the style run array
	(*pWE->hRuns)[1].runStart = 1;
	(*pWE->hRuns)[1].styleIndex = -1;

	// initialize the style table
	(*pWE->hStyles)[0].refCount = 1;

	// copy text attributes from the active graphics port
	BLOCK_CLR(attributes);
	attributes.runStyle.tsFont = pWE->port->txFont;
	attributes.runStyle.tsSize = pWE->port->txSize;
	attributes.runStyle.tsFace = pWE->port->txFace;
	if (BTST(pWE->flags, weFHasColorQD))
	{
		GetForeColor(&attributes.runStyle.tsColor);
//ShadowIRC Modification: restore background color
		GetBackColor(&attributes.runStyle.tsBackColor);
	}
	attributes.runStyle.extraFlags = 0;
	
	_WEFillFontInfo(pWE->port, &attributes);
	(*pWE->hStyles)[0].info = attributes;

	// initialize the line array
	if ((err = WECalText(*hWE)) != noErr)
	{
		goto cleanup;
	}

#if WASTE_TSM_SUPPORT
	// register with the Text Services Manager
	if ((err = _WERegisterWithTSM(*hWE)) != noErr)
	{
		goto cleanup;
	}
#endif

	// unlock the WE record
	HUnlock((Handle)*hWE);

	// skip clean-up section
	return noErr;

cleanup:
	// clean up
	if (pWE != nil)
	{
		_WEForgetHandle((Handle *) &pWE->hText);
		_WEForgetHandle((Handle *) &pWE->hLines);
		_WEForgetHandle((Handle *) &pWE->hStyles);
		_WEForgetHandle((Handle *) &pWE->hRuns);
		_WEForgetHandle(&pWE->hUserInfo);
		if (pWE->viewRgn != nil)
		{
			DisposeRgn(pWE->viewRgn);
		}
	}
	_WEForgetHandle((Handle *)hWE);

	return err;
}

pascal void _WEResetStyleTable(WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	SInt32 index;
	WEStyleTableEntry *pTable;
	Boolean saveTableLock;

	// sanity check
	if (pWE->hStyles == nil)
	{
		return;
	}

	// lock the style table
	saveTableLock = _WESetHandleLock((Handle) pWE->hStyles, true);
	pTable = *pWE->hStyles;

	// walk the style table, disposing of all embedded objects referenced there
	index = pWE->nStyles;
	while ( --index >= 0 )
	{

#if WASTE_OBJECTS
		if ((pTable->refCount > 0) && (pTable->info.runStyle.tsObject != nil))
		{
			_WEFreeObject(pTable->info.runStyle.tsObject);
		}
#endif

		pTable->refCount = 0;
		pTable++;
	};

	// unlock the style table
	_WESetHandleLock((Handle) pWE->hStyles, saveTableLock);
}

pascal void WEDispose(WEHandle hWE)
{
	WEPtr pWE;

	// sanity check: make sure WE isn't nil
	if (hWE == nil)
	{
		return;
	}

	// lock the WE record
	HLock((Handle) hWE);
	pWE = *hWE;

	// clear the Undo buffer
	_WEForgetAction(&pWE->hActionStack);

#if WASTE_TSM_SUPPORT
	// unregister with the Text Services Manager
	if (pWE->tsmReference != nil)
	{
		DeleteTSMDocument(pWE->tsmReference);
		pWE->tsmReference = nil;
	}
#endif

	// dispose of the offscreen graphics world
	if (pWE->offscreenPort != nil)
	{
		DisposeGWorld((GWorldPtr)pWE->offscreenPort);
		pWE->offscreenPort = nil;
	}

#if WASTE_OBJECTS
	// release all embedded objects we know about
	_WEResetStyleTable(hWE);

	// dispose instance-specific object handler table
	_WEForgetHandle((Handle *) &pWE->hObjectHandlerTable);
#endif

	// dispose of auxiliary data structures
	_WEForgetHandle((Handle *) &pWE->hText);
	_WEForgetHandle((Handle *) &pWE->hLines);
	_WEForgetHandle((Handle *) &pWE->hStyles);
	_WEForgetHandle((Handle *) &pWE->hRuns);
	_WEForgetHandle(&pWE->hUserInfo);
	DisposeRgn(pWE->viewRgn);

	// dispose of the WE record
	DisposeHandle((Handle) hWE);
}

pascal OSErr WEUseText(Handle text, WEHandle hWE)
{
	WEPtr pWE;
	WERunArrayEntry *pRuns;
	WELineRec *pLines;
	SInt32 textLength;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// stop any ongoing inline session
	WEStopInlineSession(hWE);

	// reset modification count and clear undo buffer, if any
	WEResetModCount(hWE);

	// reset the style table, disposing of all embedded objects it references
	_WEResetStyleTable(hWE);

	// install the text
	_WEForgetHandle(&pWE->hText);
	pWE->hText = text;
	textLength = GetHandleSize(text);
	pWE->textLength = textLength;

	// reset the run array
	SetHandleSize((Handle) pWE->hRuns, 2 * sizeof(WERunArrayEntry));
	pWE->nRuns = 1;
	pRuns = *pWE->hRuns;
	pRuns[1].runStart = textLength + 1;
	pRuns[1].styleIndex = -1;

	// fix the refCount of the style table entry referenced by the only run
	(*pWE->hStyles)[pRuns[0].styleIndex].refCount = 1;

	// reset the line array
	SetHandleSize((Handle) pWE->hLines, 2 * sizeof(WELineRec));
	pWE->nLines = 1;
	pLines = *pWE->hLines;
	_WEBlockClr(pLines, 2 * sizeof(WELineRec));
	pLines[1].lineStart = textLength;

	// reset several fields of the WE record
	pWE->selStart = 0;
	pWE->selEnd = 0;
	pWE->firstByte = 0;
	pWE->clickCount = 0;
#if WASTE_TSM_SUPPORT
	pWE->tsmAreaStart = kInvalidOffset;
	pWE->tsmAreaEnd = kInvalidOffset;
#endif
#if WASTE_DRAG_AND_DROP
	pWE->dragCaretOffset = kInvalidOffset;
#endif

	// recalculate and redraw everything
	// err = _WERedraw(0, LONG_MAX, hWE);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	return noErr;
}
