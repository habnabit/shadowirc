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

#ifndef _AppearanceHelp_
#define _AppearanceHelp_

#include <Carbon/Carbon.h>

enum {
	cWhite = -1,
	cBlack = 0,
	cVLtGrey = 61307,
	cLtGrey = 57079, //kStandardGrey
	cMedGrey = 44395,
	cHalfGrey = 32766,
	cMDkGrey = 35939,
	cDkGrey = 29596,
	cVDkGrey = 21140
};


extern const RGBColor white, black, VLtGrey, LtGrey, MedGrey, HalfGrey, MDkGrey, DkGrey, VDkGrey;

pascal ListHandle GetAppearanceListBoxHandle(DialogPtr theDialog, short itemNum);
pascal void SetTextColor(short color);
pascal void SetBackground(short color);

pascal void DrawBorder(const Rect *rIn, long state, char draggable);

#endif