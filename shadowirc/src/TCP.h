/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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



enum {
	C_NoEvent,
	C_Found,
	C_SearchFailed,
	C_Established,
	C_FailedToOpen,
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
        /* Formerly named tcpc (for TCPConnection)
         * Berkely Sockets use a normal file descriptor
         * Use 'int sockfd'
         */
	int sockfd;
        /*
         * XXX landonf: The "const" long "value" is used to pass back a pointer
         * to a pascal string (hostname). It also was used to pass back the IP address.
	 * sas will hold the IP address instead
         */
	CONST long value;
	struct sockaddr_storage *sas;
} connectionEventRecord, *CEPtr;

#undef CONST

//TCPConnections
// XXX landonf: These will be reworked for TCP sockets
// First long * argument is a connectionIndex
OSErr NewListenConnection (long * cp, int af, u_short localport, int backlog);
OSErr NewActiveConnection (long * cp, struct sockaddr *remotehost, u_short remoteport);

pascal OSErr InitConnections(void);
pascal OSErr FindAddress(long*, ConstStr255Param);
pascal OSErr FindName(long*, long);

void inet_ntoa_str(struct in_addr addr, StringPtr string);
void ntohl_str(u_int32_t net32, StringPtr string);
u_int32_t str_htonl(StringPtr string);

int GetConnectionSocket(long cp);

size_t TCPReceiveUntil(int sockfd, Ptr d, char c, size_t len);
size_t TCPReceiveChars(int sockfd, Ptr d, size_t len);

pascal void AbortConnection(long);
pascal void CloseTCPConnection(long);
pascal void FinishEverything(void);
pascal char GetConnectionEvent(CEPtr);

// getnameinfo is implemented on 10.1, definition is just missing in
// /usr/include/netdb.h
int getnameinfo (const struct sockaddr *, size_t, char *, size_t, char *, size_t, int);

#endif
