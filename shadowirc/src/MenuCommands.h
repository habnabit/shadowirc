/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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

enum menuCommands
{
	kCommandPrevWin = 'PWIN',
	kCommandNextWin = 'NWIN',
	
	kCommandWindowService = 'WINS',
	kCommandWindowSelect = 'WINP'
};

extern MenuHandle gAppleMenu, gEditMenu, gFileMenu, gShortcutsMenu, gWindowMenu, gHelpMenu;

void DoFind(MWPtr mw, char again);
void DoFontSizeWindow(MWPtr mw);
void HitFontsMenu(MWPtr mw, short item);

void HitSelectConnectionMenu(short item);

pascal void HitConnectionListMenu(short item);
pascal void HitSignoffConnectionListMenu(short item);

int HitWindowPluginServiceMenu(const short item);
int HitWindowSelectWindowMenu(const short item);

void HitEditMenu(MWPtr mw, short item);

void HitApplicationURLMenu(short item);

void MenuInit(void);
void ConnectionMenuInit(void);
void ConnectionMenuHilites(void);

char ToggleConsoleWindow(void);
