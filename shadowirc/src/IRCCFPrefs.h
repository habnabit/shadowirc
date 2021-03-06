/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
	dshadow@shadowirc.com
	Written for Mac OS X by:
			Michael Ledford <mledford@ugaalum.uga.edu>
			Copyright (C) 2002 Michael Ledford
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

#ifndef _IRCCFPreferences_
#define _IRCCFPreferences_

#define kShortcutArrayKey CFSTR("ShortcutItems")

void InitColorPrefs(void);
void ReadColorCFPrefs(RGBColor colors[]);
void SaveColorsCFPrefs(void);

void SaveShortcutDataCFPrefs(Str255 shortcutData[]);
void ReadShortCutDataCFPrefs(Str255 shortcutData[]);

OSStatus WriteDirURLRef(CFStringRef prefName, const FSRef *ref);
OSStatus ReadDirURLRef(CFStringRef prefName, FSRef *ref);

#endif
