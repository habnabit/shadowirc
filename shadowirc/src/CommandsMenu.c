/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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
#include "IRCGlobals.h"

#include "connections.h"
#include "MsgWindows.h"
#include "InputLine.h"
#include "channels.h"
#include "IRCChannels.h"
#include "utils.h"
#include "inline.h"
#include "CommandsMenu.h"

void HitCommandsMenu(short item)
{
	LongString ls, il;
	Str255 targ;
	int tack;
	short len;
	Str255 s2;
	short modifiers = 0;
	inputAreaDataPtr iad = ILGetInputDataFromMW(0);
	
	IADGetText(iad, &il);
	len = il.len;
	
	if(!CurrentTarget.bad && CurrentTarget.chan)
		pstrcpy(CurrentTarget.chan->chName, targ);
	else
		targ[0]=0;
		
	ls.len=0;
	tack = 0;
	switch(item)
	{
		case 1:
			LSStrLS("\p/join ", &ls);
			tack = 1;
			break;

		case 2:
			if(len)
			{
				LSStrLS("\p/part ", &ls);
				tack = 1;
			}
			else
				LSConcatStrAndStr("\p/part ", targ, &ls);
			break;

		case 3: //¥
				LSConcatStrAndStrAndStr("\p/kick ", targ, "\p ", &ls);
			break;
		
		case 4: //¥
			if(CurrentTarget.bad || (modifiers & shiftKey))
				LSConcatStrAndStrAndStr("\p/mode ", CurrentTarget.link->CurrentNick, "\p ", &ls);
			else
				LSConcatStrAndStrAndStr("\p/mode ", targ, "\p ", &ls);
			break;
		
		case 5: //¥
			LSConcatStrAndStrAndStr("\p/topic ", targ, "\p ", &ls);
			tack = 2;
			break;
		
		case 6:
			if(len)
			{
				LSNextArgND(&il, s2);
				if(s2[1]=='/')
				{
					LSNextArg(&il, 0);
					LSNextArg(&il, s2);
				}
				
				SAppend1(s2, ' ');
				LSConcatStrAndStrAndStr("\p/invite ", s2, targ, &ls);
				IADSetText(iad, &ls);
				IADSetCursorSelection(iad, 8, 8+s2[0]);
			}
			else
			{
				LSConcatStrAndStr("\p/invite  ", targ, &ls);
				s2[0] = 1;
			}
			IADSetText(iad, &ls);
			IADSetCursorSelection(iad, 8,8+s2[0]-1);
			ls.len=0;
			break;
		
		case 8:
			LSStrLS("\p/who ", &ls);
			tack = 1;
			break;
			
		case 9:
			LSStrLS("\p/whois ", &ls);
			tack = 1;
			break;
			
		case 11:
			LSStrLS("\p/nick ", &ls);
			tack = 1;
			break;
			
		case 12:
			LSStrLS("\p/away ", &ls);
			tack = 2;
			break;
			
		case 14:
			LSStrLS("\p/msg ", &ls);
			tack = 2;
			break;
			
		case 15:
			LSStrLS("\p/query ", &ls);
			tack = 1;
			break;
	}
	
	if(ls.len)
	{
		//Delete the first arg if it's a command.
		LSNextArgND(&il, s2);
		if(s2[1]=='/')
			LSNextArg(&il, 0);
		
		if(tack == 1) //first arg appended
		{
			LSNextArg(&il, s2);
			LSConcatLSAndStr(&ls, s2, &ls);
		}
		else if(tack == 2)
			LSConcatLSAndLS(&ls, &il, &ls);
	
		IADSetText(iad, &ls);
	}
}

