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

#ifndef _IRCNotify_
#define _IRCNotify_

#ifndef __IRCNOTIFY__
#define CONST const
#else
#define CONST
#endif

typedef struct notifyRec notifyRec, *notifyPtr;
struct notifyRec {
	CONST notifyPtr next, back;		//Next and previous notify entries.
	CONST Str63 nick;					//The nickname
	char waitingForResponse;			//True if we sent out an /ison
	char signedon;							//True if the user is signed on, false if not.
	char changed;							//True if we need to inform the user
	CONST char ISON;						//True if we're doing this for an /ison
};

pascal void DoNotify(linkPtr link, LongString *parse);
pascal void RunNotify(void);

pascal void ListNotify(linkPtr link);
pascal notifyPtr findNotify(linkPtr link, ConstStr255Param nick);
pascal void deleteNotify(linkPtr link, ConstStr255Param nick);
pascal notifyPtr addNotify(linkPtr link, ConstStr63Param nick, char ison);

pascal void killISONs(linkPtr link);
pascal void ProcessISON(linkPtr link, LongString *rest);
pascal void HandleISON(linkPtr link, LongString *rest);

#undef CONST

#endif