/*
	Popups - ShadowIRC plugin for context-based menus in Message Windows
	Copyright (C) 1998-2000 John Bafford
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

#include "ShadowIRC.h"

#define INLINE inline

static ShadowIRCDataRecord* sidr; //For 68k plugins, so we don't have to pass sidr all over the place in the PPC version for no reason at all.

typedef struct menuItem {
	struct menuItem *next, *prev;
	
	Str31 name;
	short scope;
	short theMenuID;
	MenuHandle m;
	char isSubmenu;
	char op, oper, ip, host, user, userOp, userVoice, channel, aelse;
	char everything;
	char pad;
	Str255 command;
} menuItem, *menuPtr;


menuPtr popup = 0;
menuPtr lastPopup = 0;

char isOper(linkPtr link);
inline void CMCleanUp(void);
INLINE char GetNextQualifier(LongString *ls, Str255 s, char *not);
inline void handleReadLine(Ptr p, long *start, long *finish, LongString *ls);
inline menuPtr AddPopupItem(Str31 name, short scope, char submenu, Str255 command);
void AddSubmenuItem(menuPtr x);
inline void ClearList(void);
void CreateConf(void);
void ParseFile(void);
inline void Rehash(void);
inline void MakeCMMenu(pCMPopupsDataPtr d);
inline void doCMItems(pCMPopupsReturnDataPtr d);
void DoCommand(pCMPopupsReturnDataPtr d, menuPtr x);
inline char parseCommand(ShadowIRCDataRecord* sidr, pCommandDataRec *p);
pascal void displayTooOldMessage(void);

inline void handleReadLine(Ptr p, long *start, long *finish, LongString *ls)
{
	long i, len;
	long max = GetPtrSize(p);
	
	if(*start<0)
		*start=0;
	
	i=*start-1;
	if(!*finish)
		*finish=max;
	
	do
	{
		i++;
		if(p[i]==13)
		{
			*finish=i;
			len=i-*start;
			if(len>600)
				len=600;
			
			BlockMoveData(p +((*start)), &ls->data[1], len);
			ls->len=len;
			return;
		}
	} while((i<=*finish) && (i<max));

	ls->len = 0;
}

inline menuPtr AddPopupItem(Str31 name, short scope, char submenu, Str255 command)
{
	menuPtr x = (menuPtr)NewPtr(sizeof(menuItem));
	
	x->prev = lastPopup;
	if(lastPopup)
		lastPopup->next = x;
	lastPopup = x;
	if(!popup)
		popup = x;
	x->next = 0;
	
	pstrcpy(name, x->name);
	x->scope = scope;
	x->isSubmenu = submenu;
	pstrcpy(command, x->command);
	x->m = 0;
	
	x->op = x->ip = x->host = x->user = x->userOp = x->userVoice = x->channel = x->aelse = 0;
	x->everything = 1;
	
	return x;
}

void AddSubmenuItem(menuPtr x)
{
	menuPtr y = popup;
	
	while(y)
	{
		if(y->scope == x->scope && y->isSubmenu && y->m)
		{
			AppendMenu(y->m, x->name);
			return;
		}
		
		y=y->next;
	}
}

inline void CMCleanUp(void)
{
	menuPtr x = popup;
	
	while(x)
	{
		x->m = 0;
		x=x->next;
	}
}

INLINE char GetNextQualifier(LongString *ls, Str255 s, char *not)
{
	if(!ls->len || ls->data[1] == ':')
		return false;
	else
	{
		short p = LSPosChar(',', ls);
		short pp = LSPosChar(':', ls);
		
		if(p && p<pp)
		{
			LSCopyString(ls, 1, p-1, s);
			LSDelete(ls, 1, p);
		}
		else
		{
			LSCopyString(ls, 1, pp-1, s);
			LSDelete(ls, 1, pp -1);
		}
		
		if(s[1] == '~')
		{
			pdelete(s, 1, 1);
			*not = 1;
		}
		else
			*not = 0;
		return true;
	}
}

inline void ClearList(void)
{
	menuPtr x = popup;
	
	while(x)
	{
		menuPtr y = x->next;
		DisposePtr((Ptr)x);
		x=y;
	}
	
	popup = 0;
}

void CreateConf(void)
{
	long siz;
	short f;
	Handle text = Get1Resource('TEXT', 128); //the default
	
	HLock(text);
	
	PFCreate("\pPopups.conf", 'TEXT', 'SIRC', false);
	PFOpen("\pPopups.conf", false, &f, 0);
	siz = GetHandleSize(text);
	SetEOF(f, siz);
	SetFPos(f, fsFromStart, 0);
	FSWrite(f, &siz, *text);
	PFClose(f);
	
	ReleaseResource(text);
}

void ParseFile(void)
{
	short f;
	LongString ls;
	long siz;
	Ptr text;
	long start, finish;
	short p, pp;
	Handle h;
	long scope;
	Str255 name;
	Str255 command;
	
	menuPtr x;
	char everything, op, oper, ip, host, user, userOp, userVoice, channel, aelse;
	char submenu;
	char not;
	
	if(!PFExists("\pPopups.conf")) //no conf file; make a new one
		CreateConf();
	else //check and see if it's been unmodified. If it has, delete it and get a new one
	{
		FSSpec fss;
		CInfoPBRec pb;
		
		PFOpen("\pPopups.conf", false, &f, &fss);
		
		PFSize(f, &siz);
		PFClose(f);
		
		h = Get1Resource('TEXT', 128);
		if(siz != GetHandleSize(h))
		{
			pb.hFileInfo.ioCompletion= 0;
			pb.hFileInfo.ioNamePtr = fss.name;
			pb.hFileInfo.ioVRefNum = fss.vRefNum;
			pb.hFileInfo.ioDirID = fss.parID;
			pb.hFileInfo.ioFDirIndex = -1;
			
			PBGetCatInfoSync(&pb);
			if(pb.hFileInfo.ioFlMdDat - pb.hFileInfo.ioFlCrDat < 10) //10 seconds
			{
				FSpDelete(&fss);
				CreateConf();
			}
		}
		ReleaseResource(h);
	}
	
	if(!PFOpen("\pPopups.conf", false, &f, 0))
	{
		SetFPos(f, fsFromStart, 0);
		GetEOF(f, &siz);
		text = NewPtr(siz);
		FSRead(f, &siz, text);
		
		finish = 0;
		do
		{
			if(!finish)
				start = 0;
			else
				start = finish + 1;
			finish = 0;
			
			handleReadLine(text, &start, &finish, &ls);
			
			if(ls.len && ls.data[1] != '#')
			{
				p = LSPosChar(':', &ls);
				if(p==1)
				{
					scope = 0;
					LSDelete(&ls, 1, 1);
				}
				else
				{
					LSCopyString(&ls, 1, p-1, name);
					StringToNum(name, &scope);
					LSDelete(&ls, 1, p);
				}
					
				p = LSPosChar(':', &ls);
				LSCopyString(&ls, 1, p-1, name);
				LSDelete(&ls, 1, p);
				
				pp = LSPosChar(':', &ls);
				
				everything = 1;
				op = oper = ip = host = user = userOp = userVoice = channel = aelse = 0;
				if(pp==1)
					LSDelete(&ls, 1, 1);
				else //parse qualifiers
				{
					Str255 qual;
					
					while(GetNextQualifier(&ls, qual, &not))
					{
						if(pstrcmp(qual, "\pop"))
							op = 1 + not;
						if(pstrcmp(qual, "\poper"))
							oper = 1 + not;
						else if(pstrcmp(qual, "\pip"))
							ip = 1 + not, everything = 0;
						else if(pstrcmp(qual, "\phost"))
							host = 1 + not, everything = 0;
						else if(pstrcmp(qual, "\puser"))
							user = 1 + not, everything = 0;
						else if(pstrcmp(qual, "\puserOp"))
							userOp = 1 + not, everything = 0;
						else if(pstrcmp(qual, "\puserVoice"))
							userVoice = 1 + not, everything = 0;
						else if(pstrcmp(qual, "\pchannel"))
							channel = 1 + not, everything = 0;
						else if(pstrcmp(qual, "\pelse"))
							aelse = 1 + not;
					}
					LSDelete(&ls, 1, 1);
				}
				
				LSCopyString(&ls, 1, 255, command);
				
				submenu = *(long*)&command[1] == '$SUB';
				if(submenu)
				{
					if(command[5] == '(')
					{
						pdelete(command, 1, 5);
						if(command[command[0]] == ')')
							command[0]--;
						StringToNum(command, &scope);
					}
					else
						submenu = 0;
				}
				x = AddPopupItem(name, scope, submenu, command);
				x->everything = everything;
				x->op = op;
				x->oper =oper;
				x->ip = ip;
				x->host = host;
				x->user = user;
				x->userOp = userOp;
				x->userVoice = userVoice;
				x->channel = channel;
				x->aelse = aelse;
			}
		} while(finish < siz);
				
		DisposePtr(text);
		PFClose(f);
	}
}

char isOper(linkPtr link)
{
	int x;
	
	if(link && link->yourUmodes)
		for(x=1; x<= link->yourUmodes[0]; x++)
			if(link->yourUmodes[x] == 'o')
				return 1;
	
	return 0;
}

#define T(c) ((!x->c) || (x->c == 1 && c) || (x->c == 2 && !c))
#define B(c) ((x->c == 1 && c) || (x->c == 2 && !c))
inline void MakeCMMenu(pCMPopupsDataPtr d)
{
	menuPtr x = popup;
	
	if(x && d->type == cmMW && d->oneWord && d->mw ->winType != textWin)
	{
		long i = 0;
		channelPtr ch = MWGetChannel(d->mw);
		UserListPtr user;
		char oper = isOper((linkPtr)d->mw->link);
		char channel = IsChannel(d->theWord);
		char ip = isIPNumber(d->theWord);
		char host = pos('.', d->theWord)>=1;
		char op;
		char ok;
		char userOp, userVoice;
		
		if(ch)
		{
			user = ULFindUserName(ch, d->theWord);
			op = ch->hasOps;
			userOp = user->isOp;
			userVoice = user->hasVoice;
		}
		else
		{
			user = 0;
			userOp = userVoice = op = 0;
		}
		
		CMCleanUp();
		
		while(x)
		{
			ok = T(op) && T(oper);
			if(ok)
			{
				if(x->scope == 0 && !x->isSubmenu)
					i++;
				
				if(!x->everything)
					ok = B(channel) || B(ip) || B(host) || B(user) || B(userOp) || B(userVoice) || x->aelse;
			}
				
			if(ok)
			{
				if(x->isSubmenu)
				{
					CMAddSubmenu(d, x->name, &x->theMenuID, &x->m);
				}
				else
				{
					if(x->scope == 0)
						CMAdd(d, x->name, i);
					else
						AddSubmenuItem(x);
				}
			}			
			x=x->next;
		}
	}
}

void DoCommand(pCMPopupsReturnDataPtr d, menuPtr x)
{
	Str255 s;
	Str255 proc;
	LongString temp;
	short n;
	long p;
	LongString ls;
	channelPtr ch;
	UserListPtr u;

	pstrcpy(x->command, s);
	
	while(s[0])
	{
		n = pos('¥', s);
		if(!n)
		{
			pstrcpy(s, proc);
			s[0] = 0;
		}
		else
		{
			pstrcpy(s, proc);
			proc[0] = n-1;
			pdelete(s, 1, n);
		}
		
		if(proc[0])
		{
			Handle h = NewHandle(proc[0]);
			if(h)
			{
				BlockMoveData(&proc[1], *h, proc[0]);
				
				//$word
				do
				{
					p = Munger(h, 0, "$word", 5, &d->theWord[1], d->theWord[0]);
				} while(p>=0);
				//$channel
				ch = MWGetChannel(d->mw);
				if(ch)
					pstrcpy(ch->chName, temp.data);
				else
					temp.data[0] = 0;
				
				do
				{
					p = Munger(h, 0, "$channel", 8, &temp.data[1], temp.data[0]);
				} while(p>=0);
				
				//$makemask
				if(ch)
					u = ULFindUserName(ch, d->theWord);
				else
					u = 0;
				
				if(u)
				{
					makeMask(u->userhost, temp.data);
				}
				else
					temp.data[0] = 0;
				
				do
				{
					p = Munger(h, 0, "$makemask", 9, &temp.data[1], temp.data[0]);
				} while(p>=0);
				
				//$il
				GetInputLine(&temp);
				do
				{
					p = Munger(h, 0, "$il", 3, &temp.data[1], temp.len);
				} while(p>=0);
				
				
				ls.len = GetHandleSize(h);
				if(ls.len > 500)
					ls.len = 500;
				BlockMoveData(*h, &ls.data[1], ls.len);
				HandleCommand(d->mw->link, &ls);
				DisposeHandle(h);
			}
		}
	}
}

inline void doCMItems(pCMPopupsReturnDataPtr d)
{
	menuPtr x = popup;
	int i = 0;
	
	if(x)
	{
		if(d->menuID == -1) //find in scope zero and do command
		{
			while(x)
			{
				if(x->scope == 0)
				{
					i++;
					if(i==d->id)
					{
						DoCommand(d, x);
						return;
					}
				}
				x=x->next;
			}
		}
		else //find menu with this scope, then find the nth item
		{
			while(x)
			{
				int y;
				if(x->theMenuID == d->menuID)
				{
					y = x->scope;
					x=popup;
					while(x)
					{
						if(x->scope == y && !x->isSubmenu)
						{
							i++;
							if(i == d->id)
							{
								DoCommand(d, x);
								return;
							}
						}
						
						x=x->next;
					}
					return;
				}
				x=x->next;
			}
		}
	}
}

inline void Rehash(void)
{
	ClearList();
	ParseFile();
}

inline char parseCommand(ShadowIRCDataRecord* sidr, pCommandDataRec *p)
{
	#pragma unused(sidr)
	LongString ls;
	
	if(pstrcmp(p->com, "\pRELOAD"))
	{
		LSNextArg(p->rest, ls.data);
		if(pstrcasecmp2("\pPOPUPS", ls.data))
		{
			p->dontProcess = 1;
			Rehash();
			LSStrLS("\pReloaded popups datafile.", &ls);
			SMPrefix(&ls, dsFrontWin);
			p->outgoing->len = 0;
			return 1;
		}

		p->outgoing->len = 0;
	}

	return 0;
}

pascal void displayTooOldMessage(void)
{
	LongString ls;

	LSStrLS("\pThe popups plugin requires ShadowIRC 1.1 or later.", &ls);
	SMPrefixIrcleColor(&ls, dsConsole, '2');
}

pascal void main(ShadowIRCDataRecord* sidrIN)
{
	unsigned long l;
	#if !__POWERPC__
	EnterCodeResource();
	#endif
	
	switch(sidrIN->message)
	{
		case pVersionCheckMessage:
			sidr=sidrIN;

			l=((pVersionCheckDataPtr)sidrIN->messageData)->version;
			if(l<0x0101000b) //we must have ShadowIRC 1.1d11 or later.
			{
				displayTooOldMessage();
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionShadowIRCTooOld;
			}
			else
			{
				l = _UndocumentedAPI('popu', 8);
				if(l)
					((pVersionCheckDataPtr)sidrIN->messageData)->version = l;
				else
					((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionCheckMessageReply;
			}
			break;
			
		case pInitMessage:
			sidrIN->yourInfo->captureMessages[pCMPopupsMessage]=1;
			sidrIN->yourInfo->captureMessages[pUserCommandMessage]=1;
			ParseFile();
			break;
		
		case pUserCommandMessage:
			sidrIN->completelyProcessed=parseCommand(sidrIN, (pCommandDataPtr)sidrIN->messageData);
			break;

		case pCMPopupsMessage:
			MakeCMMenu((pCMPopupsDataPtr)sidrIN->messageData);
			break;
			
		case pCMPopupsReturnMessage:
			doCMItems((pCMPopupsReturnDataPtr)sidrIN->messageData);
			break;
	}
	#if !__POWERPC__
	ExitCodeResource();
	#endif
}