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

#include "LongStrings.h"
#include "StringList.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "connections.h"
#include "channels.h"
#include "IRCChannels.h"
#include "IRCAux.h"
#include "IRCCommands.h"
#include "inputline.h"
#include "plugins.h"
#include "IRCNComm.h"
#include "utils.h"
#include "Inline.h"
#include "IRCNotify.h"
#include "TextManip.h"

#pragma internal on
#pragma dont_inline on

pascal void doMODE(linkPtr link, ConstStringPtr channel, StringPtr modeChange, ConstStringPtr setBy, LongString *tls);

static pascal void AttemptNickRegainNickChange(linkPtr link, ConstStringPtr cantUse)
{
	LongString ls;
	
	if(link->linkPrefs->regainNick)
	{
		if(!pstrcasecmp(cantUse, link->linkPrefs->nick))
		{
			LSConcatStrAndStr("\pNICK ", link->linkPrefs->nick, &ls);
			SendCommand(link, &ls);
		}
		else if(!pstrcasecmp(cantUse, link->linkPrefs->alterNick) && !pstrcmp(cantUse, link->CurrentNick))
		{
			LSConcatStrAndStr("\pNICK ", link->linkPrefs->alterNick, &ls);
			SendCommand(link, &ls);
		}
	}
}


inline void n302userhost(linkPtr link, LongString *ls);
inline void n353names(LongString *rest, linkPtr link);
inline void n433nickInUse(linkPtr link, LongString *rest);

/*
	Unet options:
	
	Gather for future use
		WHOX WALLCHOPS USERIP CPRIVMSG CNOTICE MAP
	
	Use now:
		TOPICLEN=160
		MODES=6
	
	Use later:
		SILENCE=15 MAXCHANNELS=10 MAXBANS=30 NICKLEN=9 KICKLEN=160
	
	Dalnet:
		WATCH=128 - serverside notify
		SAFELIST - does /list in chunks
*/

static void n005ServerFeatures(linkPtr link, LongString *ls)
{
	Str255 feature, value;
	long x, y;
	htDataType dt;
	void* data;
	LongString copy;
	
	LSDupe(ls, &copy);
	SMPrefixLink(link, &copy, dsConsole);
	
	while(1)
	{
		LSNextArg(ls, feature);
		if(!feature[0] || feature[1] == ':')
			break;
		
		//Find the = in the feature, if any
		x = pos('=', feature);
		if(!x)
		{
			data = 0;
			dt = htTypeNull;
		}
		else
		{
			myCopy(feature, x + 1, 255, value);
			if(isNumber(value))
			{
				StringToNum(value, &y);
				data = (void*)y;
				dt = htTypeInt;
			}
			else
			{
				data = value;
				dt = htTypeString;
			}
			feature[0] = x-1;
		}
		
		HTAdd(link->serverOptions, feature, data, dt);
	}
}


inline void n302userhost(linkPtr link, LongString *ls)
{
	int i;
	Str255 s1, s2;
	UserListPtr ul;
	channelPtr ch;
	pServiceULUserhostsData p;
	
	char isAway, isOper;
	
	if(link->outstandingUSERHOST>0)
		link->outstandingUSERHOST--;
	while(ls->len>1)
	{
		LSNextArgIRC(ls, s1);
		i=pos('=', s1);
		isAway=s1[i+1]=='-'; //is - is away, + is here
		myCopy(s1, i+2, 255, s2);
		s1[0]=i-1;
		if(s1[s1[0]]=='*') //is oper
		{
			s1[0]--;
			isOper=1;
		}
		else
			isOper=0;
		
		linkfor(ch, link->channelList)
		{
			ul=ULFindUserName(ch, s1);
			if(ul)
			{
				pstrcpy(s2, ul->userhost);
				ul->isOper=isOper;
				ul->isAway=isAway;
				ul->lastUHUpdate=now;
			}
		}
	}
	
	p.link = link;
	runIndService(userlistServiceClass, pServiceULUserhosts, &p);
}

inline void n353names(LongString *rest, linkPtr link)
{
	short p;
	UserListPtr ul, ul2;
	LongString ls;
	Str255 s1, s2;
	char o,v,h;
	channelPtr cc, c2;
	
	LSDelete(rest, 1, 2);
	ls=*rest;
	LSNextArgIRC(rest, s1);
	MWColor(&ls, sicServer);

	cc=ChFind(s1, link);
	if(cc)
	{
		ChMsg(cc, &ls);
		while(rest->len>1)
		{
			LSNextArg(rest, s1);
			
			if(s1[s1[0]]==' ')
				s1[0]--;
			
			if(s1[0])
			{
				o=0;
				v=0;
				h=0;

				p=pos('@', s1);
				if(p)
				{
					o=1;
					pdelete(s1, p, 1);
				}
				p=pos('+', s1);
				if(p)
				{
					v=1;
					pdelete(s1, p, 1);
				}
				p=pos('%', s1);
				if(p)
				{
					h=1;
					pdelete(s1, p, 1);
				}
				
				//if we already have a userhost, just copy it and don't do a /userhost
				s2[0]=0;
				linkfor(c2, link->channelList)
					if(c2!=cc)
					{
						ul2=ULFindUserName(c2, s1);
						if(ul2)
						{
							if(ul2->userhost[0])
							{
								pstrcpy(ul2->userhost, s2);
								break;
							}
							else
							{
								ul2->lastUHUpdate=-1; //se we don't do userhost on it twice
							}
						}
					}

				//add userlist entry
				ul=ULAddUser(cc, s1, s2);
				if(ul)
				{
					ul->isOp=o;
					ul->hasVoice=v;
					ul->isHalfOp=h;
					if(ul->next == 0) //it's me
					{
						cc->hasOps = o;
						cc->hasVoice = v;
						cc->hasHalfOps=h;
					}
					if(!s2[0])
						link->needUserHosts=1;
				}
			}
		}
	}
	else
		ChannelMsg(link, s1, &ls);
}

inline void n433nickInUse(linkPtr link, LongString *rest)
{
	int i;
	Str255 s1;
	LongString ls;
	
	LSNextArgIRC(rest, s1);
	LSParamString(&ls, GetIntStringPtr(spServer, sNickInUse), s1, 0, 0, 0);
	SMPrefixLink(link, &ls, dsFrontWin);
	
	if(!link->gotMOTD) //error during connect
	{
		linkPrefsPtr lp = link->linkPrefs;
	
		i=s1[0];
		if((link->connectingNickAttempt==-2) && lp->alterNick[0]) //first time
		{
			link->connectingNickAttempt=-1;
			pstrcpy(lp->alterNick, s1);
			NickListAdd(link, lp->alterNick);
		}
		else if((link->connectingNickAttempt==-1) || ((link->connectingNickAttempt==-2) && !lp->alterNick[0])) //alternate didn't work, so take the main and build on it
		{
			link->connectingNickAttempt=0;
			pstrcpy(lp->nick, s1);
			SAppend1(s1, '0');
		}
		else
			s1[i]='0'+ (++link->connectingNickAttempt);

		LSConcatStrAndStr("\pNICK ", s1, &ls);
		SendCommand(link, &ls);
		pstrcpy(s1, link->CurrentNick);
	}
	else if(!pstrcasecmp(link->CurrentNick, s1))
		AttemptNickRegainNickChange(link, s1);
}

pascal char NumericComm(short comm, StringPtr from, ConstStringPtr target, LongString *rest, linkPtr link)
{
	Str255 s1, s2, s3, s4;
	LongString ls;
	unsigned long ul;
	LongString unparsedrest;
	pNumericCommandDataRec p;
	char returnValue;
	channelPtr ch;
	int x;
	long l;
	linkPtr lp;
	
	returnValue=1;
	unparsedrest=*rest;
	
	for(x=8;x>=0;x--)
		link->lastNumerics[x+1]=link->lastNumerics[x];
	link->lastNumerics[0]=comm;

	p.link = link;
	p.from = from;
	p.command=comm;
	p.incoming=&unparsedrest;
	p.commandProcessed=&returnValue;
	p.lastNumerics=link->lastNumerics;
	p.DONT_PROCESS = 0;
	runPlugins(pNumericCommandMessage, &p);
	
	if(p.DONT_PROCESS)
		return returnValue;
	
	switch(comm)
	{
		case 324: //mode
			LSNextArgIRC(rest, s1);
			if((rest->len>1) && ((rest->len>2) || (rest->data[2] != ' ')))
			{
				LSConcatStrAndLS("\pMode is ", rest, &ls);
				ch=ChFind(s1, link);
				if(ch)
				{
					SMPrefix(&ls, dsNowhere);
					ChMsg(ch, &ls);
				}
				else
				{
					SMPrefixLink(link, &ls, dsNowhere);
					ChannelMsg(link, s1, &ls);
				}
				
				LSMakeStr(*rest);
				doMODE(link, s1, rest->data, "\p", 0);
				ch=ChFind(s1, link);
				if(ch)
					DrawMWinStatus(ch->window);
			}
			break;
		
		case 353: //names
			n353names(rest, link);
			break;
		
		case 367: // banlist
		case 348: //exception list
		{
			bansP f;
			LSNextArg(rest, s1); //channel

			ch=ChFind(s1, link);
			if(ch)
			{
				LSNextArg(rest, s1); //ban
				ChGetBan(ch, s1, &f, comm == 348); //true if exception
				if(f)
				{
					LSNextArg(rest, f->setBy); //who
					LSNextArg(rest, s1); //date

					StringToNum(s1, &l);
					f->time= l + ircDateModifier;
				}
			}
		}
		break;

		case 433: //nick in use
		case 453: //TS4 nick lost
			n433nickInUse(link, rest);
			break;
		
		case 302: //userhost
			n302userhost(link, rest);
			break;
		
		case 441: //not in channel
			LSNextArg(rest, s1); //nick
			LSNextArgIRC(rest, s2); //channel
			LSParamString(&ls, GetIntStringPtr(spError, sNotOnChannel), s1, s2, 0, 0);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
			
		case 451: //not registered
			LSGetIntString(&ls, spError, sRegistrationFailed); //Registration failed, trying again...
			SMPrefixLink(link, &ls, dsFrontWin);
			link->UserRegistered=0;
			RegUser(link);
			break;
		
		case 1: //2.8 intro
		{
			pServiceCWLinkStateChangeData pCWSC;
			
			if(!pstrcmp(link->CurrentNick, target))
			{
				LSGetIntString(&ls, spError, sServerNickMismatch);
				SMPrefixLink(link, &ls, dsFrontWin);
				pstrcpy(target, link->CurrentNick);
			}
			
			pstrcpy(from, link->CurrentServer);
			SMPrefixLink(link, rest, dsFrontWin);
			pCWSC.link = link;
			pCWSC.connectStage = link->conn->connectStage = csOnline;
			
			//Clear and create a new hashtable.
			if(link->serverOptions)
				HTDestroy(link->serverOptions);
			link->serverOptions = HTCreate(DEFAULT_HASHSIZE);
			
			runIndService(connectionWindowServiceClass, pServiceCWLinkStateChange, &pCWSC);
			UpdateStatusLine();
			break;
		}
		
		case 5: //map
			//This is a supports command at connect.
			if(!link->gotMOTD)
			{
				n005ServerFeatures(link, rest);
				break;
			}
			
		case 2:
		case 3:
		case 14: //efnet TS4 cookie
		case 200: //trace
		case 201:
		case 202:
		case 203:
		case 204:
		case 205:
		case 206:
		case 208:
		case 209: //trace server
		case 261:
		case 251: //lusers there are n users ...
		case 255: //i have n clients
		case 265: //efnet current lusers
		case 266: 
		case 351: //server version
		case 371: //info reply
		case 211: //stats l
		case 212: //stats m
		case 213: //stats c
		case 214:	 //stats n
		case 215: //stats i
		case 216: //stats
		case 217: //stats p
		case 218: //stats y
		case 222: //stats q
		case 242: //stats u-uptime
		case 243: //stats o
		case 244: //stats h
		case 247: //stats g
		case 248: //srars U
		case 249: //stats tz
		case 275:	//stats d
		case 282:	//current global users (superchat)
		case 283: //current local users (superchat)
		case 284: //num users listening to wallops (superchat)
		case 286: //whois user is irc administrator (superchat)
		case 287: //whois user is a network service (superchat)
		case 292: //hello nick (u@h) Welcome! (superchat)
		case 293: //If you need an IRCop (superchat)
		case 250: //stats u-connection count
		case 322: //list
		case 381: //operator stats
		case 15: //map
		case 357: //austnet map
		case 8: //undernet mode +s mask
		case 409: //no origin
		case 411: //no recipient
		case 412: //no text
		case 416: //too many lines in output
		case 382: //rehash
		case 280: //gline list (undernet)
		case 423: //no /admin
		case 432: //erroneous nickname
		case 437: //cannot change nickname while banned
		case 440:	//services currently down (dalnet)
		case 446: //users has been disabled
		case 452: //TS4 ID collision kill
		case 468: //various, incl. only servers can change that mode (rco.net)
		case 484: //can't kill, kick, or deop CService
		case 501: //umode unknown flag (undernet)
		case 502: //umode can't change mode for other users (undernet)
		case 512:	//no such gline (undernet)
		case 513: //incplatible client
			SMPrefixLink(link, rest, dsFrontWin);
			break;
		
		
		case 252:
		case 253:
		case 254:
			LSNextArgIRC(rest, s1);
			LSConcatStrAndStr("\pThere are ", s1, &ls);
			LSAppend1(ls, ' ');
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 289: //admins online (superchat)
			LSNextArgIRC(rest, s1);
			SAppend1(s1, ' ');
			LSConcatStrAndLS(s1, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 306: //away
			link->isAway = anyAway = 1;
			SMPrefixLink(link, rest, dsFrontWin);
			UpdateStatusLine();
			DrawMWinStatus(consoleWin);
			break;
			
		case 305: //unaway
			if(link->awayString)
			{
				DisposePtr((Ptr)link->awayString);
				link->awayString = 0;
			}
			
			link->isAway = anyAway=0;
			linkfor(lp, firstLink)
				if(lp->isAway)
				{
					anyAway=1;
					break;
				}
			SMPrefixLink(link, rest, dsFrontWin);
			UpdateStatusLine();
			DrawMWinStatus(consoleWin);
			break;
			

		case 431: //bogus nick
		case 436: 
		case 462: //registration probs
		case 464: //need password (?)
		case 481: //no privliges
		case 483: 
			if(rest->data[1]==':')
				LSDelete(rest, 1, 1);
		case 288: //gammanet ident found
		case 291: //gammanet welcome
		case 290: //gammanet need help
		case 517: //gammanet nospoof help
			SMPrefixLink(link, rest, dsConsole);
			break;
		
		case 303: //ison
			ProcessISON(link, rest);
			break;
		
		case 372: //new motd
		case 375:
		case 377: //efnet
		case 378: //austnet
			link->gotMOTD = 1;
			if(link->finishedMOTD)
				SMPrefixLink(link, rest, dsFrontWin);
			else if(link->linkPrefs->displayMOTD)
				SMPrefixLink(link, rest, dsConsole);
			break;
		
		case 329: //channel creation date
			LSNextArg(rest, s1); //channel
			LSNextArg(rest, s2); //time
			StringToNum(s2,  (long*)&ul);
			ul+=ircDateModifier;
			DateString(ul, longDate, s2, 0);
			TimeString(ul, true, s3, 0);
			LSParamString(&ls, GetIntStringPtr(spInfo, sChannelCreatedTime), s2, s3, 0, 0);
			ch=ChFind(s1, link);
			if(ch)
			{
				SMPrefix(&ls, dsNowhere);
				ChMsg(ch, &ls);
			}
			else
			{
				SMPrefixLink(link, &ls, dsNowhere);
				ChannelMsg(link, s1, &ls);
			}
			break;
		
		case 554: //whowas not supported (TalkCity)
			if(link->lastNumerics[1] == 318) //318 is end of whois
				break;
			else
				goto _Skip;
		
		case 401: //no such nick
			//if we're currently doing userhosts, don't display (break)
			//Need to check if the user exists before not displaying it...
			if(link->outstandingUSERHOST || link->lastNumerics[1] == 302) //in case it's zero and lastcomm was a userhost result
				break;
		
_Skip:
		case 256: //admin
		case 257: 
		case 258: 
		case 259: 
		case 331: //no topic
		case 402: // no such server/channel, can't send to channel, too many channels, whowas-noshch 
		case 403: 
		case 404: 
		case 405: 
		case 406: 
		case 407: //too many targets
		case 413: //no toplevel
		case 414: //no wildtoplevel
		case 421: //unknown command
		case 434: //rules (shadownet)
		case 438: //nick change too fast
		case 439: //nick changes not supported (talkcity), target hange too fast (undernet)
		case 442: //not on channel
		case 461: //need more parms
		case 467: //channel key set
		case 471: //+l
		case 472: //unknown mode
		case 473: //+i
		case 474: //+b
		case 475: //+k
		case 553: //cannot create dynamic channels due to admin restriction
		case 555: //server option for this command not supported (TalkCity)
			LSNextArgIRC(rest, s1);
			LSStrLS(s1, &ls);
			LSAppend2(ls, ': ');
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 321: //list header
		case 334: //list instructions
		case 364: //links
			Message(rest);
			break;
		
		case 422: //no motd
			link->gotMOTD = link->finishedMOTD = 1;
			DrawMWinStatus(consoleWin);
			finishConnect(link);
		case 424: //file err
		case 465: //banned
		case 491: //no O:
			pstrcpy(from, (StringPtr)&ls.data[1]);
			ls.len=3+ls.data[1];
			ls.data[1]='(';
			*(short*)&ls.data[ls.len-1]=') ';
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsConsole);
			break;
		
		case 4: //2.8 name version umodes chmodes
			LSNextArg(rest, link->CurrentServer);
			LSNextArg(rest, 0); //server version
			LSNextArg(rest, s1); //user modes
			LSNextArg(rest, s2); //channel modes
			
			if(link->umodes)
				DisposePtr((Ptr)link->umodes);
			link->umodes = NewPString(s1);

			if(link->yourUmodes)
				DisposePtr((Ptr)link->yourUmodes);
			link->yourUmodes = (StringPtr)NewPtrClear(s1[0] + 1);
			link->yourUmodes[0] = s1[0];
			
			if(link->chModes)
				DisposePtr((Ptr)link->chModes);
			link->chModes = NewPString(s2);
			
			UpdateStatusLine();
			break;
		
		case 221: //umode
			LSGetIntString(&ls, spServer, sUserModeIs);
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);

			//Update yourUmodes
			LSNextArgND(rest, s1);
			{
				int y;

				//this is a list of all umodes, so we clear out the current list..
				for(x=1;x<=link->yourUmodes[0];x++)
					link->yourUmodes[x] = 0;
				
				//rest[1] is +
				for(x=2; x<= s1[0]; x++)
					for(y=1;y<=link->umodes[0];y++)
						if(s1[x] == link->umodes[y])
						{
							link->yourUmodes[y] = link->umodes[y];
							break;
						}

				UpdateStatusLine();
			}
			break;
		
		case 301: //user is away
			LSNextArgIRC(rest, s1);
			LSConcatStrAndStr(s1, "\p is away", &ls);
			LSAppend4(ls, 0x202808FA); //' (' + save text style
			LSConcatLSAndLS(&ls, rest, &ls);
			LSAppend3(ls, 0x08FB2900); //reset color and close paren
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 332: //topic
			LSNextArgIRC(rest, s1);
			LSParamString(&ls, GetIntStringPtr(spServer, sTopicFor), s1, 0, 0, 0);
			LSConcatLSAndLS(&ls, rest, &ls);
			ch=ChFind(s1, link);
			if(ch)
			{
				SMPrefix(&ls, dsNowhere);
				ChMsg(ch, &ls);
				LSCopyString(rest, 1, 255, ch->topic);
				DrawMWinStatus(ch->window);
			}
			else
			{
				SMPrefixLink(link, &ls, dsNowhere);
				ChannelMsg(link, s1, &ls);
			}
			break;
		
		case 333: //topic set by
			LSNextArg(rest, s1); //channel
			LSNextArg(rest, s2); //name
			LSNextArg(rest, s3); //time
			StringToNum(s3,  (long*)&ul);
			ul+=ircDateModifier;
			DateString(ul, longDate, s3, 0);
			TimeString(ul, true, s4, 0);
			LSParamString(&ls, GetIntStringPtr(spInfo, sTopicSetBy2), s2, s3, s4, 0);
			ch=ChFind(s1, link);
			if(ch)
			{
				pstrcpy(s2, ch->topicSetBy);
				ch->topicSetOn=ul;
				DrawMWinStatus(ch->window);
				SMPrefix(&ls, dsNowhere);
				ChMsg(ch, &ls);
			}
			else
			{
				SMPrefixLink(link, &ls, dsNowhere);
				ChannelMsg(link, s1, &ls);
			}
			break;
		
		case 300: //EliteIRCd channel url
		case 328: //dalnet channel url
			LSNextArgIRC(rest, s1); //channel
			LSGetIntString(&ls, spServer, sChannelURLIs);
			LSConcatLSAndLS(&ls, rest, &ls);
			ch=ChFind(s1, link);
			if(ch)
			{
				SMPrefix(&ls, dsNowhere);
				ChMsg(ch, &ls);
			}
			else
			{
				SMPrefixLink(link, &ls, dsNowhere);
				ChannelMsg(link, s1, &ls);
			}
			break;
		
		case 311: //whois
		case 314: //whowas
			LSNextArg(rest, s1); //nick
			LSNextArg(rest, s2); //username
			LSNextArg(rest, s3); //host
			LSNextArgIRC(rest, 0); //unknown
			LSStrCat(5, &ls, s1, ((comm==311)?"\p is ":"\p was "), s2, "\p@", s3);
			LSAppend4(ls, 0x202808FA); //' (' + save text style
			LSConcatLSAndLS(&ls, rest, &ls);
			LSAppend3(ls, 0x08FB2900); //reset color and close paren
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 310: //whois looks very helpful (dalnet)
		case 313: //whois oper
		case 307: //whois registered nick (dalnet), or userip on undernet
		case 308: //whois server admin
		case 309: //whois services admin
		case 295: //whois server admin (chaos.njquake.com)
			LSNextArgIRC(rest, s1);
			LSStrLS(s1, &ls);
			LSAppend1(ls, ' ');
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
			
		case 319: //whois channels
			LSNextArgIRC(rest, s1);
			LSParamString(&ls, GetIntStringPtr(spServer, sIsOnChannels), s1, 0, 0, 0);
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 312: //whois host/server
			LSNextArg(rest, s1); //nick
			LSNextArgIRC(rest, s2); //server
			LSStrLS(s1, &ls);
			if(link->lastNumerics[1]==314) //whowas
				LSAppend4(ls, ' was')
			else
				LSAppend3(ls, ' is ');
			
			LSConcatLSAndStrAndStr(&ls, "\p on IRC via server ", s2, &ls);
			LSAppend2(ls, ' (');
			LSConcatLSAndLS(&ls, rest, &ls);
			LSAppend1(ls, ')');
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 317: //whois idle, signon time
			LSNextArg(rest, s1); //nick
			LSNextArg(rest, s2); //idle seconds
			LSNextArg(rest, s3); //signon time
			//Ignore :info
			
			StringToNum(s2, &l);
			SecsToHMS(l, rest);
			LSConcatStrAndStr(s1, "\p has been idle for ", &ls);
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			StringToNum(s3,  (long*)&ul);
			ul+=ircDateModifier;
			DateString(ul, longDate, s2, 0);
			TimeString(ul, true, s3, 0);
			LSParamString(&ls, GetIntStringPtr(spWhois, sWhSignedOnAt), s1, s2, s3, 0);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 341: //invite confirmation
			LSNextArgIRC(rest, s1);
			LSNextArgIRC(rest, s2);
			LSParamString(&ls, GetIntStringPtr(spServer, sInvitingToChannel), s1, s2, 0, 0);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 443: //unnecessary invite
			LSNextArg(rest, s1); //nick
			LSNextArg(rest, s2); //channel
			//ignoring :is already on chanel
			LSParamString(&ls, GetIntStringPtr(spServer, sAlreadyOnChannel), s1, s2, 0, 0);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 352: //who
			LSNextArg(rest, s1); //channel
			padEnd(s1, 15, s1);
			LSNextArg(rest, s2); //user
			LSNextArg(rest, s3); //host
			x=s2[0]+1;
			pstrcpy(s3, &s2[x]);
			s2[0]+=s2[x]+1;
			s2[x]='@';
			LSNextArg(rest, s3); //server
			LSNextArg(rest, s3); //nick
			padBegin(s3,9,s3);
			SAppend1(s3, ' ');
			LSNextArgIRC(rest, s4); //user info with :hopcount
			padEnd(s4, 7, s4);
			
			LSNextArg(rest, unparsedrest.data); //hopcount
			//ignoring username
			SAppend1(s1, ' ');
			SAppend1(unparsedrest.data, ' ');
			LSStrCat(5, &ls, s1, s3, s4, unparsedrest.data, s2);
			MWColor(&ls, sicServer);
			Message(&ls);
			break;
		
		case 463: //server refuses connection
		case 466:
			LSConcatStrAndStr(from, "\p refuses connection ", &ls);
			LSConcatLSAndLS(&ls, rest, &ls);
			SMPrefixLink(link, &ls, dsConsole);
			break;
		
		case 391: //time
			//server unixTime ?? :Long_date -- hh:mm -hh:mm[TimeDiff]
			LSNextArg(rest, s1); //server
			LSNextArg(rest, s2); //unix time
			LSNextArgIRC(rest, 0); //??
			//rest is long date
			LSConcatStrAndStr("\pTime for ", s1, &ls);
			LSAppend2(ls, ': ');
			LSConcatLSAndLS(&ls, rest, &ls);
			LSAppend2(ls, ' (');
			LSConcatLSAndStr(&ls, s2, &ls);
			LSAppend1(ls, ')');
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 478: //channel banlist is full
			//channel failedBan :Error Message
			LSNextArgIRC(rest, s1); //channel
			LSNextArgIRC(rest, s2); //ban;
			
			LSAppend2(*rest, ': ');
			LSConcatLSAndStr(rest, s2, rest);
			ch=ChFind(s1, link);
			if(ch)
			{
				SMPrefix(rest, dsNowhere);
				ChMsg(ch, rest);
			}
			else
			{
				SMPrefixLink(link, rest, dsNowhere);
				ChannelMsg(link, s1, rest);
			}
			break;
		
		case 511: //silence is full
			//channel failedBan :Error Message
			LSNextArgIRC(rest, s1); //silence
			
			LSAppend2(*rest, ': ');
			LSConcatLSAndStr(rest, s1, rest);
			SMPrefixLink(link, rest, dsFrontWin);
			break;
		
		case 271: //silence
			//nick silence
			LSNextArgIRC(rest, 0); //nick
			//LSNextArgIRC(rest, s2); //ignore
			LSConcatStrAndLS("\pSilenced: ", rest, &ls);
			SMPrefixLink(link, &ls, dsFrontWin);
			break;
		
		case 477: //modeless channel
			if(link->lastNumerics[1]!=477)
			{
		case 482: //not channel op
		case 326: //TS4 no channel pass
		case 338: //TS4 channel password ok
		case 339: //TS4 invalid channel password
		case 487: //TS4 channel created too recently
		case 488: //TS4 mode not available on TS-less channel
				LSNextArgIRC(rest, s1); //channel name
				ch=ChFind(s1, link);
				if(ch)
				{
					SMPrefix(rest, dsNowhere);
					ChMsg(ch, rest);
				}
				else
				{
					SMPrefixLink(link, rest, dsNowhere);
					ChannelMsg(link, s1, rest);
				}
			}
			break;
			
		case 325: //TS4 channel password
			LSNextArg(rest, s1); //channel
			ch = ChFind(s1, link);
			LSNextArg(rest, s2); //password
			LSParamString(&ls, GetIntStringPtr(spServer, sChPasswd + (ch == 0)), s1, s2, 0, 0);
			if(ch)
			{
				SMPrefix(rest, dsNowhere);
				ChMsg(ch, rest);
			}
			else
			{
				SMPrefixLink(link, rest, dsNowhere);
				ChannelMsg(link, s1, rest);
			}
			break;
		
		case 274: //TS4 stats n timedelta
			LSMakeStr(*rest);
			LSParamString(&ls, GetIntStringPtr(spInfo, sStatsNTimeDelta), rest->data, 0, 0, 0);
			SMPrefixLink(link, &ls, dsNowhere);
			Message(&ls);
			break;
		
// -------End of command------
		case 219: //stats 
			if(mainPrefs->showEndMessages)
			{
				LSNextArgIRC(rest, s1);
				LSAppend2(*rest, ' (');
				LSConcatLSAndStr(rest, s1, rest);
				LSAppend1(*rest, ')');
				SMPrefixLink(link, rest, dsFrontWin);
			}
			break;
		
		case 366: //end of names
			LSNextArgND(rest, s1); //channel
			ch = ChFind(s1, link);
			
			if(ch)
			{
				if(mainPrefs->showEndMessages)
				{
					SMPrefix(rest, dsNowhere);
					ChMsg(ch, rest);
				}
				
				if(!ch->gotNames)
				{
					runIndService(userlistServiceClass, pServiceULNamesEnd, &ch);
					ch->gotNames = 1;
				}
			}
			break;

		case 376: //motd
			link->finishedMOTD=1;
			DrawMWinStatus(consoleWin);
			finishConnect(link);
		case 7: //map
		case 17: //new map
		case 315: //who
		case 323: //list
		case 349: //TS4 channel exception list
		case 359: //austnet end of map
		case 365: //links
		case 374: //info
		case 281: //undernet gline
		case 272: //silence
		case 444: //end of /akill list (superchat)
			if(mainPrefs->showEndMessages)
				SMPrefixLink(link, rest, dsFrontWin);
			break;
		
		case 318: //whois
			if(mainPrefs->doWhowasIfFailedWhois && (link->lastNumerics[1]==401)) //user not found
			{
				LSNextArgIRC(rest, s1);
				LSConcatStrAndStr("\pWHOWAS ", s1, &ls);
				SendCommand(link, &ls);
			}
			break;
			
		case 368: //channel ban list
		case 369: //whowas
			break;
		
		default:
			if(comm == 999) //X/W stuff
			{
				LSNextArg(rest, s1); //channel
				ch = ChFind(target, link);
				if(ch)
				{
					ChMsg(ch, rest);
				}
				else
				{
					SMPrefixLink(link, rest, dsNowhere);
					ChannelMsg(link, target, rest);
				}
			}
			else
				returnValue=0;
	}
	
	return returnValue;
}