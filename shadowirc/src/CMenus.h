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
	sPaste
};

typedef enum menuCommand {
    mcNull,
    mcCloseWindow,
    mcCopy,
    mcCopyIL,
    mcClear,
    mcLog,
    mcSaveText,
    mcConnect,
    mcPlugin,
    mcSubmenu,

    mcILCut,
    mcILCopy,
    mcILPaste,
    mcILClear
} menuCommand;

typedef struct CMItem {
    MenuHandle m;
    long key;
    plugsPtr pluginRef;
    short mid;
    short command;
} CMItem;

typedef struct CMItemList {
    long numItems;
    CMItem list[1];
} CMItemList, *CMItemListP, **CMItemListH;

enum cmType {
    cmMW = 'cmmw',
    cmInputline = 'cmil',
    cmPlugin = 'cmpl'
};

typedef struct cmmwData {
    long type;

    WindowPtr window;
    MWPtr mw;
    mwPanePtr pane;

    Point where;

    long s0, s1;
    char noSelection;
    char oneWord;
    short unused;
    Str255 theWord;

    long id;
    short menuID;
    short popupsMenuID;

    MenuHandle m;
    MenuHandle colorMenu;
    CMItemListH items;
} cmmwData, *cmmwDataPtr;


pascal void CMAdd(struct cmmwData* d, ConstStr255Param item, long key);
pascal void CMAddSubmenu(struct cmmwData* d, ConstStr255Param item, short *id, MenuHandle *m);
pascal void CMSetCheckmark(struct cmmwData *d, long key, char checked);

pascal char DetermineCM(WindowPtr w, Point where, char optCM);
void CMSetCursor(char ctrlDown);
pascal char CMClick(WindowPtr w, const EventRecord *e);

#endif