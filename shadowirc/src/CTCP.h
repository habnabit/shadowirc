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

#ifndef _CTCP
#define _CTCP

void CTCPSendReply(linkPtr link, ConstStr255Param fr, ConstStr255Param co, const LongString *st);

char doCTCP(ConstStr255Param from, ConstStr255Param uah, ConstStr255Param target, const LongString *s, char isReply, char ign, linkPtr link);
char DCCCTCP(connectionPtr connn, const LongString *ls);

#endif