/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
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

#include <Carbon/Carbon.h>

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

pascal void SetTextColor(short color)
{
	SetThemeTextColor(color, 16, true);
}

pascal void SetBackground(short color)
{
	SetThemeBackground(color, 16, true);
}
