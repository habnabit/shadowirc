/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
	dshadow@shadowirc.com
	Written for MacOS X by:
			Michael Ledford <mledford@ugaalum.uga.edu>
	Copyright (C) 2002 Michael Ledford, John Bafford
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

#include "IRCCFPrefs.h"


/* void SaveshortcutDataCFPrefs()
	This function takes a pointer to the shortcut window data.
*/
void SaveShortcutDataCFPrefs(Str255 shortcutData[]) 
{
	int x;
	CFStringRef shortcutString;
	CFMutableArrayRef shortcutsArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	
	/* Iterate over the shortcuts and add them to the array. The current number of 
	shortcuts is currently hardcoded. We might want to define this instead later.*/
	for(x = 0; x < 30; x++)
	{
		shortcutString = CFStringCreateWithPascalString(NULL, shortcutData[x], kCFStringEncodingMacRoman);
		CFArrayAppendValue(shortcutsArray, shortcutString);
	}
	
	CFPreferencesSetAppValue(kShortcutArrayKey, shortcutsArray, kCFPreferencesCurrentApplication);
	CFRelease(shortcutsArray);
}

/* void ReadShortCutDataCFPrefs()
	This function takes a pointer to an already allocated block of memory.
	It will fill this memory with the values stored on disk.
*/
void ReadShortCutDataCFPrefs(Str255 shortcutData[])
{
	int x;
	CFStringRef shortcutString;
	CFPropertyListRef shortcutsArray;
	
	shortcutsArray = CFPreferencesCopyAppValue(kShortcutArrayKey, kCFPreferencesCurrentApplication);
	
	if(shortcutsArray != NULL)
	{
		for(x = 0; x < 30; x++)
		{
			shortcutString = CFArrayGetValueAtIndex( shortcutsArray, x);
			CFStringGetPascalString( shortcutString, shortcutData[x], 256, kCFStringEncodingASCII );
		}
		
		CFRelease(shortcutsArray);
	}
	else
	{
		for(x=0;x<30;x++)
			shortcutData[x][0]=0;
	}
}

OSStatus WriteDirURLRef(CFStringRef prefName, const FSRef *ref)
{
	OSStatus err = noErr;
	CFURLRef url;
	
	if(ref == NULL)
		return paramErr;	// break out here, stupid to go on.
	
	url = CFURLCreateFromFSRef(kCFAllocatorDefault, ref);
	
	if(url)
	{
		CFStringRef urlString;
		
		urlString = CFURLGetString(url);
		
		if(urlString)
		{
			CFPreferencesSetAppValue(prefName, urlString, kCFPreferencesCurrentApplication);
			CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
		}
		
		CFRelease(urlString);
		CFRelease(url);
	}
	else
		err = paramErr;
	
	return err;
}

OSStatus ReadDirURLRef(CFStringRef prefName, FSRef *ref)
{
	OSStatus err = noErr;
	CFStringRef urlString;
	
	if(ref == NULL)
		return paramErr;	// break out here, stupid to go on.
	
	urlString = CFPreferencesCopyAppValue(prefName, kCFPreferencesCurrentApplication);

	if(urlString)
	{
		CFURLRef url;
		
		url = CFURLCreateWithString(kCFAllocatorDefault, urlString, NULL);
		
		if(url)
			CFURLGetFSRef(url, ref);
		
		CFRelease(urlString);
		CFRelease(url);
	}
	else
		err = paramErr;
		
	return err;
}
