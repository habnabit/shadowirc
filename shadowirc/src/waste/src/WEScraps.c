/*
 *	WEScraps.c
 *
 *	WASTE PROJECT
 *  Routines for Manipulating Style Scraps and Object Soups
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

pascal OSErr _WEPrependStyle(Handle hStyleScrap, const WERunInfo *info, SInt32 offsetDelta)
{
	// compare the stylistic attributes in info with the first element of the specified
	// style scrap: if they differ, prepend a new element to the style scrap.
	// in any case, advance all character offsets in the style scrap by offsetDelta

	TEStyleScrap *pScrap;
	TEStyleScrapElement entry;
	SInt16 i;
	OSErr err;

	pScrap = * (TEStyleScrapHandle) hStyleScrap;

	// compare this style info with that stored in the first entry of our private style scrap
	if (!_WEBlockCmp(&pScrap->scrpStyleTab[0].scrpTEAttrs, &info->runAttrs, sizeof(TERunAttributes)))
	{
		// insert a new style scrap entry at the beginning of the style scrap
		entry.scrpStartChar = 0;
		entry.scrpTEAttrs = * (TERunAttributes *) &info->runAttrs;

		if ((err = _WESplice(hStyleScrap, &entry, sizeof(entry), sizeof(SInt16))) != noErr)
		{
			return err;
		}

		pScrap = * (TEStyleScrapHandle) hStyleScrap;

		// increment entry count
		pScrap->scrpNStyles++;

	} // if not _WEBlockCmp

	// update char offsets within the style scrap
	for ( i = pScrap->scrpNStyles - 1; i > 0; i-- )
	{
		pScrap->scrpStyleTab[i].scrpStartChar += offsetDelta;
	}

	return noErr;
}

pascal OSErr _WEAppendStyle(Handle hStyleScrap, const WERunInfo *info, SInt32 offset)
{

	// compare the stylistic attributes in info with the last entry of the specified
	// style scrap: if they differ, append a new entry to the style scrap.

	TEStyleScrap *pScrap;
	TEStyleScrapElement entry;
	OSErr err;

	pScrap = * (TEStyleScrapHandle) hStyleScrap;
	// compare this style info with that stored in the first entry of our private style scrap
	if (!_WEBlockCmp(&pScrap->scrpStyleTab[pScrap->scrpNStyles - 1].scrpTEAttrs,
		&info->runAttrs, sizeof(TERunAttributes)))
	{

		// create a new style scrap entry
		entry.scrpStartChar = offset;
		entry.scrpTEAttrs = * (TERunAttributes *) &info->runAttrs;

		// append it at the end of the style scrap
		if ((err = _WESplice(hStyleScrap, &entry, sizeof(entry), -1)) != noErr)
		{
			return err;
		}

		// increment scrap counter
		pScrap = * (TEStyleScrapHandle) hStyleScrap;
		pScrap->scrpNStyles++;

	} // if not _WEBlockCmp

	// return result code
	return noErr;
}

#if WASTE_OBJECTS

pascal OSErr _WEPrependObject(Handle hSoup, const WERunInfo *info, SInt32 offsetDelta)
{
	// if info describes an embedded object, prepend a new object descriptor,
	// complete with the associated object data, to the specified soup.
	// in any case, advance all character offsets in the soup by offsetDelta

	WEObjectDescHandle hObjectDesc = info->runAttrs.runStyle.tsObject;
	Handle hObjectData = nil;
	FlavorType objectType;
	Boolean disposeData;
	Ptr pSoup;
	WESoup soup;
	Size soupSize, objectDataSize, extraSize;
	OSErr err;

	// get size of existing soup
	soupSize = GetHandleSize(hSoup);

	if (hObjectDesc != nil)
	{
		// create a temporary handle for the streaming callback
		if ((err = _WEAllocate(0, kAllocTemp, &hObjectData)) != noErr)
		{
			goto cleanup;
		}

		// get the object type/data
		if ((err = _WEStreamObject(weToSoup, &objectType, &hObjectData, &disposeData, hObjectDesc)) != noErr)
		{
			goto cleanup;
		}

		// get size of object data
		objectDataSize = GetHandleSize(hObjectData);

		// extraSize is size of descriptor + size of object data
		extraSize = sizeof(soup) + objectDataSize;

		// fill in a soup item
		BLOCK_CLR(soup);
		soup.soupType = objectType;
		soup.soupSize = (*hObjectDesc)->objectSize;
		soup.soupDataSize = objectDataSize;

		// make room for the descriptor and the object data
		if ((err = _WESplice(hSoup, nil, extraSize, 0)) != noErr)
		{
			goto cleanup;
		}

		// insert the new object descriptor at the beginning
		pSoup = *hSoup;
		BlockMoveData(&soup, pSoup, sizeof(soup));

		// copy the object data
		BlockMoveData(*hObjectData, pSoup + sizeof(soup), objectDataSize);
	}
	else
	{
		pSoup = *hSoup;
		extraSize = 0;
	}

	// update char offsets within the soup
	while (soupSize > 0)
	{
		pSoup += extraSize;

		//	we have to use BlockMoveData to access descriptors within
		//	the soup, as they might be odd-aligned (duh!) and that would
		//	cause fatal address errors on 68000 CPUs

		BlockMoveData(pSoup, &soup, sizeof(soup));
		soup.soupOffset += offsetDelta;
		BlockMoveData(&soup, pSoup, sizeof(soup));
		extraSize = sizeof(soup) + soup.soupDataSize;
		soupSize -= extraSize;
	}

	err = noErr;

cleanup:
	if (disposeData)
	{
		_WEForgetHandle(&hObjectData);
	}

	return err;

}

pascal OSErr _WEAppendObject(Handle hSoup, const WERunInfo *info, SInt32 offset)
{

	// if info describes an embedded object, append a new object descriptor,
	// complete with the associated object data, to the specified soup.

	WEObjectDescHandle hObjectDesc = info->runAttrs.runStyle.tsObject;
	Handle hObjectData = nil;
	FlavorType objectType;
	WESoup soup;
	Boolean saveDataLock;
	Boolean disposeData;
	OSErr err;

	if (hObjectDesc != nil)
	{
		// create a temporary handle for the streaming callback
		if ((err = _WEAllocate(0, kAllocTemp, &hObjectData)) != noErr)
		{
			goto cleanup;
		}

		// get the object type/data
		if ((err = _WEStreamObject(weToSoup, &objectType, &hObjectData, &disposeData, hObjectDesc)) != noErr)
		{
			goto cleanup;
		}

		// fill in a soup item
		BLOCK_CLR(soup);
		soup.soupOffset = offset;
		soup.soupType = objectType;
		soup.soupSize = (*hObjectDesc)->objectSize;
		soup.soupDataSize = GetHandleSize(hObjectData);

		// append it to the soup handle
		if ((err = _WESplice(hSoup, &soup, sizeof(soup), -1)) != noErr)
		{
			goto cleanup;
		}

		// append the actual object data to the soup handle
		saveDataLock = _WESetHandleLock(hObjectData, true);
		err = _WESplice(hSoup, *hObjectData, soup.soupDataSize, -1);
		_WESetHandleLock(hObjectData, saveDataLock);
		if (err != noErr)
		{
			goto cleanup;
		}
	} // if object reference is not nil

	err = noErr;

cleanup:
	if (disposeData)
	{
		_WEForgetHandle(&hObjectData);
	}

	return err;

}

#endif

pascal OSErr WECopyRange(SInt32 rangeStart, SInt32 rangeEnd, Handle hText,
					Handle hStyles, Handle hSoup, WEHandle hWE)
{

	// Make a copy of the specified range of text: store the characters in hText
	// and the associated style scrap in hStyles.  The handles are resized as necessary.
	// Specify nil in hText or hStyles if you don't want the corresponding info returned.

	WEPtr pWE;
	TEStyleScrapElement *pEntry = nil;
	SInt32 rangeLength;
	SInt32 firstRun, nRuns, i;
	SInt32 startChar;
	WERunInfo info;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// range-check parameters and reorder them if necessary
	rangeStart = _WEPinInRange(rangeStart, 0, pWE->textLength);
	rangeEnd = _WEPinInRange(rangeEnd, 0, pWE->textLength);
	_WEReorder(&rangeStart, &rangeEnd);
	rangeLength = rangeEnd - rangeStart;

	if (hText != nil)
	{
		// resize the given handle
		SetHandleSize(hText, rangeLength);
		if ((err = MemError()) != noErr)
		{
			goto cleanup;
		}

		// copy the text range
		BlockMoveData(*pWE->hText + rangeStart, *hText, rangeLength);
	}

	// make the soup handle zero-length
	if (hSoup != nil)
	{
		SetHandleSize(hSoup, 0);
		if ((err = MemError()) != noErr)
		{
			goto cleanup;
		}
	}

	if ((hStyles != nil) || (hSoup != nil))
	{
		// count how many style runs there are in the selection range
		firstRun = WEOffsetToRun(rangeStart, hWE);
		nRuns = WEOffsetToRun(rangeEnd - 1, hWE) - firstRun + 1;

		if (hStyles != nil)
		{
			// resize the given style scrap handle and lock it in high heap
			SetHandleSize(hStyles, (nRuns * sizeof(myScrpSTElement)) + sizeof(SInt16));
			if ((err = MemError()) != noErr)
			{
				goto cleanup;
			}
			HLockHi(hStyles);

			// fill in the entry count in the style scrap
			// pin the entry count at 32,767
			(* (TEStyleScrapHandle) hStyles)->scrpNStyles = (nRuns < SHRT_MAX) ? nRuns : SHRT_MAX;

			// get pointer to first style table element
			pEntry = & ((* (TEStyleScrapHandle) hStyles)->scrpStyleTab[0]);
		}

		// loop through every style run in the selection range
		for ( i = 0; i < nRuns; i++ )
		{
			_WEGetIndStyle(firstRun + i, &info, hWE);

			// calculate the start character for this style run, relative to the beginning of the range
			startChar = info.runStart - rangeStart;
			if (startChar < 0)
			{
				startChar = 0;
#if WASTE_OBJECTS
				info.runAttrs.runStyle.tsObject = nil;
#endif
			}
			if (hStyles != nil)
			{
				info.runAttrs.runStyle.tsFlags = 0; // don't export internal flags
				pEntry->scrpStartChar = startChar;
				pEntry->scrpTEAttrs = * (TERunAttributes *) &info.runAttrs;
				pEntry++;
			}

#if WASTE_OBJECTS
			if (hSoup != nil)
			{
				// if this style run references an embedded object, append it to the "soup"
				if (info.runAttrs.runStyle.tsObject != nil)
				{
					if ((err = _WEAppendObject(hSoup, &info, startChar)) != noErr)
					{
						goto cleanup;
					}
				}
			}
#endif

		}
	}
	// clear result code
	err = noErr;

cleanup:

	// unlock the style scrap handle
	if (hStyles != nil)
	{
		HUnlock(hStyles);
	}

	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);
	// return result code
	return err;
}

//ShadowIRC: Convert an extended style to standard
pascal Handle MyExtendedStyleToTE(TEStyleScrapHandle styles);
pascal Handle MyExtendedStyleToTE(TEStyleScrapHandle styles)
{
	if(styles)
	{
		long numStyles = (GetHandleSize((Handle)styles) - sizeof(SInt16)) / sizeof(myScrpSTElement);
		StScrpHandle h;
		StScrpPtr p;
		long x;
		
		h = (StScrpHandle)NewHandle(sizeof(short) + (numStyles * sizeof(ScrpSTElement)));
		HLock((Handle)h);
		p = *h;
		
		if(numStyles > 32767)
			p->scrpNStyles = 32767;
		else
			p->scrpNStyles = numStyles;
			
		for(x=0;x<numStyles;x++)
			p->scrpStyleTab[x] = *(ScrpSTElement*)&(**styles).scrpStyleTab[x];
		
		return (Handle)h;
	}
	else
		return 0;
}

pascal OSErr _WEPutScrapHandle ( FlavorType dataFlavor, Handle dataHandle )
{
	Boolean saveDataLock ;
	OSErr err ;

	saveDataLock = _WESetHandleLock ( dataHandle, true ) ;
	if(dataFlavor == kTypeStyles) //convert it to a TextEdit scrap, rather than modified stuff
	{
		Handle myData;
//ShadowIRC: Convert style to standard		
		myData = MyExtendedStyleToTE((TEStyleScrapHandle)dataHandle);
		if(myData)
		{
			err = PutScrap ( GetHandleSize ( myData ), dataFlavor, * myData ) ;
			DisposeHandle(myData);
		}
	}
	else
		err = PutScrap ( GetHandleSize ( dataHandle ), dataFlavor, * dataHandle ) ;
	_WESetHandleLock ( dataHandle, saveDataLock ) ;

	return err ;
}

pascal OSErr WECopy ( WEHandle hWE )
{
	//	copy the selection range to the desk scrap

	WEPtr pWE ;
	Handle dataHandle = nil ;
	Boolean saveWELock;
	OSErr err;
#if WASTE_OBJECTS
	WEObjectDescHandle hObjectDesc = nil;
#endif

	//	lock the WE record
	saveWELock = _WESetHandleLock ( ( Handle ) hWE, true ) ;
	pWE = * hWE ;

	//	return weEmptySelectionErr if the selection range is empty
	err = weEmptySelectionErr ;
	if ( pWE -> selStart == pWE -> selEnd )
	{
		goto cleanup ;
	}

	//	clear the desk scrap
	if ( ( err = ZeroScrap ( ) ) != noErr )
	{
		goto cleanup ;
	}

	//	allocate a temporary handle
	if ( ( err = _WEAllocate ( 0, kAllocTemp, & dataHandle ) ) != noErr )
	{
		goto cleanup ;
	}

#if WASTE_OBJECTS
	//	is there a selected object?
	if ( WEGetSelectedObject ( & hObjectDesc, hWE ) == noErr )
	{
		FlavorType objectType = 0 ;
		Boolean disposeData = false ;

		//	stream it
		if ( ( err = _WEStreamObject ( weToScrap, & objectType, & dataHandle,
				& disposeData, hObjectDesc ) ) != noErr )
		{
			goto cleanup ;
		}

		//	put it on the scrap
		if ( ( err = _WEPutScrapHandle ( objectType, dataHandle ) ) != noErr )
		{
			goto cleanup ;
		}

		if ( ! disposeData )
		{
			dataHandle = nil ;
		}
	}
	else
#endif
	{
		//	make a copy of the selected text
		if ( ( err = WECopyRange ( pWE -> selStart, pWE -> selEnd, dataHandle, nil, nil, hWE ) ) != noErr )
		{
			goto cleanup ;
		}

		//	put it on the scrap
		if ( ( err = _WEPutScrapHandle ( kTypeText, dataHandle ) ) != noErr )
		{
			goto cleanup ;
		}

		//	don't copy styles/soup if the text is monostyled
		if ( ! BTST ( pWE -> features, weFMonoStyled ) )
		{
			//	build & copy a style scrap
			if ( ( err = WECopyRange ( pWE -> selStart, pWE -> selEnd, nil, dataHandle, nil, hWE ) ) != noErr )
			{
				goto cleanup ;
			}
			if ( ( err = _WEPutScrapHandle ( kTypeStyles, dataHandle ) ) != noErr )
			{
				goto cleanup ;
			}

			//	build & copy a font table
			if ( ( err = WEBuildFontTable ( dataHandle, nil, hWE ) ) != noErr )
			{
				goto cleanup ;
			}
			if ( ( err = _WEPutScrapHandle ( kTypeFontTable, dataHandle ) ) != noErr )
			{
				goto cleanup ;
			}

#if WASTE_OBJECTS
			//	build & copy an object soup
			if ( ( err = WECopyRange ( pWE -> selStart, pWE -> selEnd, nil, nil, dataHandle, hWE ) ) != noErr )
			{
				goto cleanup ;
			}
			if ( ( err = _WEPutScrapHandle ( kTypeSoup, dataHandle ) ) != noErr )
			{
				goto cleanup ;
			}
#endif
		}
	}

	//	clear result code
	err = noErr;

cleanup:
	//	clean up
	_WEForgetHandle ( & dataHandle ) ;
	_WESetHandleLock ( ( Handle ) hWE, saveWELock ) ;

	//	return result code
	return err ;
}
