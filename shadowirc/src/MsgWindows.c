/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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

#define MSGWINDOWS

#include <Carbon/Carbon.h>

#include "WASTE.h"

#include "LongStrings.h"
#include "StringList.h"
#include "AppearanceHelp.h"
#include "IRCGlobals.h"
#include "MsgWindows.h"
#include "WindowList.h"
#include "utils.h"
#include "connections.h"
#include "TextManip.h"
#include "channels.h"
#include "plugins.h"
#include "Inline.h"
#include "Floaters.h"
#include "filesMan.h"
#include "DragDrop.h"
#include "MWPanes.h"
#include "Events.h"
#include "MoreFilesX.h"
#include "IRCCFPrefs.h"
#include "StringKeys.h"
#include "InputLine.h"
#include "IRCInput.h"

#define kLogFolderNameKey CFSTR("LogFolderName")

MWPtr mwl = 0;

MWPtr consoleWin=0;
Rect cornerstone;

enum colorRamp {
	pct258 = 16912,
	pct322 = 21140,
	pct516 = 33825,
	pct677 = 44395,
	pct774 = 50737,
	pct1000 = -1
};

const RGBColor macColors[kMaxMacColors]=	{	{pct1000,	pct1000,	pct1000},
												{0	,			0,				0},
												{pct1000,	0,				0},
												{pct1000,	pct516,	0},
												{pct1000,	pct1000,	0},
												{pct516,	pct1000,	0},
												{0,			pct1000,	0},
												{0,			pct1000,	pct516},
												{0,			pct1000,	pct1000},
												{0,			pct516,	pct1000},
												{0,			0,				pct1000},
												{pct516,	0,				pct1000},
												{pct1000,	0,				pct1000},
												{pct1000,	0,				pct516},
												{pct677,	pct677,	pct677},
												{pct322,	pct322,	pct322},
												{pct516,	0,				0},
												{pct516,	pct258,	0},
												{pct516,	pct516,	0},
												{pct258,	pct516,	0},
												{0,			pct516,	0},
												{0,			pct516,	pct258},
												{0,			pct516,	pct516},
												{0,			pct258,	pct516},
												{0,			0,				pct516},
												{pct258,	0,				pct516},
												{pct516,	0,				pct516},
												{pct516,	0,				pct258}
											};

const RGBColor mircColors[kMaxMircColors]=	{	{pct1000,	pct1000,	pct1000},
												{0, 			0,				0},
												{0,			0,				pct516},
												{0,			pct516,	0},
												{pct1000,	0,				0},
												{pct516,	0,				0},
												{pct516,	0,				pct516},
												{pct516,	pct516,	0},
												{pct1000,	pct1000,	0},
												{0,			pct1000,	0},
												{0,			pct516,	pct516},
												{0,			pct1000,	pct1000},
												{0,			0,				pct1000},
												{pct1000,	0,				pct1000},
												{pct516,	pct516,	pct516},
												{pct774,	pct774,	pct774}
											};

static Style DoAddHunk(Style s, myStScrpHandle sty, int i, int *nsty);
static Style DoAddColorHunk(Style s, myStScrpHandle sty, int i, int *nsty, int colornum, short colorMethod);
static void DoAddRGBColorHunk(myStScrpHandle sty, int i, int *nsty, const RGBColor *front, const RGBColor *back);


static WEScrollUPP sScrollerUPP = 0;
static void TextScrolled(WEReference we);

static void ScrollBarChanged(WEReference we, long val);

static pascal void SelectLogFileNavHook(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD);
static OSStatus SelectLogFile(MWPtr mw, const CFStringRef fileName, long s0, long s1);

/*
	This function returns the frontmost message window.
*/
MWPtr GetFrontMW(void)
{
	MWPtr m;
	WindowPtr w;
	
	w = FrontNonFloatingWindow();
	while(w)
	{
		m = MWFromWindow(w);
		if(m)
			return m;
		w = GetNextWindow(w);
	}
		
	return 0;
}

/*
	This is the replacment for the MWActive global.
	If the front window is a message window, it returns that MWPtr.
	Otherwise, it returns nil.
*/
MWPtr GetActiveMW(void)
{
	WindowPtr w;

	w = FrontNonFloatingWindow();
	if(w)
		return MWFromWindow(w);
	else
		return 0;
}

pascal char MWValid(MWPtr mw)
{
	if(mw && mw->magic == MW_MAGIC)
		return true;
	else
		return false;
}

pascal channelPtr MWGetChannel(MWPtr mw)
{
	if(mw && mw->winType == chanWin)
		return mw->mwdata.chan;
	else
		return 0;
}

pascal char MWSetChannel(MWPtr mw, channelPtr ch)
{
	if(mw && mw->winType == chanWin)
	{
		mw->mwdata.chan = ch;
		if(ch)
			pstrcpy(ch->chName, mw->name);

		return true;
	}
	else
		return false;
}

pascal ConstStringPtr MWGetName(MWPtr mw, Str255 s)
{
	ConstStringPtr sp;
	
	if(mw)
	{
		if(mw->winType == chanWin)
		{
			if(mw->mwdata.chan)
				sp = mw->mwdata.chan->chName;
			else
				sp = mw->name;
			
			if(s)
				pstrcpy(sp, s);
			
			return sp;
		}
		else if(mw->winType == dccWin ||
				mw->winType == queryWin)
		{
			if(s)
				pstrcpy(mw->name, s);
			return mw->name;
		}
		else if(mw->winType == conWin)
		{
		}
		else if(mw->winType == pluginWin)
		{
		}
	}
	
	if(s)
		s[0] = 0;
	return 0;
}

pascal void MWSetNameQuery(MWPtr mw, ConstStr255Param from, linkPtr link)
{
	if(mw && from && link)
	{
		LongString ls;
		
		pstrcpy(from, mw->name);
		LSConcatStrAndStrAndStr(from, "\p: ", link->linkPrefs->linkName, &ls);
		LSMakeStr(ls);
		wmChange(mw, ls.data);
	}
}

pascal connectionPtr MWGetDCC(MWPtr mw)
{
	if(mw && mw->winType == dccWin)
		return mw->mwdata.dcc;
	else
		return 0;
}


pascal char MWSetDCC(MWPtr mw, connectionPtr dcc, Str255 name)
{
	if(mw && mw->winType == dccWin)
	{
		mw->mwdata.dcc = dcc;
		pstrcpy(name, mw->name);
		return true;
	}
	else
		return false;
}

#pragma mark -

static void ScrollBarChanged(WEReference we, long val)
{
	LongRect viewRect, destRect;
	
	WEGetViewRect(&viewRect, we);
	WEGetDestRect(&destRect, we);
	
	WEScroll(0, viewRect.top-destRect.top-val, we);
}

pascal void MWScroll(MWPtr mw, long delta)
{
	if(mw)
	{
		ControlHandle vscr = mw->vscr;
		long value;
		long max;
		
		value = GetControl32BitValue(vscr);
		max = GetControl32BitMaximum(vscr);
		
		if(((value<max) && (delta>0)) || ((value>0) && (delta<0)))
		{
			SetControl32BitValue(vscr, value+delta);
			value = GetControl32BitValue(vscr);
			ScrollBarChanged(mw->we, value);
		}
	}
}

void MWVScrollTrack(ControlRef vscr, ControlPartCode part)
{
	MWPtr mw;
	long scrollStep;
	long pageSize;
	LongRect viewRect;
	
	if(part == kControlNoPart)
		return;
	
	GetControlProperty(vscr, kApplicationSignature, MW_MAGIC, sizeof(MWPtr), NULL, &mw);
	WEGetViewRect(&viewRect, mw->we);
	pageSize = viewRect.bottom - viewRect.top;
	
	switch(part)
	{
		case kControlPageUpPart:
			scrollStep = -(pageSize - mw->scrpHeight);
			break;
			
		case kControlPageDownPart:
			scrollStep = (pageSize - mw->scrpHeight);
			break;
			
		case kControlUpButtonPart:
			scrollStep=- mw->scrpHeight;
			break;
			
		case kControlDownButtonPart:
			scrollStep = mw->scrpHeight;
			break;
		
		case kControlIndicatorPart:
			ScrollBarChanged(mw->we, GetControl32BitValue(vscr));
			scrollStep = 0;
			break;
	}
	
	if(scrollStep)
		MWScroll(mw, scrollStep);
}

pascal void MWPage(MWPtr mw, char up)
{
	LongRect viewRect;
	long pageSize;
	
	if(mw)
	{
		WEGetViewRect(&viewRect, mw->we);
		pageSize=viewRect.bottom-viewRect.top;

		if(up==1)
			MWScroll(mw, -(pageSize-mw->scrpHeight));
		else if(up==0)
			MWScroll(mw, pageSize-mw->scrpHeight);
		else if(up==3)
			MWScroll(mw, -0x6FFFFFFF);
		else if(up==2)
			MWScroll(mw, 0x6FFFFFFF);
	}
}

static void TextScrolled(WEReference we)
{
	MWPtr mw;
	
	if(WEGetInfo(weRefCon, &mw, we) == noErr)
	{
		ControlHandle bar = mw->vscr;
		LongRect viewRect, destRect ;
		long vis, tot, val, max;
		
		WEGetViewRect(&viewRect, we);
		WEGetDestRect(&destRect, we);
		
		tot = destRect.bottom - destRect.top;
		vis = viewRect.bottom - viewRect.top;
		max = tot-vis;
		val = viewRect.top - destRect.top;
		
		if(max<0)
			max=0;
		
		SetControl32BitMaximum(bar, max);
		SetControl32BitValue(bar, val);
		SetControlViewSize(bar, vis);
		
		if(val>max)
			WEScroll(0, viewRect.top-destRect.top-max, we);
	}
}

#pragma mark -

OSStatus MWLogToFSRef(MWPtr mw, const FSRef *ref, long s0, long s1)
{
	OSStatus err = noErr;
	HFSUniStr255 forkName;
	short refNum;
	Handle text;
	long textlen;
	
	err = FSGetDataForkName(&forkName);
	if((err = FSOpenFork(ref, forkName.length, forkName.unicode, fsRdWrPerm, &refNum)) == noErr)
	{
		text= WEGetText(mw->we);
		textlen=WEGetTextLength(mw->we);

		if(s0==s1)
		{
			s0=0;
			s1=textlen;
		}

		textlen=s1-s0;

		FSSetForkSize(refNum, fsFromStart, textlen);
		FSSetForkPosition(refNum, fsFromStart, 0);
		FSWriteFork(refNum, fsAtMark, 0, textlen, &((*text)[s0]), NULL);
		FSCloseFork(refNum);
	}

	return err;
}

typedef struct SelectLogFileRec {
	MWPtr mw;
	long s0;
	long s1;
} SelectLogFileRec, *SelectLogFileRecPtr;

static pascal void SelectLogFileNavHook(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
{
        OSStatus err = noErr;
	
	switch (callBackSelector)
	{
		case kNavCBUserAction:
		{
			NavReplyRecord reply;
			NavUserAction userAction = 0;
                        FSRef parentRef;
			
			if((err = NavDialogGetReply(callBackParms->context, &reply)) == noErr)
 			{
			    userAction = NavDialogGetUserAction(callBackParms->context);
			    switch(userAction)
			    {
				    case kNavUserActionSaveAs:
					    err = UnpackFSRefFromNavReply(&reply, &parentRef);
					    
					    if(err == noErr)
					    {
						    SelectLogFileRecPtr selRec = (SelectLogFileRecPtr)callBackUD;
						    FSRef ref;
						    
						    if((err = CreateFileRef(reply.saveFileName, &parentRef, (OSType)0, (OSType)'TEXT', reply.replacing, &ref)) == noErr)
							    MWLogToFSRef(selRec->mw, &ref, selRec->s0, selRec->s1);
					    }
					    break;
			    }
			    err = NavDisposeReply(&reply);
 			}
			break;
		}
		case kNavCBTerminate:	
			NavDialogDispose(callBackParms->context);
			break;
        }
}

static OSStatus SelectLogFile(MWPtr mw, const CFStringRef fileName, long s0, long s1)
{
	OSStatus err = noErr;
	NavDialogRef theDialog;
	NavDialogCreationOptions dialogOptions;
	NavEventUPP eventUPP = NULL;
	SelectLogFileRecPtr selRec = NULL; 
	
	err = NavGetDefaultDialogCreationOptions(&dialogOptions);
	dialogOptions.modality = kWindowModalityAppModal;
	dialogOptions.preferenceKey = kNavPutFile;
	
	dialogOptions.clientName = (CFStringRef) CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(), CFSTR("CFBundleName"));
	
	dialogOptions.saveFileName = CFStringCreateCopy(NULL, fileName);
	
	eventUPP = NewNavEventUPP(SelectLogFileNavHook);
	if(eventUPP == NULL)
		return paramErr;
	
	selRec = (SelectLogFileRecPtr)NewPtr(sizeof(SelectLogFileRec));
	if(selRec != NULL)
	{
		selRec->mw = mw;
		selRec->s0 = s0;
		selRec->s1 = s1;
	}
	
	if((err = NavCreatePutFileDialog(&dialogOptions, (OSType)'TEXT', (OSType)0, eventUPP, selRec, &theDialog)) == noErr)
	{
		if(theDialog != NULL)
		{
			if((err = NavDialogRun(theDialog)) != noErr)
			{
				// something went wrong, deallocate stuff and return the error
				if(dialogOptions.saveFileName)
					CFRelease(dialogOptions.saveFileName);
				if(selRec)
					DisposePtr((Ptr)selRec);
				NavDialogDispose(theDialog);
				DisposeNavEventUPP(eventUPP);
				return err;
			}
 		}
 	}
	
	if(dialogOptions.saveFileName)
		CFRelease(dialogOptions.saveFileName);
	if(selRec)
		DisposePtr((Ptr)selRec);
	DisposeNavEventUPP(eventUPP);
	
 	return err;
}

void MWLogToFile(MWPtr mw, long s0, long s1)
{
	OSStatus err = noErr;
	CFStringRef dateString;
	Str255 date, tmpDate;
	int k;
	LongString ls;

	GetDateTime(&now);
	DateString(now, shortDate, tmpDate, 0);
	k=tmpDate[0]+1;
	TimeString(now, false, &tmpDate[k], 0);
	tmpDate[0]+=tmpDate[k]+1;
	tmpDate[k]=' ';

	GetWTitle(mw->w, ls.data);
	ls.len = ls.data[0];
	k = pos(':', ls.data);
	if(k)
		LSCopyString(&ls, 1, k-1, date);
	else
		pstrcpy(ls.data, date);
	
	SAppend1(date, ' ');
	LSConcatStrAndStr(date, tmpDate, &ls);
	
	for(k=1;k<ls.len;k++)
		if(ls.data[k]==':')
		{
			LSDelete(&ls, k, k);
			k--;
		}
	
	dateString = CFStringCreateWithPascalString(NULL, date, CFStringGetSystemEncoding());
	err = SelectLogFile(mw, dateString, s0, s1);
	
	if(dateString)
		CFRelease(dateString);
}

OSStatus SetupLogFolder(FSRef *ref)
{
	OSStatus err = noErr;
	FSRef parentRef, localRef;
	CFStringRef string;
	
	err = ReadDirURLRef(kPrefLogFolder, &localRef);
	if(err != noErr)
	{
		err = FSFindFolder(kUserDomain, kDomainLibraryFolderType, kCreateFolder, &parentRef);
		if(err == noErr)
		{
			string = CFCopyLocalizedString(kLogFolderNameKey, NULL);
			err = UseDirFSRef(&parentRef, string, TRUE, &localRef);
			CFRelease(string);
			
			if((err == noErr) && (FSRefValid(&localRef)))
				BlockMoveData(&localRef, &parentRef, sizeof(FSRef));
			else
				return paramErr;
			
			string = (CFStringRef) CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(), CFSTR("CFBundleName"));
			err = UseDirFSRef(&parentRef, string, TRUE, &localRef);
			
			if((err == noErr) && (FSRefValid(&localRef)))
			{
				BlockMoveData(&localRef, ref, sizeof(FSRef));
				err = WriteDirURLRef(kPrefLogFolder, ref);
			}
			else
				err = paramErr;
		}
	}
	else
		BlockMoveData(&localRef, ref, sizeof(FSRef));
	
 	return err;
}

void MWStopLogging(MWPtr mw)
{
	if(mw->logRefNum)
	{
		Str255 s1, s2, s;
		LongString ls;
		
		GetDateTime(&now);
		DateString(now, longDate, s1, 0);
		SAppend1(s1, ' ');
		TimeString(now, true, s2, 0);
		GetWTitle(mw->w, s);
		if(s[1] == '(')
		{
			s[0]--;
			pdelete(s, 1, 1);
		}
		LSParamString(&ls, GetIntStringPtr(spInfo, sClosingLogFile), s, s1, s2, 0);
		SMPrefix(&ls, dsNowhere);
		MWMessage(mw, &ls);

		FileCloseFork(mw->logRefNum);
		mw->logRefNum=0;
	}
}

void MWStartLogging(MWPtr mw)
{
	OSStatus err = noErr;
	FSRef linkRef, ref;
	HFSUniStr255 forkName;
	CFStringRef string;
	LongString ls;
	linkPtr link;
	UniChar *nameBuf = NULL;
	UniCharCount nameBufLen = 0;
	SInt16 position = 0;
	
	if(!mw->logRefNum) //if we're already logging, then don't do anything.
	{
		if(FSRefValid(&logFolderRef))
		{
			linkRef = logFolderRef;
			link = mw->link;
			
			if(link)
			{
				FSCatalogInfo catalogInfo;
				
				catalogInfo.textEncodingHint = kTextEncodingUnicodeDefault;
				
				string = CFStringCreateWithPascalString(NULL, link->linkPrefs->linkName, CFStringGetSystemEncoding());
			
				err = UseDirFSRef(&logFolderRef, string, TRUE, &linkRef);
				if(err != noErr)
 				{
					// FAILED, display error in console and return
					LSGetIntString(&ls, spError, sCantMakeLogFolderThisConn);
					MWMessage(mw, &ls);
					CFRelease(string);
					return;
				}
				
				err = FSGetCatalogInfo(&linkRef, kFSCatInfoNodeFlags, &catalogInfo, NULL, NULL, NULL);
				if(err == noErr)
				{
					Boolean isDirectory;
					
					isDirectory = ((catalogInfo.nodeFlags & kFSNodeIsDirectoryMask) != 0);
					if(!isDirectory)
					{
						// FAILED, display error in console and return
						LSGetIntString(&ls, spError, sLogFolderThisConnIsFile);
						MWMessage(mw, &ls);
						CFRelease(string);
						return;
					}
				}
				
				CFRelease(string);
			}
			
			GetWTitle(mw->w, ls.data);
			if(ls.data[0] == '(')
			{
				ls.data[0]--;
				pdelete(ls.data, 1, 1);
			}
			ls.len = ls.data[0];
			if((position = pos(':', ls.data)) != 0)
				LSDelete(&ls, position, ls.len);
			
			string = LSCreateCFString(&ls);
			
			if(string)
			{
				nameBufLen = (UniCharCount) CFStringGetLength(string);
				nameBuf = (UniChar *)NewPtr(nameBufLen * sizeof(UniChar));
				
				if(nameBuf)
					CFStringGetCharacters(string, CFRangeMake(0, nameBufLen), &nameBuf[0]);
				
				err = FSMakeFSRefUnicode(&linkRef, nameBufLen, nameBuf, kTextEncodingUnicodeDefault, &ref);
				if(err == fnfErr)
				{
					err = CreateFileRef(string, &linkRef, (OSType)0, (OSType)'TEXT', FALSE, &ref);
					
					if(err != noErr)
						return;
				}
				
				if(nameBuf)
					DisposePtr((Ptr)nameBuf);
			}
			
			//open the file and make a note in the log that it was opened
			//MWMessage() makes sure it is appended to the *end* of the file
			err = FSGetDataForkName(&forkName);
			if((err = FSOpenFork(&ref, forkName.length, forkName.unicode, fsRdWrPerm, &mw->logRefNum)) == noErr)
			{
				Str255 date, time, filename;
				
				CFStringGetPascalString(string, filename, sizeof(Str255), CFStringGetSystemEncoding());
				
				FileAdd(mw->logRefNum, false);
				GetDateTime(&now);
				DateString(now, longDate, date, NULL);
				SAppend1(date, ' ');
				TimeString(now, TRUE, time, NULL);
				LSParamString(&ls, GetIntStringPtr(spInfo, sOpeningLogFile), filename, date, time, 0);
				SMPrefix(&ls, dsNowhere);
				MWMessage(mw, &ls);
			}
			else
				mw->logRefNum = 0;
		}
		else	// log folder is invalid
		{
			Str255 pstring;
			
			string = CFCopyLocalizedString(kNoLogFolderKey, NULL);
			CFStringGetPascalString(string, pstring, sizeof(Str255), CFStringGetSystemEncoding());
			CFRelease(string);
			LSStrLS(pstring, &ls);
			LineMsg(&ls);
		}
	}
}

#pragma mark -

pascal void MWQuote(MWPtr mw)
{
	Str255 s2;
	short i;
	
	if(mw && !mw->inactive) //do nothing if quoted
	{
		GetWTitle(mw->w, s2);
		if(s2[0]>253)
			i=253;
		else
			i=s2[0];
		BlockMoveData(&s2[1], &s2[2], i);
		s2[1]='(';
		s2[0]+=2;
		s2[s2[0]]=')';
		wmChange(mw, s2);
		mw->inactive=1;
	}
}

pascal void MWUnquote(MWPtr mw)
{
	Str255 s;
	
	if(mw && mw->inactive)
	{
		GetWTitle(mw->w, s);
		s[0]--;
		pdelete(s,1,1);
		wmChange(mw, s);
	}
}

#pragma mark -

pascal MWPtr MWFromWindow(WindowPtr w)
{
	if(w)
	{
		MWPtr x = (MWPtr)GetWRefCon(w);
		if(x && x->magic==MW_MAGIC)
			return x;
	}

	return 0;
}

//Record the change in the window position from a user drag
pascal void MWReposition(MWPtr mw)
{
	Rect r;
	WindowPtr win = mw->w;
	
	if(mw->winType == conWin)
	{
		WGetBBox(win, &r);
		if(r.bottom-r.top >= 64)
			mainPrefs->consoleLoc = r;
	}
	else if(mw->winType==chanWin)
	{
		linkPrefsPtr lp = mw->link->linkPrefs;
		
		WGetBBox(win, &r);
		if(r.bottom-r.top >= 64)
			lp->windowLoc[mw->channelWindowNumber] = r;
	}
	else if(mw->pluginRef)
	{
		pMWWindowMoveDataRec p;
		
		p.mw = mw;
		WGetBBox(win, &p.newpos);
		runIndPlugin(mw->pluginRef, pMWWindowMoveMessage, &p);
	}
}

void MWSetDimen(MWPtr win, short left, short top, short width, short height)
{
	if(win)
	{
		MoveWindow(win->w, left, top, false);
		SizeWindow(win->w, width, height, false);
	}
}

pascal void MWSetFontSize(MWPtr win, short font, short size)
{
	FontInfo f;
	TextStyle s;
	long s0,s1;
	myScrpSTElement *st;
	GrafPtr gp;
	
	if(win)
	{
		GetPort(&gp);
		SetPortWindowPort(win->w);
		
		if(font!=-1)
		{
			win->font=font;
			TextFont(font);
		}
		if(size!=-1)
		{
			win->size=size;
			TextSize(size);
		}

		GetFontInfo(&f);
		SetPort(gp);

		win->scrpHeight=f.descent+f.leading+f.ascent;
		
		st=(**(win->sty)).scrpStyleTab;
		st->scrpStartChar = 1;
		st->scrpHeight = win->scrpHeight;
		st->scrpAscent = f.ascent;
		st->scrpFont = win->font;
		st->scrpSize = win->size;
		st->scrpFace=0;
		st->scrpColor=shadowircColors[sicStandard];

		s.tsFont = win->font;
		s.tsFace = 0;
		s.tsSize = win->size;
		s.tsColor=st->scrpColor;
		
		WEGetSelection(&s0, &s1, win->we);
		WEDeactivate(win->we);
		WESetSelection(0, 0x7FFFFFFF, win->we);
		WESetStyle(weDoFont + weDoSize, &s, win->we);
		WESetSelection(s0, s1, win->we);
	}
}

//Should use GetThemeWindowRegion
pascal void MWNewPosition(Rect *windowSize)
{
	short x;
	short windowTopHeight, windowBotHeight;
	short windowLeftWid, windowRightWid;
	Rect sb;

	GetAvailableWindowPositioningBounds(GetGDevice(), &sb);
	
	if(consoleWin)
	{
		Rect crr, srr;
		RgnHandle cr, sr;
		
		cr = NewRgn();
		sr = NewRgn();
		
		GetWindowRegion(consoleWin->w, kWindowContentRgn, cr);
		GetWindowRegion(consoleWin->w, kWindowStructureRgn, sr);
		GetRegionBounds(cr, &crr);
		GetRegionBounds(sr, &srr);
		
		windowTopHeight = crr.top - srr.top;
		windowBotHeight = srr.bottom - crr.bottom;

		windowLeftWid = crr.left - srr.left;
		windowRightWid = srr.right - crr.right;

		DisposeRgn(cr);
		DisposeRgn(sr);
		SetRect(&sb, -sb.left + windowLeftWid, -sb.top + GetMBarHeight() + 16, sb.right - sb.left - windowRightWid, sb.bottom - sb.top - 35);
	}
	else
	{
		SetRect(&sb, -sb.left + 4, -sb.top + GetMBarHeight() + 16, sb.right - sb.left - 4, sb.bottom - sb.top - 35);
	}
	
	OffsetRect(&cornerstone, 8, 16);
	if(cornerstone.bottom > sb.bottom)
	{
		x=cornerstone.top-sb.top;
		cornerstone.top-=x;
		cornerstone.bottom-=x;
	}
	if(cornerstone.right > sb.right)
	{
		x=cornerstone.left-sb.left;
		cornerstone.left-=x;
		cornerstone.right-=x;
	}
	*windowSize=cornerstone;
}

#define STY_0 (sizeof(myScrpSTElement)+2)
#define STY_1 sizeof(myScrpSTElement)

pascal MWPtr MWNewQuery(ConstStr255Param name, linkPtr link)
{
	MWPtr mw;
	LongString ls;
	
	//Query windows require a link target
	if(name[0] && link)
	{
		LSConcatStrAndStrAndStr(name, "\p: ", link->linkPrefs->linkName, &ls);
		LSMakeStr(ls);
		
		mw = MWNew(ls.data, queryWin, link, 0);
		pstrcpy(name, mw->name);
		return mw;
	}
	else
		return 0;
}

pascal MWPtr MWNew(ConstStr255Param title, short winType, linkPtr link, long mwinRefCon)
{
	Rect windowSize;
	short fontNum;
	MWPtr h;
	GrafPtr p0;
	LongRect lr;
	Str255 plugTitle;
	Rect *r;
	pMWNewData pd;
	
	if((winType==conWin) || (winType==chanWin))
	{
		if(winType==conWin)
		{
			mwinRefCon=-1;
			r=&mainPrefs->consoleLoc;
		}
		else
			r=&link->linkPrefs->windowLoc[mwinRefCon];
		
		if(!EmptyRect(r))
			windowSize=*r;
		else
			MWNewPosition(&windowSize);
	}
	else // not a channel/console win
	{
		MWNewPosition(&windowSize);
		if(winType==pluginWin)
		{
			pstrcpy(title, &plugTitle[1]);
			plugTitle[1]='~';
			plugTitle[0]=title[0]+2;
			plugTitle[plugTitle[0]]='~';
			title=plugTitle;
		}
	}
	
	if(!RectInRgn(&windowSize, GetGrayRgn())) //then it's displaying offscreen.
		MWNewPosition(&windowSize);
	
	GetFNum(mainPrefs->defaultFontName, &fontNum);
	
	h=(MWPtr)NewPtr(sizeof(MessageWindow));
	if(h)
	{
		WEReference we;
		
		h->logRefNum=0;
		
		h->name[0] = 0;
		h->mwdata.data=0;

		h->link=link;
		h->inactive=0;
		h->channelWindowNumber=mwinRefCon;
		h->widgetList = 0;
		h->paneList = 0;
		h->refCon = 0;
		h->colorMethod = mainPrefs->colorMethod;
		h->protect = false;
		h->vscr = 0;
		h->we = 0;
		h->drawingStatus = 0;
		
		GetPort(&p0);

		h->w=WCreate(&windowSize, title, kWindowResizableAttribute | kWindowLiveResizeAttribute | kWindowStandardHandlerAttribute, (long)h, false);

		if(!h->w)
			goto failedWCreate;
		else
		{
			const long translucencyTreshold = 200000;
			
			SetPortWindowPort(h->w);
			TextFont(fontNum);
			TextSize(mainPrefs->defaultFontSize);

			lr.top = lr.left = lr.bottom = lr.right = 0;
			WENew(&lr,&lr, weDoOutlineHilite + weDoDragAndDrop + weDoAutoScroll, &h->we);
			if(!h->we) //failed to create the MPtr
				goto failedWENew;
			
			we = h->we;
			WESetAlignment(weFlushLeft, we);
			
			WESetInfo(weRefCon, &h, we);	//set the refcon to the MWPtr
			WESetInfo(weScrollProc, &sScrollerUPP, we);
			WESetInfo(wePreTrackDragHook, &sPreTrackerUPP, we);
			WESetInfo(weTranslucencyThreshold, &translucencyTreshold, we);
		
			h->magic=MW_MAGIC;
			h->vscr=NewControl(h->w, &windowSize, "\p", true, 0, 0, 0, kControlScrollBarLiveProc, 0);
			DeactivateControl(h->vscr);
				
			h->winType=winType;
			
			//Create the style here. This way, we don't need to continually regenerate this
			//on every call to MWMessage() for this window.
			h->sty=(myStScrpHandle)NewHandleClear(STY_0);
			(**(h->sty)).scrpNStyles=1;

			if(winType==pluginWin)
				h->pluginRef=(plugsPtr)mwinRefCon; //¥¥¥dangerous
			else
				h->pluginRef=0;
			
			h->font=fontNum;
			h->size=mainPrefs->defaultFontSize;

			if(!h->protect)
			{
				SetWindowContentColor(h->w, &shadowircColors[sicWindowBG]);
			}

			WEActivate(we);
			MWSetFontSize(h, fontNum, h->size);

//Really should NOT be assuming the panes and widgets get allocated,
//but they don't use that much RAM...
			{
				plugsPtr tempyi = sidr.yourInfo;
				if(winType != pluginWin)
					sidr.yourInfo=0;

			 	h->textPane = MWNewPane(h, mwTextPane, mwPaneCenter, -1, -1);
				h->widgetsPane = MWNewPane(h, mwWidgetsPane, mwPaneBottom, 14, -1);

				if(noFloatingInput && !h->protect)
				{
					Rect zeroRect = {0, 0, 0, 0};
					MWNewPane(h, mwInputPane, mwPaneBottom, 32, -1);
					
					h->inputData = IADNew(h->w, zeroRect, NewKey);
				}
				else
					h->inputData = 0;

			MWPaneRecalculate(h);
			MWRecalculateRects(h);
			MWPaneResize(h);

				MWNewWidget(h, mwTopicWidget, mwAlignLeft, -1);
				
			pd.mw = h;
			runPlugins(pMWNewMessage, &pd);
			
				MWNewWidget(h, mwCMWidget, mwForceLeft, 17);
				
				sidr.yourInfo =tempyi;
			}

			SetPort(p0);
			
			h->next = mwl;
			h->prev = 0;
			if(h->next)
				h->next->prev = h;
			mwl = h;

			MWInstallEventHandlers(h);
		}
	}
	return h;

//	DisposeHandle((Handle)h->sty);
failedWENew:
	DisposeWindow(h->w);
	MWDestroyAllWidgets(h);
	MWDestroyAllPanes(h);
	SetPort(p0);
failedWCreate:
	DisposePtr((Ptr)h);
	
	return 0;
}

pascal void MWDelete(MWPtr w)
{
	pMWDestroyData pd;
	MWPtr mw;
	
	if(w->w)
		HideWindow(w->w);
	MWDestroyAllWidgets(w);
	
	pd.mw = w;
	runPlugins(pMWDestroyMessage, &pd);

	MWStopLogging(w);

	MWDestroyAllPanes(w);
	wmDelete(w);
	
	linkfor(mw, mwl)
		if(mw==w)
		{
			if(mw->next)
				mw->next->prev=mw->prev;
			if(mw->prev)
				mw->prev->next=mw->next;
			if(mw==mwl)
				mwl=w->next;
			break;
		}
	
	if(w->w)
	{
		DisposeWindow(w->w);
		w->w = 0;
	}
	
	if(w->sty)
	{
		DisposeHandle((Handle)w->sty);
		w->sty = 0;
	}

	w->magic = 0; //clear magic so we aren't screwed by stray message window pointers
	w->winType = invalWin;
	DisposePtr((Ptr)w);
}

static Style DoAddHunk(Style s, myStScrpHandle sty, int i, int *nsty)
{
	myScrpSTElement *q;

	(*nsty)++;
	SetHandleSize((Handle)sty, 2+((++(**sty).scrpNStyles)*STY_1));
	q=&(**sty).scrpStyleTab[*nsty];
	*q=*(q-1);
	q->scrpStartChar=i;
	if(s & italic)
	{
		q->scrpFace = s ^ italic;
		q->extraFlags |= kInverse;
	}
	else
	{
		q->scrpFace=s;
		q->extraFlags &= ~kInverse;
	}
	
	return s;
}

static Style DoAddColorHunk(Style s, myStScrpHandle sty, int i, int *nsty, int colornum, short colorMethod)
{
	myScrpSTElement *q;

	(*nsty)++;
	SetHandleSize((Handle)sty, 2+((++(**sty).scrpNStyles)*STY_1));
	q=&(**sty).scrpStyleTab[*nsty];
	*q=*(q-1);
	q->scrpStartChar=i;
	if(s & italic)
	{
		q->scrpFace = s ^ italic;
		q->extraFlags |= kInverse;
	}
	else
	{
		q->scrpFace=s;
		q->extraFlags &= ~kInverse;
	}
	
	if(colornum>=0)
	{
		if(colorMethod==cmIrcle && colornum < kMaxMacColors)
			q->scrpColor=macColors[colornum];
		else if(colorMethod==cmMIRC && colornum < kMaxMircColors)
			q->scrpColor=mircColors[colornum];
	}
	else
		q->scrpColor=shadowircColors[(-1)-colornum];
	return s;
}

static void DoAddRGBColorHunk(myStScrpHandle sty, int i, int *nsty, const RGBColor *front, const RGBColor *back)
{
	myScrpSTElement *q;

	(*nsty)++;
	SetHandleSize((Handle)sty, 2+((++(**sty).scrpNStyles)*STY_1));
	q=&(**sty).scrpStyleTab[*nsty];
	*q=*(q-1);
	q->scrpStartChar=i;
	q->scrpColor=*front;
	if(back)
	{
		q->extraFlags |= kBackColor;
		q->scrpBackColor = *back;
	}
	else
	{
		q->extraFlags &= ~kBackColor;
	}
}

#define AddHunk(y) DoAddHunk(y, sty, i, &nsty)
#define AddColorHunk(y) DoAddColorHunk(y, sty, i, &nsty, colornum, colorMethod)
#define ToggleHunk(x) AddHunk(newstyle ^ x)

pascal void MWMessage(MWPtr win, const LongString *msg)
{
	long s0, s1, d1;
	long len;
	SInt64 nl;
	int i,ii;
	WEReference we;
	LongString ls;
	int nsty;
	Style newstyle, savedstyle;
	short c;
	int numbeeps;
	int colornum, savedcolor;
	char noScroll;
	myStScrpHandle sty;
	short colorMethod, colorMethod_saved, cm;
	char dontLog;
	unsigned char noCR;
	pMWTextDataRec p;
	char reactivate;

	if(msg->len && win)
	{
		we=win->we;

		WEGetSelection(&s0, &s1, we);
		if(s0==s1)
		{
			s1 = s0 = 0x7FFFFFFF;
			noScroll= GetControl32BitMaximum(win->vscr) != GetControl32BitValue(win->vscr);
		}
		else
			noScroll=1;
		
		ls = *msg;
		p.mw = win;
		p.ls = &ls;
		runPlugins(pMWTextMessage, &p);

		sty=win->sty;
		nsty=0;
		
		//Insert the timestamp
		if(mainPrefs->timestamp && (!mainPrefs->timestampWhenAway || ((win->link && win->link->isAway) || anyAway)))
		{
			Str255 temp;
			
			TimeString(now, mainPrefs->timestampSeconds, temp, 0);
			SAppend1(temp, ':');
			SAppend1(temp, ' ');
			DoAddRGBColorHunk(sty, 0, &nsty, &shadowircColors[sicTimestampColor], 0);
			i=temp[0]+1;
			DoAddRGBColorHunk(sty, i, &nsty, &shadowircColors[sicStandard], 0);
			LSConcatStrAndLS(temp, msg, &ls);
		}
		else
			i=1;
		
		//i is the position in the LS that we should read from.
		
		savedcolor = colornum = -2;
		savedstyle = newstyle = 0;
		numbeeps = 0;
		colorMethod_saved = colorMethod = win->colorMethod;
		noCR = dontLog = false;
		while(i<=ls.len)
		{
			c=ls.data[i];
			if(c<32)
				switch(c)
				{
					case C_BOLD:
						if(!mainPrefs->disableStyles)
							newstyle = ToggleHunk(bold);
						LSDelete(&ls, i, i);
						break;
					
					case C_UNDERLINE:
						if(!mainPrefs->disableStyles)
							newstyle = ToggleHunk(underline);
						LSDelete(&ls, i, i);
						break;
					
					case C_INVERSE:
						if(!mainPrefs->disableStyles)
							newstyle = ToggleHunk(italic);
						LSDelete(&ls, i, i);
						break;
					
					case C_NORMAL:
						colornum=-2;
						colorMethod = win->colorMethod;
						newstyle=AddColorHunk(0);
						LSDelete(&ls, i, i);
						break;
					
					case C_BEEP:
						if(++numbeeps<3 && !mainPrefs->squelchBeeps) //only beep a maximim of three times
							SysBeep(0);
						if(ls.len+1 < maxLSlen)
						{
							BlockMoveData(&ls.data[i+1], &ls.data[i+2], (ls.len - i) + 1);
							ls.data[i]='^';
							ls.data[i+1]='G';
							ls.len++;
							AddHunk(newstyle ^ bold);
							i+=2;
						}
						else
						{
							ls.data[i]='G';
							AddHunk(newstyle ^ bold);
							i++;
						}
						AddHunk(newstyle);
						break;
					
					case C_COLOR:
					{
						//Delete multiple ctrl-c's in a row
						while(i <ls.len && ls.data[i+1] == C_COLOR)
							LSDelete(&ls, i, i);
						
						if(i>=ls.len)
						{
							LSDelete(&ls, i, i);
							break;
						}
						
						cm = win->colorMethod;
						
						if(cm == cmNone)
							cm = mainPrefs->colorMethod;
							
						if(cm == cmNone || cm == cmIrcle)
						{
							//Delete the ctrl-c and the character after it.
							colornum=ls.data[i+1]-'0';
							LSDelete(&ls, i, i+1);
							if(!mainPrefs->disableColor && win->colorMethod && (colornum>=0) && (colornum<=27))
								AddColorHunk(newstyle);
						}
						else if(cm==cmMIRC)
						{
							char bail = false;
							
							//If this and the next characters are ^c, then switch to the defalt colors
							if(i < ls.len - 2)
								if(ls.data[i+1] == C_COLOR && ls.data[i+2] == C_COLOR)
								{
									colornum=-2;
									newstyle=AddColorHunk(0);
									LSDelete(&ls, i, i+2);
									break;
								}
							
							//ii = length of color run minus ctrl-c.
							ii=0;
							colornum=ls.data[i+1]-'0';
							if(colornum<=9 && colornum>=0)
							{
								ii=1;
								if(i + 1 >= ls.len)
									bail = true;
								else if(ls.data[i+2]>='0' && ls.data[i+2]<='9') //make sure we're still in the str
								{	
									ii=2; //
									colornum=colornum*10 + ls.data[i+2]-'0';
								}
								
								//Check for bg color. (c == -1 if no bg color)
								c=-1;
								if(!bail && i + ii < ls.len) //make sure we still have space
								{
									if(ls.data[i+ii+1]==',')
									{
										ii++;
										if(i + ii + 1< ls.len)
										{
											c=ls.data[i+ii+1];
											if(c<=9 || c>=0)
											{
												ii++;
												if(i + ii < ls.len && ls.data[i+ii+1]>='0' && ls.data[i+ii+1]<='9')
												{	
													ii++;
													c=c*10 + ls.data[ii]-'0';
												}
											}
										}
									}
								}
								//else
								//	bail = true;
							
								LSDelete(&ls, i, i+ii);
								if(!bail)
								{
									colornum%=16;
									if(!mainPrefs->disableColor && win->colorMethod && colornum>=0)
									{
										const RGBColor *bgColor;
										
										if(c==-1)
											bgColor = 0;
										else
											bgColor = &mircColors[c%16];
										DoAddRGBColorHunk(sty, i, &nsty, &mircColors[colornum], bgColor);
									}
								}
							}
							else //next character is not a number. delete the ctrl c and procede as normal.
								LSDelete(&ls, i, i);
						}
						break;
					}
					
					case C_SIColor:
						if(i >= ls.len)
						{
							//DebugStr("\p!colorErr!;g");
							LSDelete(&ls, i, i+1);
							i = ls.len + 1;
							break;
						}
						c = ls.data[i+1];
						LSDelete(&ls, i, i+1); //Delete the 0x08 and the character immediately folowing it.
						if((c>=0) && (c<numSIColors))
						{
							colornum = -c+(-1);
							DoAddRGBColorHunk(sty, i, &nsty, &shadowircColors[c], 0);
						}
						else if(c==sicCustomColor)
						{
							RGBColor rgb;
							short s;
							
							s=ls.data[i];
							rgb.red = s + (s<<8);

							s=ls.data[i+1];
							rgb.green = s + (s<<8);

							s=ls.data[i+2];
							rgb.blue = s + (s<<8);

							LSDelete(&ls, i, i+2); //delete the RGB value
							DoAddRGBColorHunk(sty, i, &nsty, &rgb, 0);
						}
						else if(c==sicCustomColorBG)
						{
							RGBColor rgb, back;
							short s;
							
							s=ls.data[i];
							rgb.red = s + (s<<8);
							
							s=ls.data[i+1];
							rgb.green = s + (s<<8);
							
							s=ls.data[i+2];
							rgb.blue = s + (s<<8);
							
							s=ls.data[i+3];
							back.red = s + (s<<8);
							
							s=ls.data[i+4];
							back.green = s + (s<<8);
							
							s=ls.data[i+5];
							back.blue = s + (s<<8);
							
							LSDelete(&ls, i, i+5); //delete the RGB value
							DoAddRGBColorHunk(sty, i, &nsty, &rgb, &back);
						}
						else if(c==sicDontProcess)
							i=ls.len + 1; //make sure we're past the end
						else if(c==sicSaveStyle)
						{
							savedstyle = newstyle;
							savedcolor = colornum;
						}
						else if(c==sicRestoreStyle)
						{
							newstyle = savedstyle;
							colornum = savedcolor;
							AddColorHunk(newstyle);
						}
						else if(c==sicSetColorMethod)
						{
							colorMethod = ls.data[i];
							if(colorMethod == cmDefaultColorMethod)
								colorMethod = mainPrefs->colorMethod;
							else if(colorMethod == cmWindowColorMethod)
								colorMethod = win->colorMethod;
							LSDelete(&ls, i, i); //delete the color method number
						}
						else if(c==sicSaveColorMethod)
						{
							colorMethod_saved = colorMethod;
						}
						else if(c==sicRestoreColorMethod)
						{
							colorMethod = colorMethod_saved;
						}
						else if(c==sicDontLog)
							dontLog = true;
						else if(c==sicNoCR)
							noCR = true;
						break;
					
					case 13: //CR, to please kevin
					case 9: //tab, for kevin
						i++;
						break;
						
					default: //unknown character
/*
						{
							unsigned char errchar[] = "\pBad character: ***;g";
							NumToString((unsigned char)c, &errchar[15]);
							errchar[15] = ' ';
							DebugStr(errchar);
						}
*/
						LSDelete(&ls, i, i);
				}
			else //if a character >= 32, meaning parse it...
				i++;
		}
		
		len=WEGetTextLength(we);
		reactivate = WEIsActive(we);
		WEDeactivate(we);
		if(len > 65536)
		{
			WEGetLineRange(win->vislines+3, 0, &d1, we); //don't need start
			WEFeatureFlag(weFAutoScroll, weBitClear, we);
			WESetSelection(0, d1, we);
			WEDelete(we);
			s0-=d1;
			if(s0<0)
				s0=0;
			s1-=d1;
			if(s1<0)
				s1=0;
				
			WESetSelection(s0, s1, we);
			
			if(!noScroll)
				WEFeatureFlag(weFAutoScroll, weBitSet, we);
		}
		else
			if(noScroll)
				WEFeatureFlag(weFAutoScroll, weBitClear, we);

		WESetSelection(0x7FFFFFFF, 0x7FFFFFFF, we);
		
		if(!noCR)
		{
			ls.data[0] = 13;
			WEInsert(&ls.data[0], -(ls.len+1), (StScrpHandle)sty, 0, we);
		}
		else //No insert character. So we hve to adjust all the offsets
		{
			myScrpSTElementPtr sp;
			long l, m;
			
			m = (**sty).scrpNStyles;
			sp = (**sty).scrpStyleTab;
			
			for(l = 1; l <= m; l++, sp++)
				sp->scrpStartChar--;
			
			WEInsert(&ls.data[1], -(ls.len), (StScrpHandle)sty, 0, we);
		}
		SetHandleSize((Handle)sty, STY_0);
		(**sty).scrpNStyles=1;
		WESetSelection(s0, s1, we);
		
		if(noScroll)
			WEFeatureFlag(weFAutoScroll, weBitSet, we);
		//else
			//WESelView(we);
		
		if(reactivate)
			WEActivate(we);
		
		//If noCR, then we don't want to output ls.data[0].
		if(win->logRefNum && !dontLog)
		{
			len = ls.len + !noCR;
			FSGetForkSize(win->logRefNum, &nl);
			FSSetForkSize(win->logRefNum, fsFromStart, nl + len);
			FSSetForkPosition(win->logRefNum, fsFromStart, nl);
			FSWriteFork(win->logRefNum, fsAtMark, 0, len, &ls.data[noCR], NULL);
		}
	}
}

pascal void InitMsgWindows(void)
{
	sScrollerUPP = NewWEScrollProc(TextScrolled);

	SetRect(&cornerstone, 5, GetMBarHeight() + 16, 425, 340);
	
	consoleWin=MWNew(GetIntStringPtr(spTopic, sConsole), conWin, 0, 0);
	MWNewWidget(consoleWin, mwLinkWidget, mwForceLeft, 17);
	if(mainPrefs->consoleOpen)
		WSelect(consoleWin->w);
}

pascal MWPtr NewPluginMWindow(ConstStr255Param title)
{
	MWPtr mw;
	//We need to clear the current plugin so adding the widgets and panes work as one would
	//expect.
	plugsPtr thisPlugin = sidr.yourInfo;
	sidr.yourInfo=0;
	mw = MWNew(title, pluginWin, 0, (long)thisPlugin);
	sidr.yourInfo = thisPlugin;
	
	wmAdd(mw);
	return mw;
}
