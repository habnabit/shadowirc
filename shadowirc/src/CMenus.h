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

#ifndef _CMenus_
#define _CMenus_

enum CMStrings {
	sCloseWindow = 1,
	sCopySelection,
	sCopyToInputline,
	sClearSelection,
	sClearAll ,
	sStopLogging,
	sCopyAll,
	sCutAll,
	sColorStyle,
	sStartLogging,
	sSaveSelectionToFile,
	sSaveWindowTextToFile,
	sConnectMenu,
	sDisconnectMenu,
	sConnectTo,
	sDisconnectFrom,
	sCloseDCCConn,
	sNone,
	sSave,
	sSaveAs,
	sCutS,
	sCopyS,
	sClearS,
	sPaste,
	smSavePreferences,
	smSaveTextFile
};

#pragma lib_export on
#pragma export on
pascal void CMAdd(struct cmmwData* d, ConstStr255Param item, long key);
pascal void CMAddSubmenu(struct cmmwData* d, ConstStr255Param item, short *id, MenuHandle *m);
pascal void CMSetCheckmark(struct cmmwData *d, long key, char checked);
#pragma export off
#pragma lib_export off

#pragma internal on
pascal char DetermineCM(WindowPtr w, Point where, char optCM);
pascal void CMSetCursor(void);
pascal char CMClick(WindowPtr w, const EventRecord *e);
#pragma internal reset

#endif