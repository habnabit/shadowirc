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

#ifndef _TCP
#define _TCP

#ifndef _TCPStuff_
#include "TCPStuff.h"
#endif

#pragma internal on

enum {
	C_NoEvent,
	C_Found,
	C_SearchFailed,
	C_NameFound,
	C_NameSearchFailed,
	C_Established,
	C_FailedToOpen,
	C_Closing,
	C_Closed,
	C_CharsAvailable
};

#ifndef __TCPCONNECTIONS__
#define CONST const
#else
#define CONST
#endif

typedef struct connectionEventRecord {
	CONST char event;
	CONST char timedout;
	const short PAD;
	CONST long connection;
	CONST TCPConnectionPtr tcpc;
	CONST long value;
} connectionEventRecord, *CEPtr;

#undef CONST

//TCPConnections
pascal OSErr NewPassiveConnection (long*, long, short, long, short);
pascal OSErr NewActiveConnection (long*, long, short, long, short);

pascal OSErr InitConnections(void);
pascal OSErr FindAddress(long*, ConstStr255Param);
pascal OSErr FindName(long*, long);

pascal TCPConnectionPtr GetConnectionTCPC(long cp);

pascal OSErr TCPReceiveUpTo(TCPConnectionPtr, char, long, Ptr, long, long*, char*);
pascal OSErr TCPReceiveChars(TCPConnectionPtr, void*, short);

pascal void AbortConnection(long);
pascal void CloseTCPConnection(long);
pascal void FinishEverything(void);
pascal char GetConnectionEvent(CEPtr);
#pragma internal reset

#endif