/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
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

#ifndef _IRCChannels
#define _IRCChannels

enum targtype {
	targInvalid=-1,
	targConsole=0,
	targChannel,
	targQuery,
	targPlugin,
	targHelp, //unused
	targDCC,
	targText
};

typedef struct target target, *targetPtr;
struct target {
#ifndef _MsgWindows_
	Ptr mw;
#else
	MWPtr mw;
#endif
	linkPtr link;			//The link the window was created for.
	short type;			//targtype
	char inactive;		//for targChannel, targDCC. If true, then the dcc/channel isn't active. (you aren't really in it)
	char bad;				//If true, this is an invalid target (i.e. target is bad, or console, or inactive channel, etc)
	channelPtr chan;		//For targChannel && !inactive, a channelPtr. otherwise nil
	connectionPtr dcc;	//For targDCC && !inactive, the connectionPtr for the dcc. otherwise nil
};

extern target CurrentTarget;

pascal void DoModeLWindow(channelPtr ch, LongString *ls);
pascal void DoModeKWindow(channelPtr ch, LongString *ls);

pascal MWPtr ChannelWindow(linkPtr link, ConstStr255Param ch);

pascal void ChTopicWindow(channelPtr ch);

#ifdef _LongStrings
pascal void Message(const LongString *msg);
pascal void ChMsg(channelPtr ch, const LongString *msg);
pascal void ChannelMsg(linkPtr link, ConstStr255Param ch, const LongString *msg);
#endif

pascal void DrawMWinStatus(MWPtr mw);

#ifdef _MsgWindows_
pascal void MWPart(MWPtr mw);

pascal void SetTarget(MWPtr w, targetPtr targ);
pascal void ChPart(MWPtr w);
pascal void DoJoinSelWin(MWPtr w);
pascal void MWStatusClick(MWPtr mw, Point p);
#endif

pascal void GetIcons(void);

pascal MWPtr ChJoin(channelPtr ch);
pascal MWPtr DoJoinQuery(ConstStr255Param ch, linkPtr link);

pascal void InvalTarget(targetPtr targ);

pascal void CloseHelp(void);
pascal void ShowHelp(short helpID);

#endif