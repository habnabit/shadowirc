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

#ifndef _MWPanes_
#define _MWPanes_

enum mwAlign {
	mwForceLeft = -1,
	mwAlignLeft=0,
	mwAlignRight=1,
	mwPlaceholder = 255
};

enum mwDefaultWidgets {
	mwShadowIRCWidget = 'SIRC',
	mwPluginWidget = 'PLUG',
	
	mwTopicWidget = 'topc',
	mwModesWidget = 'mode',
	mwCMWidget = 'cmnu',
	mwHelpWidget = 'help',
	mwLinkWidget = 'link',
	
	mwModesWidgetWidth = 65
};

typedef struct mwModesWidgetData {
	short modeStart[8];
} mwModesWidgetData, *mwModesWidgetDataPtr;


enum mwDefaultPanes {
	mwShadowIRCPane = 'SIRC',
	mwPluginPane = 'PLUG',
	
	mwTextPane = 'text',
	mwWidgetsPane = 'widg',
	mwInputPane = 'inpt'
};

enum mwPaneAlign {
	mwPaneTop,
	mwPaneBottom,
	mwPaneLeft,
	mwPaneRight,
	mwPaneCenter
};


#pragma lib_export on
#pragma export on
pascal mwWidgetPtr MWNewWidget(MWPtr mw, long type, short align, short width);
pascal void MWRecalculateRects(MWPtr mw);
pascal void MWDestroyWidget(mwWidgetPtr w);
pascal mwWidgetPtr MWFindWidget(MWPtr mw, long type);

pascal mwPanePtr MWFindPane(MWPtr mw, long type);
pascal mwPanePtr MWNewPane(MWPtr mw, long type, short align, short height, short width);
pascal void MWPaneRecalculate(MWPtr mw);
pascal void MWPaneResize(MWPtr mw);
pascal char MWDestroyPane(mwPanePtr o);
pascal void MWPaneUpdate(MWPtr mw);
#pragma export off
#pragma lib_export off

#pragma internal on
pascal void MWDestroyAllPanes(MWPtr mw);
pascal void MWDestroyAllWidgets(MWPtr mw);

pascal mwWidgetPtr MWFindWidgetPoint(MWPtr mw, Point p);
pascal mwPanePtr MWFindPanePoint(MWPtr mw, Point p);
pascal void MWPaneActivate(MWPtr mw, char activate);
pascal void MWPaneClick(MWPtr mw, EventRecord *e);

#pragma internal reset
#endif