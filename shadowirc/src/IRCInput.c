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

#include "WASTE.h"

#include "StringList.h"
#include "IRCGlobals.h"
#include "TCP.h"
#include "utils.h"
#include "MsgWindows.h"
#include "channels.h"
#include "DCC.h"
#include "connections.h"
#include "IRCChannels.h"
#include "IRCInputPrivate.h"
#include "IRCInput.h"
#include "InputLine.h"
#include "MsgBuffer.h"
#include "plugins.h"
#include "IRCCommands.h"
#include "IRCAux.h"
#include "Inline.h"
#include "TextManip.h"
#include "Floaters.h"
#include "AppearanceHelp.h"

long lastInput, lastKey;
char iwFront=0;
char inputLocked = 0;

static pascal char MWNavKey(EventModifiers modifiers, long message);

pascal void ServerCommands(LongString *ls, linkPtr link);
inline void pluginMWGotText(LongString *ls, MWPtr win);

static pascal void SOCKSConnect(connectionPtr conn);
static pascal char CommandPeriod(void);

static pascal char CommandPeriod(void)
{
	KeyMap map;
	
	GetKeys(map);
	return (map[1] & 0x808000) == 0x808000;
}

pascal char ProcessInputHunk(const char* text, long len, MWPtr mw, char clearEnd, long *lastCR)
{
	long x;
	long start;
	char gotACR;
	LongString ls;
	long lCR;
	
	start=0;
	gotACR=0;
	lCR = 0;
	if(lastCR)
		*lastCR = -1;
	for(x=0;x<len;x++)
	{
		if(text[x]==13 && x==lCR+1)
		{
			lCR = x;
			start = x+1;
			if(lastCR)
				*lastCR = x;
			continue;
		}
		if(text[x]==13 || x-start > 490 || (x+1==len && clearEnd))
		{
			if(CommandPeriod())
				return true;
			if(x-start>0)
			{
				lCR = x;
				if(lastCR)
					*lastCR = x;
				gotACR=1;
				ls.len = x-start;
				if(text[x]!=13)
					ls.len++;
				if(ls.len)
				{
					BlockMoveData(&text[start], &ls.data[1], ls.len);
					ProcessLine(&ls, false, 0, mw);
				}
				start=x+1;
			}
			else
				start++;
		}
	}
	
	return clearEnd || lCR >= len;
}

pascal void processPaste(MWPtr mw, char dragAndDrop)
{
	long l;
	CharsHandle h;
	Handle h2;
	long lastCR;
	Ptr p;
	char cp;
	WEReference il = _ILGetWE(mw);
	
	WEDeactivate(il);
	l=WEGetTextLength(il);
	h=(CharsHandle)NewHandle(l);
	HLock(h);
	p = *h;
	
	h2=WEGetText(il);
	BlockMoveData(*h2, p, l);
	
	cp=0;
	//If D&D and there's a CR *anywhere*, completely process it
	if(dragAndDrop)
		for(lastCR = 0; lastCR < l;lastCR++)
			if(p[lastCR]==13)
			{
				cp=1;
				break;
			}
		
	cp = ProcessInputHunk(*h, l, mw, cp, &lastCR);
	if(dragAndDrop)
	{
		if(cp)
		{
			l = 0;
			ILSetLine(mw, (LongString*)&l);
		}
		//Else do nothing. It's compeltely processed unless it's not a complete line.
	}
	else
	{
		if(lastCR > -1 && l - lastCR > 0)
		{
			WESetSelection(0, 0x7FFFFFFF, il);
			WEDelete(il);
			
			WEInsert(&p[lastCR+1], l - lastCR - 1, 0, 0, il);
			WESetSelection(0x7FFFFFFF, 0x7FFFFFFF, il);
			WESelView(il);
		}
	}
	
	DisposeHandle((Handle)h);
	WEActivate(il);
}

pascal void ProcessLine(LongString* line, char stackup, char action, MWPtr mw)
{
	int i;
	Str255 s, s2;
	pInputMessageDataRec p;
	target targ;

	lastInput=now;
	
	if(!mw)
		targ = CurrentTarget;
	else
		SetTarget(mw, &targ);

	p.link = targ.link;
	p.inputText=line;
	p.mw = mw;
	p.action=action;
	runPlugins(pInputMessage, &p);
	
	i=line->len;
	while((i>0)&&(line->data[i]==' '))
		i--;
	line->len=i;
	
	if((line->len>0) && p.action)
	{
		s[0]=4;
		s[1]=CmdChar;
		*(short*)&s[2] = 'me';
		s[4]=' ';
		LSCopyString(line, 1, 4, s2);
		if(!pstrcmp(s, s2))
			LSConcatStrAndLS(s, line, line);
	}
	
	if(!((line->data[1]==line->data[2])&&(line->data[1]==CmdChar)))
	{
		if(stackup)
			ILAddHistory(mw, line);
	}
	else
		LSDelete(line, 1, 1);

	InputHandler(line, &targ);
	mbnum=-1;
}

pascal void GetLine(char action, MWPtr mw)
{
	LongString line;
	short s;
	
	ILGetLine(mw, &line);
	s=0;
	ILSetLine(mw, (LongString*)&s);
	ProcessLine(&line, true, action, mw);
}

inline void pluginMWGotText(LongString *ls, MWPtr win)
{
	pUIMWGotTextDataRec p;
	
	p.window=win;
	p.text=ls;
	runIndPlugin(win->pluginRef, pUIMWGotTextMessage,&p);
}

pascal void HandleMessage(LongString *ls, targetPtr targ)
{
	LongString tls;
	pInputTextDisplayData p;
	
	if(targ->bad)
		StatusMsg(targ->link, 2); //E_NOTARGET
	else if(targ->type == targPlugin)
		pluginMWGotText(ls, targ->mw);
	else if(targ->type == targDCC)
		DCCSSay(&targ->dcc, ls);
	else if(targ->type == targChannel || targ->type == targQuery)
	{
		FormatNick(targ->link->CurrentNick, &tls, 0, kNickNormal + kNickSend);

		LSConcatLSAndLS(&tls, ls, &tls);
		MWColor(&tls, sicOutgoing);
		
		p.targ = targ;
		p.display = &tls;
		p.input = ls;
		p.textIndenting = mainPrefs->textIndenting;
		p.ircIIDisplay = mainPrefs->ircIIDisplay;
		runPlugins(pInputTextDisplayMessage, &p);
		
		MWMessage(targ->mw, &tls);
		LSConcatStrAndStrAndStr("\pPRIVMSG ", MWGetName(targ->mw, 0), "\p :", &tls);
		LSConcatLSAndLS(&tls, ls, &tls);
		SendCommand(targ->link, &tls);
	}
}

pascal void InputHandler(LongString *ls, targetPtr targ)
{
	if(ls->len)
	{
		if(ls->data[1]==CmdChar)
			HandleCommand(targ->link, ls);
		else
			HandleMessage(ls, targ);
	}
	idleTime=now;
}

#pragma mark -

pascal void LockInput(void)
{
	if(!inputLocked)
	{
		inputLocked = 1;
		IWLock();
	}
}

pascal void UnlockInput(void)
{
	if(inputLocked)
	{
		inputLocked = 0;
		IWUnlock();
	}
}

#pragma mark -

static pascal char MWNavKey(EventModifiers modifiers, long message)
{
	char c = message % 256;
	char cmd = (modifiers & cmdKey) == cmdKey;
	char opt = (modifiers & optionKey) == optionKey;
	
	char proc = 1;
	
	if(c == 11 || c == 12)
		MWPage(MWActive, c==11);
	else if((modifiers & controlKey) == controlKey)
	{
		if(c == 30)
			MWScroll(MWActive, -MWActive->scrpHeight);
		else if(c==31)
			MWScroll(MWActive, MWActive->scrpHeight);
		else
			proc = 0;
	}
	else if(!cmd)
	{
		if(c == 1) //home
			MWPage(MWActive, 3);
		else if(c==4)
			MWPage(MWActive, 2);
		else
			proc=0;
	}
	else if(cmd)
	{
		if(c == 30 || c==31)
			MWPage(MWActive, (c==30) + (opt*2));
		else
			proc=0;
	}
	else
		proc=0;
	
	return proc;
}

pascal void Key(EventRecord *e, char dontProcess)
{
	char c;
	LongString ls;
	pKeyDownDataRec p;
	Str255 s;
	WEReference il;
	
	c=e->message & 0xFF;

	if(!iwFront && (MWActive && MWActive->winType == textWin))
	{
		if(!MWNavKey(e->modifiers, e->message))
		{
			if(c!=27)
			{
				WEKey(c, e->modifiers, MWActive->we);
				SetWindowModified(MWActive->w, WEGetModCount(MWActive->we) != 0);
			}
		}
		return;
	}

	p.character=&c;
	p.e = e;
	runPlugins(pKeyDownMessage, &p);
	
	il = ILGetWE();
	if(!il)
		return;
	
	if(!c)
		return;
	
	if(MWNavKey(e->modifiers, e->message) || dontProcess)
		return;
	
	iwFront = true;
	lastKey=now;
	if((c==3) && !(e->modifiers & controlKey)) //enter key
		c=13;
	
	switch(c)
	{
		case 13:	//return
			SoundService(sndUserHitReturn, 0);
			GetLine((e->modifiers & optionKey)==optionKey, CurrentTarget.mw);
			mbInput = false;
			break;
		
		case 9: //tab
		{
			LongString ls2;
			long p1, p2;
			
			mbInput = true;
			if(e->modifiers & shiftKey)
			{
				if(--mbnum < 0)
				{
					mbnum = MAXMB -1;
					while(mbnum && !messagebuffers[mbnum]->nick[0])
						mbnum--;
				}
			}
			else
			{
				if(++mbnum>=MAXMB || !messagebuffers[mbnum]->nick[0])
				{
					if(!mbnum)
						mbnum=-1;
					else
						mbnum=0;
				}
			}
			
			if(mbnum>-1)
			{
				pstrcpy(messagebuffers[mbnum]->nick, s);
				SAppend1(s, ' ');
			}
			else
				s[0]=0;
			
			LSConcatStrAndStr("\p/msg ", s, &ls);
			GetInputLine(&ls2);
			if(ls2.len > 4 && *(long*)&ls2.data[1] == '/msg' && ls2.data[5]==' ') //replace
			{
				LSNextArg(&ls2, 0);
				LSNextArg(&ls2, 0);
				LSConcatLSAndLS(&ls, &ls2, &ls);
			}
			else //insert
				LSConcatLSAndLS(&ls, &ls2, &ls);
			
			p1= LSPosChar(' ', &ls);
			ls.data[p1]='_';
			p2 = LSPosChar(' ', &ls);
			ls.data[p1] = ' ';
			
			if(++p1>--p2)
				p1--;
			
			SetInputLineCursorSelection(p1, p2);
			SetInputLine(&ls);
			break;
		}
		
		case 27: //esc
			if(mainPrefs->escClearsInputline)
			{
		case 21: //0x15 = ctrl-u
		case 4: //ctrl-d
				ls.len=0;
				SetInputLine(&ls);
			}
			break;
		
		case 30:
			if(!mainPrefs->optionToMoveInputLine || (e->modifiers & optionKey)==optionKey)
				RecallLineUp();
			else
				WEKey(c, e->modifiers, il);
			break;
		
		case 31:
			if(!mainPrefs->optionToMoveInputLine || (e->modifiers & optionKey)==optionKey)
				RecallLineDown();
			else
				WEKey(c, e->modifiers, il);
			break;
		
		case 0:
		case 1: //ctrl-a
			break;
		
		default:
			WEKey(c, e->modifiers, il);
			WESelView(il);
	}
}

#pragma mark -

static pascal void SOCKSConnect(connectionPtr conn)
{
	pServiceCWLinkStateChangeData p;
	Str255 send;
	long nn;

	/*				
			VER		CMD		RSV		ATTYP			DST-ADDR	DST-Port
			0x05	0x01	0x00	0x03			(var)			2b
						connect				domain name	pstr
			
			VER		CD		DSTPort	DSTIP	USERID	NULL
			0x04	0x01	2b				4b			xb			0x00
			4a:							0x000000xx						hostStr + NULL
	*/
	
	if(mainPrefs->firewallType == fwSOCKS5)
	{
		*(long*)send = 0x05010003;
		nn=4;
		pstrcpy(conn->socksName, &send[4]);
		nn+=send[4]+1;
		*(short*)&send[nn] = conn->socksPort;
		nn+=2;
	}
	else if(mainPrefs->firewallType == fwSOCKS4A || mainPrefs->firewallType == fwSOCKS4)
	{
		pstrcpy(mainPrefs->socksUser, &send[7]);
		*(short*)send = 0x0401;
		*(short*)&send[2] = conn->socksPort;
		if(mainPrefs->firewallType == fwSOCKS4)
			*(struct in_addr*)&send[4] = conn->ip2;

		nn = 9 + mainPrefs->socksUser[0];
		if(mainPrefs->firewallType == fwSOCKS4A)
		{
			*(long*)&send[4] = 0x00000001; //dest ip 0.0.0.1
			
			pstrcpy(conn->socksName, &send[nn]);
			send[nn] = 0; //trailing null for name
			nn+=conn->socksName[0] + 1;
		}

		send[nn] = 0; //trailing null for name (or host)
	}
	
	ConnPut(&conn, send, nn);

	conn->connectStage=csSOCKSSendingRequest;
	if(conn->socksType == connIRC)
	{
		p.link = conn->link;
		p.connectStage = conn->connectStage;
		runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
	}
}

pascal void processSOCKS(CEPtr c, connectionPtr conn)
{
	pServiceCWLinkStateChangeData p;
	Str255 send;
	long nn;
	LongString ls;
	
	conn->tryingToConnect = false;
		
	if(c->event == C_Established)
	{
		conn->connectStage=csSOCKSNegotiatingMethod;
		
		if(conn->socksType == connIRC)
		{
			p.link = conn->link;
			p.connectStage = conn->connectStage;
			runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
		}
		
		if(mainPrefs->firewallType == fwSOCKS5)
		{
			//Send out a identifier - in our case, 0x05020002 -- SOCKS5, 2 method, #0 no auth and #2 user/pass
			send[0] = 5;
			send[2] = 0; //no auth
			if(mainPrefs->socksUser[0] && mainPrefs->socksPass[0])
			{
				send[1] = 2; //2 methods
				send[3] = 2; //user/pass auth
				nn = 4;
			}
			else
			{
				send[1] = 1;
				nn = 3;
			}
			
			ConnPut(&conn, send, nn);
		}
		else if(mainPrefs->firewallType == fwSOCKS4A || mainPrefs->firewallType == fwSOCKS4)
		{
			SOCKSConnect(conn);
		}
	}
	else if(c->event == C_CharsAvailable)
	{
		if(conn->connectStage == csSOCKSNegotiatingMethod)
		{
			//Read two bytes: version, and method
			nn = c->value; //This had better be >= 2.
			if(nn < 2)
				return;
			nn = 2;

			ConnGetData(conn, (Ptr)send, nn);
			conn->socksMethodVersion = send[0];
			conn->socksMethod = send[1];
			
			if(conn->socksMethod == 0) //no auth
				SOCKSConnect(conn);
			else if(conn->socksMethod == 2) //user/pass auth
			{
				send[0] = 1;
				pstrcpy(mainPrefs->socksUser, &send[1]);
				nn = 2 + send[1];
				pstrcpy(mainPrefs->socksPass, &send[nn]);
				nn+=send[nn]+1;
				ConnPut(&conn, send, nn);

				conn->connectStage=csSOCKSSendingAuthRequest;
				if(conn->socksType == connIRC)
				{
					p.link = conn->link;
					p.connectStage = conn->connectStage;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
				}
			}
			else if(conn->socksMethod == 0xFF)
			{
				ConstStringPtr sp = GetIntStringPtr(spSOCKS, 12);

				conn->connectStage=csSOCKSAuthFailed;
				if(conn->socksType == connIRC)
				{
					LSParamString(&ls, GetIntStringPtr(spSOCKS, 9), sp, 0, 0, 0);
					SMPrefix(&ls, dsConsole);

					p.link = conn->link;
					p.connectStage = conn->connectStage;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
					ServerOK(kServerOKBadSOCKS, conn->link);
				}
				else if(conn->socksType == connDCC)
				{
					DCCFailed(&conn, sp);
				}
			}
		}
		else if(conn->connectStage == csSOCKSSendingAuthRequest) //authing for user/host respponse
		{
			nn = c->value; //This had better be >= 2.
			if(nn < 2)
				return;
			nn = 2;

			ConnGetData(conn, (Ptr)send, nn);
			
			if(send[1] == 0) //success!
				SOCKSConnect(conn);
			else
			{
				ConstStringPtr sp = GetIntStringPtr(spSOCKS, 11);
				
				conn->connectStage=csSOCKSAuthFailed;
				if(conn->socksType == connIRC)
				{
					LSParamString(&ls, GetIntStringPtr(spSOCKS, 9), sp, 0, 0, 0);
					SMPrefix(&ls, dsConsole);

					p.link = conn->link;
					p.connectStage = conn->connectStage;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
					ServerOK(kServerOKBadSOCKS, conn->link);
				}
				else if(conn->socksType == connDCC)
				{
					DCCFailed(&conn, sp);
				}
			}
		}
		else if(conn->connectStage == csSOCKSSendingRequest)
		{
			//This is the reply to our connection request
			nn = c->value; //This had better be >= 2.
			if(nn < 8)
				return;
			else if(nn>255)
				nn = 255;
			
			ConnGetData(conn, (Ptr)send, nn);
			
			//We don't need anything other than the error code, which is now in send[1].
			/*
					VER		REP	RSV		ATYP	BND-ADDR	BND-PORT
					0x05	1b		0x00	1b			var				2b
					
					VER		CD		DSTPORT		DSTIP
					0x00	*1b		2b					4b
					
					* -	90 = granted
							91 = request rejected or failed
							92 = request rejected becasue SOCKS server cannot connect to identd on the client
							93 = request rejected because the client program and identd report different user-ids
			*/
			
			if((mainPrefs->firewallType == fwSOCKS5 && send[1] == 0)
				|| ((mainPrefs->firewallType == fwSOCKS4A || mainPrefs->firewallType == fwSOCKS4) && send[1] == 90)) //excellent!
			{
				ConnDeSOCKS(conn);
				if(conn->connType == connIRC)
					LinkSuccessfulConnection(conn->link, true);
				else if(conn->connType == connDCC)
					DCCConnOpened(&conn);
			}
			else
			{
				ConstStringPtr sp = GetIntStringPtr(spSOCKS, 10);
				
				if(mainPrefs->firewallType == fwSOCKS5)
				{
					if(send[1]>8)
						send[1] = 9;
					GetIntString(send, spSOCKS, send[1]);
				}
				else// if(mainPrefs->firewallType == fwSOCKS4A || mainPrefs->firewallType == fwSOCKS4)
				{
					if(send[1] > 93 || send[1] < 90)
						send[1] = 9;
					else
						send[1] += 13 - 91;
					GetIntString(send, spSOCKS, send[1]);
				}

				conn->connectStage=csSOCKSConnectFailed;
				if(conn->socksType == connIRC)
				{
					LSParamString(&ls, sp, send, 0, 0, 0);
					SMPrefix(&ls, dsConsole);
					p.link = conn->link;
					p.connectStage = conn->connectStage;
					runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &p);
					ServerOK(kServerOKBadSOCKS, conn->link);
				}
				else if(conn->socksType == connDCC)
				{
					DCCFailed(&conn, send);
				}
			}
		}
	}
	else
	{
		if(conn->socksType == connIRC)
			ServerOK(c->event, conn->link);
		else if(conn->socksType == connDCC)
			dccEvent(c, conn);
	}
}

pascal void processIdentd(CEPtr c, connectionPtr conn)
{
	int i;
	Str255 s;
	long nn;
	LongString ls;
	
	if(c->event==C_CharsAvailable)
	{
		nn = ConnGetUntil(conn, (Ptr)&s[1], '\n', 250);
		while((nn>0) && ((s[nn]==10)||(s[nn]==13)))
			nn--;
		if(nn)
		{
			s[0]=nn;
			for(i=1;i<=nn;i++)
				s[i]=ISODecode[s[i]];
			
			LSGetIntString(&ls, spError, sCIdentdCalled);
			SMPrefixLink(conn->link,&ls, dsConsole);
			
			LSConcatStrAndStrAndStr(s, "\p : USERID : MACOS : ", (StringPtr)conn->refCon, &ls);
			DisposePtr((Ptr)conn->refCon);
			ConnPutLS(&conn, &ls);
			
			conn->link->identConn = 0; //detach this from the link
			if(conn)
			{
				conn->lastData = now; //let the server close it, or close it if it's stale...
				conn->closeTime = now + 30; // Give it 30 seconds.
				ConnStale(conn);
			}
		}
	}
}

pascal void processServer(CEPtr c, connectionPtr conn)
{
	LongString ls;
	long nn, i;
	size_t abytes = c->value;
	linkPtr link = conn->link;
	char cr;
	
	if(c->event==C_CharsAvailable)
	{
		int count = -8;

		conn->lastData=now;
		do
		{
			//read to LF
			nn = ConnGetUntil(conn, (Ptr)&ls.data[1], '\n', MIN(maxLSlen-1, abytes));
			
			abytes -= nn;
			cr = 0;
			//kill CR/LF at end
			while(nn>0 && (ls.data[nn]=='\n' || ls.data[nn]=='\r'))
			{
				cr = 1;
				nn--;
			}

			if(nn>0)
			{
				ls.len=nn;
				for(i=1;i<=nn;i++)
					ls.data[i]=ISODecode[ls.data[i]];

				LSConcatLSAndLS(&link->firstHalfOfIncoming, &ls, &link->firstHalfOfIncoming);
			}
			
			if(cr)
			{
				ServerCommands(&link->firstHalfOfIncoming, link);
				link->firstHalfOfIncoming.len=0;
			}
                /*
                 * abytes > nn is fuzzy math, but a reasonable way
                 * to decide when to stop.
                 * If the number of available bytes is less than the size of the last server message
                 * then stop here.
                 */
		} while(++count && abytes > nn);
	}
	else
		ServerOK(c->event, link);
}

pascal void processPlugin(CEPtr c, connectionPtr conn)
{
	long nn;
	size_t abytes;
	LongString ls;
	pConnectionData p;
	
	p.conn = conn;
	p.event = c->event;
	abytes = c->value;
	p.sendEvent = false;
	
	if(c->event == C_CharsAvailable && conn->textOrBinary)
	{
		conn->lastData=now;
		nn = ConnGetUntil(conn, (Ptr)&ls.data[1], '\n', MIN(maxLSlen-1, abytes));
		ls.len=nn;
		
		if(debugOn)
		{
			LongString tls;
			
			tls.len=2;
			*(short*)&tls.data[1]=0x08FF;
			LSConcatLSAndLS(&tls, &ls, &tls);
			LineMsg(&tls);
		}
		
		p.data=&ls;
		runIndPlugin(conn->pluginRef, pTextConnectionMessage, &p);
	}
	else
	{
		if(conn->textOrBinary) // !C_CharsAvailable
			runIndPlugin(conn->pluginRef, pTextConnectionMessage, &p);
		else //data connection
			runIndPlugin(conn->pluginRef, pDataConnectionMessage, &p);
	}
}

pascal void dnsEvent(CEPtr c, connectionPtr conn)
{
	Str255 s2;
	LongString ls;
	struct in_addr ip;
	Str255 name;
	pDNSLookupData p;
	DNSLookupDataPtr dldp;
	
	switch(c->event)
	{
		case C_Found:
			p.successful=1;
			memcpy(&ip, &c->addr, sizeof(ip));
			pstrcpy(*(StringHandle)(c->value), name);
			DisposeHandle((Handle)c->value);
			*(long*)&c->value = 0; //const-cast
			break;
		case C_SearchFailed:
			p.successful=0;
			break;
	}
	
	if(conn->pluginRef) // then it's a plugin //conn->refCon && (dldp->magic!='SIRC')) //saving data
	{
		p.refCon=(long)conn->refCon;
		memcpy(&p.ip, &ip, sizeof(p.ip));
		p.nameToIP=c->event<3;
		p.search=conn->name;
		if(p.successful)
		{
			if(p.nameToIP)
			{
				inet_ntoa_str(ip, s2);
				p.reply=s2;
			}
			else
				p.reply=name;
		}
		else
		{
			s2[0]=0;
			p.reply=s2;
		}
		if(conn->pluginRef)
			runIndPlugin(conn->pluginRef, pDNSLookupMessage, &p);
	}
	else
	{
		dldp=(DNSLookupDataPtr)conn->refCon;
		if(conn->refCon && (dldp->magic=='SIRC'))
		{
			LSStrCat(5, &ls, "\pDNS ", dldp->nick, "\p (", dldp->host, "\p): ");
		}
		else
			LSConcatStrAndStrAndStr("\pDNS ", conn->name, "\p: ", &ls);
		if(!p.successful)
			LSConcatLSAndStr(&ls, "\pfailed", &ls);
		else
		{
			if(conn->connType==connDNSIP)
			{
				inet_ntoa_str(ip, s2);
				LSConcatLSAndStr(&ls,s2,&ls);
			}
			else
				LSConcatLSAndStr(&ls,name,&ls);
		}
		SMPrefix(&ls, dsFrontWin);
	}
	deleteConnection(&conn);
}

pascal void processStale(CEPtr c, connectionPtr conn)
{
	LongString ls;
	Str255 s;
	
	if(!c || c->event == C_Closed)
	{
		if(conn->realConnType == connIDENTD)
		{
			LSGetIntString(&ls, spError, sCIdentdClosed);
			SMPrefixLink(conn->link, &ls, dsConsole);
			deleteConnection(&conn);
		}
		else if(conn->realConnType == connIRC)
		{
			if(debugOn)
			{
				LSConcatStrAndStr("\p¥Completed close of link ", conn->link->linkPrefs->linkName, &ls);
				LineMsg(&ls);
			}
			deleteConnection(&conn);
		}
		else //
		{
			NumToString(conn->realConnType, s);
			LSConcatStrAndStr("\p¥Bad stale connection: ", s, &ls);
			LineMsg(&ls);
		}
	}
	else
	{
		long nn, read;
		long fm = FreeMem();
		Ptr getbuf;
		Str255 s2;
		
		if(c->event==C_CharsAvailable)
		{
			nn = c->value;
			do {
				if(!nn)
					return;
				if(nn > fm - 10000)
					nn = fm-10000;
				getbuf = NewPtr(nn);
				read = ConnGetData(conn, getbuf, nn);
				nn -= read;
				DisposePtr(getbuf);
				if(debugOn)
				{
					NumToString(conn->realConnType, s);
					NumToString(read, s2);
					SAppend1(s2, ' ');
					LSConcatStrAndStrAndStr("\p¥Data received for stale connection: ", s2, s, &ls);
					LineMsg(&ls);
				}
			} while(1);
		}
		else
		{
			NumToString(conn->realConnType, s);
			NumToString(c->event, s2);
			LSConcatStrAndStrAndStr("\p¥unknown event for stale connection: ", s2, s, &ls);
		}
	}
}