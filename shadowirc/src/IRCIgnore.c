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

#include "StringList.h"
#include "LongStrings.h"
#include "IRCIgnore.h"
#include "utils.h"
#include "IRCGlobals.h"
#include "TextManip.h"
#include "Inline.h"

ignorePtr ignores=0;

inline void deleteIgnores(ConstStr255Param str);
inline void addignores(ConstStr255Param str);

pascal void ListIgnores(void)
{
	short i;
	LongString ls;
	ignorePtr ign=ignores;
	Str255 s;
	ConstStringPtr s2;
	
	if(ign)
	{
		i=0;
		LSGetIntString(&ls, spInfo, sIgnoreList);
		while(ign)
		{
			if(ls.len+ign->mask[0]> (maxLSlen-10))
			{
				SMPrefix(&ls, dsFrontWin);
				ls.len=0;
			}
			LSConcatLSAndStr(&ls, ign->mask, &ls);
			LSAppend1(ls, ' ');
			i++;
			ign=ign->next;
		}
		if(ls.len)
			SMPrefix(&ls, dsFrontWin);
		NumToString(i, s);
		s2  = GetIntStringPtr(spInfo, 7 + (i>1));
		LSParamString(&ls, GetIntStringPtr(spInfo, sIgnoring), s, s2, 0, 0);
		SMPrefix(&ls, dsFrontWin);
	}
	else
	{
		LSGetIntString(&ls, spInfo, sNotIgnoringAnyone);
		SMPrefix(&ls, dsFrontWin);
	}
}

pascal char IsIgnored(ConstStr255Param uah)
{
	ignorePtr ign;

	linkfor(ign, ignores)
		if(maskMatch(uah, ign->mask))
		{
			ign->lastIgnored=now;
			return 1;
		}

	return 0;
}

pascal char IsIgnoredNickUser(ConstStr255Param nick, ConstStr255Param user)
{
	Str255 s;
	
	pstrcpy(user, &s[nick[0]+1]);
	pstrcpy(nick, s);
	s[s[0]+1] = '!';
	s[0]+=user[0]+1;
	
	return IsIgnored(s);
}

pascal ignorePtr findIgnore(ConstStr255Param mask, ignorePtr ign)
{
	if(!ign)
		ign=ignores;
	
	while(ign)
	{
		if(maskMatch(ign->mask, mask))
			return ign;
		ign=ign->next;
	}
	return 0;
}

inline void deleteIgnores(ConstStr255Param str)
{
	ignorePtr ign=0;
	ignorePtr temp;
	
	do
	{
		ign=findIgnore(str, ign);
		if(ign)
		{
			if(ign->back)
				ign->back->next=ign->next;
			if(ign->next)
				ign->next->back=ign->back;
			temp=ign->next;
			if(ignores==ign)
				ignores=temp;
			DisposePtr((Ptr)ign);
			ign=temp;
		}
	} while(ign);
}

inline void addignores(ConstStr255Param str)
{
	ignorePtr ign;
	
	ign=(ignorePtr)NewPtr(sizeof(ignoreRec));
	ign->next=ignores;
	ign->back=0;
	if(ign->next)
		ign->next->back=ign;
	
	pstrcpy(str, ign->mask);
	ign->time=now;
	ign->lastIgnored=now;
	ignores=ign;
}

pascal void DoIgnore(LongString  *in)
{
	Str255 str;
	char remove;
	LongString out;
	int x;
	char dontList = 0;
	
	if(in->len)
	{
		if(in->data[1]=='¥')
		{
			dontList = 1;
			LSDelete(in, 1, 1);
		}
		for(x=1;x<=in->len;x++)
			if(in->data[x]==',')
				in->data[x]=' ';
		while(in->len)
		{
			LSNextArg(in, str);
			remove=str[1]=='-';
			if(remove || (str[1]=='+'))
				pdelete(str, 1, 1);

			if(!pos('@', str)) //tack on a !*@*
			{
				*(long*)&str[str[0]+1]='!*@*';
				str[0]+=4;
			}
			else if(!pos('!', str)) //no 
			{
				pstrcpy(str, &str[2]);
				*(short*)&str[1]='*!';
				str[0]+=2;
			}
			if(remove)
				deleteIgnores(str);
			else
			{
				if(IsIgnored(str))
				{
					LSParamString(&out, GetIntStringPtr(spInfo, sAlreadyIgnoring), str, 0, 0, 0);
					SMPrefix(&out, dsFrontWin);
				}
				else
					addignores(str);
			}
		}
	}
	
	if(!dontList)
		ListIgnores();
}