/*
 *	WASTE.h
 *
 *	C/C++ interface to the WASTE text engine
 *
 *	version 1.3 (January 1998)
 *
 *	Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 */

#ifndef _WASTE_
#define _WASTE_

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef UNIVERSAL_INTERFACES_VERSION
#error "You need Universal Headers version 2.1 or newer to use WASTE.h"
#endif

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __QUICKDRAWTEXT__
#include <QuickdrawText.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __SCRIPT__
#include <Script.h>
#endif

#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

#ifndef __DRAG__
#include <Drag.h>
#endif

#ifndef __LONGCOORDINATES__
#ifndef _LongCoords_
#include "LongCoords.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ShadowIRC: Not needed since we set this explicitly in the target settings
#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

#if PRAGMA_IMPORT_SUPPORTED
#pragma import on
#endif
*/

//	The macro WASTE_VERSION expands to the current version of WASTE,
//	expressed in standard NumVersion format (see Types.h)
//	The macro WASTE11 is obsolete but still supported for backward
//	compatibility

#define WASTE_VERSION	0x01308000		/* 1.3 */
#define WASTE11

/*	result codes */

enum
{
	weCantUndoErr			=	-10015,	/* undo buffer is clear (= errAECantUndo) */
	weEmptySelectionErr		=	-10013,	/* selection range is empty (= errAENoUserSelection) */
	weNotHandledErr			=	-1708,	/* please use default behavior (= errAEEventNotHandled) */
	weUnknownObjectTypeErr	=	-9478,	/* specified object type is not registered */
	weObjectNotFoundErr		=	-9477,	/* no object found at specified offset */
	weReadOnlyErr			=	-9476,	/* instance is read-only */
	weUndefinedSelectorErr	=	-50		/* unknown selector (= paramErr) */
};

/*	alignment styles */

enum
{
	weFlushLeft 		=	-2,		/* flush left */
	weFlushRight		=	-1,		/* flush right */
	weFlushDefault		=	 0,		/* flush according to system direction */
	weCenter			=	 1,		/* centered */
	weJustify			=	 2		/* fully justified */
};

/*	primary line direction */

enum
{
	weDirDefault		=	 1,		/* according to system direction */
	weDirRightToLeft	=	-1,		/* force right-to-left */
	weDirLeftToRight	=	 0		/* force left-to-right */
};

/*	values for the mode parameter in WESetStyle and WEContinuousStyle */

enum
{
	weDoFont				=	0x0001,
	weDoFace				=	0x0002,
	weDoSize				=	0x0004,
	weDoColor				=	0x0008,
	weDoAll					=	weDoFont + weDoFace + weDoSize + weDoColor,
	weDoAddSize				=	0x0010,
	weDoToggleFace			=	0x0020,
	weDoReplaceFace			=	0x0040,
	weDoPreserveScript		=	0x0080,
	weDoExtractSubscript	=	0x0100,
	weDoFaceMask			=	0x0200
};

/*	values for the edge parameter in WEGetOffset etc. */

enum
{
	kLeadingEdge = -1,		/* point is on the leading edge of a glyph */
	kTrailingEdge = 0,		/* point is on the trailing edge of a glyph */
	kObjectEdge = 2			/* point is in the middle of an embedded object */
};

/*	values for WEFeatureFlag feature parameter */

enum
{
	weFAutoScroll		=	0,		/* automatically scroll the selection range into view */
	weFOutlineHilite	=	2,		/* frame selection when deactivated */
	weFReadOnly			=	5,		/* disallow modifications */
	weFUndo				=	6,		/* support WEUndo() */
	weFIntCutAndPaste	=	7,		/* use intelligent cut-and-paste rules */
	weFDragAndDrop		=	8,		/* support drag-and-drop text editing */
	weFInhibitRecal		=	9,		/* don't recalculate line starts and don't redraw text */
	weFUseTempMem		=	10,		/* use temporary memory for main data structures */
	weFDrawOffscreen	=	11,		/* draw text offscreen for smoother visual results */
	weFInhibitRedraw	=	12,		/* don't redraw text */
	weFMonoStyled		=	13,		/* disallow style changes */
	weFInhibitColor		=	31		/* draw in black & white only */
};

/*	values for WENew flags parameter */

enum
{
	weDoAutoScroll		=	1L << weFAutoScroll,
	weDoOutlineHilite	=	1L << weFOutlineHilite,
	weDoReadOnly		=	1L << weFReadOnly,
	weDoUndo			=	1L << weFUndo,
	weDoIntCutAndPaste	=	1L << weFIntCutAndPaste,
	weDoDragAndDrop		=	1L << weFDragAndDrop,
	weDoInhibitRecal	=	1L << weFInhibitRecal,
	weDoUseTempMem		=	1L << weFUseTempMem,
	weDoDrawOffscreen	=	1L << weFDrawOffscreen,
	weDoInhibitRedraw	=	1L << weFInhibitRedraw,
	weDoMonoStyled		=	1L << weFMonoStyled,
	weDoInhibitColor	=	1L << weFInhibitColor
};

/*	values for WEFeatureFlag action parameter */

enum
{
	weBitToggle = -2,	/* toggles the specified feature */
	weBitTest,			/* returns the current setting of the specified feature */
	weBitClear,			/* disables the specified feature */
	weBitSet			/* enables the specified feature */
};

/*	selectors for WEGetInfo and WESetInfo */

enum
{
	weCharByteHook				=	'cbyt', /* CharByte hook */
	weCharToPixelHook			=	'c2p ',	/* CharToPixel hook */
	weCharTypeHook				=	'ctyp', /* CharType hook */
	weClickLoop					=	'clik',	/* click loop callback */
	weCurrentDrag				=	'drag',	/* drag currently being tracked from WEClick() */
	weDrawTextHook				=	'draw', /* text drawing hook */
	weEraseHook					=	'eras', /* background erasing hook */
	weFluxProc					=	'flux', /* flux proc */
	weHiliteDropAreaHook		=	'hidr', /* drop area highlighting hook */
	weLineBreakHook				=	'lbrk',	/* line breaking hook */
	wePixelToCharHook			=	'p2c ', /* PixelToChar hook */
	wePort						=	'port',	/* graphics port */
	wePreTrackDragHook			=	'ptrk', /* pre-TrackDrag hook */
	weRefCon					=	'refc',	/* reference constant for use by application */
	weScrollProc				=	'scrl',	/* auto-scroll callback */
	weText						=	'text',	/* text handle */
	weTranslateDragHook 		=	'xdrg', /* drag translation callback */
	weTranslucencyThreshold		=	'tluc', /* area threshold for translucent drags */
	weTSMDocumentID				=	'tsmd',	/* Text Services Manager document ID */
	weTSMPreUpdate				=	'pre ',	/* Text Services Manager pre-update callback */
	weTSMPostUpdate				=	'post',	/* Text Services Manager post-update callback */
	weURLHint					=	'urlh',	/* URL hint string for Internet Config */
	weWordBreakHook				=	'wbrk'	/* word breaking hook */
};

/*	values for WEInstallObjectHandler handlerSelector parameter */

enum
{
	weNewHandler		=	'new ',		/* new handler */
	weDisposeHandler	=	'free',		/* dispose handler */
	weDrawHandler		=	'draw',		/* draw handler */
	weClickHandler		=	'clik',		/* click handler */
	weStreamHandler		=	'strm'		/* stream handler */
};

/*	action kinds */

enum
{
	weAKNone			=	0,		/* null action */
	weAKUnspecified		=	1,		/* action of unspecified nature */
	weAKTyping			=	2,		/* some text has been typed in */
	weAKCut				=	3,		/* the selection range has been cut */
	weAKPaste			=	4,		/* something has been pasted */
	weAKClear			=	5,		/* the selection range has been deleted */
	weAKDrag			=	6,		/* drag and drop operation */
	weAKSetStyle		=	7		/* some style has been applied to a text range */
};

/*	destination kinds for stream handler */

enum
{
	weToScrap			=	0,
	weToDrag			=	1,
	weToSoup			=	2
};

typedef struct OpaqueWEReference *WEReference;
typedef struct OpaqueWEObjectReference *WEObjectReference;
typedef Handle WESoupHandle;
typedef Handle WEFontTableHandle;
typedef SInt16 WEActionKind;
typedef SInt8 WEAlignment;
typedef SInt16 WEDirection;
typedef SInt8 WEEdge;
typedef UInt16 WEStyleMode;
typedef FourCharCode WESelector;
typedef WEReference WEHandle;	/* obsolete, kept for backward compatibility */

typedef struct WERunInfo
{
	SInt32 				runStart;	/* byte offset to first character of style run */
	SInt32 				runEnd;		/* byte offset past last character of style run */
	SInt16 				runHeight;	/* line height (ascent + descent + leading) */
	SInt16 				runAscent;	/* font ascent */
	TextStyle 			runStyle;	/* text attributes */
	WEObjectReference	runObject;	/* either nil or reference to embedded object */
} WERunInfo;


/*	callback prototypes */

typedef pascal Boolean (*WEClickLoopProcPtr)(WEReference we);
typedef pascal void (*WEScrollProcPtr)(WEReference we);
typedef pascal void (*WETSMPreUpdateProcPtr)(WEReference we);
typedef pascal void (*WETSMPostUpdateProcPtr)(WEReference we,
		SInt32 fixLength, SInt32 inputAreaStart, SInt32 inputAreaEnd,
		SInt32 pinRangeStart, SInt32 pinRangeEnd);
typedef pascal OSErr (*WEPreTrackDragProcPtr)(DragReference drag, WEReference we);
typedef pascal OSErr (*WETranslateDragProcPtr)(DragReference theDrag,
		ItemReference theItem, FlavorType requestedType, Handle putDataHere,
		SInt32 dropOffset, WEReference we);
typedef pascal OSErr (*WEHiliteDropAreaProcPtr)(DragReference theDrag,
		Boolean hiliteFlag, WEReference we);
typedef pascal OSErr (*WEFontIDToNameProcPtr)(SInt16 fontID, Str255 fontName);
typedef pascal OSErr (*WEFontNameToIDProcPtr)(ConstStr255Param fontName, SInt16 oldFontID, SInt16 *newFontID);
typedef pascal void (*WEDrawTextProcPtr)(Ptr pText, SInt32 textLength, Fixed slop,
		JustStyleCode styleRunPosition, WEReference we);
typedef pascal SInt32 (*WEPixelToCharProcPtr)(Ptr pText, SInt32 textLength, Fixed slop,
		Fixed *pixelWidth, WEEdge *edge, JustStyleCode styleRunPosition, Fixed hPos, WEReference we);
typedef pascal SInt16 (*WECharToPixelProcPtr)(Ptr pText, SInt32 textLength, Fixed slop,
		SInt32 offset, SInt16 direction, JustStyleCode styleRunPosition, SInt16 hPos, WEReference we);
typedef pascal StyledLineBreakCode (*WELineBreakProcPtr)(Ptr pText, SInt32 textLength,
		SInt32 textStart, SInt32 textEnd, Fixed *textWidth, SInt32 *textOffset, WEReference we);
typedef pascal void (*WEWordBreakProcPtr)(Ptr pText, SInt16 textLength, SInt16 offset,
		WEEdge edge, OffsetTable breakOffsets, ScriptCode script, WEReference we);
typedef pascal SInt16 (*WECharByteProcPtr)(Ptr pText, SInt16 textOffset, ScriptCode script,
		WEReference we);
typedef pascal SInt16 (*WECharTypeProcPtr)(Ptr pText, SInt16 textOffset, ScriptCode script, WEReference we);
typedef pascal void (*WEEraseProcPtr)(const Rect *area, WEReference we);
typedef pascal void (*WEFluxProcPtr)(SInt32 offset, SInt32 delta, WEReference we);
typedef pascal OSErr (*WENewObjectProcPtr)(Point *defaultObjectSize,
		WEObjectReference obj);
typedef pascal OSErr (*WEDisposeObjectProcPtr)(WEObjectReference obj);
typedef pascal OSErr (*WEDrawObjectProcPtr)(const Rect *destRect,
		WEObjectReference obj);
typedef pascal Boolean (*WEClickObjectProcPtr)(Point hitPt, EventModifiers modifiers, UInt32 clickTime,
		WEObjectReference obj);
typedef pascal OSErr (*WEStreamObjectProcPtr)(SInt16 destKind, FlavorType *theType,
		Handle putDataHere, WEObjectReference obj);


/*	UPP proc info */

enum
{
	uppWEClickLoopProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEScrollProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWETSMPreUpdateProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWETSMPostUpdateProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEReference /*we*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*fixLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt32 /*inputAreaStart*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(SInt32 /*inputAreaEnd*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(SInt32 /*pinRangeStart*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(SInt32 /*pinRangeEnd*/))),
	uppWEPreTrackDragProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(DragReference /*drag*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWETranslateDragProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(DragReference /*theDrag*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(ItemReference /*theItem*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(FlavorType /*requestedType*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(Handle /*putDataHere*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(SInt32 /*dropOffset*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEHiliteDropAreaProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(DragReference /*theDrag*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(Boolean /*hiliteFlag*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEFontIDToNameProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(SInt16 /*fontID*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(StringPtr /*fontName*/))),
	uppWEFontNameToIDProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(ConstStr255Param /*fontName*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*oldFontID*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt16 * /*newFontID*/))),
	uppWEDrawTextProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(Fixed /*slop*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(JustStyleCode /*styleRunPosition*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEPixelToCharProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt32)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(Fixed /*slop*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(Fixed * /*pixelWidth*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(WEEdge * /*edge*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(JustStyleCode /*styleRunPosition*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(Fixed /*hPos*/)))
		| STACK_ROUTINE_PARAMETER(8,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWECharToPixelProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt16)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(Fixed /*slop*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(SInt32 /*offset*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(SInt16 /*direction*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(JustStyleCode /*styleRunPosition*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(SInt16 /*hPos*/)))
		| STACK_ROUTINE_PARAMETER(8,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWELineBreakProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(StyledLineBreakCode )))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt32 /*textStart*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(SInt32 /*textEnd*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(Fixed * /*textWidth*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(SInt32 * /*textOffset*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEWordBreakProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt16 /*offset*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEEdge /*edge*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(OffsetTable * /*breakOffsets*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(ScriptCode /*script*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWECharByteProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt16 )))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*textOffset*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(ScriptCode /*script*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWECharTypeProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt16 )))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*textOffset*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(ScriptCode /*script*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEEraseProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(const Rect * /*area*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWEFluxProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(SInt32 /*offset*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*delta*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(WEReference /*we*/))),
	uppWENewObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Point * /*defaultObjectSize*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEObjectReference /*obj*/))),
	uppWEDisposeObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEObjectReference /*obj*/))),
	uppWEDrawObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(const Rect * /*destRect*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEObjectReference /*obj*/))),
	uppWEClickObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Point /*hitPt*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(EventModifiers /*modifiers*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(UInt32 /*clickTime*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEObjectReference /*obj*/))),
	uppWEStreamObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(SInt16 /*destKind*/)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(FlavorType * /*theType*/)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Handle /*putDataHere*/)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(WEObjectReference /*obj*/)))
};


/*	UPPs, NewÅProc macros & CallÅProc macros */

/*
	NOTE:
    For compatibility with the Pascal version, CallÅProc macros take the form:

		CallFooProc(..., userRoutine)

	instead of:

		CallFooProc(userRoutine, ...)

*/

#if TARGET_RT_MAC_CFM //ShadowIRC: was GENERATINGCFM

typedef UniversalProcPtr WEClickLoopUPP;
typedef UniversalProcPtr WEScrollUPP;
typedef UniversalProcPtr WETSMPreUpdateUPP;
typedef UniversalProcPtr WETSMPostUpdateUPP;
typedef UniversalProcPtr WEPreTrackDragUPP;
typedef UniversalProcPtr WETranslateDragUPP;
typedef UniversalProcPtr WEHiliteDropAreaUPP;
typedef UniversalProcPtr WEFontIDToNameUPP;
typedef UniversalProcPtr WEFontNameToIDUPP;
typedef UniversalProcPtr WEDrawTextUPP;
typedef UniversalProcPtr WEPixelToCharUPP;
typedef UniversalProcPtr WECharToPixelUPP;
typedef UniversalProcPtr WELineBreakUPP;
typedef UniversalProcPtr WEWordBreakUPP;
typedef UniversalProcPtr WECharByteUPP;
typedef UniversalProcPtr WECharTypeUPP;
typedef UniversalProcPtr WENewObjectUPP;
typedef UniversalProcPtr WEEraseUPP;
typedef UniversalProcPtr WEFluxUPP;
typedef UniversalProcPtr WEDisposeObjectUPP;
typedef UniversalProcPtr WEDrawObjectUPP;
typedef UniversalProcPtr WEClickObjectUPP;
typedef UniversalProcPtr WEStreamObjectUPP;

#define NewWEClickLoopProc(userRoutine) \
	(WEClickLoopUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEClickLoopProcInfo, GetCurrentArchitecture())
#define NewWEScrollProc(userRoutine) \
	(WEScrollUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEScrollProcInfo, GetCurrentArchitecture())
#define NewWETSMPreUpdateProc(userRoutine) \
	(WETSMPreUpdateUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWETSMPreUpdateProcInfo, GetCurrentArchitecture())
#define NewWETSMPostUpdateProc(userRoutine) \
	(WETSMPostUpdateUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWETSMPostUpdateProcInfo, GetCurrentArchitecture())
#define NewWEPreTrackDragProc(userRoutine) \
	(WEPreTrackDragUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEPreTrackDragProcInfo, GetCurrentArchitecture())
#define NewWETranslateDragProc(userRoutine) \
	(WETranslateDragUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWETranslateDragProcInfo, GetCurrentArchitecture())
#define NewWEHiliteDropAreaProc(userRoutine) \
	(WEHiliteDropAreaUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEHiliteDropAreaProcInfo, GetCurrentArchitecture())
#define NewWEFontIDToNameProc(userRoutine) \
	(WEFontIDToNameUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEFontIDToNameProcInfo, GetCurrentArchitecture())
#define NewWEFontNameToIDProc(userRoutine) \
	(WEFontNameToIDUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEFontNameToIDProcInfo, GetCurrentArchitecture())
#define NewWEDrawTextProc(userRoutine) \
	(WEDrawTextUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEDrawTextProcInfo, GetCurrentArchitecture())
#define NewWEPixelToCharProc(userRoutine) \
	(WEPixelToCharUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEPixelToCharProcInfo, GetCurrentArchitecture())
#define NewWECharToPixelProc(userRoutine) \
	(WECharToPixelUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWECharToPixelProcInfo, GetCurrentArchitecture())
#define NewWELineBreakProc(userRoutine) \
	(WELineBreakUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWELineBreakProcInfo, GetCurrentArchitecture())
#define NewWEWordBreakProc(userRoutine) \
	(WEWordBreakUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEWordBreakProcInfo, GetCurrentArchitecture())
#define NewWECharByteProc(userRoutine) \
	(WECharByteUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWECharByteProcInfo, GetCurrentArchitecture())
#define NewWECharTypeProc(userRoutine) \
	(WECharTypeUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWECharTypeProcInfo, GetCurrentArchitecture())
#define NewWEEraseProc(userRoutine) \
	(WEEraseUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEEraseProcInfo, GetCurrentArchitecture())
#define NewWEFluxProc(userRoutine) \
	(WEFluxUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEFluxProcInfo, GetCurrentArchitecture())
#define NewWENewObjectProc(userRoutine) \
	(WENewObjectUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWENewObjectProcInfo, GetCurrentArchitecture())
#define NewWEDisposeObjectProc(userRoutine) \
	(WEDisposeObjectUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEDisposeObjectProcInfo, GetCurrentArchitecture())
#define NewWEDrawObjectProc(userRoutine) \
	(WEDrawObjectUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEDrawObjectProcInfo, GetCurrentArchitecture())
#define NewWEClickObjectProc(userRoutine) \
	(WEClickObjectUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEClickObjectProcInfo, GetCurrentArchitecture())
#define NewWEStreamObjectProc(userRoutine) \
	(WEStreamObjectUPP) NewRoutineDescriptor((ProcPtr) (userRoutine), uppWEStreamObjectProcInfo, GetCurrentArchitecture())

#define CallWEClickLoopProc(we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEClickLoopProcInfo, (we))
#define CallWEScrollProc(we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEScrollProcInfo, (we))
#define CallWETSMPreUpdateProc(we, userRoutine) \
	CallUniversalProc((userRoutine), uppWETSMPreUpdateProcInfo, (we))
#define CallWETSMPostUpdateProc(we, fixLength, inputAreaStart, inputAreaEnd, pinRangeStart, pinRangeEnd, userRoutine) \
	CallUniversalProc((userRoutine), uppWETSMPostUpdateProcInfo, (we), (fixLength), (inputAreaStart), (inputAreaEnd), (pinRangeStart), (pinRangeEnd))
#define CallWEPreTrackDragProc(drag, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEPreTrackDragProcInfo, (drag), (we))
#define CallWETranslateDragProc(theDrag, theItem, requestedType, putDataHere, dropOffset, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWETranslateDragProcInfo, (theDrag), (theItem), (requestedType), (putDataHere), (dropOffset), (we))
#define CallWEHiliteDropAreaProc(theDrag, hiliteFlag, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEHiliteDropAreaProcInfo, (theDrag), (hiliteFlag), (we))
#define CallWEFontIDToNameProc(fontID, fontName, userRoutine) \
	CallUniversalProc((userRoutine), uppWEFontIDToNameProcInfo, (fontID), (fontName))
#define CallWEFontNameToIDProc(fontName, oldFontID, newFontID, userRoutine) \
	CallUniversalProc((userRoutine), uppWEFontNameToIDProcInfo, (fontName), (oldFontID), (newFontID))
#define CallWEDrawTextProc(pText, textLength, slop, styleRunPosition, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEDrawTextProcInfo, (pText), (textLength), (slop), (styleRunPosition), (we))
#define CallWEPixelToCharProc(pText, textLength, slop, pixelWidth, edge, styleRunPosition, hPos, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEPixelToCharProcInfo, (pText), (textLength), (slop), (pixelWidth), (edge), (styleRunPosition), (hPos), (we))
#define CallWECharToPixelProc(pText, textLength, slop, offset, direction, styleRunPosition, hPos, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWECharToPixelProcInfo, (pText), (textLength), (slop), (offset), (direction), (styleRunPosition), (hPos), (we))
#define CallWELineBreakProc(pText, textLength, textStart, textEnd, textWidth, textOffset, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWELineBreakProcInfo, (pText), (textLength), (textStart), (textEnd), (textWidth), (textOffset), (we))
#define CallWEWordBreakProc(pText, textLength, offset, edge, breakOffsets, script, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEWordBreakProcInfo, (pText), (textLength), (offset), (edge), (breakOffsets), (script), (we))
#define CallWECharByteProc(pText, textOffset, script, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWECharByteProcInfo, (pText), (textOffset), (script), (we))
#define CallWECharTypeProc(pText, textOffset, script, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWECharTypeProcInfo, (pText), (textOffset), (script), (we))
#define CallWEEraseProc(area, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEEraseProcInfo, (area), (we))
#define CallWEFluxProc(offset, delta, we, userRoutine) \
	CallUniversalProc((userRoutine), uppWEFluxProcInfo, (offset), (delta), (we))
#define CallWENewObjectProc(defaultObjectSize, obj, userRoutine) \
	CallUniversalProc((userRoutine), uppWENewObjectProcInfo, (defaultObjectSize), (obj))
#define CallWEDisposeObjectProc(obj, userRoutine) \
	CallUniversalProc((userRoutine), uppWEDisposeObjectProcInfo, (obj))
#define CallWEDrawObjectProc(destRect, obj, userRoutine) \
	CallUniversalProc((userRoutine), uppWEDrawObjectProcInfo, (destRect), (obj))
#define CallWEClickObjectProc(hitPt, modifiers, clickTime, obj, userRoutine) \
	CallUniversalProc((userRoutine), uppWEClickObjectProcInfo, (hitPt), (modifiers), (clickTime), (obj))
#define CallWEStreamObjectProc(destKind, theType, putDataHere, obj, userRoutine) \
	CallUniversalProc((userRoutine), uppWEStreamObjectProcInfo, (destKind), (theType), (putDataHere), (obj))

#else

typedef WEClickLoopProcPtr WEClickLoopUPP;
typedef WEScrollProcPtr WEScrollUPP;
typedef WETSMPreUpdateProcPtr WETSMPreUpdateUPP;
typedef WETSMPostUpdateProcPtr WETSMPostUpdateUPP;
typedef WEPreTrackDragProcPtr WEPreTrackDragUPP;
typedef WETranslateDragProcPtr WETranslateDragUPP;
typedef WEHiliteDropAreaProcPtr WEHiliteDropAreaUPP;
typedef WEFontIDToNameProcPtr WEFontIDToNameUPP;
typedef WEFontNameToIDProcPtr WEFontNameToIDUPP;
typedef WEDrawTextProcPtr WEDrawTextUPP;
typedef WEPixelToCharProcPtr WEPixelToCharUPP;
typedef WECharToPixelProcPtr WECharToPixelUPP;
typedef WELineBreakProcPtr WELineBreakUPP;
typedef WEWordBreakProcPtr WEWordBreakUPP;
typedef WECharByteProcPtr WECharByteUPP;
typedef WECharTypeProcPtr WECharTypeUPP;
typedef WEEraseProcPtr WEEraseUPP;
typedef WEFluxProcPtr WEFluxUPP;
typedef WENewObjectProcPtr WENewObjectUPP;
typedef WEDisposeObjectProcPtr WEDisposeObjectUPP;
typedef WEDrawObjectProcPtr WEDrawObjectUPP;
typedef WEClickObjectProcPtr WEClickObjectUPP;
typedef WEStreamObjectProcPtr WEStreamObjectUPP;

#define NewWEClickLoopProc(userRoutine) ((WEClickLoopUPP) (userRoutine))
#define NewWEScrollProc(userRoutine) ((WEScrollUPP) (userRoutine))
#define NewWETSMPreUpdateProc(userRoutine) ((WETSMPreUpdateUPP) (userRoutine))
#define NewWETSMPostUpdateProc(userRoutine) ((WETSMPostUpdateUPP) (userRoutine))
#define NewWEPreTrackDragProc(userRoutine) ((WEPreTrackDragUPP) (userRoutine))
#define NewWETranslateDragProc(userRoutine) ((WETranslateDragUPP) (userRoutine))
#define NewWEHiliteDropAreaProc(userRoutine) ((WEHiliteDropAreaUPP) (userRoutine))
#define NewWEFontIDToNameProc(userRoutine) ((WEFontIDToNameUPP) (userRoutine))
#define NewWEFontNameToIDProc(userRoutine) ((WEFontNameToIDUPP) (userRoutine))
#define NewWEDrawTextProc(userRoutine) ((WEDrawTextUPP) (userRoutine))
#define NewWEPixelToCharProc(userRoutine) ((WEPixelToCharUPP) (userRoutine))
#define NewWECharToPixelProc(userRoutine) ((WECharToPixelUPP) (userRoutine))
#define NewWELineBreakProc(userRoutine) ((WELineBreakUPP) (userRoutine))
#define NewWEWordBreakProc(userRoutine) ((WEWordBreakUPP) (userRoutine))
#define NewWECharByteProc(userRoutine) ((WECharByteUPP) (userRoutine))
#define NewWECharTypeProc(userRoutine) ((WECharTypeUPP) (userRoutine))
#define NewWEEraseProc(userRoutine) ((WEEraseUPP) (userRoutine))
#define NewWEFluxProc(userRoutine) ((WEFluxUPP) (userRoutine))
#define NewWENewObjectProc(userRoutine) ((WENewObjectUPP) (userRoutine))
#define NewWEDisposeObjectProc(userRoutine) ((WEDisposeObjectUPP) (userRoutine))
#define NewWEDrawObjectProc(userRoutine) ((WEDrawObjectUPP) (userRoutine))
#define NewWEClickObjectProc(userRoutine) ((WEClickObjectUPP) (userRoutine))
#define NewWEStreamObjectProc(userRoutine) ((WEStreamObjectUPP) (userRoutine))

#define CallWEClickLoopProc(we, userRoutine) \
	(*(userRoutine))((we))
#define CallWEScrollProc(we, userRoutine) \
	(*(userRoutine))((we))
#define CallWETSMPreUpdateProc(we, userRoutine) \
	(*(userRoutine))((we))
#define CallWETSMPostUpdateProc(we, fixLength, inputAreaStart, inputAreaEnd, pinRangeStart, pinRangeEnd, userRoutine) \
	(*(userRoutine))((we), (fixLength), (inputAreaStart), (inputAreaEnd), (pinRangeStart), (pinRangeEnd))
#define CallWEPreTrackDragProc(drag, we, userRoutine) \
	(*(userRoutine))((drag), (we))
#define CallWETranslateDragProc(theDrag, theItem, requestedType, putDataHere, dropOffset, we, userRoutine) \
	(*(userRoutine))((theDrag), (theItem), (requestedType), (putDataHere), (dropOffset), (we))
#define CallWEHiliteDropAreaProc(theDrag, hiliteFlag, we, userRoutine) \
	(*(userRoutine))((theDrag), (hiliteFlag), (we))
#define CallWEFontIDToNameProc(fontID, fontName, userRoutine) \
	(*(userRoutine))((fontID), (fontName))
#define CallWEFontNameToIDProc(fontName, oldFontID, newFontID, userRoutine) \
	(*(userRoutine))((fontName), (oldFontID), (newFontID))
#define CallWEDrawTextProc(pText, textLength, slop, styleRunPosition, we, userRoutine) \
	(*(userRoutine))((pText), (textLength), (slop), (styleRunPosition), (we))
#define CallWEPixelToCharProc(pText, textLength, slop, pixelWidth, edge, styleRunPosition, hPos, we, userRoutine) \
	(*(userRoutine))((pText), (textLength), (slop), (pixelWidth), (edge), (styleRunPosition), (hPos), (we))
#define CallWECharToPixelProc(pText, textLength, slop, offset, direction, styleRunPosition, hPos, we, userRoutine) \
	(*(userRoutine))((pText), (textLength), (slop), (offset), (direction), (styleRunPosition), (hPos), (we))
#define CallWELineBreakProc(pText, textLength, textStart, textEnd, textWidth, textOffset, we, userRoutine) \
	(*(userRoutine))((pText), (textLength), (textStart), (textEnd), (textWidth), (textOffset), (we))
#define CallWEWordBreakProc(pText, textLength, offset, edge, breakOffsets, script, we, userRoutine) \
	(*(userRoutine))((pText), (textLength), (offset), (edge), (breakOffsets), (script), (we))
#define CallWECharByteProc(pText, textOffset, script, we, userRoutine) \
	(*(userRoutine))((pText), (textOffset), (script), (we))
#define CallWECharTypeProc(pText, textOffset, script, we, userRoutine) \
	(*(userRoutine))((pText), (textOffset), (script), (we))
#define CallWEEraseProc(area, we, userRoutine) \
	(*(userRoutine))((area), (we))
#define CallWEFluxProc(offset, delta, we, userRoutine) \
	(*(userRoutine))((offset), (delta), (we))
#define CallWENewObjectProc(defaultObjectSize, obj, userRoutine) \
	(*(userRoutine))((defaultObjectSize), (obj))
#define CallWEDisposeObjectProc(obj, userRoutine) \
	(*(userRoutine))((obj))
#define CallWEDrawObjectProc(destRect, obj, userRoutine) \
	(*(userRoutine))((destRect), (obj))
#define CallWEClickObjectProc(hitPt, modifiers, clickTime, obj, userRoutine) \
	(*(userRoutine))((hitPt), (modifiers), (clickTime), (obj))
#define CallWEStreamObjectProc(destKind, theType, putDataHere, obj, userRoutine) \
	(*(userRoutine))((destKind), (theType), (putDataHere), (obj))

#endif


/*	WASTE public calls */

/*	getting the shared library version number */

extern pascal UInt32 /* NumVersion */ WEVersion(void);

/*	creation and destruction */

extern pascal OSErr WENew(const LongRect *destRect, const LongRect *viewRect, UInt32 flags, WEReference *we);
extern pascal void WEDispose(WEReference we);

/*	getting variables */

extern pascal Handle WEGetText(WEReference we);
extern pascal SInt16 WEGetChar(SInt32 offset, WEReference we);
extern pascal SInt32 WEGetTextLength(WEReference we);
extern pascal SInt32 WEGetHeight(SInt32 startLine, SInt32 endLine, WEReference we);
extern pascal void WEGetSelection(SInt32 *selStart, SInt32 *selEnd, WEReference we);
extern pascal void WEGetDestRect(LongRect *destRect, WEReference we);
extern pascal void WEGetViewRect(LongRect *viewRect, WEReference we);
extern pascal Boolean WEIsActive(WEReference we);
extern pascal SInt32 WEOffsetToLine(SInt32 offset, WEReference we);
extern pascal void WEGetLineRange(SInt32 lineIndex, SInt32 *lineStart, SInt32 *lineEnd, WEReference we);
extern pascal SInt32 WECountLines(WEReference we);
extern pascal SInt32 WEOffsetToRun(SInt32 offset, WEReference we);
extern pascal void WEGetRunRange(SInt32 runIndex, SInt32 *runStart, SInt32 *runEnd, WEReference we);
extern pascal SInt32 WECountRuns(WEReference we);
extern pascal UInt16 WEGetClickCount(WEReference we);

/*	setting variables */

extern pascal void WESetSelection(SInt32 selStart, SInt32 selEnd, WEReference we);
extern pascal void WESetDestRect(const LongRect *destRect, WEReference we);
extern pascal void WESetViewRect(const LongRect *viewRect, WEReference we);

/*	accessing style run information */

extern pascal Boolean WEContinuousStyle(WEStyleMode *mode, TextStyle *ts, WEReference we);
extern pascal void WEGetRunInfo(SInt32 offset, WERunInfo *runInfo, WEReference we);
extern pascal Boolean WEGetRunDirection(SInt32 offset, WEReference we);

/*	converting byte offsets to screen position and vice versa */

extern pascal SInt32 WEGetOffset(const LongPt *thePoint, WEEdge *edge, WEReference we);
extern pascal void WEGetPoint(SInt32 offset, SInt16 direction, LongPt *thePoint, SInt16 *lineHeight, WEReference we);

/*	finding words, lines and paragraphs */

extern pascal void WEFindWord(SInt32 offset, WEEdge edge, SInt32 *wordStart, SInt32 *wordEnd, WEReference we);
extern pascal void WEFindLine(SInt32 offset, WEEdge edge, SInt32 *lineStart, SInt32 *lineEnd, WEReference we);
extern pascal void WEFindParagraph(SInt32 offset, WEEdge edge, SInt32 *paragraphStart, SInt32 *paragraphEnd, WEReference we);

/*	making a copy of a text range */

extern pascal OSErr WECopyRange(SInt32 rangeStart, SInt32 rangeEnd, Handle hText, StScrpHandle hStyles, WESoupHandle hSoup, WEReference we);

/*	getting and setting the alignment style */

extern pascal WEAlignment WEGetAlignment(WEReference we);
extern pascal void WESetAlignment(WEAlignment alignment, WEReference we);

/*	getting and setting the primary line direction */

extern pascal WEDirection WEGetDirection(WEReference we);
extern pascal void WESetDirection(WEDirection direction, WEReference we);

/*	recalculating line breaks, drawing and scrolling */

extern pascal OSErr WECalText(WEReference we);
extern pascal void WEUpdate(RgnHandle updateRgn, WEReference we);
extern pascal void WEScroll(SInt32 hOffset, SInt32 vOffset, WEReference we);
extern pascal void WESelView(WEReference we);

/*	handling activate / deactivate events */

extern pascal void WEActivate(WEReference we);
extern pascal void WEDeactivate(WEReference we);

/* 	handling key-down events */

extern pascal void WEKey(SInt16 key, EventModifiers modifiers, WEReference we);

/*	handling mouse-down events and mouse tracking */

extern pascal void WEClick(Point hitPt, EventModifiers modifiers, UInt32 clickTime, WEReference we);

/*	adjusting the cursor shape */

extern pascal Boolean WEAdjustCursor(Point mouseLoc, RgnHandle mouseRgn, WEReference we);

/*	blinking the caret */

extern pascal void WEIdle(UInt32 *maxSleep, WEReference we);

/*	modifying the text and the styles */

extern pascal OSErr WEInsert(const void *pText, SInt32 textLength, StScrpHandle hStyles, WESoupHandle hSoup, WEReference we);
extern pascal OSErr WEDelete(WEReference we);
extern pascal OSErr WESetStyle(WEStyleMode mode, const TextStyle *ts, WEReference we);
extern pascal OSErr WEUseStyleScrap(StScrpHandle hStyles, WEReference we);
extern pascal OSErr WEUseText(Handle hText, WEReference we);

/*	undo */

extern pascal OSErr WEUndo(WEReference we);
extern pascal void WEClearUndo(WEReference we);
extern pascal WEActionKind WEGetUndoInfo(Boolean *redoFlag, WEReference we);
extern pascal Boolean WEIsTyping(WEReference we);
extern pascal OSErr WEBeginAction(WEReference we);
extern pascal OSErr WEEndAction(WEActionKind actionKind, WEReference we);

/*	keeping track of changes */

extern pascal UInt32 WEGetModCount(WEReference we);
extern pascal void WEResetModCount(WEReference we);

/*	embedded objects */

extern pascal OSErr WEInstallObjectHandler(FlavorType objectType, WESelector handlerSelector, UniversalProcPtr handler, WEReference we);
extern pascal OSErr WEGetObjectHandler(FlavorType objectType, WESelector handlerSelector, UniversalProcPtr *handler, WEReference we);
extern pascal OSErr WEInsertObject(FlavorType objectType, Handle objectDataHandle, Point objectSize, WEReference we);
extern pascal OSErr WEGetSelectedObject(WEObjectReference *obj, WEReference we);
extern pascal SInt32 WEFindNextObject(SInt32 offset, WEObjectReference *obj, WEReference we);
extern pascal OSErr WEUseSoup(WESoupHandle hSoup, WEReference we);

/*	accessing embedded object attributes */

extern pascal FlavorType WEGetObjectType(WEObjectReference obj);
extern pascal Handle WEGetObjectDataHandle(WEObjectReference obj);
extern pascal Point WEGetObjectSize(WEObjectReference obj);
extern pascal WEReference WEGetObjectOwner(WEObjectReference obj);
extern pascal SInt32 WEGetObjectRefCon(WEObjectReference obj);
extern pascal void WESetObjectRefCon(WEObjectReference obj, SInt32 refCon);

/*	clipboard operations */

extern pascal OSErr WECut(WEReference we);
extern pascal OSErr WECopy(WEReference we);
extern pascal OSErr WEPaste(WEReference we);
extern pascal Boolean WECanPaste(WEReference we);

/*	Drag Manager support */

extern pascal RgnHandle WEGetHiliteRgn(SInt32 rangeStart, SInt32 rangeEnd, WEReference we);
extern pascal OSErr WETrackDrag(DragTrackingMessage message, DragReference drag, WEReference we);
extern pascal OSErr WEReceiveDrag(DragReference drag, WEReference we);
extern pascal Boolean WECanAcceptDrag(DragReference drag, WEReference we);
extern pascal Boolean WEDraggedToTrash(DragReference drag);

/*	font tables */

extern pascal OSErr WEBuildFontTable(WEFontTableHandle hFontTable, WEFontIDToNameUPP fontIDToNameProc, WEReference we);
extern pascal OSErr WEUpdateFontTable(WEFontTableHandle hFontTable, WEFontNameToIDUPP fontNameToIDProc, Boolean *wasChanged);
extern pascal OSErr WEUpdateStyleScrap(StScrpHandle hStyles, WEFontTableHandle hFontTable);

/*	Script Manager utilities */

extern pascal SInt16 WECharByte(SInt32 offset, WEReference we);
extern pascal SInt16 WECharType(SInt32 offset, WEReference we);

/*	Text Services Manager support */

extern pascal OSErr WEInstallTSMHandlers(void);
extern pascal OSErr WERemoveTSMHandlers(void);
extern pascal OSErr WEHandleTSMEvent(const AppleEvent *ae, AppleEvent *reply);
extern pascal void WEStopInlineSession(WEReference we);

/*	additional features */

extern pascal SInt16 WEFeatureFlag(SInt16 feature, SInt16 action, WEReference we);
extern pascal OSErr WEGetInfo(WESelector selector, void *info, WEReference we);
extern pascal OSErr WESetInfo(WESelector selector, const void *info, WEReference we);

/*	getting and setting user-defined info */

extern pascal OSErr WEGetUserInfo(WESelector tag, SInt32 *userInfo, WEReference we);
extern pascal OSErr WESetUserInfo(WESelector tag, SInt32 userInfo, WEReference we);

/*	long coordinate utilities */

extern pascal void WELongPointToPoint(const LongPt *lp, Point *p);
extern pascal void WEPointToLongPoint(Point p, LongPt *lp);
extern pascal void WESetLongRect(LongRect *lr, SInt32 left, SInt32 top, SInt32 right, SInt32 bottom);
extern pascal void WELongRectToRect(const LongRect *lr, Rect *r);
extern pascal void WERectToLongRect(const Rect *r, LongRect *lr);
extern pascal void WEOffsetLongRect(LongRect *lr, SInt32 hOffset, SInt32 vOffset);
extern pascal Boolean WELongPointInLongRect(const LongPt *lp, const LongRect *lr);

#if PRAGMA_IMPORT_SUPPORTED
#pragma import off
#endif

/* ShadowIRC: Not needed since we set this explicitly in the target settings
if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif
*/

#ifdef __cplusplus
}
#endif

#endif	/* _WASTE_ */
