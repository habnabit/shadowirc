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

#include <Carbon/Carbon.h>

#include "WASTE.h"
#include "LongStrings.h"
#include "utils.h"
#include "MsgWindows.h"
#include "MWPanes.h"
#include "TextWindows.h"
#include "Floaters.h"
#include "IRCGlobals.h"
#include "FilesMan.h"
#include "Inline.h"
#include "IRCChannels.h"
#include "IRCInput.h"

static long untitledNum = -1;

static pascal MWPtr TWCreate(ConstStr255Param name);
static pascal char TWSaveAs(MWPtr mw, FSSpec *f);
static pascal unsigned char TWSaveWindowDialogFilter(DialogPtr d, EventRecord *e, short *item);

typedef struct textWinData {
	FSSpec file;
	short ref;
	short res;
	char saved;
} textWinData, *textWinDataPtr;

pascal void TWStatusClick(mwWidgetPtr o, Point p)
{
	#pragma unused(p)
	TWStatus(o, IsWindowHilited(o->mw->w));
}

pascal void TWStatus(mwWidgetPtr o, char winActive)
{
	#pragma unused(winActive)
	MWPtr mw = o->mw;
	long size;
	Str255 s;
	
	DrawString("\pText Window: ");
	size = WEGetTextLength(mw->we);
	NumToString(size, s);
	TextFace(0);
	DrawString(s);
	DrawString("\p bytes");
}

static pascal MWPtr TWCreate(ConstStr255Param name)
{
	MWPtr mw= MWNew(name, textWin, 0, 0);
	textWinDataPtr p;

	if(mw)
	{
		p = (textWinDataPtr)NewPtr(sizeof(textWinData));
		mw->refCon = (long)p;
		p->ref = 0;
		p->res=0;
		p->saved = 0;
		iwFront=0;
		WEFeatureFlag(weFUndo, weBitSet, mw->we);
		
		SetWindowProxyCreatorAndType(mw->w, 'SIRC', 'TEXT', kOnSystemDisk);
	}
	
	return mw;
}

pascal MWPtr NewTextWin(void)
{
	Str255 s, sn;
	MWPtr mw;
	MWPtr x;
	
	mw=0;
	linkfor(x, mwl)
		if(x->winType == textWin)
		{
			mw=x;
			break;
		}
	
	if(!mw)
		untitledNum = -1;
	
	pstrcpy("\puntitled", s);
	
	if(++untitledNum)
	{
		SAppend1(s, ' ');
		
		NumToString(untitledNum, sn);
		pstrcat(s, sn, s);
	}
	else
		untitledNum = 1;
	
	mw= TWCreate(s);
	if(mw)
		WSelect(mw->w);
	return mw;
}

pascal MWPtr TWOpen(const FSSpec *f)
{
	short ref;
	MWPtr mw = 0;
	long size;
	Ptr data;
	short err;
	textWinDataPtr p;
	
	err = FSpOpenDF(f, fsRdWrPerm, &ref);
	if(!err)
	{
		char memerr = 0;
		
		FileAdd(ref, false);
		GetEOF(ref, &size);
		if(FreeMem()-(size*2) < 6144) //low on mem. need size * 2 to handle file read in and then inserting to waste, plus 6kb for spare...
		{
			memerr = 1;
		}
		else
		{
			data = NewPtr(size);
			err = MemError();
			if(!err)
			{
				SetFPos(ref, fsFromStart, 0);
				FSRead(ref, &size, data);

				mw = TWCreate(f->name);
				if(mw)
				{
					p=(textWinDataPtr)mw->refCon;
					p->ref = ref;
					p->file = *f;
					p->saved = 1;
					err = WEInsert(data, size, 0, 0, mw->we);
					
					if(!err)
					{
						WEResetModCount(mw->we);
						MWPaneResize(mw);
						WEActivate(mw->we);
						WESetSelection(0, 0, mw->we);
						WESelView(mw->we);
						
						SetWindowProxyFSSpec(mw->w, f);
						SetWindowModified(mw->w, false);
						WSelect(mw->w);
					}
				}
				
				if(err)
				{
					memerr = 1;
					MWDelete(mw);
					mw = 0;
				}
				
				DisposePtr(data);
			}
			else
				memerr = 1;
		}
		
		if(memerr)
		{	
			DialogPtr d = GetNewDialog(139, 0, (WindowPtr)-1);
			ParamText(f->name, "\p", "\p", "\p");
			SetupModalDialog(d, 1, 1);
			do {
				ModalDialog(0, &err);
			} while(err != 1);
			DisposeDialog(d);
			FinishModalDialog();
			FileClose(ref);
		}
	}
	else //error opening
	{
		if(err == -49) //file already openbed with write permission
		{
			//Troll through the text windows and find one with this refnum.
			//if it exists, select it.
			
			MWPtr mw;
			linkfor(mw, mwl)
				if(mw->winType == textWin)
					if(((textWinDataPtr)mw->refCon)->ref == ref)
					{
						WSelect(mw->w);
						return mw;
					}
		}
	}
	
	return mw;
}

pascal MWPtr TWSelect(void)
{
	MWPtr retval = 0;
	
	AEKeyword key;
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	OSErr				theErr;
	long				count;
	FSSpec	finalFSSpec;	
	AEDesc 	resultDesc;
	FInfo	fileInfo;
	long index;
	NavTypeListHandle	openList;

	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	dialogOptions.preferenceKey = kNavGetFile;
	dialogOptions.dialogOptionFlags = kNNoTypePopup;
	
	pstrcpy("\pShadowIRC", dialogOptions.clientName);
	openList = (NavTypeListHandle)GetResource('open', kOpenText);

	theErr = NavGetFile(0, &theReply, &dialogOptions, StdNavFilter, 0, 0, openList, 0);
	
	if (openList != NULL)
		ReleaseResource((Handle)openList);

	if(theReply.validRecord && !theErr)
	{
		// we are ready to open the document(s), grab information about each file for opening:
		count = 0;
		theErr = AECountItems(&(theReply.selection),&count);
		for (index=1;index<=count;index++)
			{
			resultDesc.dataHandle = 0L;
			if ((theErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, &key,&resultDesc)) == noErr)
			{
				AEGetDescData(&resultDesc, &finalFSSpec, sizeof(FSSpec));
			
				// decide if the doc we are opening is a 'PICT' or 'TEXT':
				if ((theErr = FSpGetFInfo(&finalFSSpec,&fileInfo)) == noErr)
					{
					if (fileInfo.fdType == 'TEXT')
						retval = TWOpen(&finalFSSpec);
					else
							{
							// error:
							// if we got this far, the document is a type we can't open and
							// (most likely) built-in translation was turned off.
							// You can alert the user that this returned selection or file spec
							// needs translation.
							}
					}
				theErr = AEDisposeDesc(&resultDesc);
				}
			}
	}
	theErr = NavDisposeReply(&theReply);	// clean up after ourselves	
	
	return retval;
}

static pascal unsigned char TWSaveWindowDialogFilter(DialogPtr d, EventRecord *e, short *item)
{
	char handled = 0;
	char c;
	
	switch(e->what)
	{
		case keyDown:
			c=e->message % 256;
			if(c=='d' || c== 'D')
			{
				*item = 3;
				return true;
			}
	}
	
	if(!handled)
		return StandardDialogFilter(d, e, item);
	else
		return true;
}

pascal char TWClose(MWPtr mw, char lowMem)
{
	textWinDataPtr p =  (textWinDataPtr)mw->refCon;
	DialogPtr d;
	short i;
	ModalFilterUPP df;
	Str255 s;
	
	if(WEGetModCount(mw->we)) //ask to save
	{
		d = GetNewDialog(137 + lowMem, 0, (WindowPtr)-1);
		if(p->saved)
			pstrcpy(p->file.name, s);
		else
			GetWTitle(mw->w, s);
		ParamText(s, "\p", "\p", "\p");
		df = NewModalFilterUPP(TWSaveWindowDialogFilter);
		SetupModalDialog(d, 1, 2);
		i=0;
		do
		{
			ModalDialog(df, &i);
		} while(!i);
		
		DisposeDialog(d);
		FinishModalDialog();
		DisposeModalFilterUPP(df);
		
		switch(i)
		{
			case 1:
				TWSave(mw, false);
				break;
			
			case 2:
				return 0;
			
			case 3:
				break;
		}
	}

	//Close the file
	if(p->ref)
		FileClose(p->ref);
	if(p->res)
		FileClose(p->res);
	DisposePtr((Ptr)p);
	MWDelete(mw);
	return 1;
}

pascal char TWCloseAll(void)
{
	MWPtr mw, mw2;
	char r = true;
	
	mw = mwl;
	while(mw)
	{
		mw2=mw->next;
		if(mw->winType == textWin)
			r=TWClose(mw, false);
		if(!r)
			return false;
		mw=mw2;
	}
	
	return true;
}

pascal void TWSave(MWPtr mw, char saveas)
{
	textWinDataPtr p = (textWinDataPtr)mw->refCon;
	char ok;
	Handle text;
	long textlen;
	SInt8 hstate;
	FSSpec f;
	short ref;
	
	if(p->saved)
	{
		if(saveas)
			ok = TWSaveAs(mw, &f);
		else
			ok = 1;
	}
	else //save as
	{
		if((ok = (TWSaveAs(mw, &f))) != 0)
		{
			SetWTitle(mw->w, f.name);

			saveas = 1;
		}
	}
	
	if(ok) //ok to save
	{
		if(saveas || !p->saved) //make a new file
		{
			if(FSpCreate(&f, 'SIRC', 'TEXT', 0))
				return;
			if(FSpOpenDF(&f, fsRdWrPerm, &ref))
				return;
		}
		else if(!saveas && p->saved)
			ref = p->ref;

		WEResetModCount(mw->we);
		text=WEGetText(mw->we);
		textlen = WEGetTextLength(mw->we);
		SetEOF(ref, textlen);
		SetFPos(ref, fsFromStart, 0);
		hstate=HGetState(text);
		HLock(text);
		FSWrite(ref, &textlen, *text);
		HSetState(text, hstate);
		
		SetWindowProxyFSSpec(mw->w, &f);
		SetWindowModified(mw->w, false);

		if(saveas)
		{
			if(p->saved) //close
				FileClose(p->ref);
			p->file = f;
			p->ref = ref;
			
			SetWTitle(mw->w, f.name);
		}
		p->saved = 1;
		
	}
}

static pascal char TWSaveAs(MWPtr mw, FSSpec *f)
{
	Str255 s;
	
	GetWTitle(mw->w, s);
	return MyStandardPutFile("\p", s, 'TEXT', 'SIRC', kNDefault, f, true) <= 0;
}
