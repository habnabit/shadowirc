/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
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

#include "AppearanceHelp.h"
#include "IRCGlobals.h"

const RGBColor white = {-1, -1, -1};
const RGBColor black = {0, 0, 0};
const RGBColor VLtGrey = {cVLtGrey, cVLtGrey, cVLtGrey};
const RGBColor LtGrey = {cLtGrey, cLtGrey, cLtGrey};
const RGBColor MedGrey = {cMedGrey, cMedGrey, cMedGrey};
const RGBColor HalfGrey = {cHalfGrey, cHalfGrey, cHalfGrey};
const RGBColor MDkGrey = {cMDkGrey, cMDkGrey, cMDkGrey};
const RGBColor DkGrey = {cDkGrey, cDkGrey, cDkGrey};
const RGBColor VDkGrey = {cVDkGrey, cVDkGrey, cVDkGrey};

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


pascal void DrawPlacard(const Rect *r, long state)
{
	DrawThemePlacard(r, state);
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
	GetThemeDrawingState((ThemeDrawingState*)outState);
}

pascal void SetDrawingState(DrawingState instate)
{
	SetThemeDrawingState((ThemeDrawingState)instate, true);
}

pascal void NormalizeDrawingState()
{
	Pattern			whitePat;
	GrafPtr			curPort;
	
	GetQDGlobalsWhite(&whitePat);
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

