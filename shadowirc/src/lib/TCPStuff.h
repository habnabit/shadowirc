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

#ifndef _TCPStuff_
#define _TCPStuff_

#include <Carbon/Carbon.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "MasterTypes.h" // connectionPtr

typedef struct TCPConnection *TCPConnectionPtr;

OSErr TCPSendChars(int sockfd, const void* d, size_t len);

int TCPLocalPort(int sockfd);
int TCPRemoteIP(int sockfd, struct in_addr *ip);
int TCPLocalIP(int sockfd, struct in_addr *ip);

#endif
