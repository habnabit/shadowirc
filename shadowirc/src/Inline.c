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

#include "LongStrings.h"
#include "MsgWindows.h"
#include "IRCChannels.h"
#include "IRCGlobals.h"

pascal char IsChannel(ConstStringPtr s);
pascal char IsDCCName(ConstStringPtr s);
pascal void pstrcpy(ConstStringPtr f, StringPtr dest);
pascal void LineMsg(const LongString *ls);

#pragma lib_export on
#pragma export on
pascal char IsChannel(ConstStringPtr s)
{
	if((s[0]!=0) && ((s[1]=='#') || (s[1]=='&') || (s[1]=='+')))
		return true;
	else
		return false;
}

pascal char IsDCCName(ConstStringPtr s)
{
	if((s[0]!=0)&&(s[1]=='='))
		return true;
	else
		return false;
}

pascal void pstrcpy(ConstStringPtr f, StringPtr dest)
{
	BlockMoveData(f, dest, f[0]+1);
}

pascal void LineMsg(const LongString *ls)
{
	MWMessage(consoleWin, ls);
}

#pragma export off
#pragma lib_export off
