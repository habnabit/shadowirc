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

#include "LongStrings.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "connections.h"
#include "MsgBuffer.h"
#include "MsgWindows.h"
#include "WindowList.h"
#include "connections.h"
#include "channels.h"
#include "inputline.h"
#include "IRCIgnore.h"
#include "IRCChannels.h"
#include "plugins.h"
#include "IRCCommands.h"
#include "utils.h"
#include "Inline.h"
#include "Floaters.h"
#include "CTCP.h"
#include "IRCNComm.h"
#include "TextManip.h"

inline pascal void checkLimitKey(linkPtr link, char mode, channelPtr curChannel, StringPtr modeChange);
inline pascal void checkOpsBans(linkPtr link, char up, char voice, StringPtr modeChange, channelPtr curChannel, ConstStringPtr setBy);
pascal void doMODE(linkPtr link, ConstStringPtr channel, StringPtr modeChange, ConstStringPtr setBy, LongString *tls);

static pascal void nNotice(linkPtr link, LongString *ls, StringPtr fromuser, StringPtr target, StringPtr from);
static pascal void nPrivmsg(linkPtr link, LongString *ls, StringPtr fromuser, StringPtr target, StringPtr from);
static pascal void nJoin(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser);
static pascal void nPart(linkPtr link, LongString *target, StringPtr from, StringPtr fromuser, StringPtr targ);
static pascal void nQuit(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target);
static pascal void nTopic(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target);
static pascal void nNick(linkPtr link, LongString *newNick, StringPtr from, StringPtr fromuser);
static pascal void nKill(linkPtr link, LongString *ls, StringPtr from);
static pascal void nMode(linkPtr link, LongString *ls, StringPtr from, StringPtr target);
static pascal void nKick(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target);
static pascal void nInvite(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser);
static pascal void nRPong(linkPtr link, LongString *ls);
static pascal void nSilence(linkPtr link, LongString *ls);
static pascal void nPing(linkPtr link, LongString *ls);
static pascal void nWallops(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target);
static pascal void nError(linkPtr link, LongString *ls, StringPtr target);
pascal void ServerCommands(LongString *ls, linkPtr link);

static pascal void AttemptNickRegainSignoff(linkPtr link, ConstStringPtr signedOff)
{
	LongString ls;
	
	if(link->linkPrefs->regainNick && !pstrcasecmp(link->CurrentNick, link->linkPrefs->nick)) //enabled and I'm not the main nick.
	{
		if(pstrcasecmp(signedOff, link->linkPrefs->nick)) //main nick signed off
		{
			LSConcatStrAndStr("\pNICK ", link->linkPrefs->nick, &ls);
			SendCommand(link, &ls);
		}
		else if(pstrcasecmp(signedOff, link->linkPrefs->alterNick) && !pstrcasecmp(link->CurrentNick, link->linkPrefs->nick) && !pstrcasecmp(link->CurrentNick, link->linkPrefs->alterNick))
		{
			//alter nick signed off and my nick isn't the main nick or the alter nick
			LSConcatStrAndStr("\pNICK ", link->linkPrefs->alterNick, &ls);
			SendCommand(link, &ls);
		}
	}
}

static pascal void nNotice(linkPtr link, LongString *ls, StringPtr fromuser, StringPtr target, StringPtr from)
{
	Str255 dispFrom;
	pServerNOTICEDataRec p;
	MWPtr mw;
	char 	ign=IsIgnoredNickUser(from, fromuser);

	if(!doCTCP(from, fromuser, target, ls, true, ign, link))
	{
		if(!from[0] || pstrcmp(from, link->CurrentServer))
			p.fromServer=1;
		else
		{
			p.fromServer = !link->CurrentServer[0];
			if(p.fromServer)
				pstrcmp(from, link->CurrentServer);
		}
		
		p.fromServer= !from[0] || pos('.', from)>0;
		p.link=link;
		p.username=from;
		p.userhost=fromuser;
		p.target=target;
		p.message=ls;
		p.ignored=ign;
		p.targOther=0;
		p.autoQuery=0;
		p.dontSound = false;
		p.displayUserHost = mainPrefs->showUserHostsWithMsgs && !p.fromServer;
		p.redirect = 0;
		
		if((p.targChan = ((p.chan=ChFind(target, link))!=0)) == 0)
		{
			if((p.targMe = pstrcasecmp(target, link->CurrentNick)) == 0)
				p.targOther=1;
		}
		else
			p.targMe = 0;
		
		p.toConsole = p.fromServer && mainPrefs->serverNoticesToConsole;
		
		runPlugins(pServerNOTICEMessage, &p);
		if(!p.ignored)
		{
			if(p.fromServer)
				dispFrom[0] = 0;
			else
			{
				pstrcpy(from, dispFrom);
				if(p.displayUserHost)
				{
					SAppend1(dispFrom, '[');
					pstrcat(dispFrom, fromuser, dispFrom);
					SAppend1(dispFrom, ']');
				}
			}
			
			FormatMessage(dispFrom, ls, 0, 0, kNickNotice, ls);
			
			if(p.toConsole)
			{
				SMLink(link, ls);
				MWColor(ls, sicNotice);
				if(p.redirect)
					MWMessage(p.redirect, ls);
				else
					LineMsg(ls);
			}
			else if(p.targChan)
			{
				MWColor(ls, sicNotice);
				if(p.redirect)
					MWMessage(p.redirect, ls);
				else
					ChMsg(p.chan, ls);
			}
			else
			{
				MWColor(ls, sicNotice);
				mw=ChannelWindow(link, from);
				if(mw->link != link)
					SMLink(link, ls)	;
				if(p.redirect)
					MWMessage(p.redirect, ls);
				else
					MWMessage(mw, ls);
			}
			
			if(!p.dontSound)
				SoundService(sndNotice, p.fromServer);
		}
	}
}

static pascal void nPrivmsg(linkPtr link, LongString *ls, StringPtr fromuser, StringPtr target, StringPtr from)
{
	Str255 dispFrom;
	pServerPRIVMSGDataRec p;
	MWPtr mw;
	unsigned char p_nickStyle[10];
	unsigned char p_textStyle[10];
	char 	ign;
	
	ign = IsIgnoredNickUser(from, fromuser);
	if(!doCTCP(from, fromuser, target, ls, false, ign, link))
	{
		p.fromServer=pos('.', from)>0;
		p.link=link;
		p.username=from;
		p.userhost=fromuser;
		p.target=target;
		p.message=ls;
		p.ignored=ign;
		p.targOther =
		p.autoQuery = 0;
		p.redirect = 0;
		p.nickStyleP = p_nickStyle;
		p.nickStyleP = p_nickStyle;
		
		if(!(p.targChan = (p.chan = ChFind(target, link)) != 0)) //not channel
		{
			if((p.targMe = pstrcasecmp(target, link->CurrentNick)) != 0)
			{
				if(mainPrefs->autoQuery && mainPrefs->autoQueryOpen != aqOutgoing)
					p.autoQuery = 1;

				*(long*)p_nickStyle = *(long*)p_textStyle = 0x030F0802; //msg
			}
			else
			{
				p.targOther=1;
				*(short*)p_nickStyle = *(short*)p_textStyle = 0x010E; //normal
			}
		}
		else //channel
		{
			p.targMe = 0;
			
			*(short*)p_nickStyle = *(short*)p_textStyle = 0x010E; //normal
		}
		
		p.displayUserHost = p.targMe && mainPrefs->showUserHostsWithMsgs;
		p.dontSound = false;
		
		runPlugins(pServerPRIVMSGMessage, &p);
		if(!p.ignored)
		{
			pstrcpy(from, dispFrom);
			if(p.displayUserHost)
			{
				SAppend1(dispFrom, '[');
				pstrcat(dispFrom, fromuser, dispFrom);
				SAppend1(dispFrom, ']');
			}
			
			if(p.targChan)
			{
				FormatMessage(dispFrom, ls, p_nickStyle, p_textStyle, kNickNormal, ls);
				
				if(p.redirect)
					MWMessage(p.redirect, ls);
				else
					ChMsg(p.chan, ls);
				if(!p.dontSound)
					SoundService(sndChanMsg, 0);
			}
			else if(p.targMe)
			{
				FormatMessage(dispFrom, ls, p_nickStyle, p_textStyle, kNickPrivmsg, ls);
				
				MBNewMessage(link, from);

				if(p.autoQuery && FreeMem() > 16000) //don't open if < 16k free
					mw = DoJoinQuery(from, link);
				else
				{
					mw=ChannelWindow(link, from);
					if(mw->link != link)
						SMLink(link, ls);
				}
				
				if(p.redirect)
					MWMessage(p.redirect, ls);
				else
					MWMessage(mw, ls);
				if(!p.dontSound)
				{
					if(FindService(soundServiceType))
						SoundService(sndPrivMsg, 0);
					else
						if(mainPrefs->beepOnPrivmsgs)
							SysBeep(0);
				}
			}
			else if(p.targOther)
			{
				FormatMessage(dispFrom, ls, p_nickStyle, p_textStyle, kNickOther, ls);
				SMLink(link, ls);
				MWColor(ls, sicPrivmsg);
				if(p.redirect)
					MWMessage(p.redirect, ls);
				else
					Message(ls);
				if(!p.dontSound)
					SoundService(sndMiscMsg, 0);
			}
		}
	}
}

static pascal void nJoin(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser)
{
	MWPtr dd;
	pServerJOINDataRec p;
	StringPtr chName = ls->data;
	
	LSMakeStr(*ls);
	
	p.link=link;
	p.channelName=chName;
	p.username=from;
	p.userhost=fromuser;
	p.dontDisplay = !mainPrefs->displayJoin;
	p.dontSound = false;
	
	if((p.isMe = pstrcasecmp(from, link->CurrentNick)) != 0)
	{
		p.channel=ChCreate(chName, link);
		if(p.channel)
		{
			dd=ChJoin(p.channel);
			LSConcatStrAndStr("\pMODE ", p.channel->chName, ls); //modes
			SendCommand(link, ls);
			LSAppend2(*ls, ' b');
			SendCommand(link, ls); //banlist
			
/*--causes problems on some servers. Disable for now.
			if(LinkCanMode(link, 'e', false))
			{
				ls->data[ls->len] = 'e'; //change 'b' to 'e'
				SendCommand(link, ls); //Get exception list
			}
*/
			DrawMWinStatus(dd);
		}
		ChannelListAdd(link, p.channel->chName);
	}
	else
		p.channel=ChFind(chName, link);
	
	p.userPtr=ULAddUser(p.channel, from, fromuser);
	runPlugins(pServerJOINMessage, &p);

	if(!p.isMe && !p.dontDisplay)
	{
		LSParamString(ls, GetIntStringPtr(spServer, sNickJoinedChannel), from, fromuser, p.channel->chName, 0);
		
		SMPrefix(ls, dsNowhere);
		ChMsg(p.channel, ls);
	}

	pstrcpy(from, p.channel->lastJoiner);
	
	if(!p.dontSound)
		SoundService(sndJoin, p.isMe);
}

static pascal void nPart(linkPtr link, LongString *target, StringPtr from, StringPtr fromuser, StringPtr targ)
{
	Str255 channel;
	pServerPARTDataRec p;
	LongString tls;
	
	if(targ[0]) //then we had a reason!
		pstrcpy(targ, channel);
	else
		LSCopyString(target, 1, 255, channel);

	p.channel=ChFind(channel, link);
	p.isMe=pstrcasecmp(from, link->CurrentNick);
	p.link=link;
	p.username=from;
	p.userhost=fromuser;
	p.channelName=channel;
	p.partReason=target;
	p.userPtr=ULFindUserName(p.channel, from);
	p.dontDisplay = !mainPrefs->displayPart;
	p.dontSound = false;

	runPlugins(pServerPARTMessage, &p);

	if(p.isMe)
		WHide(p.channel->window->w);
	
	if(!p.dontDisplay)
	{
		LSConcatStrAndStrAndStr(from, "\p has left ", channel, &tls);
		if(targ[0])
		{
			LSAppend2(tls, ' (');
			LSAppend2(tls, 0x08FA); //save text style
			LSConcatLSAndLS(&tls, target, &tls);
			LSAppend3(tls, 0x08FB2900); //restore style+ )
		}

		SMPrefix(&tls, dsNowhere);
		ChMsg(p.channel, &tls);
	}

	if(!p.dontSound)
		SoundService(sndPart, p.isMe);

	if(p.isMe)
		ChPart(p.channel->window);
	else
		ULDeleteUser(p.userPtr);
}

static pascal void nQuit(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target)
{
	LongString signoffMessage;
	UserListPtr ul;
	channelPtr x;
	pServerQUITDataRec p;
	
	if(target[0])
	{
		if(target[1]==':')
			pdelete(target, 1, 1);
		SAppend1(target, ' ');
	}
	LSConcatStrAndLS(target, ls, &signoffMessage);
	
	p.link=link;
	p.username=from;
	p.userhost=fromuser;
	p.signoffMessage=&signoffMessage;
	p.isMe=0;
	p.dontDisplay = !mainPrefs->displayQuit;
	p.dontSound = false;
	runPlugins(pServerQUITMessage, &p);
	
	LSConcatStrAndStr(from, "\p has signed off (", ls);
	LSAppend2(*ls, 0x08FA); //save text style
	LSConcatLSAndLS(ls, &signoffMessage, ls);
	LSAppend3(*ls, 0x08FB2900); //restore style+ )
	SMPrefix(ls, dsNowhere);
	
	if(!p.dontSound)
		SoundService(sndQuit, 0);
	
	linkfor(x, link->channelList)
		if((ul=ULFindUserName(x, from)) != 0)
		{
			ULDeleteUser(ul);
			if(!p.dontDisplay)
				ChMsg(x, ls);
		}
	
	AttemptNickRegainSignoff(link, from);
}

static pascal void nTopic(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target)
{
	pServerTOPICDataRec p;
	LongString tls;
	
	p.channel=ChFind(target, link);

	p.link=link;
	p.username = from;
	p.userhost = fromuser;
	p.channelName = target;
	p.newtopic = ls;
	p.isMe = pstrcmp(from, link->CurrentNick);
	p.dontDisplay = false;
	p.dontSound = false;
	runPlugins(pServerTOPICMessage, &p);
	
	if(!p.dontDisplay)
	{
		LSParamString(&tls, GetIntStringPtr(spInfo, sSetTopicOn), from, target, 0, 0);
		SMPrefix(&tls, dsNowhere);
		LSConcatLSAndLS(&tls, ls, &tls);
		
		if(p.channel)
			ChMsg(p.channel, &tls);
		else
			ChannelMsg(link, target, &tls);
	}

	if(p.channel)
	{
		LSCopyString(ls, 1, 255, p.channel->topic);
		pstrcpy(from, p.channel->topicSetBy);

		DrawMWinStatus(p.channel->window);
	}
		
	if(!p.dontSound)
		SoundService(sndTopic, 0);
}

static pascal void nNick(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser)
{
	pServerNICKDataRec p;
	channelPtr x;
	UserListPtr ul;
	Str63 newNickStr;
	MWPtr mw;
	
	LSCopyString(ls, 1, 63, newNickStr);
	
	if((p.isMe = pstrcasecmp(from, link->CurrentNick)) != 0)
	{
		pstrcpy(newNickStr, link->CurrentNick);
		NickListAdd(link, newNickStr);
		UpdateStatusLine();
		DrawMWinStatus(consoleWin);
	}
	
	p.link=link;
	p.username=from;
	p.userhost=fromuser;
	p.newnick=newNickStr;
	
	p.dontDisplay = !(mainPrefs->displayNicks || p.isMe);
	p.dontSound= false;
	
	mw = MWFindQuery(from, link);
	if(mw)
		MWSetNameQuery(mw, from, link);

	runPlugins(pServerNICKMessage, &p);

	LSParamString(ls, GetIntStringPtr(spInfo, sIsNowKnownAs), from, newNickStr, 0, 0);
	SMPrefix(ls, dsNowhere);

	linkfor(x, link->channelList)
		if((ul=ULFindUserName(x, from)) != 0)
		{
			pstrcpy(newNickStr, ul->nick);
			if(!p.dontDisplay)
				ChMsg(x, ls);
		}
	
	runIndService(userlistServiceClass, pServiceULNickChange, &p);
	if(p.isMe)
		LineMsg(ls);
	
	if(!p.dontSound)
		SoundService(sndNick, p.isMe);
	
	if(!p.isMe)
		AttemptNickRegainSignoff(link, from);
}

static pascal void nKill(linkPtr link, LongString *ls, StringPtr from)
{
	LongString tls;
	DialogPtr d;
	short i;
	
	SoundService(sndKill, 0);
	LSParamString(&tls, GetIntStringPtr(spInfo, sRejected), from, 0, 0, 0);
	LSConcatLSAndLS(&tls, ls, &tls);

	//pop up alert to alert the user
	d=GetNewDialog(260, 0, (WindowPtr)-1);
	SetDlogItemTextHdl(d, 2, (char*)&tls.data[1], tls.len);

	SMPrefixLink(link, &tls, dsConsole);
	Message(&tls);
	link->neverConnected=true; //don't try reconnecting
	
	SetupModalDialog(d, 1, 1);
	do {
		ModalDialog(StdDlgFilter, &i);
	} while(i!=1);
	DisposeDialog(d);
	FinishModalDialog();
}

inline pascal void checkLimitKey(linkPtr link, char mode, channelPtr curChannel, StringPtr modeChange)
{
	short p, p2, p3;
	long l;
	Str255 limStr;
	pServerMODEData pUL;

	p=pos(' ', modeChange);
	modeChange[p]='&';
	p2=pos(' ', modeChange);
	if(!p2)
		p3 = p2=modeChange[0];
	else
	{
		p3 = p2;
		p2--;
	}
	
	myCopy(modeChange, p+1, p2, limStr);
	pdelete(modeChange, p, p3-p);

	pUL.done = 0;
	pUL.ch=curChannel;
	if(mode == 'k')
		pUL.data=(long)limStr;
	else
	{
		StringToNum(limStr, &l);
		pUL.data=l;
	}
	pUL.up=true;
	pUL.link=link;
	pUL.mode = mode;
	runPlugins(pServerMODEMessage, &pUL);
	
	if(mode == 'l')
		curChannel->limit = l;
	else
		pstrcpy(limStr, curChannel->key);
}

inline pascal void checkOpsBans(linkPtr link, char up, char mode, StringPtr modeChange, channelPtr curChannel, ConstStringPtr setBy)
{
	short p, p2, p3;
	Str255 opString;
	pServerMODEData pUL;
	
	p=pos(' ', modeChange);
	modeChange[p]='&';
	p2=pos(' ', modeChange);
	if(!p2)
		p3 = p2=modeChange[0];
	else
		p3 = p2--;
	
	myCopy(modeChange, p+1, p2, opString);
	if(mode == 'b' || mode == 'e')
	{
		bansP bp;
		char e;
		
		e = mode == 'e';
		if(up)
		{
			ChGetBan(curChannel, opString, &bp, e);
			if(bp)
				pstrcpy(setBy, bp->setBy);
		}
		else
		{
			bp=ChFindBan(curChannel, opString, e);
			if(bp)
				ChKillBan(curChannel, &bp, e);
		}
		
		pUL.data = (long)opString;
	}
	else
	{
		UserListPtr ul;
		
		if(pstrcasecmp(opString, link->CurrentNick)) //then me
		{
			if(mode == 'v')
				curChannel->hasVoice=up;
			else if(mode == 'o')
				curChannel->hasOps=up;
			else if(mode == 'h')
				curChannel->hasHalfOps = up;
		}
		
		ul=ULFindUserName(curChannel, opString);
		if(ul)
		{
			if(mode == 'v')
				ul->hasVoice=up;
			else if(mode == 'o')
				ul->isOp=up;
			else if(mode == 'h')
				ul->isHalfOp = up;
		}
		
		pUL.data=(long)ul;
	}
	
	pdelete(modeChange, p, p3-p);
	
	pUL.done = 0;
	pUL.link=link;
	pUL.ch=curChannel;
	pUL.up=up;
	pUL.mode = mode;
	runPlugins(pServerMODEMessage, &pUL);
}

pascal void doMODE(linkPtr link, ConstStringPtr channel, StringPtr modeChange, ConstStringPtr setBy, LongString *tls)
{
	int x, y;
	short len;
	char up;
	channelPtr ch;
	pServerMODEData pUL;
	char sendUL;
	long numModes;
	Str255 fullmode;
	
	pstrcpy(modeChange, fullmode);
	pUL.dontDisplay = !mainPrefs->displayModes;
	pUL.dontSound = false;
	ch=ChFind(channel, link);
	if(!ch) //umode
	{
		//updateUmodes2
		up = 1;
		for(x=1; x<= modeChange[0];x++)
		{
			if(modeChange[x] == '+')
				up = 1;
			else if(modeChange[x] == '-')
				up = 0;
			else
			{
				for(y=1;y<=link->umodes[0];y++)
					if(modeChange[x] == link->umodes[y])
					{
						if(up)
							link->yourUmodes[y] = link->umodes[y];
						else
							link->yourUmodes[y] = 0;
						break;
					}
			}
		}
		UpdateStatusLine();
		//end
		
		pUL.link=link;
		pUL.ch=0;
		pUL.up=0;
		pUL.mode='¥';
		pUL.data=(long)modeChange;
		pUL.done=1;
		runPlugins(pServerMODEMessage, &pUL);
		if(!pUL.dontSound)
			SoundService(sndUMode, 0);
		goto display;
	}
	
	len=pos(' ', modeChange)-1; //find the space, and go back
	if(len==-1) //fucking global stage
		len=modeChange[0];

	pUL.link=link;
	pUL.ch=ch;
	pUL.done=0;
	numModes = 0;
	for(x=1;x<=len;x++)
	{
		sendUL=1;
		numModes++;
		
		switch(modeChange[x])
		{
			case '+':
				up=1;
				sendUL=0;
				numModes--;
				break;
			case '-':
				up=0;
				sendUL=0;
				numModes--;
				break;
			case 't':
				ch->modes[modeT]=up;
				break;
			case 'n':
				ch->modes[modeN]=up;
				break;
			case 'i':
				ch->modes[modeI]=up;
				break;
			case 'm':
				ch->modes[modeM]=up;
				break;
			case 'p':
				ch->modes[modeP]=up;
				break;
			case 's':
				ch->modes[modeS]=up;
				break;
			case 'l':
				ch->modes[modeL] = up;
				
			if(0) //skip the next instr without a goto
			case 'k':
				ch->modes[modeK] = up;

				if(up)
				{
					checkLimitKey(link, modeChange[x], ch, modeChange);
					sendUL=0;
				}
				break;
			case 'b':
			case 'o':
			case 'v':
			case 'h':
			case 'e':
				checkOpsBans(link, up, modeChange[x], modeChange, ch, setBy);
				sendUL=0;
				break;
		}
		
		if(sendUL)
		{
			pUL.up=up;
			pUL.mode=modeChange[x];
			runPlugins(pServerMODEMessage, &pUL);
		}
	}
	
	pUL.data = (long)fullmode;
	pUL.done=1;
	runPlugins(pServerMODEMessage, &pUL);

	if(ch)
		DrawMWinStatus(ch->window);
	
	if(!pUL.dontSound)
		SoundService(sndMode, numModes);
	
	display:
	if(tls && !pUL.dontDisplay)
	{
		if(ch)
		{
			SMPrefix(tls, dsNowhere);
			ChMsg(ch, tls);
		}
		else
		{
			SMPrefixLink(link, tls, dsNowhere);
			ChannelMsg(link, channel, tls);
		}
	}
}

static pascal void nMode(linkPtr link, LongString *ls, StringPtr from, StringPtr target)
{
	LongString tls;
	
	LSMakeStr(*ls);
	LSParamString(&tls, GetIntStringPtr(spServer, sModeChange), ls->data, target, from, 0);

	doMODE(link, target, ls->data, from, &tls);
}

static pascal void nKick(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target)
{
	Str63 kickedNick;
	pServerKICKDataRec p;
	channelPtr ch;

	LSNextArgIRC(ls, kickedNick); //Nicks should never be longer than 63 chars...if they are, we die

	p.isMe=(pstrcasecmp(kickedNick, link->CurrentNick));
	p.channel=ch=ChFind(target, link);
	
	p.link=link;
	p.username=from;
	p.userhost=fromuser;
	p.channelName=target;
	p.kickedNick=kickedNick;
	p.message=ls;
	p.autoRejoin=mainPrefs->autoRejoinAfterKick;
	p.dontDisplay = !mainPrefs->displayKicks;
	p.dontSound = false;
	runPlugins(pServerKICKMessage, &p);
	
	if(!p.dontDisplay)
	{
		LongString tls;

		LSParamString(&tls, GetIntStringPtr(spInfo, sKickedOffBy), kickedNick, target, from, 0);
		LSAppend4(tls, 0x202808FA); //' (' + save text style
		LSConcatLSAndLS(&tls, ls, &tls);
		LSAppend3(tls, 0x08FB2900); //reset color and close paren
		SMPrefix(&tls, dsNowhere);
		ChMsg(ch, &tls);
	}

	if(p.isMe)
	{
		MWQuote(ch->window);
		if(ch->window == CurrentTarget.mw)
		{
			InvalTarget(&CurrentTarget);
			UpdateStatusLine();
		}
		if(p.autoRejoin)
		{
			LSConcatStrAndStr("\pJOIN ", target, ls);
			if(ch->key[0])
			{
				LSAppend1(*ls, ' ');
				LSConcatLSAndStr(ls, ch->key, ls);
			}
			SendCommand(link, ls);
		}
		ChDestroy(ch);
	}
	else
		ULDeleteUser(ULFindUserName(ch, kickedNick));
	
	if(!p.dontSound)
		SoundService(sndKick, p.isMe);
}

static pascal void nInvite(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser)
{
	pServerINVITEDataRec p;
	LongString tls;
	Str255 s;
	
	pstrcpy(from, s);
	SAppend1(s, '!');
	pstrcat(s, fromuser, s);
	
	p.ignored = IsIgnored(s);
	p.link=link;
	p.autojoin= !p.ignored && (mainPrefs->autoacceptInvites && (!link->isAway || !mainPrefs->autoacceptInvites));
	p.username=from;
	p.userhost=fromuser;
	LSMakeStr(*ls);
	p.channel=ls->data;
	p.dontDisplay = !mainPrefs->displayInvites;
	p.dontSound = false;
	
	runPlugins(pServerINVITEMessage, &p);

	if(!p.dontDisplay)
	{
		LSMakeStr(*ls);
		LSParamString(&tls, GetIntStringPtr(spServer, sInvitesYouToChannel), from, ls->data, 0, 0);
		SMPrefixLink(link, &tls, dsFrontWin);
	}
	
	if(p.autojoin)
	{
		LSConcatStrAndStr("\pJOIN ", ls->data, &tls);
		SendCommand(link, &tls);
	}
	
	if(!p.ignored && !p.dontSound)
		SoundService(sndInvite, 0);
}

static pascal void nRPong(linkPtr link, LongString *ls)
{
	Str255 server, num, st, st2;
	unsigned long l1, l2, lo;
	pServerRPONGData p;
	unsigned long long t1, td;
	
	LSNextArg(ls, server);
	LSNextArgIRC(ls, num);
	LSNextArg(ls, st);

	Microseconds((UnsignedWide*)&td);
	td/=1000;
	ulong64val(st, &t1);
	td -= t1;
	
	lo = td;
	p.pingTime = lo;
	l2= lo%1000;
	l1=(lo-l2)/1000;
	if(l1<0)
		l1=0;
	
	NumToString(l1, st);
	SAppend1(st, '.');
	NumToString(l2, st2);
	padBegin(st2, 3, st2);
	if(st2[1]==' ')
		st2[1]='0';
	if(st2[2]==' ')
		st2[2]='0';
	
	p.link=link;
	p.server=server;
	p.num=num;
	p.dontDisplay=0;
	p.dontSound = false;
	
	runPlugins(pServerRPONGMessage, &p);
	if(!p. dontDisplay)
	{
		LSParamString(ls, GetIntStringPtr(spServer, sRPINGReply), server, st, st2, 0);
		if(l1==1 && l2 == 0)
			ls->len--;
		SMPrefixLink(link, ls, dsFrontWin);
	}
	if(!p.dontSound)
		SoundService(sndRPong, p.pingTime);
}

static pascal void nSilence(linkPtr link, LongString *ls)
{
	pServerSILENCEData p;
	Str255 silence;
	
	p.link=link;
	p.add=ls->data[1]=='+';
	p.mask=silence;
	p.completelyProcessed=0;
	
	LSCopyString(ls, 2, 255, silence);
	
	runPlugins(pServerSILENCEMessage, &p);
	if(!p.completelyProcessed)
	{
		LSParamString(ls, GetIntStringPtr(spInfo, sAddedToSilence + (!p.add)), silence, 0, 0, 0);
			SMPrefixLink(link, ls, dsFrontWin);
	}
	
	SoundService(sndSilence, 0);
}

static pascal void nPing(linkPtr link, LongString *ls)
{
	LongString tls;
	pServerPINGDataRec p;
	
	LSConcatStrAndLS("\pPONG ", ls, &tls);
	SendCommand(link, &tls);

	if(mainPrefs->displayPingsInConsole)
	{
		LSConcatStrAndLS("\pPING ", ls, &tls);
		SMPrefix(&tls, dsConsole);
	}
	p.link=link;
	p.source=ls;
	runPlugins(pServerPINGMessage, &p);
	SoundService(sndServerPing, 0);
}

static pascal void nPong(linkPtr link, StringPtr from)
{
	LongString tls;
	pServerPONGDataRec p;
	
	LSParamString(&tls, GetIntStringPtr(spServer, sGotPongFor), from, 0, 0, 0);
	SMPrefixLink(link, &tls, dsConsole);
	p.link=link;
	p.source=from;
	runPlugins(pServerPONGMessage, &p);
}

static pascal void nWallops(linkPtr link, LongString *ls, StringPtr from, StringPtr fromuser, StringPtr target)
{
	LongString tls;
	pServerWALLOPSDataRec p;

	LSStrLS(target, &tls);
	LSAppend1(tls, ' ');
	LSConcatLSAndLS(&tls, ls, &tls);

	p.link=link;
	p.username=from;
	p.userhost=fromuser;
	p.message=&tls;
	p.ignore=0;
	runPlugins(pServerWALLOPSMessage, &p);
	
	if(!p.ignore)
	{
		LSConcatStrAndStr("\pWALLOPS !", from, ls);
		LSAppend2(*ls, '! ');
		LSConcatLSAndLS(ls, &tls, ls);
		SMPrefixLink(link, ls, (mainPrefs->wallopsToConsole?dsConsole:dsFrontWin));
		SoundService(sndWallops, 0);
	}
}

static pascal void nError(linkPtr link, LongString *ls, StringPtr target)
{
	LongString tls;
	pServerERRORDataRec p;
	
	LSConcatStrAndStr("\pERROR: ", target, &tls);
	LSAppend1(tls, ' ');
	LSConcatLSAndLS(&tls, ls, &tls);
	SMPrefixLink(link, &tls, dsConsole);
	
	p.link=link;
	p.message=ls;
	runPlugins(pServerERRORMessage, &p);
}

pascal void ServerCommands(LongString *ls, linkPtr link)
{
	Str255 target, fromuser;
	Str255 from, comm;
	short i;
	LongString tls;
	
	if(debugOn)
	{
		tls.len=2;
		*(short*)&tls.data[1]=0x08FF;
		LSConcatLSAndLS(&tls, ls, &tls);
		LineMsg(&tls);
	}
	
	if(ls->data[1]==':')
	{
		i=LSPosChar(' ', ls);
		if(!i) //gibberish
		{	
			LSStrLS("\pERROR: Strange data server (no space):", &tls);
			LineMsg(&tls);
			LineMsg(ls);
			return;
		}
		else
		{
			LSCopyString(ls, 2, i-2, fromuser);
			LSDelete(ls, 1, i);
		}
	}
	else
		fromuser[0]=0;
	
	LSNextArgIRC(ls, comm);
	
	i=LSPosChar(' ', ls);
	if(!i)
		target[0]=0;
	else
	{
		LSCopyString(ls, 1, i-1, target);
		LSDelete(ls, 1, i);
	}
	
	i=pos('!', fromuser);
	if(i)
	{
		myCopy(fromuser, 1, i-1, from);
		pdelete(fromuser, 1, i);
	}
	else
	{
		pstrcpy(fromuser, from);
		fromuser[0]=0;
	}
	
	if(ls->data[1]==':')
		LSDelete(ls, 1, 1);
	
	if(isNumber(comm))
	{
		if(!NumericComm(comm[1]*100 + comm[2]*10 + comm[3] - 5328, from, target, ls, link))
			goto unprocessed;
	}
	else if(pstrcmpX(comm, 6, 'NOT'))
		nNotice(link, ls, fromuser, target, from);
	else if(pstrcmpX(comm, 7, 'PRI'))
		nPrivmsg(link, ls, fromuser, target, from);
	else if(pstrcmpX(comm, 4, 'JOI'))
		nJoin(link, ls, from,fromuser);
	else if(pstrcmpX(comm, 4, 'PAR'))
		nPart(link, ls, from,fromuser, target);
	else if(pstrcmpX(comm, 4, 'MOD')) //MODE
		nMode(link, ls, from, target);
	else if(pstrcmpX(comm, 5, 'TOP'))
		nTopic(link, ls, from,fromuser, target);
	else if(pstrcmpX(comm, 4, 'NIC'))
		nNick(link, ls, from,fromuser);
	else if(pstrcmpX(comm, 4, 'QUI'))
		nQuit(link, ls, from,fromuser, target);
	else if(pstrcmpX(comm, 4, 'PIN')) //PING
		nPing(link, ls);
	else if(pstrcmpX(comm, 4, 'PON')) //PONG
		nPong(link, from);
	else if(pstrcmpX(comm, 4, 'KIL'))
		nKill(link, ls,from);
	else if(pstrcmpX(comm, 4, 'KIC'))
		nKick(link, ls, from,fromuser, target);
	else if(pstrcmpX(comm, 6, 'INV'))
		nInvite(link, ls, from,fromuser);
	else if(pstrcmpX(comm, 5, 'RPO'))
		nRPong(link, ls);
	else if(pstrcmpX(comm, 7, 'SIL'))
		nSilence(link, ls);
	else if(pstrcmpX(comm, 7, 'WAL')) //WALLOPS
		nWallops(link, ls, from, fromuser, target);
	else if(pstrcmpX(comm, 5, 'ERR') || pstrcmpX(comm, 6, 'ERR')) //ERROR
		nError(link, ls, target);
	else
	{
unprocessed:
		LSStrCat(6, &tls, "\p/",comm,"\p/", from, "\p/", target);
		LSAppend1(tls, '/');
		LSConcatLSAndLS(&tls, ls, &tls);
		LineMsg(&tls);
	}
}