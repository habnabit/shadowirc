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

#ifndef _ApplBase_
#define _ApplBase_

#if TARGET_CARBON
#include <Dialogs.h>
#endif

#ifndef _LongStrings
#include "LongStrings.h"
#endif

#pragma internal on

extern UserItemUPP AboutDlgVersionFilter;
extern MenuHandle gAppleMenu, gFileMenu, gShortcutsMenu, gWindowMenu;

extern WindowPtr ContextWindow;

extern long sScrollStep;

pascal void ApplRun(void);
pascal void ApplInit(void);
pascal void ApplExit(void);
pascal char doQuit(char force, LongString *reason);

pascal void menuEdit(short item);
pascal void MenuConnectionList(short item);
pascal void MenuSignoffConnectionList(short item);
pascal void wmenu(short item);

pascal void doUpdateEvent(EventRecord *e);
pascal void DoSuspendResumeEvent(EventRecord *e);
pascal void doNetworkCheck(void);

#pragma internal reset

#pragma lib_export on
#pragma export on
pascal void WindowClose(WindowPtr wp);
pascal OSErr AsyncSoundPlay(Handle sound, long refcon, Ptr *channel);
#pragma export reset
#pragma lib_export reset

#endif