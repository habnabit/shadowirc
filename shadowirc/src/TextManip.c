/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2000 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.comes

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
#include "utils.h"

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

pascal void FormatMessage(ConstStr255Param nick, const LongString *text, ConstStringPtr nickC, ConstStringPtr textC, int type, LongString *out)
{
	LongString o;
	
	//First off, format the nick
	if(nick[0])
		FormatNick(nick, &o, nickC, type);
	else
		o.len = 0;
	
	//Prepare the text
	if(!textC)
	{
		if(type & kNickSend)
			textC = "\p\xE\x8\x4";
		else if(type & kNickPrivmsg)
			textC = "\p\xE\x8\x2"; //should be E and not F?
		else if(type & kNickNotice)
			textC = "\p\xE\x8\x5";
		else //if(type & kNickNormal)
			textC = "\p\xE";
	}
	
	LSConcatLSAndStrAndLS(&o, textC, text, out);
}

pascal void FormatNick(ConstStr255Param nick, LongString *ls, ConstStringPtr nickC, int type)
{	
	//Set the coloration on the nick
	if(!nickC)
	{
		if(type & kNickSend)
			nickC = "\p\xE\x8\x4";
		else if(type & kNickPrivmsg)
			nickC = "\p\xE\x8\x2"; //should be E and not F?
		else if(type & kNickNotice)
			nickC = "\p\xE\x8\x5";
		else //if(type & kNickNormal)
			nickC = "\p\xE";
	}
	
	//Tack the nick into the LS
	pstrcpy(nickC, ls->data);
	ls->len = ls->data[0];
	
	if(type & (kNickNormal | kNickPrivmsg | kNickOther | kNickAction))
	{
		int extraSpace;
		
		if(type & kNickAction)
		{
			extraSpace = -1;
			if(mainPrefs->textIndenting)
				LSConcatLSAndStrAndStr(ls, "\p             ", nick, ls);
			else
				LSConcatLSAndStrAndStr(ls, "\p * ", nick, ls);
			LSAppend1(*ls, ' ');
		}
		else if(type & kNickPrivmsg)
		{
			extraSpace =1;
			LSConcatLSAndStr(ls, nickC, ls);
			LSAppend1(*ls, '*');
			LSConcatLSAndStr(ls, nick, ls);
			LSAppend2(*ls, '* ');
		}
		else if(type & kNickOther)
		{
			extraSpace =1;
			LSConcatLSAndStr(ls, nickC, ls);
			LSAppend1(*ls, '(');
			LSConcatLSAndStr(ls, nick, ls);
			LSAppend2(*ls, ') ');
		}
		else if(mainPrefs->ircIIDisplay)
		{
			extraSpace = 1;
			LSConcatLSAndStr(ls, nickC, ls);
			LSAppend1(*ls, '<');
			LSConcatLSAndStr(ls, nick, ls);
			LSAppend2(*ls, '> ');
		}
		else
		{
			extraSpace = 0;
			LSConcatLSAndStr(ls, nick, ls);
			LSAppend2(*ls, ': ');
		}
		
		if(mainPrefs->textIndenting && extraSpace != -1)
		{
			LSMakeStr(*ls);
			padBegin(ls->data, 12 + extraSpace, ls->data);
			ls->len = ls->data[0];
		}
	}
	else if(type & kNickNotice)
	{
		if(type & kNickSend)
			LSConcatLSAndStrAndStr(ls, "\p> -", nick, ls);
		else
			LSConcatLSAndStrAndStr(ls, "\p-", nick, ls);
		LSAppend2(*ls, '- ');
	}
}


#pragma mark -

pascal void SMPrefix(LongString *ls, short where)
{
	SMPrefixColor(ls, where, sicServer);
}

pascal char SMLink(linkPtr link, LongString *ls)
{
	Str255 s;
	
	if(link && link != CurrentTarget.link)
	{
		pstrcpy("\p[", s);
		if(link->CurrentServer[0])
			pstrcat(s, link->CurrentServer, s);
		else
			pstrcat(s, link->linkPrefs->linkName, s);
		LSConcatStrAndStrAndLS(s, "\p] ", ls, ls);
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
	
	*(short*)s=0x0208; //len 2 + 2
	s[2]=color;
	
	LSConcatStrAndStrAndLS(s, mainPrefs->serverMessagePrefix, ls, ls);

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
	short cm;
	prefsPtr mp=mainPrefs;
	
	*(short*)s=0x0203; //len 2 + 2
	s[2]=color;
	
	LSConcatStrAndStrAndLS(s, mp->serverMessagePrefix, ls, ls);
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
	
	*(short*)s=0x0508; //len 2 + 6
	s[2] = sicCustomColor;
	s[3] = *(char*)&rgb->red;
	s[4] = *(char*)&rgb->green;
	s[5] = *(char*)&rgb->blue;
	
	LSConcatStrAndStrAndLS(s, mainPrefs->serverMessagePrefix, ls, ls);

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