/*
 *	WESelectors.c
 *
 *	WASTE PROJECT
 *  Routines for manipulating selector lookup tables
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

// MPW can't handle FIELD_DESC stuff (duh!)

#ifdef applec

//#error "hand-made selector tables not up-to-date!"

// this is **ugly** and will break if we change the declaration of WERec

WELookupTable _weMainSelectorTable[] = {
{ weRefCon,     				0x0078, 0x0004 },
{ wePort,						0x0000, 0x0004 },
{ weText,						0x0004, 0x0004 },
{ weCharToPixelHook,			0x00C0, 0x0004 },
{ weCharByteHook,				0x00CC, 0x0004 },
{ weCharTypeHook,				0x00D0, 0x0004 },
{ weClickLoop,					0x009C, 0x0004 },
{ weDrawTextHook,				0x00B8, 0x0004 },
{ weEraseHook,					0x00D4, 0x0004 },
{ weFluxProc,					0x00D8, 0x0004 },
{ weLineBreakHook,				0x00C4, 0x0004 },
{ weLineArray,					0x0008, 0x0004 },
{ wePixelToCharHook,			0x00BC, 0x0004 },
{ weRunArray,					0x0010, 0x0004 },
{ weScrollProc,					0x00A0, 0x0004 },
{ weStyleTable,					0x000C, 0x0004 },
{ weWordBreakHook,				0x00C8, 0x0004 },

#if WASTE_TSM_SUPPORT
{ weTSMPostUpdate,				0x00A8, 0x0004 },
{ weTSMPreUpdate,				0x00A4, 0x0004 },
{ weTSMDocumentID,				0x0080, 0x0004 },
#endif

#if WASTE_DRAG_AND_DROP
{ weCurrentDrag,				0x008C, 0x0004 },
{ wePreTrackDragHook,			0x00AC, 0x0004 },
{ weHiliteDropAreaHook, 		0x00B4, 0x0004 },
{ weTranslateDragHook,			0x00B0, 0x0004 },
#endif

#if WASTE_TRANSLUCENT_DRAGS
{ weTranslucencyThreshold,		0x00F4, 0x0004 },
#endif

#if WASTE_IC_SUPPORT
{ weURLHint,					0x00F8, 0x0004 },
#endif

{ 0, 					0, 0 }};

#if WASTE_OBJECTS

WELookupTable _weObjectHandlerSelectorTable[] = {
{ weClickHandler,		0x0010, 0x0004 },
{ weDrawHandler,		0x000C, 0x0004 },
{ weDisposeHandler,		0x0008, 0x0004 },
{ weNewHandler,			0x0004, 0x0004 },
{ weStreamHandler,      0x0014, 0x0004 },
{ weRefCon,             0x0018, 0x0004 },
{ 0, 					0, 0 }};

#endif  // WASTE_OBJECTS

#else

// proper way of defining selector tables

#define FIELD_OFFSET(FIELD, STRUCT)		(int) &((STRUCT *) 0L)->FIELD
#define FIELD_SIZE(FIELD, STRUCT)		sizeof(((STRUCT *) 0L)->FIELD)
#define FIELD_DESC(FIELD, STRUCT)		{ FIELD_OFFSET(FIELD, STRUCT), FIELD_SIZE(FIELD, STRUCT) }

WELookupTable _weMainSelectorTable[] = {
{ weRefCon,     			FIELD_DESC(refCon,            		WERec) },
{ wePort,					FIELD_DESC(port,              		WERec) },
{ weText,					FIELD_DESC(hText,             		WERec) },
{ weCharToPixelHook,		FIELD_DESC(charToPixelHook,   		WERec) },
{ weCharByteHook,			FIELD_DESC(charByteHook,      		WERec) },
{ weCharTypeHook,			FIELD_DESC(charTypeHook,      		WERec) },
{ weClickLoop,				FIELD_DESC(clickLoop,         		WERec) },
{ weDrawTextHook,			FIELD_DESC(drawTextHook,      		WERec) },
{ weEraseHook,				FIELD_DESC(eraseHook,				WERec) },
{ weFluxProc,				FIELD_DESC(fluxProc,				WERec) },
{ weLineBreakHook,			FIELD_DESC(lineBreakHook,     		WERec) },
{ weLineArray,				FIELD_DESC(hLines,            		WERec) },
{ wePixelToCharHook,		FIELD_DESC(pixelToCharHook,   		WERec) },
{ weRunArray,				FIELD_DESC(hRuns,             		WERec) },
{ weScrollProc,				FIELD_DESC(scrollProc,        		WERec) },
{ weStyleTable,				FIELD_DESC(hStyles,           		WERec) },
{ weWordBreakHook,			FIELD_DESC(wordBreakHook,			WERec) },

#if WASTE_TSM_SUPPORT
{ weTSMPostUpdate,			FIELD_DESC(tsmPostUpdate,     		WERec) },
{ weTSMPreUpdate,			FIELD_DESC(tsmPreUpdate,      		WERec) },
{ weTSMDocumentID,			FIELD_DESC(tsmReference,      		WERec) },
#endif

#if WASTE_DRAG_AND_DROP
{ weCurrentDrag,			FIELD_DESC(currentDrag,       		WERec) },
{ wePreTrackDragHook,		FIELD_DESC(preTrackDragHook,		WERec) },
{ weHiliteDropAreaHook,		FIELD_DESC(hiliteDropAreaHook,		WERec) },
{ weTranslateDragHook,		FIELD_DESC(translateDragHook, 		WERec) },
#endif

#if WASTE_TRANSLUCENT_DRAGS
{ weTranslucencyThreshold, 	FIELD_DESC(translucencyThreshold,	WERec) },
#endif

#if WASTE_IC_SUPPORT
{ weURLHint,				FIELD_DESC(hURLHint,          		WERec) },
#endif

{ 0, 					0, 0 }};

#if WASTE_OBJECTS

WELookupTable _weObjectHandlerSelectorTable[] = {
{ weClickHandler,		FIELD_DESC(clickHandler,      WEOHTableEntry) },
{ weDrawHandler,		FIELD_DESC(drawHandler,       WEOHTableEntry) },
{ weDisposeHandler,		FIELD_DESC(freeHandler,       WEOHTableEntry) },
{ weNewHandler,			FIELD_DESC(newHandler,        WEOHTableEntry) },
{ weStreamHandler,      FIELD_DESC(streamHandler,     WEOHTableEntry) },
{ weRefCon,             FIELD_DESC(refCon,            WEOHTableEntry) },
{ 0, 					0, 0 }};

#endif	// WASTE_OBJECTS

#endif	// !applec

pascal void _WELookupSelector(const WELookupTable *table, WESelector selector, WEFieldDescriptor *desc)
{
	for ( ; table->selector != selector ; table++ )
	{
		if ( * (SInt32 *) &(table->desc) == 0L )
		{
			break;
		}
	}

	*desc = table->desc;
}

pascal OSErr _WEGetField(const WELookupTable *table, WESelector selector, SInt32 *info, void *structure)
{
	WEFieldDescriptor desc;

	_WELookupSelector(table, selector, &desc);

	if (desc.fLength == 0)
	{
		return weUndefinedSelectorErr;
	}

	*info = * (SInt32 *) ((SInt32) structure + desc.fOffset);
	return noErr;
}

pascal OSErr _WESetField(const WELookupTable *table, WESelector selector, SInt32 *info, void *structure)
{
	WEFieldDescriptor desc;

	_WELookupSelector(table, selector, &desc);

	if (desc.fLength == 0)
	{
		return weUndefinedSelectorErr;
	}

	* (SInt32 *) ((SInt32) structure + desc.fOffset) = *info;
	return noErr;
}

pascal OSErr WEGetInfo(WESelector selector, void *info, WEHandle hWE)
{
	return _WEGetField(_weMainSelectorTable, selector, (SInt32 *) info, *hWE);
}

pascal OSErr WESetInfo(WESelector selector, const void *info, WEHandle hWE)
{
	OSErr err;

	err = _WESetField(_weMainSelectorTable, selector, (SInt32 *) info, *hWE);

	// the hook fields can never be nil, so replace any nil field with the default address
	_WESetStandardHooks(hWE);

	return err;
}
