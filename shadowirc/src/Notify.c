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

#include "Notify.h"
#include "inline.h"
#include "utils.h"

typedef struct NotifyRec {
	struct NotifyRec *next;
	NMRec n;
} NotifyRec, *NotifyPtr;

static NMUPP nmResponseProc = 0;
static NotifyPtr nmList = 0;
	
static pascal NMRecPtr NotifyAdd(void);
static pascal void NotifyReply(NMRec *n);

static pascal NMRecPtr NotifyAdd(void)
{
	NotifyPtr n = (NotifyPtr)NewPtr(sizeof(NotifyRec));
	n->next = nmList;
	nmList = n;
	return &n->n;
}

pascal void Notify(char mark, Handle icon, Handle sound, ConstStr255Param s)
{
	NMRecPtr n = NotifyAdd();

	n->qType = 8;
	n->nmMark = mark;
	
	if((long)icon == -1)
		n->nmIcon = GetResource('SICN', 128);
	else
		n->nmIcon = icon;
	
	n->nmSound = sound;
	n->nmStr = NewPString(s);
	if(!nmResponseProc)
		nmResponseProc = NewNMUPP(NotifyReply);	
	n->nmResp = nmResponseProc;
	n->nmRefCon = 0;
	NMInstall(n);
}

static pascal void NotifyReply(NMRec *n)
{
	DisposePtr((Ptr)n->nmStr);
	n->nmStr = 0;
}

pascal void NotifyRemove(void)
{
	NotifyPtr n, n2;
	
	n = nmList;
	while(n)
	{
		n2 = n->next;
		if(n->n.nmStr)
			DisposePtr((Ptr)n->n.nmStr);
		NMRemove(&n->n);
		DisposePtr((Ptr)n);
		n=n2;
	}
	
	nmList = 0;
}