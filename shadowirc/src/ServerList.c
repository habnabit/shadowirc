/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
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

#include "IRCGlobals.h"
#include "Utils.h"
#include "filesMan.h"
#include "inline.h"
#include "ServerList.h"
#include "AppearanceHelp.h"

#define enableinline 1
#if enableinline
	#define INLINE inline
#else
	#define INLINE pascal
#endif

static SLNetsHand netsRes = 0;
static SLNetsPtr netsR = 0;

static pascal void HiliteButtons(DialogPtr d, short net, short serv);
INLINE void SetupPrefsFile(void);
static pascal void ServerOpenPrefs(void);

INLINE short findServer(SLServHand sv, Str255 s);
INLINE short findNetworkType(long type);
static pascal void fillNetsList(ListHandle nets);
pascal void SetOneServer(ListHandle servs, short curNet, short curServ, SLServHand servers[]);
pascal void fillServsList(ListHandle servs, short curNet, SLServHand servers[]);
pascal void SetupServers(SLServHand *servers[]);
pascal void ReleaseServers(SLServHand servers[], char save);

pascal void WriteString(short ref, StringPtr s);
static pascal void ExportServers(SLServHand servers[], short ref);
INLINE void DoExport(SLServHand servers[]);
pascal char AddServer(SLServHand servers[], short curNet, short curServ);
INLINE char EditServer(SLServHand servers[], short curNet, short curServ);
INLINE void DeleteServer(SLServHand servers[], short curNet, short curServ);
INLINE char DoDefaults(ListHandle nets, ListHandle servs, SLServHand *servers[]);
static pascal unsigned char ServerDialogFilter(DialogPtr d, EventRecord *e, short *item);
pascal void DoServerSelect(ServerListServiceData *p);

static pascal void HiliteButtons(DialogPtr d, short net, short serv)
{
	char a, b;
	
	if(net >= 0)
	{
		a = 1;
		if(serv >= 0)
			b = 1;
		else
			b = 0;
	}
	else
		a = b = 0;
	
	setButtonEnable(d, 5, b);
	setButtonEnable(d, 7, b);
	
	setButtonEnable(d, 8, a);
}

INLINE void SetupPrefsFile(void)
{
	int n, x;
	Handle h;
	Str255 name;
	short id;
	ResType type;
	short rf = CurResFile();
	
	//Copy NTWK resource
	CopyResource('NTWK', 128, gApplResFork, mainResNum);
	
	//Copy the SRVR resources
	UseResFile(gApplResFork);
	n = Count1Resources('SRVR');
	
	for(x=1;x<=n;x++)
	{
		h = Get1IndResource('SRVR', x);
		GetResInfo(h, &id, &type, name);
		CopyResource('SRVR', id, gApplResFork, mainResNum);
	}
	
	UseResFile(rf);
}

static pascal void ServerOpenPrefs(void)
{
	short rf = CurResFile();
	UseResFile(mainResNum);
	
	netsRes = (SLNetsHand)Get1Resource('NTWK', 128);
	if(!netsRes)
	{
		SetupPrefsFile();
		netsRes = (SLNetsHand)Get1Resource('NTWK', 128);
	}
	
	if(netsRes)
	{
		HLockHi((Handle)netsRes);
		netsR = *netsRes;
	}
	UseResFile(rf);
}

#pragma mark -

INLINE short findServer(SLServHand sv, Str255 s)
{
	SLServPtr sp;
	int x;
	
	HLock((Handle)sv);
	sp = *sv;
	
	ucase(s);
	for(x=0;x<sp->count;x++)
	{
		if(pstrcasecmp2(s, sp->servs[x].name))
			goto stop;
	}

	x = -1;
stop:
	HUnlock((Handle)sv);
	return x;
}

INLINE short findNetworkType(long type)
{
	int x;
	
	for(x=0;x<netsR->count;x++)
		if(netsR->nets[x].type == type)
			return x;
	
	return -1;
}

#pragma mark -

pascal void SetOneServer(ListHandle servs, short curNet, short curServ, SLServHand servers[])
{
	LongString ls;
	Point c;
	Str255 st;
	
	if(servers[curNet])
	{
		SetPt(&c, 0, curServ);

		LSStrLS((**servers[curNet]).servs[curServ].name, &ls);
		LSAppend1(ls, ':');
		NumToString((**servers[curNet]).servs[curServ].port, st);
		LSConcatLSAndStr(&ls, st, &ls);
		LSetCell(&ls.data[1], ls.len, c, servs);
	}
}

pascal void fillServsList(ListHandle servs, short curNet, SLServHand servers[])
{
	//SLNetworkPtr n = &netsR->nets[curNet];
	SLServHand sH = servers[curNet];
	SLServPtr s;
	int x;
	Point c;
	LongString ls;
	Str255 st;

	LDelRow(0, 0, servs); //delete everything
	
	if(!sH)
		return;
	
	//Now, add all servers..
	HLockHi((Handle)sH);
	s = *sH;
	
	LAddRow(s->count, 0, servs);
	
	for(x=0;x<s->count;x++)
	{
		SetPt(&c, 0, x);
		LSStrLS(s->servs[x].name, &ls);
		LSAppend1(ls, ':');
		NumToString(s->servs[x].port, st);
		LSConcatLSAndStr(&ls, st, &ls);
		LSetCell(&ls.data[1], ls.len, c, servs);
	}

	HUnlock((Handle)sH);
}

static pascal void fillNetsList(ListHandle nets)
{
	int x;
	Point c;
	
	LDelRow(0, 0, nets); //delete everything
	
	LAddRow(netsR->count, 0, nets);
	
	for(x=0;x<netsR->count;x++)
	{
		SetPt(&c, 0, x);
		LSetCell(&netsR->nets[x].name[1], netsR->nets[x].name[0], c, nets);
	}
}

#pragma mark -

pascal void SetupServers(SLServHand *servers[])
{
	int x;
	
	if(*servers)
		DisposePtr((Ptr)*servers);
	
	*servers = (SLServHand*)NewPtr(sizeof(Handle)*netsR->count);

	for(x=0;x<netsR->count;x++)
		(*servers)[x] = (SLServHand)Get1NamedResource('SRVR', netsR->nets[x].name);
}

pascal void ReleaseServers(SLServHand servers[], char save)
{
	int x;
	
	if(servers)
	{
		for(x=0;x<netsR->count;x++)
		{	
			if(save)
			{
				ChangedResource((Handle)servers[x]);
				WriteResource((Handle)servers[x]);
			}
			ReleaseResource((Handle)servers[x]);
		}
		UpdateResFile(mainResNum);
		DisposePtr((Ptr)servers);
	}
}

#pragma mark -

pascal void WriteString(short ref, StringPtr s)
{
	long l;
	
	GetEOF(ref, &l);
	
	SetFPos(ref, fsFromStart, l);
	SetEOF(ref, l + s[0]);
	l=s[0];
	FSWrite(ref, &l, &s[1]);
}

static pascal void ExportServers(SLServHand servers[], short ref)
{
	int x, n = netsR->count;
	int y, m;
	Str255 s;
	SLServPtr sp;
	
	SetEOF(ref, 0);
	WriteString(ref, "\p#ShadowIRC Server List\n");
	WriteString(ref, "\p\nLIST-FORMAT 1\n");
	for(x=0;x<n;x++)
	{
		WriteString(ref, "\p\nBEGIN-NETWORK ");
		WriteString(ref, netsR->nets[x].name);
		s[0] = 0;
		SAppend1(s, ' ');
		SAppend4(s, netsR->nets[x].type);
		SAppend1(s, '\n');
		WriteString(ref, s);
		
		HLock((Handle)servers[x]);
		sp = *servers[x];
		m = sp->count;
		for(y = 0;y<m;y++)
		{
			WriteString(ref, sp->servs[y].name);
			NumToString(sp->servs[y].port, &s[1]);
			s[0] = s[1]+2;
			s[1] = ':';
			s[s[0]] = '\n';
			WriteString(ref, s);
		}
		HUnlock((Handle)servers[x]);
		
		WriteString(ref, "\pEND-NETWORK\n");
	}
	
	WriteString(ref, "\p\nEND-LIST\n");
}

INLINE void DoExport(SLServHand servers[])
{
	FSSpec fs;
	short refNum;
	
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	NavEventUPP			eventUPP = NewNavEventUPP(NavDialogFilter);
	OSErr err;
	AEKeyword key;
	
	NavGetDefaultDialogOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavPutFile;
	dialogOptions.dialogOptionFlags |= kNavNoTypePopup;
	
	pstrcpy("\pServerList Export", dialogOptions.savedFileName);
	pstrcpy("\pShadowIRC", dialogOptions.clientName);
	
	err = NavPutFile(0, &theReply, &dialogOptions, eventUPP, 'TEXT', 'SIRC', 0);
	
	DisposeNavEventUPP(eventUPP);
	
	if(theReply.validRecord && !err)
	{
		AEDesc resultDesc;	
		resultDesc.dataHandle = 0L;
		
		// retrieve the returned selection:
		if((err = AEGetNthDesc(&(theReply.selection),1,typeFSS, &key,&resultDesc)) == noErr)
		{
			AEGetDescData(&resultDesc, &fs, sizeof(FSSpec));

			if(theReply.replacing)
			{
				if(FSpDelete(&fs)) //error
				{
					return;
				}
			}
				
			AEDisposeDesc(&resultDesc);
		}

		NavDisposeReply(&theReply);
	}
	else
		return;
		
	if(!FSpCreate(&fs, 'SIRC', 'TEXT', 0))
	{
		if(!FSpOpenDF(&fs, fsRdWrPerm, &refNum))
		{
			ExportServers(servers, refNum);
			
			FSClose(refNum);
		}
	}
}

INLINE char DoDefaults(ListHandle nets, ListHandle servs, SLServHand *servers[])
{
	Point c;
	DialogPtr d;
	short i;
	
	d = GetNewDialog(501, 0, (WindowPtr)-1);
	
	SetDialogDefaultItem(d, 1);
	SetDialogCancelItem(d, 2);
	do {
		ModalDialog(0, &i);
	} while(i!= 1 && i!=2);
	
	DisposeDialog(d);
	
	if(i==1)
	{
		ReleaseServers(*servers, 0);
		
		RemoveResource((Handle)netsRes);
		netsRes = 0;
		netsR = 0;
		DeleteResourceType(mainResNum, 'SRVR');
		
		ServerOpenPrefs();
		
		fillNetsList(nets);
		fillServsList(servs, 0, *servers);

		*(long*)&c = 0;
		selectOneCell(nets, c);
		selectOneCell(servs, c);
		
		SetupServers(servers);
		return true;
	}
	else
		return false;
}

INLINE void DeleteServer(SLServHand servers[], short curNet, short curServ)
{
	SLServHand s = servers[curNet];
	
	if(s)
	{
		if(curServ < (**s).count -1)
		{
			SLServPtr sp;
			HLock((Handle)s);
			sp = *s;
			
			//move everything above curServ down one
			BlockMoveData(&sp->servs[curServ+1], &sp->servs[curServ], sizeof(SLServerRec) * (sp->count - curServ -1));
			HUnlock((Handle)s);
		}
		(**s).count--;
		SetHandleSize((Handle)s, GetHandleSize((Handle)s) - sizeof(SLServerRec));
	}
}

pascal char AddServer(SLServHand servers[], short curNet, short curServ)
{
	if(servers[curNet])
	{
		DialogPtr d;
		Str255 s;
		short i;
		long l;
		SLServHand sh = servers[curNet];

		d = GetNewDialog(502, 0, (WindowPtr)-1);
		SetDlogFont(d);
		SetDialogDefaultItem(d, 1);
		SetDialogCancelItem(d, 2);
		//SetDialogTracksCursor(d, true);
		
		if(curServ != -1)
		{
			SetText(d, 5, (**sh).servs[curServ].name);
			NumToString((**sh).servs[curServ].port, s);
			SetText(d, 6, s);
		}
		else
		{
			SetText(d, 5, "\p");
			SetText(d, 6, "\p6667");
		}
		
		do {
			ModalDialog(StdDlgFilter, &i);
		} while(i != 1 && i!= 2);
		
		if(i==1)
		{
			SetHandleSize((Handle)sh, GetHandleSize((Handle)sh) + sizeof(SLServerRec));
			
			GetText(d, 5, s);
			if(s[0]>39)
				s[0] = 39;
			pstrcpy(s, (**sh).servs[(**sh).count].name);
			GetText(d, 6, s);
			StringToNum(s, &l);
			(**sh).servs[(**sh).count].port = l;

			(**sh).count++;
		}
		
		DisposeDialog(d);
		
		return i ==1;
	}
	
	return 0;
}

INLINE char EditServer(SLServHand servers[], short curNet, short curServ)
{
	if(servers[curNet])
	{
		DialogPtr d;
		Str255 s;
		short i;
		long l;
		
		d = GetNewDialog(502, 0, (WindowPtr)-1);
		SetDlogFont(d);
		SetDialogDefaultItem(d, 1);
		SetDialogCancelItem(d, 2);
		//SetDialogTracksCursor(d, true);
		
		SetText(d, 5, (**servers[curNet]).servs[curServ].name);
		NumToString((**servers[curNet]).servs[curServ].port, s);
		SetText(d, 6, s);
		
		do {
			ModalDialog(StdDlgFilter, &i);
		} while(i != 1 && i!= 2);
		
		if(i==1)
		{
			GetText(d, 5, s);
			if(s[0]>39)
				s[0] = 39;
			pstrcpy(s, (**servers[curNet]).servs[curServ].name);
			GetText(d, 6, s);
			StringToNum(s, &l);
			(**servers[curNet]).servs[curServ].port = l;
		}
		
		DisposeDialog(d);
		
		return i ==1;
	}
	
	return 0;
}

static struct {
	ListHandle nets, servs;
	short *curNet, *curServ;
} slData;

static pascal unsigned char ServerDialogFilter(DialogPtr d, EventRecord *e, short *item)
{
	#pragma unused(item)
	GrafPtr gp;
	Point p;
	pascal unsigned char PrefsDialogFilter(DialogPtr d, EventRecord *e, short *item);
	
	if(!PrefsDialogFilter(d, e, item) || !StandardDialogFilter(d, e, item))
	{
		if(e->what == updateEvt)
		{
			if((DialogPtr)e->message == d)
			{
				RgnHandle rh;
				
				GetPort(&gp);
				SetPortDialogPort(d);
				
				rh = NewRgn();
				
				GetPortVisibleRegion(GetDialogPort(d), rh);
				
				LUpdate(rh, slData.nets);
				drawListBorder(slData.nets);
				LUpdate(rh, slData.servs);
				drawListBorder(slData.servs);
				
				p.h = 0;
				p.v = *slData.curNet;
				selectOneCell(slData.nets, p);
				p.v = *slData.curServ;
				selectOneCell(slData.servs, p);
				SetPort(gp);
				
				DisposeRgn(rh);
				
				return false; //we aren't completely processing this!
			}
		}
		
		return false;
	}
	else
		return true;
}

pascal void DoServerSelect(ServerListServiceData *p)
{
#pragma unused(p)
	GrafPtr gp;
	DialogPtr d;
	ListHandle nets, servs;
	Point c, pt;
	short i;
	short curNet;
	short curServ;
	char save = 0;
	SLServHand *servers = 0;
	ModalFilterUPP df = NewModalFilterUPP(ServerDialogFilter);
	short rf = CurResFile();
	DrawingState ts;
	WindowPtr w;
	short dType;
	Handle dItem;
	Rect netsRect;
	Rect temprect, db;
	
	UseResFile(gApplResFork);
	GetPort(&gp);
	d = GetNewDialog(500, 0, (WindowPtr)-1);
	SetPortDialogPort(d);
	SetDlogFont(d);

	w = GetDialogWindow(d);
	
	//nets = GetAppearanceListBoxHandle(d, 3);
	GetDialogItem(d, 3, &dType, &dItem, &netsRect);
	SetRect(&db, 0, 0, 1, 0);
	*(long*)&c = 0;
	netsRect.right -= 15;
	
	nets = LNew(&netsRect, &db, c, 0, w, true, false, false, true);
	(**nets).selFlags = lOnlyOne + lNoNilHilite;

	//Set up servers list
	GetDialogItem(d, 4, &dType, &dItem, &temprect);
	SetRect(&db, 0, 0, 1, 0);
	*(long*)&c = 0;
	temprect.right -= 15;
	
	servs = LNew(&temprect, &db, c, 0, w, true, false, false, true);
	(**servs).selFlags = lOnlyOne + lNoNilHilite;
	
	slData.nets = nets;
	slData.servs = servs;
	slData.curNet = &curNet;
	slData.curServ = &curServ;
	
	UseResFile(mainResNum);

	GetDrawingState(&ts);
	NormalizeDrawingState();
	
	fillNetsList(nets);
	
	//Set items
	c.h = 0;
	curNet = findNetworkType(p->network);
	if(curNet == -1)
		c.v = curNet = 0;
	else
		c.v = curNet;
	
	selectOneCell(nets, c);

	SetupServers(&servers);
	if(curNet != -1)
	{
		fillServsList(servs, curNet, servers);
		
		c.h = 0;
		c.v = findServer(servers[curNet], p->serverName);
		
		curServ = c.v;
		
		if(c.v != -1)
		{
			selectOneCell(servs, c);
			makeCellVisible(servs, c);
		}
	}
	else
		curServ = -1;

	LSetDrawingMode(true, nets);
	LSetDrawingMode(true, servs);

	//Display dialog
	SetDialogDefaultItem(d, 1);
	SetDialogCancelItem(d, 2);
	ShowWindow(GetDialogWindow(d));
	drawListBorder(nets);
	drawListBorder(servs);
	
	HiliteButtons(d, curNet, curServ);
	do {
		ModalDialog(df, &i);
		
		switch(i)
		{
			case 1: //ok
				if(curNet > -1 && curServ > -1 && servers[curNet])
				{
					p->result = 1;
					p->network = netsR->nets[curNet].type;
					pstrcpy((**servers[curNet]).servs[curServ].name, p->serverName);
					p->port = (**servers[curNet]).servs[curServ].port;
					save = true;
				}
				break;
			
			case 3: //nets list
				GetMouse(&pt);
				LClick(pt, 0, nets);
				*(long*)&c = 0;
				if(LGetSelect(true, &c, nets))
				{
					if(c.v != curNet)
					{
						curNet = c.v;
						fillServsList(servs, c.v, servers);

						*(long*)&c = 0;
						selectOneCell(servs, c);
						makeCellVisible(servs, c);
						curServ = 0;
					}
				}
				else
				{
					if(curNet != -1)
					{
						c.h = 0;
						c.v = curNet;
						selectOneCell(nets, c);
						makeCellVisible(servs, c);
					}
				}
				HiliteButtons(d, curNet, curServ);
				break;

			case 4: //servers list
				GetMouse(&pt);
				LClick(pt, 0, servs);
				*(long*)&c = 0;
				if(LGetSelect(true, &c, servs))
				{
					curServ = c.v;
				}
				else
				{
					if(curServ != -1)
					{
						c.h = 0;
						c.v = curServ;
						selectOneCell(servs, c);
						makeCellVisible(servs, c);
					}
				}
				HiliteButtons(d, curNet, curServ);
				break;
			
			case 5: //edit
				if(EditServer(servers, curNet, curServ))
					SetOneServer(servs, curNet, curServ, servers);
				break;
			
			case 7: //delete
				DeleteServer(servers, curNet, curServ);
				LDelRow(1, curServ, servs);
				c.h = 0;
				if(curServ >= (**servers[curNet]).count)
					curServ = (**servers[curNet]).count-1;
				c.v = curServ;
				selectOneCell(servs, c);
				makeCellVisible(servs, c);
				break;
			
			case 8: //add
				if(AddServer(servers, curNet, curServ))
				{
					c.h = 0;
					c.v =  (**servers[curNet]).count -1;
					curServ = c.v;
					LAddRow(1, c.v, servs);
					SetOneServer(servs, curNet, curServ, servers);
					selectOneCell(servs, c);
					makeCellVisible(servs, c);
				}
				break;
			
			case 9: //export
				DoExport(servers);
				break;
			
			case 6: //default
				if(DoDefaults(nets, servs, &servers))
				{
					curNet = 0;
					curServ = 0;
					fillServsList(servs, 0, servers);
				}
				break;
		}
	} while(i != 1 && i != 2);
	
	SetDrawingState(ts);

	LDispose(nets);
	LDispose(servs);
	DisposeDialog(d);
	SetPort(gp);
	ReleaseServers(servers, save);
	DisposeModalFilterUPP(df);
	UseResFile(rf);
}

pascal void DoServerList(ServerListServiceData *p)
{
	//Need to open ShadowIRC's prefs file first..
	OpenPrefs();
	ServerOpenPrefs();
	DoServerSelect(p);
	ClosePrefs();
}