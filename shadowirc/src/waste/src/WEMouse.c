/*
 *	WEMouse.c
 *
 *	WASTE PROJECT
 *  Mouse Clicks and Support for Drag and Drop
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

#include <Carbon/Carbon.h>

#if WASTE_IC_SUPPORT
#ifndef __INTERNETCONFIG__
#include "InternetConfig.h"
#endif
#endif // WASTE_IC_SUPPORT

#if WASTE_DRAG_AND_DROP

//	static variables

static DragSendDataUPP _weFlavorSender = nil ;

pascal Boolean _WEIsOptionDrag ( DragReference drag )
{
	EventModifiers downModifiers, upModifiers ;

	// get drag modifiers
	// ??? shouldn't GetDragModifiers take EventModifiers* parameters ???
	GetDragModifiers ( drag, nil, ( SInt16 * ) & downModifiers, ( SInt16 * ) & upModifiers ) ;

	// return true if the option key was held down at the beginning and/or at the end
	return ( ( ( downModifiers | upModifiers ) & optionKey ) != 0 ) ;
}

pascal OSErr _WEGetFlavor ( DragReference drag, ItemReference dragItem,
				FlavorType requestedType, Handle hFlavor,
				SInt32 dropOffset, WEHandle hWE )
{
	// get the requested flavor out of the specified drag reference and put it into
	// the given handle, if any -- if hFlavor is nil, just check whether the specified flavor
	// is there or can be obtained by invoking a user-defined translation routine

	FlavorFlags flags ;
	Size dataSize ;
	WETranslateDragUPP translateDragHook ;
	Boolean saveFlavorLock ;
	OSErr err ;

	// see if the drag item has the requested flavor type,
	// without forcing the actual data to be sent and/or translated
	err = GetFlavorFlags ( drag, dragItem, requestedType, & flags ) ;
	if ( err == badDragFlavorErr )
	{
		// requested flavor is not available: our client may try a custom translation
		// this is especially handy to translate HFS objects like TEXT and PICT files
		if ( ( translateDragHook = ( * hWE ) -> translateDragHook ) != nil )
		{
			err = CallWETranslateDragProc ( drag, dragItem, requestedType, hFlavor,
						dropOffset, hWE, translateDragHook ) ;
		}
	}
	else if ( err == noErr )
	{

		// requested flavor is available: get it if hFlavor is not nil
		if ( hFlavor == nil )
		{
			return err ;
		}

		// get size of flavor data
		if ( ( err = GetFlavorDataSize( drag, dragItem, requestedType, & dataSize ) ) != noErr )
		{
			return err ;
		}

		// resize the handle
		SetHandleSize ( hFlavor, dataSize ) ;
		if ( ( err = MemError ( ) ) != noErr )
		{
			return err ;
		}

		// get flavor data
		saveFlavorLock = _WESetHandleLock ( hFlavor, true ) ;
		err = GetFlavorData ( drag, dragItem, requestedType, * hFlavor, & dataSize, 0 ) ;
		_WESetHandleLock ( hFlavor, saveFlavorLock ) ;
	}

	return err;
}

pascal Handle MyTEStyleToExtended(StScrpHandle styles);

pascal OSErr _WEExtractFlavor ( DragReference drag, ItemReference dragItem,
						FlavorType theType, Handle *hFlavor,
						SInt32 dropOffset, WEHandle hWE )
{
	OSErr err;

	// allocate a new handle
	if ( ( err = _WEAllocate ( 0, kAllocTemp, hFlavor ) ) == noErr )
	{
		// put the requested flavor into this handle
		if ( ( err = _WEGetFlavor ( drag, dragItem, theType, * hFlavor, dropOffset, hWE ) ) != noErr )
		{

		// if an error occurred, forget the handle
			_WEForgetHandle ( hFlavor ) ;
		}
		else
		{
			if(theType == kTypeStyles)
			{
				*hFlavor = MyTEStyleToExtended((StScrpHandle)*hFlavor);
			}
		}
	}

	return err ;
}

pascal void _WEUpdateDragCaret ( SInt32 offset, WEHandle hWE )
{
	WEPtr pWE = * hWE ;		// assume WE record is already locked
	UInt32 currentTime ;

	// get current time
	currentTime = TickCount ( ) ;

	if ( offset == pWE -> dragCaretOffset )
	{

		// drag caret offset didn't change; blink the caret
		if ( ( currentTime > pWE -> caretTime + GetCaretTime ( ) ) && ( offset != kInvalidOffset ) )
		{
			_WEDrawCaret ( pWE -> dragCaretOffset, hilite, false, hWE ) ;
			BCHG ( pWE -> flags, weFDragCaretVisible ) ;		// invert flag
			pWE -> caretTime = currentTime ;
		}
	}
	else
	{

		// drag caret offset did change
		// hide old caret, if it's showing
		if ( BTST ( pWE -> flags, weFDragCaretVisible ) )
		{
			_WEDrawCaret ( pWE -> dragCaretOffset, hilite, false, hWE ) ;
		}

		// show new caret (unless offset is kInvalidOffset)
		if ( offset != kInvalidOffset )
		{
			_WEDrawCaret ( offset, hilite, false, hWE ) ;
			BSET ( pWE -> flags, weFDragCaretVisible ) ;
			pWE -> caretTime = currentTime ;
		}
		else
		{
			BCLR ( pWE -> flags, weFDragCaretVisible ) ;
		}

		// remember drag caret offset
		pWE -> dragCaretOffset = offset ;
	}
}

pascal Handle MyExtendedStyleToTE(TEStyleScrapHandle styles);

static pascal OSErr _WESendFlavor(FlavorType requestedType, void *dragSendRefCon, WEHandle hWE, DragReference drag)
{
#pragma unused(dragSendRefCon)

#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #if !TARGET_RT_MAC_CFM
	SInt32 saveA5 = SetCurrentA5();	// this fixes a conflict with HoverBar
									// (well, probably a bug in the Drag Manager)
  #endif
#else
  #if !GENERATINGCFM
	SInt32 saveA5 = SetCurrentA5();	// this fixes a conflict with HoverBar
									// (well, probably a bug in the Drag Manager)
  #endif
#endif

	WEPtr pWE = *hWE;
	SInt32 selStart = pWE->selStart;
	SInt32 selEnd = pWE->selEnd;
	Handle hItem = nil;
	Boolean disposeItem = true;		// dispose of item when done	
#if WASTE_OBJECTS
	WEObjectDescHandle hObjectDesc;
#endif
	OSErr err;
//ShadowIRC
	Handle myData =0;

	// allocate a temporary handle to hold a copy of the requested flavor
	if ((err = _WEAllocate(0, kAllocTemp, &hItem)) != noErr)
	{
		goto cleanup;
	}

#if WASTE_OBJECTS
	// see if the selection contains an embedded object whose type matches the flavortype
	if (WEGetSelectedObject(&hObjectDesc, hWE) == noErr)
	{
		FlavorType theType;

		if ((err = _WEStreamObject(weToDrag, &theType, &hItem, &disposeItem, hObjectDesc)) != noErr)
		{
			goto cleanup;
		}

		// make sure theType matches the requested type
		err = badDragFlavorErr;
		if (theType != requestedType)
		{
			goto cleanup;
		}
	}
	else
#endif
	{
		// identify the requested flavor type as either 'TEXT', 'styl' or 'SOUP'
		if (requestedType == kTypeText)
		{
			err = WECopyRange(selStart, selEnd, hItem, nil, nil, hWE);
		}
		else if (requestedType == kTypeStyles)
		{
			Handle x;
			
			//ShadowIRC: ???
			err = WECopyRange(selStart, selEnd, nil, hItem, nil, hWE);
			if(!err)
			{
				x = MyExtendedStyleToTE((TEStyleScrapHandle)hItem);
				myData = hItem;
				hItem = x;
			}
		}
		else if (requestedType == kTypeFontTable)
		{
			err = WEBuildFontTable(hItem, nil, hWE);
		}
#if WASTE_OBJECTS
		else if (requestedType == kTypeSoup)
		{
			err = WECopyRange(selStart, selEnd, nil, nil, hItem,hWE);
		}
#endif
		else
		{
			err = badDragFlavorErr;
		}

		if (err != noErr)
		{
			goto cleanup;
		}
	}

	// set the drag flavor data
	HLock(hItem);
	err = SetDragItemFlavorData(drag, (ItemReference) hWE, requestedType, *hItem,
			GetHandleSize(hItem), 0);
	HUnlock(hItem);
	
	//ShadowIRC: ???
	if(myData)
	{
		DisposeHandle((Handle)hItem);
		hItem = myData;
	}

cleanup:
	if (disposeItem)
	{
		_WEForgetHandle(&hItem);
	}

#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #if !TARGET_RT_MAC_CFM
	SetA5(saveA5);
  #endif
#else
  #if !GENERATINGCFM
	SetA5(saveA5);
  #endif
#endif

	// return result code
	return err;

}

pascal RgnHandle _WEOutlineRgn ( RgnHandle solidRgn )
{
	RgnHandle outlineRgn ;

	//	make an outline region out of the given solid region
	if ( ( outlineRgn = NewRgn ( ) ) != nil )
	{
		CopyRgn ( solidRgn, outlineRgn ) ;
		InsetRgn ( outlineRgn, 1, 1 ) ;
		DiffRgn ( solidRgn, outlineRgn, outlineRgn ) ;
	}

	return outlineRgn ;
}

#if WASTE_TRANSLUCENT_DRAGS

pascal OSErr _WEMakeDragImage ( GWorldPtr * imageGWorld, RgnHandle * imageRgn, WEHandle hWE )
{
	WEPtr pWE = * hWE ;		//	assume WE record is already locked
	PixMapHandle pixels = nil ;
	Point offset ;
	Rect localBounds ;
	Rect globalBounds ;
	SInt32 dragArea ;
	GDHandle saveDevice ;
	CGrafPtr savePort ;
	GrafPtr textPort ;
	Boolean wasInactive = false ;
	OSErr err ;

	//	init return values
	* imageGWorld = nil ;
	* imageRgn = nil ;

	//	save current graphics world
	GetGWorld ( & savePort, & saveDevice ) ;

	//	set up the port
	textPort = pWE -> port ;
	SetPort ( textPort ) ;

	//	calculate delta between global coords and local coords
	offset . v = 0 ;
	offset . h = 0 ;
	LocalToGlobal ( & offset ) ;

	//	get image region (in local coordinates)
	if ( ( * imageRgn = WEGetHiliteRgn ( pWE -> selStart, pWE -> selEnd, hWE ) ) == nil )
	{
		err = memFullErr ;
		goto cleanup ;
	}

	//	get bounding rectangle of image region
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	GetRegionBounds( *imageRgn, & localBounds );
#else
	localBounds = ( ** imageRgn ) -> rgnBBox ;
#endif

	//	translate to global coordinates
	globalBounds = localBounds ;
	OffsetRect ( & globalBounds, offset . h, offset . v ) ;

	//	calculate drag region area
	dragArea = ( ( SInt32 ) globalBounds . right - globalBounds . left ) *
			   ( ( SInt32 ) globalBounds . bottom - globalBounds . top ) ;

	//	if translucent drags are available and drag region area
	//	is reasonably small, prepare a drag image
	if ( ( dragArea <= pWE -> translucencyThreshold ) && BTST ( pWE -> flags, weFHasTranslucentDrags ) )
	{

		//	create an 8-bit deep gworld for drawing the drag image
		if ( ( err = NewGWorld ( imageGWorld, 8, & globalBounds, nil, nil, useTempMem ) ) == noErr )
		{
			//	get the pixmap from the gworld
			pixels = GetGWorldPixMap ( * imageGWorld ) ;

			//	set up the gworld
			SetGWorld ( * imageGWorld, nil ) ;
			LockPixels ( pixels ) ;
			SetOrigin ( localBounds . left, localBounds . top ) ;

			//	temporarily set weFActive so we'll draw the full highlighting
			if ( ! BTST ( pWE -> flags, weFActive ) )
			{
				wasInactive = true ;
				BSET ( pWE -> flags, weFActive ) ;
			}

			//	draw the selection into the gworld
			EraseRect ( & localBounds ) ;
			pWE -> port = ( GrafPtr ) * imageGWorld ;
			WEUpdate ( * imageRgn, hWE ) ;
			pWE -> port = textPort ;

			//	restore original setting of weFActive
			if ( wasInactive )
			{
				BCLR ( pWE -> flags, weFActive ) ;
			}

			//	end drawing
			UnlockPixels ( pixels ) ;
			SetOrigin ( 0, 0 ) ;
		}
	}

	//	convert the image region to global coordinates
	OffsetRgn ( * imageRgn, offset . h, offset . v ) ;

	if ( pixels != nil )
	{
		//	convert the image itself to global coordinates
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
		Rect	imageRgnBBox;
		
		GetRegionBounds( * imageRgn, & imageRgnBBox );
		offset = topLeft ( imageRgnBBox );
#else
		offset = topLeft ( ( ** imageRgn ) -> rgnBBox ) ;
#endif
		OffsetRect ( & ( * pixels ) -> bounds, offset . h, offset . v ) ;
	}

	//	clear result code
	err = noErr ;

cleanup :
	//	restore original graphics world
	SetGWorld ( savePort, saveDevice ) ;

	//	return result code
	return err ;
}

#endif	// WASTE_TRANSLUCENT_DRAGS

pascal OSErr _WEDrag ( Point mouseLoc, EventModifiers modifiers, UInt32 clickTime, WEHandle hWE )
{
	WEPtr pWE = * hWE ;		//	assume WE record is already locked
	DragReference drag = kNullDrag ;
	RgnHandle dragRgn = nil ;
	RgnHandle dragOutline = nil ;
	EventRecord event ;
	Rect dragBounds ;
	Point tempPoint ;
	GrafPtr savePort ;
	OSErr err ;
#if WASTE_OBJECTS
	WEObjectDescHandle hObjectDesc ;
#endif
#if WASTE_TRANSLUCENT_DRAGS
	GWorldPtr dragGWorld = nil ;
#endif

	// set up the port
	GetPort ( & savePort ) ;
	SetPort ( pWE -> port ) ;

	// turn the cursor into an arrow
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	{
	static Cursor arrow;

	GetQDGlobalsArrow ( &arrow );
	SetCursor ( &arrow ) ;
	}
#else
	SetCursor ( & qd . arrow ) ;
#endif

	// fabricate an EventRecord for TrackDrag
	event . what = mouseDown ;
	event . message = 0 ;
	event . when = clickTime ;
	event . where = mouseLoc ;
	LocalToGlobal ( & event . where ) ;
	event . modifiers = modifiers ;

	// to start the drag, the user must move the mouse a certain distance
	// away from the initial mouse location; this allows a short click in
	// the selection area to set the insertion point
	err = weNoDragErr ;
	if ( ! WaitMouseMoved ( event . where ) )
	{
		goto cleanup ;
	}

	// create a drag object
	if ( ( err = NewDrag ( & drag ) ) != noErr )
	{
		goto cleanup ;
	}

#if WASTE_OBJECTS
	// if the selection range consists of an embedded object,
	// then use its object type as flavor type
	if ( WEGetSelectedObject ( & hObjectDesc, hWE ) == noErr )
	{
		FlavorType theType ;
		Handle theData = nil ;
		Boolean disposeData ;

		if ( ( err = _WEStreamObject ( weToDrag, & theType, & theData, & disposeData, hObjectDesc ) ) != noErr )
		{
			goto cleanup ;
		}

		if ( ( err = AddDragItemFlavor ( drag, ( ItemReference ) hWE, theType, nil, 0, 0 ) ) != noErr )
		{
			goto cleanup ;
		}
	}
	else
#endif
	{

		// add a 'TEXT' flavor to the drag
		if ( ( err = AddDragItemFlavor ( drag, ( ItemReference ) hWE, kTypeText, nil, 0, 0 ) ) != noErr )
		{
			goto cleanup ;
		}

		// add a 'styl' flavor to the drag
		if ( ( err = AddDragItemFlavor ( drag, ( ItemReference ) hWE, kTypeStyles, nil, 0, 0 ) ) != noErr )
		{
			goto cleanup ;
		}

		// add a 'FISH' flavor to the drag
		if ( ( err = AddDragItemFlavor ( drag, ( ItemReference ) hWE, kTypeFontTable, nil, 0, 0 ) ) != noErr )
		{
			goto cleanup ;
		}

#if WASTE_OBJECTS
		// add a 'SOUP' flavor to the drag
		if ( ( err = AddDragItemFlavor ( drag, ( ItemReference ) hWE, kTypeSoup, nil, 0, 0 ) ) != noErr )
		{
			goto cleanup ;
		}
#endif

	}

	// since we didn't provide the flavor data for any of the above flavors,
	// we need supply a data send callback
	if ( _weFlavorSender == nil )
	{
		_weFlavorSender = NewDragSendDataUPP ( ( DragSendDataProcPtr ) _WESendFlavor ) ;
	}

	if ( ( err = SetDragSendProc ( drag, _weFlavorSender, 0 ) ) != noErr )
	{
		goto cleanup ;
	}

#if WASTE_TRANSLUCENT_DRAGS
	//	get drag image & region
	if ( ( err = _WEMakeDragImage( & dragGWorld, & dragRgn, hWE ) ) != noErr )
	{
		goto cleanup ;
	}
#else
	//	get drag region (in local coordinates)
	if ( ( dragRgn = WEGetHiliteRgn ( pWE -> selStart, pWE -> selEnd, hWE ) ) == nil )
	{
		err = memFullErr ;
		goto cleanup ;
	}

	//	convert drag region to global coordinates
	tempPoint . v = 0 ;
	tempPoint . h = 0 ;
	LocalToGlobal ( & tempPoint ) ;
	OffsetRgn ( dragRgn, tempPoint . h, tempPoint . v ) ;
#endif

	//	set the bounds of the drag
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	GetRegionBounds( dragRgn, &dragBounds );
#else
	dragBounds = ( * dragRgn ) -> rgnBBox ;
#endif
	if ( ( err = SetDragItemBounds ( drag, ( ItemReference ) hWE, & dragBounds ) ) != noErr )
	{
		goto cleanup ;
	}

#if WASTE_TRANSLUCENT_DRAGS
	//	set drag image, if any
	if ( dragGWorld != nil )
	{
		tempPoint . v = 0 ;
		tempPoint . h = 0 ;
		SetDragImage ( drag, GetGWorldPixMap ( dragGWorld ), dragRgn, tempPoint, kDragStandardTranslucency ) ;
	}
#endif

	// create an outline region for TrackDrag
	dragOutline = _WEOutlineRgn ( dragRgn ) ;

	//	call the pre-TrackDrag hook, if any
	if ( pWE -> preTrackDragHook != nil )
	{
		if ( ( err = CallWEPreTrackDragProc ( drag, hWE, pWE -> preTrackDragHook ) ) != noErr )
		{
			goto cleanup ;
		}
	}

	// stash drag reference in currentDrag so WETrackDrag and WEReceiveDrag
	// can tell whether a given drag originated from this WE instance
	pWE -> currentDrag = drag ;

	// track the drag
	err = TrackDrag ( drag, & event, dragOutline ) ;
	pWE -> currentDrag = kNullDrag ;
	if (err != noErr)
	{
		goto cleanup ;
	}

	// if the selection was dragged to the trash and the option key wasn't held down
	// and if the instance is editable, delete the selection
	if ( ! BTST ( pWE -> features, weFReadOnly ) )
	{
		if ( WEDraggedToTrash ( drag ) )
		{
			if ( ! _WEIsOptionDrag ( drag ) )
			{
				if ( ( err = WEDelete ( hWE ) ) != noErr )
				{
					goto cleanup ;
				}
			}
		}
	}

	// clear result code
	err = noErr ;

cleanup:
	// dispose of the drag
	if ( drag != kNullDrag )
	{
		DisposeDrag ( drag ) ;
	}

#if WASTE_TRANSLUCENT_DRAGS
	//	dispose of the image gworld
	if ( dragGWorld != nil )
	{
		DisposeGWorld ( dragGWorld ) ;
	}
#endif

	//	dispose of auxiliary regions
	if ( dragRgn != nil )
	{
		DisposeRgn ( dragRgn ) ;
	}
	if ( dragOutline != nil )
	{
		DisposeRgn ( dragOutline ) ;
	}

	// restore the port
	SetPort ( savePort ) ;

	// return result code
	return err;
}

#endif	// WASTE_DRAG_AND_DROP

pascal Boolean WECanAcceptDrag ( DragReference drag, WEHandle hWE )
{
#if WASTE_DRAG_AND_DROP
	WEPtr pWE ;
	UInt16 numDragItems ;
	UInt16 dragItemIndex ;
	ItemReference dragItem ;
	Boolean saveWELock ;
	OSErr err ;
	Boolean retval = false ;

	// lock the WE record
	saveWELock = _WESetHandleLock ( ( Handle ) hWE, true ) ;
	pWE = * hWE ;

	// refuse all drags if the weFReadOnly feature is enabled
	if ( BTST ( pWE -> features, weFReadOnly ) )
	{
		goto cleanup ;
	}

	// count items in this drag
	if ( ( err = CountDragItems ( drag, & numDragItems ) ) != noErr )
	{
		goto cleanup ;
	}

	for ( dragItemIndex = 1 ; dragItemIndex <= numDragItems ; dragItemIndex ++ )
	{
		// get item reference number for current drag item
		if ( ( err = GetDragItemReferenceNumber ( drag, dragItemIndex, & dragItem ) ) != noErr )
		{
			goto cleanup ;
		}

		// see if this drag item contains a text flavor
		err = _WEGetFlavor ( drag, dragItem, kTypeText, nil, kInvalidOffset, hWE ) ;

#if WASTE_OBJECTS
		if ( err == badDragFlavorErr )
		{
			SInt32 objectIndex ;
			FlavorType objectType ;

			// see if this drag item contains a flavor matching one of the registered object types
			for ( objectIndex = 0 ;
				  _WEGetIndObjectType ( objectIndex, & objectType, hWE ) == noErr ;
				  objectIndex ++ )
			{
				if ( ( err = _WEGetFlavor(drag, dragItem, objectType, nil, kInvalidOffset, hWE ) ) != badDragFlavorErr )
				{
					break ;
				}
			}
		}
#endif	// WASTE_OBJECTS

		if ( err != noErr )
		{
			goto cleanup ;
		}
	} // for

	retval = true;

cleanup:

	// unlock the WE record
	_WESetHandleLock ( ( Handle ) hWE, saveWELock ) ;

	return retval ;
#else
	#pragma unused(drag, hWE)
	return false ;
#endif	// WASTE_DRAG_AND_DROP
}

pascal OSErr WETrackDrag ( DragTrackingMessage message, DragReference drag, WEHandle hWE )
{
#if WASTE_DRAG_AND_DROP
	WEPtr pWE ;
	DragAttributes attributes ;
	Point mouse ;
	LongPt thePoint ;
	SInt32 offset ;
	WEEdge edge ;
	Boolean saveWELock ;
	OSErr err ;

	// lock the WE record
	saveWELock = _WESetHandleLock ( ( Handle ) hWE, true ) ;
	pWE = * hWE ;

	// dispatch on message
	switch ( message )
	{
		case kDragTrackingEnterWindow :
		{
			// determine whether we can accept this drag
			if ( WECanAcceptDrag ( drag, hWE ) )
			{
				BSET ( pWE -> flags, weFCanAcceptDrag ) ;
			}
			else
			{
				BCLR ( pWE -> flags, weFCanAcceptDrag ) ;
			}

			// reset clickTime
			pWE -> clickTime = 0 ;
			break ;
		}

		case kDragTrackingInWindow :
		{
			if ( BTST ( pWE -> flags, weFCanAcceptDrag ) )
			{

				// get drag attributes
				if ( ( err = GetDragAttributes ( drag, & attributes ) ) != noErr )
				{
					goto cleanup;
				}

				// get current mouse location in local coordinates
				if ( ( err = GetDragMouse ( drag, & mouse, nil ) ) != noErr )
				{
					goto cleanup;
				}
				GlobalToLocal ( & mouse ) ;

				if ( PtInRgn ( mouse, pWE -> viewRgn ) )
				{
					// mouse is in text area
					// hilite the text rectangle, if we haven't already
					// and if the drag has left sender window since drag tracking started
					if ( ( ! BTST ( pWE -> flags, weFHilited ) ) &&
						 ( attributes & kDragHasLeftSenderWindow ) )
					{
						CallWEHiliteDropAreaProc ( drag, true, hWE, pWE -> hiliteDropAreaHook ) ;
						BSET ( pWE -> flags, weFHilited ) ;
					}

					// hide the regular caret
					if ( BTST ( pWE -> flags, weFCaretVisible ) )
					{
						_WEBlinkCaret ( hWE ) ;
					}

					// get text offset corresponding to mouse location
					WEPointToLongPoint ( mouse, & thePoint ) ;
					offset = WEGetOffset ( & thePoint, & edge, hWE ) ;

					// if offset is within the original selection range, don't display drag feedback
					if ( drag == pWE -> currentDrag )
					{
						if ( _WEOffsetInRange ( offset, edge, pWE -> selStart, pWE -> selEnd ) )
						{
							offset = kInvalidOffset ;
						}
					}

					// provide a drag feedback in the form of a blinking caret
					_WEUpdateDragCaret ( offset, hWE ) ;

					// clear clickTime
					pWE -> clickTime = 0 ;
				}
				else
				{

					// mouse is outside text area
					// dehilite the text rectangle, if it's hilited
					if ( BTST ( pWE -> flags, weFHilited ) )
					{
						CallWEHiliteDropAreaProc ( drag, false, hWE, pWE->hiliteDropAreaHook ) ;
						BCLR ( pWE -> flags, weFHilited ) ;
					}

					// hide the drag caret, if it's showing
					_WEUpdateDragCaret ( kInvalidOffset, hWE ) ;

					// if the mouse has been remaining outside the view region for 10 ticks or more
					// and this drag was created by this WE instance, call the click loop routine
					if ( drag == pWE->currentDrag )
					{
						UInt32 currentTime = TickCount ( ) ;
						if ( pWE -> clickTime == 0 )
						{
							pWE -> clickTime = currentTime ;
						}
						else if ( currentTime > pWE -> clickTime + kAutoScrollDelay )
						{
							if ( pWE -> clickLoop != nil )
							{
								CallWEClickLoopProc ( hWE, pWE -> clickLoop ) ;
							}
						}
					}
				}
			}
			break ;
		}

		case kDragTrackingLeaveWindow :
		{

			// drag has left this window
			// dehilite the text area if necessary
			if ( BTST ( pWE -> flags, weFHilited ) )
			{
				CallWEHiliteDropAreaProc ( drag, false, hWE, pWE->hiliteDropAreaHook ) ;
				BCLR ( pWE -> flags, weFHilited ) ;
			}

			// hide the drag caret, if it's showing
			_WEUpdateDragCaret ( kInvalidOffset, hWE ) ;
			break;
		}

		default:
		{
			break;
		}
	}

	// clear result code
	err = noErr ;

cleanup:
	// unlock the WE record
	_WESetHandleLock ( ( Handle ) hWE, saveWELock ) ;

	// return result code
	return err ;
#else
	#pragma unused(message, drag, hWE)
	return noErr ;
#endif
}

pascal OSErr WEReceiveDrag(DragReference drag, WEHandle hWE)
{
#if WASTE_DRAG_AND_DROP
	WEPtr pWE;
	WEActionHandle hAction;
	Handle hText = nil;
	Handle hStyles = nil;
	Handle hFontTable = nil;
#if WASTE_OBJECTS
	Handle hSoup = nil;
#endif
	Handle hObjectData = nil;
	Point mouse;
	LongPt dropLocation;
	SInt32 insertionOffset;
	SInt32 insertionLength;
	SInt32 sourceStart;
	SInt32 sourceEnd;
	SInt32 destStart;
	SInt32 destEnd = 0;
	SInt32 delta = 0;
	UInt16 dragItemIndex;
	UInt16 numDragItems;
	ItemReference dragItem;
	GrafPtr savePort;
	SInt16 intPasteAction;
	WEEdge dropEdge;
	char space = kSpace;
	Boolean isMove = false;
	Boolean isBackwards = false;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// set up the port
	GetPort(&savePort);
	SetPort(pWE->port);

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	// hide the drag caret
	_WEUpdateDragCaret(kInvalidOffset, hWE);

	// refuse this drag if it doesn't taste good
	err = badDragFlavorErr;
	if (!WECanAcceptDrag(drag, hWE))
	{
		goto cleanup;
	}

	// get drop location in local coordinates
	if ((err = GetDragMouse(drag, &mouse, nil)) != noErr)
	{
		goto cleanup;
	}
	GlobalToLocal(&mouse);

	// for the drag to be accepted, the drop location must be within the view region
	err = dragNotAcceptedErr;
	if (!PtInRgn(mouse, pWE->viewRgn))
	{
		goto cleanup;
	}

	// get drop offset into the text
	WEPointToLongPoint(mouse, &dropLocation);
	insertionOffset = WEGetOffset(&dropLocation, &dropEdge, hWE);
	// destStart/destEnd define the range to highlight at the end of the drag
	destStart = insertionOffset;

	// drag originated from this same window?
	if (drag == pWE->currentDrag)
	{

		// sourceStart/sourceEnd define the range to delete at the end of the move
		sourceStart = pWE->selStart;
		sourceEnd = pWE->selEnd;

		// remember text length before insertion
		delta = pWE->textLength;

		// if insertion offset is within the original selection range, abort the drag
		// (*err = dragNotAcceptedErr;*)
		if (_WEOffsetInRange(insertionOffset, dropEdge, sourceStart, sourceEnd))
		{
			goto cleanup;
		}

		// if the drag originated from this window, a move,
		// rather than a copy, should be performed
		// Exception: the option key may be held down at mouse-down
		// or mouse-up time to force a copy operation.

		isMove = !_WEIsOptionDrag(drag);
		isBackwards = (insertionOffset <= sourceStart);
	} // if intra-window drag

	// clear null style
	BCLR(pWE->flags, weFUseNullStyle);

	// hide selection highlighting
	_WEHiliteRange(pWE->selStart, pWE->selEnd, hWE);

	// increment modification count
	pWE->modCount++;

	// if undo support is enabled, create a new action so we'll be able to undo the insertion
	if (BTST(pWE->features, weFUndoSupport))
	{
		WEClearUndo(hWE);
		if (_WENewAction(insertionOffset, insertionOffset, 0, weAKDrag, 0, hWE, &hAction) == noErr)
		{
			_WEPushAction(hAction);
		}
	}

	// count items in this drag
	if ((err = CountDragItems(drag, &numDragItems)) != noErr)
	{
		goto cleanup;
	}

	for (dragItemIndex = 1; dragItemIndex<=numDragItems; dragItemIndex++)
	{
		// get item reference number for current drag item
		if ((err = GetDragItemReferenceNumber(drag, dragItemIndex, &dragItem)) != noErr)
		{
			goto cleanup;
		}

		// see if this drag item contains a text flavor
		err = _WEExtractFlavor(drag, dragItem, kTypeText, &hText, insertionOffset, hWE);
		if (err == noErr)
		{
			if (!BTST(pWE->features, weFMonoStyled))
			{
				// extract accompanying styles, if any
				err = _WEExtractFlavor(drag, dragItem, kTypeStyles, &hStyles, insertionOffset, hWE);
				if ((err != noErr) && (err != badDragFlavorErr))
				{
					goto cleanup;
				}
				if (hStyles != nil)
				{
					//	the styles may be further accompanied by a font table...
					err = _WEExtractFlavor(drag, dragItem, kTypeFontTable, &hFontTable,
							insertionOffset, hWE);
					if ((err != noErr) && (err != badDragFlavorErr))
					{
						goto cleanup;
					}
				}
#if WASTE_OBJECTS
				//	extract the object soup, if any
				err = _WEExtractFlavor(drag, dragItem, kTypeSoup, &hSoup, insertionOffset, hWE);
				if ((err != noErr) && (err != badDragFlavorErr))
				{
					goto cleanup;
				}
#endif
			}

			// any extra space added because of intelligent cut-and-paste rules will use the
			// style attributes set at the insertion point
			if (dragItemIndex == 1)
			{
				pWE->selStart = insertionOffset;
				pWE->selEnd = insertionOffset;
				_WESynchNullStyle(hWE);
			}

			// get text length
			insertionLength = GetHandleSize(hText);
			destEnd = insertionOffset + insertionLength;

			// insert the new text at the insertion point
			HLock(hText);
			err = _WEInsertText(insertionOffset, *hText, insertionLength, hWE);
			_WEForgetHandle(&hText);
			if (err != noErr)
			{
				goto cleanup;
			}

			// adjust deletion range length in undo buffer
			_WEAdjustUndoRange(insertionLength, hWE);

			if (hStyles != nil)
			{
				if (hFontTable != nil)
				{
					Boolean wasChanged = false;

					// if a font table is available, see whether we should re-map font IDs
					if ((WEUpdateFontTable(hFontTable, nil, &wasChanged) == noErr) && wasChanged)
					{
						WEUpdateStyleScrap((StScrpHandle) hStyles, hFontTable);
					}
				}

				// apply the accompanying styles
				if ((err = _WEApplyStyleScrap(insertionOffset, destEnd,
							(myStScrpHandle) hStyles, hWE)) != noErr)
				{
					goto cleanup;
				}
				_WEForgetHandle(&hStyles);
			}

#if WASTE_OBJECTS
			// apply the accompanying soup, if any
			if (hSoup != nil)
			{
				if ((err = _WEApplySoup(insertionOffset, hSoup, hWE)) != noErr)
				{
					goto cleanup;
				}
				_WEForgetHandle(&hSoup);
			}
#endif

			// determine whether an extra space should be added before or after the inserted text
			intPasteAction = _WEIntelligentPaste(insertionOffset, destEnd, hWE);

			// add the extra space, if necessary
			if (intPasteAction != weDontAddSpaces)
			{
				if (intPasteAction == weAddSpaceOnLeftSide)
				{
					if ((err = _WEInsertText(insertionOffset, &space, sizeof(space), hWE)) != noErr)
					{
						goto cleanup;
					}

					destEnd++;

					// if an extra space is inserted in front of all dropped items,
					// don't count it when eventually highlighting the destination range
					if (dragItemIndex == 1)
					{
						destStart++;
					}
				}
				else
				{
					if ((err = _WEInsertText(destEnd, &space, sizeof(space), hWE)) != noErr)
					{
						goto cleanup;
					}
				}

				insertionLength++;
				_WEAdjustUndoRange(1, hWE);
			} // if extra space

		}

#if WASTE_OBJECTS
		else if (err == badDragFlavorErr)
		{
			SInt32 objectIndex;
			FlavorType objectType;
			Point objectSize;
			SInt16 saveUndoSupport;
			SInt16 saveInhibitRecal;

			objectSize.v = 0;
			objectSize.h = 0;

			// no text flavor: there must be a flavor matching one of the registered object types
			objectIndex = 0;
			while (_WEGetIndObjectType(objectIndex, &objectType, hWE) == noErr)
			{
				err = _WEExtractFlavor(drag, dragItem, objectType, &hObjectData, insertionOffset, hWE);
				if (err == noErr)
				{
					break;	 // enclosing while
				}
				if (err != badDragFlavorErr)
				{
					goto cleanup;
				}
				objectIndex++;
			} // while
			if (err != noErr)
			{
				goto cleanup;
			}

			// set insertion point on first iteration (*after* extracting flavors, in case we are
			// doing an intra-window move, otherwise our send proc would be confused)
			if (dragItemIndex == 1)
			{
				pWE->selStart = insertionOffset;
				pWE->selEnd = insertionOffset;
			}

			// insert the object, but without touching undo or redrawing the text
			saveUndoSupport = WEFeatureFlag(weFUndoSupport, weBitClear, hWE);
			saveInhibitRecal = WEFeatureFlag(weFInhibitRecal, weBitSet, hWE);
			err = WEInsertObject(objectType, hObjectData, objectSize, hWE);
			WEFeatureFlag(weFUndoSupport, saveUndoSupport, hWE);
			WEFeatureFlag(weFInhibitRecal, saveInhibitRecal, hWE);
			if (err != noErr)
			{
				goto cleanup;
			}

			insertionLength = 1;
			destEnd = insertionOffset + 1;
			pWE->modCount--; // compensate for increment made by WEInsertObject
			_WEAdjustUndoRange(1, hWE);
		}
#endif	// WASTE_OBJECTS

		else
			goto cleanup;

		// advance insertion offset for subsequent drag items, if any
		insertionOffset += insertionLength;

	} // for

	if (isMove)
	{
		// adjust source range
		if (isBackwards)
		{
			delta -= pWE->textLength;
			sourceStart -= delta;
			sourceEnd -= delta;
		}

		// extend range according to intelligent cut-and-paste rules
		_WEIntelligentCut(&sourceStart, &sourceEnd, hWE);

		// increment mod count
		pWE->modCount++;

		// if undo support is enabled, create a new action so we'll be able to undo the deletion
		if (BTST(pWE->features, weFUndoSupport))
		{
			if (_WENewAction(sourceStart, sourceEnd, 0, weAKDrag, 0, hWE, &hAction) == noErr)
			{
				_WEPushAction(hAction);
			}
		}

		// delete source range
		delta = pWE->textLength;
		if ((err = _WEDeleteRange(sourceStart, sourceEnd, hWE)) != noErr)
		{
			goto cleanup;
		}

		// adjust destination range
		if (!isBackwards)
		{
			delta -= pWE->textLength;
			destStart -= delta;
			destEnd -= delta;
		}

	} // if isMove

	// select the range encompassing all items dropped
	pWE->selStart = destStart;
	pWE->selEnd = destEnd;

	// redraw the text
	if (isMove)
	{
		if (sourceStart < destStart)
		{
			err = _WERedraw(sourceStart, destEnd, hWE);
		}
		else
		{
			err = _WERedraw(destStart, sourceEnd, hWE);
		}
	}
	else
	{
		err = _WERedraw(destStart, destEnd, hWE);
	}

cleanup:
	// dispose of temporary handles
	_WEForgetHandle(&hText);
	_WEForgetHandle(&hStyles);
	_WEForgetHandle(&hFontTable);
#if WASTE_OBJECTS
	_WEForgetHandle(&hSoup);
#endif

	// restore the port
	SetPort(savePort);

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
#else
	#pragma unused(drag, hWE)
	return dragNotAcceptedErr ;
#endif	// WASTE_DRAG_AND_DROP
}

pascal Boolean WEDraggedToTrash(DragReference drag)
{
#if WASTE_DRAG_AND_DROP
	// return true if the drop location of the specified drag is the trash

	const int bDirectoryAttr = 4;

	AEDesc dropLocation, coercedDropLocation;
	CInfoPBRec pb;
	FSSpecPtr pSpec;
	SInt16 trashVRefNum;
	SInt32 trashDirID;
	Boolean draggedToTrash;

	draggedToTrash = false;
	dropLocation.dataHandle = nil;
	coercedDropLocation.dataHandle = nil;

	// get drop location
	if (GetDropLocation(drag, &dropLocation) != noErr)
	{
		goto cleanup;
	}

	// do nothing if dropLocation is a null descriptor
	if (dropLocation.descriptorType == typeNull)
	{
		goto cleanup;
	}

	// try to coerce the descriptor to a file system specification record
	if (AECoerceDesc(&dropLocation, typeFSS, &coercedDropLocation) != noErr)
	{
		goto cleanup;
	}

	// lock the data handle of the coerced descriptor
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON
	if (AEGetDescData(&coercedDropLocation, &pSpec, sizeof(pSpec)))
	{
		goto cleanup;
	}
#else
	HLock(coercedDropLocation.dataHandle);
	pSpec = *(FSSpecHandle)coercedDropLocation.dataHandle;
#endif

	// determine the directory ID of the drop location (assuming it's a folder!)
	BLOCK_CLR(pb);
	pb.hFileInfo.ioVRefNum = pSpec->vRefNum;
	pb.hFileInfo.ioDirID = pSpec->parID;
	pb.hFileInfo.ioNamePtr = pSpec->name;
	if (PBGetCatInfoSync(&pb) != noErr)
	{
		goto cleanup;
	}

	// make sure the specified file system object is really a directory
	if (!BTST(pb.hFileInfo.ioFlAttrib, bDirectoryAttr))
	{
		goto cleanup;
	}

	// find the directory ID of the trash folder
	if (FindFolder(pSpec->vRefNum, kTrashFolderType, kDontCreateFolder, &trashVRefNum, &trashDirID) != noErr)
	{
		goto cleanup;
	}

	// compare the two directory IDs: if they're the same, the drop location is the trash
	if (pb.dirInfo.ioDrDirID == trashDirID)
	{
		draggedToTrash = true;
	}

cleanup:
	// clean up
	AEDisposeDesc(&dropLocation);
	AEDisposeDesc(&coercedDropLocation);

	return draggedToTrash;
#else
	#pragma unused(drag)
	return false ;
#endif
}

#if WASTE_IC_SUPPORT

pascal void _WEResolveURL(EventModifiers modifiers, SInt32 urlStart, SInt32 urlEnd, WEHandle hWE)
{
	WEPtr pWE = *hWE;			// assume WE record is already locked
	Str255 hint;
	FourCharCode signature;
	ProcessSerialNumber psn;
	ProcessInfoRec info;
	ICInstance inst;
#if defined(OLDROUTINENAMES) && OLDROUTINENAMES
	ICError err;
#else
	OSStatus err;
#endif
#if (UNIVERSAL_INTERFACES_VERSION < 0x0300)
	SInt32 junkLong;
#else
	UInt32 junkLong;
#endif
	Handle hURL = nil;
	Boolean saveTextLock;

	// get the hint string IC will use to parse slack URLs
	hint[0] = 0;
	if (pWE->hURLHint != nil)
	{
		BlockMoveData(*pWE->hURLHint, hint, StrLength(*pWE->hURLHint) + 1);
	}

	// find out the signature of the current process
	signature = '\?\?\?\?';
	psn.highLongOfPSN = 0;
	psn.lowLongOfPSN = 0;
	if (GetCurrentProcess(&psn) == noErr)
	{
		BLOCK_CLR(info);
		info.processInfoLength = sizeof(info);
		if (GetProcessInformation(&psn, &info) == noErr)
		{
			signature = info.processSignature;
		}
	}

	if (ICStart(&inst, signature) == noErr)
	{
		{
			saveTextLock = _WESetHandleLock(pWE->hText, true);

			// cmd + option click just highlights the URL,
			// without actually resolving it

//ShadowIRC: Modified so option key doesn't do the hilighting, since we use it for something else.
/*
			if (modifiers & optionKey)
			{
				hURL = NewHandle(0);
				err = ICParseURL(inst, hint, *pWE->hText, WEGetTextLength(hWE), &urlStart, &urlEnd, hURL);
				_WEForgetHandle(&hURL);
			}
			else
*/
			{
				err = ICLaunchURL(inst, hint, *pWE->hText, WEGetTextLength(hWE), &urlStart, &urlEnd);
			}
			_WESetHandleLock(pWE->hText, saveTextLock);
			WESetSelection(urlStart, urlEnd, hWE);

			// flash selection if successful (unless option key was down)
			if (err == noErr)
			{
				if(!(modifiers & optionKey))
				{
					Delay(5, &junkLong);
					WEDeactivate(hWE);
					Delay(5, &junkLong);
					WEActivate(hWE);
				}
				else
				{
					WESetSelection(urlStart, urlStart, hWE);
				}
			}
		}
		ICStop(inst);
	}
}

#endif	// WASTE_IC_SUPPORT

pascal void WEClick(Point mouseLoc, EventModifiers modifiers, UInt32 clickTime, WEHandle hWE)
{
	WEPtr pWE;
	LongPt thePoint;
	SInt32 offset, anchor;
	SInt32 rangeStart, rangeEnd;
	WEEdge edge;
	Boolean isMultipleClick;
	Boolean saveWELock;
#if WASTE_OBJECTS
	WEObjectDescHandle hObjectDesc;
#endif
#if WASTE_IC_SUPPORT
	SInt32 urlStart, urlEnd;
#endif

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

#if WASTE_IC_SUPPORT
	// remember the selection range before the click
	urlStart = pWE->selStart;
	urlEnd = pWE->selEnd;
#endif

	// hide the caret if it's showing
	if (BTST(pWE->flags, weFCaretVisible))
	{
		_WEBlinkCaret(hWE);
	}

	// find click offset
	WEPointToLongPoint(mouseLoc, &thePoint);
	offset = WEGetOffset(&thePoint, &edge, hWE);

	// determine whether this click is part of a sequence
	// a single click inside an object selects it, so it's like a double click in a word
	isMultipleClick = ((clickTime < pWE->clickTime + GetDblTime()) && (offset == pWE->clickLoc));

	// remember click time, click offset and edge value
	pWE->clickTime = clickTime;
	pWE->clickLoc = offset;
	pWE->clickEdge = edge;

#if WASTE_OBJECTS
	// when selected, embedded objects can intercept mouse clicks
	if (WEGetSelectedObject(&hObjectDesc, hWE) == noErr)
	{
		if (_WEOffsetInRange(offset, edge, pWE->selStart, pWE->selEnd))
		{
			if (_WEClickObject(mouseLoc, modifiers + isMultipleClick, clickTime, hObjectDesc))
			{
					goto cleanup;
			}
		}
	}
#endif

	if ((modifiers & shiftKey) == 0)
	{

		// is this click part of a sequence or is it a single click?
		if (isMultipleClick)
		{
			pWE->clickCount++;

			// a double (triple) click creates an anchor-word (anchor-line)
			if (pWE->clickCount > 1)
			{
				WEFindLine(offset, edge, &pWE->anchorStart, &pWE->anchorEnd, hWE);
			}
			else
			{
				WEFindWord(offset, edge, &pWE->anchorStart, &pWE->anchorEnd, hWE);
			}

			offset = pWE->anchorStart;
		}
		else
		{
			// single-click

#if WASTE_DRAG_AND_DROP
			// if the Drag Manager is available and the click went in the selection range,
			// this click may be the beginning of a drag gesture
			if (BTST(pWE->flags, weFHasDragManager) && BTST(pWE->features, weFDragAndDrop))
			{
				if (_WEOffsetInRange(offset, edge, pWE->selStart, pWE->selEnd))
				{
					if (_WEDrag(mouseLoc, modifiers, clickTime, hWE) != weNoDragErr)
					{
						goto cleanup;
					}
				}
			}
#endif

			pWE->clickCount = 0;
			anchor = offset;
		}
	}
	else
	{

		// if the shift key was down, use the old anchor offset found with the previous click
		anchor = BTST(pWE->flags, weFAnchorIsEnd) ? pWE->selEnd : pWE->selStart;
	}

	// set the weFMouseTracking bit while we track the mouse
	BSET(pWE->flags, weFMouseTracking);

	// MOUSE TRACKING LOOP
	do
	{

		// get text offset corresponding to mouse position
		WEPointToLongPoint(mouseLoc, &thePoint);
		offset = WEGetOffset(&thePoint, &edge, hWE);

		// if we're selecting words or lines, pin offset to a word or line boundary
		if (pWE->clickCount > 0)
		{
			if (pWE->clickCount > 1)
			{
				WEFindLine(offset, edge, &rangeStart, &rangeEnd, hWE);
			}
			else
			{
				WEFindWord(offset, edge, &rangeStart, &rangeEnd, hWE);
			}

			// choose the word/line boundary and the anchor that are farthest away from each other
			if (offset > pWE->anchorStart)
			{
				anchor = pWE->anchorStart;
				offset = rangeEnd;
			}
			else
			{
				offset = rangeStart;
				anchor = pWE->anchorEnd;
			}
		}
		else
		{
			// if the point is in the middle of an object, the selection should include it
			if (edge == kObjectEdge)
			{
				offset++;
			}
		}

		// set the selection range from anchor point to current offset
		WESetSelection(anchor, offset, hWE);

		// call the click loop callback, if any
		if (pWE->clickLoop != nil)
		{
			if (!CallWEClickLoopProc(hWE, pWE->clickLoop))
			{
				break;
			}
		}

		// update mouse position
		GetMouse(&mouseLoc);

	} while(WaitMouseUp());

	// clear the weFMouseTracking bit
	BCLR(pWE->flags, weFMouseTracking);

	// redraw the caret immediately if the selection range is empty
	if (anchor == offset)
	{
		_WEBlinkCaret(hWE);
	}

#if WASTE_IC_SUPPORT
	if (modifiers & cmdKey)
	{
		// command+clicking a URL tries to resolve it
		// we normally ask IC to parse the text surrounding the clicked point,
		// but if a selection already existed prior to the click, we pass
		// that to IC rather than forcing a re-parse
		if ((anchor != offset) || (anchor < urlStart) || (anchor > urlEnd))
		{
			urlStart = anchor;
			urlEnd = offset;
		}
		_WEResolveURL(modifiers, urlStart, urlEnd, hWE);
	}
#endif

cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

}

pascal UInt16 WEGetClickCount(WEHandle hWE)
{
	return (*hWE)->clickCount;
}
