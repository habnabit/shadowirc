/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2005 John Bafford
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

#ifndef _channels
#define _channels

#ifndef _MasterTypes_
#include "MasterTypes.h"
#endif

#ifndef CHANNELS
	#define CONST const
#else
	#define CONST
#endif

typedef struct bansRec bansRec, *bansP;
struct bansRec {
	CONST bansP next;
	Str255 ban;
	Str63 setBy;
	unsigned long time;
};

typedef struct UserListRec UserListRec, *UserListPtr;
struct UserListRec {
	CONST UserListPtr next;
	Str63 nick;
	Str255 userhost;
	unsigned long lastUHUpdate;
	char isOper, isAway;
	char isOp, hasVoice;
	CONST struct Channel* channel;
	CONST long userlistServiceRefCon;
	CONST long userNum;
	char userlistIsSelected;
	char isHalfOp;
	char isIgnored;
	char pad;
	CONST long facesServiceRefCon;
};

enum chModes {
	modeT, modeN, modeI, modeM, modeP, modeS, modeL, modeK
};

typedef struct Channel {
	CONST channelPtr next;
	CONST char PAD;
	char active;
	char hasOps, hasVoice;
	CONST Str255 chName;
	Str255 topic;
	char modes[8];
	long limit;
	Str255 key;
	CONST bansP bans;
	CONST UserListPtr userlist;
	CONST short numUsers;
	Str63 topicSetBy;
	long topicSetOn;
	MWPtr window;
	CONST linkPtr link;
	Str63 lastJoiner;
	CONST bansP exceptions;
	char hasHalfOps;
	char gotNames;
} Channel;

extern long userNum;

extern char IsChannel(linkPtr link, ConstStringPtr s);

pascal bansP ChFindBan(channelPtr ch, ConstStr255Param n, char exceptionList);
pascal bansP ChMatchBan(channelPtr ch, ConstStr255Param n, char exceptionList);
pascal char ChGetBan(channelPtr ch, ConstStr255Param from, bansP *f, char exceptionList);
pascal void ChKillBan(channelPtr ch, bansP *x, char exceptionList);
pascal void ChKillAllBans(channelPtr ch, char exceptionList);

pascal channelPtr ChFind(ConstStr255Param name, linkPtr link);

pascal UserListPtr ULAddUser(channelPtr channel, ConstStr255Param nick, ConstStr255Param userhost);
pascal void ULDeleteUser(UserListPtr user);
pascal UserListPtr ULFindUserName(channelPtr channel, ConstStr255Param nick);
pascal char IsChannelValid(channelPtr c);
pascal char IsUserValid(UserListPtr u);

pascal UserListPtr ChFindUser(ConstStr255Param user);
pascal void ChClear(channelPtr ch);
pascal void ChDestroy(channelPtr ch);
pascal channelPtr ChCreate(ConstStr255Param name, linkPtr link);

#undef CONST
#endif