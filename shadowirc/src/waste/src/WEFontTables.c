/*
 *	WEFontTables.c
 *
 *	WASTE PROJECT
 *  Font Mapping Tables
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

enum
{
	weFontTableSignature			=	'FISH',
	weCurrentFontTableVersion		=	0x01008000		//	1.0
};

typedef struct
{
	FourCharCode ftSignature;	//	must be weFontTableSignature
	UInt32 ftVersion;			//	font table version, in NumVersion format
	UInt32 ftNumEntries;		//	entry count
	UInt32 ftReserved;			//	reserved for future use
} WEFontTableHeader, *WEFontTableHeaderPtr;

typedef struct
{
	SInt16 ftOldFontID;		//	font ID on save
	SInt16 ftNewFontID;		//	font ID on re-open
	SInt32 ftEncoding;		//	character encoding (currently = script code)
	SInt32 ftReserved;		//	reserved for future use
	UInt32 ftNameOffset;	//	offset to font name
} WEFontTableEntry, *WEFontTableEntryPtr;

typedef struct
{
	WEFontTableHeader ftHeader;
	WEFontTableEntry ftEntries [ kVariableLengthArray ];
	//	packed font names follow
} WEFontTable, *WEFontTablePtr, **WEFontTableHandle;

pascal OSErr WEBuildFontTable(Handle hFontTable, WEFontIDToNameUPP fontIDToNameProc, WEHandle hWE)
{
	Handle hStyles = nil;
	WEStyleTableEntry *pStyle, *pStyle2;
	WEFontTablePtr pFontTable;
	SInt32 nStyles, styleIndex, styleIndex2;
	WEFontTableEntry entry;
	WEFontTableEntryPtr pEntry;
	Str255 fontName;
	SInt32 fontCount;
	SInt32 fontNameOffset;
	Size fontNameSize, offsetAdjustment;
	Boolean saveStylesLock;
	OSErr err;

	//	return an error code is hFontTable is nil
	err = nilHandleErr;
	if (hFontTable == nil)
	{
		goto cleanup;
	}

	//	clone the style table, as we need a modifiable copy
	hStyles = (Handle) (*hWE)->hStyles;
	if ((err = HandToHand(&hStyles)) != noErr)
	{
		hStyles = nil;
		goto cleanup;
	}

	//	lock the style table
	saveStylesLock = _WESetHandleLock(hStyles, true);

	//	fill in font table header
	SetHandleSize(hFontTable, sizeof(WEFontTableHeader));
	if ((err = MemError()) != noErr)
	{
		goto cleanup;
	}
	pFontTable = * (WEFontTableHandle) hFontTable;
	_WEBlockClr(pFontTable, sizeof(WEFontTableHeader));
	pFontTable->ftHeader.ftSignature = weFontTableSignature;
	pFontTable->ftHeader.ftVersion = weCurrentFontTableVersion;

	//	init counters
	fontCount = 0;
	fontNameOffset = sizeof(WEFontTableHeader);

	//	walk the style table
	nStyles = (*hWE)->nStyles;
	for (	styleIndex = 0,
			pStyle = (WEStyleTableEntry *) *hStyles;
			styleIndex < nStyles;
			styleIndex++, pStyle++ )
	{
		//	skip this style table entry if it's either unused
		//	or it references a font we have already processed
		if (pStyle->refCount == 0)
		{
			continue;
		}

		//	get the name of this font
		BLOCK_CLR(fontName);
		GetFontName(pStyle->info.runStyle.tsFont, fontName);

		//	caller can customize the building process by supplying
		//	a mapping function
		if (fontIDToNameProc != nil)
		{
			if ((err = CallWEFontIDToNameProc(pStyle->info.runStyle.tsFont, fontName, fontIDToNameProc)) != noErr)
			{
				goto cleanup;
			}
		}

		//	calculate the space needed by the font name
		//	for optimal alignment, make sure it's a multiple of 4
		fontNameSize = StrLength(fontName) + 4;
		fontNameSize &= ~3;

		//	append font name at the end of the font table
		if ((err = _WESplice(hFontTable, fontName, fontNameSize, -1)) != noErr)
		{
			goto cleanup;
		}

		//	create a new font table entry for this font
		BLOCK_CLR(entry);
		entry.ftNewFontID = entry.ftOldFontID = pStyle->info.runStyle.tsFont;
		entry.ftEncoding = FontToScript(entry.ftOldFontID);
		entry.ftNameOffset = fontNameOffset;
		fontNameOffset += fontNameSize;

		//	add a new entry to the font table
		pFontTable = * (WEFontTableHandle) hFontTable;
		if ((err = _WESplice(hFontTable, &entry, sizeof(entry),
				sizeof(WEFontTableHeader) + sizeof(WEFontTableEntry) * fontCount)) != noErr)
		{
			goto cleanup;
		}
		fontCount++;

		//	mark all remaining entries in the style table which reference
		//	the font ID we have just processed, so that we'll skip them
		for ( 	styleIndex2 = styleIndex + 1, pStyle2 = pStyle + 1;
				styleIndex2 < nStyles;
				styleIndex2++, pStyle2++ )
		{
			if (pStyle2->info.runStyle.tsFont == entry.ftOldFontID)
			{
				pStyle2->refCount = 0;
			}
		}
	}	// for

	//	save font count in font table header
	pFontTable = * (WEFontTableHandle) hFontTable;
	pFontTable->ftHeader.ftNumEntries = fontCount;

	//	adjust name offsets in the font table
	offsetAdjustment = sizeof(WEFontTableEntry) * fontCount;
	for (	pEntry = &pFontTable->ftEntries[0];
			fontCount > 0;
			fontCount--, pEntry++ )
	{
		pEntry->ftNameOffset += offsetAdjustment;
	}

	//	clear result code
	err = noErr;

cleanup:
	_WEForgetHandle(&hStyles);

	//	return result code
	return err;
}

pascal OSErr WEUpdateFontTable(Handle hFontTable, WEFontNameToIDUPP fontNameToIDProc, Boolean *wasChanged)
{
	WEFontTablePtr pFontTable;
	WEFontTableEntryPtr pEntry;
	ConstStr255Param fontName;
	Size fontTableSize;
	SInt32 fontCount;
	Str255 systemFontName;
	Boolean saveFontTableLock = false;
	Boolean changed = false;	//	assume font table won't change
	OSErr err;

	//	return an error code is hFontTable is nil
	err = nilHandleErr;
	if (hFontTable == nil)
	{
		goto cleanup;
	}

	//	lock the font table
	saveFontTableLock = _WESetHandleLock(hFontTable, true);
	pFontTable = * (WEFontTableHandle) hFontTable;

	//	make sure the font table is a reasonable size
	fontTableSize = GetHandleSize(hFontTable);
	err = weCorruptDataErr;
	if (fontTableSize < sizeof(WEFontTableHeader))
	{
		goto cleanup;
	}

	//	check font table signature
	if (pFontTable->ftHeader.ftSignature != weFontTableSignature)
	{
		goto cleanup;
	}

	//	check font table version
	//	assume we know how to handle all 1.x versions
	//	if a new, incompatible format is designed in the future, it will
	//	have to bump the version number to 2.x so this code can safely skip it
	err = weNewerVersionErr;
	if ((pFontTable->ftHeader.ftVersion & 0xFF000000) != 0x01000000)
	{
		goto cleanup;
	}

	//	check entry count
	err = weCorruptDataErr;
	if (fontTableSize < sizeof(WEFontTableHeader) + sizeof(WEFontTableEntry) * pFontTable->ftHeader.ftNumEntries)
	{
		goto cleanup;
	}

	//	get name of system font
	GetFontName(systemFont, systemFontName);

	//	walk the font table
	for (	fontCount = pFontTable->ftHeader.ftNumEntries,
			pEntry = &pFontTable->ftEntries[0];
			fontCount > 0 ;
			fontCount--, pEntry++ )
	{
		//	calculate pointer to font name
		fontName = (ConstStr255Param) (((UInt32) pFontTable) + pEntry->ftNameOffset);

		//	by default, map font name to font ID using GetFNum()
		GetFNum(fontName, &pEntry->ftNewFontID);

		//	GetFNum() returns zero either because the specified font
		//	is the system font, or because it doesn't exist
		if ((pEntry->ftNewFontID == 0) && !EqualString(fontName, systemFontName, false, false))
		{
			//	missing font:
			//	use a replacement font with the same character encoding, if possible
			pEntry->ftNewFontID = GetScriptVariable(GetScriptVariable(pEntry->ftEncoding,
						smEnabled) ? pEntry->ftEncoding : smSystemScript, smScriptAppFond);
		}

		//	caller can customize the name resolution process
		//	by specifying a mapping function
		if (fontNameToIDProc != nil)
		{
			//	map font name to font ID using the supplied mapping function
			if ((err = CallWEFontNameToIDProc(fontName, pEntry->ftOldFontID, &pEntry->ftNewFontID, fontNameToIDProc)) != noErr)
			{
				goto cleanup;
			}
		}

		if (pEntry->ftOldFontID != pEntry->ftNewFontID)
		{
			changed = true;
		}
	}	//	for

	//	clear result code
	err = noErr;

cleanup:
	//	unlock the font table
	if (hFontTable != nil)
	{
		_WESetHandleLock(hFontTable, saveFontTableLock);
	}

	//	return wasChanged flag
	if (wasChanged != nil)
	{
		*wasChanged = changed;
	}

	//	return result code
	return err;
}

pascal OSErr WEUpdateStyleScrap(StScrpHandle hStyles, Handle hFontTable)
{
	WEFontTablePtr pFontTable;
	WEFontTableEntryPtr pEntry;
	ScrpSTElement *pStyle;
	SInt32 fontCount, styleCount;
	SInt16 oldFontID, newFontID;

	//	this function doesn't move memory, so we can
	//	safely dereference hFontTable without locking it
	pFontTable = * (WEFontTableHandle) hFontTable;

	//	remap font IDs in the style table associated with the specified WE instance
	for (	fontCount = pFontTable->ftHeader.ftNumEntries,
			pEntry = &pFontTable->ftEntries[0];
			fontCount > 0 ;
			fontCount--, pEntry++ )
	{
		oldFontID = pEntry->ftOldFontID;
		newFontID = pEntry->ftNewFontID;

		//	skip inner loop if there's nothing to change
		if (oldFontID == newFontID)
		{
			continue;
		}

		for (	styleCount = (*hStyles)->scrpNStyles,
				pStyle = & (*hStyles)->scrpStyleTab[0];
				styleCount > 0;
				styleCount--, pStyle++ )
		{
			if (pStyle->scrpFont == oldFontID)
			{
				pStyle->scrpFont = newFontID;
			}
		}	//	for
	}	//	for

	return noErr;
}
