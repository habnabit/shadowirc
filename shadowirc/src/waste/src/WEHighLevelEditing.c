/*
 *	WEHighLevelEditing.c
 *
 *	WASTE PROJECT
 *  High-Level Editing Routines
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

pascal void _WEPushAction(WEActionHandle hAction)
{
	WEPtr pWE = *((*hAction)->hOwner);
	WEActionHandle hLast;

	// find the last action in the given stack
	for ( hLast = hAction; (*hLast)->hNext != nil; hLast = (*hLast)->hNext )
		;

	// prepend hAction in front of the action stack
	(*hLast)->hNext = pWE->hActionStack;
	pWE->hActionStack = hAction;
}

pascal OSErr _WENewAction(SInt32 rangeStart, SInt32 rangeEnd, SInt32 newTextLength,
							WEActionKind actionKind, WEActionFlags actionFlags,
							WEHandle hWE, WEActionHandle *hAction)
{
	WEAction * pAction = nil;
	OSErr err;

	// allocate a new action record
	if ((err = _WEAllocate(sizeof(WEAction), kAllocClear, (Handle *)hAction)) != noErr)
	{
		goto cleanup;
	}

	// lock it down
	HLock((Handle) *hAction);
	pAction = **hAction;

	// fill in the fields
	pAction->hOwner = hWE;
	pAction->delRangeStart = rangeStart;
	pAction->delRangeLength = newTextLength;
	pAction->insRangeLength = rangeEnd - rangeStart;
	pAction->actionKind = actionKind;
	pAction->actionFlags = actionFlags;

	// remember selection range
	WEGetSelection(&pAction->hiliteStart, &pAction->hiliteEnd, hWE);

	// allocate a handle to hold the text to be saved, unless otherwise specified
	if ((actionFlags & weAFDontSaveText) == 0)
	{
		if ((err = _WEAllocate(0, kAllocTemp, &pAction->hText)) != noErr)
		{
			goto cleanup;
		}
	}

	// allocate a handle to hold the styles to be saved, unless otherwise specified
	if ((actionFlags & weAFDontSaveStyles) == 0)
	{
		if ((err = _WEAllocate(0, kAllocTemp, &pAction->hStyles)) != noErr)
		{
			goto cleanup;
		}
	}

#if WASTE_OBJECTS
	// allocate a handle to hold the "soup" to be saved, unless otherwise specified
	if ((actionFlags & weAFDontSaveSoup) == 0)
	{
		if ((err = _WEAllocate(0, kAllocTemp, &pAction->hSoup)) != noErr)
		{
			goto cleanup;
		}
	}
#endif

	// make a copy of text range
	if ((err = WECopyRange(rangeStart, rangeEnd, pAction->hText, (Handle) pAction->hStyles, pAction->hSoup, hWE)) != noErr)
	{
		goto cleanup;
	}

	// unlock action record
	HUnlock((Handle) *hAction);

cleanup:
	// clean up if an error occurred
	if (err != noErr)
	{
		if (pAction != nil)
		{
			_WEForgetHandle(&pAction->hText);
			_WEForgetHandle(&pAction->hStyles);
#if WASTE_OBJECTS
			_WEForgetHandle(&pAction->hSoup);
#endif
		}
		_WEForgetHandle((Handle *)hAction);
	}

	// return result code
	return err;
}

pascal void _WEDisposeAction(WEActionHandle hAction)
{
	WEAction *pAction;
	WEActionHandle hNext;

	for ( ; hAction != nil; hAction = hNext )
	{
		// lock the action record
		HLock((Handle) hAction);
		pAction = *hAction;
		hNext = pAction->hNext;

		// throw away text, styles and soup
		_WEForgetHandle(&pAction->hText);
		_WEForgetHandle(&pAction->hStyles);
#if WASTE_OBJECTS
		_WEForgetHandle(&pAction->hSoup);
#endif

		// throw away the action record itself
		DisposeHandle((Handle) hAction);
	}
}

pascal void _WEForgetAction(WEActionHandle *hAction)
{
	WEActionHandle theAction;

	theAction = *hAction;
	if (theAction != nil)
	{
		*hAction = nil;
		_WEDisposeAction(theAction);
	}
}

pascal OSErr _WEDoAction(WEActionHandle hAction)
{
	WEActionHandle hRedoAction;
	WEAction *pAction = nil;
	WEHandle hWE;
	WEPtr pWE;
	SInt32 offset, delOffset, insOffset;
	SInt32 redrawStart, redrawEnd;
	Boolean saveActionLock, saveWELock, saveTextLock;
	OSErr err;

	// sanity check: make sure hAction isn't nil
	if (hAction == nil)
	{
		return nilHandleErr;
	}

	// get handle to associated WE instance
	hWE = (*hAction)->hOwner;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	// hide selection highlighting and the caret
	_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);
	if (BTST(pWE->flags, weFCaretVisible))
	{
		_WEBlinkCaret(hWE);
	}

	redrawStart = LONG_MAX;
	redrawEnd = 0;

	for ( ; hAction != nil; hAction = (*hAction)->hNext )
	{
		// lock the action record
		saveActionLock = _WESetHandleLock((Handle) hAction, true);
		pAction = *hAction;
		offset = pAction->delRangeStart;
		delOffset = offset + pAction->delRangeLength;
		insOffset = offset + pAction->insRangeLength;

		// update the modification count of the affected WE instance
		// note that undoing a change _decrements_ the modification count,
		// while redoing the change increments it again
		if (pAction->actionFlags & weAFIsRedo)
		{
			pWE->modCount++;
		}
		else
		{
			pWE->modCount--;
		}

		// if undo support is enabled, save the range to be affected by this action
		if (BTST(pWE->features, weFUndoSupport))
		{
			if (_WENewAction(offset, delOffset, pAction->insRangeLength, pAction->actionKind,
				(pAction->actionFlags ^ weAFIsRedo), hWE, &hRedoAction) == noErr)
			{
				_WEPushAction(hRedoAction);
			}
		}

		if (pAction->hText != nil)
		{
			// delete the range to replace
			if ((err = _WEDeleteRange(offset, delOffset, hWE)) != noErr)
			{
				goto cleanup;
			}

			// insert the saved text
			saveTextLock = _WESetHandleLock(pAction->hText, true);
			err = _WEInsertText(offset, *pAction->hText, pAction->insRangeLength, hWE);
			_WESetHandleLock(pAction->hText, saveTextLock);
			if (err != noErr)
			{
				goto cleanup;
			}
		}

		// apply the saved styles, if any
		if (pAction->hStyles != nil)
		{
			if ((err = _WEApplyStyleScrap(offset, insOffset, (myStScrpHandle) pAction->hStyles, hWE)) != noErr)
			{
				goto cleanup;
			}
		}

#if WASTE_OBJECTS
		// the same goes for the soup
		if (pAction->hSoup != nil)
		{
			if ((err = _WEApplySoup(offset, pAction->hSoup, hWE)) != noErr)
			{
				goto cleanup;
			}
		}
#endif

		// adjust redraw range (??? will this work with "complex" action stacks ???)
		if (offset < redrawStart)
		{
			redrawStart = offset;
		}
		if (insOffset > redrawEnd)
		{
			redrawEnd = insOffset;
		}

		// unlock action record
		_WESetHandleLock((Handle) hAction, saveActionLock);

	} // for

	// restore the original selection range
	pWE->selStart = pAction->hiliteStart;
	pWE->selEnd = pAction->hiliteEnd;
/*
	// redraw the text
	if ((err = _WERedraw(redrawStart, redrawEnd, hWE)) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;
*/
	err = _WERedraw(redrawStart, redrawEnd, hWE);
	
cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

pascal OSErr WEUndo(WEHandle hWE)
{
	WEPtr pWE;
	WEActionHandle hAction;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// "detach" the action stack from the WE instance
	hAction = pWE->hActionStack;
	pWE->hActionStack = nil;

	if (hAction != nil)
	{
		// perform the action...
		err = _WEDoAction(hAction);

		// ...and throw it away
		_WEDisposeAction(hAction);
	}
	else
	{
		// return an error code if the undo buffer is empty
		err = weCantUndoErr;
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	return err;
}

pascal void WEClearUndo(WEHandle hWE)
{
	WEPtr pWE = *hWE;

	// dispose of the action chain associated with the given WE instance,
	// unless weFAccumulateUndos is set
	if (! BTST(pWE->flags, weFAccumulateUndos))
	{
		_WEForgetAction(&pWE->hActionStack);
	}
}

pascal WEActionKind WEGetUndoInfo(Boolean *redoFlag, WEHandle hWE)
{
	WEActionHandle hAction;
	WEActionKind theKind = weAKNone;
	Boolean theFlag = false;

	if ((hAction = (*hWE)->hActionStack) != nil)
	{
		theKind = (*hAction)->actionKind;
		theFlag = (((*hAction)->actionFlags & weAFIsRedo) != 0);
	}

	if (redoFlag != nil)
	{
		*redoFlag = theFlag;
	}
	return theKind;
}

pascal OSErr WEBeginAction(WEHandle hWE)
{
	WEPtr pWE = *hWE;

	if (BTST(pWE->flags, weFAccumulateUndos))
	{
		//	calling WEBeginAction twice in a row
		//	is considered a protocol error
		return weProtocolErr;
	}

	//	set weFAccumulateUndos so that each new change will add to,
	//	rather than replace, the existing action stack
	BSET(pWE->flags, weFAccumulateUndos);

	//	start with a fresh action stack
	_WEForgetAction(&pWE->hActionStack);

	return noErr;
}

pascal OSErr WEEndAction(WEActionKind actionKind, WEHandle hWE)
{
	WEPtr pWE = *hWE;

	if (! BTST(pWE->flags, weFAccumulateUndos))
	{
		//	a call to WEEndAction not balanced by a previous
		//	call to WEBeginAction is a protocol error
		return weProtocolErr;
	}

	//	stop accumulating actions
	BCLR(pWE->flags, weFAccumulateUndos);

	//	make sure we have a non-empty action stack
	if (pWE->hActionStack == nil)
	{
		return weCantUndoErr;
	}

	//	set the action kind
	(*pWE->hActionStack)->actionKind = actionKind;

	return noErr;
}

pascal UInt32 WEGetModCount(WEHandle hWE)
{
	return (*hWE)->modCount;
}

pascal void WEResetModCount(WEHandle hWE)
{
	(*hWE)->modCount = 0;
	_WEForgetAction(&(*hWE)->hActionStack);
}

pascal void _WEAdjustUndoRange(SInt32 moreBytes, WEHandle hWE)
{
	WEActionHandle hAction;

	if ((hAction = (*hWE)->hActionStack) != nil)
	{
		(*hAction)->delRangeLength += moreBytes;
	}
}

pascal OSErr _WETypeChar(char theByte, WEHandle hWE)
{
	WEPtr pWE;
	char db[2];
	SInt32 offset, endOffset, charLength;
	OSErr err;

	pWE = *hWE;					// the WE record must be already locked
	charLength = 1;				// assume 1-byte character by default
	db[0] = theByte;
	offset = pWE->selStart;

	// delete current selection, if any
	if ((err = _WEDeleteRange(offset, pWE->selEnd, hWE)) != noErr)
	{
		goto cleanup;
	}

	pWE->selEnd = offset; // needed in case we take a premature exit

	// make sure the font script is synchronized with the keyboard script
	_WESynchNullStyle(hWE);

	if (BTST(pWE->flags, weFDoubleByte))
	{

		// special processing for double-byte characters
		if (pWE->firstByte != 0)
		{

			// if this byte is the second half of a double-byte character,
			// insert the two bytes at the same time (flush the double-byte cache)
			db[0] = pWE->firstByte;
			db[1] = theByte;
			charLength = 2;
			pWE->firstByte = 0;
		}
		else
		{

			// if theByte is the first half of a double-byte character, just cache it and exit
			if (CallWECharByteProc(&theByte, 0, FontToScript(pWE->nullStyle.runStyle.tsFont),
				hWE, pWE->charByteHook) == smFirstByte)
			{
				pWE->firstByte = theByte;
				return noErr;
			}
		}

	} // if double-byte script installed

	// insert the new character into the text
	if ((err = _WEInsertText(offset, db, charLength, hWE)) != noErr)
	{
		goto cleanup;
	}

	// adjust undo buffer for the new character
	_WEAdjustUndoRange(charLength, hWE);

	// invalid the null style
	BCLR(pWE->flags, weFUseNullStyle);

	// move the insertion point after the new character
	endOffset = offset + charLength;
	pWE->selStart = endOffset;
	pWE->selEnd = endOffset;
/*
	// redraw the text
	if ((err = _WERedraw(offset, endOffset, hWE)) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;
*/
	err = _WERedraw(offset, endOffset, hWE);
cleanup:
	// return result code
	return err;

}

pascal OSErr _WEBackspace(WEHandle hWE)
{
	// this routine is called by WEKey to handle the backspace key

	WEPtr pWE = *hWE;	// assume WE record is already locked
	WEAction *pAction;
	SInt32 rangeStart, rangeEnd, charLength;
	WERunInfo runInfo;
	char db[2];
	Boolean saveActionLock;
	OSErr err;

	// calculate the text range to delete
	// if the selection is non-empty, delete that
	rangeStart = pWE->selStart;
	rangeEnd = pWE->selEnd;
	if (rangeStart == rangeEnd)
	{

		// otherwise the selection is an insertion point
		// do nothing if insertion point is at the beginning of the text
		if (rangeStart == 0)
		{
			return noErr;
		}

		// determine the byte-type of the character preceding the insertion point
		charLength = (WECharByte(rangeStart - 1, hWE) == smSingleByte) ? 1 : 2;
		rangeStart -= charLength;

		if (pWE->hActionStack != nil)
		{
			// UNDO SUPPORT FOR BACKSPACES

			// lock the action record
			saveActionLock = _WESetHandleLock((Handle) pWE->hActionStack, true);
			pAction = *pWE->hActionStack;

			// backspaces over the newly entered text aren't a problem
			if (pAction->delRangeLength > 0)
			{
				pAction->delRangeLength -= charLength;
			}
			else
			{

				// the hard part comes when backspacing past the new text because
				// the user is about to delete a character not included in the block we saved
				db[0] = WEGetChar(rangeStart, hWE);
				if (charLength == 2)
				{
					db[1] = WEGetChar(rangeStart + 1, hWE);
				}

				// prepend the character to be deleted to the beginning of our saved text handle
				if ((err = _WESplice(pAction->hText, &db, charLength, 0)) != noErr)
				{
					return err;
				}

				// adjust internal counters
				pAction->insRangeLength += charLength;
				pAction->delRangeStart -= charLength;

				// get style run info associated with the about-to-be-deleted character
				WEGetRunInfo(rangeStart, &runInfo, hWE);

				// prepend a new style element to our style scrap, if necessary
				if ((err = _WEPrependStyle(pAction->hStyles, &runInfo, charLength)) != noErr)
				{
					return err;
				}

#if WASTE_OBJECTS
				// do the same with our object "soup"
				if ((err = _WEPrependObject(pAction->hSoup, &runInfo, charLength)) != noErr)
				{
					return err;
				}
#endif

			} // if deleting old text

			// unlock the action record
			_WESetHandleLock((Handle) pWE->hActionStack, saveActionLock);

		} // if undo support is enabled
	} // if selection is empty

	if ((err = _WEDeleteRange(rangeStart, rangeEnd, hWE)) != noErr)
	{
		return err;
	}

	// keep track of current selection range
	pWE->selStart = rangeStart;
	pWE->selEnd = rangeStart;

	// redraw the text
	err = _WERedraw(rangeStart, rangeStart, hWE);

	return err;
}

pascal OSErr _WEForwardDelete(WEHandle hWE)
{

	// this routine is called by WEKey to handle the forward delete key

	WEPtr pWE = *hWE;	// assume WE record is already locked
	WEAction *pAction;
	SInt32 rangeStart, rangeEnd, charLength;
	WERunInfo runInfo;
	char db[2];
	Boolean saveActionLock;
	OSErr err;

	// calculate the text range to delete
	// if the selection is non-empty, delete that
	rangeStart = pWE->selStart;
	rangeEnd = pWE->selEnd;
	if (rangeStart == rangeEnd)
	{

		// otherwise the selection is an insertion point
		// do nothing if insertion point is at the end of the text
		if (rangeStart == pWE->textLength)
		{
			return noErr;
		}

		// determine the byte-type of the character following the insertion point
		charLength = (WECharByte(rangeStart, hWE) == smSingleByte) ? 1 : 2;
		rangeEnd = rangeStart + charLength;

		if (pWE->hActionStack != nil)
		{

			// UNDO SUPPORT FOR FORWARD DELETE

			// lock the action record
			saveActionLock = _WESetHandleLock((Handle) pWE->hActionStack, true);
			pAction = *pWE->hActionStack;

			// make a copy of the character about to be deleted
			db[0] = WEGetChar(rangeStart, hWE);
			if (charLength == 2)
			{
				db[1] = WEGetChar(rangeStart + 1, hWE);
			}

			// append it to the end of our saved text handle
			if ((err = _WESplice(pAction->hText, &db, charLength, -1)) != noErr)
			{
				return err;
			}

			// get style run info associated with the about-to-be-deleted character
			WEGetRunInfo(rangeStart, &runInfo, hWE);

			// append a new style element to our style scrap, if necessary
			if ((err = _WEAppendStyle(pAction->hStyles, &runInfo, pAction->insRangeLength)) != noErr)
			{
				return err;
			}

#if WASTE_OBJECTS
			// do the same with our object soup
			if ((err = _WEAppendObject(pAction->hSoup, &runInfo, pAction->insRangeLength)) != noErr)
			{
				return err;
			}
#endif

			// adjust internal counters
			pAction->insRangeLength += charLength;

			// unlock the action record
			_WESetHandleLock((Handle) pWE->hActionStack, saveActionLock);

		} // if undo support is enabled
	} // if selection is empty

	if ((err = _WEDeleteRange(rangeStart, rangeEnd, hWE)) != noErr)
	{
		return err;
	}

	// keep track of current selection range
	pWE->selStart = rangeStart;
	pWE->selEnd = rangeStart;

	// redraw the text
	err = _WERedraw(rangeStart, rangeStart, hWE);

	return err;
}

pascal Boolean WEIsTyping(WEHandle hWE)
{
	// return true if we're tracking a typing sequence in the specified WE instance

	WEPtr pWE = *hWE;
	WEAction *pAction;

	// there must be an undo buffer
	if (pWE->hActionStack == nil)
	{
		return false;
	}

	pAction = *pWE->hActionStack;

	// the action kind must be "typing" and the redo flag must be clear
	if ((pAction->actionKind != weAKTyping) || ((pAction->actionFlags & weAFIsRedo) != 0))
	{
		return false;
	}

	// finally, the selection range mustn't have moved since the last WEKey
	if ((pWE->selStart != pWE->selEnd) ||
		(pWE->selStart != pAction->delRangeStart + pAction->delRangeLength))
	{
		return false;
	}

	return true;
}

pascal void WEKey(SInt16 key, EventModifiers modifiers, WEHandle hWE)
{
	WEPtr pWE;
	WEActionHandle hAction;
	Boolean saveWELock;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// hide the caret if it's showing
	if (BTST(pWE->flags, weFCaretVisible))
	{
		_WEBlinkCaret(hWE);
	}

	// hide the cursor (it will show again as soon as it's moved)
	ObscureCursor();

	// dispatch on key class (arrow keys, printable characters, backspace)
	if ((key >= kArrowLeft) && (key <= kArrowDown))
	{
		_WEDoArrowKey(key, modifiers, hWE);
	}
	else
	{

		// non-arrow keys modify the text, so make sure editing is allowed
		if (!BTST(pWE->features, weFReadOnly))
		{
			// are we tracking a typing sequence?
			if (!WEIsTyping(hWE))
			{
				// nope;  start a new one
				// increment modification count
				pWE->modCount++;

				// if undo support is enabled, create a new action to keep track of typing
				if (BTST(pWE->features, weFUndoSupport))
				{
					WEClearUndo(hWE);
					if (_WENewAction(pWE->selStart, pWE->selEnd, 0, weAKTyping, 0, hWE, &hAction) == noErr)
					{
						_WEPushAction(hAction);
					}
				}
			} // if WEIsTyping

			if (key == kBackspace)
			{
				_WEBackspace(hWE);
			}
			else if (key == kForwardDelete)
			{
				_WEForwardDelete(hWE);
			}
			else
			{
				_WETypeChar(key, hWE);
			}
		} // if not read-only
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
}

//ShadowIRC: Convert a TE style to an extended style
pascal Handle MyTEStyleToExtended(StScrpHandle styles, char del);
pascal Handle MyTEStyleToExtended(StScrpHandle styles, char del)
{
	TEStyleScrapHandle h;
	
	if(styles)
	{
		long numStyles = (GetHandleSize((Handle)styles) - sizeof(SInt16)) / sizeof(ScrpSTElement);
		TEStyleScrap *p;
		long x;
		
		h = (TEStyleScrapHandle)NewHandleClear(sizeof(short) + (numStyles * sizeof(myScrpSTElement)));
		HLock((Handle)h);
		
		p = *h;
		
		if(p->scrpNStyles > 32767)
			p->scrpNStyles = 32767;
		else
			p->scrpNStyles = numStyles;
			
		for(x=0;x<numStyles;x++)
			*(ScrpSTElement*)&p->scrpStyleTab[x] = *(ScrpSTElement*)&(**styles).scrpStyleTab[x];
		
		if(del)
			DisposeHandle((Handle)styles);
		return (Handle)h;
	}
	else
		return 0;
}

pascal OSErr WEInsert(Ptr textPtr, SInt32 textLength, myStScrpHandle hStyles, Handle hSoup, WEHandle hWE)
{
#if !WASTE_OBJECTS
#pragma unused(hSoup)
#endif
	WEPtr pWE;
	SInt32 offset, endOffset;
	WEActionHandle hAction;
	SInt16 intPasteAction;
	Boolean saveWELock;
	char space = kSpace;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;
	offset = pWE->selStart;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	// increment modification count
	pWE->modCount++;

	//ShadowIRC: Convert standard TE Styles to altered styles if the length is positive
	//if positive text length, and hStyles, convert the styles
	if(textLength > 0)
	{
		if(hStyles)
			hStyles = (myStScrpHandle)MyTEStyleToExtended((StScrpHandle)hStyles, false);
	}
	else
		textLength = -textLength;

	// if undo support is enabled, save current selection range
	if (BTST(pWE->features, weFUndoSupport))
	{
		WEClearUndo(hWE);
		if (_WENewAction(offset, pWE->selEnd, textLength, weAKUnspecified, 0, hWE, &hAction) == noErr)
		{
			_WEPushAction(hAction);
		}
	}

	// delete current selection
	if ((err = _WEDeleteRange(offset, pWE->selEnd, hWE)) != noErr)
	{
		goto cleanup;
	}

	// insert the new text at the insertion point
	if ((err = _WEInsertText(offset, textPtr, textLength, hWE)) != noErr)
	{
		goto cleanup;
	}
	endOffset = offset + textLength;

	if (hStyles != nil)
	{

		// if a style scrap was supplied, apply it to the newly inserted text
		if ((err = _WEApplyStyleScrap(offset, endOffset, hStyles, hWE)) != noErr)
		{
			goto cleanup;
		}
	}

#if WASTE_OBJECTS
	if (hSoup != nil)
	{
		// if an object soup was supplied, apply it to the newly inserted text
		if ((err = _WEApplySoup(offset, hSoup, hWE)) != noErr)
		{
			goto cleanup;
		}
	}
#endif

	// determine whether an extra space should be added before or after the inserted text
	intPasteAction = _WEIntelligentPaste(offset, endOffset, hWE);

	// add the extra space, if necessary
	if (intPasteAction != weDontAddSpaces)
	{
		if (intPasteAction == weAddSpaceOnLeftSide)
		{
			err = _WEInsertText(offset, &space, sizeof(space), hWE);
		}
		else
		{
			err = _WEInsertText(endOffset, &space, sizeof(space), hWE);
		}
		if (err != noErr)
		{
			goto cleanup;
		}
		endOffset++;

		// adjust undo buffer (if any) for the extra space
		_WEAdjustUndoRange(sizeof(space), hWE);
	}

	// invalid the null style
	BCLR(pWE->flags, weFUseNullStyle);

	// move the insertion point at the end of the inserted text
	pWE->selEnd = pWE->selStart = endOffset;

	// redraw the text
/*	if ((err = _WERedraw(offset, endOffset, hWE)) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;
*/
	err = _WERedraw(offset, endOffset, hWE);
	
cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

#if WASTE_OBJECTS

pascal OSErr WEInsertObject(FlavorType objectType, Handle objectDataHandle, Point objectSize, WEHandle hWE)
{
	WEPtr pWE;
	WEActionHandle hAction;
	SInt32 offset, endOffset;
	WETextStyle ts;
	Boolean saveWELock;
	char marker = kObjectMarker;
	OSErr err;

	BLOCK_CLR(ts);

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;
	offset = pWE->selStart;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	// call the 'new' handler to initialize private object storage (if any)
	// and to calculate the default size for this object

	if ((err = _WENewObject(objectType, objectDataHandle, hWE, &ts.tsObject)) != noErr)
	{
		goto cleanup;
	}

	// use the specified object size, unless it is (0, 0), in which case keep the default size
	if (* (SInt32 *) &objectSize != 0)
	{
		(*ts.tsObject)->objectSize = objectSize;
	}

	// increment modification count
	pWE->modCount++;

	// if undo support is enabled, save current selection range
	if (BTST(pWE->features, weFUndoSupport))
	{
		WEClearUndo(hWE);
		if (_WENewAction(offset, pWE->selEnd, 1, weAKUnspecified, 0, hWE, &hAction) == noErr)
		{
			_WEPushAction(hAction);
		}
	}

	// delete current selection
	if ((err = _WEDeleteRange(offset, pWE->selEnd, hWE)) != noErr)
	{
		goto cleanup;
	}

	// insert a kObjectMarker character at the insertion point
	if ((err = _WEInsertText(offset, &marker, sizeof(marker), hWE)) != noErr)
	{
		goto cleanup;
	}

	// move the insertion point after the inserted text
	endOffset = offset + 1;
	pWE->selStart = endOffset;
	pWE->selEnd = endOffset;

	// record a reference to the object descriptor in the style table
	err = _WESetStyleRange(offset, endOffset, weDoObject, &ts, hWE);
	ts.tsObject = nil;
	if (err != noErr)
	{
		goto cleanup;
	}

	// invalid the null style
	BCLR(pWE->flags, weFUseNullStyle);
/*
	// redraw the text
	if ((err = _WERedraw(offset, endOffset, hWE)) != noErr)
	{
		goto cleanup;
	}

	// clear result code
	err = noErr;
*/
	err = _WERedraw(offset, endOffset, hWE);
	
cleanup:
	// clean up
	_WEForgetHandle((Handle *) &ts.tsObject);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

#endif

pascal OSErr WEDelete(WEHandle hWE)
{
	WEPtr pWE;
	WEActionHandle hAction;
	SInt32 rangeStart, rangeEnd;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	// get current selection range
	rangeStart = pWE->selStart;
	rangeEnd = pWE->selEnd;

	// do nothing if the selection range is empty
	if (rangeStart < rangeEnd)
	{

		// increment modification count
		pWE->modCount++;

		// range extension for intelligent cut-and-paste
		_WEIntelligentCut(&rangeStart, &rangeEnd, hWE);

		// if undo support is enabled, save the range to be deleted
		if (BTST(pWE->features, weFUndoSupport))
		{
			WEClearUndo(hWE);
			if (_WENewAction(rangeStart, rangeEnd, 0, weAKClear, 0, hWE, &hAction) == noErr)
			{
				_WEPushAction(hAction);
			}
		}

		// delete the selection range
		if ((err = _WEDeleteRange(rangeStart, rangeEnd, hWE)) != noErr)
		{
			goto cleanup;
		}

		// reset the selection range
		pWE->selStart = rangeStart;
		pWE->selEnd = rangeStart;

		// redraw the text
		if ((err = _WERedraw(rangeStart, rangeStart, hWE)) != noErr)
		{
			goto cleanup;
		}
	} // if non-empty selection

	// clear result code
	err = noErr;

cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

pascal OSErr WECut(WEHandle hWE)
{
	OSErr err;

	// first copy...
	if ((err = WECopy(hWE)) != noErr)
	{
		goto cleanup;
	}

	// ... then delete
	if ((err = WEDelete(hWE)) != noErr)
	{
		goto cleanup;
	}

	// change the action kind of the most recent action, if any
	if ((*hWE)->hActionStack != nil)
	{
		(*(*hWE)->hActionStack)->actionKind = weAKCut;
	}

cleanup:
	// return result code
	return err;
}

pascal Boolean WECanPaste(WEHandle hWE)
{
	SInt32 scrapOffset;
	#pragma unused(scrapOffset) //not used in carbon
#if WASTE_OBJECTS
	FlavorType objectType;
	SInt32 index;
#endif

	if (!BTST((*hWE)->features, weFReadOnly))
	{
		// return true if the desk scrap contains a text flavor
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
		ScrapRef scrap;
		ScrapFlavorFlags flags;
		if (GetCurrentScrap(&scrap) == noErr &&
			GetScrapFlavorFlags(scrap, kTypeText, &flags) > 0)
#else
		if (GetScrap(nil, kTypeText, &scrapOffset) > 0)
#endif
		{
			return true;
		}

#if WASTE_OBJECTS
		// see if the desk scrap contains a flavor matching one of the registered object types
		index = 0;
		while (_WEGetIndObjectType(index, &objectType, hWE) == noErr)
		{
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
			if (GetScrapFlavorFlags(scrap, objectType, &flags) > 0)
#else
			if (GetScrap(nil, objectType, &scrapOffset) > 0)
#endif
			{
				return true;
			}
			index++;
		} // while
#endif
	} // if not read-only
	return false;
}

pascal OSErr _WEGetScrapHandle ( FlavorType dataFlavor, Handle * dataHandle )
{
	SInt32 scrapResult ;
	SInt32 scrapLength = 0 ;
	OSErr err ;
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	ScrapRef scrap ;
	//ScrapFlavorFlags scrapFlags ;
#else
	SInt32 scrapOffset ;
#endif

	* dataHandle = nil;

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	if ( ( err = GetCurrentScrap(&scrap)) != noErr ||
		 ( err = GetScrapFlavorSize(scrap, kTypeText, &scrapLength ) ) != noErr )
	{
		goto cleanup ;
	}
#endif

	//	allocate a temporary handle
	if ( ( err = _WEAllocate ( scrapLength, kAllocTemp, dataHandle ) ) != noErr )
	{
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
		_WEForgetHandle ( dataHandle ) ;
#endif
		goto cleanup ;
	}

	//	look for an item with the specified flavor on the desk scrap
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	_WESetHandleLock ( * dataHandle, true ) ;
	err = GetScrapFlavorData(scrap, kTypeText, &scrapLength,  ** dataHandle ) ;
	_WESetHandleLock ( * dataHandle, false ) ;
	if ( ( scrapResult = err ) != noErr )
#else
	if ( ( scrapResult = GetScrap ( * dataHandle, dataFlavor, & scrapOffset ) ) < 0 )
#endif
	{
		//	a negative result from GetScrap is an error code
		err = scrapResult ;
		_WEForgetHandle ( dataHandle ) ;
		goto cleanup ;
	}
	else
	{
		if(dataFlavor == kTypeStyles)
		{
			*dataHandle = MyTEStyleToExtended((StScrpHandle)*dataHandle, true);
		}
	}

	//	clear result code
	err = noErr ;

cleanup :
	//	return result code
	return err ;
}

pascal OSErr WEPaste ( WEHandle hWE )
{
	Handle hText = nil ;
	Handle hStyles = nil ;
	Handle hFontTable = nil ;
	Handle hSoup = nil ;
	OSErr err;

	// look for a text flavor
	if ( ( err = _WEGetScrapHandle ( kTypeText, & hText ) ) == noTypeErr )
	{

#if WASTE_OBJECTS
		// no text: look for a flavor matching one of the registered object types

		Handle hObjectData = nil ;
		FlavorType objectType = 0 ;
		SInt32 index = 0 ;

		while ( _WEGetIndObjectType ( index, & objectType, hWE ) == noErr )
		{
			if ( ( err = _WEGetScrapHandle ( objectType, & hObjectData ) ) == noErr )
			{
				Point objectSize ;
				* ( SInt32 * ) & objectSize = 0 ;

				// found a registered type: create a new object out of the tagged data
				if ( ( err = WEInsertObject ( objectType, hObjectData, objectSize, hWE ) ) != noErr )
				{
					_WEForgetHandle ( & hObjectData ) ;
				}
				goto cleanup;
			}
			else if ( err != noTypeErr )
			{
				goto cleanup ;
			}

			// try with next flavor
			index++;
		} // while
#endif

		// nothing pasteable: return error code
		goto cleanup;
	}
	else if ( err != noErr )
	{
		goto cleanup ;
	}

	if ( ! BTST ( ( * hWE ) -> features, weFMonoStyled ) )
	{
		//	look for a style scrap accompanying the text
		_WEGetScrapHandle ( kTypeStyles, & hStyles ) ;

		if ( hStyles != nil )
		{
			//	if a style scrap is available, further look for a supplemental font table
			if ( _WEGetScrapHandle ( kTypeFontTable, & hFontTable ) == noErr )
			{
				Boolean wasChanged = false ;

				//	perform any necessary font ID remapping
				if ( ( WEUpdateFontTable ( hFontTable, nil, & wasChanged ) == noErr ) && wasChanged )
				{
					WEUpdateStyleScrap ( ( StScrpHandle ) hStyles, hFontTable ) ;
				}
			}
		}

#if WASTE_OBJECTS
		//	look for an object soup
		_WEGetScrapHandle ( kTypeSoup, & hSoup ) ;
#endif
	} // if not mono-styled

	// lock down the text
	HLock ( hText ) ;

	// insert the text
	//ShadowIRC: negative size since we have native stuff
	err = WEInsert ( * hText, -GetHandleSize ( hText ), ( myStScrpHandle ) hStyles, hSoup, hWE ) ;

cleanup :
	// if successful, change the action kind of the most recent action, if any
	if ( ( err == noErr ) && ( ( *hWE ) -> hActionStack != nil ) )
	{
		( * ( * hWE ) -> hActionStack ) -> actionKind = weAKPaste ;
	}

	// clean up
	_WEForgetHandle ( & hText ) ;
	_WEForgetHandle ( & hStyles ) ;
	_WEForgetHandle ( & hFontTable ) ;
#if WASTE_OBJECTS
	_WEForgetHandle ( & hSoup ) ;
#endif

	// return result code
	return err ;
}

pascal OSErr _WESmartSetFont(WEStyleMode mode, const WETextStyle *ts, WEHandle hWE)
{
	WEPtr pWE = *hWE;	// assume WE record is already locked
	ScriptCode script;
	SInt32 runIndex;
	SInt32 rangeStart, rangeEnd;
	WERunInfo runInfo;
	GrafPtr savePort;
	SInt16 saveFont;
	OSErr err;

	// set up the graphics port
	GetPort(&savePort);
	SetPort(pWE->port);
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	saveFont = GetPortTextFont(pWE->port);
#else
	saveFont = pWE->port->txFont;
#endif

	// get the script corresponding to the font we're applying
	script = FontToScript(ts->tsFont);

	// walk through the style runs encompassing the selection range
	runIndex = WEOffsetToRun(pWE->selStart, hWE);
	do
	{
		_WEGetIndStyle(runIndex, &runInfo, hWE);

		if (pWE->selStart > runInfo.runStart)
		{
			rangeStart = pWE->selStart;
		}
		else
		{
			rangeStart = runInfo.runStart;
		}

		if (pWE->selEnd < runInfo.runEnd)
		{
			rangeEnd = pWE->selEnd;
		}
		else
		{
			rangeEnd = runInfo.runEnd;
		}

		// does this style run belong to the same script we're applying?
		if (FontToScript(runInfo.runAttrs.runStyle.tsFont) == script)
		{
			if ((err = _WESetStyleRange(rangeStart, rangeEnd, weDoFont, ts, hWE)) != noErr)
			{
				goto cleanup;
			}
			runIndex = WEOffsetToRun(runInfo.runEnd, hWE);
		}
		else if (mode & weDoExtractSubscript)
		{
			SInt32 runLength;
			SInt32 subrunLength;
			ScriptRunStatus runStatus;

			// FindScriptRun takes an implicit parameter through the txFont field of thePort
			TextFont(runInfo.runAttrs.runStyle.tsFont);

			runLength = rangeEnd - rangeStart;
			while (runLength > 0)
			{
				// lock text handle
				Boolean saveTextLock = _WESetHandleLock(pWE->hText, true);

				// look for blocks of subscript text
				runStatus = FindScriptRun(*pWE->hText + rangeStart, runLength, &subrunLength);

				// unlock text handle
				_WESetHandleLock(pWE->hText, saveTextLock);

				if (runStatus.script == script)
				{
					// "extract" subscript text
					if ((err = _WESetStyleRange(rangeStart, rangeStart + subrunLength, weDoFont, ts, hWE)) != noErr)
					{
						goto cleanup;
					}
				}
				rangeStart += subrunLength;
				runLength -= subrunLength;
			}
			runIndex = WEOffsetToRun(runInfo.runEnd, hWE);
		}
		else
			runIndex++;

	} while (runInfo.runEnd < pWE->selEnd);

	// clear result code
	err = noErr;

cleanup:
	// restore the port
	TextFont(saveFont);
	SetPort(savePort);

	// return result code
	return err;
}

pascal OSErr WESetStyle(WEStyleMode mode, const TextStyle *ts, WEHandle hWE)
{
	WEPtr pWE;
	WEActionHandle hAction;
	ScriptCode fontScript;
	Boolean saveWELock;
	OSErr err;
//ShadowIRC
	WETextStyle ts2;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

//ShadowIRC: Save background stuff
	*(TextStyle*)&ts2 = *ts;
	ts2.tsBackColor.red = 
	ts2.tsBackColor.green = 
	ts2.tsBackColor.blue = 0;
	ts2.extraFlags = 0;
	
	if (BTST(pWE->features, weFMonoStyled))
	{
		// MONOSTYLED TEXT
		// apply the change to the whole text, not just to the selection range
		if ((err = _WESetStyleRange(0, pWE->textLength, mode, &ts2, hWE)) != noErr)
		{
			goto cleanup;
		}

		// invalidate the null style record
		BCLR(pWE->flags, weFUseNullStyle);

		// redraw the text
		if ((err = _WERedraw(0, pWE->textLength, hWE)) != noErr)
		{
			goto cleanup;
		}
	}
	else if (pWE->selStart == pWE->selEnd)
	{
		// MULTISTYLED TEXT; NULL SELECTION
		// first make sure the nullStyle field contains valid information
		_WESynchNullStyle(hWE);

		// apply style changes to the nullStyle record
		_WECopyStyle(&ts2, &pWE->nullStyle.runStyle, pWE->nullStyle.runStyle.tsFace, mode);

		// special case: if this instance is empty, propagate the
		// change to the style table (this avoids some subtle problems)
		if (pWE->textLength == 0)
		{
			if ((err = _WESetStyleRange(0, 0, weDoAll + weDoReplaceFace, &pWE->nullStyle.runStyle, hWE)) != noErr)
			{
				goto cleanup;
			}
		}

#if !WASTE_NO_SYNCH
		// if the font was altered, synchronize the keyboard script
		if (BTST(pWE->flags, weFNonRoman) && (mode & weDoFont))
		{
			fontScript = FontToScript(pWE->nullStyle.runStyle.tsFont);
			if (fontScript != GetScriptManagerVariable(smKeyScript))
			{
				KeyScript(fontScript);
			}
		}
#endif
	}
	else
	{
		// MULTISTYLED TEXT; NON-EMPTY SELECTION

		// increment modification count
		pWE->modCount++;

		// if undo support is enabled, save the styles of the text range to be affected
		if (BTST(pWE->features, weFUndoSupport))
		{
			WEClearUndo(hWE);
			if (_WENewAction(pWE->selStart, pWE->selEnd, pWE->selEnd - pWE->selStart, weAKSetStyle,
				weAFDontSaveText + weAFDontSaveSoup, hWE, &hAction) == noErr)
			{
				_WEPushAction(hAction);
			}
		}

		// check for "smart" font modes
		if (BTST(pWE->flags, weFNonRoman) && ((mode & weDoSmartFont) == weDoSmartFont))
		{
			if ((err = _WESmartSetFont(mode, &ts2, hWE)) != noErr)
			{
				goto cleanup;
			}
			mode &= ~weDoFont;
		}

		// set the style of the selection range
		if ((err = _WESetStyleRange(pWE->selStart, pWE->selEnd, mode, &ts2, hWE)) != noErr)
		{
			goto cleanup;
		}

		// and redraw the text
		if ((err = _WERedraw(pWE->selStart, pWE->selEnd, hWE)) != noErr)
		{
			goto cleanup;
		}
	}

	// clear the result code
	err = noErr;

cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

pascal OSErr WEUseStyleScrap(myStScrpHandle hStyles, WEHandle hWE)
{
	WEPtr pWE;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// apply the style scrap to the selection range
	if ((err = _WEApplyStyleScrap(pWE->selStart, pWE->selEnd, hStyles, hWE)) != noErr)
	{
		goto cleanup;
	}

	// redraw the text
	err = _WERedraw(pWE->selStart, pWE->selEnd, hWE);

cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

#if WASTE_OBJECTS

pascal OSErr WEUseSoup(Handle hSoup, WEHandle hWE)
{
	WEPtr pWE;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// apply the soup starting from selStart
	if ((err = _WEApplySoup(pWE->selStart, hSoup, hWE)) != noErr)
	{
		goto cleanup;
	}

	// redraw the text
	err = _WERedraw(pWE->selStart, pWE->selEnd, hWE);

cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}

#endif  // WASTE_OBJECTS
