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

#include <Carbon/Carbon.h>

#include "LongStrings.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "MacBinary.h"
#include "TCP.h"
#include "DCC.h"
#include "connections.h"
#include "utils.h"
#include "channels.h"
#include "IRCChannels.h"
#include "CTCP.h"
#include "InputLine.h"
#include "plugins.h"
#include "IRCCommands.h"
#include "Inline.h"
#include "filesMan.h"
#include "Notify.h"
#include "Floaters.h"
#include "IRCInputPrivate.h"
#include "ApplBase.h"
#include "TextManip.h"
#include "WindowList.h"
#include "InetConfig.h"
#include "IRCCFPrefs.h"
#include "StringKeys.h"

pascal long ShadowIRCVersion(StringPtr str);

#define kShadowIRCChatCookie "\pcHaT"

#define kDCCFolderNameKey CFSTR("DCCFolderName")

enum ShadowIRCCookies {
	kShadowIRCChatCookie2 = 0xFDCBFDCB,

	kCookieReply = 'REPL',
	kCookieNoReply = 'REPX'
};

static dccTypeListHand dccTypeList;
static long dccSENDRevCount= 0;

typedef struct dccsendbit {
	char macbinary;
	char stuffDCC;
} dccsendbit, *dccsendbitPtr;

static void str10neg(StringPtr s);

static connectionPtr DCCFindSendPort(unsigned short port);
static void DCCSendReposition(connectionPtr conn, long newpos);

static char DCCPutFile(connectionPtr x, char forceSave);
static void DCCSendFileChunk(connectionPtr *cn);
static void DCCGetLineChat(connectionPtr conn, CEPtr c);
static void DCCGetLineGet(connectionPtr conn, CEPtr c);
static void DCCGetLineSend(connectionPtr conn, CEPtr c);
static void StartDCCGet(connectionPtr x);

static void DCCSendFileNavHookMouseDown(NavCBRecPtr callBackParms, NavCallBackUserData callBackUD);
static void DCCSendFileNavHook(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD);

static void DCCProcessChat(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, StringPtr s, long revNum);
static void DCCProcessGet(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, StringPtr s, long revNum);
static void DCCProcessRequest(linkPtr link,ConstStr255Param fr, ConstStr255Param uah, StringPtr s, short type);

static void DCCNMRequest(connectionPtr x, char overwriteFile);

char IsDCCName(ConstStringPtr s)
{
	if((s[0]!=0)&&(s[1]=='='))
		return true;
	else
		return false;
}

pascal void DCCTypeInit(void)
{
	dccTypeListPtr p;
	int x;
	
	dccTypeList = (dccTypeListHand)NewHandle(2 + (dcccMAXIMUM * sizeof(dccTypeRec)));
	HLock((Handle)dccTypeList);
	p = *dccTypeList;
	
	p->num = dcccMAXIMUM;
	
	for(x=0;x<dcccMAXIMUM;x++)
	{
		p->list[x].pluginRef = 0;
		p->list[x].reverse = -1;
	}
	
	p->list[dccNIL].name="\p¥";
	p->list[dccCHAT].name="\pCHAT";
	p->list[dccCHAT].reverse = dccCHAT;

	p->list[dccGET].name="\pGET";
	p->list[dccGET].reverse = dccSEND;
	p->list[dccSEND].name="\pSEND";
	p->list[dccSEND].reverse = dccGET;

	p->list[dccRESUME].name = "\pRESUME";
	p->list[dccREVERSE].name = "\pREVERSE";
	
	p->list[dccNIL2].name="\p¥";
	p->list[dcccLIST].name="\pLIST";
	p->list[dcccCLOSE].name="\pCLOSE";
	
	HUnlock((Handle)dccTypeList);
}

pascal short DCCTypeFind(ConstStr255Param s)
{
	if(!s[0])
		return dcccLIST;
	else
	{
		Str255 s1;
		dccTypeListPtr p;
		int x;
		short out;
		char found = false;

		HLock((Handle)dccTypeList);
		p = *dccTypeList;
		
		pstrcpyucase(s, s1);
		for(x=1;x<p->num;x++)
			if(pstrcmp(s1, p->list[x].name))
			{
				out = x;
				found = true;
				break;
			}
		
		if(!found)
			out = dccNIL;
		HUnlock((Handle)dccTypeList);
		
		return out;
	}
}

pascal short DCCTypeAdd(ConstStringPtr type)
{
	int x;
	
	if(DCCTypeFind(type) == dccNIL)
	{
		x = (**dccTypeList).num;
		SetHandleSize((Handle)dccTypeList, 2+ (x * sizeof(dccTypeRec)));
		(**dccTypeList).list[x].name = type;
		(**dccTypeList).list[x].pluginRef = sidr.yourInfo;
		
		return (**dccTypeList).num++;
	}
	else
		return -1;
}

pascal void DCCGetStatus(dccPtr d, dccStatus *status)
{
	plugsPtr pn = (**dccTypeList).list[d->dccType].pluginRef;
	LongString ls2;
	Str255 c, ss;
	pDCCStatusData p;
	long ack, time;
	
	if(pn)
	{
		p.dcc = d;
		p.status = (Ptr)status;
		runIndPlugin(pn, pDCCStatusMessage, &p);
	}
	else
	{
		if(d->dccType == dccCHAT)
		{
			status->status[0]=0;
			status->progress=status->progressMax=-1;
		}
		else if(d->dccType == dccGET || d->dccType == dccSEND)
		{
			dccGETDataPtr dd=(dccGETDataPtr)d->dccData;
			
			status->progress = dd->gotten;
			status->progressMax = dd->getmax;
			
			NumToString(dd->gotten, c);
			SAppend1(c, '/');
			NumToString(dd->getmax, ss);
			if(d->dccType == dccGET)
				LSStrCat4(&ls2, dd->gfile.name, "\p (", c, ss);
			else
			{
				Str255 ackedStr;
				NumToString(((dccSENDDataPtr)dd)->acked, ackedStr);
				SAppend1(ackedStr, '/');
				LSStrCat(5, &ls2, dd->gfile.name, "\p (", ackedStr, c, ss);
			}
			if(dd->getmax >0)
			{
				if(d->dccType == dccGET)
					NumToString(((float)dd->gotten / (float)dd->getmax) * 100L, ss);
				else
					NumToString(((float)((dccSENDDataPtr)dd)->acked / (float)dd->getmax) *100L, ss);
				LSConcatLSAndStrAndStr(&ls2,"\p, ", ss, &ls2);
				LSAppend1(ls2, '%');
			}

			if(d->dccType == dccSEND)
				ack = ((dccSENDDataPtr)dd)->acked;
			else
				ack = dd->gotten;
			
			time = now - d->timeOpened;
			if(time < 1)
				time = 1;
			NumToString(ack / time, ss);
			LSAppend1(ls2, ',');
			LSAppend1(ls2, ' ');
			LSConcatLSAndStrAndStr(&ls2, ss, "\p bytes/sec", &ls2);
			LSAppend1(ls2, ')');

			LSCopyString(&ls2, 1, 255, status->status);
		}
	}
}

#pragma mark -

pascal connectionPtr DCCFind(linkPtr link, short typ, ConstStr255Param fr)
{
	connectionPtr p;
	Str255 s1;
	
	pstrcpyucase(fr,s1);
	if(s1[s1[0]]=='=')
		s1[0]--;
	
	if(s1[1]=='=')
		pdelete(s1,1,1);
	
	linkfor(p, fConn)
		if((p->socks.type==connDCC)&&(p->dcc->dccType==typ) && (!link || link == p->link))
			if(pstrcasecmp2(s1,p->dcc->dccUserName))
				return p;

	return 0;
}

static connectionPtr DCCFindSendPort(unsigned short port)
{
	connectionPtr c;

	linkfor(c, fConn)
		if(c->socks.type == connDCC && c->dcc->dccType == dccSEND && c->port == port)
			return c;
	
	return 0;
}


#pragma mark -

static void DCCNMRequest(connectionPtr x, char overwriteFile)
{
	LongString ls;
	ConstStringPtr s;

	if(!overwriteFile)
		s = GetIntStringPtr(spDCC, sDCCGetNMMessage);
	else
		s = GetIntStringPtr(spDCC, sDCCGetOverwriteNMMessage);
	LSParamString(&ls, s, x->dcc->dccUserName, ((dccGETDataPtr)(x->dcc->dccData))->gfile.name, 0, 0);
	LSMakeStr(ls);
	Notify(true, (Handle)-1, (Handle)-1, ls.data);
}

#pragma mark -

pascal void DCCFailed(connectionPtr *cn, ConstStr255Param reason)
{
	LongString ls;
	connectionPtr conn = *cn;
	
	if(conn)
	{
		if(reason)
		{
			LSStrCat(5, &ls, reason, "\p DCC ", dccTypToStr(conn->dcc->dccType), "\p connection to ", conn->dcc->dccUserName);
			SMPrefixLinkColor(conn->link, &ls, dsFrontWin, sicCTCP);
			conn->dcc->failedDCC=true;
		}
		DCCClose(cn, false);
	}
}

pascal char DCCCreate(linkPtr link, short typ, ConstStr255Param fr, connectionPtr *c)
{
	dccPtr d;
	pDCCCreateData p;
	
	if(!link || (((*c=DCCFind(link, typ, fr)) != 0) && typ == dccCHAT) || !link->conn)
		return 0;
	else
	{
		*c=newConnection(connDCC);
		(*c)->link=link;
		ConnGetLocalIP(link->conn, &(*c)->localip);
		d=(*c)->dcc=(dccPtr)NewPtr(sizeof(dccRec));
		pstrcpy(fr, d->dccUserName);
		d->dccType=typ;
		d->failedDCC=0;
		d->refcon = NULL;
		d->dccData = 0;
		d->reverse = 0;
		d->dccFlags = 0;
		
		if(typ == dccCHAT)
		{
			d->dccData = NewPtrClear(sizeof(dccCHATData));
		}
		else if((typ==dccSEND) || (typ==dccGET))
		{
			dccGETData *dd = (dccGETData*)NewPtrClear(sizeof(dccGETData));
			dd->bufSiz = mainPrefs->dccSendPacketSize;
			d->dccData = dd;
		}
		
		switch(typ)
		{
			case dccCHAT:
				d->DCCInputFunc = DCCGetLineChat;
				break;
			case dccGET:
				d->DCCInputFunc = DCCGetLineGet;
				break;
			case dccSEND:
				d->DCCInputFunc = DCCGetLineSend;
				break;
			
			default:
				d->DCCInputFunc = 0;
		}
		
		p.connection = *c;
		runPlugins(pDCCCreateMessage, &p);
		return 1;
	}
}

static void DCCSendFileNavHookMouseDown(NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
{
	ControlHandle	whichControl;				
	Point 			where = callBackParms->eventData.eventDataParms.event->where;	
	short			theItem;	
	UInt16 			firstItem = 0;
	short			partCode;
	DialogPtr d = GetDialogFromWindow(callBackParms->window);
	
	GlobalToLocal(&where);
	theItem = FindDialogItem(d,where);	// get the item number of the control
	partCode = FindControl(where, callBackParms->window, &whichControl);	// get the control itself
	
	// ask NavServices for the first custom control's ID:
	if (callBackParms->context != 0)	// always check to see if the context is correct
	{
		OSErr theErr;
		short realItem;
		
		theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);	
		realItem = theItem - firstItem + 1;		// map it to our DITL constants:	

				
		if (realItem == 1)
		{
			setCheckBox(d, firstItem + 1, 1);
			setCheckBox(d, firstItem + 2, 0);
			*(char*)callBackUD = 0;
		}
		else if(realItem == 2)
		{
			setCheckBox(d, firstItem + 1, 0);
			setCheckBox(d, firstItem + 2, 1);
			*(char*)callBackUD = 1;
		}
	}
}

static void DCCSendFileNavHook(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
{
	static short gLastTryWidth;
	static short gLastTryHeight;
	static Handle gDitlList;
	
	OSErr theErr = noErr;
	
	switch (callBackSelector)
	{
		case kNavCBEvent:
			switch (callBackParms->eventData.eventDataParms.event->what)
			{
				case mouseDown:
					DCCSendFileNavHookMouseDown(callBackParms, callBackUD);
					break;
			}
			break;

		case kNavCBCustomize:
		{
			// here are the desired dimensions for our custom area:
			short neededWidth = callBackParms->customRect.left + 230;
			short neededHeight = callBackParms->customRect.top + 15;
			
			// check to see if this is the first round of negotiations:
			if ((callBackParms->customRect.right == 0) && (callBackParms->customRect.bottom == 0))
			{
				// it is, so tell NavServices what dimensions we want:
				callBackParms->customRect.right = neededWidth;
				callBackParms->customRect.bottom = neededHeight;
			}
			else
			{
				// we are in the middle of negotiating:
				if (gLastTryWidth != callBackParms->customRect.right)
					if (callBackParms->customRect.right < neededWidth)	// is NavServices width too small for us?
						callBackParms->customRect.right = neededWidth;

				if (gLastTryHeight != callBackParms->customRect.bottom)	// is NavServices height too small for us?
					if (callBackParms->customRect.bottom < neededHeight)
						callBackParms->customRect.bottom = neededHeight;
			}
			
			// remember our last size so the next time we can re-negotiate:
			gLastTryWidth = callBackParms->customRect.right;
			gLastTryHeight = callBackParms->customRect.bottom;	
			break;
		}
			
		case kNavCBStart:
		{
			UInt16 	firstItem = 0;	
			short	realItem = 0;
			UInt16 itemToSelect = kNavAllReadableFiles;
			
			// add the rest of the custom controls via the DITL resource list:
			gDitlList = GetResource('DITL', 601);
			if ((gDitlList != NULL)&&(ResError() == noErr))
				if ((theErr = NavCustomControl(callBackParms->context,kNavCtlAddControlList,gDitlList)) == noErr)
				{
					DialogPtr d;
					theErr = NavCustomControl(callBackParms->context,kNavCtlGetFirstControlID,&firstItem);	// ask NavServices for our first control ID
					
					d = GetDialogFromWindow(callBackParms->window);
					// set the command popup selection:
					realItem = firstItem + 1;
					setCheckBox(d, realItem, !*(char*)callBackUD);
					setCheckBox(d, realItem + 1, *(char*)callBackUD);
				}

			// the dialog is starting up, let's override the default popup menu selection:
			theErr = NavCustomControl(callBackParms->context,kNavCtlSelectAllType,&itemToSelect);
			break;
		}
			
		case kNavCBTerminate:	
			// release our appended popup menu:
			if (gDitlList)
				ReleaseResource(gDitlList);		
			break;
		}
}

pascal char DCCGetAFile(FSSpec *f, char *macbinary)
{
	char b = 0;
	
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	OSErr theErr;
    NavEventUPP eventUPP = NewNavEventUPP(DCCSendFileNavHook);
		
	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavGetFile;
	
	pstrcpy("\pShadowIRC", dialogOptions.clientName);
	GetIntString(dialogOptions.actionButtonLabel, spFile, sSend);
	
	dialogOptions.dialogOptionFlags-=kNavAllowMultipleFiles;
	
	theErr = NavGetFile(0, &theReply, &dialogOptions, eventUPP, 0, 0, 0, macbinary);
	
	DisposeNavEventUPP(eventUPP);
	
	if (theReply.validRecord && theErr == noErr)
	{
		long count = 0;
		AEKeyword key;
		long index;
		
		// since we allow for multiple objects to be returned,
		// grab the target FSSpecs from 'theReply.fileRef' list for opening:	
		AEDesc 	resultDesc;

		// we are ready to open the document(s), grab information about each file for opening:
		theErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++)
		{
			if ((theErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, &key ,&resultDesc)) == noErr)
			{
				AEGetDescData(&resultDesc, f, sizeof(FSSpec));

				b = 1;
				theErr = AEDisposeDesc(&resultDesc);
			}
		}
		
		theErr = NavDisposeReply(&theReply);	// clean up after ourselves	
	}
	return b;
}

static char DCCPutFile(connectionPtr x, char forceSave)
{
	dccGETDataPtr 	dd=(dccGETDataPtr)x->dcc->dccData;
	ConstStringPtr s;
	
	if(inBackground)
	{
		DCCNMRequest(x, forceSave);
		return 0;
	}
	
	s = GetIntStringPtr(spDCC, sSelectSaveLoc);
	return MyStandardPutFile(s, dd->gfile.name, 'BINA', 'SIRC', kNNoTypePopup, &dd->gfile, true) <= 0;
}

static void StartDCCGet(connectionPtr x)
{
	if(x->dcc->dccFlags == opening || x->dcc->dccFlags == open) //already open!!
		return;
	
	if(mainPrefs->autoSaveDCC) //go ahead
		DCCOpen(&x);
	else //ask where to save
	{
		if(DCCPutFile(x, false))
			DCCOpen(&x);
	}
}

pascal char DoDCCSendFile(linkPtr link, ConstStr255Param name, const FSSpec *file, char macbinary, char sendRev)
{
	connectionPtr x;
	dccPtr d;
	CInfoPBRec pb;
	long dlen, rlen;
	short fileRefNum;
	dccSENDDataPtr dd;
	
	if(!FSpOpenDF(file, fsRdPerm, &fileRefNum))
	{
		FileAdd(fileRefNum, false);
		SetFPos(fileRefNum, fsFromStart, 0);
	}
	else
		return 0;
	
	if(DCCCreate(link, dccSEND, name, &x))
	{
		d=x->dcc;
		dd = (dccSENDDataPtr)d->dccData;
		if(sendRev)
		{
			d->dccFlags = waitingRev;
			d->reverse = true;
		}
		
		dd->sfile=*file;
		dd->sfref=fileRefNum;

		dd->MBSend = macbinary;

		d->dccType = dccSEND;
		
		dd->sendpacketlen=dd->bufSiz;
		if(!macbinary)
		{
			GetEOF(dd->sfref, &dd->sendmax);
			dd->dfSize = dd->sendmax;
			dd->rfSize = dd->dfPad = dd->rfPad = 0;
		}
		else //is MacBinary
		{
			//total sending length = mb header + DF + pad + RF + pad
			//make the MB header while we're at it.
			pb.hFileInfo.ioCompletion = 0;
			pb.hFileInfo.ioNamePtr = dd->sfile.name;
			pb.hFileInfo.ioVRefNum = dd->sfile.vRefNum;
			pb.hFileInfo.ioDirID = dd->sfile.parID;
			pb.hFileInfo.ioFDirIndex = 0;
			PBGetCatInfoSync(&pb);
			
			dd->macB=(MBIIHeaderPtr)NewPtr(sizeof(MBIIHeader));
			CatInfo2MBHeader(&pb, dd->macB);
			dlen = ((MBIIStartHeader*)(&dd->macB->MBIIStart))->dlen;
			rlen = ((MBIIStartHeader*)(&dd->macB->MBIIStart))->rlen;
			dd->sendmax = sizeof(MBIIHeader) + dlen + rlen;
			if(dlen)
				dd->sendmax+= (128 - dlen%128);
			if(rlen)
				 dd->sendmax+= (128 - rlen%128);
			dd->dfSize = dlen;
			dd->rfSize = rlen;
			if(dlen)
				dd->dfPad = (128 - dlen%128);
			else
				dd->dfPad = 0;
			if(rlen)
				dd->rfPad = (128 - rlen%128);
			else
				dd->rfPad = 0;
		}
		if(dd->sendmax<dd->sendpacketlen)
			dd->sendpacketlen=dd->sendmax;
		dd->stage = sendHeader;
		DCCOpen(&x);
	}
	return 0;
}

pascal char DoDCCSend(linkPtr link, ConstStr255Param name, char sendRev)
{
	char macbinary;
	FSSpec sfile;

	macbinary=1;
	if(DCCGetAFile(&sfile, &macbinary))
		return DoDCCSendFile(link, name, &sfile, macbinary, sendRev);
	else
		return 0;
}

pascal void DCCCommand(linkPtr link, Str255 s)
{
	short t, i;
	Str255 c, ss, ss2, name;
	connectionPtr x;
	LongString ls;
	dccPtr d;
	dccStatus status;
	
	if(!link)
		return;
	
	NextArg(s, c);
	NextArg(s, name);
	t=DCCTypeFind(c);
	if((**dccTypeList).list[t].pluginRef)
	{
		
	}
	else
		switch(t)
		{
			case dcccLIST:
				i=0;
				linkfor(x, fConn)
					if(x->connType == connDCC || x->socks.type==connDCC)
					{
						d=x->dcc;
						
						NumToString(x->port, ss2);
						LSStrCat(7, &ls, dccTypToStr(d->dccType), "\p: ", d->dccUserName, "\p: ", dccFlagTypToStr(d->dccFlags), "\p: ", ss2);
						DCCGetStatus(d, &status);
						if(status.status[0])
							LSConcatLSAndStrAndStr(&ls, "\p: ", status.status, &ls);
						MWColor(&ls, sicCTCP);
						Message(&ls);
						i++;
					}

				if(!i)
					LSGetIntString(&ls, spDCC, sNoDCCOpen);
				else
				{
					NumToString(i, ss);
					LSParamString(&ls, GetIntStringPtr(spDCC, sListedConnections), ss, 0, 0, 0);
				}
				SMPrefixColor(&ls, dsFrontWin, sicCTCP);
				break;
			
			case dcccCLOSE:
				t=DCCTypeFind(name);
				if((t>dccNIL) && (t<dccNIL2))
				{
					if((x = DCCFind(0, t, s))!=0)
						DCCClose(&x, false);
					else
					{
						LSParamString(&ls, GetIntStringPtr(spDCC, sCouldntFindDCC), c, s, 0, 0);
						SMPrefixColor(&ls, dsFrontWin, sicCTCP);
					}
				}
				else
				{
					LSGetIntString(&ls, spDCC, sDCCCloseSyntax);
					SMPrefixColor(&ls, dsFrontWin, sicCTCP);
				}
				break;
			
			case dccCHAT:
				if(IsChannel(name))
				{
					LSGetIntString(&ls, spDCC, sCantDCCChannel);
					SMPrefixColor(&ls, dsFrontWin, sicCTCP);
				}
				else
				{
					if(name[0])
					{
						if(DCCCreate(link, dccCHAT, name, &x)) //if true, then created...
						{
						}
						if(x->dcc->dccFlags == closed || x->dcc->dccFlags == waiting || x->dcc->dccFlags == offered)
							DCCOpen(&x);
					}
					else
					{
						LSGetIntString(&ls, spDCC, sDCCChatSyntax);
						SMPrefixColor(&ls, dsFrontWin, sicCTCP);
					}
				}
				break;
			
			case dccGET:
				if(name[0])
				{
					if((x=DCCFind(0, dccGET, name))!= 0)
						StartDCCGet(x);
					else
					{
						LSParamString(&ls, GetIntStringPtr(spDCC, sNoSuchDCCGETNick), name, 0, 0, 0);
						SMPrefixColor(&ls, dsFrontWin, sicCTCP);
					}
				}
				else
				{
					LSGetIntString(&ls, spDCC, sDCCGetSyntax);
					SMPrefixColor(&ls, dsFrontWin, sicCTCP);
				}
				break;
			
			case dccSEND:
				if(name[0])
				{
					if(IsChannel(name))
					{
						LSGetIntString(&ls, spDCC, sCantDCCChannel);
						SMPrefixColor(&ls, dsFrontWin, sicCTCP);
					}
					else
						DoDCCSend(link, name, mainPrefs->dccReverseSends);
				}
				else
				{
					LSGetIntString(&ls, spDCC, sDCCSendSyntax);
					SMPrefixColor(&ls, dsFrontWin, sicCTCP);
				}
				break;
		}
}

pascal void DCCOpen(connectionPtr *x)
{
	LongString ls;
	Str255 ipa, pn, des, args;
	short i;
	connectionPtr cc = *x;
	dccPtr d=cc->dcc;
	char opened = 0;
	pDCCOpenedData p;
	ConstStringPtr sp;
	
	if(d->dccFlags==waitingRev)
	{
		d->dccFlags=offered;
		//Make a fake
		d->refcon = (void*)++dccSENDRevCount;
		
		args[0] = 0;
		if(d->dccType == dccSEND)
		{
			dccSENDDataPtr dd = (dccSENDDataPtr)d->dccData;
			pstrcpy(dd->sfile.name, des);
			NumToString(dd->sendmax, args);
		}
		else if(d->dccType == dccCHAT)
			pstrcpy(kShadowIRCChatCookie, des);
		else
			*(short*)des=0x012A; //*
		
		i=1;
		while(i<=des[0])
		{
			if(des[i] == ' ')
				des[i] = '_';
			i++;
		}

		ulongstr((long)d->refcon, pn);
		
		sp = dccTypToStr(d->dccType);
		LSStrCat(11, &ls, "\pPRIVMSG ", d->dccUserName, "\p :\1DCC REVERSE ", sp, "\p ", pn, "\p ", des, "\p 0 0 ", args, "\p\1");
		SendCommand(cc->link, &ls);
		LSParamString(&ls, GetIntStringPtr(spDCC, sRequestingRevDCC), sp, d->dccUserName, 0, 0);
		SMPrefixLinkColor(cc->link, &ls, dsFrontWin, sicCTCP);
		d->dccFlags=waiting;
	}
	else if(d->dccFlags == closed)
	{
		if(ConnNewListen(cc, AF_UNSPEC, 10))
		{
			cc->port=ConnGetLocalPort(cc);
                        /*
                         * DCCCreate sets cc->localip to local IP address
                         * based off of connected sockfd
                         */
			ntohl_str(cc->localip.s_addr, ipa);
			ulongstr(cc->port, pn);
			args[0]=0;
			
			if(d->reverse)
			{
				NumToString((long)d->refcon, des);
				d->refcon = 0;
				
				if(d->dccType == dccCHAT)
					pstrcpy(kShadowIRCChatCookie, args);
				else
					args[0] = 0;
			}
			else
			{
				if(d->dccType == dccCHAT)
					pstrcpy(kShadowIRCChatCookie, des);
				else if(d->dccType == dccSEND)
				{
					dccSENDDataPtr dd = (dccSENDDataPtr)d->dccData;
					pstrcpy(dd->sfile.name, des);
					NumToString(dd->sendmax, &args[1]);
					args[0]=args[1]+1;
					args[1]=' ';
				}
				else
					des[0]=0;
			}
			
			do
			{
				i=pos(' ', des);
				if(!i)
					break;
				des[i]='_';
			}while(1);
			
			sp = dccTypToStr(d->dccType);
			if(d->reverse) //what's up with this? it's missing the referenceID...
				LSStrCat(13, &ls, "\pPRIVMSG ", d->dccUserName, "\p :\1DCC REVERSE ", sp, "\p ", des, "\p * ", ipa, "\p ", pn, "\p ", args, "\p\1");
			else
				LSStrCat(12, &ls, "\pPRIVMSG ", d->dccUserName, "\p :\1DCC ", sp, "\p ", des, "\p ", ipa, "\p ", pn, args, "\p\1");
			SendCommand(cc->link, &ls);
			LSParamString(&ls, GetIntStringPtr(spDCC, sRequestingDCC), sp, d->dccUserName, 0, 0);
			SMPrefixLinkColor(cc->link, &ls, dsFrontWin, sicCTCP);
			d->dccFlags=waiting;
			opened=1;
		}
		else
			DCCFailed(x, GetIntStringPtr(spDCC, sFailedToCreate));
	}
	else if(d->dccFlags==offered)
	{
		inet_ntoa_str(cc->ip, des);
		NumToString(cc->port, args);
		ConnSetup(cc, des, cc->port);
		if(cc->connType == connSOCKS)
			ConnFindAddress(cc, cc->name);
		else
			ConnNewActive(cc);
		
		if(1)
		{
			d->dccFlags=opening;
			LSParamString(&ls, GetIntStringPtr(spDCC, sOpeningDCC), d->dccUserName, des, args, 0);
			SMPrefixLinkColor(cc->link, &ls, dsFrontWin, sicCTCP);
			opened=1;
		}
		else
			DCCFailed(x, GetIntStringPtr(spDCC, sFailedToCreate));
	}
	
	if(opened && *x)
	{
		p.connection = *x;
		runPlugins(pDCCOpenedMessage, &p);
	}
}

pascal void DCCWindowClose(MWPtr mw)
{
	if(mw->inactive)
		MWDelete(mw);
	else
	{
		connectionPtr c = MWGetDCC(mw);
		
		MWSetDCC(mw, 0, "\p");
		MWDelete(mw);
		
		if(c)
		{
			((dccCHATDataPtr)c->dcc->dccData)->dccWindow = 0;
			DCCClose(&c, false);
		}
	}
}

pascal void DCCClose(connectionPtr *cn, char silent)
{
	LongString ls;
	dccPtr d;
	pDCCClosedData p;
	connectionPtr conn = *cn;
	
	if(conn)
	{
		p.connection = conn;
		p.silent = silent;
		runPlugins(pDCCClosedMessage, &p);
	
		d=conn->dcc;
		
		ConnClose(conn);
		//Don't abort the connection...let deleteConnection() do it.
		
		if(!silent)
		{
			LSParamString(&ls, GetIntStringPtr(spDCC, sClosedDCC), dccTypToStr(d->dccType), d->dccUserName, 0, 0);
			SMPrefixLinkColor(conn->link, &ls, dsFrontWin, sicCTCP);
		}
		
		if(d->dccType==dccCHAT)
		{
			dccCHATDataPtr dd = (dccCHATDataPtr)d->dccData;
			if(dd->dccWindow)
			{
				if(dd->closeChatWhenConnDies)
					MWDelete(dd->dccWindow);
				else
				{
					MWQuote(dd->dccWindow);
					DrawMWinStatus(dd->dccWindow);
					if(CurrentTarget.dcc==conn && CurrentTarget.type==targDCC)
					{
						InvalTarget(&CurrentTarget);
						UpdateStatusLine();
					}
				}
			}
			SoundService(sndDCCChatClose, 0);
		}
		
		if(d->dccType>dccCHAT && d->dccType < dccNIL2)
		{
			dccGETDataPtr dd = (dccGETDataPtr)d->dccData;
			
			if(!silent)
				SoundService(sndDCCFileClose, d->failedDCC);
			if(dd->gfref)
			{
				FileClose(dd->gfref);
				dd->gfref = 0;
			}
			
			if(dd->macB)
			{
				DisposePtr((Ptr)dd->macB);
				dd->macB = 0;
			}
		}
		
		if(d->dccType < dccNIL2)
		{
			DisposePtr((Ptr)d->dccData);
			d->dccData = 0;
		}
		
		d->DCCInputFunc = 0;
		
		DisposePtr((Ptr)d);
		conn->dcc = 0;
		deleteConnection(cn); //and set to nil
	}
}

static void DCCSendReposition(connectionPtr conn, long newpos)
{
	dccSENDDataPtr dd = (dccSENDDataPtr)conn->dcc->dccData;
	long eof;
	
	//Assuming valid send and newpos < sendmax.
	
	dd->sent = newpos;
	if(dd->MBSend)
	{
		if(newpos > sizeof(MBIIHeader))
			dd->stage = sendDF;
		if(newpos > dd->dfSize + dd->dfPad) //transition over to RF
		{
			dd->stage = sendRF;
			
			if(dd->sfref)
				FileClose(dd->sfref);

			FSpOpenRF(&dd->sfile, fsRdPerm, &dd->sfref);
			FileAdd(dd->sfref, false);
			dd->dfSize2 = dd->sent;
			dd->stage = sendRF;
			GetEOF(dd->sfref, &eof);
			if(!eof) //no res fork
				dd->stage = sendDone;
		}
	}
}

static void DCCSendFileChunk(connectionPtr *cn)
{
	long off, eof, left, readIn, padLen;
	int i;
	Ptr sendbuf;
	long bufSiz;
	dccPtr d = (*cn)->dcc;
	dccSENDDataPtr dd = (dccSENDDataPtr)d->dccData;
	
	if(!dd->MBSend) //standard
	{
		SetFPos(dd->sfref, fsFromStart, dd->sent);
		if(dd->sendpacketlen+dd->sent>dd->sendmax)
			dd->sendpacketlen=dd->sendmax-dd->sent;
		
		bufSiz = dd->bufSiz;
		if(dd->sendpacketlen < bufSiz)
			bufSiz = dd->sendpacketlen;
		
		sendbuf = NewPtr(bufSiz);
		dd->sendpacketlen = bufSiz;
		if(!FSRead(dd->sfref, &dd->sendpacketlen, sendbuf))
		{
			dd->sent+=dd->sendpacketlen;
			if(ConnSend(*cn, sendbuf, dd->sendpacketlen))
				DCCFailed(cn, GetIntStringPtr(spDCC, sLostSendErr));
		}
		else
			DCCFailed(cn, GetIntStringPtr(spDCC, sLostFileReadErr));

		DisposePtr(sendbuf);
	}
	else //macbinary send
	{
		//first we send the header
		//then we send the data fork, with pads
		//then we send the resource fork, with pads
		//then we wait for ack, and close the connection
		
		sendbuf = NewPtr(dd->bufSiz);
		switch(dd->stage)
		{
			case sendNIL:
			case sendHeader:
				//send just the MB header.
				ConnSend(*cn, (Ptr)dd->macB, sizeof(MBIIHeader));
				dd->sent=sizeof(MBIIHeader);
				if(!dd->sfref)
				{
					FSpOpenDF(&dd->sfile, fsRdPerm, &dd->sfref);
					FileAdd(dd->sfref, false);
				}	
				dd->stage++;
				break;
			
			case sendDF:
				off = dd->sent - sizeof(MBIIHeader);
				SetFPos(dd->sfref, fsFromStart, off);
				GetEOF(dd->sfref, &eof);
				left = eof - off;

				readIn = dd->sendpacketlen;
				FSRead(dd->sfref, &readIn, sendbuf);
				if(readIn != dd->sendpacketlen) //then we reached the EOF
				{	//at EOF, we need to check and pad, if necessary.
					padLen = 128 - (readIn % 128);
					if(padLen)
						for(i=0;i<padLen;i++)
							sendbuf[i + readIn] = 0;
					readIn += padLen;
				}
				
				ConnSend(*cn, sendbuf, readIn);
				dd->sent+=readIn;
				if(off + readIn >= eof) //read in entire file, so advance
				{
					FileClose(dd->sfref);
					FSpOpenRF(&dd->sfile, fsRdPerm, &dd->sfref);
					FileAdd(dd->sfref, false);
					dd->dfSize2 = dd->sent;
					dd->stage = sendRF;
					GetEOF(dd->sfref, &eof);
					if(!eof) //no res fork
					{
						FileClose(dd->sfref);
						dd->sfref = 0;
						dd->stage = sendDone;
					}
				}
				break;
			
			case sendRF:
				off = dd->sent - dd->dfSize2;
				SetFPos(dd->sfref, fsFromStart, off);
				GetEOF(dd->sfref, &eof);
				left = eof - off;

				readIn = dd->sendpacketlen;
				FSRead(dd->sfref, &readIn, sendbuf);
				if(readIn != dd->sendpacketlen) //then we reached the EOF
				{	//at EOF, we need to check and pad, if necessary.
					padLen = 128 - (readIn % 128);
					if(padLen)
						for(i=0;i<padLen;i++)
							sendbuf[i + readIn] = 0;
					readIn += padLen;
				}
				
				ConnSend(*cn, sendbuf, readIn);
				dd->sent+=readIn;
				if(off + readIn >= eof) //read in entire file, so advance
				{
					FileClose(dd->sfref);
					dd->sfref = 0;
					dd->stage = sendDone;
				}
				break;
			
			case sendDone:
				DCCClose(cn, false);
				break;
		}
		
		DisposePtr(sendbuf);
	}
}

//Do not show window automatically.
static MWPtr DoJoinDCC(ConstStr255Param n, linkPtr link)
{
	MWPtr mw = MWFindDCC(n, link);
	
	if(mw)
	{
		Str255 s;
		
		if(mw->inactive)
			MWUnquote(mw);
		GetWTitle(mw->w, s);
		if(!pstrcmp(s, n))
			SetWTitle(mw->w, n);
		mw->inactive = 0;
	}
	else
	{
		mw=MWNew(n, dccWin, link, 0);
		if(mw)
		{
			if(mainPrefs->autoLogDCCChat)
				MWStartLogging(mw);
			
			wmAdd(mw);
		}
	}
	
	return mw;
}

pascal void DCCConnOpened(connectionPtr *cn)
{
	MWPtr mw;
	dccPtr d;
	Str255 s;
	OSErr err;
	pDCCConnOpenedData p;
	connectionPtr conn;
	
	if(!cn || !*cn)
		return;
	
	conn = *cn;
	d=conn->dcc;
	d->dccFlags=open;
	switch(d->dccType)
	{
		case dccCHAT:
		{
			dccCHATDataPtr dd = (dccCHATDataPtr)d->dccData;
			d->timeOpened = now;
			pstrcpy(d->dccUserName, &s[1]);
			s[0]=s[1]+2;
			s[1]='=';
			s[s[0]]='=';
			mw=DoJoinDCC(s, conn->link);
			if(mw)
			{
				dd->dccWindow=mw;
				MWSetDCC(mw, conn, d->dccUserName);
				DoJoinSelWin(mw);
			}
			else //could not open, so abort.
			{
				DCCClose(cn, false);
				return;
			}
			
			SoundService(sndDCCChatOpen, 0);

			if(dd->remoteIsShadowIRC)
			{
				LongString cookie;

				dd->sentReplyCookie = true;
				*(long*)&cookie.data[1]=ShadowIRCVersion(0);
				cookie.len = 4;
				DCCSendCookie(cn, 'SIRC', &cookie);
			}
			break;
		}
		case dccGET:
		{
			dccGETDataPtr dd = (dccGETDataPtr)d->dccData;
			CFStringRef fileName;
			ICMapEntry mapEntry;

			d->timeOpened = now;
			
			fileName = CFStringCreateWithPascalString(NULL, dd->gfile.name, kCFStringEncodingMacRoman);
			err = MapFileTypeCreator(fileName, &mapEntry);
			CFRelease(fileName);
			err = CreateUniqueFile(&dd->gfile, mapEntry.fileCreator, mapEntry.fileType);
			
/*
			if(err == dupFNErr) //file already exists. Allow them to reselect where to save it. If they don't allow, kill file.
				err = !DCCPutFile(conn, true);
*/

			if(!err)
			{
				dd->gotten=0;
				err = FSpOpenDF(&dd->gfile, fsRdWrPerm, &dd->gfref);
				FileAdd(dd->gfref, false);
				SoundService(sndDCCFileOpen, 0);
			}
			else
				DCCFailed(cn, GetIntStringPtr(spDCC, sLostNoFile));
			
			break;
		}
		case dccSEND:
		{
			dccSENDDataPtr dd = (dccSENDDataPtr)d->dccData;
			d->timeOpened = now;
			
			SoundService(sndDCCFileOpen, 0);
			DCCSendFileChunk(cn);
			if(*cn && (mainPrefs->FastDCCSends && dd->sent < dd->sendmax))
				DCCSendFileChunk(cn);
			
			break;
		}
	}
	
	if(*cn)
	{
		p.conn = *cn;
		runPlugins(pDCCConnOpenedMessage, &p);
	}
}

#pragma mark -

static void DCCGetLineChat(connectionPtr conn, CEPtr c)
{
	int i;
	long nn;
	LongString ls;
	LongString cookie;
	pDCCIncomingChatDataRec p;
	
	if((nn = ConnGetUntil(conn, (Ptr)&ls.data[1], '\n', 512)) != -1)
	{
		while((nn>0) && ((ls.data[nn]=='\n') || (ls.data[nn]=='\r')))
			nn--;
		ls.len=nn;
		
		conn->lastData=now;

		if(*(unsigned long*)&ls.data[1] == kShadowIRCChatCookie2)
		{
			dccCHATDataPtr dd = (dccCHATDataPtr)conn->dcc->dccData;
			pDCCChatCookieData p;
			p.processed = false;

			LSDelete(&ls, 1, 4);
			switch(*(unsigned long*)&ls.data[1])
			{
				case kCookieReply: //a cookie reply
					LSDelete(&ls, 1, 4);
					switch(*(unsigned long*)&ls.data[1])
					{
						case 'SIRC':
							dd->remoteIsShadowIRC = true;
							dd->remoteShadowIRCVersion = *(unsigned long*)&ls.data[5];
							dd->sentReplyCookie = true;
						
						default:
							p.conn = conn;
							p.cookie = *(unsigned long*)&ls.data[1];
							p.text = &ls;
							LSDelete(&ls, 1, 4);
							p.isReply = true;
							p.processed = true;
							runPlugins(pDCCChatCookieMessage, &p);
					}
					break;
				
				case kCookieNoReply: //an unprocessed cookie reply
				{

					LSDelete(&ls, 1, 4);
					p.conn = conn;
					p.cookie = *(unsigned long*)&ls.data[1];
					LSDelete(&ls, 1, 4);
					p.text = &ls;
					p.processed = false;
					p.isReply = true;
					runPlugins(pDCCChatCookieMessage, &p);
					break;
				}
				
				case 'SIRC':
					dd->sentReplyCookie = true;
					dd->remoteIsShadowIRC = true;
					dd->remoteShadowIRCVersion = *(unsigned long*)&ls.data[5];
					*(long*)&cookie.data[1]=ShadowIRCVersion(0);
					cookie.len = 4;
					DCCSendCookieReply(&conn, 'SIRC', &cookie);
					p.processed = true;
					
				default:
					p.conn = conn;
					p.cookie = *(long*)&ls.data[1];
					LSDelete(&ls, 1, 4);
					p.text = &ls;
					p.isReply = false;
					
					runPlugins(pDCCChatCookieMessage, &p);
					
					if(!p.processed)
					{
						LSConcatStrAndLS("\p    ", &ls, &ls);
						*(long*)&ls.data[1] = p.cookie;
						DCCSendCookieReply(&conn, kCookieNoReply, &ls);
					}
			}
			
			return;
		}
		else
		{
			for(i=1;i<=nn;i++)
				ls.data[i]=ISODecode[ls.data[i]];
			
			if(DCCCTCP(conn, &ls))
				return;
		}
		
		p.conn=conn;
		p.text=&ls;
		p.completelyProcessed=0;
		
		runPlugins(pDCCIncomingChatMessage, &p);
		if((ls.len)&&(!p.completelyProcessed))
		{
			MWMessage(((dccCHATDataPtr)conn->dcc->dccData)->dccWindow, &ls);
			SoundService(sndDCCChat, 0);
		}
	}
}

static void DCCGetLineGet(connectionPtr conn, CEPtr c)
{
	long nn = 0;
	short pt;
	char b;
	long curpos;
	MBIIStartHeader mb;
	long cnt;
	long paddedLenOfDFork;
	int readfromoffset;
	size_t abytes = c->value;
	dccPtr d = conn->dcc;
	dccGETDataPtr dd = (dccGETDataPtr)d->dccData;
	Ptr getbuf;

	while(conn)
	 {
		if(abytes<1)
			break;
			
		getbuf = NewPtr(abytes);
		if((nn = ConnGetData(conn, getbuf, abytes)) > 0)
		{
			abytes -= nn;
			if(!dd->macB) //if this isn't a MacB file, or we haven't gotten the entire header yet...
			{
				SetEOF(dd->gfref, dd->gotten+nn);
				b=!FSWrite(dd->gfref,&nn, getbuf);
			}
			else //we have a valid MacB header
			{
				curpos=dd->gotten+nn-sizeof(MBIIHeader);
				BlockMoveData(&dd->macB->MBIIStart, &mb, sizeof(mb));
				paddedLenOfDFork=mb.dlen%128;
				if(paddedLenOfDFork)
					paddedLenOfDFork=128-paddedLenOfDFork+mb.dlen;
				else
					paddedLenOfDFork=mb.dlen;
				//padded now contains size of fork, padded to 128 bytes
				
				if(!dd->inRF) //haven't gotten to parsing res fork
				{									//this means, we have either the entire df, partial df & rf this chunk
					if(curpos<=mb.dlen)	//entirely df, so just write it
					{
						SetEOF(dd->gfref, curpos);
						b=!FSWrite(dd->gfref, &nn, getbuf);
					}
					else //it's some mixture of df + pad + rf
					{										//if we have any data for the df
						if(curpos-nn<mb.dlen)	//copy from curpos-nn to mb.dlen
						{
							cnt=mb.dlen-(curpos-nn); //bytes to save
							SetEOF(dd->gfref, mb.dlen);
							FSWrite(dd->gfref, &cnt, getbuf);
							readfromoffset=cnt;
							if(curpos-paddedLenOfDFork>=0) //if we can skip the entire pad area
								readfromoffset=nn-(curpos-paddedLenOfDFork);
							FileClose(dd->gfref);
							dd->gfref=0;
						}
						else
							readfromoffset=0;
						//readfromoffset is the buffer pos to start reading from now
						
						//if we have any data for rf...
						if(curpos-paddedLenOfDFork>0) //init rf and dump
						{
							dd->inRF=true;
							if(dd->gfref)
								FileClose(dd->gfref);
							FSpOpenRF(&dd->gfile, fsRdWrPerm, &dd->gfref); //don't create, since it exists, just len=0
							FileAdd(dd->gfref, false);
							if(curpos-paddedLenOfDFork>mb.rlen) //we are also hitting pads, so kill the rest of 'em
								cnt=mb.rlen;
							else
								cnt=curpos-paddedLenOfDFork;
							SetEOF(dd->gfref, cnt);
							SetFPos(dd->gfref, fsFromStart, 0);
							b=!FSWrite(dd->gfref, &cnt, (Ptr)(((long)getbuf)+readfromoffset));
							//if this hits EOF, closing dcc will close RF
						}
					}
				}
				else //it's strictly RF
				{
					curpos-=paddedLenOfDFork;
					if(curpos>mb.rlen) //hitting pads
					{
						cnt=nn-(curpos-mb.rlen);
						curpos=mb.rlen;
					}
					else
						cnt=nn;
					
					SetEOF(dd->gfref, curpos);
					b=!FSWrite(dd->gfref, &cnt, getbuf);
				}
			}
			if(b)
			{
				b=!dd->gotten;
				dd->gotten+=nn;
				
				if(b) //first time we've gotten data, so check for MB header
				{
					dd->macB=(MBIIHeaderPtr)NewPtr(sizeof(MBIIHeader));
					BlockMoveData(getbuf, dd->macB, sizeof(MBIIHeader));
					pt=ValidateMBHeader(dd->macB, true);
					if(pt==PT_None)
					{
						DisposePtr((Ptr)dd->macB);
						dd->macB = 0;
					}
					else //we've got a MacB file. process as such
					{
						CInfoPBRec pb;
						enum clearflags {clearflags = 0x8000 + 0x4000}; //locked + invisible
						
						BlockMoveData(&dd->macB->MBIIStart, &mb, sizeof(mb));
						
						SetEOF(dd->gfref, 0);
						if(!pstrcasecmp(mb.name, dd->gfile.name)) //don't do anything if it's the same name; just zap the contents
						{
							if(!FSpRename(&dd->gfile, mb.name)) //if we can rename the file, do so.
								pstrcpy(mb.name, dd->gfile.name);
							//else we can't change the MB file to match its internal name, but we'll be ok with the existing name
						}
						
						pb.hFileInfo.ioNamePtr = dd->gfile.name;
						pb.hFileInfo.ioVRefNum = dd->gfile.vRefNum;
						pb.dirInfo.ioFDirIndex = 0;
						pb.hFileInfo.ioDirID = dd->gfile.parID;
						if(!PBGetCatInfo(&pb, false))
						{
							pb.hFileInfo.ioNamePtr = dd->gfile.name;
							pb.hFileInfo.ioVRefNum = dd->gfile.vRefNum;
							pb.dirInfo.ioFDirIndex = 0;
							pb.hFileInfo.ioDirID = dd->gfile.parID;
							pb.hFileInfo.ioFlFndrInfo.fdType = mb.ftype;
							pb.hFileInfo.ioFlFndrInfo.fdCreator = mb.fcreator;
							pb.hFileInfo.ioFlFndrInfo.fdFlags = (((mb.flags_high << 8) & 0xFF00) | (mb.flags_low & 0xFF)) & ~clearflags;
							pb.hFileInfo.ioFlXFndrInfo.fdScript = mb.script;
							pb.hFileInfo.ioFlXFndrInfo.fdXFlags = mb.flags_x;
							pb.hFileInfo.ioFlCrDat = mb.create_date;
							pb.hFileInfo.ioFlMdDat = mb.mod_date;
							PBSetCatInfo(&pb, false);
						}
						
						//FSpOpenDF(&dd->gfile, fsRdWrPerm, &dd->gfref);
						//FileAdd(dd->gfref, false);
						cnt=nn-sizeof(MBIIHeader);
						SetEOF(dd->gfref, cnt);
						FSWrite(dd->gfref, &cnt, (Ptr)(((long)getbuf)+sizeof(MBIIHeader)));
					}
				}
				
				if(ConnSend(conn, (Ptr)&dd->gotten, sizeof(long)))
					DCCFailed(&conn, GetIntStringPtr(spDCC, sLostAckErr));
			}
			else
			{
				DCCFailed(&conn, GetIntStringPtr(spDCC, sLostWriteErr));
			}
		}
		else
		{
			DCCFailed(&conn, GetIntStringPtr(spDCC, sLostRecieveErr));
		}
		
		DisposePtr(getbuf);
	};
}

static void DCCGetLineSend(connectionPtr conn, CEPtr c)
{
	long nn = 0;
        long ack;
        size_t abytes = c->value;
	dccSENDDataPtr dd = (dccSENDDataPtr)conn->dcc->dccData;
	
	while(conn)
	 {
		if(abytes<sizeof(long))
			break;
		if(abytes>sizeof(long))
			abytes=sizeof(long);
		
		if((nn = ConnGetData(conn, (Ptr)&ack, abytes)) > 0)
		{
			dd->acked = ack;
                        abytes -= nn;
			
			if(ack==dd->sent)
			{
				if(ack==dd->sendmax)
					DCCClose(&conn, false);
				else
				{
					DCCSendFileChunk(&conn);
				}
			}
			else if(ack>dd->sent)
			{
				DCCFailed(&conn, GetIntStringPtr(spDCC, sLostAckErr));
			}
//¥
			else if(ack<dd->sent && mainPrefs->FastDCCSends)
			{
				if((ack + dd->bufSiz >= dd->sent) && (dd->sent < dd->sendmax))
					DCCSendFileChunk(&conn);
			}
		}
		else
		{
			DCCFailed(&conn, GetIntStringPtr(spDCC, sLostRecieveErr));
		}
	}
}

void dccEvent(CEPtr c, connectionPtr conn)
{
	switch(c->event)
	{
		case C_Established:
			DCCConnOpened(&conn);
			break;
		
		case C_SearchFailed:
			DCCFailed(&conn, "\pCouldn't lookup host");
			break;
		
		case C_FailedToOpen:
			DCCFailed(&conn, GetIntStringPtr(spDCC, sFailedToOpen));
			break;
		
		case C_Closed:
			DCCFailed(&conn, "\pClosed");
			break;
		
		case C_CharsAvailable:
			conn->dcc->DCCInputFunc(conn, c);
			break;
			
		case C_Found:
			break;
	}
}

#pragma mark -

static void DCCProcessChat(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, StringPtr s, long revNum)
{
	LongString ls;
	Str255 des, c, st;
	connectionPtr x;
	dccPtr d;
	long l;
	char autoopen = 0;
	
	if(!DCCCreate(link, dccCHAT, fr, &x)) //collision. delete the original, create a new one, an automatically opnen it.
	{
//		LSParamString(&ls, GetIntStringPtr(spDCC, sDCCCollision), dccTypToStr(dccCHAT), fr, 0, 0);
//		SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);

		//but only if the chat's not going already...
		if(x->dcc->dccFlags == open)
			return;
		
		DCCClose(&x, true);
		DCCCreate(link, dccCHAT, fr, &x);
		autoopen = 1;
	}

	d = x->dcc;
	
	NextArg(s, des); //the "filename" - cHaT or otherwise
	
	pstrcpy(uah, x->name);
	if(pstrcmp(des, kShadowIRCChatCookie))
		((dccCHATDataPtr)d->dccData)->remoteIsShadowIRC = 1;
	
	if(revNum != -1)
	{
		NextArg(s, 0); //IP
		NextArg(s, 0); //port
		x->ip.s_addr = x->port = 0;
		d->refcon = (void*)revNum;
		d->dccFlags = closed;
		d->reverse = true;
	}
	else
	{
		NextArg(s, c); //IP
		x->ip.s_addr = str_htonl(c);
		NextArg(s, c); //port
		StringToNum(c, &l);
		x->port=l;
		d->dccFlags=offered;
	}


	if(revNum != -1)
	{
		LSParamString(&ls, GetIntStringPtr(spDCC, sDCCRevRequest), dccTypToStr(dccCHAT), fr, "\p", 0);
	}
	else
	{
		Str255 st2;
		
		inet_ntoa_str(x->ip, c);
		NumToString(x->port, st);
		pstrcat(c, "\p:", st2);
		pstrcat(st2, st, st2);
		LSParamString(&ls, GetIntStringPtr(spDCC, sDCCRequest), dccTypToStr(dccCHAT), fr, des, st2);
	}
	
	SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
	if(autoopen || (mainPrefs->autoAcceptDCCChat && (!link->isAway || !mainPrefs->autoAcceptDCCChatWhenNotAway)))
		DCCOpen(&x);
}

static void DCCProcessGet(linkPtr link, ConstStr255Param fr, ConstStr255Param uah, StringPtr s, long revNum)
{
	const short t = dccGET;
	Str255 c;
	long l;
	connectionPtr x;
	dccPtr d;
	dccGETDataPtr dd;
	short i;
	Str255 des, st;
	ConstStringPtr dccTypeStr;
	LongString ls;
	
	DCCCreate(link, t, fr, &x); //can not collide, but what about 2x same file?
	
	d = x->dcc;
	NextArg(s, des);

	i=1;
	while(i <= des[0])
	{
		if(des[i] == '/')
			pdelete(des, 1, i);
		else
		{
			if(des[i] == ':')
				des[i] = '_';
			i++;
		}
	}
	
	pstrcpy(uah, x->name);
	if(revNum != -1)
	{
		NextArg(s, 0); //IP
		NextArg(s, 0); //port
		x->ip.s_addr = x->port = 0;
		d->refcon = (void*)revNum;
		d->dccFlags = closed;
		d->reverse = true;
	}
	else
	{
		NextArg(s, c); //IP
		x->ip.s_addr = str_htonl(c);
		NextArg(s, c); //port
		StringToNum(c, &l);
		x->port=l;
		d->dccFlags=offered;
	}
	
	dd = (dccGETDataPtr)d->dccData;
	
	if(mainPrefs->autoSaveDCC)
		FSMakeFSSpec(dccFolderFSp.vRefNum, dccFolderFSp.parID, des, &dd->gfile);
	else
		FSMakeFSSpec(0, 0, des, &dd->gfile);
	
	NextArg(s, st); //get file size
	if(st[0])
	{
		StringToNum(st, &dd->getmax);
		i=des[0]+2;
		pstrcpy(st, &des[i]);
		des[0]+=des[i]+3;
		des[i-1] = ' ';
		des[i] = '(';
		des[des[0]]=')';
	}
	else
		dd->getmax=-1;
	
	pstrcpy(des, &des[2]);
	des[0]+=3;
	des[1] = ' ';
	des[2] = '[';
	des[des[0]]=']';
	
	dccTypeStr = dccTypToStr(t);
	
	if(revNum != -1)
	{
		LSParamString(&ls, GetIntStringPtr(spDCC, sDCCRevRequest), dccTypeStr, fr, des, 0);
	}
	else
	{
		inet_ntoa_str(x->ip, c);
		NumToString(x->port, st);
		LSConcatStrAndStrAndStr(c, "\p:", st, &ls);
		LSCopyString(&ls, 1, ls.len, c);
		LSParamString(&ls, GetIntStringPtr(spDCC, sDCCRequest), dccTypeStr, fr, des, c);
	}

	SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
	
	if(mainPrefs->autoDCCGet)
		StartDCCGet(x);
}

static connectionPtr DCCFindRefcon(void* refcon)
{
	connectionPtr c;

	linkfor(c, fConn)
		if(c->dcc)
			if(c->dcc->refcon == refcon)
				return c;
	
	return 0;
}

//DCC REVERSE PROTOCOL refcon filename ip port [size]
static void DCCProcessRequest(linkPtr link,ConstStr255Param fr, ConstStr255Param uah, StringPtr s, short type)
{
	Str255 c, des, dccTypeStr, st;
	connectionPtr x;
	LongString ls;
	long l;
	short t2;

	switch(type)
	{
		case dccRESUME:
		{
			long port;
			long fpos;
			
			NextArg(s, des); //filename
			NextArg(s, dccTypeStr); //port
			StringToNum(dccTypeStr, &port);
			NextArg(s, st); //fpos
			StringToNum(st, &fpos);
			
			LSParamString(&ls, GetIntStringPtr(spDCC, sDCCResumeRequest), fr, des, st, 0);
			SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);

			//Find the dcc conntion with the port...
			x = DCCFindSendPort(port);
			if(x)
			{
				dccSENDDataPtr dd = (dccSENDDataPtr)x->dcc->dccData;
				
				if(fpos > dd->sendmax)
				{
					LSGetIntString(&ls, spDCC, sDCCResumeFailedEOF);
					SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
					
					LSStrCat(5, &ls, "\pNOTICE ", fr, "\p :\1ERRMSG DCC RESUME :Resume Failed: tried to reposition past EOF\1");
					SendCommand(link, &ls);
				}
				else //accept!
				{
					DCCSendReposition(x, fpos);
					
					LSStrCat(9, &ls, "\pPRIVMSG ", fr, "\p :\1DCC ACCEPT ", des, "\p ", dccTypeStr, "\p ", st, "\p\1");
					SendCommand(link, &ls);
				}
			}
			else
			{
				LSGetIntString(&ls, spDCC, sDCCResumeFailedNoDCC);
				SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
				
				LSConcatStrAndStrAndStr("\pNOTICE ", fr, "\p :\1ERRMSG DCC RESUME :Resume Failed\1", &ls);
				SendCommand(link, &ls);
			}
			return;
		}
		
		case dccREVERSE:
			NextArg(s, c); //protocol
			type = DCCTypeFind(c);
			t2 = (**dccTypeList).list[type].reverse;
			if(t2 == -1) //unreversable
			{
				LSParamString(&ls, GetIntStringPtr(spDCC, sUnsupportedDCCRev), c, fr, 0, 0);
				SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
			}
			else
			{
				NextArg(s, c); //refcon
				StringToNum(c, &l);

				//¥Come back here and clean up, since this assumes that the only possible reversesable
				//¥types are send and get!
				
				if(type == dccCHAT)
				{
					DCCProcessChat(link, fr, uah, s, l);
				}
				if(type == dccGET) //the response from a dcc reverse send
				{
					x = DCCFindRefcon((void*)l);
					if(x)
					{
						NextArg(s, 0); //name
						NextArg(s, c); //IP
						x->ip.s_addr = str_htonl(c);
						NextArg(s, c); //port
						StringToNum(c, &l);
						x->port=l;
						
						x->dcc->dccFlags = offered;
						DCCOpen(&x);
					}
					else
					{
						LSParamString(&ls, GetIntStringPtr(spDCC, sDCCSENDREVErrNonExistant), fr, 0, 0, 0);
						SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
					}
				}
				else
				{
					DCCProcessGet(link, fr, uah, s, l);
				}
			}
			return;
		
		case dccCHAT:
			DCCProcessChat(link, fr, uah, s, -1);
			break;
		
		case dccSEND: //when receiving this from the other side, we want to turn it into a get...
			type = (**dccTypeList).list[type].reverse;
			
			if(type == dccGET)
			{
				DCCProcessGet(link, fr, uah, s, -1);
				break;
			}
			else
			{
			}
		default:
			//HUH?!?!?
			;
	}
}

pascal void DCCRequest(linkPtr link,ConstStr255Param fr, ConstStr255Param uah, Str255 s)
{
	Str255 c;
	short t;
	LongString ls;
	
	NextArg(s, c);
	if(mainPrefs->enableDCC)
	{
		t = DCCTypeFind(c);
		if(t != -1)
			DCCProcessRequest(link, fr, uah, s, t);
		else //unknown dcc request
		{
			LSParamString(&ls, GetIntStringPtr(spDCC, sUnsupportedDCC), c, fr, 0, 0);
			SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
			
			LSStrCat(5, &ls, "\pNOTICE ", fr, "\p :\1ERRMSG DCC ", c, "\p :Unsupported request\1");
			SendCommand(link, &ls);
		}
	}
	else
	{
		LSParamString(&ls, GetIntStringPtr(spDCC, sDCCRequestIgnore), c, fr, 0, 0);
		SMPrefixLinkColor(link, &ls, dsFrontWin, sicCTCP);
	}
}

#pragma mark -

pascal void DCCSSayQuiet(connectionPtr *x, LongString *ls)
{
	int i, n;
	
	n=ls->len;
	if(n>=maxLSlen)
		n=maxLSlen-1;
	
	for(i=1;i<=n;i++)
		ls->data[i]=ISOEncode[ls->data[i]];
	
	ls->data[n+1]=10;
	if(ConnSend(*x, (Ptr)&ls->data[1], n+1))
		DCCClose(x, false);
}

pascal void DCCSSay(connectionPtr *x, LongString *ls)
{
	LongString ls2;
	int i, n;
	
	LSConcatStrAndLS("\p--> ", ls, &ls2);
	MWColor(&ls2, sicOutgoing);
	MWMessage(((dccCHATDataPtr)(*x)->dcc->dccData)->dccWindow, &ls2);
	
	n=ls->len;
	if(n>=maxLSlen)
		n=maxLSlen-1;
	
	for(i=1;i<=n;i++)
		ls->data[i]=ISOEncode[ls->data[i]];
	
	ls->data[n+1]=10;
	if(ConnSend(*x, (Ptr)&ls->data[1], n+1))
		DCCClose(x, false);
}

pascal void DCCSendCookie(connectionPtr *x, long cky, LongString *ls)
{
	LongString cookie;
	
	*(long*)&cookie.data[1] = kShadowIRCChatCookie2;
	*(long*)&cookie.data[5] = cky;
	cookie.len = 8;
	LSConcatLSAndLS(&cookie, ls, &cookie);
	LSAppend1(cookie, 10);
	
	if(ConnSend(*x, &cookie.data[1], cookie.len))
		DCCClose(x, false);
}

pascal void DCCSendCookieReply(connectionPtr *x, long cky, LongString *ls)
{
	LongString cookie;
	
	*(long*)&cookie.data[1] = kShadowIRCChatCookie2;
	*(long*)&cookie.data[5] = kCookieReply;
	*(long*)&cookie.data[9] = cky;
	cookie.len = 12;
	LSConcatLSAndLS(&cookie, ls, &cookie);
	LSAppend1(cookie, 10);
	
	if(ConnSend(*x, &cookie.data[1], cookie.len))
		DCCClose(x, false);
}

static void str10neg(StringPtr s)
{
	unsigned char y[]="\p4294967296";
	int i,x,c=0;
	
	for(i=10;i>0;i--)
	{
		x=y[i]-s[i]-c;
		if(x<0)
		{
			c=1;
			x+=10;
		}
		else
			c=0;
		
		s[i]=(x+48);
	}
}

pascal void ulongstr(unsigned long l, StringPtr s)
{
	int i;
	char n;
	
	if(l)
	{
		n=(l<0);
		if(l<0)
			l=-l;
		
		for(i=10;i>0;i--)
		{
			s[i]=(l%10)+48;
			l/=10;
		}
		s[0]=10;
		if(n)
			str10neg(s);
		n=0;
		while(!s[n+1])
			n++;
		for(s[0]-=n, i=1;i<n;i++)
			s[i]=s[n+i];
	}
	else
		s[0]=0;
	//else
	//bad
}

pascal ConstStringPtr dccTypToStr(short d)
{
	return (**dccTypeList).list[d].name;
}

pascal ConstStringPtr dccFlagTypToStr(short d)
{
	switch(d)
	{
		case closed:
			return "\pCLOSED";
		case waiting:
			return "\pWAITING";
		case offered:
			return "\pOFFERED";
		case opening:
			return "\pOPENING";
		case open:
			return "\pOPEN";
		default:
			return "\pUNKNOWN";
	}
}

#pragma mark -

OSStatus SetupDCCFolder(FSRef *ref)
{
	OSStatus err = noErr;
	FSRef localRef;
	
	err = ReadDirURLRef(kPrefDCCFolder, &localRef);
	if(err != noErr)
	{
		err = GetFSRefForDownloadsFolder(&localRef);
		if(err == noErr)
		{
			BlockMoveData(&localRef, ref, sizeof(FSRef));
			err = WriteDirURLRef(kPrefDCCFolder, ref);
		}
		else
			err = paramErr;
	}
	else
		BlockMoveData(&localRef, ref, sizeof(FSRef));
	
	return err;
}
