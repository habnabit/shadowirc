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

#ifndef _IRCInput
#define _IRCInput

#ifdef _IRCChannels
pascal void InputHandler(LongString *ls, targetPtr targ);
#endif
pascal void GetLine(char action, MWPtr mw);
pascal char ProcessInputHunk(const char* data, long l, MWPtr mw, char clearEnd, long *lastCR);

pascal void LockInput(void);
pascal void UnlockInput(void);

#ifdef _InputControl_
void ProcessLine(LongString* line, inputAreaDataPtr stackup, char action, MWPtr mw);
OSStatus NewKey(inputAreaDataPtr iad, EventRef event);
#endif

pascal void parseShortcutText(ConstStr255Param s);
pascal void processPaste(MWPtr mw, char dragAndDrop);
#ifdef _IRCChannels
pascal void HandleMessage(LongString *ls, targetPtr targ);
#endif

extern long lastInput, lastKey;
extern char iwFront;
extern char inputLocked;

#endif