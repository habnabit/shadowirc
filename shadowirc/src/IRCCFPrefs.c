/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
	dshadow@shadowirc.com
	Written for Mac OS X by:
			Michael Ledford <mledford@ugaalum.uga.edu>
			Copyright (C) 2002 - 2004 Michael Ledford, Sean McGovern, John Bafford
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
#include <CoreFoundation/CoreFoundation.h>
#include "IRCGlobals.h"
#include "IRCCFPrefs.h"

#define kColorDictKey CFSTR("Colors")

enum { kShadowIRCColorCount = 15 };

static const RGBColor ShadowIRCDefaultColors[] = {
	{28180, 15728, 0},		//sicServer
	{0, 0, 0},		//sicStandard
	{-1, 0, 0},		//sicPrivmsg
	{0, 42281, 12684},		//sicCTCP
	{0, 0, -1},		//sicOutgoing
	{28180, 15728, 0},		//sicNotice
	{-1, -1, -1},		//sicWindowBG
	{-1, 0, 0},		//sicUserChannelOp
	{0, 51773, 17040},		//sicUserVoice
	{-1, 32768, 0},		//sicUserOper
	{0, 0, 0},		//sicUserRegular
	{0, 0, 0},		//sicStatusLine
	{18350, 26214, 1966},		//sicUserIgnored
	{25368, 0, 25368},		//sicNotifyColor
	{0, 0, 0},		//sicTimestampColor
};

static const char* COLOR_NAMES[] = {
	"sicServer",
	"sicStandard",
	"sicPrivmsg",
	"sicCTCP",
	"sicOutgoing",
	"sicNotice",
	"sicWindowBG",
	"sicUserChannelOp",
	"sicUserVoice",
	"sicUserOper",
	"sicUserRegular",
	"sicStatusLine",
	"sicUserIgnored",
	"sicNotifyColor",
	"sicTimestampColor",
	NULL
};

CFArrayRef MakeColorComponentArray(RGBColor *color);
void CFNumberListToRGBColor(CFNumberRef colorVals[3], RGBColor *rgbColor);
CFDictionaryRef MakeColorDictionary(RGBColor colors[]);

#pragma mark -

void InitColorPrefs(void)
{
	BlockMoveData(ShadowIRCDefaultColors, shadowircColors, sizeof(ShadowIRCDefaultColors));
}

static CFStringRef* GetColorNamesList(void)
{
	static CFStringRef *colorNames = NULL;
	
	if(!colorNames)
	{
		int x = 0;
		const char *name;
		
		colorNames = malloc(sizeof(CFStringRef) * kShadowIRCColorCount);
		while((name = COLOR_NAMES[x]))
		{
			colorNames[x] = CFStringCreateWithCString(NULL, name, kCFStringEncodingMacRoman);
			x++;
		}
		colorNames[x] = NULL;
	}
	
	return colorNames;
}

CFDictionaryRef MakeColorDictionary(RGBColor colors[])
{
	CFStringRef *colorNames = GetColorNamesList();
	CFMutableDictionaryRef colorDict;
	CFArrayRef colorCompArr;
	CFStringRef curColorName;
	CFIndex x;
	
	colorDict = CFDictionaryCreateMutable(NULL, 0, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	
	if(colorDict)
	{
		x = 0;
		while((curColorName = colorNames[x]))
		{
			colorCompArr = MakeColorComponentArray(&colors[x]);
			if(colorCompArr)
			{
				CFDictionarySetValue(colorDict, curColorName, colorCompArr);
				CFRelease(colorCompArr);
			}
			
			x++;
		}
	}
	
	return colorDict;
}

CFArrayRef MakeColorComponentArray(RGBColor *color)
{
	CFMutableArrayRef colorCompArr;
	CFNumberRef red, green, blue;
	
	colorCompArr = CFArrayCreateMutable(NULL, 3, &kCFTypeArrayCallBacks);
	
	if(colorCompArr)
	{
		int r, g, b;
		
		r = color->red;
		g = color->green;
		b = color->blue;
		
		red = CFNumberCreate(NULL, kCFNumberIntType, &r);
		green = CFNumberCreate(NULL, kCFNumberIntType, &g);
		blue = CFNumberCreate(NULL, kCFNumberIntType, &b);
		
		CFArrayAppendValue(colorCompArr, red);
		CFArrayAppendValue(colorCompArr, green);
		CFArrayAppendValue(colorCompArr, blue);
		
		CFRelease(red);
		CFRelease(green);
		CFRelease(blue);
	}
	
	return colorCompArr;
}

void CFNumberListToRGBColor(CFNumberRef colorVals[3], RGBColor *rgbColor)
{
	int r, g, b;
	
	if(!CFNumberGetValue(colorVals[0], kCFNumberIntType, &r))
		r = 0;
	
	if(!CFNumberGetValue(colorVals[1], kCFNumberIntType, &g))
		g = 0;
	
	if(!CFNumberGetValue(colorVals[2], kCFNumberIntType, &b))
		b = 0;
	
	rgbColor->red = r;
	rgbColor->green = g;
	rgbColor->blue = b;
}

void ReadColorCFPrefs(RGBColor colors[])
{
	CFStringRef *colorNames = GetColorNamesList();
	CFDictionaryRef colorDict;
	CFIndex colorKeyCount;
	CFRange colorRange = {0, 3};
	CFNumberRef colorVals[3];
	CFArrayRef colorComponentArr;
	
	colorDict = CFPreferencesCopyAppValue(kColorDictKey, kCFPreferencesCurrentApplication);
	
	if(colorDict)
	{
		int x = 0;
		CFStringRef curColorName;
		
		while((curColorName = colorNames[x]))
		{
			colorKeyCount = 0;
			colorComponentArr = CFDictionaryGetValue(colorDict, curColorName);
			
			if(colorComponentArr)
				colorKeyCount = CFArrayGetCount(colorComponentArr);
			
			if(colorKeyCount != 3)
				colors[x] = ShadowIRCDefaultColors[x];
			else
			{
				CFArrayGetValues(colorComponentArr, colorRange, &colorVals);
				
				CFNumberListToRGBColor(colorVals, &colors[x]);
			}
			
			x++;
		}
		
		CFRelease(colorDict);
	}
	else
		InitColorPrefs();
}

void SaveColorsCFPrefs(void)
{
	CFDictionaryRef colorDict = MakeColorDictionary(shadowircColors);
	
	CFPreferencesSetAppValue(kColorDictKey, colorDict, kCFPreferencesCurrentApplication);
	CFRelease(colorDict);
}

#pragma mark -

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
	FSRef localRef;
	
	if(ref == NULL)
		return paramErr;	// break out here, stupid to go on.
	
	urlString = CFPreferencesCopyAppValue(prefName, kCFPreferencesCurrentApplication);

	if(urlString)
	{
		CFURLRef url;
		
		url = CFURLCreateWithString(kCFAllocatorDefault, urlString, NULL);
		
		if(url)
		{
			if(CFURLGetFSRef(url, &localRef))
				BlockMoveData(&localRef, ref, sizeof(FSRef));
			else
			{
				ref = NULL;
				err = paramErr;
			}
		}
		
		CFRelease(urlString);
		CFRelease(url);
	}
	else
		err = paramErr;
		
	return err;
}
