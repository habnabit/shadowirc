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

#ifndef _IRCCommands
#define _IRCCommands

typedef struct DNSLookupData {
	OSType magic;
	Str255 nick, host;
	Ptr u;
} DNSLookupData, *DNSLookupDataPtr;

#pragma lib_export on
#pragma export on
pascal void doBroadcast(linkPtr link, const LongString *ls, char action);
pascal void DoONotice(channelPtr ch, const LongString *text);
pascal void DNSLookup(Str255 addr, long saveReply);
pascal void HandleCommand(linkPtr link, LongString *ls);
pascal void StackModes(LongString *theMode, channelPtr channel, ConstStr255Param mode, ConstStr255Param arg);
pascal void SendCommand(linkPtr link, LongString *ls);
pascal void GetSignoffMessage(linkPtr link, LongString *s);

pascal void DoServer(linkPtr link, LongString *rest, const LongString* reason);
pascal void DoSignoff(linkPtr link, const LongString *reason);
pascal void NickListAdd(linkPtr link, ConstStr255Param n);
pascal void ChannelListAdd(linkPtr link, ConstStr255Param n);
#pragma export off
#pragma lib_export off

#pragma internal on
pascal void sendCTCP(linkPtr link, ConstStr255Param t, LongString *ls);

pascal void RegUser(linkPtr link);
pascal int signoffClearChannels(linkPtr link);
pascal void finishConnect(linkPtr link);
#pragma internal reset
#endif
