/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2000 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.com

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <Appearance.h>
#include "AppearanceHelp.h"
#include <ControlDefinitions.h>
#include "IRCGlobals.h"

#pragma internal on
const RGBColor white = {-1, -1, -1};
const RGBColor black = {0, 0, 0};
const RGBColor VLtGrey = {cVLtGrey, cVLtGrey, cVLtGrey};
const RGBColor LtGrey = {cLtGrey, cLtGrey, cLtGrey};
const RGBColor MedGrey = {cMedGrey, cMedGrey, cMedGrey};
const RGBColor HalfGrey = {cHalfGrey, cHalfGrey, cHalfGrey};
const RGBColor MDkGrey = {cMDkGrey, cMDkGrey, cMDkGrey};
const RGBColor DkGrey = {cDkGrey, cDkGrey, cDkGrey};
const RGBColor VDkGrey = {cVDkGrey, cVDkGrey, cVDkGrey};
#pragma internal reset

pascal ListHandle GetAppearanceListBoxHandle(DialogPtr theDialog, short itemNum)
{
	ListHandle		list = nil;
	ControlHandle	theControl;
	Size			theDataSize;
	
	if (GetDialogItemAsControl(theDialog, itemNum, &theControl) == noErr)
	{
		GetControlData(theControl, 0, kControlListBoxListHandleTag, sizeof(list), (Ptr) &list, &theDataSize);
	if (theDataSize != sizeof(list))
		list = nil;
	}
	
	return list;
}

#define SC(c, g) c.red=c.green=c.blue=g

pascal void DrawBorder(const Rect *r, long state, char draggable)
{
	RGBColor rgbF;
	short cV, cH;
	short top, bottom;
	
	GetForeColor(&rgbF);

	if(state == kThemeStatePressed) 
		RGBForeColor(&DkGrey);
	else
		RGBForeColor(&LtGrey);

	PaintRect(r);
	
	if(state != kThemeStateInactive && draggable)
	{
		RGBForeColor(&MedGrey);
		cV = (r->bottom + r->top)/2;
		cH = (r->left + r->right)/2;
		
		if(r->bottom - r->top < r->right - r->left)
		{
			cV--;
			
			top = cH - 20;
			bottom = cH + 20;
			
			if(top < r->left)
				top = r->left + 2;
			if(bottom > r->right)
				bottom = r->right - 2;

			MoveTo(top, cV);
			LineTo(bottom, cV);
			MoveTo(top, cV + 2);
			LineTo(bottom, cV + 2);
		}
		else
		{
			cH--;
			
			top = cV - 20;
			bottom = cV + 20;
			
			if(top < r->top)
				top = r->top + 2;
			if(bottom > r->bottom)
				bottom = r->bottom - 2;

			MoveTo(cH, top);
			LineTo(cH, bottom);
			MoveTo(cH + 2, top);
			LineTo(cH + 2, bottom);
		}
	}
	
	RGBForeColor(&rgbF);
}

typedef struct dpPaintRgnInfo {
	const RGBColor *color;
	Rect ir;
} dpPaintRgnInfo;

static DeviceLoopDrawingUPP dpPaintRgnUPP;

static pascal void dpPaintRgn(short depth, short deviceFlags, GDHandle device, long data);
static pascal void dpPaintRgn(short depth, short deviceFlags, GDHandle device, long data)
{
#pragma unused(deviceFlags, device)
	dpPaintRgnInfo* x = (dpPaintRgnInfo*)data;

	if(depth < 16)
		RGBForeColor(&white);
	else
		RGBForeColor(x->color);
	PaintRect(&x->ir);
}

pascal void DrawPlacard(const Rect *r, long state)
{
	if(hasAppearance11)
		DrawThemePlacard(r, state);
	else
	{
		RGBColor fore;
		const RGBColor *cBorder, *cTLHilite, *cBRHilite, *cCorners, *cFill;
		
		if(state == kThemeStateActive)
		{
			cBorder = &black;
			cTLHilite = &white;
			cBRHilite = &MedGrey;
			cCorners = &LtGrey;
			cFill = &LtGrey;
		}
		else if(state == kThemeStatePressed)
		{
			cBorder = &black;
			cTLHilite = &VDkGrey;
			cBRHilite = &MedGrey;
			cCorners = &MDkGrey;
			cFill = &MDkGrey;
		}
		else if(state == kThemeStateInactive)
		{
			cBorder = &VDkGrey;
			cTLHilite = &VLtGrey;
			cBRHilite = &VLtGrey;
			cCorners = &VLtGrey;
			cFill = &VLtGrey;
		}
		else
			return;
		
		//Save colors
		GetForeColor(&fore);

		//Draw black outline
		RGBForeColor(cBorder);
		MoveTo(r->left, r->top);
		LineTo(r->right -1, r->top);
		LineTo(r->right -1, r->bottom -1);
		LineTo(r->left, r->bottom -1);
		LineTo(r->left, r->top);
		
		//white hilite
		RGBForeColor(cTLHilite);
		MoveTo(r->right - 2, r->top + 1);
		LineTo(r->left + 1, r->top + 1);
		LineTo(r->left +1, r->bottom - 2);
		
		//Dark hilite
		RGBForeColor(cBRHilite);
		MoveTo(r->left + 2, r->bottom - 2);
		LineTo(r->right - 2, r->bottom - 2);
		LineTo(r->right - 2, r->top + 2);
		
		//Corner greys
		RGBForeColor(cCorners);
		MoveTo(r->left + 1, r->bottom -2);
		LineTo(r->left + 1, r->bottom -2);
		
		MoveTo(r->right - 2, r->top +1);
		LineTo(r->right - 2, r->top+1);
		
		if(!dpPaintRgnUPP)
			dpPaintRgnUPP  = NewDeviceLoopDrawingProc(dpPaintRgn);
		
		{
			dpPaintRgnInfo info;
			RgnHandle dr = NewRgn();
			GrafPtr gp;
			
			info.color = cFill;
			info.ir.left = r->left + 2;
			info.ir.right = r->right - 2;
			info.ir.top = r->top + 2;
			info.ir.bottom = r->bottom - 2;
			RectRgn(dr, &info.ir);
			GetPort(&gp);
			DeviceLoop(dr, dpPaintRgnUPP, (long)&info, 0);
			DisposeRgn(dr);
		}
		
		//Restore colors
		RGBForeColor(&fore);
	}
}

pascal void SetTextColor(short color)
{
	SetThemeTextColor(color, 16, true);
}

pascal void SetBackground(short color)
{
	SetThemeBackground(color, 16, true);
}

#pragma mark -

struct InternalDrawingState
{
	RGBColor		foreColor;
	RGBColor		backColor;
	PenState		pen;
	SInt16			textMode;
	PixPatHandle	pnPixPat;
	PixPatHandle	bkPixPat;
	Pattern			bkPat;
};
typedef struct InternalDrawingState InternalDrawingState;

//Replacments for the color/pen state stuff that's in Appearance 1.1
//Copied from Apple's code from somewhere, I think
//But, assuming we have a color port, since ShadowIRC only generates color windows...
pascal void GetDrawingState(DrawingState *outState)
{
	if(TARGET_CARBON || hasAppearance11)
		GetThemeDrawingState((ThemeDrawingState*)outState);
#if(!TARGET_CARBON)
	else
	{
		InternalDrawingState* state = (InternalDrawingState*)NewPtr(sizeof(InternalDrawingState));
		GrafPtr	 curPort;
		
		*outState = (DrawingState)state;
		GetPort(&curPort);
		
		state->pnPixPat = nil;
		state->bkPixPat = nil;

		GetForeColor(&state->foreColor);
		GetBackColor(&state->backColor);
		
		// If the pen pattern is not an old style pattern,
		// copy the handle. If it is an old style pattern,
		// GetPenState below will save the right thing.
		if ( (**((CGrafPtr)curPort)->pnPixPat).patType != 0 )
			state->pnPixPat = ((CGrafPtr)curPort)->pnPixPat;
		
		// If the pen pattern is not an old style pattern,
		// copy the handle, else get the old pattern into
		// bkPat for restoring that way.
		if ( (**((CGrafPtr)curPort)->bkPixPat).patType != 0 )
			state->bkPixPat = ((CGrafPtr)curPort)->bkPixPat;
		else
			state->bkPat = *(PatPtr)(*(**((CGrafPtr)curPort)->bkPixPat).patData);
			
		GetPenState(&state->pen);
		state->textMode = GetPortTextMode(curPort);
	}
#endif
}

pascal void SetDrawingState(DrawingState instate)
{
	if(TARGET_CARBON || hasAppearance11)
		SetThemeDrawingState((ThemeDrawingState)instate, true);
#if(!TARGET_CARBON)
	else
	{
		InternalDrawingState *state = (InternalDrawingState*)instate;
		GrafPtr	 curPort;
		
		GetPort( &curPort );

		SetPenState( &state->pen );

		RGBForeColor( &state->foreColor );
		RGBBackColor( &state->backColor );

		if ( state->pnPixPat )
			PenPixPat( state->pnPixPat );

		if ( state->bkPixPat )
			BackPixPat( state->bkPixPat );
		else
			BackPat( &state->bkPat );

		TextMode( state->textMode );
	}
#endif
}

pascal void NormalizeDrawingState()
{
	Pattern			whitePat = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	GrafPtr			curPort;
	
	GetPort( &curPort );
	
	if(IsPortColor( curPort ) )
	{
		RGBForeColor( &black );
		RGBBackColor( &white );
	}
	PenNormal();
	BackPat( &whitePat );
	TextMode( srcOr );
}

#pragma mark -

#if !TARGET_CARBON
#if __POWERPC__
#pragma internal on
//Glue necessary to call AppendDialogItemList.
#define RESULT_OFFSET(type) ((sizeof(type) == 1) ? 3 : ((sizeof(type) == 2) ? 1 : 0))

pascal OSErr
AppendDialogItemList( DialogPtr dialog, SInt16 ditlID, DITLMethod method )
{
	long	private_result;
	
	private_result = CallUniversalProc((UniversalProcPtr)GetToolTrapAddress(0xAA68),
		kD0DispatchedPascalStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		 | DISPATCHED_STACK_ROUTINE_SELECTOR_SIZE(kTwoByteCode)
		 | DISPATCHED_STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(dialog)))
		 | DISPATCHED_STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ditlID)))
		 | DISPATCHED_STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(method))),
		0x0412,
		dialog,
		ditlID,
		method );
		
	return *(((OSErr*)&private_result) + RESULT_OFFSET(OSErr));
}
#pragma internal off
#endif
#endif