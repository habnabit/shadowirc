/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
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

ComponentInstance internetConfig;

pascal void StartupIC(void)
{
	if(ICCStart(&internetConfig, 'SIRC'))
		internetConfig = 0;
	else //setup prefs
	{
		if(ICCFindConfigFile(internetConfig, 0, 0))
			internetConfig = 0;
	}
}

pascal void StopIC(void)
{
	if(internetConfig)
		ICCStop(internetConfig);
}

pascal OSErr OpenURL(Str255 url)
{
	if(internetConfig)
	{
		long selStart = 0;
		long endSel = url[0];
		return ICCLaunchURL(internetConfig, "\p", (char*)&url[1], url[0], &selStart, &endSel);
	}
	else
		return -1;
}