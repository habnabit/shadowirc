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

	Note: This code based on Peter N. Lewis's MacTCP source code.
*/

#pragma mark -
#pragma mark ¥¥¥ÊTCPStuff
#pragma mark -

#define TCPCONTROL
#define __TCPSTUFF__

#include "TCPStuff.h"
#include "MyMemory.h"

#include "inline.h"
#include "myDNR.h"

#pragma dont_inline off
#pragma internal on

enum {
	TCPMAGIC = 'TCPC',
	
	INCOMINGBUFSIZE = 512,
	
	dispose_block_max = 100,
	control_block_max = 256
};

typedef enum TCPStateType {
	T_WaitingForOpen, T_Closed, T_Listening, T_Opening, T_Established,
	T_Closing, T_PleaseClose, T_Unknown
} TCPStateType;

typedef struct TCPiopb TCPControlBlock, *TCPControlBlockPtr;

typedef struct TCPConnection {
	OSType magic;
	StreamPtr stream;
	long asends, asendcompletes;
	Ptr incomingPtr;
	long incomingSize;
	Ptr buffer;
	char closedone;
	char PAD[3];
	char inBuf[INCOMINGBUFSIZE];
} TCPConnection;

typedef struct MyControlBlock {
	TCPControlBlock tcp;
	UniversalProcPtr proc;
	TCPConnectionPtr tcpc;
	char inuse;
} MyControlBlock, *MyControlBlockPtr;


static Ptr disposeblocks[dispose_block_max];
static MyControlBlockPtr controlblocks[control_block_max];
static TCPIOCompletionUPP gDoIOCompletionProc;
static UniversalProcPtr gTCPSendCompleteProc;

static short mactcp_driver_refnum;

static pascal OSErr ValidateConnection(TCPConnectionPtr conn);

static pascal void TCPRawState(TCPConnectionPtr conn, short *state, short *localport);

static pascal OSErr TCPRawReceiveChars(TCPConnectionPtr conn, Ptr returnPtr, short readCount);
pascal OSErr TCPReceiveChars(TCPConnectionPtr conn, void* returnPtr, short readCount);
pascal OSErr TCPReceiveUpTo(TCPConnectionPtr conn, char term, long timeout, Ptr readPtr, long readSize, long *readPos, char *gotterm);

typedef struct IPParamBlock IPControlBlock, *IPControlBlockPtr;

inline OSErr ipPBSync(IPControlBlockPtr cb) {return PBControlSync((ParamBlockRec*)cb);}
inline OSErr PBSync(TCPControlBlockPtr cb) {return PBControlSync((ParamBlockRec*)cb);}
inline OSErr PBAsync(TCPControlBlockPtr cb) {return PBControlAsync((ParamBlockRec*)cb);}

static pascal OSErr GetCB(MyControlBlockPtr *cbp, TCPConnectionPtr tcpc, short call, UniversalProcPtr proc);
static pascal void ZotBlocks(void);
static pascal OSErr MTTCPRelease(StreamPtr *stream);
static pascal void ZeroCB(TCPiopb *cb, StreamPtr stream, short call);
static pascal void tcpDestroyConnection(TCPConnectionPtr *conn);
static pascal OSErr MyPBControlAsync(MyControlBlockPtr *cbp);

static pascal OSErr doTCPRelease(TCPConnectionPtr *connection);
static pascal OSErr doTCPAbort(TCPConnectionPtr connections);
static pascal OSErr doTCPClose(TCPConnectionPtr connections);

static pascal TCPStateType TCPState(TCPConnectionPtr connection);
static pascal void TCPFinish(void);


inline void IPZeroCB(IPControlBlock *cb, short call)
{
	MZero(cb, sizeof(*cb));
	cb->ioCRefNum = mactcp_driver_refnum;
	cb->csCode = call;
}

pascal OSErr IPGetMyIPAddr(ip_addr *myIP)
{
	IPControlBlock cb;
	OSErr err;
	
	IPZeroCB(&cb, ipctlGetAddr);
	err = ipPBSync(&cb);
	*myIP = cb.csParam.IPEchoPB.dest;
	return err;
}

static pascal void ZeroCB(TCPiopb *cb, StreamPtr stream, short call)
{
	MZero(cb, sizeof(*cb));
	cb->tcpStream = stream;
	cb->ioCRefNum = mactcp_driver_refnum;
	cb->csCode = call;
}

static pascal OSErr MTTCPRelease(StreamPtr *stream)
{
	TCPControlBlock cb;
	
	ZeroCB(&cb, *stream, TCPRelease);
	*stream = 0;
	return PBSync(&cb);
}

#pragma mark -

static pascal void ZotBlocks(void)
{
	int i;
	
	for(i=0;i<dispose_block_max;i++)
		if(disposeblocks[i])
		{
			DisposePtr(disposeblocks[i]);
			disposeblocks[i] = 0;
		}
}

static pascal OSErr GetCB(MyControlBlockPtr *cbp, TCPConnectionPtr tcpc, short call, UniversalProcPtr proc)
{	//NOTE: Must not move memory if there is a free block available (ie, during a Completion call)
	OSErr err;
	int i;
	MyControlBlockPtr x;
	
	i = 0;
	while(i < control_block_max-1 && controlblocks[i] && controlblocks[i]->inuse)
		i++;
	
	*cbp = controlblocks[i];
	err = noErr;
	if(!*cbp)
	{
		*cbp = (MyControlBlockPtr)NewPtr(sizeof(MyControlBlock));
		err = MemError();
		if(!err)
		{
			(*cbp)->inuse = false;
			controlblocks[i] = *cbp;
		}
	}
	x = *cbp;
	if(!err && !x->inuse)
	{
		ZeroCB(&x->tcp, tcpc->stream, call);
		x->tcp.ioCompletion = gDoIOCompletionProc;
		x->inuse = true;
		x->tcpc = tcpc;
		x->proc = proc;
		err = noErr;
	}
	else
	{
		*cbp = 0;
		err = memFullErr;
	}
	
	return err;
}

static pascal void tcpDestroyConnection(TCPConnectionPtr *conn)
{
	if(*conn)
	{
		TCPConnectionPtr c = *conn;
		c->magic = '????';
		if(c->buffer)
		{
			DisposePtr(c->buffer);
			c->buffer = 0;
		}
		
		DisposePtr((Ptr)c);
		*conn = 0;
	}
}

static pascal OSErr ValidateConnection(TCPConnectionPtr conn)
{
	if(!conn || conn->magic != TCPMAGIC)
		return connectionDoesntExist;
	else
		return noErr;
}

static pascal OSErr MyPBControlAsync(MyControlBlockPtr *cbp)
{
	OSErr err = PBAsync((TCPControlBlockPtr)*cbp);
	if(err && *cbp) //Was FreeCB(cbp)
	{
		(*cbp)->inuse = false;
		*cbp = 0;
	}
	return err;
}

static pascal OSErr TCPRawReceiveChars(TCPConnectionPtr conn, Ptr returnPtr, short readCount)
{
	TCPControlBlock cb;
	OSErr err = 0;
	
	while(!err && readCount>0)
	{
		ZotBlocks();
		ZeroCB(&cb, conn->stream, TCPRcv);
		cb.csParam.receive.rcvBuff = returnPtr;
		cb.csParam.receive.rcvBuffLen = readCount;
		err = PBSync(&cb);
		returnPtr+= cb.csParam.receive.rcvBuffLen;
		readCount-=cb.csParam.receive.rcvBuffLen;
	}
	
	return err;
}

pascal OSErr TCPReceiveChars(TCPConnectionPtr conn, void* returnPtr, short readCount)
{
	long l;
	OSErr err;
	
	err = ValidateConnection(conn);
	if(!err)
	{
		if(readCount < 0)
			err = invalidLength;
		else if(readCount > 0)
		{
			if(conn->incomingSize > 0)
			{
				if(readCount < conn->incomingSize)
					l = readCount;
				else
					l = conn->incomingSize;
				
				BlockMoveData(conn->incomingPtr, returnPtr, l);
				conn->incomingPtr += l;
				conn->incomingSize -= l;
				readCount -= l;
				(char*)returnPtr += l;
			}
			
			if(readCount > 0)
				err = TCPRawReceiveChars(conn, (char*)returnPtr, readCount);
		}
	}
	
	return err;
}

pascal OSErr TCPReceiveUpTo(TCPConnectionPtr conn, char term, long timeout, Ptr readPtr, long readSize, long *readPos, char *gotterm)
{
	OSErr err;
	long waitUntil;
	long readIn;
	
	err = ValidateConnection(conn);
	*gotterm = false;

	while(!err && *readPos < readSize && !*gotterm)
	{
		//Copy from the buffer into the output
		if(conn->incomingSize > 0)
		{
			*gotterm = (term == (readPtr[(*readPos)++] = *conn->incomingPtr));
			(conn->incomingPtr)++;
			conn->incomingSize--;
			continue;
		}
		
		//Fill the buffer
		if(conn->incomingSize == 0)
		{
			if(timeout == 0 && TCPCharsAvailable(conn) == 0)
				err = commandTimeout;
			else
			{
				waitUntil = TickCount() + timeout;
				while(!err)// && conn->incomingSize == 0)
				{
					readIn = TCPCharsAvailable(conn);
					if(readIn > 0)
					{
						if(readIn > INCOMINGBUFSIZE)
							readIn = INCOMINGBUFSIZE;
						
						err = TCPRawReceiveChars(conn, conn->inBuf, readIn);
						if(!err)
						{
							conn->incomingSize = readIn;
							conn->incomingPtr = conn->inBuf;
							break;
						}
					}
					else if(TickCount() > waitUntil)
						err = commandTimeout;
				}
			}
		}
	}

	if(err == commandTimeout)
		err = 0;
	
	return err;
}

pascal OSErr TCPSendAsync(TCPConnectionPtr conn, const void* writePtr, short writeCount, char push)
{
	typedef struct myBlock {
		wdsEntry wds;
		short pad;
		char data[1];
	} myBlock, *myBlockPtr;
	
	OSErr err;
	MyControlBlockPtr cb;
	myBlockPtr p;
	
	err = ValidateConnection(conn);
	if(!err)
	{
		if(writeCount > 0)
		{
			p = (myBlockPtr)NewPtr(sizeof(wdsEntry) + sizeof(short) + writeCount);
			if(p)
			{
				p->wds.ptr = p->data;
				p->wds.length = writeCount;
				p->pad = err; //MUST be set to zero, or no sends. (WHY?!) Shortcut - set to err, since err = 0, and we don't have to load a register.
				BlockMoveData(writePtr, p->wds.ptr, writeCount);
				err = GetCB(&cb, conn, TCPSend, gTCPSendCompleteProc);
				if(!err)
				{
					cb->tcp.csParam.send.wdsPtr = (Ptr)p;
					cb->tcp.csParam.send.pushFlag = push;
					conn->asends++;
					err = MyPBControlAsync(&cb);
				}
				if(err)
					DisposePtr((Ptr)p);
			}
			else
				err = memFullErr;
		}
		else if(writeCount < 0)
			return invalidLength;
	}
	
	return err;
}

static pascal OSErr doTCPClose(TCPConnectionPtr conn)
{
	OSErr err;
	MyControlBlockPtr cbp;
	
	err = ValidateConnection(conn);
	if(!err)
	{
		if(!conn->closedone)
		{
			conn->closedone = true;
			if(conn->asends == conn->asendcompletes)
			{
				ZotBlocks();
				err = GetCB(&cbp, conn, TCPClose, 0);
				if(!err)
					err = MyPBControlAsync(&cbp);
			}
		}
	}

	return err;
}

static pascal OSErr doTCPAbort(TCPConnectionPtr conn)
{
	OSErr err;
	TCPiopb cb;
	
	err = ValidateConnection(conn);
	if(!err)
	{
		ZotBlocks();
		ZeroCB(&cb, conn->stream, TCPAbort);
		err = PBSync(&cb);
	}
	return err;
}

static pascal OSErr doTCPRelease(TCPConnectionPtr *conn)
{
	OSErr err;
	
	err = ValidateConnection(*conn);
	if(!err)
	{
		ZotBlocks();
		err = MTTCPRelease(&(*conn)->stream);
		tcpDestroyConnection(conn);
	}
	return err;
}

static pascal void TCPRawState(TCPConnectionPtr conn, short *state, short *localport)
{
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
}

static pascal TCPStateType TCPState(TCPConnectionPtr conn)
{
	short state, localport;
	
	if(!conn)
		return T_Closed;
	
	TCPRawState(conn, &state, &localport);
	switch(state)
	{
		case 0:
			return T_Closed;
		case 2:
			return T_Listening;
		case 4:
		case 6:
			return T_Opening;
		case 8:
			return T_Established;
		case 10:
		case 12:
		case 16:
		case 18:
		case 20:
			return T_Closing;
		case 14:
			return T_PleaseClose;
		default:
			return T_Unknown;
	}
}

pascal long TCPCharsAvailable(TCPConnectionPtr conn)
{
	TCPControlBlock cb;
	
	if(ValidateConnection(conn))
		return 0;
	else
	{
		ZeroCB(&cb, conn->stream, TCPStatus);
		if(PBSync(&cb))
			return 0;
		else
			return cb.csParam.status.amtUnreadData + conn->incomingSize;
	}
}

pascal short TCPLocalPort(TCPConnectionPtr conn)
{
	short state, localport;
	
	TCPRawState(conn, &state, &localport);
	return localport;
}
/*
pascal long TCPRemoteIP(TCPConnectionPtr conn)
{
	short state, localport;
	long remotehost;
	
	TCPRawState(conn, &state, &localport, &remotehost);
	return remotehost;
}
*/

static pascal void TCPFinish(void)
{
	int i;
	for(i=0;i<control_block_max;i++)
		if(controlblocks[i])
		{
			DisposePtr((Ptr)controlblocks[i]);
			controlblocks[i]=0;
		}
}

#pragma mark -
#pragma mark ¥¥¥ÊTCPConnections
#pragma mark -

#define __TCPCONNECTIONS__

#include "TCP.h"
#include "utils.h"

#pragma internal on

#define maxlongint 0x7FFFFFFF

enum {
	tooManyConnections = -23099,

	max_connections = 32,
	TO_FindAddress = 40 * 60,
	TO_FindName = 40 * 60,
	TO_ActiveOpen = 20 * 60,
	TO_Closing = 2 * 60 * 60,
	TO_PassiveOpen = 365 * 24 * 2600 * 60,
	
	TCPCMagic = 'TCPC',
	TCPCBadMagic = 'badc'
};

enum statusType {
	CS_None, CS_Searching, CS_NameSearching, CS_Opening, CS_Established, CS_Closing
};

typedef struct DNRRecord {
	OSErr ioResult;
	Str255 name;
	long addr;
	hostInfo hi;
} DNRRecord, *DNRRecordPtr;

typedef long connectionIndex;

typedef struct tcpConnectionRecord {
	OSType magic;
	long conmagic;
	TCPConnectionPtr tcpc;
	DNRRecordPtr dnrrp;
	long timeout;
	short status;
	char pad;
	char closedone;
} tcpConnectionRecord;

static ResultUPP gDNRNameToAddrCompletionProc;
static ResultUPP gDNRAddrToNameCompletionProc;

static tcpConnectionRecord connections[max_connections];
static long connectionmagic;

static pascal OSErr PAOpen(TCPConnectionPtr *connection, short cs, long bufferSize, short *localPort, long remoteIP, short remotePort);

static pascal void TerminateConnections(void);
static pascal void CloseConnections(void);
inline pascal char CanQuit(void);
static pascal OSErr CreateConnection(connectionIndex *cp);
static pascal void DestroyConnection(connectionIndex *cp);
inline pascal void HandleConnection(tcpConnectionRecord *c, connectionEventRecord *cer);

static pascal void DNRNameToAddrCompletion(hostInfo *hip, DNRRecordPtr drp);
static pascal void DNRAddrToNameCompletion(hostInfo* hip, DNRRecordPtr drp);
static pascal void FindAddressDNR(ConstStr255Param, DNRRecordPtr);
static pascal void FindNameDNR(long addr, DNRRecordPtr drp);
static void DoIOCompletion(MyControlBlockPtr cbp);

inline OSErr doTCPActiveOpen(TCPConnectionPtr *conn, long buffersize, short localport, long remoteIP, short remoteport);
inline OSErr doTCPPassiveOpen(TCPConnectionPtr *conn, long buffersize, short localport, long remoteIP, short remoteport);


inline pascal OSErr MTTCPCreate(StreamPtr *stream, Ptr buffer, long bufferSize)
{
	OSErr err;
	TCPControlBlock cb;
	
	ZeroCB(&cb, 0, TCPCreate);
	cb.csParam.create.rcvBuff = buffer;
	cb.csParam.create.rcvBuffLen = bufferSize;
	err = PBSync(&cb);
	if(!err)
		*stream = cb.tcpStream;
	else
		*stream = 0;
	return err;
}

static pascal OSErr CreateStream(TCPConnectionPtr *conn, long buffersize)
{
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
}

static pascal OSErr PAOpen(TCPConnectionPtr *connection, short cs, long bufferSize, short *localPort, long remoteIP, short remotePort)
{
	OSErr err;
	MyControlBlockPtr cbp;
	
	ZotBlocks();
	err = CreateStream(connection, bufferSize);
	if(!err)
	{
		err = GetCB(&cbp, *connection, cs, 0);
		if(!err)
		{
			cbp->tcp.csParam.open.localPort = *localPort;
			cbp->tcp.csParam.open.remoteHost = remoteIP;
			cbp->tcp.csParam.open.remotePort = remotePort;
			cbp->tcp.csParam.open.ulpTimeoutAction = -1;
			err = MyPBControlAsync(&cbp);
			if(!err && cs == TCPPassiveOpen)
			{
				while(cbp->tcp.ioResult >= 0 && cbp->tcp.csParam.open.localPort == 0)
					;
				*localPort = cbp->tcp.csParam.open.localPort;
			}
		}
		if(err)
		{
			MTTCPRelease(&(*connection)->stream);
			tcpDestroyConnection(connection);
		}
	}

	return err;
}

inline OSErr doTCPActiveOpen(TCPConnectionPtr *conn, long buffersize, short localport, long remoteIP, short remoteport)
{
	return PAOpen(conn, TCPActiveOpen, buffersize, &localport, remoteIP, remoteport);
}

inline OSErr doTCPPassiveOpen(TCPConnectionPtr *conn, long buffersize, short localport, long remoteIP, short remoteport)
{
	return PAOpen(conn, TCPPassiveOpen, buffersize, &localport, remoteIP, remoteport);
}

#pragma mark -

static pascal char ValidConnection(connectionIndex *cp)
{
	long ocp;
	
	ocp = *cp;
	(*cp) %= (max_connections+1);
	if(*cp > 0 && connections[(*cp)-1].magic == TCPCMagic && connections[(*cp)-1].conmagic == ocp)
		return true;
	
	return false;
}

pascal TCPConnectionPtr GetConnectionTCPC(long cp)
{
	if(ValidConnection(&cp))
		return connections[cp-1].tcpc;
	else
		return 0;
}

static pascal void DNRNameToAddrCompletion(hostInfo *hip, DNRRecordPtr drp)
{
	if(hip->rtnCode == cacheFault)
		hip->rtnCode = 0;
	drp->ioResult = hip->rtnCode;
	drp->addr = drp->hi.addr[0];
}

static pascal void FindAddressDNR(ConstStr255Param name, DNRRecordPtr drp)
{
	OSErr err;
	
	drp->ioResult = 1;
	pstrcpy(name, drp->name);
	err = DNRStrToAddr(name, &drp->hi, (UniversalProcPtr)gDNRNameToAddrCompletionProc, (Ptr)drp);
	if(err != cacheFault)
	{
		drp->hi.rtnCode = err;
		DNRNameToAddrCompletion(&drp->hi, drp);
	}
}

static pascal void strc2p(char *cstr)
{
	if(*cstr)
	{
		char *cstrStart = cstr;
		//strlen
		int len = 0;
		
		while(*cstr++)
			;
		
		len = cstr - cstrStart - 1;
		
		if(len > 255)
			len = 255;
		
		BlockMoveData(cstrStart, cstrStart+1, len);
		*(unsigned char*)cstrStart = len;
	}
}

static pascal void DNRAddrToNameCompletion(hostInfo* hip, DNRRecordPtr drp)
{
	drp->ioResult = hip->rtnCode;
	if(drp->ioResult == noErr)
	{
		BlockMoveData(hip->cname, drp->name, sizeof(drp->name));
		strc2p((char*)drp->name);
		if(drp->name[drp->name[0]] == '.')
			drp->name[0]--;
	}
}

static pascal void FindNameDNR(long addr, DNRRecordPtr drp)
{
	OSErr err;
	
	drp->ioResult = 1;
	drp->addr = addr;
	DNRAddrToStr(addr, drp->name);
	err = DNRAddrToName(addr, &drp->hi, (UniversalProcPtr)gDNRAddrToNameCompletionProc, (Ptr)drp);
	if(err != cacheFault)
	{
		drp->hi.rtnCode = err;
		DNRAddrToNameCompletion(&drp->hi, drp);
	}
}

static void DoIOCompletion(MyControlBlockPtr cbp)
{
	cbp->inuse = false;
	if(cbp->proc)
		CallTCPIOCompletionProc((TCPIOCompletionUPP)cbp->proc, (TCPiopb*)cbp);
}

inline void AddBlock(void* p)
{
	int i;
	
	for(i=0;i<dispose_block_max;i++)
		if(!disposeblocks[i])
		{
			disposeblocks[i] = (char*)p;
			return;
		}
}

static pascal void TCPSendComplete(MyControlBlockPtr cbp)
{
	AddBlock(cbp->tcp.csParam.send.wdsPtr);
	if(++cbp->tcpc->asendcompletes == cbp->tcpc->asends && cbp->tcpc->closedone)
		if(!GetCB(&cbp, cbp->tcpc, TCPClose, 0))
			MyPBControlAsync(&cbp);
}

#define uppPascal04ProcInfo (kPascalStackBased + 0x10 * kNoByteCode + 0x40 * kFourByteCode)

inline OSStatus TCPInit(void)
{
	OSErr err;
	
	gDNRNameToAddrCompletionProc = NewResultProc(DNRNameToAddrCompletion);
	gDNRAddrToNameCompletionProc = NewResultProc(DNRAddrToNameCompletion);
	gDoIOCompletionProc = NewTCPIOCompletionProc(DoIOCompletion);
	gTCPSendCompleteProc = NewRoutineDescriptor((ProcPtr)TCPSendComplete, uppPascal04ProcInfo, GetCurrentISA());
	
	err = OpenDriver("\p.IPP", &mactcp_driver_refnum);
	
	return err;
}

pascal OSErr InitConnections(void)
{
	OSErr err;
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		connections[i].magic = TCPCBadMagic;
	
	connectionmagic = 0;
	
	err = TCPInit();
	if(!err)
	{
		err = DNROpenResolver();
		if(err)
			TCPFinish();
	}
	
	return err;
}

static pascal void TerminateConnections(void)
{
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		if(connections[i].magic == TCPCMagic)
			if(connections[i].status == CS_Established || connections[i].status == CS_Opening || connections[i].status == CS_Closing)
				if(TCPState(connections[i].tcpc) != T_Closed)
					doTCPAbort(connections[i].tcpc);
}

static pascal void CloseConnections(void)
{
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		if(connections[i].magic == TCPCMagic)
			if(connections[i].status == CS_Established || connections[i].status == CS_Opening || connections[i].status == CS_Closing)
				if(TCPState(connections[i].tcpc) != T_Closed)
					doTCPClose(connections[i].tcpc);
}

inline pascal char CanQuit(void)
{
	connectionIndex i;
	
	for(i=0;i<max_connections;i++)
		if(connections[i].magic == TCPCMagic)
			return false;
	
	return true;
}

pascal void FinishEverything(void)
{
	connectionEventRecord cer;
	EventRecord e;
	
	TerminateConnections();
	while(!CanQuit())
	{
		if(GetConnectionEvent(&cer))
			WaitNextEvent(everyEvent, &e, 0, 0);
		else
			WaitNextEvent(everyEvent, &e, 5, 0);
	}
	
	DNRCloseResolver();
	TCPFinish();
}

static pascal OSErr CreateConnection(connectionIndex *cp)
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
		c->tcpc = 0;
		c->status = CS_None;
		c->dnrrp = 0;
		c->timeout = maxlongint;
		*cp += connectionmagic;
		return noErr;
	}
	
}

static pascal void DestroyConnection(connectionIndex *cp)
{
	if(ValidConnection(cp))
		connections[(*cp)-1].magic = TCPCBadMagic;
	
	*cp = -1;
}

pascal OSErr FindAddress(connectionIndex *cp, ConstStr255Param hostname)
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
				FindAddressDNR(hostname, connections[cpi].dnrrp);
				connections[cpi].timeout = TickCount() + TO_FindAddress;
				connections[cpi].status = CS_Searching;
			}
		}
		
		if(err)
			DestroyConnection(cp);
	}
	
	return err;
}

pascal OSErr FindName(connectionIndex *cp, long hostIP)
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
				FindNameDNR(hostIP, connections[cpi].dnrrp);
				connections[cpi].timeout = TickCount() + TO_FindName;
				connections[cpi].status = CS_NameSearching;
			}
		}
		
		if(err)
			DestroyConnection(cp);
	}
	
	return err;
}

pascal OSErr NewPassiveConnection(connectionIndex *cp, long buffersize, short localport, long remotehost, short remoteport)
{
	OSErr err;
	connectionIndex cpi;
	
	err = CreateConnection(cp);
	if(!err)
	{
		cpi = *cp;
		if(ValidConnection(&cpi))
		{
			err = doTCPPassiveOpen(&connections[--cpi].tcpc, buffersize, localport, remotehost, remoteport);
			connections[cpi].timeout = TickCount() + TO_PassiveOpen;
			connections[cpi].status = CS_Opening;
			if(err)
				DestroyConnection(cp);
		}
	}
	
	return err;
}

pascal OSErr NewActiveConnection(connectionIndex *cp, long buffersize, short localport, long remotehost, short remoteport)
{
	OSErr err;
	connectionIndex cpi;
	
	err = CreateConnection(cp);
	if(!err)
	{
		cpi = *cp;
		if(ValidConnection(&cpi))
		{
			err = doTCPActiveOpen(&connections[--cpi].tcpc, buffersize, localport, remotehost, remoteport);
			connections[cpi].timeout = TickCount() + TO_ActiveOpen;
			connections[cpi].status = CS_Opening;
			if(err)
				DestroyConnection(cp);
		}
	}
	
	return err;
}

pascal void CloseTCPConnection(connectionIndex cp)
{
	if(ValidConnection(&cp))
	{
		tcpConnectionRecord *c = &connections[cp-1];
		
		if(!c->closedone)
		{
			if(TCPState(c->tcpc) != T_Closed)
				doTCPClose(c->tcpc);
			c->closedone = true;
		}
		c->status = CS_Closing;
		c->timeout = TickCount() + TO_Closing;
	}
}

pascal void AbortConnection(connectionIndex cp)
{
	if(ValidConnection(&cp))
	{
		tcpConnectionRecord *c = &connections[cp-1];
		
		if(TCPState(c->tcpc) != T_Closed)
			doTCPAbort(c->tcpc);
		
		c->status = CS_Closing;
		c->timeout = TickCount() + TO_Closing;
	}
}

inline pascal void HandleConnection(tcpConnectionRecord *c, connectionEventRecord *cer)
{
	connectionIndex rcp;
	
	cer->connection = rcp = c->conmagic;
	cer->tcpc = c->tcpc;
	cer->timedout = false;
	
	switch(c->status)
	{
		case CS_NameSearching:
			if(c->dnrrp->ioResult != inProgress)
			{
				if(c->dnrrp->ioResult == noErr)
				{
					cer->event = C_NameFound;
					cer->value = (long)NewString(c->dnrrp->name);
				}
				else
				{
					cer->event = C_NameSearchFailed;
					cer->value = c->dnrrp->ioResult;
				}
			}
			else if(TickCount() > c->timeout)
			{
				cer->event = C_NameSearchFailed;
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
		
		case CS_Searching:	
			if(c->dnrrp->ioResult == noErr)
			{
				cer->event = C_Found;
				cer->value = c->dnrrp->addr;
			}
			else if(c->dnrrp->ioResult != inProgress)
			{
				cer->event = C_SearchFailed;
				cer->value = c->dnrrp->ioResult;
			}
			else if(TickCount() > c->timeout)
			{
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
			switch(TCPState(c->tcpc))
			{
				case T_WaitingForOpen:
				case T_Opening:
				case T_Listening:
					if(TickCount() > c->timeout)
					{
						CloseTCPConnection(rcp);
						cer->event = C_FailedToOpen;
						cer->timedout = true;
					}
					break;
				case T_Established:
					cer->event = C_Established;
					c->status = CS_Established;
					c->timeout = maxlongint;
					break;
				case T_PleaseClose:
				case T_Closing:
					CloseTCPConnection(rcp);
					cer->value = 1;
					cer->event = C_FailedToOpen;
					c->timeout = TickCount() + TO_Closing;
					break;
				case T_Closed:
					c->status = CS_Closing;
					cer->value = 2;
					cer->event = C_FailedToOpen;
					c->timeout = TickCount() + TO_Closing;
					break;
			}
			break;
			
		case CS_Established:
			switch(TCPState(c->tcpc))
			{
				case T_Established:
					cer->value = TCPCharsAvailable(c->tcpc);
					if(cer->value > 0)
						cer->event = C_CharsAvailable;
					break;
				case T_PleaseClose:
				case T_Closing:
					cer->value = TCPCharsAvailable(c->tcpc);
					if(cer->value > 0)
						cer->event = C_CharsAvailable;
					else
					{
						//CloseTCPConnection(rcp);
						c->status = CS_Closing;
						cer->event = C_Closing;
						c->timeout = TickCount() + TO_Closing;
					}
					break;
				case T_Closed:
					c->status = CS_Closing;
					cer->event = C_Closing;
					c->timeout = TickCount() + TO_Closing;
					break;
			}
			break;
		
		case CS_Closing:
			switch(TCPState(c->tcpc))
			{
				case T_WaitingForOpen:
				case T_Opening:
				case T_Listening:
					//DebugStr("\pStrange State 2");
					break;
				case T_PleaseClose:
				case T_Closing:
				case T_Established:
					cer->value = TCPCharsAvailable(c->tcpc);
					if(cer->value > 0)
						cer->event = C_CharsAvailable;
					else
					{
						cer->event = C_Closed;
						if(c->tcpc)
						{
							//DebugStr("\pClosing timeout, call Abort;g");
							//doTCPAbort(c->tcpc);
							doTCPRelease(&c->tcpc);
						}
						cer->timedout = true;
						DestroyConnection(&rcp);
					}
					break;
				case T_Closed:
					cer->event = C_Closed;
					if(c->tcpc)
						doTCPRelease(&c->tcpc);
					DestroyConnection(&rcp);
					break;
			}
			break;
	}
}

pascal char GetConnectionEvent(connectionEventRecord *cer)
{
	static connectionIndex connectionItem = 0;
	connectionIndex oci;
	
	cer->event = C_NoEvent;
	oci = connectionItem;
	do {
		if(connections[connectionItem].magic == TCPCMagic)
			HandleConnection(&connections[connectionItem], cer);

		if(connectionItem != max_connections -1)
			connectionItem++;
		else
			connectionItem = 0;
	} while(oci != connectionItem && cer->event == C_NoEvent);
	
	return cer->event != C_NoEvent;
}