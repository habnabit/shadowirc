/*
*  Events.c
*  ShadowIRC
*
*  Created by Chris Campbell on Mon Feb 11 2002.
*  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
*
*/

#include "WASTE.h" // WECountLines()
#include "Events.h"
#include "IRCPreferences.h" // OpenPreferencesWindow()

/*
* Carbon Event Handlers
*/

static pascal OSStatus MWDoMouseWheelEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData)
{
	OSStatus myErr = eventNotHandledErr;
	Point mouseLoc;
	EventMouseWheelAxis wheelAxis;
	long wheelDelta;
	MWPtr mw = userData;
	
	GetEventParameter(theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(mouseLoc), NULL, &mouseLoc);
	GetEventParameter(theEvent, kEventParamMouseWheelAxis, typeMouseWheelAxis, NULL, sizeof(wheelAxis), NULL, &wheelAxis);
	GetEventParameter(theEvent, kEventParamMouseWheelDelta, typeLongInteger, NULL, sizeof(wheelDelta), NULL, &wheelDelta);
	
	if(wheelAxis == kEventMouseWheelAxisY)
	{
		// Scroll the vertical scroll bar
		int maxValue = GetControl32BitMaximum(mw->vscr);
		int numLines = WECountLines(mw->we);
		float lineValue = (float)maxValue/(float)numLines;
		long adjustedDelta = (((float)lineValue * (float)wheelDelta) + ((wheelDelta < 0)?(-0.5):(0.5)));
		
		MWScroll((MWPtr)userData, 4 * -adjustedDelta);
		myErr = noErr;
	}
	
	return myErr;
}

void MWInstallMouseWheelHandlers(MWPtr mw)
{
	EventTypeSpec wheelType = {kEventClassMouse, kEventMouseWheelMoved};
	
	InstallWindowEventHandler(mw->w, NewEventHandlerUPP(MWDoMouseWheelEvent), 1, &wheelType, mw, NULL);
}

static pascal OSStatus DoCommandEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData)
{
	OSStatus myErr = eventNotHandledErr;
	HICommand hiCommand;
	
	GetEventParameter(theEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(hiCommand), NULL, &hiCommand);
	
	if(hiCommand.commandID == kHICommandPreferences)
	{
		// Open the Preferences window
		OpenPreferencesWindow(-1);
		myErr = noErr;
	}
	
	return myErr;
}

void InitEventHandlers()
{
	EventTypeSpec commandType;
	
	commandType.eventClass = kEventClassCommand;
	commandType.eventKind  = kEventProcessCommand;
	
	InstallApplicationEventHandler(NewEventHandlerUPP(DoCommandEvent), 1, &commandType, NULL, NULL);
}
