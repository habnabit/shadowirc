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

#ifndef _Libs_
#define _Libs_
#pragma lib_export on
#pragma export on
pascal void SetSmartScrollInfo (ControlRef theScrollBar, long amountVisible , long amountTotal);
pascal void SetSmartScrollProp (ControlRef theScrollBar, Fract proportion);
pascal Fract GetSmartScrollProp (ControlRef theScrollBar);

OSErr LCAttach (ControlRef inControl);
void LCDetach (ControlRef inControl);
void LCSetValue (ControlRef inControl, SInt32 inValue);
void LCSetMin (ControlRef inControl, SInt32 inValue);
void LCSetMax (ControlRef inControl, SInt32 inValue);
SInt32 LCGetValue (ControlRef inControl);
SInt32 LCGetMin (ControlRef inControl);
SInt32 LCGetMax (ControlRef inControl);
void LCSynch (ControlRef inControl);

#pragma export off
#pragma lib_export off

#endif