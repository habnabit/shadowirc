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

	Note: This code was originally based on Peter N. Lewis's MacTCP source code.
*/

#pragma mark -
#pragma mark ¥¥¥ÊTCPStuff
#pragma mark -

#define TCPCONTROL
#define __TCPSTUFF__
#define __TCPCONNECTIONS__

#include <machine/endian.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "TCPStuff.h"
#include "TCP.h"

#include "inline.h"

#ifndef _utils_
#include "utils.h"
#endif

#define SA struct sockaddr

#define IDENT_TOOL "ident_tool"

/*
 * TCPConnection defines
 */

enum {
	TCPMAGIC = 'TCPC',
	INCOMINGBUFSIZE = 4096,
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

typedef struct TCPConnection {
	OSType magic;
	Ptr incomingPtr;
	long incomingSize;
	char closedone;
	char inBuf[INCOMINGBUFSIZE];
} TCPConnection;

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
    pthread_mutex_t lock;
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
    char closedone;
} tcpConnectionRecord;

static tcpConnectionRecord connections[max_connections];
static long connectionmagic;

static void CloseConnections(void);
inline char CanQuit(void);
static OSErr CreateConnection(connectionIndex *cp);
inline void HandleConnection(tcpConnectionRecord *c, connectionEventRecord *cer, fd_set *rset, fd_set *wset);

static void FindAddressDNR(DNRRecordPtr);

TCPStateType doTCPActiveOpen(int *sockfd, struct in_addr remotehost, u_short remoteport);
TCPStateType doTCPListenOpen(int *sockfd, u_short localport, int backlog);

int GetConnectionSocket(long cp);

#pragma mark -

/*
 * inet_ntoa_str
 * Converts an in_addr struct to presentation format
 * Copies result to a pascal string
 * Replaces hostToIPStr
 */
void inet_ntoa_str(struct in_addr addr, StringPtr string)
{
    char *cstr;
    
    cstr = inet_ntoa(addr);
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
        
        return (0);
}

/*
 * Socket supporting functions
 * writen() and readn() from Richard Stevens
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
    return (0);
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
    return (0);
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
static ssize_t writen(int fd, const void *vptr, size_t n)
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

ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd) {
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t n;

	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;
	
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ((n = recvmsg(fd, &msg, 0)) < 0)
		return (n);

	if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
			cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
		if (cmptr->cmsg_level != SOL_SOCKET)
			return -1;
		if (cmptr->cmsg_type != SCM_RIGHTS)
			return -1;
		*recvfd = *((int *) CMSG_DATA(cmptr));
	} else
		*recvfd = -1; /* descriptor was not passed */

	return (n);
}

int ident_bind(void) {
	int fd = -1, sockfd[2], status;
	char c, path[MAXPATHLEN];
        CFBundleRef bundle;
        CFURLRef resources, toolURL;
	pid_t pid;
        
        /* Get path to ident_tool */
        bundle = CFBundleGetMainBundle();
        if (!bundle)
            return -1;
        
        resources = CFBundleCopyResourcesDirectoryURL(bundle);
        if (!resources)
            return -1;
            
        toolURL = CFURLCreateCopyAppendingPathComponent(NULL, resources, CFSTR(IDENT_TOOL), FALSE);
        CFRelease(resources);
        if (!toolURL)
            return FALSE;
            
        CFURLGetFileSystemRepresentation(toolURL, TRUE, (UInt8 *) &path, MAXPATHLEN);
        CFRelease(toolURL);

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) != 0)
		return -1;

	switch(pid = fork()) {
		case 0: /* child */
			close(sockfd[0]);
			dup2(sockfd[1], STDOUT_FILENO);
			execl(path, IDENT_TOOL, NULL);
			_exit(1);
		case -1: /* error */
			close(sockfd[0]);
			close(sockfd[1]);
			return -1;
	}
	close(sockfd[1]);
	if (waitpid(pid, &status, 0) != pid) {
		goto exit;
	}
	if (!WIFEXITED(status)) {
		goto exit;
	}
	if ((status = WEXITSTATUS(status)) == 0) {
                read_fd(sockfd[0], &c, 1, &fd);
	} else {
		errno = status;
	}
exit:
	close(sockfd[0]);
	return fd;
}

#pragma mark -

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

OSErr TCPSendChars(int sockfd, const void* d, size_t len)
{
    if((writen(sockfd, d, len) == len))
        return (noErr);
    else
        return (C_Closed);
}

static size_t TCPCharsAvailable(int sockfd)
{
	int nbytes = 0;
	ioctl(sockfd, FIONREAD, &nbytes);
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

int TCPRemoteIP(int sockfd, struct sockaddr *ip)
{
    struct sockaddr_storage ss;
    int len;
    
    len = sizeof(ss);
    if(getpeername(sockfd, (struct sockaddr *) &ss, &len) < 0)
        return (-1);
    
    memcpy(ip, &ss, sizeof(struct sockaddr));
    return (0);
}


/*
 * doTCPActiveOpen
 * Open an outgoing connection using a non-blocking connect()
 * Socket is set blocking after connect() is called
 */

TCPStateType doTCPActiveOpen(int *sockfd, struct in_addr remotehost, u_short remoteport)
{
        int n;
        struct sockaddr_in servaddr;
        
        if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                return (T_Failed);
                
        set_nblk(*sockfd);
                
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        bcopy(&remotehost, &servaddr.sin_addr, sizeof(servaddr.sin_addr));
        servaddr.sin_port = htons(remoteport);
        
        fd_add(*sockfd);
        n = connect(*sockfd, (SA *) &servaddr, sizeof(servaddr));

        if((set_blk(*sockfd)) == -1)
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

#ifndef PORTSTR_LEN
    #define PORTSTR_LEN 8
#endif

TCPStateType doTCPListenOpen(int *sockfd, u_short localport, int backlog)
{
        if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                return (T_Failed);
                
        /*
         * Special case (low) ident port
         */
        if(localport == 113) {
            if((*sockfd = ident_bind()) < 0)
                    return (T_Failed);
        } else {
	    struct addrinfo hints, *res, *res_saved;
	    char portstr[PORTSTR_LEN];
	    
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = PF_UNSPEC;
	    hints.ai_flags = AI_PASSIVE;
	    hints.ai_socktype = SOCK_STREAM;
	    snprintf(portstr, PORTSTR_LEN, "%d", htons(localport));
	    
	    getaddrinfo(NULL, portstr, &hints, &res);
	    
	    res_saved = res;
	    
	    while(res)
	    {
		    if ((bind(*sockfd, (struct sockaddr *)res->ai_addr, res->ai_addrlen)) < 0)
			    if(res->ai_next != NULL)
				    res = res->ai_next;
			    else
			    {
				    freeaddrinfo(res_saved);
				    return (T_Failed);
			    }
		    else
		    {
			    freeaddrinfo(res_saved);
			    break;
		    }
	    }
        }
        
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
		c->timeout = LONG_MAX;
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
static pthread_mutex_t dnslock = PTHREAD_MUTEX_INITIALIZER;
static void FindAddressDNR(DNRRecordPtr drp)
{
        struct hostent *hp;
        char *str;

        str = safe_malloc(drp->name[0] + 1);
        CopyPascalStringToC(drp->name, str);
        pthread_mutex_lock(&dnslock);
	pthread_cleanup_push((void *) pthread_mutex_unlock, (void *) &dnslock);
        hp = gethostbyname(str);
        free(str);
	pthread_mutex_lock(&drp->lock);
	/*
	 * If we have been asked to cancel, we should do it before
	 * we attempt to play with possibly deallocated data structures.
	 *
	 * Otherwise, we hold the lock on the data structures
	 * and a cancel will not be sent until we release the lock.
	 */
	pthread_testcancel();

        if(hp == NULL)
        {
            drp->ioResult = -1;
        } else {
            drp->ioResult = noErr;
            bcopy(hp->h_addr_list[0], &drp->ip.s_addr, sizeof(drp->ip.s_addr));
            CopyCStringToPascal(hp->h_name, drp->name);
        }
	pthread_mutex_unlock(&drp->lock);
        /*
         * pop pthread_mutex_unlock off the cleanup stack and execute it
         * this unlocks our lock mutex lock
         */
	pthread_cleanup_pop(1);
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
			if(pthread_mutex_init(&connections[cpi].dnrrp->lock, NULL) != 0)
				err = -1;
			if(!err)
			{
				pthread_mutex_lock(&connections[cpi].dnrrp->lock);
				connections[cpi].dnrrp->ioResult = inProgress;
				pstrcpy(hostname, connections[cpi].dnrrp->name);
				pthread_mutex_unlock(&connections[cpi].dnrrp->lock);
				connections[cpi].timeout = TickCount() + TO_FindAddress;
				connections[cpi].status = CS_Searching;
				
				if(pthread_create(&connections[cpi].dnrrp->thread, NULL, (void *) FindAddressDNR, (void *) connections[cpi].dnrrp) != 0)
					err = -1;
				
				pthread_detach(connections[cpi].dnrrp->thread);
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
			cpi--;
			connections[cpi].state = doTCPListenOpen(&connections[cpi].sockfd, localport, backlog);
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
			cpi--;
			connections[cpi].state = doTCPActiveOpen(&connections[cpi].sockfd, remotehost, remoteport);
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
			/*
			 * Lookup thread will lock the DNRRecord, set dnrrp->ioResult,
			 * dnrrp->name, and dnrrp->ip, and then unlock the DNRRecord.
			 * Until dnrrp->ioResult is set by the applicable thread,
			 * it is set to inProgress.
			 */
			pthread_mutex_lock(&c->dnrrp->lock);
			if(c->dnrrp->ioResult == noErr)
			{
				cer->event = C_Found;
				cer->value = (long)NewString(c->dnrrp->name);
				cer->addr = c->dnrrp->ip;
			} else if(c->dnrrp->ioResult != inProgress) {
				/*
				 * If dnrrp->ioResult is != inProgress and we hold the mutex,
				 * then the thread will have already completed, and we do not need
				 * to call pthread_cancel
				 */
				cer->event = C_SearchFailed;
				cer->value = c->dnrrp->ioResult;
			}
			else if(TickCount() > c->timeout)
			{
				/*
				 * Cancel thread - precludes possibilty of a race condition
				 * in which we destroy the DNRRecord right before
				 * the thread uses the data structures
				 * To maintain integrity, we MUST hold the mutex lock before
				 * we cancel the DNS thread
				 */
                                pthread_cancel(c->dnrrp->thread);
				cer->event = C_SearchFailed;
				cer->value = 1;
				cer->timedout = true;
			}
			pthread_mutex_unlock(&c->dnrrp->lock);
			/*
			 * If we earlier set the event to indicate an error,
			 * we now clean up the associated data structures
			 * The thread MUST be canceled before this point, or
			 * we may free() its DNRRecord before it attempts to use it
			 */
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
					c->timeout = LONG_MAX;
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
                                                if(cer->value > 0) {
                                                    cer->event = C_CharsAvailable;
                                                } else {
                                                    c->status = CS_Closing;
                                                    cer->event = C_Closed;
                                                    CloseTCPConnection(rcp);
                                                }
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
