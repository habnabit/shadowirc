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

#include <Carbon/Carbon.h>

#include "AppearanceHelp.h"
#include "OffscreenDrawing.h"

/*
Here's how it works.  Before you start drawing, put in this code...

BufferData		drawingInfo;
StartDrawingOffscreen(&drawingInfo, &rect, true);


rect should be the rect you wish to draw in.  It should be as small as possible to reduce
memory requirements.  For the last variable, pass true if you want to copy the image
in the window to the gworld before you draw in the gworld.  This may or may not be necessary.
Pass false if you can, since it's SLIGHTLY faster.  After you're done drawing, call...

EndDrawingOffscreen(&drawingInfo);

This will copy what you drew to the screen.  Remember to draw exactly as you would draw in the
window.  Everything will work out, and as long as you draw within rect (which you passed earlier)
everything will get copied to the screen correctly.


The neat thing about this system is that if
there isn't enough memory to create the GWorld, you will draw directly to the window without
having to worry about writing extra code.

*/

pascal char StartDrawingOffscreen(Ptr *returnData, RectPtr bounds, Boolean copyDest)
{
	QDErr			err;
	Rect			globalRect;
	RGBColor		oldFore, oldBack;
	PixMapHandle	pm;
	BufferDataPtr data;
	
	data = (BufferDataPtr)NewPtr(sizeof(BufferData));
	*returnData = (Ptr)data;
	data->bounds = *bounds;
	
	GetGWorld(&data->oldPort, &data->oldDevice);
	
	globalRect = *bounds;
	
	LocalToGlobal((Point*)&globalRect.top);
	LocalToGlobal((Point*)&globalRect.bottom);

	err = NewGWorld(&data->offscreenWorld, 0, &globalRect, nil, nil, useTempMem);
	if(err == noErr)
	{
		Rect portRect;
		
		GetForeColor(&oldFore);
		GetBackColor(&oldBack);
		GetPortBounds(data->oldPort, &portRect);
		SetPt(&data->oldOrigin, portRect.left, portRect.top);
		SetGWorld(data->offscreenWorld, nil);
		SetOrigin(bounds->left, bounds->top);
		RGBForeColor(&oldFore);
		RGBBackColor(&oldBack);
		
		pm = GetGWorldPixMap( data->offscreenWorld );
		if(LockPixels(GetGWorldPixMap(data->offscreenWorld)))
		{
			EraseRect(bounds);
			TextFont(GetPortTextFont(data->oldPort));
			TextSize(GetPortTextSize(data->oldPort));
			TextFace(GetPortTextFace(data->oldPort));
			TextMode(GetPortTextMode(data->oldPort));
			if(copyDest)
				CopyBits((BitMap*)*GetPortPixMap(data->oldPort), (BitMap*)*pm, bounds, GetPortBounds(data->offscreenWorld, 0), srcCopy, nil);
		}
		else
		{
			DisposeGWorld(data->offscreenWorld);
			data->offscreenWorld = nil;
		}
	}
	
	return err == 1;
}

pascal void AbortDrawingOffscreen(Ptr dataIn)
{
	BufferDataPtr data = (BufferDataPtr)dataIn;
	
	if(data->offscreenWorld == nil)
		return;
	
	SetOrigin(data->oldOrigin.h, data->oldOrigin.v);
	SetGWorld(data->oldPort, data->oldDevice);
	DisposeGWorld(data->offscreenWorld);
	DisposePtr(dataIn);
}

pascal void EndDrawingOffscreen(Ptr dataIn)
{
	PixMapHandle	pm;
	RGBColor oldFore, oldBack;
	BufferDataPtr data = (BufferDataPtr)dataIn;
	Rect portRect;
	
	if(data->offscreenWorld == nil)
		return;
	
	SetGWorld(data->oldPort, data->oldDevice);
	
	SetOrigin(data->oldOrigin.h, data->oldOrigin.v);
	GetForeColor(&oldFore);
	GetBackColor(&oldBack);
	RGBForeColor(&black);
	RGBBackColor(&white);

	pm = GetGWorldPixMap(data->offscreenWorld);
	GetPortBounds(data->offscreenWorld, &portRect);
	CopyBits((BitMap*)*pm, (BitMap*)*GetPortPixMap(data->oldPort), &portRect, &data->bounds, srcCopy, NULL);

	RGBForeColor(&oldFore);
	RGBBackColor(&oldBack);
	UnlockPixels(pm);
	DisposeGWorld(data->offscreenWorld);
	DisposePtr(dataIn);
}