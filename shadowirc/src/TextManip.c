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

#include "IRCGlobals.h"
#include "LongStrings.h"

#include "MsgWindows.h"
#include "connections.h"
#include "IRCChannels.h"
#include "TextManip.h"
#include "inline.h"

pascal void MWColor(LongString *ls, short colorNum)
{
	int i = maxLSlen - 2;
	
	if(i > ls->len)
		i = ls->len;
	
	//Move an extra byte so the copy is aligned
	BlockMoveData(&ls->data[0], &ls->data[2], i+1);
	ls->data[1] = 8;
	ls->data[2] = colorNum;
	ls->len = i + 2;
}

pascal void MWIrcleColor(LongString *ls, short colorNum)
{
	int i = maxLSlen - 2;
	
	if(i > ls->len)
		i = ls->len;
	
	//Move an extra byte so the copy is aligned
	BlockMoveData(&ls->data[0], &ls->data[2], i+1);
	ls->data[1] = 3;
	ls->data[2] = colorNum;
	ls->len = i + 2;
}

#pragma mark -

pascal void SMPrefix(LongString *ls, short where)
{
	Str255 s;
	char i;
	prefsPtr mp = mainPrefs;
	
	*(short*)s=0x0208;
	if(mp->serverMessagePrefix[0]>252)
		i=252;
	else
		i=mp->serverMessagePrefix[0];
	BlockMoveData(mp->serverMessagePrefix, &s[2], i+1);
	s[0]+=i;
	s[2]=sicServer;
	LSConcatStrAndLS(s, ls, ls);

	if(where == dsConsole)
		LineMsg(ls);
	else if(where == dsFrontWin)
		Message(ls);
}

pascal char SMLink(linkPtr link, LongString *ls)
{
	Str255 s;
	
	if(link && link != CurrentTarget.link)
	{
		if(link->CurrentServer[0])
			pstrcpy(link->CurrentServer, &s[1]);
		else
			pstrcpy(link->linkPrefs->linkName, &s[1]);
		s[0]=s[1]+3;
		s[1]='[';
		*(short*)&s[s[0]-1] = '] ';
		LSConcatStrAndLS(s, ls, ls);
		return 1;
	}
	else
		return 0;
}

pascal char SMPrefixLink(linkPtr link, LongString *ls, short where)
{
	char r;
	
	r=SMLink(link, ls);
	SMPrefix(ls, where);
	return r;
}

pascal void SMPrefixColor(LongString *ls, short where, short color)
{
	Str255 s;
	char i;
	prefsPtr mp = mainPrefs;
	
	*(short*)s=0x0208; //len 2 + 2
	if(mp->serverMessagePrefix[0]>252)
		i=252;
	else
		i=mp->serverMessagePrefix[0];
	BlockMoveData(mp->serverMessagePrefix, &s[2], i+1);
	s[0]+=i;
	s[2]=color;
	LSConcatStrAndLS(s, ls, ls);

	if(where == dsConsole)
		LineMsg(ls);
	else if(where == dsFrontWin)
		Message(ls);
}

pascal char SMPrefixLinkColor(linkPtr link, LongString *ls, short where, short color)
{
	char r;
	
	r=SMLink(link, ls);
	SMPrefixColor(ls, where, color);
	return r;
}

pascal void SMPrefixIrcleColor(LongString *ls, short where, short color)
{
	Str255 s;
	char i;
	short cm;
	prefsPtr mp=mainPrefs;
	
	*(short*)s=0x0203; //len 2 + 2
	if(mp->serverMessagePrefix[0]>252)
		i=252;
	else
		i=mp->serverMessagePrefix[0];
	BlockMoveData(mp->serverMessagePrefix, &s[2], i+1);
	s[0]+=i;
	s[2]=color;
	LSConcatStrAndLS(s, ls, ls);
	cm=mp->colorMethod;
	mp->colorMethod=cmIrcle;

	if(where == dsConsole)
		LineMsg(ls);
	else if(where == dsFrontWin)
		Message(ls);

	mp->colorMethod=cm;
}

pascal char SMPrefixLinkIrcleColor(linkPtr link, LongString *ls, short where, short color)
{
	char r;
	
	r=SMLink(link, ls);
	SMPrefixIrcleColor(ls, where, color);
	return r;
}

pascal void SMPrefixRGBColor(LongString *ls, short where, const RGBColor *rgb)
{
	Str255 s;
	char i;
	prefsPtr mp = mainPrefs;
	
	*(short*)s=0x0508; //len 2 + 6
	s[2] = sicCustomColor;
	s[3] = *(char*)&rgb->red;
	s[4] = *(char*)&rgb->green;
	s[5] = *(char*)&rgb->blue;
	if(mp->serverMessagePrefix[0]>248)
		i=252;
	else
		i=mp->serverMessagePrefix[0];
	BlockMoveData(mp->serverMessagePrefix, &s[6], i+1);
	s[0]+=i;
	LSConcatStrAndLS(s, ls, ls);

	if(where == dsConsole)
		LineMsg(ls);
	else if(where == dsFrontWin)
		Message(ls);
}

pascal char SMPrefixRGBLinkColor(linkPtr link, LongString *ls, short where, const RGBColor *rgb)
{
	char r;
	
	r=SMLink(link, ls);
	SMPrefixRGBColor(ls, where, rgb);
	return r;
}