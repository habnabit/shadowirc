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

#include "YASTControl.h"

#include "LongStrings.h"
#include "IRCGlobals.h"
#include "CircularList.h"
#include "InputControl.h"

#define MAXHISTLINES 100

struct inputAreaData {
	ControlRef inputControl;
	TXNObject il;
	InputControlTextProc inputProc;
	
	WindowPtr window;
	
	CLPtr hist;
};

#pragma mark Accessors

WindowPtr IADGetWindow(inputAreaDataPtr iad)
{
	return iad->window;
}

TXNObject IADGetTXN(inputAreaDataPtr iad)
{
	if(iad)
		return iad->il;
	else
		return 0;
}

void IADSetFieldBounds(inputAreaDataPtr iad, Rect r)
{
	SetControlBounds(iad->inputControl, &r);
}

#pragma mark Text Actions

void IADSetCursorSelection(inputAreaDataPtr iad, long start, long finish)
{
	TXNSetSelection(iad->il, start, finish);
}

void IADGetCursorSelection(inputAreaDataPtr iad, long *start, long *finish)
{
	TXNGetSelection(iad->il, start, finish);
}

void IADSetTextPtrRange(inputAreaDataPtr iad, Ptr text, long length, int start, int end)
{
	TXNSetData(iad->il, kTXNTextData, text, length, start, end);
}

void IADSetTextPtr(inputAreaDataPtr iad, Ptr text, long length)
{
	TXNSetData(iad->il, kTXNTextData, text, length, kTXNStartOffset, kTXNEndOffset);
}

void IADSetText(inputAreaDataPtr iad, LongString *ls)
{
	short i;
	
	while((i = LSPosChar(13, ls)) != 0)
		LSDelete(ls, i, i);
	
	IADSetTextPtr(iad, &ls->data[1], ls->len);
}

long IADGetTextHandleRange(inputAreaDataPtr iad, Handle *text, int start, int end)
{
	TXNGetDataEncoded(iad->il, start, end, text, kTXNTextData);
	
	return GetHandleSize(*text);
}

long IADGetTextHandle(inputAreaDataPtr iad, Handle *text)
{
	TXNGetDataEncoded(iad->il, kTXNStartOffset, kTXNEndOffset, text, kTXNTextData);
	
	return GetHandleSize(*text);
}

long IADGetText(inputAreaDataPtr iad, LongString *ls)
{
	Handle text;
	long textLen;
	
	textLen = TXNDataSize(iad->il) / sizeof(UniChar);
	
	if(ls)
	{
		long tl = textLen;
		
		TXNGetDataEncoded(iad->il, kTXNStartOffset, kTXNEndOffset, &text, kTXNTextData);
		
		if(tl > 500)
			tl = 500;
		
		BlockMoveData(*text, &ls->data[1], tl);
		ls->len = tl;

		DisposeHandle(text);
	}
	
	return textLen;
}

#pragma mark History

void ILAddHistory(inputAreaDataPtr iad, LongString *line)
{
	CLEnqueue(iad->hist, &line->data[1], line->len);
}

static void RecallLineUp(inputAreaDataPtr iad)
{
	void* data;
	int size;
	
	CLWalk(iad->hist, -1, &data, &size);
	IADSetTextPtr(iad, data, size);
}

static void RecallLineDown(inputAreaDataPtr iad)
{
	void* data;
	int size;
	
	CLWalk(iad->hist, 1, &data, &size);
	IADSetTextPtr(iad, data, size);
}

#pragma mark Init/Destroy

static void _IADDispose(inputAreaDataPtr iad)
{
	CLDispose(iad->hist);
	
	free(iad);
}

void IADDispose(inputAreaDataPtr iad)
{
	DisposeControl(iad->inputControl);
}

//Separate out the text event handler, so that it runs quickly
static const EventTypeSpec iadTextEventTypes[] = {
		{kEventClassTextInput, kEventUnicodeForKeyEvent},
};

static OSStatus InputLineControlTextEventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
#pragma unused(handlerCallRef)
	OSStatus result;
	inputAreaDataPtr iad = (inputAreaDataPtr)userData;
	
	result = iad->inputProc(iad, event);
	
	//See if there's any keystrokes we should handle internally before returning
	if(result == eventNotHandledErr)
	{
		UniChar mUnicodeText[8];
		UInt32 bytecount, nchars;
		OSErr err;
		
		/* get the character */
		err = GetEventParameter(event, kEventParamTextInputSendText, typeUnicodeText, NULL, sizeof(mUnicodeText), &bytecount, (char*) mUnicodeText);
		if ((err == noErr) && (bytecount >= sizeof(UniChar)))
		{
			nchars = bytecount / sizeof(UniChar);
			EventRef rawKeyEvent;
			UInt32 modifiers;
			
			GetEventParameter(event, kEventParamTextInputSendKeyboardEvent, typeEventRef, NULL, sizeof(rawKeyEvent), NULL, &rawKeyEvent);
			GetEventParameter(rawKeyEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(modifiers), NULL, &modifiers);
			
			switch(mUnicodeText[0])
			{
				case 27: //esc
					if(mainPrefs->escClearsInputline)
					{
				case 21: //0x15 = ctrl-u
				case 4: //ctrl-d
						IADSetTextPtr(iad, NULL, 0);
					}
					result = noErr;
					break;
				
				case 30:
					if(!mainPrefs->optionToMoveInputLine || (modifiers & optionKey)==optionKey)
					{
						RecallLineUp(iad);
						result = noErr;
					}
					break;
				
				case 31:
					if(!mainPrefs->optionToMoveInputLine || (modifiers & optionKey)==optionKey)
					{
						RecallLineDown(iad);
						result = noErr;
					}
					break;
			}
		}
	}
	
	return result;
}

OSStatus IADHandleTextEvent(inputAreaDataPtr iad, EventRef event)
{
	return SendEventToControl(event, iad->inputControl);
}

static const EventTypeSpec iadControlEventTypes[] = {
		{kEventClassControl, kEventControlDispose},
};

static OSStatus InputLineControlEventHandler(EventHandlerCallRef handlerCallRef, EventRef event, void *userData)
{
#pragma unused(handlerCallRef)
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass, eventKind;
	
	eventClass = GetEventClass(event);
	eventKind = GetEventKind(event);
	
	switch(eventClass)
	{
		case kEventClassControl:
		{
			switch(eventKind)
			{
				case kEventControlDispose:
					//The control is going away, so we can dispose of the iad.
					_IADDispose((inputAreaDataPtr)userData);
					break;
			}
			break;
		}
	}
	
	return result;
}


OSErr IADFieldTrackDrag(inputAreaDataPtr iad, DragTrackingMessage message, WindowPtr window, DragReference drag)
{
	OSErr ret = dragNotAcceptedErr;
	
/*
	if(!inputLocked)
		ret = TXNDragTracker(iad->il, iad->frameID, message, window, drag, true);
*/
	
	return ret;
}

OSErr IADFieldReceiveDrag(inputAreaDataPtr iad, WindowPtr window, DragReference drag)
{
	OSErr err = dragNotAcceptedErr;
	
/*
	if(!inputLocked)
	{
			err = TXNDragReceiver(iad->il, iad->frameID, window, drag, false);
			if(!err)
				processPaste(CurrentTarget.mw, true);
	}
*/
	
	return err;
}

void IADActivate(inputAreaDataPtr iad, char activate)
{
	if(activate)
	{
		SetUserFocusWindow(iad->window);
		SetKeyboardFocus(iad->window, iad->inputControl, kYASTControlOnlyPart);
	}
}

static void IADSetInputStyle(inputAreaDataPtr iad)
{
	const TXNTypeAttributes attrs[] = 
	{
		{kTXNQDFontNameAttribute, kTXNQDFontNameAttributeSize, {mainPrefs->defaultFontName}},
		{kTXNQDFontSizeAttribute, kTXNFontSizeAttributeSize, {Long2Fix(mainPrefs->defaultFontSize)}}
	};
	OSStatus err;

	err = TXNSetTypeAttributes(iad->il, sizeof(attrs) / sizeof(TXNTypeAttributes), attrs, kTXNStartOffset, kTXNEndOffset);
}

inputAreaDataPtr IADNew(WindowPtr window, Rect textRect, InputControlTextProc inputProc)
{
	ControlRef inputLineControl;
	Boolean b;
	inputAreaDataPtr iad  = malloc(sizeof(inputAreaData));
	Size size;
	static EventHandlerUPP thUPP = NULL;
	static EventHandlerUPP chUPP = NULL;
	
	CreateYASTControl(window, &textRect, &inputLineControl);
	b = false;
	SetControlData(inputLineControl, kYASTControlOnlyPart, kYASTControlDoDrawFocusTag, sizeof(Boolean), &b);
	SetControlData(inputLineControl, kYASTControlOnlyPart, kYASTControlTabsAdvanceFocusTag, sizeof(Boolean), &b);
	GetControlData(inputLineControl, kYASTControlOnlyPart, kYASTControlTXNObjectTag, sizeof(TXNObject), &iad->il, &size);
	
	iad->inputControl = inputLineControl;
	iad->window = window;
	iad->inputProc = inputProc;
	
	iad->hist = CLNew(MAXHISTLINES);
	
	IADSetInputStyle(iad);
	
	if(!thUPP)
	{
		thUPP = NewEventHandlerUPP(InputLineControlTextEventHandler);
		chUPP = NewEventHandlerUPP(InputLineControlEventHandler);
	}

	InstallControlEventHandler(inputLineControl, thUPP, GetEventTypeCount(iadTextEventTypes), iadTextEventTypes, iad, NULL);
	InstallControlEventHandler(inputLineControl, chUPP, GetEventTypeCount(iadControlEventTypes), iadControlEventTypes, iad, NULL);
	
	return iad;
}
