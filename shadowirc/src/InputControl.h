/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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

#ifndef _InputControl_
#define _InputControl_

typedef struct inputAreaData inputAreaData, *inputAreaDataPtr;
typedef OSStatus (*InputControlTextProc)(inputAreaDataPtr iad, EventRef event);

#pragma mark Accessors
WindowPtr IADGetWindow(inputAreaDataPtr iad);
TXNObject IADGetTXN(inputAreaDataPtr iad);
void IADSetFieldBounds(inputAreaDataPtr iad, Rect r);

#pragma mark Text Actions
void IADSetCursorSelection(inputAreaDataPtr iad, long start, long finish);
void IADGetCursorSelection(inputAreaDataPtr iad, long *start, long *finish);
void IADSetTextPtrRange(inputAreaDataPtr iad, Ptr text, long length, int start, int end);
void IADSetTextPtr(inputAreaDataPtr iad, Ptr text, long length);
void IADSetText(inputAreaDataPtr iad, LongString *ls);
long IADGetTextHandleRange(inputAreaDataPtr iad, Handle *text, int start, int end);
long IADGetTextHandle(inputAreaDataPtr iad, Handle *text);
long IADGetText(inputAreaDataPtr iad, LongString *ls);

#pragma mark History
void ILAddHistory(inputAreaDataPtr iad, LongString *line);

#pragma mark Init/Destroy
void IADDispose(inputAreaDataPtr iad);
inputAreaDataPtr IADNew(WindowPtr window, Rect textRect, InputControlTextProc inputProc);


void IADActivate(inputAreaDataPtr iad, char activate);

OSErr IADFieldTrackDrag(inputAreaDataPtr iad, DragTrackingMessage message, WindowPtr window, DragReference drag);
OSErr IADFieldReceiveDrag(inputAreaDataPtr iad, WindowPtr window, DragReference drag);

#endif
