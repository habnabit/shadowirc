/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 2002 John Bafford, Chris Campbell
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
