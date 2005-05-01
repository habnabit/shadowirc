/*
 *	WEInlineInput.c
 *
 *	WASTE PROJECT
 *  Inline Input Support
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */

#include <AvailabilityMacros.h>
#include "WASTEIntf.h"

// special event ID used by Kotoeri and other input methods

enum
{
	kGetText = 'gtxt'
};

#if WASTE_TSM_SUPPORT

// static variables

static AEEventHandlerUPP _weUpdateActiveInputAreaHandler = nil;
static AEEventHandlerUPP _wePositionToOffsetHandler = nil;
static AEEventHandlerUPP _weOffsetToPositionHandler = nil;
static AEEventHandlerUPP _weGetTextHandler = nil;

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_3
#define kTSMHiliteCaretPosition kCaretPosition
#define kTSMHiliteRawText kRawText
#endif

pascal OSErr _WEHiliteRangeArray(TextRangeArrayHandle hTray, WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	TextRangePtr pRange;
	SInt32 rangeStart, rangeEnd;
	SInt16 hiliteStyle;
	WETextStyle ts;
	SInt16 rangeIndex;
	Boolean saveTrayLock;
	OSErr err;

	// lock down the range array
	saveTrayLock = _WESetHandleLock((Handle)hTray, true);
	pRange = (*hTray)->fRange;

	// walk the hilite range array
	for (rangeIndex = (*hTray)->fNumOfRanges - 1; rangeIndex >= 0; rangeIndex-- )
	{

		// the offsets in the range array are relative to the beginning
		// of the active input area: convert them to absolute offsets
		rangeStart = pWE->tsmAreaStart + pRange->fStart;
		rangeEnd = pWE->tsmAreaStart + pRange->fEnd;
		hiliteStyle = pRange->fHiliteStyle;

		// take the absolute value of hiliteStyle
		hiliteStyle = ABS(hiliteStyle);

		// if hiliteStyle is kCaretPosition, set the selection range
		if (hiliteStyle == kTSMHiliteCaretPosition)
		{
			pWE->selStart = rangeStart;
			pWE->selEnd = rangeEnd;
		}
		else
		{
			hiliteStyle -= kTSMHiliteRawText;
			// otherwise set the WETextStyle flags of the specified range appropriately
			if ((hiliteStyle >= 0) && (hiliteStyle <= 3))
			{
				ts.tsFlags = 0x10 + (hiliteStyle << tsTSMSelected);
				if ((err = _WESetStyleRange(rangeStart, rangeEnd, weDoFlags, &ts, hWE)) != noErr)
				{
					goto cleanup;
				}
			}
		}
		// go to next text range element
		pRange++;
	}

	// clear result code
	err = noErr;

cleanup:
	// unlock the range array
	_WESetHandleLock((Handle)hTray, saveTrayLock);

	return err;
}

static pascal OSErr _WEHandleUpdateActiveInputArea(const AppleEvent *ae, AppleEvent *reply, SInt32 handlerRefCon)
{
#pragma unused(reply, handlerRefCon)
	WEHandle hWE = nil;
	WEPtr pWE;
	AEDesc text;
	AEDesc hiliteTray;
	TextRange pinRange;
	SInt32 totalLength;
	SInt32 fixLength;
	SInt32 tsmOffset;
	DescType actualType;
	SInt32 actualSize;
	GrafPtr savePort;
	WEActionHandle hAction;
	Boolean saveAutoScroll;
	Boolean saveTextLock;
	Boolean saveWELock = false;
	OSErr err;
	Handle hDescData = nil;

	// initialize descriptors to null values
	text.descriptorType = typeNull;
	text.dataHandle = nil;
	hiliteTray.descriptorType = typeNull;
	hiliteTray.dataHandle = nil;

	// extract WE handle
	if ((err = AEGetParamPtr(ae, keyAETSMDocumentRefcon, typeLongInteger,
			&actualType, &hWE, sizeof(hWE), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	// lock the WE handle
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// call the pre-update callback, if present
	if (pWE->tsmPreUpdate != nil)
	{
		CallWETSMPreUpdateProc(hWE, pWE->tsmPreUpdate);
	}

	// hide the caret if it's showing
	if (BTST(pWE->flags, weFCaretVisible))
	{
		_WEBlinkCaret(hWE);
	}

	// extract the text descriptor
	if ((err = AEGetParamDesc(ae, keyAETheData, typeChar, &text)) != noErr)
	{
		goto cleanup;
	}

	// get total length of text in the active input area
	totalLength = AEGetDescDataSize(&text);

	// extract the length of confirmed text in the active input area
	if ((err = AEGetParamPtr(ae, keyAEFixLength, typeLongInteger, &actualType,
			&fixLength, sizeof(fixLength), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	// if fixLength = -1, all text is confirmed
	if (fixLength == -1)
	{
		fixLength = totalLength;
	}

	// if there's currently no active input area, open one
	if (pWE->tsmAreaStart == kInvalidOffset)
	{
		pWE->tsmAreaStart = pWE->selStart;
		pWE->tsmAreaEnd = pWE->selEnd;

		// are we tracking a typing sequence?
		if (!WEIsTyping(hWE))
		{
			// nope; so start a new one
			// increment modification count
			pWE->modCount++;

			// if undo support is enabled, the inline session just started may initiate
			if (BTST(pWE->features, weFUndoSupport))
			{
				WEClearUndo(hWE);
				if (_WENewAction(pWE->selStart, pWE->selEnd, 0, weAKTyping, 0, hWE, &hAction) == noErr)
				{
					_WEPushAction(hAction);
				}
			}
		}
	}

	tsmOffset = pWE->tsmAreaStart;

	// the new text replaces whatever is in the active input area
	if ((err = _WEDeleteRange(tsmOffset, pWE->tsmAreaEnd, hWE)) != noErr)
	{
		goto cleanup;
	}

	// synchronize the null style, so font script matches the keyboard script
	_WESynchNullStyle(hWE);

	// set the port font for good measure
	GetPort(&savePort);
	SetPort(pWE->port);
	TextFont(pWE->nullStyle.runStyle.tsFont);
	SetPort(savePort);

	// insert the text
	if ((err = _WEAllocate(totalLength, kAllocTemp, &hDescData)) != noErr)
	{
		goto cleanup;
	}
	saveTextLock = _WESetHandleLock(hDescData, true);
	if ((err = AEGetDescData(&text, *(hDescData), totalLength)) != noErr)
	{
		_WEForgetHandle(&hDescData);
		goto cleanup;
	}
	err = _WEInsertText(tsmOffset, *(hDescData), totalLength, hWE);
	_WEForgetHandle(&hDescData);
	if (err != noErr)
	{
		goto cleanup;
	}

	// extract pin range
	if ((err = AEGetParamPtr(ae, keyAEPinRange, typeTextRange, &actualType,
			&pinRange, sizeof(pinRange), &actualSize)) == noErr)
	{
		// we want absolute offsets
		pinRange.fStart += tsmOffset;
		pinRange.fEnd += tsmOffset;
	}
	else
	{
		// a missing pin range descriptor isn't an error; everything else is
		if (err != errAEDescNotFound)
		{
			goto cleanup;
		}

		// default pin range is active input area
		pinRange.fStart = tsmOffset;
		pinRange.fEnd = pWE->tsmAreaEnd;
	}

	// NOTE: if fixLength == totalLength, the inline input session is over, so, in theory,
	// hiliteTray should either be missing or not specify any range to be underlined.
	// Unfortunately, some input methods (like Apple's input method for Simplified Chinese)
	// do specify kConvertedText (= thin black underline) for the whole text when
	// the text is confirmed (is this a bug?).  To work around this, we deliberately ignore
	// the hiliteTray parameter when fixLength = totalLength.

	if (fixLength != totalLength)
	{
		pWE->tsmAreaStart += fixLength;  // added by Kiyoshi Gomasaki for ATOK compatibility

		// extract the highlight range array
		if ((err = AEGetParamDesc(ae, keyAEHiliteRange, typeTextRangeArray, &hiliteTray)) != noErr)
		{
			if (err != errAEDescNotFound)
			{
				goto cleanup;
			}
		}
	}

	if (hiliteTray.dataHandle != nil)
	{
		if ((err = _WEHiliteRangeArray((TextRangeArrayHandle) hiliteTray.dataHandle, hWE)) != noErr)
		{
			goto cleanup;
		}
	}
	else
	{
		pWE->selStart = tsmOffset + fixLength;
		pWE->selEnd = pWE->selStart;
	}

	// temporarily disable auto-scroll, as we need to scroll manually according to pinRange
	saveAutoScroll = BTST(pWE->features, weFAutoScroll) ? true : false;
	BCLR(pWE->features, weFAutoScroll);

	// redraw the active input area
	if ((err = _WERedraw(tsmOffset, tsmOffset + totalLength, hWE)) != noErr)
	{
		goto cleanup;
	}

	if (saveAutoScroll)
	{

		// re-enable auto-scroll
		BSET(pWE->features, weFAutoScroll);

		// scroll the pin range into view
		if (!_WEScrollIntoView(pinRange.fStart, hWE))
		{
			if (pinRange.fStart != pinRange.fEnd)
			{
				_WEScrollIntoView(pinRange.fEnd, hWE);
			}
		}
	}

	// update the boundaries of the active input area
	// if fixLength = totalLength, the inline input session is over: close the active input area
	if (fixLength == totalLength)
	{
		pWE->tsmAreaStart = kInvalidOffset;
		pWE->tsmAreaEnd = kInvalidOffset;

		// adjust undo buffer (if any) for the confirmed text
		_WEAdjustUndoRange(fixLength, hWE);
	}
	else
	{
		// otherwise, fixLength defines the boundaries of the active input area
		pWE->tsmAreaStart = tsmOffset + fixLength;
		pWE->tsmAreaEnd = tsmOffset + totalLength;
	}

	// call the post-update callback, if present
	if (pWE->tsmPostUpdate != nil)
	{
		CallWETSMPostUpdateProc(hWE, fixLength, pWE->tsmAreaStart, pWE->tsmAreaEnd,
			pinRange.fStart, pinRange.fEnd, pWE->tsmPostUpdate);
	}

	// clear result code
	err = noErr;

cleanup:
	// clean up
	AEDisposeDesc(&text);
	AEDisposeDesc(&hiliteTray);

	// unlock the WE record
	if (hWE !=nil)
	{
		_WESetHandleLock((Handle) hWE, saveWELock);
	}

	// return result code
	return err;
}

static pascal OSErr _WEHandlePositionToOffset(const AppleEvent *ae, AppleEvent *reply, SInt32 handlerRefCon)
{
#pragma unused(handlerRefCon)
	WEHandle hWE = nil;
	WEPtr pWE;
	Point position;
	LongPt thePoint;
	SInt16 regionClass;
	SInt32 offset;
	DescType actualType;
	SInt32 actualSize;
	GrafPtr savePort;
	WEEdge edge;
	Boolean saveWELock = false;
	OSErr err;

	// extract WE handle
	if ((err = AEGetParamPtr(ae, keyAETSMDocumentRefcon, typeLongInteger,
			&actualType, &hWE, sizeof(hWE), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// extract position parameter
	if ((err = AEGetParamPtr(ae, keyAECurrentPoint, typeQDPoint, &actualType, &position, sizeof(position), &actualSize)) != noErr)
	{
			goto cleanup;
	}

	// convert position to local...
	GetPort(&savePort);
	SetPort(pWE->port);
	GlobalToLocal(&position);
	SetPort(savePort);

	// ...and long coordinates
	WEPointToLongPoint(position, &thePoint);

	// find the byte offset and the edge value corresponding to the given position
	offset = WEGetOffset(&thePoint, &edge, hWE);

	// determine the region class
	if (WELongPointInLongRect(&thePoint, &pWE->viewRect))
	{
		if (_WEOffsetInRange(offset, edge, pWE->tsmAreaStart, pWE->tsmAreaEnd))
		{
  			regionClass = kTSMInsideOfActiveInputArea;
			// if the given position is within the active input area, we're supposed to return
			// an offset relative to the beginning of this area (thanks, Martin!)
        	offset -= pWE->tsmAreaStart;
		}
		else
		{
			// otherwise the offset is relative to the beginning of the body
			regionClass = kTSMInsideOfBody;
		}
	}
	else
	{
		regionClass = kTSMOutsideOfBody;
	}

	// add region class parameter to reply
	if ((err = AEPutParamPtr(reply, keyAERegionClass, typeShortInteger, &regionClass, sizeof(regionClass))) != noErr)
	{
		goto cleanup;
	}

	// add offset parameter to reply
	if ((err = AEPutParamPtr(reply, keyAEOffset, typeLongInteger, &offset, sizeof(offset))) != noErr)
	{
		goto cleanup;
	}

	// add edge parameter to reply
	if ((err = AEPutParamPtr(reply, keyAELeftSide, typeBoolean, &edge, sizeof(edge))) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;

cleanup:
	// unlock the WE record
	if (hWE != nil)
	{
		_WESetHandleLock((Handle) hWE, saveWELock);
	}

	// return result code
	return err;
}

static pascal OSErr _WEHandleOffsetToPosition(const AppleEvent *ae, AppleEvent *reply, SInt32 handlerRefCon)
{
#pragma unused(handlerRefCon)
	WEHandle hWE = nil;
	WEPtr pWE;
	SInt32 offset;
	LongPt thePoint;
	Point position;
	SInt16 lineHeight;
	DescType actualType;
	SInt32 actualSize;
	GrafPtr savePort;
	Boolean saveWELock = false;
	OSErr err;

	// extract WE handle
	if ((err = AEGetParamPtr(ae, keyAETSMDocumentRefcon, typeLongInteger,
			&actualType, &hWE, sizeof(hWE), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// if there's no active input area, return errOffsetInvalid
	err = errOffsetInvalid;
	if (pWE->tsmAreaStart < 0)
	{
		goto cleanup;
	}

	// extract the offset parameter
	if ((err = AEGetParamPtr(ae, keyAEOffset, typeLongInteger, &actualType, &offset, sizeof(offset), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	// offset is relative to the beginning of the active input area; we want an absolute offset
	offset += pWE->tsmAreaStart;

	// make sure the offset is within the input area
	if ((offset < pWE->tsmAreaStart) || (offset >= pWE->tsmAreaEnd))
	{
		err = errOffsetInvalid;
		goto cleanup;
	}

	// find the position corresponding to the given offset (in long coordinates)
	WEGetPoint(offset, hilite, &thePoint, &lineHeight, hWE);
	thePoint.v += lineHeight;

	// make sure offset is within view rectangle
	if ((thePoint.h < pWE->viewRect.left) ||
		(thePoint.h > pWE->viewRect.right ) ||
		(thePoint.v < pWE->viewRect.top) ||
		(thePoint.v > pWE->viewRect.bottom))
	{
		err = errOffsetIsOutsideOfView;
		goto cleanup;
	}

	// convert the point to short...
	WELongPointToPoint(&thePoint, &position);

	// ...and global coordinates
	GetPort(&savePort);
	SetPort(pWE->port);
	LocalToGlobal(&position);
	SetPort(savePort);

	// add keyAEPoint parameter to the reply Apple event
	if ((err = AEPutParamPtr(reply, keyAEPoint, typeQDPoint,
			&position, sizeof(position))) != noErr)
	{
		goto cleanup;
	}

	// add keyAETSMTextFont parameter to the reply Apple event
	if ((err = AEPutParamPtr(reply, keyAETSMTextFont, typeShortInteger,
			&pWE->nullStyle.runStyle.tsFont, sizeof(pWE->nullStyle.runStyle.tsFont))) != noErr)
	{
		goto cleanup;
	}

	// add keyAETSMTextPointSize parameter to the reply Apple event
	if ((err = AEPutParamPtr(reply, keyAETSMTextPointSize, typeShortInteger,
			&pWE->nullStyle.runStyle.tsSize, sizeof(pWE->nullStyle.runStyle.tsSize))) != noErr)
	{
		goto cleanup;
	}

	// add keyAETextLineAscent parameter to the reply Apple event
	if ((err = AEPutParamPtr(reply, keyAETextLineAscent, typeShortInteger,
			&pWE->nullStyle.runAscent, sizeof(pWE->nullStyle.runAscent))) != noErr)
	{
		goto cleanup;
	}

	// add keyAETextLineHeight parameter to the reply Apple event
	if ((err = AEPutParamPtr(reply, keyAETextLineHeight, typeShortInteger,
			&pWE->nullStyle.runHeight, sizeof(pWE->nullStyle.runHeight))) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;

cleanup:
	// unlock the WE record
	if (hWE != nil)
	{
		_WESetHandleLock((Handle) hWE, saveWELock);
	}

	// return result code
	return err;
}

static pascal OSErr _WEHandleGetText(const AppleEvent *ae, AppleEvent *reply, SInt32 handlerRefCon)
{
#pragma unused(handlerRefCon)

	WEHandle hWE = nil;
	WEPtr pWE;
	DescType actualType;
	SInt32 actualSize;
	Boolean saveWELock = false;
	Boolean saveTextLock;
	OSErr err;

	// extract WE handle
	if ((err = AEGetParamPtr(ae, keyAETSMDocumentRefcon, typeLongInteger,
			&actualType, &hWE, sizeof(hWE), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return a copy of the selected text in the reply event
	saveTextLock = _WESetHandleLock(pWE->hText, true);
	err = AEPutParamPtr(reply, keyAETheData, typeText,
			*pWE->hText + pWE->selStart, pWE->selEnd - pWE->selStart);
	_WESetHandleLock(pWE->hText, saveTextLock);
	if (err != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;

cleanup:
	// unlock the WE record
	if (hWE != nil)
	{
		_WESetHandleLock((Handle) hWE, saveWELock);
	}

	// return result code
	return err;
}

#endif	// WASTE_TSM_SUPPORT

pascal OSErr WEInstallTSMHandlers(void)
{
#if WASTE_TSM_SUPPORT
	OSErr err;

	// the first time we're called, create routine descriptors for our Apple event handlers
	if (_weUpdateActiveInputAreaHandler == nil)
	{
		_weUpdateActiveInputAreaHandler = NewAEEventHandlerUPP((AEEventHandlerProcPtr) _WEHandleUpdateActiveInputArea);
		_wePositionToOffsetHandler = NewAEEventHandlerUPP((AEEventHandlerProcPtr) _WEHandlePositionToOffset);
		_weOffsetToPositionHandler = NewAEEventHandlerUPP((AEEventHandlerProcPtr) _WEHandleOffsetToPosition);
		_weGetTextHandler = NewAEEventHandlerUPP((AEEventHandlerProcPtr) _WEHandleGetText);
	}

	// install Apple Event handlers to be used by Text Service components
	if ((err = AEInstallEventHandler(kTextServiceClass, kUpdateActiveInputArea, _weUpdateActiveInputAreaHandler, 0, false)) != noErr)
	{
		goto cleanup;
	}

	if ((err = AEInstallEventHandler(kTextServiceClass, kPos2Offset, _wePositionToOffsetHandler, 0, false)) != noErr)
	{
		goto cleanup;
	}

	if ((err = AEInstallEventHandler(kTextServiceClass, kOffset2Pos, _weOffsetToPositionHandler, 0, false)) != noErr)
	{
		goto cleanup;
	}

	if ((err = AEInstallEventHandler(kTextServiceClass, kGetText, _weGetTextHandler, 0, false)) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;

cleanup:
	// return result code
	return err;
#else
	return noErr ;
#endif	// WASTE_TSM_SUPPORT
}

pascal OSErr WERemoveTSMHandlers(void)
{
#if WASTE_TSM_SUPPORT
	OSErr err;

	// return an error code if WEInstallTSMHandlers has never been called
	if (_weUpdateActiveInputAreaHandler == nil)
	{
		return errAEHandlerNotFound;
	}

	// remove the handlers
	if ((err = AERemoveEventHandler(kTextServiceClass, kUpdateActiveInputArea, _weUpdateActiveInputAreaHandler, false)) != noErr)
	{
		goto cleanup;
	}

	if ((err = AERemoveEventHandler(kTextServiceClass, kPos2Offset, _wePositionToOffsetHandler, false)) != noErr)
	{
		goto cleanup;
	}

	if ((err = AERemoveEventHandler(kTextServiceClass, kOffset2Pos, _weOffsetToPositionHandler, false)) != noErr)
	{
		goto cleanup;
	}

	if ((err = AERemoveEventHandler(kTextServiceClass, kGetText, _weGetTextHandler, false)) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;

cleanup:
	// clear result code
	return err;
#else
	return noErr ;
#endif	// WASTE_TSM_SUPPORT
}

pascal OSErr WEHandleTSMEvent(const AppleEvent *ae, AppleEvent *reply)
{
#if WASTE_TSM_SUPPORT
	AEEventClass eventClass;
	AEEventID eventID;
	DescType actualType;
	Size actualSize;
	OSErr err;

	//	get event class
	if ((err = AEGetAttributePtr(ae, keyEventClassAttr, typeType, &actualType, &eventClass, sizeof(eventClass), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	//	make sure the event class is kTextServiceClass
	err = errAEEventNotHandled;
	if (eventClass != kTextServiceClass)
	{
		goto cleanup;
	}

	//	get event ID
	if ((err = AEGetAttributePtr(ae, keyEventIDAttr, typeType, &actualType, &eventID, sizeof(eventID), &actualSize)) != noErr)
	{
		goto cleanup;
	}

	//	dispatch to appropriate handler
	switch (eventID)
	{
		case kUpdateActiveInputArea:
		{
			err = _WEHandleUpdateActiveInputArea(ae, reply, 0);
			break;
		}

		case kPos2Offset:
		{
			err = _WEHandlePositionToOffset(ae, reply, 0);
			break;
		}

		case kOffset2Pos:
		{
			err = _WEHandleOffsetToPosition(ae, reply, 0);
			break;
		}

		case kGetText:
		{
			err = _WEHandleGetText(ae, reply, 0);
			break;
		}

		default:
		{
			break;
		}
	}

cleanup:
	return err;
#else
	#pragma unused(ae, reply)
	return errAEEventNotHandled ;
#endif
}

pascal void WEStopInlineSession(WEHandle hWE)
{
#if WASTE_TSM_SUPPORT
	WEPtr pWE = *hWE;

	// call FixTSMDocument() only if the inline input area is actually "open"
	if ((pWE->tsmAreaStart != kInvalidOffset) && (pWE->tsmReference != nil))
	{
		FixTSMDocument(pWE->tsmReference);
	}
#else
#pragma unused(hWE)
#endif
}
