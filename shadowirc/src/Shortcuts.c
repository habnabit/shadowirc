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

#include "WASTE.h"

#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "connections.h"
#include "channels.h"
#include "plugins.h"
#include "IRCChannels.h"
#include "Inputline.h"
#include "MenuCommands.h"
#include "IRCInput.h"
#include "utils.h"
#include "inline.h"
#include "Shortcuts.h"

inline void PluginShortcutText(Handle lh);
static pascal void ParseShortcutText(ConstStr255Param s);


void DoShortcutsEditor(void)
{
	short x;
	DialogPtr shortcutsDlg;
	short num=0;
	short num2;
	Str255 s;
	Handle sct;
	Str255 *shorts=(Str255*)NewPtr(30 * 256);
	
	if(!shorts) //not enough memory
		return;
	
	BlockMoveData(mainPrefs->shortcuts, shorts, 30 * 256);
	
	shortcutsDlg=GetNewDialog(1003, 0, (WindowPtr)-1);
	
	for(x=0;x<=9;x++)
		SetText(shortcutsDlg, 14+x, shorts[x]);
	
	sct = GetResource('TEXT', 127);
	if(sct)
	{
		SetDlogItemTextHdl(shortcutsDlg, 24, *sct, GetHandleSize(sct));
		ReleaseResource(sct);
	}
	
	SetupModalDialog(shortcutsDlg, 1, 2);
	do
	{
		ModalDialog(StdDlgFilter, &x);
		
		if(x==1)
		{
			for(x=0;x<=9;x++)
				GetText(shortcutsDlg, 14+x, shorts[x + (num * 10)]);

			BlockMoveData(shorts, mainPrefs->shortcuts, 30 * 256);
			ShortcutsMenuUpdate();
			break;
		}
		else if(x==2)
			break;
		else  if(x==25)
		{
			num2=GetControlValue(GetControlHandle(shortcutsDlg, 25))-1;
			if(num2!=num)
			{
				for(x=0;x<=9;x++)
					GetText(shortcutsDlg, 14+x, shorts[x + (num * 10)]);
				num=num2;
				
				for(x=0;x<=9;x++)
				{
					NumToString(x+1 + (num*10), s);
					SAppend1(s, ':');
					SetText(shortcutsDlg, 4+x, s);
					SetText(shortcutsDlg, 14+x, shorts[x + (num * 10)]);
				}
			}
		}
	} while(1);
	
	DisposePtr((Ptr)shorts);
	DisposeDialog(shortcutsDlg);
	FinishModalDialog();
}

pascal void ShortcutsMenuUpdate(void)
{
	short x;
	Str255 s;
	MenuHandle mh;
	
	mh=gShortcutsMenu;
	for(x=0;x<30;x++)
	{
		pstrcpy(mainPrefs->shortcuts[x], s);
		if(!s[0])
		{
			*(short*)&s=0x0120; //' '
			DisableMenuItem(mh, 3+x);
		}
		else
			EnableMenuItem(mh, 3+x);
		
		if(s[0]>35)
		{
			s[0]=36;
			s[36]='É';
		}
		SetMenuItemText(mh, 3+x, s);
	}
}

#pragma mark -

inline void PluginShortcutText(Handle lh)
{
	pShortcutProcessData p;
	
	p.textH = lh;
	runPlugins(pShortcutProcessMessage, &p);
}

inline void RepeatMunger(Handle text, long offset, const void* ptr1, long len1, const void* ptr2, long len2)
{
	int i;
	do {
		i = Munger(text, offset, ptr1, len1, ptr2, len2);
	} while(i >= 0);
}

pascal void ProcessShortcutText(LongString *ls, short *cursorPos, short *endPos)
{
	Handle lh;
	long s;
	Str255 st;
	long l;
	LongString ilt;
	
	if(ls->len)
	{
		lh=NewHandle(ls->len);
		if(lh)
		{
			BlockMoveData(&ls->data[1], *lh, ls->len);
			
			if(CurrentTarget.bad)
				st[0]=0;
			else
				MWGetName(CurrentTarget.mw, st);
			
			RepeatMunger(lh, 0, "$channel", 8, &st[1], st[0]);
			RepeatMunger(lh, 0, "$nick", 5, &CurrentTarget.link->CurrentNick[1], CurrentTarget.link->CurrentNick[0]);
			RepeatMunger(lh, 0, "$server", 7, &CurrentTarget.link->CurrentServer[1], CurrentTarget.link->CurrentServer[0]);
			
			if(CurrentTarget.chan)
				pstrcpy(CurrentTarget.chan->lastJoiner, st);
			else if((CurrentTarget.dcc && CurrentTarget.dcc->dcc) ||
						(CurrentTarget.type==targQuery &&!CurrentTarget.bad))
				MWGetName(CurrentTarget.mw, st);
			else
				st[0]=0;

			RepeatMunger(lh, 0, "$lastjoiner", 11, &st[1], st[0]);
			RepeatMunger(lh, 0, "$version", 8, &CL_VERSION[1], CL_VERSION[0]);
			
			GetDateTime((unsigned long*)&l);
			DateString(l, longDate, st, 0);
			RepeatMunger(lh, 0, "$date.l", 7, &st[1], st[0]);
			
			DateString(l, shortDate, st, 0);
			RepeatMunger(lh, 0, "$date", 5, &st[1], st[0]);
			
			TimeString(l, true, st, 0);
			RepeatMunger(lh, 0, "$time.s", 7, &st[1], st[0]);
			
			TimeString(l, false, st, 0);
			RepeatMunger(lh, 0, "$time", 5, &st[1], st[0]);
			
			PluginShortcutText(lh);
			
			GetInputLine(&ilt);
			RepeatMunger(lh, 0, "$il", 3, &ilt.data[1], ilt.len);
			
			if(cursorPos)
			{
				*cursorPos=Munger(lh, 0, "$->", 3, (Ptr)-1, 0);
				if(endPos)
					*endPos=Munger(lh, 0, "$<-", 3, (Ptr)-1, 0);
			}
			
			s=GetHandleSize(lh);
			if(s>maxLSlen)
				s=maxLSlen-20;
			ls->len=s;
			BlockMoveData(*lh, &ls->data[1], s);
			DisposeHandle(lh);
		}
	}
}

static pascal void ParseShortcutText(ConstStr255Param s)
{
	Str255 s2,s3;
	LongString ls;
	int p;
	int x;
	short cp, ep;
	Handle h;
	long l1,l2;
	WEReference il = ILGetWE();
	
	if(!il)
		return;
	
	pstrcpy(s, s2);
	do
	{
		x=1;
		p=0;
		while(x<s2[0])
		{
			if(s2[x]=='\\')
			{
				if(s2[x+1]=='n')
				{
					p=x;
					break;
				}
				else
					x+=2;
			}
			else
				x++;
		}
		if(p)
		{
			myCopy(s2, 1, p-1, s3);
			LSStrLS(s3, &ls);
			pdelete(s2, 1, p+1);
			ProcessShortcutText(&ls, &cp, &ep);
			SetInputLine(&ls);
			GetLine(false, CurrentTarget.mw);
		}
		else
		{
			LSStrLS(s2, &ls);
			ProcessShortcutText(&ls, &cp, &ep);
			p=0;
			x=0;
			if(cp>=0) //check for a selection and get it, if there is one
			{
				WEGetSelection(&l1,&l2, il);
				if(l1!=l2)
				{
					h=NewHandle(l2-l1);
					WECopyRange(l1, l2, h, 0, 0, il);
					SetInputLine(&ls);
					if(ep<0)
						ep=cp;
					SetInputLineCursorSelection(cp,ep);
					if(l2-l1!=0)
					{
						HLock(h);
						WEInsert(*h, l2-l1, 0, 0, il);
						SetInputLineCursorSelection(cp, (l2-l1)+cp);
					}
					DisposeHandle(h);
				}
				else
				{
					SetInputLine(&ls);
					if(ep<cp)
						ep=cp;
					SetInputLineCursorSelection(cp,ep);
				}
			}
			else
				SetInputLine(&ls);
			s2[0]=0;
		}
	}while(s2[0]);
}

pascal void ShortcutsMenu(short item)
{
	if(item >= 3)
	{
		if(mainPrefs->shortcuts[item-3][0])
			ParseShortcutText(mainPrefs->shortcuts[item-3]);
	}
}

void ShortcutsMenuInit(MenuHandle sm)
{
	int x;
	const int kShortcutsOffset = 2;
	
	for(x = 1; x <= 30; x++)
	{
		AppendMenuItemText(sm, "\p.");
		SetMenuItemCommandID(sm, kShortcutsOffset + x, 'SHOR');
		SetItemCmd(sm, kShortcutsOffset + x, '0' + x % 10);
		if(x >= 10 && x <= 19)
			SetMenuItemModifiers(sm, kShortcutsOffset + x, kMenuOptionModifier);
		else if(x >= 20)
			SetMenuItemModifiers(sm, kShortcutsOffset + x, kMenuControlModifier | kMenuNoCommandModifier);
	}
}

