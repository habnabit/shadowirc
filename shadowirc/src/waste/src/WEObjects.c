/*
 *	WEObjects.c
 *
 *	WASTE PROJECT
 *  Embedded Objects
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

#if WASTE_OBJECTS

// static variables

static WEOHTableHandle _weGlobalObjectHandlerTable;

pascal FlavorType WEGetObjectType(WEObjectDescHandle hObjectDesc)
{
	return (*hObjectDesc)->objectType;
}

pascal Handle WEGetObjectDataHandle(WEObjectDescHandle hObjectDesc)
{
	return (*hObjectDesc)->objectDataHandle;
}

pascal Point WEGetObjectSize(WEObjectDescHandle hObjectDesc)
{
	return (*hObjectDesc)->objectSize;
}

pascal WEHandle WEGetObjectOwner(WEObjectDescHandle hObjectDesc)
{
	return (*hObjectDesc)->objectOwner;
}

pascal SInt32 WEGetObjectRefCon(WEObjectDescHandle hObjectDesc)
{
	return (*hObjectDesc)->objectRefCon;
}

pascal void WESetObjectRefCon(WEObjectDescHandle hObjectDesc, SInt32 refCon)
{
	(*hObjectDesc)->objectRefCon = refCon;
}

pascal SInt32 _WELookupObjectType(FlavorType objectType, WEOHTableHandle hTable)
{
	// look for a WEOHTableEntry record for the specified object kind
	// in the given object handler table

	SInt32 nEntries, index;
	WEOHTableEntry *pTable;

	// do nothing if the Object Handler Table has not been inited yet
	if (hTable == nil)
	{
		return kUnknownObjectType;
	}

	// calculate entry count
	nEntries = GetHandleSize((Handle) hTable) / sizeof(WEOHTableEntry);

	// scan the Object Handler Table looking for a type match
	pTable = *hTable;
	for ( index = 0; index < nEntries; index++ )
	{
		if (pTable->objectType == objectType)
		{
			return index;
		}
		pTable++;
	}

	return kUnknownObjectType;
}

pascal OSErr _WEGetIndObjectType(SInt32 index, FlavorType *objectType, WEHandle hWE)
{
	WEOHTableHandle hTable;
	SInt32 nEntries;

	*objectType = 0L;

	// index must be non-negative
	if (index < 0)
	{
		return weUnknownObjectTypeErr;
	}

	// calculate number of entries in the instance-specific handler table
	nEntries = 0;
	hTable = (*hWE)->hObjectHandlerTable;
	if (hTable != nil)
	{
		nEntries = GetHandleSize((Handle) hTable) / sizeof(WEOHTableEntry);
	}

	// low indices refer to the instance-specific handler table
	if (index < nEntries)
	{
		*objectType = (*hTable)[index].objectType;
		return noErr;
	}
	// indices above that refer to the global handler table
	index -= nEntries;

	// calculate number of entries in the global handler table
	nEntries = 0;
	hTable = _weGlobalObjectHandlerTable;
	if (hTable != nil)
	{
		nEntries = GetHandleSize((Handle) hTable) / sizeof(WEOHTableEntry);
	}

	// return an error code if index is too large
	if (index >= nEntries)
	{
		return weUnknownObjectTypeErr;
	}

	*objectType = (*hTable)[index].objectType;

	return noErr;
}

pascal OSErr _WENewObject(FlavorType objectType, Handle objectDataHandle, WEHandle hWE,
					WEObjectDescHandle *hObjectDesc)
{
	WEObjectDesc *pDesc;
	WEOHTableHandle hTable;
	SInt32 index;
	OSErr err;

	*hObjectDesc = nil;

	// first look up the specified object type in the instance-specific handler table
	hTable = (*hWE)->hObjectHandlerTable;
	if ((index = _WELookupObjectType(objectType, hTable)) == kUnknownObjectType)
	{
		// no match: try with the global handler table
		hTable = _weGlobalObjectHandlerTable;
		if ((index = _WELookupObjectType(objectType, hTable)) == kUnknownObjectType)
		{
			hTable = nil;
		}
	}

	// create a new relocatable block to hold the object descriptor
	if ((err = _WEAllocate(sizeof(WEObjectDesc), kAllocClear, (Handle *)hObjectDesc)) != noErr)
	{
		return err;
	}

	// lock it down
	HLock((Handle) *hObjectDesc);
	pDesc = (WEObjectDesc *) **hObjectDesc;

	// fill in the object descriptor
	pDesc->objectType = objectType;
	pDesc->objectDataHandle = objectDataHandle;
	pDesc->objectSize.v = kDefaultObjectHeight;
	pDesc->objectSize.h = kDefaultObjectWidth;
	pDesc->objectTable = hTable;
	pDesc->objectIndex = index;
	pDesc->objectOwner = hWE;

	if (hTable != nil)
	{
		WENewObjectUPP newHandler = (*hTable)[index].newHandler;
		pDesc->objectRefCon = (*hTable)[index].refCon;

		// call the new handler, if any
		if (newHandler != nil)
		{
			if ((err = CallWENewObjectProc(&pDesc->objectSize, *hObjectDesc, newHandler)) != noErr)
			{
				_WEForgetHandle((Handle *) hObjectDesc);
				return err;
			}
		}
	}

	// unlock the object descriptor
	HUnlock((Handle) *hObjectDesc);

	// clear result code
	return noErr;
}

pascal OSErr _WEFreeObject(WEObjectDescHandle hObjectDesc)
{
	WEObjectDesc *pDesc;
	OSErr err = noErr;

	// sanity check: do nothing if we have a null descriptor handle
	if (hObjectDesc == nil)
	{
		return nilHandleErr;
	}

	// lock the descriptor record
	HLock((Handle)hObjectDesc);
	pDesc = *hObjectDesc;

	if (pDesc->objectTable != nil)
	{
		WEDisposeObjectUPP freeHandler = (*pDesc->objectTable)[pDesc->objectIndex].freeHandler;

		// call the dispose handler, if any
		if (freeHandler != nil)
		{
			err = CallWEDisposeObjectProc(hObjectDesc, freeHandler);
			pDesc->objectDataHandle = nil;
		}
	}

	// if object kind is unknown or there's no custom dispose handler, use DisposeHandle
	_WEForgetHandle(&pDesc->objectDataHandle);

	// finally, dispose of the object descriptor itself
	DisposeHandle((Handle)hObjectDesc);

	return err;
}

pascal OSErr _WEDrawObject(WEObjectDescHandle hObjectDesc)
{
	WEObjectDesc *pDesc;
	Rect destRect;
	PenState state;
	Boolean saveDescLock;
	OSErr err = noErr;

	// lock the object descriptor
	saveDescLock = _WESetHandleLock((Handle)hObjectDesc, true);
	pDesc = *hObjectDesc;

	// get current pen state
	// state.pnLoc has already been set to the bottom left of the rectangle to draw
	GetPenState(&state);

	// calculate the new pen position
	state.pnLoc.h += pDesc->objectSize.h;

	// calculate the destination rectangle
	* (SInt32 *) (&topLeft(destRect)) = DeltaPoint(state.pnLoc, pDesc->objectSize);
	botRight(destRect) = state.pnLoc;

	if (pDesc->objectTable != nil)
	{
		WEDrawObjectUPP drawHandler = (*pDesc->objectTable)[pDesc->objectIndex].drawHandler;

		// call the drawing handler, if any
		if (drawHandler != nil)
		{
			err = CallWEDrawObjectProc(&destRect, hObjectDesc, drawHandler);
		}
	}
	else
	{
		// if this object kind was not registered, draw an empty frame
		PenNormal();
		FrameRect(&destRect);
	}

	// restore original pen state, advancing the pen position by the object width
	SetPenState(&state);

	// unlock the object descriptor
	_WESetHandleLock((Handle)hObjectDesc, saveDescLock);

	return err;
}

pascal Boolean _WEClickObject(Point hitPt, EventModifiers modifiers, UInt32 clickTime,
								WEObjectDescHandle hObjectDesc)
{
	WEObjectDesc *pDesc;
	Boolean saveDescLock;
	Boolean clickHandled = false;

	// lock the object descriptor
	saveDescLock = _WESetHandleLock((Handle)hObjectDesc, true);
	pDesc = *hObjectDesc;

	if (pDesc->objectTable != nil)
	{
		WEClickObjectUPP clickHandler = (*pDesc->objectTable)[pDesc->objectIndex].clickHandler;

		// call the click handler, if any
		if (clickHandler != nil)
		{
			clickHandled = CallWEClickObjectProc(hitPt, modifiers, clickTime, hObjectDesc, clickHandler);
		}
	}

	// unlock the object descriptor
	_WESetHandleLock((Handle)hObjectDesc, saveDescLock);

	return clickHandled;
}

pascal OSErr _WEStreamObject(SInt16 destKind, FlavorType *theType,
				Handle *theData, Boolean *canDisposeData, WEObjectDescHandle hObjectDesc)
{
	WEObjectDesc *pDesc;
	Boolean saveDescLock;
	OSErr err = weNotHandledErr;

	*canDisposeData = true;

	// lock the object descriptor
	saveDescLock = _WESetHandleLock((Handle)hObjectDesc, true);
	pDesc = *hObjectDesc;

	if (pDesc->objectTable != nil)
	{
		WEStreamObjectUPP streamHandler = (*pDesc->objectTable)[pDesc->objectIndex].streamHandler;

		// call the stream handler, if any
		if (streamHandler != nil)
		{
			err = CallWEStreamObjectProc(destKind, theType, *theData, hObjectDesc, streamHandler);
		}
	}

	if (err == weNotHandledErr)
	{
		// default streaming behavior
		_WEForgetHandle(theData);
		*canDisposeData = false;
		*theType = pDesc->objectType;
		*theData = pDesc->objectDataHandle;
		err = noErr;
	}

	// unlock the object descriptor
	_WESetHandleLock((Handle) hObjectDesc, saveDescLock);

	return err;
}

pascal OSErr WEInstallObjectHandler(FlavorType objectType, WESelector handlerSelector,
				UniversalProcPtr handler, WEHandle hWE)
{
	WEOHTableHandle hTable;
	SInt32 index;
	WEOHTableEntry entry;
	OSErr err;

	// if hWE is nil, install the handler in the global handler table,
	// otherwise install the handler in the instance-specific handler table
	if (hWE == nil)
	{
		hTable = _weGlobalObjectHandlerTable;
	}
	else
	{
		hTable = (*hWE)->hObjectHandlerTable;
	}

	// create the handler table, if it doesn't exist
	if (hTable == nil)
	{
		hTable = (WEOHTableHandle) NewHandle(0);
		if ((err = MemError()) != noErr)
		{
			return err;
		}
		if (hWE == nil)
		{
			_weGlobalObjectHandlerTable = hTable;
		}
		else
		{
			(*hWE)->hObjectHandlerTable = hTable;
		}
	}

	// look for the entry corresponding to the specified object type
	if ((index = _WELookupObjectType(objectType, hTable)) == kUnknownObjectType)
	{
		BLOCK_CLR(entry);
		entry.objectType = objectType;

		// previously unknown object type: append a new entry at the end of the handler table
		index = GetHandleSize((Handle) hTable) / sizeof(WEOHTableEntry);
		if ((err = _WESplice((Handle) hTable, &entry, sizeof(entry), -1)) != noErr)
		{
			return err;
		}
	}

	// install the handler
	return _WESetField(_weObjectHandlerSelectorTable, handlerSelector, (SInt32 *) &handler, *hTable + index);
}

pascal OSErr WEGetObjectHandler(FlavorType objectType, WESelector handlerSelector,
				UniversalProcPtr *handler, WEHandle hWE)
{
	WEOHTableHandle hTable;
	SInt32 index;

	// if hWE is nil, look for the handler in the global handler table,
	// otherwise look in the instance-specific handler table
	if (hWE == nil)
	{
		hTable = _weGlobalObjectHandlerTable;
	}
	else
	{
		hTable = (*hWE)->hObjectHandlerTable;
	}

	// first make sure the handler table exists
	// then check to see if an entry for the specified object type is present
	if ((hTable == nil) || ((index = _WELookupObjectType(objectType, hTable)) == kUnknownObjectType))
	{
		return weUnknownObjectTypeErr;
	}

	// return the address of the specified handler
	return _WEGetField(_weObjectHandlerSelectorTable, handlerSelector, (SInt32 *) handler, *hTable + index);
}

#endif  // WASTE_OBJECTS
