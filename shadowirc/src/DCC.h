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

#ifndef _DCC
#define _DCC

#ifndef _MacBinary
typedef Ptr MBIIHeaderPtr;
#endif

enum DCCStrings {
	sNoDCCOpen = 1,
	sListedConnections,
	sCouldntFindDCC,
	sDCCCloseSyntax,
	sDCCChatSyntax,
	sDCCGetSyntax,
	sDCCSendSyntax,
	sNoSuchDCCGETNick,
	sRequestingDCC,
	sOpeningDCC,
	sClosedDCC,
	sFailedToCreate,
	sLostSendErr,
	sLostFileReadErr,
	sLostNoFile,
	sLostAckErr,
	sLostWriteErr,
	sLostRecieveErr,
	sFailedToOpen,
	sCantDCCChannel,
	sSelectSaveLoc,
	sDCCRequest,
	sDCCResumeRequest,
	sDCCResumeFailedEOF,
	sDCCResumeFailedNoDCC,
	sDCCCollision,
	sDCCSENDREVErrNonExistant,
	sRequestingRevDCC,
	sDCCRevRequest,
	sUnsupportedDCC,
	sUnsupportedDCCRev,
	sDCCGetNMMessage,
	sDCCGetOverwriteNMMessage,
	sDCCRequestIgnore
};

enum dccTypes {dccNIL, dccCHAT, dccGET, dccSEND, dccRESUME, dccREVERSE, dccNIL2, dcccLIST, dcccCLOSE, dcccMAXIMUM};
enum dccFlags {closed, waiting, offered, opening, open, waitingRev};
enum dccSendStage {sendNIL, sendHeader, sendDF, sendRF, sendDone};

typedef struct dccTypeRec {
	ConstStringPtr name;
	struct plugsRec* pluginRef;
	short reverse;
	short unused;
} dccTypeRec, dccTypePtr;

typedef struct dccTypeListRec {
	long num;
	dccTypeRec list[1];
} dccTypeListRec, *dccTypeListPtr, **dccTypeListHand;

typedef struct dccStatus {
	Str255 status;
	long progress, progressMax;
} dccStatus, *dccStatusPtr;

typedef struct dccCHATData {
	MWPtr dccWindow;	
	long remoteShadowIRCVersion;
	char remoteIsShadowIRC;
	char sentReplyCookie;
	char closeChatWhenConnDies;
} dccCHATData, *dccCHATDataPtr;

typedef struct dccGETData {
	FSSpec gfile;
	short gfref;
	long gotten;
	long getmax;
	long unused2;
	long padLong;
	MBIIHeaderPtr macB;
	short pad;
	char inRF;
	char pad2;
	long unused;
	long unused3[4];
	long bufSiz;
} dccGETData, *dccGETDataPtr;

typedef struct dccSENDData {
	FSSpec sfile;
	short sfref;
	long sent;
	long sendmax;
	long sendpacketlen;
	long dfSize2;
	MBIIHeaderPtr macB;
	short stage;
	char MBSend;
	char pad;
	long acked;
	long dfSize, rfSize, dfPad, rfPad;
	long bufSiz;
} dccSENDData, *dccSENDDataPtr;

#ifdef _TCP
typedef void (*DCCInputFuncPtr)(connectionPtr conn, CEPtr c);
#else
typedef void* DCCInputFuncPtr;
#endif

typedef struct dccRec {
	short dccType; //dccTypeRec
	short dccFlags; //dccFlagsRec
	DCCInputFuncPtr DCCInputFunc;
	Str255 dccUserName;
	void* refcon;
	void* dccData;
	long timeOpened;
	char failedDCC;
	char reverse;
} dccRec, *dccPtr;

extern char IsDCCName(ConstStringPtr s);

pascal void DCCWindowClose(MWPtr mw);

pascal connectionPtr DCCFind(linkPtr link, short typ, ConstStr255Param from);
pascal void DCCClose(connectionPtr *conn, char silent);
pascal char DCCCreate(linkPtr link, short typ, ConstStr255Param fr, connectionPtr *c);
pascal void DCCRequest(linkPtr link,ConstStr255Param fr, ConstStr255Param uah, Str255 s);
pascal void DCCOpen(connectionPtr *x);
pascal void DCCCommand(linkPtr link, Str255 s);

pascal char DoDCCSend(linkPtr link, ConstStr255Param name, char sendRev);
pascal char DoDCCSendFile(linkPtr link, ConstStr255Param name, const FSSpec *file, char macbinary, char sendRev);

#ifdef _LongStrings
pascal void DCCSSay(connectionPtr *x, LongString *ls);
pascal void DCCSendCookie(connectionPtr *x, long cky, LongString *ls);
pascal void DCCSendCookieReply(connectionPtr *x, long cky, LongString *ls);
#endif

pascal char DCCGetAFile(FSSpec *f, char *macbinary);

pascal void ulongstr(unsigned long l, StringPtr s);
pascal ConstStringPtr dccTypToStr(short d);
pascal ConstStringPtr dccFlagTypToStr(short d);

pascal short DCCTypeAdd(ConstStringPtr type);
pascal short DCCTypeFind(ConstStr255Param s);
pascal void DCCGetStatus(dccPtr d, dccStatus *status);

pascal void DCCConnOpened(connectionPtr *cn);
pascal void DCCFailed(connectionPtr *conn, ConstStr255Param reason);

#ifdef _LongStrings
pascal void DCCSSayQuiet(connectionPtr *x, LongString *ls);
#endif

pascal void DCCTypeInit(void);

OSStatus SetupDCCFolder(FSRef *ref);
#endif