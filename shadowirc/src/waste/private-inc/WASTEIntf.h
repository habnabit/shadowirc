/*
 *	WASTEIntf.h
 *
 *	WASTE PROJECT
 *  Internal (private) interface, version 1.3
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */

//	Toolbox #includes

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef UNIVERSAL_INTERFACES_VERSION
#error "You need Universal Headers version 2.1 or newer to compile WASTE"
#endif

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __GESTALT__
#include <Gestalt.h>
#endif

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef __FONTS__
#include <Fonts.h>
#endif

#ifndef __QUICKDRAWTEXT__
#include <QuickdrawText.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif

#ifndef __PALETTES__
#include <Palettes.h>
#endif

#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

#ifndef __SCRAP__
#include <Scrap.h>
#endif

#ifndef __SCRIPT__
#include <Script.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

#ifndef __LOWMEM__
#include <LowMem.h>
#endif

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef __DRAG__
#include <Drag.h>
#endif

#ifndef __TEXTSERVICES__
#include <TextServices.h>
#endif

#ifndef __FIXMATH__
#include <FixMath.h>
#endif

//	ANSI #includes

#ifndef _LIMITS
#include <limits.h>
#endif

//	other #includes

#ifndef __LONGCOORDINATES__
#ifndef _LongCoords_
#include "LongCoords.h"
#endif
#endif

//	there are slight differences between Universal Headers version 2.1.x and 3.0
//	that need to be addressed here, if WASTE is to support both sets of headers

#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x300)
	//	we're using Universal Headers 3.0 or later
	#define		kNullDrag							nil
#else
	//	we're using Universal Headers 2.1.x: we need to define some additional symbols
	#define		kNullDrag							0L
	#define		kDragTrackingEnterWindow			dragTrackingEnterWindow
	#define		kDragTrackingInWindow				dragTrackingInWindow
	#define		kDragTrackingLeaveWindow			dragTrackingLeaveWindow
	#define		kDragHasLeftSenderWindow			dragHasLeftSenderWindow
	#define		kDragStandardTranslucency			0L
	typedef		UInt32								DragImageFlags ;
	extern pascal OSErr SetDragImage(DragReference drag, PixMapHandle imagePixMap,
		RgnHandle imageRgn, Point imageOffsetPt, DragImageFlags imageFlags)
 		TWOWORDINLINE(0x7027,0xABED);
#endif

// MPW needs topLeft and botRight defined here
#ifndef topLeft
#define topLeft(r)              		(((Point *) &(r))[0])
#endif

#ifndef botRight
#define botRight(r)             		(((Point *) &(r))[1])
#endif

// pascal-like macros for testing, setting, clearing and inverting bits
#define BTST( FLAGS, BIT )				((FLAGS) &   (1UL << (BIT)))
#define BSET( FLAGS, BIT )  			((FLAGS) |=  (1UL << (BIT)))
#define BCLR( FLAGS, BIT )  			((FLAGS) &= ~(1UL << (BIT)))
#define BCHG( FLAGS, BIT )  			((FLAGS) ^=  (1UL << (BIT)))

// pascal-like macros for shifting bits
#define BSL( A, B )						(((SInt32) (A)) << (B))
#define BSR( A, B )						(((SInt32) (A)) >> (B))

// macros used in conjunction with the styleRunPosition parameter
#define IS_LEFTMOST_RUN(RUN)			((RUN) <= leftStyleRun)
#define IS_RIGHTMOST_RUN(RUN)			(! ((RUN) & 0x01))

// other macros
#define ABS(A) 							((A) > 0 ? (A) : -(A))
#define BLOCK_CLR(X)					_WEBlockClr(&(X), sizeof(X));

// define WASTE_VERSION is standard NumVersion format

#define WASTE_VERSION					0x01308000	/* 1.3 */

// set WASTE_SHARED_LIBRARY to 1 when building a CFM-based shared library
#ifndef WASTE_SHARED_LIBRARY
#define WASTE_SHARED_LIBRARY			0
#endif

//	make sure WASTE_SHARED_LIBRARY is false if building a non-CFM project
#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #if ( WASTE_SHARED_LIBRARY && ( ! TARGET_RT_MAC_CFM ) )
  #error "WASTE_SHARED_LIBRARY must be 0 in a classic 68K project"
  #endif
#else
#if ( WASTE_SHARED_LIBRARY && ( ! GENERATINGCFM ) )
#error "WASTE_SHARED_LIBRARY must be 0 in a classic 68K project"
#endif
#endif

// set WASTE_DEBUG to 1 to perform various consistency checks;
// errors will be reported with DebugStr()
// WARNING: when WASTE_DEBUG is set, WEIdle() calls can be extremely time-consuming
#ifndef WASTE_DEBUG
#define WASTE_DEBUG						0
#endif

// set WASTE_OBJECTS to 0 if you don't need embedded objects
#ifndef WASTE_OBJECTS
#define	WASTE_OBJECTS					0
#endif

// set WASTE_TSM_SUPPORT to 0 to disable support for the Text Services Manager
#ifndef WASTE_TSM_SUPPORT
#define WASTE_TSM_SUPPORT				1
#endif

// set WASTE_DRAG_AND_DROP to 0 to disable support for the Drag Manager
#ifndef WASTE_DRAG_AND_DROP
#define WASTE_DRAG_AND_DROP				1
#endif

// set WASTE_USE_UPPS to 0 if you don't need UPPs
// carbon builds are by definition powerpc-only, so no UPPs
#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #ifndef WASTE_USE_UPPS
  #define WASTE_USE_UPPS					TARGET_RT_MAC_CFM && !TARGET_API_MAC_CARBON
  #endif
#else
  #ifndef WASTE_USE_UPPS
  #define WASTE_USE_UPPS					GENERATINGCFM && !TARGET_API_MAC_CARBON
  #endif
#endif

// set WASTE_NO_SYNCH to 1 if you don't want WASTE to synchronize keyboard and fonts
#ifndef WASTE_NO_SYNCH
#define WASTE_NO_SYNCH					0
#endif

// set WASTE_NO_RO_CARET to 1 to hide the caret in read-only mode
#ifndef WASTE_NO_RO_CARET
#define WASTE_NO_RO_CARET				1
#endif

// set WASTE_IC_SUPPORT to 1 to use Dan Crevier's
// support of URL cmd-clicking through Internet Config
#ifndef WASTE_IC_SUPPORT
#define WASTE_IC_SUPPORT				1
#endif

// If WASTE_OBJECTS_ARE_GLYPHS is set to 0 (default), WEGetOffset returns kObjectEdge in
// the edge parameter when thePoint is in the middle half of an object: as a result,
// clicking in the middle of an object immediately _selects_ the object, so that a second
// click immediately following triggers the 'clik' callback (this can be handy to make
// sound objects play when they are double-clicked, for example).

// If WASTE_OBJECTS_ARE_GLYPHS is set to 1, WEGetOffset treats embedded objects
// like ordinary glyphs and never returns kObjectEdge in the edge parameter:
// as a result, clicking in the middle of an object always positions the caret either
// to the left or to the right of the object.

#ifndef WASTE_OBJECTS_ARE_GLYPHS
#define WASTE_OBJECTS_ARE_GLYPHS		0
#endif

// set WASTE_RESOLVE_FONT_DESIGNATORS to false if you don't want WASTE to
// map font designators (the special IDs 0 and 1 that identify the system
// and application fonts, respectively) to the actual font IDs.

#ifndef WASTE_RESOLVE_FONT_DESIGNATORS
#define WASTE_RESOLVE_FONT_DESIGNATORS	1
#endif

// set WASTE_WECALTEXT_DOES_REDRAW to 1 if you want WECalText()
// to automatically redraw all the text (as in WASTE 1.1 final)

#ifndef WASTE_WECALTEXT_DOES_REDRAW
#define WASTE_WECALTEXT_DOES_REDRAW		0
#endif

// set WASTE_TRANSLUCENT_DRAGS to 1 if you want to be able to
// use translucent dragging for text

#ifndef WASTE_TRANSLUCENT_DRAGS
#define WASTE_TRANSLUCENT_DRAGS			0
#endif

// macros for debugging

#if WASTE_DEBUG
#define WEASSERT(CONDITION, WARNING)	{ if (!(CONDITION)) DebugStr(WARNING); }
#else
#define WEASSERT(CONDITION, WARNING)
#endif

// determine if inline functions are supported

#if defined(__cplusplus) || defined(__MWERKS__)
#define INLINE inline
#else
#define INLINE static
#endif

// result codes
enum
{
	weCantUndoErr				=	-10015,	// undo buffer is clear (= errAECantUndo)
	weEmptySelectionErr			=	-10013,	// selection range is empty (= errAENoUserSelection)
	weUnknownObjectTypeErr		=	-9478,	// specified object type is not registered
	weObjectNotFoundErr			=	-9477,	// no object found at specified offset
	weReadOnlyErr				=	-9476,	// instance is read-only
	weMonoStyledErr				=	-9475,	// instance is mono-styled
	weNotHandledErr				=	-1708,  // please use default handling (= errAEEventNotHandled)
	weNewerVersionErr			=	-1706,	// need a newer version of WASTE (= errAENewerVersion)
	weCorruptDataErr			=	-1702,	// corrupt data (= errAECorruptData)
	weProtocolErr				=	-603,	// improper call order (= protocolErr)
	weUndefinedSelectorErr		=	-50,	// unknown selector
	weNoDragErr 				= 	128		// can be returned internally by _WEDrag
};

// values for WEInstallObjectHandler handlerSelector parameter
enum
{
	weNewHandler 		= 	'new ',
	weDisposeHandler 	= 	'free',
	weDrawHandler 		= 	'draw',
	weClickHandler 		= 	'clik',
	weStreamHandler		=	'strm'
};

// action kinds
enum
{
	weAKNone			=	0,		// null action
	weAKUnspecified		=	1,		// action of unspecified nature
	weAKTyping			=	2,		// some text has been typed in
	weAKCut				=	3,		// the selection range has been cut
	weAKPaste			=	4,		// something has been pasted
	weAKClear			=	5,		// the selection range has been deleted
	weAKDrag			=	6,		// drag and drop operation
	weAKSetStyle		=	7		// some style has been applied to a text range
};

// action flags
enum
{
	weAFIsRedo			=	0x0001,	// action saves edit state prior to a WEUndo call
	weAFDontSaveText	=	0x0002, // don't save text
	weAFDontSaveStyles	=	0x0004,	// don't save styles
	weAFDontSaveSoup	=	0x0008	// don't save soup
};

// destination kinds for streaming objects
enum
{
	weToScrap			=	0,
	weToDrag			=	1,
	weToSoup			=	2
};

// alignment styles
enum
{
	weFlushLeft 		=	-2,		// flush left
	weFlushRight		=	-1,		// flush right
	weFlushDefault		=	 0,		// flush according to system direction
	weCenter			=	 1,		// centered
	weJustify			=	 2		// fully justified
};

// dominant line directions
enum
{
	weDirRightToLeft	=	-1,		// right-to-left
	weDirLeftToRight	=	 0,		// left-to-right
	weDirDefault		=	 1		// according to system direction
};

#define IsRightToLeft(direction)	(((direction) == weDirRightToLeft) || (((direction) == weDirDefault) && (GetSysDirection() != 0)))

// values for the edge parameter
enum
{
	kLeadingEdge 		= 	-1,		// point is on the leading edge of a glyph
	kTrailingEdge 		= 	 0,		// point is on the trailing edge of a glyph
	kObjectEdge 		= 	 2		// point is inside an embedded object
};

// control character codes
enum
{
	kObjectMarker 		=	0x01,
	kBackspace 			=	0x08,
	kTab 				=	0x09,
	kEOL 				=	0x0D,
	kArrowLeft 			=	0x1C,
	kArrowRight 		=	0x1D,
	kArrowUp 			=	0x1E,
	kArrowDown 			=	0x1F,
	kSpace 				=  	0x20,
	kForwardDelete 		=	0x7F
};

// bit equates for the tsFlags field of the WETextStyle record
enum
{
	tsTSMHilite 		=	4,		// set if style run is part of active input area
	tsTSMSelected 		=	5,		// set for selected raw/converted text
	tsTSMConverted 		=	6,		// set for converted text, clear for raw text
	tsRightToLeft 		=	7		// set for right-to-left runs
};

// bit equates for the mode parameter in WESetStyle and WEContinuousStyle
enum
{
	kModeFont 			=	0,		// change font
	kModeFace 			=	1,		// change Quickdraw styles
	kModeSize 			=	2,		// change point size
	kModeColor 			=	3,		// change color
	kModeAddSize 		=	4,		// add tsSize to existing size
	kModeToggleFace 	=	5,		// toggle continuous styles rather than setting them
	kModeReplaceFace 	=	6,		// tsFace replaces existing styles outright
	kModePreserveScript =	7,		// apply font to runs of same script only
	kModeExtractSubscript = 8,		// apply font to eligible subruns of different script
	kModeFaceMask 		= 	9,		// change Quickdraw styles; use mask
	kModeObject 		= 	14,		// change tsObject field
	kModeFlags 			= 	15		// change tsFlags field
};

// values for the mode parameter in WESetStyle and WEContinuousStyle
enum
{
	weDoFont				=	1 << kModeFont,
	weDoFace				=	1 << kModeFace,
	weDoSize				=	1 << kModeSize,
	weDoColor				=	1 << kModeColor,
	weDoAll					=	weDoFont + weDoFace + weDoSize + weDoColor,
	weDoAddSize				=	1 << kModeAddSize,
	weDoToggleFace			=	1 << kModeToggleFace,
	weDoReplaceFace			=	1 << kModeReplaceFace,
	weDoPreserveScript		=	1 << kModePreserveScript,
	weDoSmartFont			=	weDoFont + weDoPreserveScript,
	weDoExtractSubscript	=	1 << kModeExtractSubscript,
	weDoFaceMask			=	1 << kModeFaceMask,
	weDoObject				=	1 << kModeObject,
	weDoFlags				=	1 << kModeFlags
};

// values for WEFeatureFlag action parameter
enum
{
	weBitSet 			=	 1,		// enables the specified feature
	weBitClear 			=	 0,		// disables the specified feature
	weBitTest 			=	-1,		// returns the current setting of the specified feature
	weBitToggle 		=	-2		// toggles the specified feature
};

// bit equates for the flags field in the WE record
enum
{
	weFHasColorQD 			=	31,		// Color Quickdraw is available
	weFHasDragManager 		=	30,		// the Drag Manager is available
	weFHasTextServices		=	29,		// the Text Services Manager is available
	weFNonRoman				=	28,		// at least one non-Roman script is enabled
	weFDoubleByte			=	27,		// a double-byte script is enabled
	weFBidirectional		=	26,		// a bidirectional script is enabled
	weFCaretRight			=	25,		// set if caret was drawn for R-L keyboard script
	weFCaretVisible			=	24,		// the caret is currently visible
	weFMouseTracking		=	23,		// set internally during mouse tracking
	weFAnchorIsEnd			=	22,		// anchor offset is selEnd
	weFUseNullStyle			=	21,		// a null style is associated with the empty selection
	weFActive				=	20,		// we're active
	weFHilited				=	19,		// true if text pane is highlighted (for Drag & Drop)
	weFCanAcceptDrag		=	18,		// the drag in the text pane can be accepted
	weFDragCaretVisible		=	17,		// drag caret is currently visible
	weFDestRectChanged		=	16,		// set if scroll callback needs to be called
	weFHasTranslucentDrags	=	15,		// SetDragImage is available
	weFAccumulateUndos		=	14		// don't clear undo stack before new change
};

// bit equates for the features field in the WE record
enum
{
	weFInhibitColor		=	31,		// draw in black & white only; ignore color
	weFMonoStyled		=	13,		// disallow style changes
	weFInhibitRedraw	=	12,		// don't redraw text
	weFDrawOffscreen	=	11,		// draw text offscreen for smoother visual results
	weFUseTempMem		=	10,		// use temporary memory for main data structures
	weFInhibitRecal		=	 9,		// don't recalculate line breaks and don't redraw text
	weFDragAndDrop		=	 8,		// support drag and drop
	weFIntCutAndPaste	=	 7,		// intelligent cut & paste
	weFUndoSupport		=	 6,		// support undo/redo
	weFReadOnly			=	 5,		// disallow editing
	weFOutlineHilite	=	 2,		// frame selection range when text pane is inactive
	weFAutoScroll		=	 0		// automatically scroll text when cursor is outside pane
};

// scrap types
enum
{
	kTypeText			=	'TEXT',
	kTypeStyles 		=	'styl',
	kTypeSoup 			=	'SOUP',
	kTypeFontTable		=	'FISH'
};

// selectors for WEGetInfo/WESetInfo
enum
{
	weCharByteHook			=	'cbyt',
	weCharTypeHook			=	'ctyp',
	weCharToPixelHook		=	'c2p ',
	weClickLoop				=	'clik',
	weCurrentDrag			=	'drag',
	weDrawTextHook			=	'draw',
	weEraseHook				=	'eras',
	weFluxProc				=	'flux',
	weHiliteDropAreaHook	=	'hidr',
	weLineArray				=	'line',
	weLineBreakHook			=	'lbrk',
	wePixelToCharHook		=	'p2c ',
	wePort					=	'port',
	wePreTrackDragHook		=	'ptrk',
	weRefCon				=	'refc',
	weRunArray				=	'runa',
	weScrollProc			=	'scrl',
	weStyleTable			=	'styl',
	weText					=	'text',
	weTranslateDragHook		=	'xdrg',
	weTranslucencyThreshold	=	'tluc',
	weTSMDocumentID			=	'tsmd',
	weTSMPostUpdate			=	'post',
	weTSMPreUpdate			=	'pre ',
	weURLHint				=	'urlh',
	weWordBreakHook			=	'wbrk'
};

// possible values returned by WEIntelligentPaste
enum
{
	weDontAddSpaces			=	 0,
	weAddSpaceOnLeftSide	=	-1,
	weAddSpaceOnRightSide	=	 1
};

// values for WEAllocate allocFlags parameter
enum
{
	kAllocClear			=	0x00000001,	// clear handle after allocation
	kAllocTemp			=	0x00000002	// use temporary memory if available
};

// other miscellaneous constants
enum
{
	kCaretWidth = 1,			// width of the caret, in pixels
	kMinFontSize = 1,			// minimum valid font size
	kMaxFontSize = SHRT_MAX,	// maximum valid font size
	kInvalidOffset = -1, 		// used to flag an invalid or nonexistent offset
	kUnknownObjectType = -1,	// returned by _WELookupObjectType for unknown flavors
	kTextMargin = 3,			// width of border area surrounding the text (in pixels)
	kMaxScrollDelta = 30,		// maximum scroll amount used by standard click loop
	kAutoScrollDelay = 10,		// delay before auto-scroll starts (in ticks)
	kDefaultObjectHeight = 32,	// default height for new objects
	kDefaultObjectWidth = 32,	// default width for new objects
	kAutoOrderingSize = 32		// size of stack-based format array used by _WESegmentLoop
};

#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #if PRAGMA_STRUCT_ALIGN
  #pragma options align=mac68k
  #endif
#else
  #if PRAGMA_ALIGN_SUPPORTED
  #pragma options align=mac68k
  #endif
#endif

typedef UInt16 WEStyleMode;
typedef SInt8 WEAlignment;
typedef SInt16 WEDirection;
typedef SInt8 WEEdge;
typedef SInt16 WEActionKind;
typedef UInt16 WEActionFlags;

// forward declarations

typedef struct WERec *WEPtr, **WEHandle;
typedef struct WEObjectDesc **WEObjectDescHandle;

#pragma mark ¥¥ ShadowIRC Additions

/*
	Additions for ShadowIRC
*/

enum extraFlags {
	kBackColor = 1 << 0,
	kInverse = 1 << 1
};

struct mySTElement {
	short 							stCount;					/*number of runs in this style*/
	short 							stHeight;					/*line height*/
	short 							stAscent;					/*font ascent*/
	short 							stFont;						/*font (family) number*/
	StyleField 						stFace;						/*character Style*/
	short 							stSize;						/*size in points*/
	RGBColor 						stColor;					/*absolute (RGB) color*/
	RGBColor						stBackColor;
	long extraFlags;
};
typedef struct mySTElement				mySTElement;

typedef mySTElement 						myTEStyleTable[1777];
typedef mySTElement *						mySTPtr;
typedef mySTPtr *							mySTHandle;

struct myTextStyle {
	short 							tsFont;						/*font (family) number*/
	StyleField 						tsFace;						/*character Style*/
	short 							tsSize;						/*size in point*/
	RGBColor 						tsColor;					/*absolute (RGB) color*/
	RGBColor						tsBackColor;
	long extraFlags;
};
typedef struct myTextStyle				myTextStyle;
typedef myTextStyle *						myTextStylePtr;
typedef myTextStylePtr *					myTextStyleHandle;

struct myScrpSTElement {
	long 							scrpStartChar;				/*starting character position*/
	short 							scrpHeight;
	short 							scrpAscent;
	short 							scrpFont;
	StyleField 						scrpFace;					/*unpacked byte*/
	short 							scrpSize;
	RGBColor 						scrpColor;
	RGBColor						scrpBackColor;
	long extraFlags;
};
typedef struct myScrpSTElement			myScrpSTElement;
/* ARRAY [0..1600] OF ScrpSTElement */

typedef myScrpSTElement 					myScrpSTTable[1601];

struct myStScrpRec {
	short 							scrpNStyles;				/*number of styles in scrap*/
	myScrpSTTable 					scrpStyleTab;				/*table of styles for scrap*/
};
typedef struct myStScrpRec				myStScrpRec;

typedef myStScrpRec *						myStScrpPtr;
typedef myStScrpPtr *						myStScrpHandle;


/*
	End of Additions for ShadowIRC
*/

#pragma mark ¥¥ End of ShadowIRC Additions

typedef struct WETextStyle
{
	SInt16 tsFont;					// font family number
	Style tsFace;					// set of Quickdraw styles
	UInt8 tsFlags;					// flags (FOR INTERNAL USE ONLY)
	SInt16 tsSize;					// font size, in integer points
	RGBColor tsColor;				// absolute red-green-blue color
	RGBColor tsBackColor;
	long extraFlags;
#if WASTE_OBJECTS
	WEObjectDescHandle tsObject;	// embedded object reference
#endif
} WETextStyle;

typedef struct WERunAttributes
{
	SInt16 runHeight;			// style run height (ascent + descent + leading)
	SInt16 runAscent;			// font ascent
	WETextStyle runStyle;		// text style
} WERunAttributes;

typedef struct WERunInfo
{
	SInt32 runStart;			// byte offset to first character of this style run
	SInt32 runEnd;				// byte offset to first character of next style run
	WERunAttributes runAttrs;	// run attributes
} WERunInfo;

typedef struct QDEnvironment
{
	GrafPtr envPort;
	PenState envPen;
	WETextStyle envStyle;
	SInt16 envMode;
} QDEnvironment;

typedef struct WERunArrayEntry
{
	SInt32 runStart;			// offset to first character in style run
	SInt32 styleIndex;			// index into style table
} WERunArrayEntry, **WERunArrayHandle;

typedef struct TERunAttributes
{
	SInt16 runHeight;
	SInt16 runAscent;
	myTextStyle runTEStyle;
} TERunAttributes;

typedef struct TEStyleScrapElement
{
	SInt32 scrpStartChar;
	TERunAttributes scrpTEAttrs;
} TEStyleScrapElement;

typedef struct TEStyleScrap
{
	SInt16 scrpNStyles;
	TEStyleScrapElement scrpStyleTab[ kVariableLengthArray ];
} TEStyleScrap, **TEStyleScrapHandle;

typedef struct WEStyleTableEntry
{
	SInt32 refCount;			// reference count
	WERunAttributes info;		// style information
} WEStyleTableEntry, **WEStyleTableHandle;

typedef struct WELineRec
{
	SInt32 lineStart;			// byte offset to first character in line
	SInt32 lineOrigin;			// pixel offset from destRect.top
	SInt16 lineAscent;			// maximum font ascent for this line
	SInt16 lineSlop;			// extra pixels needed to fill up the line
	Fixed lineJustAmount;		// normalized slop value, used for justification
} WELineRec, **WELineArrayHandle;

typedef struct WEAction
{
	WEHandle hOwner;			// handle to associated WE instance
	struct WEAction **hNext;	// used to keep a linked list of actions
	Handle hText;				// handle to saved text
	Handle hStyles;				// handle to saved styles
	Handle hSoup;				// handle to saved soup
	SInt32 delRangeStart;		// start of range to delete
	SInt32 delRangeLength;		// length of range to delete
	SInt32 insRangeLength;		// lenfth of range to insert
	SInt32 hiliteStart;			// start of range to hilite
	SInt32 hiliteEnd;			// end of range to hilite
	WEActionKind actionKind;	// identifies event that caused this action to be pushed
	WEActionFlags actionFlags;	// miscellaneous flags
} WEAction, **WEActionHandle;

// A WESoup record is a static description of an object embedded in the text.
// The 'SOUP' data type is just a collection of WESoup records, each followed
// by the corresponding object data.
// This data type complements the standard TEXT/styl pair.

typedef struct WESoup
{
	SInt32 soupOffset;			// insertion offset for this object
	FlavorType soupType;		// 4-letter tag identifying object type
	SInt32 soupReserved1;		// reserved for future use; set to zero
	Size soupDataSize;			// size of object data following this record
	Point soupSize;				// object height and width, in pixels
	SInt32 soupReserved2;		// reserved for future use; set to zero
} WESoup;

// callback prototypes

typedef pascal Boolean (*WEClickLoopProcPtr)(WEHandle hWE);
typedef pascal void (*WEScrollProcPtr)(WEHandle hWE);
typedef pascal void (*WETSMPreUpdateProcPtr)(WEHandle hWE);
typedef pascal void (*WETSMPostUpdateProcPtr)(WEHandle hWE,
		SInt32 fixLength, SInt32 inputAreaStart, SInt32 inputAreaEnd,
		SInt32 pinRangeStart, SInt32 pinRangeEnd);
typedef pascal OSErr (*WEPreTrackDragProcPtr)(DragReference drag, WEHandle hWE);
typedef pascal OSErr (*WETranslateDragProcPtr)(DragReference drag,
		ItemReference dragItem, FlavorType requestedType, Handle putDataHere,
		SInt32 dropOffset, WEHandle hWE);
typedef pascal OSErr (*WEHiliteDropAreaProcPtr)(DragReference drag,
		Boolean hiliteFlag, WEHandle hWE);
typedef pascal OSErr (*WEFontIDToNameProcPtr)(SInt16 fontID, Str255 fontName);
typedef pascal OSErr (*WEFontNameToIDProcPtr)(ConstStr255Param fontName, SInt16 oldFontID, SInt16 *newFontID);
typedef pascal void (*WEDrawTextProcPtr)(Ptr pText, SInt32 textLength, Fixed slop,
		JustStyleCode styleRunPosition, WEHandle hWE);
typedef pascal SInt32 (*WEPixelToCharProcPtr)(Ptr pText, SInt32 textLength, Fixed slop,
		Fixed *pixelWidth, WEEdge *edge, JustStyleCode styleRunPosition, Fixed hPos, WEHandle hWE);
typedef pascal SInt16 (*WECharToPixelProcPtr)(Ptr pText, SInt32 textLength, Fixed slop,
		SInt32 offset, SInt16 direction, JustStyleCode styleRunPosition, SInt16 hPos, WEHandle hWE);
typedef pascal StyledLineBreakCode (*WELineBreakProcPtr)(Ptr pText, SInt32 textLength,
		SInt32 textStart, SInt32 textEnd, Fixed *textWidth, SInt32 *textOffset, WEHandle hWE);
typedef pascal void (*WEWordBreakProcPtr)(Ptr pText, SInt16 textLength, SInt16 offset,
		WEEdge edge, OffsetTable breakOffsets, ScriptCode script, WEHandle hWE);
typedef pascal SInt16 (*WECharByteProcPtr)(Ptr pText, SInt16 textOffset, ScriptCode script,
		WEHandle hWE);
typedef pascal SInt16 (*WECharTypeProcPtr)(Ptr pText, SInt16 textOffset, ScriptCode script, WEHandle hWE);
typedef pascal void (*WEEraseProcPtr)(const Rect *area, WEHandle hWE);
typedef pascal void (*WEFluxProcPtr)(SInt32 offset, SInt32 delta, WEHandle hWE);
typedef pascal OSErr (*WENewObjectProcPtr)(Point *defaultObjectSize,
		WEObjectDescHandle hObjectDesc);
typedef pascal OSErr (*WEDisposeObjectProcPtr)(WEObjectDescHandle hObjectDesc);
typedef pascal OSErr (*WEDrawObjectProcPtr)(const Rect *destRect,
		WEObjectDescHandle hObjectDesc);
typedef pascal Boolean (*WEClickObjectProcPtr)(Point hitPt, EventModifiers modifiers, UInt32 clickTime,
		WEObjectDescHandle hObjectDesc);
typedef pascal OSErr (*WEStreamObjectProcPtr)(SInt16 destKind, FlavorType *theType, Handle putDataHere, WEObjectDescHandle hObjectDesc);


// UPP proc info

enum
{
	uppWEClickLoopProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWEScrollProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWETSMPreUpdateProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWETSMPostUpdateProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEHandle /*hWE*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*fixLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt32 /*inputAreaStart*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(SInt32 /*inputAreaEnd*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(SInt32 /*pinRangeStart*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(SInt32 /*pinRangeEnd*/))),
	uppWEPreTrackDragProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(DragReference /*drag*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWETranslateDragProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(DragReference /*drag*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(ItemReference /*dragItem*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(FlavorType /*requestedType*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(Handle /*putDataHere*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(SInt32 /*dropOffset*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWEHiliteDropAreaProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(DragReference /*drag*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(Boolean /*hiliteFlag*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
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
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWEPixelToCharProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt32)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(Fixed /*slop*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(Fixed * /*pixelWidth*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(WEEdge * /*edge*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(JustStyleCode /*styleRunPosition*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(Fixed /*hPos*/)))
		| STACK_ROUTINE_PARAMETER(8,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWECharToPixelProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt16)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(Fixed /*slop*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(SInt32 /*offset*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(SInt16 /*direction*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(JustStyleCode /*styleRunPosition*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(SInt16 /*hPos*/)))
		| STACK_ROUTINE_PARAMETER(8,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWELineBreakProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(StyledLineBreakCode )))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt32 /*textStart*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(SInt32 /*textEnd*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(Fixed * /*textWidth*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(SInt32 * /*textOffset*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWEWordBreakProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*textLength*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(SInt16 /*offset*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEEdge /*edge*/)))
		| STACK_ROUTINE_PARAMETER(5,SIZE_CODE(sizeof(OffsetTable * /*breakOffsets*/)))
		| STACK_ROUTINE_PARAMETER(6,SIZE_CODE(sizeof(ScriptCode /*script*/)))
		| STACK_ROUTINE_PARAMETER(7,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWECharByteProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt16 )))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*textOffset*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(ScriptCode /*script*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWECharTypeProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(SInt16 )))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Ptr /*pText*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*textOffset*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(ScriptCode /*script*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWEEraseProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(const Rect * /*area*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWEFluxProcInfo = kPascalStackBased
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(SInt32 /*offset*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt32 /*delta*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(WEHandle /*hWE*/))),
	uppWENewObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Point * /*defaultObjectSize*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEObjectDescHandle /*hObjectDesc*/))),
	uppWEDisposeObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(WEObjectDescHandle /*hObjectDesc*/))),
	uppWEDrawObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(const Rect * /*destRect*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(WEObjectDescHandle /*hObjectDesc*/))),
	uppWEClickObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))
		| STACK_ROUTINE_PARAMETER(1,SIZE_CODE(sizeof(Point /*hitPt*/)))
		| STACK_ROUTINE_PARAMETER(2,SIZE_CODE(sizeof(SInt16 /*modifiers*/)))
		| STACK_ROUTINE_PARAMETER(3,SIZE_CODE(sizeof(UInt32 /*clickTime*/)))
		| STACK_ROUTINE_PARAMETER(4,SIZE_CODE(sizeof(WEObjectDescHandle /*hObjectDesc*/))),
	uppWEStreamObjectProcInfo = kPascalStackBased
		| RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(SInt16 /*destKind*/)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(FlavorType * /*theType*/)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Handle /*putDataHere*/)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(WEObjectDescHandle /*hObjectDesc*/)))
};


/*	UPPs, NewÅProc macros & CallÅProc macros */

/*
	NOTE:
    For compatibility with the Pascal version, CallÅProc macros take the form:

		CallFooProc(..., userRoutine)

	instead of:

		CallFooProc(userRoutine, ...)

*/

#if WASTE_USE_UPPS

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
typedef UniversalProcPtr WEEraseUPP;
typedef UniversalProcPtr WEFluxUPP;
typedef UniversalProcPtr WENewObjectUPP;
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

#define CallWEClickLoopProc(hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEClickLoopProcInfo, (hWE))
#define CallWEScrollProc(hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEScrollProcInfo, (hWE))
#define CallWETSMPreUpdateProc(hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWETSMPreUpdateProcInfo, (hWE))
#define CallWETSMPostUpdateProc(hWE, fixLength, inputAreaStart, inputAreaEnd, pinRangeStart, pinRangeEnd, userRoutine) \
	CallUniversalProc((userRoutine), uppWETSMPostUpdateProcInfo, (hWE), (fixLength), (inputAreaStart), (inputAreaEnd), (pinRangeStart), (pinRangeEnd))
#define CallWEPreTrackDragProc(drag, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEPreTrackDragProcInfo, (drag), (hWE))
#define CallWETranslateDragProc(drag, dragItem, requestedType, putDataHere, dropOffset, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWETranslateDragProcInfo, (drag), (dragItem), (requestedType), (putDataHere), (dropOffset), (hWE))
#define CallWEHiliteDropAreaProc(drag, hiliteFlag, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEHiliteDropAreaProcInfo, (drag), (hiliteFlag), (hWE))
#define CallWEFontIDToNameProc(fontID, fontName, userRoutine) \
	CallUniversalProc((userRoutine), uppWEFontIDToNameProcInfo, (fontID), (fontName))
#define CallWEFontNameToIDProc(fontName, oldFontID, fontID, userRoutine) \
	CallUniversalProc((userRoutine), uppWEFontNameToIDProcInfo, (fontName), (oldFontID), (fontID))
#define CallWEDrawTextProc(pText, textLength, slop, styleRunPosition, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEDrawTextProcInfo, (pText), (textLength), (slop), (styleRunPosition), (hWE))
#define CallWEPixelToCharProc(pText, textLength, slop, pixelWidth, edge, styleRunPosition, hPos, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEPixelToCharProcInfo, (pText), (textLength), (slop), (pixelWidth), (edge), (styleRunPosition), (hPos), (hWE))
#define CallWECharToPixelProc(pText, textLength, slop, offset, direction, styleRunPosition, hPos, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWECharToPixelProcInfo, (pText), (textLength), (slop), (offset), (direction), (styleRunPosition), (hPos), (hWE))
#define CallWELineBreakProc(pText, textLength, textStart, textEnd, textWidth, textOffset, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWELineBreakProcInfo, (pText), (textLength), (textStart), (textEnd), (textWidth), (textOffset), (hWE))
#define CallWEWordBreakProc(pText, textLength, offset, edge, breakOffsets, script, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEWordBreakProcInfo, (pText), (textLength), (offset), (edge), (breakOffsets), (script), (hWE))
#define CallWECharByteProc(pText, textOffset, script, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWECharByteProcInfo, (pText), (textOffset), (script), (hWE))
#define CallWECharTypeProc(pText, textOffset, script, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWECharTypeProcInfo, (pText), (textOffset), (script), (hWE))
#define CallWEEraseProc(area, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEEraseProcInfo, (area), (hWE))
#define CallWEFluxProc(offset, delta, hWE, userRoutine) \
	CallUniversalProc((userRoutine), uppWEFluxProcInfo, (offset), (delta), (hWE))
#define CallWENewObjectProc(defaultObjectSize, hObjectDesc, userRoutine) \
	CallUniversalProc((userRoutine), uppWENewObjectProcInfo, (defaultObjectSize), (hObjectDesc))
#define CallWEDisposeObjectProc(hObjectDesc, userRoutine) \
	CallUniversalProc((userRoutine), uppWEDisposeObjectProcInfo, (hObjectDesc))
#define CallWEDrawObjectProc(destRect, hObjectDesc, userRoutine) \
	CallUniversalProc((userRoutine), uppWEDrawObjectProcInfo, (destRect), (hObjectDesc))
#define CallWEClickObjectProc(hitPt, modifiers, clickTime, hObjectDesc, userRoutine) \
	CallUniversalProc((userRoutine), uppWEClickObjectProcInfo, (hitPt), (modifiers), (clickTime), (hObjectDesc))
#define CallWEStreamObjectProc(destKind, theType, putDataHere, hObjectDesc, userRoutine) \
	CallUniversalProc((userRoutine), uppWEStreamObjectProcInfo, (destKind), (theType), (putDataHere), (hObjectDesc))

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

#define CallWEClickLoopProc(hWE, userRoutine) \
	(*(userRoutine))((hWE))
#define CallWEScrollProc(hWE, userRoutine) \
	(*(userRoutine))((hWE))
#define CallWETSMPreUpdateProc(hWE, userRoutine) \
	(*(userRoutine))((hWE))
#define CallWETSMPostUpdateProc(hWE, fixLength, inputAreaStart, inputAreaEnd, pinRangeStart, pinRangeEnd, userRoutine) \
	(*(userRoutine))((hWE), (fixLength), (inputAreaStart), (inputAreaEnd), (pinRangeStart), (pinRangeEnd))
#define CallWEPreTrackDragProc(drag, hWE, userRoutine) \
	(*(userRoutine))((drag), (hWE))
#define CallWETranslateDragProc(drag, dragItem, requestedType, putDataHere, dropOffset, hWE, userRoutine) \
	(*(userRoutine))((drag), (dragItem), (requestedType), (putDataHere), (dropOffset), (hWE))
#define CallWEHiliteDropAreaProc(drag, hiliteFlag, hWE, userRoutine) \
	(*(userRoutine))((drag), (hiliteFlag), (hWE))
#define CallWEFontIDToNameProc(fontID, fontName, userRoutine) \
	(*(userRoutine))((fontID), (fontName))
#define CallWEFontNameToIDProc(fontName, oldFontID, fontID, userRoutine) \
	(*(userRoutine))((fontName), (oldFontID), (fontID))
#define CallWEDrawTextProc(pText, textLength, slop, styleRunPosition, hWE, userRoutine) \
	(*(userRoutine))((pText), (textLength), (slop), (styleRunPosition), (hWE))
#define CallWEPixelToCharProc(pText, textLength, slop, pixelWidth, edge, styleRunPosition, hPos, hWE, userRoutine) \
	(*(userRoutine))((pText), (textLength), (slop), (pixelWidth), (edge), (styleRunPosition), (hPos), (hWE))
#define CallWECharToPixelProc(pText, textLength, slop, offset, direction, styleRunPosition, hPos, hWE, userRoutine) \
	(*(userRoutine))((pText), (textLength), (slop), (offset), (direction), (styleRunPosition), (hPos), (hWE))
#define CallWELineBreakProc(pText, textLength, textStart, textEnd, textWidth, textOffset, hWE, userRoutine) \
	(*(userRoutine))((pText), (textLength), (textStart), (textEnd), (textWidth), (textOffset), (hWE))
#define CallWEWordBreakProc(pText, textLength, offset, edge, breakOffsets, script, hWE, userRoutine) \
	(*(userRoutine))((pText), (textLength), (offset), (edge), (breakOffsets), (script), (hWE))
#define CallWECharByteProc(pText, textOffset, script, hWE, userRoutine) \
	(*(userRoutine))((pText), (textOffset), (script), (hWE))
#define CallWECharTypeProc(pText, textOffset, script, hWE, userRoutine) \
	(*(userRoutine))((pText), (textOffset), (script), (hWE))
#define CallWEEraseProc(area, hWE, userRoutine) \
	(*(userRoutine))((area), (hWE))
#define CallWEFluxProc(offset, delta, hWE, userRoutine) \
	(*(userRoutine))((offset), (delta), (hWE))
#define CallWENewObjectProc(defaultObjectSize, hObjectDesc, userRoutine) \
	(*(userRoutine))((defaultObjectSize), (hObjectDesc))
#define CallWEDisposeObjectProc(hObjectDesc, userRoutine) \
	(*(userRoutine))((hObjectDesc))
#define CallWEDrawObjectProc(destRect, hObjectDesc, userRoutine) \
	(*(userRoutine))((destRect), (hObjectDesc))
#define CallWEClickObjectProc(hitPt, modifiers, clickTime, hObjectDesc, userRoutine) \
	(*(userRoutine))((hitPt), (modifiers), (clickTime), (hObjectDesc))
#define CallWEStreamObjectProc(destKind, theType, putDataHere, hObjectDesc, userRoutine) \
	(*(userRoutine))((destKind), (theType), (putDataHere), (hObjectDesc))

#endif

typedef Boolean (*WESegmentLoopProcPtr)
	(
		WELineRec *pLine,
		const WERunAttributes *pAttrs,
		Ptr pSegment,
		SInt32 segmentStart,
		SInt32 segmentLength,
		JustStyleCode styleRunPosition,
		WEHandle hWE,
		void *callbackData
	);

typedef FourCharCode WESelector;

typedef struct WEFieldDescriptor
{
	SInt16 fOffset;
	SInt16 fLength;
} WEFieldDescriptor;

typedef struct WELookupTable
{
	WESelector selector;
	WEFieldDescriptor desc;
} WELookupTable;

typedef struct WEUserInfoEntry
{
	WESelector tag;
	SInt32 info;
} WEUserInfoEntry;

typedef struct WEOHTableEntry
{
	FlavorType objectType;
	WENewObjectUPP newHandler;
	WEDisposeObjectUPP freeHandler;
	WEDrawObjectUPP drawHandler;
	WEClickObjectUPP clickHandler;
	WEStreamObjectUPP streamHandler;
	SInt32 refCon;
} WEOHTableEntry, **WEOHTableHandle;

typedef struct WEObjectDesc
{
	FlavorType objectType;				// 4-letter tag identifying object type
	Handle objectDataHandle;			// handle to object data
	Point objectSize;					// object height and width, in pixels
	WEOHTableHandle objectTable;		// handle to object handler table
	SInt32 objectIndex;					// index into object handler table
	WEHandle objectOwner;				// handle to owner WE instance
	SInt32 objectRefCon;				// free for use by object handlers
} WEObjectDesc;

typedef struct WERec
{
	GrafPtr port;							// graphics port text is drawn into
	Handle hText;							// handle to the text
	WELineArrayHandle hLines;				// handle to the line array
	WEStyleTableHandle hStyles;				// handle to the style table
	WERunArrayHandle hRuns;					// handle to the style run array
	SInt32 textLength;						// length of text
	SInt32 nLines;							// number of lines
	SInt32 nStyles;							// number of unique styles in the style table
	SInt32 nRuns;							// number of style runs
	LongRect viewRect;						// view rectangle, all drawing is clipped to this
	LongRect destRect;						// destination rectangle
	SInt32 selStart;						// start of selection range
	SInt32 selEnd;							// end of selection range
	UInt32 flags;							// 32 bits of miscellaneous flags (private)
	UInt32 features;						// 32 bit of feature flags (public)
	UInt32 caretTime;						// time of most recent caret drawing, in ticks
	UInt32 clickTime;						// time of most recent click, in ticks
	SInt32 clickLoc;						// byte offset of most recent click
	SInt32 anchorStart;						// start offset of anchor word/line
	SInt32 anchorEnd;						// end offset of anchor word/line
	char firstByte;							// first byte of a double-byte character
	WEEdge clickEdge;						// edge of character hit by most recent click
	UInt8 clickCount;						// multiple click count
	WEAlignment alignment;					// alignment style
	WEDirection direction;					// dominant line direction
	SInt16 filler1;							// unused
	GrafPtr offscreenPort;					// offscreen graphics world
	RgnHandle viewRgn;						// handle to the view region
	void *refCon;							// reference value for client use
	Handle hUserInfo;						// more junk for client use
	TSMDocumentID tsmReference;				// reference value for the text services manager
	SInt32 tsmAreaStart;					// start of active input area (for TSM)
	SInt32 tsmAreaEnd;						// end of active input area
	DragReference currentDrag;				// reference of drag being tracked by _WEDrag
	SInt32 dragCaretOffset;					// offset to caret displayed during a drag
	WEActionHandle hActionStack;			// action stack for undo
	UInt32 modCount;						// modification count
	WEClickLoopUPP clickLoop;				// click loop callback
	WEScrollUPP scrollProc;					// scroll callback
	WETSMPreUpdateUPP tsmPreUpdate;			// TSM pre-update callback
	WETSMPostUpdateUPP tsmPostUpdate;		// TSM post-update callback
	WEPreTrackDragUPP preTrackDragHook;		// pre-TrackDrag hook
	WETranslateDragUPP translateDragHook;	// drag translation hook
	WEHiliteDropAreaUPP hiliteDropAreaHook;	// drop area highlighting hook
	WEDrawTextUPP drawTextHook;				// hook for drawing text
	WEPixelToCharUPP pixelToCharHook;		// hook for hit-testing
	WECharToPixelUPP charToPixelHook;		// hook for locating glyph position
	WELineBreakUPP lineBreakHook;			// hook for finding line breaks
	WEWordBreakUPP wordBreakHook;			// hook for finding word breaks
	WECharByteUPP charByteHook;				// hook for finding character byte type
	WECharTypeUPP charTypeHook;				// hook for finding character type
	WEEraseUPP eraseHook;					// hook for erasing background
	WEFluxUPP fluxProc;						// flux callback
	WERunAttributes nullStyle;				// style for null selection
	WEOHTableHandle hObjectHandlerTable;	// handle to object handler table for this instance
	SInt32 translucencyThreshold;			// use translucent drags unless selection area exceeds this
	StringHandle hURLHint;					// hint string for slack URLs
} WERec;

struct SLDrawData
{
	GrafPtr screenPort;
	GDHandle screenDevice;
	PixMapHandle offscreenPixels;	// offscreen pixel map
	Rect bounds;					// bounds of offscreen buffer, in global coords
	Rect lineRect;					// rect enclosing current line
	Rect drawRect;					// visible portion of line rect
	Boolean usingColor;				// true if drawing in color
	Boolean usingOffscreen;			// true if an offscreen graphics world has been set up
	Boolean drawingOffscreen;		// true if actually drawing to an offscreen buffer
	Boolean doErase;				// true if line rectangle should be erased before drawing
};

struct SLCalcSlopData
{
	SInt16 lineWidth;
	SInt16 totalSlop;
	Fixed totalProportion;
};

struct SLPixelToCharData
{
	Fixed hPos;
	Fixed pixelWidth;
	SInt32 offset;
	WEEdge edge;
};

struct SLCharToPixelData
{
	LongPt *thePoint;
	SInt32 offset;
	SInt16 direction;
	Boolean lineDir;	// is the line direction RL?
	Boolean rightEdge;	// are we getting the right edge of the line?
};

struct SLCollectHiliteRgnData
{
	SInt32 rangeStart;
	SInt32 rangeEnd;
	SInt16 hPos;
};

struct SLCrossDirectionBoundaryData
{
	SInt32 oldOffset;		//	offset before crossing direction boundary
	SInt32 newOffset;		//	offset after crossing direction boundary
	Boolean movingRight;	//	moving insertion point to the right?
	Boolean isDone;
};

struct FormatOrderData
{
	SInt32 firstRun;
	WEHandle hWE;
};

#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #if PRAGMA_STRUCT_ALIGN
  #pragma options align=reset
  #endif
#else
  #if PRAGMA_ALIGN_SUPPORTED
  #pragma options align=reset
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

//	PUBLIC ROUTINES

//	WEAccessors.c (public)
pascal SInt32 WEOffsetToLine(SInt32 offset, WEHandle hWE);
pascal SInt32 WECountLines(WEHandle hWE);
pascal void WEGetLineRange(SInt32 lineIndex, SInt32 *lineStart, SInt32 *lineEnd, WEHandle hWE);
pascal SInt32 WEOffsetToRun (SInt32 offset, WEHandle hWE);
pascal SInt32 WECountRuns(WEHandle hWE);
pascal void WEGetRunRange(SInt32 runIndex, SInt32 *runStart, SInt32 *runEnd, WEHandle hWE);
pascal Boolean WEGetRunDirection(SInt32 offset, WEHandle hWE);
pascal void WEGetRunInfo(SInt32 offset, WERunInfo *info, WEHandle hWE);
pascal void WESetDestRect(const LongRect *destRect, WEHandle hWE);
pascal void WEGetDestRect(LongRect *destRect, WEHandle hWE);
pascal void WESetViewRect(const LongRect *viewRect, WEHandle hWE);
pascal void WEGetViewRect(LongRect *viewRect, WEHandle hWE);
pascal WEAlignment WEGetAlignment(WEHandle hWE);
pascal WEDirection WEGetDirection(WEHandle hWE);
pascal void WEGetSelection(SInt32 *selStart, SInt32 *selEnd, WEHandle hWE);
pascal SInt32 WEGetTextLength(WEHandle hWE);
pascal SInt32 WEGetHeight(SInt32 startLine, SInt32 endLine, WEHandle hWE);
pascal Handle WEGetText(WEHandle hWE);
pascal SInt16 WEGetChar(SInt32 offset, WEHandle hWE);
pascal SInt16 WEFeatureFlag(SInt16 feature, SInt16 action, WEHandle hWE);
pascal UInt32 WEVersion(void);

//	WEBirthDeath.c (public)
pascal OSErr WENew(const LongRect *destRect, const LongRect *viewRect, UInt32 features, WEHandle *hWE);
pascal void WEDispose(WEHandle hWE);
pascal OSErr WEUseText(Handle text, WEHandle hWE);

//	WEDrawing.c (public)
pascal OSErr WEGetSelectedObject(WEObjectDescHandle *hObjectDesc, WEHandle hWE);
pascal SInt32 WEFindNextObject(SInt32 offset, WEObjectDescHandle *hObjectDesc, WEHandle hWE);
pascal Boolean WEContinuousStyle(WEStyleMode *mode, TextStyle *ts, WEHandle hWE);

//	WEFontTables.c (public)
pascal OSErr WEBuildFontTable(Handle hFontTable, WEFontIDToNameUPP fontIDToNameProc, WEHandle hWE);
pascal OSErr WEUpdateFontTable(Handle hFontTable, WEFontNameToIDUPP fontNameToIDProc, Boolean *wasChanged);
pascal OSErr WEUpdateStyleScrap(StScrpHandle hStyles, Handle hFontTable);

//	WEHighLevelEditing.c (public)
pascal OSErr WEUndo(WEHandle hWE);
pascal void WEClearUndo(WEHandle hWE);
pascal WEActionKind WEGetUndoInfo(Boolean *redoFlag, WEHandle hWE);
pascal OSErr WEBeginAction(WEHandle hWE);
pascal OSErr WEEndAction(WEActionKind actionKind, WEHandle hWE);
pascal UInt32 WEGetModCount(WEHandle hWE);
pascal void WEResetModCount(WEHandle hWE);
pascal Boolean WEIsTyping(WEHandle hWE);
pascal void WEKey(SInt16 key, EventModifiers modifiers, WEHandle hWE);
pascal OSErr WEInsert(Ptr textPtr, SInt32 textLength, myStScrpHandle hStyles, Handle hSoup, WEHandle hWE);
pascal OSErr WEInsertObject(FlavorType objectType, Handle objectDataHandle, Point objectSize, WEHandle hWE);
pascal OSErr WEDelete(WEHandle hWE);
pascal OSErr WECut(WEHandle hWE);
pascal Boolean WECanPaste(WEHandle hWE);
pascal OSErr WEPaste(WEHandle hWE);
pascal OSErr WESetStyle(WEStyleMode mode, const TextStyle *ts, WEHandle hWE);
pascal OSErr WEUseStyleScrap(myStScrpHandle hStyles, WEHandle hWE);
pascal OSErr WEUseSoup(Handle hSoup, WEHandle hWE);

//	WEInlineInput.c (public)
pascal OSErr WEInstallTSMHandlers(void);
pascal OSErr WERemoveTSMHandlers(void);
pascal OSErr WEHandleTSMEvent(const AppleEvent *ae, AppleEvent *reply);
pascal void WEStopInlineSession(WEHandle hWE);

//	WELineLayout (public)
pascal OSErr WECalText(WEHandle hWE);
pascal void WESetAlignment(WEAlignment alignment, WEHandle hWE);
pascal void WESetDirection(WEDirection direction, WEHandle hWE);

//	WELongCoords.c (public)
pascal void WELongPointToPoint(const LongPt *lp, Point *p);
pascal void WEPointToLongPoint(Point p, LongPt *lp);
pascal void WESetLongRect(LongRect *lr, SInt32 left, SInt32 top, SInt32 right, SInt32 bottom);
pascal void WELongRectToRect(const LongRect *lr, Rect *r);
pascal void WERectToLongRect(const Rect *r, LongRect *lr);
pascal void WEOffsetLongRect(LongRect *lr, SInt32 hOffset, SInt32 vOffset);
pascal Boolean WELongPointInLongRect(const LongPt *lp, const LongRect *lr);

//	WEMouse.c (public)
pascal Boolean WECanAcceptDrag(DragReference drag, WEHandle hWE);
pascal OSErr WETrackDrag(DragTrackingMessage message, DragReference drag,
									WEHandle hWE);
pascal OSErr WEReceiveDrag(DragReference drag, WEHandle hWE);
pascal Boolean WEDraggedToTrash(DragReference drag);
pascal void WEClick(Point mouseLoc, EventModifiers modifiers, UInt32 clickTime, WEHandle hWE);
pascal UInt16 WEGetClickCount(WEHandle hWE);

//	WEObjects.c (public)
pascal FlavorType WEGetObjectType(WEObjectDescHandle hObjectDesc);
pascal Handle WEGetObjectDataHandle(WEObjectDescHandle hObjectDesc);
pascal Point WEGetObjectSize(WEObjectDescHandle hObjectDesc);
pascal WEHandle WEGetObjectOwner(WEObjectDescHandle hObjectDesc);
pascal SInt32 WEGetObjectRefCon(WEObjectDescHandle hObjectDesc);
pascal void WESetObjectRefCon(WEObjectDescHandle hObjectDesc, SInt32 refCon);
pascal OSErr WEInstallObjectHandler(FlavorType objectType, WESelector handlerSelector,
				UniversalProcPtr handler, WEHandle hWE);
pascal OSErr WEGetObjectHandler(FlavorType objectType, WESelector handlerSelector,
				UniversalProcPtr *handler, WEHandle hWE);

//	WEScraps.c (public)
pascal OSErr WECopyRange(SInt32 rangeStart, SInt32 rangeEnd, Handle hText, Handle
					hStyles, Handle hSoup, WEHandle hWE);
pascal OSErr WECopy(WEHandle hWE);

//	WESelecting.c (public)
pascal SInt32 WEGetOffset(const LongPt *thePoint, WEEdge *edge, WEHandle hWE);
pascal void WEGetPoint(SInt32 offset, SInt16 direction, LongPt *thePoint, SInt16 *lineHeight, WEHandle hWE);
pascal void WEFindWord(SInt32 offset, WEEdge edge, SInt32 *wordStart, SInt32 *wordEnd, WEHandle hWE);
pascal void WEFindLine(SInt32 offset, WEEdge edge, SInt32 *lineStart, SInt32 *lineEnd, WEHandle hWE);
pascal void WEFindParagraph(SInt32 offset, WEEdge edge, SInt32 *paragraphStart, SInt32 *paragraphEnd, WEHandle hWE);
pascal SInt16 WECharByte(SInt32 offset, WEHandle hWE);
pascal SInt16 WECharType(SInt32 offset, WEHandle hWE);
pascal RgnHandle WEGetHiliteRgn(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal void WESetSelection(SInt32 selStart, SInt32 selEnd, WEHandle hWE);
pascal Boolean WEAdjustCursor(Point mouseLoc, RgnHandle mouseRgn, WEHandle hWE);
pascal void WEIdle(UInt32 *maxSleep, WEHandle hWE);
pascal void WEUpdate(RgnHandle updateRgn, WEHandle hWE);
pascal void WEDeactivate(WEHandle hWE);
pascal void WEActivate(WEHandle hWE);
pascal Boolean WEIsActive(WEHandle hWE);
pascal void WEScroll(SInt32 hOffset, SInt32 vOffset, WEHandle hWE);
pascal void WEPinScroll(SInt32 hOffset, SInt32 vOffset, WEHandle hWE);
pascal void WESelView(WEHandle hWE);

//	WESelectors.c (public)
pascal OSErr WEGetInfo(WESelector selector, void *info, WEHandle hWE);
pascal OSErr WESetInfo(WESelector selector, const void *info, WEHandle hWE);

//	WEUserSelectors.c (public)
pascal OSErr WEGetUserInfo(WESelector tag, SInt32 *userInfo, WEHandle hWE);
pascal OSErr WESetUserInfo(WESelector tag, SInt32 userInfo, WEHandle hWE);

//	WESharedLibrary.c
pascal OSErr _WECFMInitialize(const CFragInitBlock *theInitBlock);
pascal void _WECFMTerminate(void);


//	PRIVATE ROUTINES

//	These routines should be eligible to be tagged "internal" when
//	building a CFM shared library: they should never be exported
//	(listed in the .exp file) and their address should never be passed
//	to an external fragment.

#if WASTE_SHARED_LIBRARY
#pragma internal on
#endif

//	WEAccessors.c (private)
pascal SInt32 _WEPixelToLine(SInt32 vOffset, WEHandle hWE);
pascal void _WEGetIndStyle(SInt32 runIndex, WERunInfo *info, WEHandle hWE);
pascal Boolean _WEGetIndDirection(SInt32 runIndex, WEHandle hWE);

//	WEBirthDeath.c (private)
extern const Point kOneToOneScaling;
pascal void _WEResetStyleTable(WEHandle hWE);
pascal OSErr _WERegisterWithTSM(WEHandle hWE);
pascal void _WESetStandardHooks(WEHandle hWE);
pascal SInt16 _WEScriptToFont(ScriptCode script);

//	WEDebug.c (private)
pascal void _WESanityCheck(WEHandle hWE);

//	WEDrawing.c (private)
pascal void _WEContinuousStyleRange(SInt32 rangeStart, SInt32 rangeEnd,
		WEStyleMode *mode, WETextStyle *ts, WEHandle hWE);
pascal void _WESynchNullStyle(WEHandle hWE);
pascal void _WESegmentLoop(SInt32 firstLine, SInt32 lastLine, WESegmentLoopProcPtr callback, void *callbackData, WEHandle hWE);
pascal void _WEDrawTSMHilite(Rect *segmentRect, UInt8 tsFlags);
pascal void _WEDrawLines (SInt32 firstLine, SInt32 lastLine, Boolean doErase, WEHandle hWE);
pascal SInt16 _WECalcPenIndent(const WELineRec * pLine, WEAlignment alignment, WEDirection direction);
pascal void _WESaveQDEnvironment(GrafPtr port, Boolean saveColor, QDEnvironment *environment);
pascal void _WERestoreQDEnvironment(const QDEnvironment *environment);
pascal void _WEFillFontInfo (GrafPtr port, WERunAttributes *targetStyle);
pascal void _WECopyStyle (const WETextStyle *sourceStyle, WETextStyle *targetStyle,
		Style offStyles, WEStyleMode mode);
pascal Boolean _WEOffsetInRange(SInt32 offset, WEEdge edge, SInt32 rangeStart, SInt32 rangeEnd);

//	WEHighLevelEditing.c (private)
pascal void _WEPushAction(WEActionHandle hAction);
pascal OSErr _WENewAction(SInt32 rangeStart, SInt32 rangeEnd, SInt32 newTextLength,
							WEActionKind actionKind, WEActionFlags actionFlags,
							WEHandle hWE, WEActionHandle *hAction);
pascal void _WEDisposeAction(WEActionHandle hAction);
pascal void _WEForgetAction(WEActionHandle *hAction);
pascal OSErr _WEDoAction(WEActionHandle hAction);
pascal void _WEAdjustUndoRange(SInt32 moreBytes, WEHandle hWE);
pascal OSErr _WETypeChar(char theByte, WEHandle hWE);
pascal OSErr _WEBackspace(WEHandle hWE);
pascal OSErr _WEForwardDelete(WEHandle hWE);
pascal OSErr _WEGetScrapHandle(FlavorType dataFlavor, Handle *dataHandle);
pascal OSErr _WESmartSetFont(WEStyleMode mode, const WETextStyle *ts, WEHandle hWE);

//	WEInlineInput.c (private)
pascal OSErr _WEHiliteRangeArray(TextRangeArrayHandle hTray, WEHandle hWE);

//	WELineLayout.c (private)
INLINE pascal void _WERemoveLine(SInt32 lineIndex, WEPtr pWE); // Motorola 12-Nov-97
pascal OSErr _WEInsertLine(SInt32 lineIndex, const WELineRec *pLine, WEPtr pWE);
pascal void _WEBumpOrigin(SInt32 lineIndex, SInt32 deltaOrigin, WEPtr pWE);
pascal SInt32 _WEFindLineBreak(SInt32 lineStart, WEHandle hWE);
pascal void _WECalcHeights(SInt32 rangeStart, SInt32 rangeEnd, SInt16 *lineAscent, SInt16 *lineDescent,
		WEHandle hWE);
pascal OSErr _WERecalBreaks(SInt32 *startLine, SInt32 *endLine, WEHandle hWE);
pascal void _WERecalSlops(SInt32 firstLine, SInt32 lastLine, WEHandle hWE);

//	WELowLevelEditing.c (private)
pascal Boolean _WEIsWordRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal Boolean _WEIsPunct(SInt32 offset, WEHandle hWE);
pascal void _WEIntelligentCut(SInt32 *rangeStart, SInt32 *rangeEnd, WEHandle hWE);
pascal SInt16 _WEIntelligentPaste(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal OSErr _WEInsertRun(SInt32 runIndex, SInt32 offset, SInt32 styleIndex, WEPtr pWE);
pascal void _WERemoveRun(SInt32 runIndex, WEPtr pWE);
pascal void _WEChangeRun(SInt32 runIndex, SInt32 newStyleIndex, Boolean keepOld, WEPtr pWE);
pascal OSErr _WENewStyle(const WETextStyle *ts, SInt32 *styleIndex, WEPtr pWE);
pascal OSErr _WERedraw(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal OSErr _WESetStyleRange(SInt32 rangeStart, SInt32 rangeEnd, WEStyleMode mode,
				const WETextStyle *ts, WEHandle hWE);
pascal OSErr _WEApplyStyleScrap(SInt32 rangeStart, SInt32 rangeEnd, myStScrpHandle styleScrap, WEHandle hWE);
pascal OSErr _WEApplySoup(SInt32 offset, Handle hSoup, WEHandle hWE);
pascal void _WEBumpRunStart(SInt32 runIndex, SInt32 deltaRunStart, WEPtr pWE);
pascal void _WERemoveRunRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal void _WEBumpLineStart(SInt32 lineIndex, SInt32 deltaLineStart, WEPtr pWE);
pascal void _WERemoveLineRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal OSErr _WEDeleteRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal OSErr _WEInsertText(SInt32 offset, Ptr textPtr, SInt32 textLength, WEHandle hWE);

//	WELongCoords (private)
pascal SInt32 _WEPinInRange(SInt32 value, SInt32 rangeStart, SInt32 rangeEnd);

//	WEMouse.c (private)
pascal Boolean _WEIsOptionDrag(DragReference drag);
pascal OSErr _WEGetFlavor(DragReference drag, ItemReference dragItem,
				FlavorType requestedType, Handle hFlavor,
				SInt32 dropOffset, WEHandle hWE);
pascal OSErr _WEExtractFlavor(DragReference drag, ItemReference dragItem,
						FlavorType theType, Handle *hFlavor,
						SInt32 dropOffset, WEHandle hWE);
pascal void _WEUpdateDragCaret(SInt32 offset, WEHandle hWE);
pascal RgnHandle _WEOutlineRgn(RgnHandle solidRgn);
pascal OSErr _WEMakeDragImage(GWorldPtr *imageGWorld, RgnHandle *imageRgn, WEHandle hWE);
pascal OSErr _WEDrag(Point mouseLoc, EventModifiers modifiers, UInt32 clickTime, WEHandle hWE);
pascal void _WEResolveURL(EventModifiers modifiers, SInt32 urlStart, SInt32 urlEnd, WEHandle hWE);

//	WEObjects.c (private)
pascal SInt32 _WELookupObjectType(FlavorType objectType, WEOHTableHandle hTable);
pascal OSErr _WEGetIndObjectType(SInt32 index, FlavorType *objectType, WEHandle hWE);
pascal OSErr _WENewObject(FlavorType objectType, Handle objectDataHandle, WEHandle hWE,
					WEObjectDescHandle *hObjectDesc);
pascal OSErr _WEFreeObject(WEObjectDescHandle hObjectDesc);
pascal OSErr _WEDrawObject(WEObjectDescHandle hObjectDesc);
pascal Boolean _WEClickObject(Point hitPt, EventModifiers modifiers, UInt32 clickTime,
								WEObjectDescHandle hObjectDesc);
pascal OSErr _WEStreamObject(SInt16 destKind, FlavorType *theType, Handle *theData,
				Boolean *canDisposeData, WEObjectDescHandle hObjectDesc);

//	WEScraps.c (private)
pascal OSErr _WEPrependStyle(Handle hStyleScrap, const WERunInfo *info, SInt32 offsetDelta);
pascal OSErr _WEAppendStyle(Handle hStyleScrap, const WERunInfo *info, SInt32 offset);
pascal OSErr _WEPrependObject(Handle hSoup, const WERunInfo *info, SInt32 offsetDelta);
pascal OSErr _WEAppendObject(Handle hSoup, const WERunInfo *info, SInt32 offset);
pascal OSErr _WEPutScrapHandle(FlavorType dataFlavor, Handle dataHandle);

//	WESelecting.c (private)
INLINE pascal void _WEClearHiliteBit(void); // Motorola 9-Jun-97
pascal SInt16 _WEGetContext(SInt32 offset, SInt32 *contextStart, SInt32 *contextEnd,
						WEHandle hWE);
pascal SInt16 _WEGetRestrictedContext(SInt32 offset, SInt32 *contextStart, SInt32 *contextEnd,
						WEHandle hWE);
pascal void _WEGetCaretRect(SInt32 offset, SInt16 direction, Rect *caretRect, WEHandle hWE);
pascal void _WEDrawCaret(SInt32 offset, SInt16 direction, Boolean useDualCaret, WEHandle hWE);
pascal void _WEBlinkCaret(WEHandle hWE);
pascal void _WEHiliteRange(SInt32 rangeStart, SInt32 rangeEnd, WEHandle hWE);
pascal SInt32 _WECrossDirectionBoundary(SInt32 offset, Boolean movingRight, WEHandle hWE);
pascal SInt32 _WEArrowOffset(SInt16 action, SInt32 offset, WEHandle hWE);
pascal void _WEDoArrowKey (SInt16 arrow, EventModifiers modifiers, WEHandle hWE);
pascal Boolean _WEScrollIntoView (SInt32 offset, WEHandle hWE);

// WESelectors.c (private)
extern WELookupTable _weMainSelectorTable[];
extern WELookupTable _weObjectHandlerSelectorTable[];
pascal void _WELookupSelector(const WELookupTable *table, WESelector selector, WEFieldDescriptor *desc);
pascal OSErr _WEGetField(const WELookupTable *table, WESelector selector, SInt32 *info, void *structure);
pascal OSErr _WESetField(const WELookupTable *table, WESelector selector, SInt32 *info, void *structure);

// WEUtilities.c (private)
pascal void _WEForgetHandle(Handle *h);
pascal Boolean _WESetHandleLock(Handle h, Boolean lock);
pascal void _WEBlockClr(void *block, Size blockSize);
pascal Boolean _WEBlockCmp(const void *block1, const void *block2, Size blockSize);
pascal void _WEReorder(SInt32 *a, SInt32 *b);
pascal OSErr _WEAllocate(Size blockSize, UInt32 allocFlags, Handle *h);
pascal OSErr _WESplice(Handle h, const void *blockPtr, SInt32 blockSize, SInt32 offset);

#if WASTE_SHARED_LIBRARY
#pragma internal reset
#endif

#ifdef __cplusplus
}
#endif
