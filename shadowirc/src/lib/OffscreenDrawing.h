/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1997-2000 John Bafford
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

#pragma once

typedef struct {
	GWorldPtr offscreenWorld;
	Rect bounds;
	
	CGrafPtr	oldPort;
	GDHandle	oldDevice;
	Point			oldOrigin;
} BufferData, *BufferDataPtr;


#pragma lib_export on
#pragma export on
pascal char StartDrawingOffscreen(Ptr *data, RectPtr bounds, Boolean copyDest);
pascal void EndDrawingOffscreen(Ptr data);
pascal void AbortDrawingOffscreen(Ptr data);
#pragma export off
#pragma lib_export off
