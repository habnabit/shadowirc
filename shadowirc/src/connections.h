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

#ifndef _connections
#define _connections

#ifndef _MasterTypes_
#include "MasterTypes.h"
#endif

#ifndef _LongStrings
#include "LongStrings.h"
#endif

#ifndef _HashTable_
#include "HashTable.h"
#endif

#ifndef _TCP
# include "TCP.h"
#endif

#include "IRCGlobals.h"

#ifndef __CONNECTIONS__
#define CONST const
#else
#define CONST
#endif

enum connTypes {connNIL, connIRC, connIDENTD, connDCC, connDNSIP, connDNSNAME, connPLUGIN, connSOCKS, connSTALE};

enum connectStage {
	csNil = 0,								//No connection in progress
	csStartingToConnect,			//Starting to connect
	csLookingUp,							//Looking up IP
	csOpeningConnection,			//Trying to open the connection (found IP)
	csConnected,							//Data transfer is now possible
	csSentRegistration,				//Send USER/NICK/PASS
	csOnline,								//Server has sent numeric 001 - we are online
	
	csSOCKSNegotiatingMethod,		//Negotiating an authentication method with SOCKS server
	csSOCKSSendingAuthRequest,	//Attempting authentication with SOCKS server
	csSOCKSSendingRequest,			//Sending connection request to SOCKS server

	csLookingUp2,						//Secondary IP lookup
	
	
	csFailedToLookup = -1,			//IP lookup failed
	csConnectFailed = -2,			//connect failed
	csFailedToConnect = -3,		//connect failed
	csSOCKSAuthFailed = -4,		//SOCKS authentication failed
	csSOCKSConnectFailed = -5	//SOCKS connection failed
};

enum {
	kServerOKBadSOCKS = 50
};

enum lineTermList {
	eolLF,
	eolCR,
	eolCRLF
};

#ifndef _TCP
typedef pascal void (*ConnInputFuncPtr)(void*, connectionPtr conn);
#else
typedef pascal void (*ConnInputFuncPtr)(CEPtr, connectionPtr conn);
#endif

typedef pascal void (*DCCInputFuncPtr)(connectionPtr conn);

struct Connection {
	CONST connectionPtr next;
	CONST long private_socket;
	CONST short connType;
	CONST short realConnType;
	
#ifndef _DCC
	Ptr dcc;
#else
	dccPtr dcc;
#endif
	void* refCon;
	linkPtr link;
	/*
         * ip: store ip address
         * ip2: used w/ storing SOCKS proxy IP address
         * sockfd: hold TCP socket
         */
	struct in_addr ip;
        struct in_addr ip2;
        int sockfd;
	Str255 name;
	unsigned short port;
	short connectStage;
	unsigned long lastData;

	char outgoing;
	char tryingToConnect;
	char textOrBinary;
	char lineTerm;
	
	unsigned long dataIn, dataOut;	//Bytes sent or received. (Valid for text-based connections only)
	
	//Not in API
	CONST ConnInputFuncPtr InputFunc;
	DCCInputFuncPtr DCCInputFunc; //Should be CONST...
	struct plugsRec* pluginRef;
	
	unsigned long closeTime;
	
	Str63 socksName;
	short socksPort;
	CONST short socksType;
	char socksMethodVersion, socksMethod;
};

enum maxLinks {
	maxLinks = 10
};

typedef struct Link Link;
struct Link {
	connectionPtr conn;
	connectionPtr identConn;
	long connectedTime;
	
	short serverStatus;
	short connectingNickAttempt;
	char neverConnected;
	char UserRegistered;
	char gotMOTD, finishedMOTD;
	char alreadyFinished;
	char isAway;
	short lastNumerics[10];

	short linkNum;

	Str63 CurrentNick;
	Str255 CurrentServer;
	Str255 serverTryingName;
	short serverTryingPort;
	short numOutstandingISON;
#ifndef _IRCNotify_
	Ptr notifyList;
#else
	notifyPtr notifyList;
#endif

	CONST linkPrefsPtr linkPrefs;
#ifndef _MsgWindows_
	Ptr windows[10];
#else
	MWPtr windows[10];
#endif
	unsigned long lastConnAttempt;
	short connectionAttempts;
	char waitingToRetry;
	char needUserHosts;

	StringPtr umodes;						//The possible usermodes as reported by the server
	StringPtr yourUmodes;				//The usermodes you have.
	StringPtr chModes;
	StringPtr awayString;				//A pointer to the array message, or nil if not away.
	struct Channel *channelList;
	CONST linkPtr next, prev;

	short outstandingUSERHOST;
	
	//End of public API
	HTPtr serverOptions;
	LongString firstHalfOfIncoming;
};

extern connectionPtr fConn;
extern linkPtr firstLink;

pascal short CountLinks(void);
pascal linkPtr GetLinkNum(int i);

pascal void initLinks(void);

pascal char ConnStartNetworking();
pascal void ConnStopNetworking();

pascal connectionPtr newConnection(short connType);
pascal void newIRCConnection(linkPtr link);

pascal void ConnStale(connectionPtr conn);
pascal void ConnDeSOCKS(connectionPtr conn);

pascal void ConnSetup(connectionPtr c, ConstStr255Param name, unsigned short port);

pascal char IsLinkValid(linkPtr link);

pascal char LinkCanMode(linkPtr link, char mode, char umode);
pascal void deleteConnection(connectionPtr *c);
pascal connectionPtr findConnectionSock(long sock);
pascal connectionPtr pluginNewConnection(char textOrBinary);

pascal void ConnClose(connectionPtr conn);
pascal char ConnNewActive(connectionPtr c);
pascal char ConnNewListen(connectionPtr c, int backlog);
size_t ConnGetData(connectionPtr conn, Ptr d, size_t len);
size_t ConnGetUntil(connectionPtr conn, Ptr d, char c, size_t len);
pascal unsigned short ConnGetLocalPort(connectionPtr conn);

pascal char ConnNewPassiveBlankListener(connectionPtr c);
pascal OSErr NetGetLocalIP(unsigned long *ip);

pascal OSErr ConnFindAddress(connectionPtr conn, ConstStr255Param host);

pascal OSErr ConnSend(connectionPtr conn, const void* writePtr, short writeCount, char push);
pascal connectionPtr ConnNewDNS(ConstStr255Param name, short type);

#undef CONST
#endif