/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
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

#include "InetConfig.h"
#include "IRCGlobals.h"

ICInstance internetConfig;

void StartupIC(void)
{
	if(ICStart(&internetConfig, kApplicationSignature))
		internetConfig = 0;
}

void StopIC(void)
{
	if(internetConfig)
		ICStop(internetConfig);
}

OSErr OpenURL(Str255 url)
{
	if(internetConfig)
	{
		long selStart = 0;
		long endSel = url[0];
		return ICLaunchURL(internetConfig, "\p", (char*)&url[1], url[0], &selStart, &endSel);
	}
	else
		return -1;
}

OSStatus GetFSRefForDownloadsFolder(FSRef *ref)
{
	OSStatus err = noErr;
	SInt32 prefSize;
	ICFileSpecHandle fileSpecHdl = NULL;
	ICAttr attr;
	AliasHandle fsAliasHdl = NULL;
	
	if(internetConfig)
	{
		err = ICGetPref(internetConfig, kICDownloadFolder, &attr, NULL, &prefSize);
		if(err == noErr)
		{
			fileSpecHdl = (ICFileSpecHandle)NewHandle(prefSize);
			if(fileSpecHdl)
			{
				err = ICFindPrefHandle(internetConfig, kICDownloadFolder, &attr, (Handle)fileSpecHdl);
				
				fsAliasHdl = (AliasHandle)fileSpecHdl;
				err = HandToHand((Handle *)&fsAliasHdl);
				Munger((Handle)fsAliasHdl, 0, NULL, kICFileSpecHeaderSize, (Ptr)-1, 0);
				
				err = FSResolveAlias(NULL, fsAliasHdl, ref, NULL);
				
				DisposeHandle((Handle)fileSpecHdl);
				DisposeHandle((Handle)fsAliasHdl);
			}
		}
	}
	else
		err = paramErr; // something intelligible in case IC didn't work/isn't here
	
	return err;
}

OSStatus MapFileTypeCreator(const CFStringRef fileName, ICMapEntry *mapEntry)
{
	if(internetConfig)
	{
		Str255 pstrFileName;
		
		CFStringGetPascalString(fileName, pstrFileName, sizeof(Str255), CFStringGetSystemEncoding());
		return ICMapFilename(internetConfig, pstrFileName, mapEntry);
	}
	else
		return paramErr; // something intelligible in case IC didn't work/isn't here
}
