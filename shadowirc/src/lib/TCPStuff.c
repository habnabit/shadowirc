/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2000 John Bafford <dshadow@shadowirc.com>
	Re-written for MacOS X by:
			Landon Fuller <landonf@fullers.org>
			Chris Campbell <chris_campbell@mac.com>
	Copyright (C) 2002 Landon Fuller, Chris Campbell, John Bafford
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

	Note: This code based on Peter N. Lewis's MacTCP source code.
*/

#pragma mark -
#pragma mark ¥¥¥ÊTCPStuff
#pragma mark -

#define TCPCONTROL
#define __TCPSTUFF__
#define __TCPCONNECTIONS__

#include <machine/endian.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "TCPStuff.h"
#include "connections.h"
#include "TCP.h"

#include "inline.h"

#ifndef _utils_
#include "utils.h"
#endif

#define SA struct sockaddr
typedef void* StreamPtr;
typedef void (*TCPIOCompletionUPP)();
typedef struct TCPiopb {} TCPiopb;
typedef unsigned long ip_addr;

/*
 * TCPConnection defines
 */

enum {
	TCPMAGIC = 'TCPC',
	INCOMINGBUFSIZE = 512,
};

/*
 * Socket states
 */

typedef enum TCPStateType {
	T_Closed, T_Listening, T_Failed, T_Opening, T_Established
} TCPStateType;

/*
 * DNS states
 */
enum {
        inProgress = 1
};

typedef struct TCPiopb TCPControlBlock, *TCPControlBlockPtr;

typedef struct TCPConnection {
	OSType magic;
	Ptr incomingPtr;
	long incomingSize;
	char closedone;
	char inBuf[INCOMINGBUFSIZE];
} TCPConnection;

typedef struct MyControlBlock {
	TCPControlBlock tcp;
	UniversalProcPtr proc;
	TCPConnectionPtr tcpc;
	char inuse;
} MyControlBlock, *MyControlBlockPtr;

static OSErr ValidateConnection(TCPConnectionPtr conn);

static void TCPRawState(TCPConnectionPtr conn, short *state, short *localport);

OSErr TCPReceiveUpTo(connectionPtr conn, char term, long timeout, Ptr readPtr, long readSize, long *readPos, char *gotterm);

typedef struct IPParamBlock IPControlBlock, *IPControlBlockPtr;

/*
inline OSErr ipPBSync(IPControlBlockPtr cb) {return PBControlSync((ParamBlockRec*)cb);}
inline OSErr PBSync(TCPControlBlockPtr cb) {return PBControlSync((ParamBlockRec*)cb);}
inline OSErr PBAsync(TCPControlBlockPtr cb) {return PBControlAsync((ParamBlockRec*)cb);}
*/

static void ZotBlocks(void);
static void tcpDestroyConnection(TCPConnectionPtr *conn);
static OSErr MyPBControlAsync(MyControlBlockPtr *cbp);

static OSErr doTCPRelease(TCPConnectionPtr *connection);

static void TCPFinish(void);

#pragma mark -
#pragma mark ¥¥¥ÊTCPConnections
#pragma mark -


#define maxlongint 0x7FFFFFFF

enum {
    tooManyConnections = -23099,

    max_connections = 32,
    TO_FindAddress = 40 * 60,
    TO_ActiveOpen = 20 * 60,
    TO_ListenOpen = 365 * 24 * 2600 * 60,

    TCPCMagic = 'TCPC',
    TCPCBadMagic = 'badc'
};

enum statusType {
    CS_None, CS_Searching, CS_Opening, CS_Established, CS_Closing
};

/*
 * DNRRecord
 * Contains ioResult to store current state/result
 * pthread_t thread to hold pthread ID of lookup thread
 * name is packed with the hostname we are looking for
 */
typedef struct DNRRecord {
    OSErr ioResult;
    pthread_t thread;
    Str255 name;
    struct in_addr ip;
} DNRRecord, *DNRRecordPtr;

typedef long connectionIndex;

typedef struct tcpConnectionRecord {
    OSType magic;
    long conmagic;
    /*
     * sockfd contains socket file descriptor
     * state contains MacTCP-style socket state
     */
    int sockfd;
    TCPStateType state;
    DNRRecordPtr dnrrp;
    long timeout;
    short status;
    char pad;
    char closedone;
} tcpConnectionRecord;

static tcpConnectionRecord connections[max_connections];
static long connectionmagic;

static void CloseConnections(void);
inline char CanQuit(void);
static OSErr CreateConnection(connectionIndex *cp);
inline void HandleConnection(tcpConnectionRecord *c, connectionEventRecord *cer, fd_set *rset, fd_set *wset);

static void FindAddressDNR(DNRRecordPtr);
static void DoIOCompletion(MyControlBlockPtr cbp);

TCPStateType doTCPActiveOpen(int *sockfd, struct in_addr remotehost, u_short remoteport);
TCPStateType doTCPListenOpen(int *sockfd, u_short localport, int backlog);

tcpConnectionRecord *GetConnection(long cp);

int GetConnectionSocket(long cp);

/*
 * inet_ntoa_str
 * Converts an in_addr struct to presentation format
 * Copies result to a pascal string
 * Replaces hostToIPStr
 */
void inet_ntoa_str(struct in_addr addr, StringPtr string)
{
    char *cstr;
    
    if((cstr = inet_ntoa(addr)) == NULL)
    {
            
            /* XXX do nothing, but this is bad! */
    }
    CopyCStringToPascal(cstr, string);
}

/*
 * ntohl_str
 * Converts a host from network to host byte order
 * Copies result to a pascal string
 * Replaces ulongstr
 */
void ntohl_str(u_int32_t net32, StringPtr string)
{
    char cstr[INET_ADDRSTRLEN];
    u_int32_t host32;
    
    host32 = ntohl(net32);
    sprintf(cstr, "%u", host32);
    CopyCStringToPascal(cstr, string);
}

/*
 * str_htonl
 * Converts a string containing a long from host to network byte order
 * Returns result
 * Replaces ulongval
 */
u_int32_t str_htonl(StringPtr string)
{
    char *cstr, *end_ptr;
    size_t size;
    u_int32_t net32;
    
    cstr = string;
    size = *string;
    cstr = safe_malloc(size);
    
    CopyPascalStringToC(string, cstr);
    net32 = strtoul(cstr, &end_ptr, 0);
    if (errno == ERANGE || *end_ptr != '\0')
        net32 = 0;
        
    CopyCStringToPascal(cstr, string);
    free(cstr);
    return(net32);
}

/*
 * fd_set related functions
 * readfds holds global set of readable file descriptors
 * readfds is initialized in InitConnections()
 * fd_max holds the maximum file descriptor for select()
*/

static fd_set readfds;
static int fd_max;
/*
 * fd_add
 * Add a new file descriptor to the fd tables
 */
static int fd_add (int fd)
{
        if (fd < 0)
            return fd;
            
        if (!FD_ISSET(fd, &readfds))
                FD_SET(fd, &readfds);
        
        if (fd > fd_max)
                fd_max = fd;
        
        return fd;
}

/*
 * fd_remove
 * Remove a file descriptor from the fd tables and close the socket
 */
static int fd_remove (int fd)
{
        if (fd < 0)
            return fd;
        if (FD_ISSET(fd, &readfds))
                FD_CLR(fd, &readfds);
        
        close(fd);
        if (fd == fd_max && fd_max != 0) {
                do
                    fd--;
                while(!FD_ISSET(fd, &readfds) && fd > 0);
                fd_max = fd;
        }
        
        return -1;
}

/*
 * Socket supporting functions from W. Richard Stevens
 */
 
/*
 * set_nblk
 * Set O_NONBLOCK flag on sockfd
 */
static int set_nblk(int sockfd)
{
    int flags;
    /*
     * Set O_NONBLOCK flag
     */
    if((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
        return (-1);
    if((fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) < 0)
        return (-1);
                
}

/*
 * set_blk
 * Remove O_NONBLOCK flag from sockfd
 */
static int set_blk(int sockfd)
{
    int flags;
    if((flags = fcntl(sockfd, F_GETFL, 0)) == -1)
        return (-1);
    /*
     * Remove O_NONBLOCK flag
     */
    if(fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK) < 0)
        return (-1);
}
    
/*
 * readn macro
 * Use MSG_WAITALL flag to cause read() to block until all requested bytes are read
 * Can still return fewer than requested bytes if a signal is caught,
 * the connection is terminated, or an error is pending for the socket
 */
#define readn(fd, ptr, n)	recv(fd, ptr, n, MSG_WAITALL)
 
/*
 * writen
 * Write n number of bytes to fd
 * Retry on EINTR
 */
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nwritten = write(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nwritten = 0;	/* call read() again */
			else
				return (-1);
		} else if (nwritten == 0)
			break;			/* eof */

		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}

static int nblk_accept(int sockfd, struct sockaddr *addr, int *addrlen)
{
        int retval;
        set_nblk(sockfd);
        retval = accept(sockfd, addr, addrlen);
        set_blk(sockfd);
        return retval;
}

#pragma mark -

static void ZotBlocks(void)
{
    /*
	int i;
	
	for(i=0;i<dispose_block_max;i++)
		if(disposeblocks[i])
		{
			DisposePtr(disposeblocks[i]);
			disposeblocks[i] = 0;
		}
     */
}

static OSErr ValidateConnection(TCPConnectionPtr conn)
{ 
/* XXX only used by currently commented out routines. connectionDoesntExist doesn't exist :)
	if(!conn || conn->magic != TCPMAGIC)
		return connectionDoesntExist;
	else
		return noErr;
*/
    return noErr;
}

static OSErr MyPBControlAsync(MyControlBlockPtr *cbp)
{
    /*
	OSErr err = PBAsync((TCPControlBlockPtr)*cbp);
	if(err && *cbp) //Was FreeCB(cbp)
	{
		(*cbp)->inuse = false;
		*cbp = 0;
	}
	return err;
     */
    return noErr;
}

/*
 * TCPReceiveChars
 * Attempt to read len bytes from fd sockfd, writing output to buffer d
 * Return number of bytes read
 */
size_t TCPReceiveChars(int sockfd, Ptr d, size_t len)
{        
        return(readn(sockfd, d, len));
}

/* TCPReceiveUntil
 * Read len bytes from fd sockfd until byte matching char c is reached
 * This function costs at least 2 calls to read(), one to 'PEEK' at the message,
 * and the other to do the actual read. (see recv man page for MSG_PEEK definition)
 * return number of bytes read, or 0 for error
 */
size_t TCPReceiveUntil(int sockfd, Ptr d, char c, size_t len)
{
        char *buffer;
        char *ptr;
        size_t nbytes;
        
        nbytes = 0;
        buffer = safe_malloc(len);
        len = recv(sockfd, buffer, len, MSG_PEEK);
        if(len > 0) {
                ptr = buffer;
                while(ptr != buffer + len)
                {
                        nbytes++;
                        if(*ptr == c) {
                                break;
                        }
                        ptr++;
                }
                if (nbytes == len && *ptr != c)
                {
                        /*
                         * XXX
                         * This can occur under two conditions. The second is the dangerous one
                         * 1) Not enough data is available on the socket, terminating characters are not found
                         * 2) Server/Client doesn't follow spec and sends > max len bytes without terminating character
                         * 	This should be solved by having dynamically allocated LongStrings that can handle
                         *	this condition, or by dropping data when this occurs
                         */
                        nbytes = 0;
                } else
                        nbytes = readn(sockfd, d, nbytes);
         
        }
        free(buffer);
        return (nbytes);
}

OSErr TCPSendAsync(connectionPtr conn, const void* writePtr, short writeCount, char push)
{
    OSErr err;
    int bytes_sent;
    int socket = GetConnectionSocket(conn->private_socket);

    if(writeCount > 0) {
        if (socket != 0) {
            bytes_sent = write(socket, writePtr, writeCount);
            if (bytes_sent == writeCount)
                err = noErr;
            else
                err = C_Closed; // What error code should be returned?
        }
        else
            err = C_Closed; // Invalid socket. What error code should be returned?
		}
		else if(writeCount < 0)
        err = C_Closed; // What error code should be returned?

    return err;
}

static OSErr doTCPRelease(TCPConnectionPtr *conn)
{
    /*
	OSErr err;
	
	err = ValidateConnection(*conn);
	if(!err)
	{
		ZotBlocks();
		err = MTTCPRelease(&(*conn)->stream);
		tcpDestroyConnection(conn);
	}
	return err;
     */
    return noErr;
}

static void TCPRawState(TCPConnectionPtr conn, short *state, short *localport)
{
    /*
	TCPControlBlock cb;
	
	*localport = 0;
	
	if(ValidateConnection(conn))
		*state = 0;
	else
	{
		ZotBlocks();
		ZeroCB(&cb, conn->stream, TCPStatus);
		if(PBSync(&cb))
			*state = 0;
		else
		{
			*state = cb.csParam.status.connectionState;
			*localport = cb.csParam.status.localPort;
		}
	}
     */
}

static size_t TCPCharsAvailable(int sockfd)
{
        char inBuf[INCOMINGBUFSIZE];
        int nbytes;
        nbytes = recv(sockfd, inBuf, INCOMINGBUFSIZE, MSG_PEEK);
        if(nbytes == -1)
            return (0);
        return(nbytes);
}

    /*
 * sockaddr_union
 * utilized by getsockname below in:
 * TCPLocalPort
 * TCPLocalIP
 * TCPRemoteIP
     */

typedef union {
        struct sockaddr sa;
        struct sockaddr_in sin;
        struct sockaddr_in6 sin6;
    } sockaddr_union;

/*
 * TCPLocalPort
 * Return local port for sockfd, else return -1 on failure
 */
int TCPLocalPort(int sockfd)
{
    sockaddr_union sau;
    int len;
    
    len = sizeof(sockaddr_union);
    if(getsockname(sockfd, (SA *) &sau.sa, &len) < 0)
        return (-1);
    return (ntohs(sau.sin.sin_port));
}


int TCPLocalIP(int sockfd, struct in_addr *ip)
{
    sockaddr_union sau;
    int len;
    
    len = sizeof(sockaddr_union);
    if(getsockname(sockfd, (SA *) &sau.sa, &len) < 0)
        return (-1);
    
    memcpy(ip, &sau.sin.sin_addr, sizeof(ip));
    return (0);
}

/*
long TCPRemoteIP(TCPConnectionPtr conn)
{
	short state, localport;
	long remotehost;
	
	TCPRawState(conn, &state, &localport, &remotehost);
	return remotehost;
}
*/

static void TCPFinish(void)
{
    /*
	int i;
	for(i=0;i<control_block_max;i++)
		if(controlblocks[i])
		{
			DisposePtr((Ptr)controlblocks[i]);
			controlblocks[i]=0;
		}
     */
}

static OSErr CreateStream(TCPConnectionPtr *conn, long buffersize)
{
    /*
	OSErr err;
	TCPConnectionPtr x;
	
	*conn = x = (TCPConnectionPtr)NewPtr(sizeof(TCPConnection));
	if(x)
	{
		if(!buffersize)
			buffersize = 6 * 1024;
		else if(buffersize < 4096)
			buffersize = 4096;
		
		if((x->buffer = NewPtr(buffersize)) != 0)
		{
			x->magic = TCPMAGIC;
			x->asends = 0;
			x->asendcompletes = 0;
			x->closedone = false;
			x->incomingSize = 0;
			err = MTTCPCreate(&x->stream, x->buffer, buffersize);
		}
		else
			err = memFullErr;
		
		if(err)
			tcpDestroyConnection(conn);
	}
	else
		err = memFullErr;
	
	return err;
     */
    return noErr;
}

/*
 * doTCPActiveOpen
 * Open an outgoing connection using a non-blocking connect()
 * Socket is set blocking after connect() is called
 */

TCPStateType doTCPActiveOpen(int *sockfd, struct in_addr remotehost, u_short remoteport)
{
        int flags, n;
        struct sockaddr_in servaddr;
        
        if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                return (T_Failed);
                
        if((flags = fcntl(*sockfd, F_GETFL, 0)) == -1)
                return (T_Failed);
        if((fcntl(*sockfd, F_SETFL, flags | O_NONBLOCK)) == -1)
                return (T_Failed);
                
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        bcopy(&remotehost, &servaddr.sin_addr, sizeof(servaddr.sin_addr));
        servaddr.sin_port = htons(remoteport);
        
        fd_add(*sockfd);
        n = connect(*sockfd, (SA *) &servaddr, sizeof(servaddr));
        /*
         * Restore socket flags
         */
        if((fcntl(*sockfd, F_SETFL, flags)) == -1)
                return(T_Failed);
        if (n == -1) {
                if(errno != EINPROGRESS)
                        return (T_Failed);
                else
                        return (T_Opening);
        }
        
        return (T_Established);
}

/*
 * doTCPListenOpen
 * Bind a listen socket with specified backlog
 */
 
TCPStateType doTCPListenOpen(int *sockfd, u_short localport, int backlog)
{
        struct sockaddr_in servaddr;
            
        if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                return (T_Failed);
                
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(localport);
                
        if((bind(*sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0)
                return (T_Failed);
        if (listen(*sockfd, backlog) < 0)
                return (T_Failed);
        fd_add(*sockfd);
        return T_Listening;
}

#pragma mark -

static char ValidConnection(connectionIndex *cp)
{
	long ocp;
	
	ocp = *cp;
	(*cp) %= (max_connections+1);
	if(*cp > 0 && connections[(*cp)-1].magic == TCPCMagic && connections[(*cp)-1].conmagic == ocp)
		return true;
	
	return false;
    return 0;
}

/*
 * GetConnection
 * XXX: Not used anywhere. Delete it after verifying its uselessness
 */
tcpConnectionRecord *GetConnection(long cp)
{
   if (ValidConnection(&cp))
      return &connections[cp-1];
   else
      return 0;
}

/*
 * GetConnectionSocket
 * Returns the TCP socket fd for a tcpConnectionRecord
 */ 
int GetConnectionSocket(long cp)
{
    if(ValidConnection(&cp))
		return connections[cp-1].sockfd;
	else
		return 0;
}

static void DoIOCompletion(MyControlBlockPtr cbp)
{
    /*
	cbp->inuse = false;
	if(cbp->proc)
		CallTCPIOCompletionProc((TCPIOCompletionUPP)cbp->proc, (TCPiopb*)cbp);
     */
}

inline void AddBlock(void* p)
{
    /*
	int i;
	
	for(i=0;i<dispose_block_max;i++)
		if(!disposeblocks[i])
		{
			disposeblocks[i] = (char*)p;
			return;
		}
     */
}

static void TCPSendComplete(MyControlBlockPtr cbp)
{
    /*
	AddBlock(cbp->tcp.csParam.send.wdsPtr);
	if(++cbp->tcpc->asendcompletes == cbp->tcpc->asends && cbp->tcpc->closedone)
		if(!GetCB(&cbp, cbp->tcpc, TCPClose, 0))
			MyPBControlAsync(&cbp);
     */
}

/*
 * InitConnections
 * Set up connections
 * Zero out fd_set readfds, set fd_max to 0
 * Set SIGPIPE handler to SIG_IGN
 * Replaces functionality of old TCPInit
 */
OSErr InitConnections(void)
{
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		connections[i].magic = TCPCBadMagic;
	
	connectionmagic = 0;
	signal(SIGPIPE, SIG_IGN);
        FD_ZERO(&readfds);
	fd_max = 0;
        TCPFinish();
        return noErr;
}

/*
 * CloseConnections
 * Cycle through connectionIndex and close all connections, calling fd_remove for each
 */
static void CloseConnections(void)
{
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		if(connections[i].magic == TCPCMagic)
			if(connections[i].status == CS_Established || connections[i].status == CS_Opening || connections[i].status == CS_Closing)
				if(connections[i].state != T_Closed) {
                                        fd_remove(connections[i].sockfd);
                                        connections[i].magic = TCPCBadMagic;
                                        connections[i].state = T_Closed;
                                }
}

/*
 * CanQuit
 * Return false if any connections remain (connections.magic == TCPCMagic)
 * Otherwise return true
 */
inline char CanQuit(void)
{
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		if(connections[i].magic == TCPCMagic)
			return false;
	
	return true;
}

/*
 * FinishEverything
 * Exit loop called on quit. All connections are closed and CanQuit() is checked
 */
void FinishEverything(void)
{
	connectionEventRecord cer;
	EventRecord e;
	
	CloseConnections();
	while(!CanQuit())
	{
		if(GetConnectionEvent(&cer))
			WaitNextEvent(everyEvent, &e, 0, 0);
		else
			WaitNextEvent(everyEvent, &e, 5, 0);
	}
	
	TCPFinish();
}

/* CreateConnection
 * Initialize a tcpConnectionRecord
 */
static OSErr CreateConnection(connectionIndex *cp)
{
	tcpConnectionRecord *c;
	
	connectionmagic += max_connections + 1;
	*cp = 0;
	while(connections[*cp].magic == TCPCMagic && *cp < max_connections-1)
		(*cp)++;
	
	c = &connections[*cp];
	(*cp)++; //the +1 offset
	if(c->magic == TCPCMagic)
		return tooManyConnections;
	else
	{
		c->magic = TCPCMagic;
		c->conmagic = *cp + connectionmagic;
		c->closedone = false;
		c->sockfd = 0;
		c->status = CS_None;
		c->dnrrp = 0;
		c->timeout = maxlongint;
		*cp += connectionmagic;
		return noErr;
	}
    return noErr;
}

/*
 * DestroyConnection
 * Set connection to "destroyed" by setting connection.magic to TCPCBadMagic
 */
static void DestroyConnection(connectionIndex *cp)
{
	if(ValidConnection(cp))
		connections[(*cp)-1].magic = TCPCBadMagic;
	
	*cp = -1;
}

/*
 * FindAddressDNR
 * Look up the host and/or IP address passed in as drp->name
 * Return the result of gethostbyname() in drp->name
 * Called as a thread, uses mutex locks around non-thread-safe gethostbyname()
 * If timeout is reached by the TCP event loop, pthread_cancel is called on
 * this thread. Thus, pthread_mutex_unlock is pushed onto the cleanup
 * stack
 */
static void FindAddressDNR(DNRRecordPtr drp)
{
        struct hostent *hp;
        char *str;
        static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

        str = safe_malloc(drp->name[0] + 1);
        CopyPascalStringToC(drp->name, str);
        pthread_cleanup_push((void *) pthread_mutex_unlock, (void *) &lock);
        pthread_mutex_lock(&lock);
        hp = gethostbyname(str);
        free(str);
        if(hp == NULL)
        {
            drp->ioResult = -1;
        } else {
            drp->ioResult = noErr;
            bcopy(hp->h_addr_list[0], &drp->ip.s_addr, sizeof(drp->ip.s_addr));
            CopyCStringToPascal(hp->h_name, drp->name);
        }
        pthread_mutex_unlock(&lock);
        /*
         * pop pthread_mutex_unlock off the cleanup stack and execute it
         * this unlocks our lock mutex lock
         */
        pthread_cleanup_pop(1)
}

/*
 * FindAddress
 * Externally visable DNS function
 * Starts FindAddressDNR as a thread
 */
OSErr FindAddress(connectionIndex *cp, ConstStr255Param hostname)
{
	OSErr err;
	connectionIndex cpi;
	
	err = CreateConnection(cp);
	if(!err)
	{
		cpi = *cp;
		if(ValidConnection(&cpi))
		{
			connections[--cpi].dnrrp = (DNRRecordPtr)NewPtr(sizeof(DNRRecord));
			err = MemError();
			if(!err)
			{
                                connections[cpi].dnrrp->ioResult = inProgress;
                                pstrcpy(hostname, connections[cpi].dnrrp->name);
                                pthread_create(&connections[cpi].dnrrp->thread, NULL, (void *) FindAddressDNR, connections[cpi].dnrrp);
				connections[cpi].timeout = TickCount() + TO_FindAddress;
				connections[cpi].status = CS_Searching;
			}
		}
		
		if(err)
			DestroyConnection(cp);
	}
	
	return err;
}

/*
 * NewListenConnection
 * Create a new listen socket, once listening set state to T_Listening
 */
OSErr NewListenConnection(connectionIndex *cp, u_short localport, int backlog)
{
        int err;
	connectionIndex cpi;
	
	err = CreateConnection(cp);
	if(!err)
	{
		cpi = *cp;
		if(ValidConnection(&cpi))
		{
			connections[cpi].state = doTCPListenOpen(&connections[--cpi].sockfd, localport, backlog);
			connections[cpi].timeout = TickCount() + TO_ListenOpen;
			connections[cpi].status = CS_Opening;
                        /*
                         * If doTCPListenOpen failed, deallocate cp
                         * Set state accordingly.
                         */
			if(connections[cpi].state == T_Failed)
                        {
                                err = -1;
				DestroyConnection(cp);
                        }
		}
	}
	
	return err;
}

/*
 * NewActiveConnection
 * Create a new socket and start a non-blocking connect
 * Set state to T_Opening
 */
OSErr NewActiveConnection(connectionIndex *cp, struct in_addr remotehost, u_short remoteport)
{
	OSErr err;
	connectionIndex cpi;
	
	err = CreateConnection(cp);
	if(!err)
	{
		cpi = *cp;
		if(ValidConnection(&cpi))
		{
			connections[cpi].state = doTCPActiveOpen(&connections[--cpi].sockfd, remotehost, remoteport);
			connections[cpi].timeout = TickCount() + TO_ActiveOpen;
			connections[cpi].status = CS_Opening;
                        /*
                         * If doTCPActiveOpen failed, deallocate cp
                         * Set state accordingly.
                         */
			if(connections[cpi].state == T_Failed) {
                                err = -1;
				DestroyConnection(cp);
                        }
		}
	}
	
	return err;
}

void CloseTCPConnection(connectionIndex cp)
{
	if(ValidConnection(&cp))
	{
		tcpConnectionRecord *c = &connections[cp-1];
		
		if(!c->closedone)
		{
			if(c->state != T_Closed) {
                                fd_remove(c->sockfd);
                                c->state = T_Closed;
                        }
			c->closedone = true;
		}
		c->status = CS_Closing;
		c->timeout = 0;
	}
}

inline void HandleConnection(tcpConnectionRecord *c, connectionEventRecord *cer, fd_set *rset, fd_set *wset)
{
	connectionIndex rcp;
	
	cer->connection = rcp = c->conmagic;
	cer->sockfd = c->sockfd;
	cer->timedout = false;
	
	switch(c->status)
	{
		case CS_Searching:
                        if(c->dnrrp->ioResult == noErr)
                        {
                                cer->event = C_Found;
					cer->value = (long)NewString(c->dnrrp->name);
                                cer->addr = c->dnrrp->ip;
                        } else if(c->dnrrp->ioResult != inProgress) {
                                cer->event = C_SearchFailed;
                                cer->value = c->dnrrp->ioResult;
			}
			else if(TickCount() > c->timeout)
			{
                                pthread_cancel(c->dnrrp->thread);
				cer->event = C_SearchFailed;
				cer->value = 1;
				cer->timedout = true;
			}
			if(cer->event != C_NoEvent)
			{
				if(c->dnrrp->ioResult != inProgress)
					DisposePtr((Ptr)c->dnrrp);
				c->dnrrp = 0;
				DestroyConnection(&rcp);
			}
			break;
		
		case CS_Opening:
			switch(c->state)
			{
				case T_Opening:
                                        if(FD_ISSET(c->sockfd, rset) || FD_ISSET(c->sockfd, wset)) {
                                                if (read(c->sockfd, NULL, 0) != 0) {
                                                        CloseTCPConnection(rcp);
                                                        cer->event = C_FailedToOpen;
                                                        break;
                                                }
                                                c->state = T_Established;
                                                c->status = CS_Established;
                                                cer->event = C_Established;
                                                break;
                                        }
                                        if(TickCount() > c->timeout)
					{
						CloseTCPConnection(rcp);
						cer->event = C_FailedToOpen;
						cer->timedout = true;
					}
					break;
				case T_Listening:
                                        /*
                                         * If a connection is ready to be accepted, call non-blocking
                                         * accept(). Replace old connection sockfd with new fd.
                                         * This means that only ONE connection can be made on a listen
                                         * socket
                                         */
                                        if(FD_ISSET(c->sockfd, rset) || FD_ISSET(c->sockfd, wset)) {
                                                struct sockaddr_in clientaddr;
                                                int len, newfd;

                                                len = sizeof(clientaddr);
                                                newfd = nblk_accept(c->sockfd, (SA *) &clientaddr, &len);
                                                fd_remove(c->sockfd);
                                                c->sockfd = newfd;
                                                fd_add(c->sockfd);
                                                c->state = T_Established;
                                                c->status = CS_Established;
                                                cer->event = C_Established;
                                                break;
                                        }
					if(TickCount() > c->timeout)
					{
						CloseTCPConnection(rcp);
						cer->event = C_FailedToOpen;
						cer->timedout = true;
					}
					break;
                                case T_Failed:
                                        CloseTCPConnection(rcp);
                                        cer->event = C_FailedToOpen;
                                        break;
				case T_Established:
					cer->event = C_Established;
					c->status = CS_Established;
					c->timeout = maxlongint;
					break;
				case T_Closed:
					c->status = CS_Closing;
					cer->value = 2;
					cer->event = C_FailedToOpen;
					c->timeout = 0;
					break;
                                default:
                                        /*
                                         * XXX landonf
                                         * Do nothing. Should we handle T_Unknown here?
                                         */
					break;
			}
			break;
			
		case CS_Established:
			switch(c->state)
			{
				case T_Established:
                                        if(FD_ISSET(c->sockfd, rset)) {
                                                cer->value = TCPCharsAvailable(c->sockfd);
					if(cer->value > 0)
						cer->event = C_CharsAvailable;
                                        }
					break;
				case T_Closed:
					c->status = CS_Closing;
					cer->event = C_Closed;
					break;
                                default:
                                        /*
                                         * XXX do what here
                                         */
					break;
			}
			break;
		
		case CS_Closing:
			switch(c->state)
			{
				case T_Opening:
				case T_Listening:
					//DebugStr("\pStrange State 2");
					break;
				case T_Established:
                                        cer->event = C_Closed;
                                        cer->timedout = true;
                                        DestroyConnection(&rcp);
					break;
				case T_Closed:
					cer->event = C_Closed;
					DestroyConnection(&rcp);
					break;
                                default:
                                        /*
                                         * XXX Do what here?
                                         */
                                         break;
			}
			break;
	}
}

char GetConnectionEvent(connectionEventRecord *cer)
{
	static connectionIndex connectionItem = 0;
	connectionIndex oci;
        fd_set rset, wset;
        struct timeval time;
        
        FD_COPY(&readfds, &rset);
        FD_COPY(&readfds, &wset);
        bzero(&time, sizeof(time));
	
	cer->event = C_NoEvent;
	oci = connectionItem;
        
        select(fd_max + 1, &rset, &wset, NULL, &time);
	do {
		if(connections[connectionItem].magic == TCPCMagic)
			HandleConnection(&connections[connectionItem], cer, &rset, &wset);

		if(connectionItem != max_connections -1)
			connectionItem++;
		else
			connectionItem = 0;
	} while(oci != connectionItem && cer->event == C_NoEvent);
	return cer->event != C_NoEvent;
}
