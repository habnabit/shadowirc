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

/*	ChangeLog
	2000-10-04	JB	Fixed prefs code. & has lower precedence than !=. Doh!
	2000-09-16	JB	Merged in CJB's (Chris Behm) modifications.
									Strict Matching
									Auto-hilight current nick
*/

#include <Sound.h>
#include "ShadowIRC.h"

#define linkfor(list, init) for((list) = (init); (list); (list)=(list)->next)

static ShadowIRCDataRecord* sidr;

typedef struct strN
{
	short count;
	unsigned char strs[0];
} strN, *strnPtr, **strnHand;

typedef struct WordPref
{
	Str63 word;	//Size should to match length of nick
	char matchStrict;
} WordPref, *WordPrefPtr;

typedef struct GPrefs
{
	long version;
	
	char boldToken, boldNick;
	char autoHilightCurNick;
	char reserved;
} GPrefs;

typedef struct Prefs
{
	GPrefs prefs;
	
	long numWords;
	WordPref p[];
} Prefs, *PrefsPtr, **PrefsHand;


// Prototypes
static WordPrefPtr SearchLS(LongString *text, ConstStr63Param curNick);
static void HandleChanMsg(pServerPRIVMSGDataRec *p);
static void PrefsWindowSet(pPWSetWindowDataPtr p);
static void PrefsWindowGet(pPWGetWindowDataPtr p);
static void PrefWinRedraw(pPWRedrawDataPtr p);
static ListHandle BuildTheList(DialogPtr dlog);
static void AddThisItem(ListHandle listHdl, ConstStr255Param theString);
static void ReadPrefs(void);

// Constants
#define kPrefsFileName "\pHighlight Preferences"
#define preferencesDitl 4242

#define kCurrentVersion			0x01010001	//aa bb cc dd (major, minor, debug, prefs)
#define kPrefVersionMask			0x000000FF
#define kCurrentPrefsVersion	(kCurrentVersion & kPrefVersionMask)

// Prefs Struct
/*
typedef struct PrefsRec
{
	Boolean	boldToken;
	Boolean boldNick;
	Boolean playSnd;
	Boolean colorCode;
	short	color;
	FSSpec	sndFile;
} PrefsRec, **PrefsHandle;
*/

// DITL Item Constants
enum
{
	iButtonGroup	= 4,
	iAddButton,
	iRemoveButton,
	iAliasList,
	iAliasEdit,
	iAliasText,
	iBoldText,
	iBoldNick,
	iSelectSnd,
	iSoundCheck,
	iNickHilight,
	iStrictMatch,
	iColorPopup
};

// Globals
static short prefPanel;
static ListHandle aliasList;
static PrefsHand prefs;

/* ** ****************************************************** ** */

inline char IsWS(char c)
{
	if(c==' ' || c== 10 || c==13 || c=='\t')
		return 1;
	else
		return 0;
}

static char IsPunc(char c)
{
	switch(c)
	{
		case '!':
		case '@':
		case '(':
		case ')':
		case '.':
		case ',':
		case ':':
		case '+':
		case '"':
		case '\'':
		case '?':
		case '<':
		case '>':
		case '*':
		case '-':
//		case '_':
			return 1;
		
		default:
			if(c == '_')
				return 1;
			return 0;
	}
}
static char ValidPosition(LongString *text, int start, int end)
{
	return(		(start == 1 || IsWS(text->data[start-1]) || IsPunc(text->data[start-1])) //begin ok
		&& (end == text->len || IsWS(text->data[end+1]) || IsPunc(text->data[end+1]))); //end ok
}

static const unsigned char setstyle[] = {5, 8, sicSaveStyle, 2,  8, sicPrivmsg};
static const unsigned char restoreStyle[] = {2, 8, sicRestoreStyle};

static char DoSearch(LongString *text, WordPrefPtr wp)
{
	int i, max;
	int x = wp->word[0];
	Str255 string;
	
	max = text->len - x + 1;
	// Search that text
	for(i = 1; i <= max; i++)
	{
		LSCopyString(text, i, x, string);
		
		if(pstrcasecmp(wp->word, string) && (!wp->matchStrict || ValidPosition(text, i, i + x - 1)))
		{
//if(wp->boldToken)
			if((**prefs).prefs.boldToken)
			{
				LSInsertStr(setstyle, i-1, text);
				LSInsertStr(restoreStyle, i+x+setstyle[0]-1, text);
			}
			
			return true;
		}
	}
	
	return false;
}

// Search for our nick
static WordPrefPtr SearchLS(LongString *text, ConstStr63Param curNick)
{
	static WordPref CurrentNick = {"\p", 0};
	PrefsPtr pp = *prefs;
	int num = pp->numWords;
	int x;
	
	for(x=0; x < num; x++)
		if(DoSearch(text, &pp->p[x]))
			return &pp->p[x];
	
	if(pp->prefs.autoHilightCurNick)
	{
		pstrcpy(curNick, CurrentNick.word);
		if(DoSearch(text, &CurrentNick))
			return &CurrentNick;
	}
	
	return false;
}
	
// Handle that message
static void HandleChanMsg(pServerPRIVMSGDataRec *p)
{
	WordPrefPtr wp;
	
	if(p->targChan == true)
	{
		wp = SearchLS(p->message, p->link->CurrentNick);
		if(wp)
		{
//if(wp->boldNick)
			if((**prefs).prefs.boldNick)
				*(long *)p->nickStyle = 0x03020802;
			
/*
			if(wp->playSnd)
			{
				Handle snd = NULL;
				short  save, file;
				
				save = CurResFile();
				// Play a sound
				file = FSpOpenResFile(&(*prefs)->sndFile, fsRdPerm);
				UseResFile(file);
				snd = GetIndResource('snd ', 1);
				if(snd)
				{
					HLock(snd);
					err = AsyncSoundPlay(snd, 0, nil);
				}
				CloseResFile(file);
				UseReesFile(save);
			}
*/
		}
	}
}

static void HandleCTCPMsg(pCTCPDataPtr p)
{
	WordPrefPtr wp;
	
	if(pstrcmp("\pACTION", p->cmd))
	{
		LongString ls;
		
		LSStrLS(p->rest, &ls);
		wp = SearchLS(&ls, p->link->CurrentNick);
		
		if(wp)
		{
			
		}
	}
}

// Set the value of our Prefs
static void PrefsWindowSet(pPWSetWindowDataPtr p)
{
	if(p->id == prefPanel)
	{
		aliasList = BuildTheList(p->PrefsDlg);

		setCheckBox(p->PrefsDlg, iBoldText, (**prefs).prefs.boldToken);
		setCheckBox(p->PrefsDlg, iBoldNick, (**prefs).prefs.boldNick);
		//setCheckBox(p->PrefsDlg, iSoundCheck, (*prefs)->playSnd);
		setCheckBox(p->PrefsDlg, iNickHilight, (**prefs).prefs.autoHilightCurNick);
		
		setButtonEnable(p->PrefsDlg, iAddButton, false);
		setButtonEnable(p->PrefsDlg, iRemoveButton, (**aliasList).dataBounds.bottom >= 1);
		setButtonEnable(p->PrefsDlg, iStrictMatch, false);
		// SetDialogItemValue(p->PrefsDlg, iColorPopup, (*prefs)->color + 1);
	}
}

// Get our Prefs values
static void PrefsWindowGet(pPWGetWindowDataPtr p)
{
	if(p->id == prefPanel)
	{
		(**prefs).prefs.boldToken = getCheckBox(p->PrefsDlg, iBoldText);
		(**prefs).prefs.boldNick = getCheckBox(p->PrefsDlg, iBoldNick);
		(**prefs).prefs.autoHilightCurNick = getCheckBox(p->PrefsDlg, iNickHilight);
		//(**prefs).prefs.playSnd = getCheckBox(p->PrefsDlg, iSoundCheck);
		// (*prefs)->color = GetDialogItemValue(p->PrefsDlg, iColorPopup) - 1;
	}
}

// Redraw our list
static void PrefWinRedraw(pPWRedrawDataPtr p)
{
#pragma unused(p)
	DrawingState ts;
	
	GetDrawingState(&ts);
	NormalizeDrawingState();
	
	SetDrawingState(ts);
}

static void ResizePrefs(void)
{
	SetHandleSize((Handle)prefs, (**prefs).numWords * sizeof(WordPref) + sizeof(long) + sizeof(GPrefs));
}

static int PFindWord(ConstStr255Param st)
{
	int max = (**prefs).numWords;
	int x;
	
	for(x = 0; x < max; x++)
		if(pstrcasecmp(st, (**prefs).p[x].word))
			return x;
	
	return -1;
}

static char DoWordMatchOnWord(ConstStr255Param inWord)
{
	int w = PFindWord(inWord);
	
	if(w != -1)
		return (**prefs).p[w].matchStrict;
	else
		return false;
}

static char AddWord(ConstStr255Param st)
{
	int w = PFindWord(st);
	
	if(w == -1)
	{
		WordPrefPtr p;
		int x = (**prefs).numWords++;
		
		ResizePrefs();
		
		p = &(**prefs).p[x];
		
		pstrcpy(st, p->word);
	//	p->boldToken = 
	//	p->boldNick = 1;
		p->matchStrict = true;
		return true;
	}
	else
		return false;
}

static void DeleteWord(ConstStr255Param st)
{
	int delWord;
	
	delWord = PFindWord(st);
	if(delWord >= 0)
	{
		WordPrefPtr a;
		int siz;
		
		HLock((Handle)prefs);
		
		//Move everything above down
		a = (**prefs).p;
		siz = (**prefs).numWords - delWord - 1;
		BlockMoveData(&a[delWord+1], &a[delWord], siz);
		
		HUnlock((Handle)prefs);
		
		(**prefs).numWords--;
		ResizePrefs();
	}
}


static void PrefWinHit(pPWItemHitDataPtr p)
{
	if(p->id == prefPanel)
	{
		Str255 st;
		DrawingState ts;
		
		SetPort(p->PrefsDlg);
		
		GetDrawingState(&ts);
		NormalizeDrawingState();
		
		switch (p->itemNum)
		{
			case iAliasList:
			{
				Cell cell;
				short offset, dataLen;
				
				cell.h = cell.v = 0;
				if(LGetSelect(true, &cell, aliasList))
				{
					LGetCellDataLocation(&offset, &dataLen, cell, aliasList);
					LGetCell(&st[1], &dataLen, cell, aliasList);
					st[0] = (SInt8)dataLen;
					setButtonEnable(p->PrefsDlg, iStrictMatch, true);
					setCheckBox(p->PrefsDlg, iStrictMatch, DoWordMatchOnWord(st));
				}
				else
				{
					setCheckBox(p->PrefsDlg, iStrictMatch, false);
					setButtonEnable(p->PrefsDlg, iStrictMatch, false);
				}
				break;
			}
			
			case iAddButton:
			GetText(p->PrefsDlg, iAliasEdit, st);
			SetText(p->PrefsDlg, iAliasEdit, "\p");
			if(st[0] && AddWord(st))
		 	{
				AddThisItem(aliasList, st);
				setButtonEnable(p->PrefsDlg, iRemoveButton, 1);
			}
			break;
			
			case iRemoveButton:
			{
				Cell cell;
				short offset, dataLen;
				
				cell.h = cell.v = 0;
				if(LGetSelect(true, &cell, aliasList))
				{
					LGetCellDataLocation(&offset, &dataLen, cell, aliasList);
					LGetCell(&st[1], &dataLen, cell, aliasList);
					st[0] = (SInt8)dataLen;
					DeleteWord(st);
					LDelRow(1, cell.v, aliasList);
					
					if((**aliasList).dataBounds.bottom < 1)
						setButtonEnable(p->PrefsDlg, iRemoveButton, 0);
					else
					{
						if(cell.v >= (**aliasList).dataBounds.bottom)
							cell.v--;
						selectOneCell(aliasList, cell);
					}
				}
			}
			break;
			
			case iAliasEdit:
				GetText(p->PrefsDlg, iAliasEdit, st);
				setButtonEnable(p->PrefsDlg, iAddButton, st[0]);
				break;
			
			case iBoldText:
			case iBoldNick:
			case iNickHilight:
//			case iSoundCheck:
				setCheckBox(p->PrefsDlg, p->itemNum, !getCheckBox(p->PrefsDlg, p->itemNum));
				break;
			
			case iStrictMatch:
			{
				Cell cell;
				short offset, dataLen;
				
				setCheckBox(p->PrefsDlg, p->itemNum, !getCheckBox(p->PrefsDlg, p->itemNum));
				cell.h = cell.v = 0;
				if(LGetSelect(true, &cell, aliasList))
				{
					int w;
					LGetCellDataLocation(&offset, &dataLen, cell, aliasList);
					LGetCell(&st[1], &dataLen, cell, aliasList);
					st[0] = (SInt8)dataLen;
					w = PFindWord(st);
					if(w != -1)
						(**prefs).p[w].matchStrict = getCheckBox(p->PrefsDlg, p->itemNum);
				}
				break;
			}

/*
			case iSelectSnd:
			{
				SFTypeList types = {'sfil', 0, 0, 0};
				StandardFileReply reply;
				
				StandardGetFile(nil, 1, types, &reply);
				if (reply.sfGood)
					(*prefs)->sndFile = reply.sfFile;
				break;
			}
*/
		}
			
		SetDrawingState(ts);
	}
}
	
// Make the list
static ListHandle BuildTheList(DialogPtr dlog)
{
	ListHandle textListHdl;
	DrawingState ts;
	StringPtr	theString;
	Cell cell;
	PrefsPtr p;
	int x, max;
	
	GetDrawingState(&ts);
	NormalizeDrawingState();

	textListHdl = GetAppearanceListBoxHandle(dlog, iAliasList);
	(**textListHdl).selFlags = lOnlyOne + lNoNilHilite;

	HLock((Handle)prefs);
	p = *prefs;
	max = p->numWords;
	
	LAddRow(max, (**textListHdl).dataBounds.bottom, textListHdl);
	
	SetPt(&cell,0,0);
	for(x = 0; x < max; x++)
	{
		theString = p->p[x].word;
		LSetCell(&theString[1], theString[0], cell, textListHdl);
		cell.v++;
	}
	
	SetPt(&cell,0,0);
	LSetSelect(true, cell, textListHdl);
	LSetDrawingMode(true, textListHdl);
	
	SetDrawingState(ts);
	
	return textListHdl;
}

// Throw this item in the list
static void AddThisItem(ListHandle listHdl, ConstStr255Param theString)
{
	Cell cell;
	
	cell.h = 0;
	cell.v = (**listHdl).dataBounds.bottom;
	LAddRow(1, cell.v, listHdl);
	LSetCell(&theString[1], theString[0], cell, listHdl);
	selectOneCell(listHdl, cell);
}

static void WritePrefs(void)
{
	short refNum;
	long siz;
	OSErr err;
	
	if(!PFExists(kPrefsFileName)) //create it
		err = PFCreate(kPrefsFileName, 'pref', 'sirc', false);
		
	err = PFOpen(kPrefsFileName, false, &refNum, 0);
	if(!err)
	{
		siz = GetHandleSize((Handle)prefs);
		SetEOF(refNum, siz);
		SetFPos(refNum, fsFromStart, 0);
		HLock((Handle)prefs);
		
		(**prefs).prefs.version = kCurrentVersion;
		
		FSWrite(refNum, &siz, *prefs);
		HUnlock((Handle)prefs);
		PFClose(refNum);
	}
}

//Open the prefs file and read in the prefs
static void ReadPrefs(void)
{
	short refNum;
	OSErr err;
	long siz;
	char ok;
	
	if(PFExists(kPrefsFileName))
	{	
		err = PFOpen(kPrefsFileName, false, &refNum, 0);
		if(!err)
		{
			err = GetEOF(refNum, &siz);
			
			if(siz >= sizeof(long) + sizeof(GPrefs)) //valid size
			{
				prefs = (PrefsHand)NewHandle(siz);
				HLock((Handle)prefs);
				FSRead(refNum, &siz, *prefs);
				HUnlock((Handle)prefs);
				PFClose(refNum);

				if(((**prefs).prefs.version & kPrefVersionMask) != kCurrentPrefsVersion) //version bad
				{
					DisposeHandle((Handle)prefs);
					ok = false;
				}
				else
					ok = true;
			}
		}
		
		if(ok)
			return;
	}

	prefs = (PrefsHand)NewHandleClear(sizeof(GPrefs) + sizeof(long));
	(**prefs).prefs.boldToken = 
	(**prefs).prefs.boldNick = 1;
	WritePrefs();
}

// Turn on the messages we need
static void SetupMessages(char captureMessages[numMessages])
{
	captureMessages[pServerPRIVMSGMessage] = 
	captureMessages[pCTCPMessage] = 
	captureMessages[pSavePreferencesMessage] = 1;
}

static pascal void displayOldVersionMsg(void)
{
	LongString ls;
	
	LSStrLS("\pThe highlight plugin requires ShadowIRC 1.1 or later.", &ls);
	if(!WIsVisible((*sidr->consoleWin)->w))
		WShow((*sidr->consoleWin)->w);
	SMPrefixIrcleColor(&ls, dsConsole, '2');
}

// Entry point of our code
pascal void main(ShadowIRCDataRecord* sidrIN)
{
	unsigned long l;
	
	switch (sidrIN->message)
	{
		case pVersionCheckMessage:
			sidr=sidrIN;
			l=((pVersionCheckDataPtr)sidrIN->messageData)->version;
			if(l<0x01010042) //we must have ShadowIRC 1.1d66 or later.
			{
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionShadowIRCTooOld;
				displayOldVersionMsg();
			}
			else
			{
				l = _UndocumentedAPI('hlgt', 0);
				if(l)
					((pVersionCheckDataPtr)sidrIN->messageData)->version = l;
				else
					((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionCheckMessageReply;
			}
			break;
		
		case pInitMessage:
			SetupMessages(sidrIN->yourInfo->captureMessages);
			prefPanel = PMLAdd("\pHighlight");
			ReadPrefs();
			break;
		
		case pQuitMessage:
			break;
		
		case pSavePreferencesMessage:
			WritePrefs();
			break;
		   
		case pPWActivateMessage:
			if(((pPWActivateDataPtr)sidrIN->messageData)->id == prefPanel)
				((pPWActivateDataPtr)sidrIN->messageData)->ditlNum = preferencesDitl;
			break;
			
		case pPWSetWindowMessage:
			PrefsWindowSet((pPWSetWindowDataPtr)sidrIN->messageData);
			break;
			
		case pPWGetWindowMessage:
			PrefsWindowGet((pPWGetWindowDataPtr)sidrIN->messageData);
			break;
		
		case pPWRedrawMessage:
			PrefWinRedraw((pPWRedrawDataPtr)sidrIN->messageData);
			break;
			
		case pPWItemHitMessage:
			PrefWinHit((pPWItemHitDataPtr)sidrIN->messageData);
			break;
			
		case pServerPRIVMSGMessage:
			HandleChanMsg((pServerPRIVMSGDataPtr)sidrIN->messageData);
			break;
		
		case pCTCPMessage:
			HandleCTCPMsg((pCTCPDataPtr)(sidrIN->messageData));
			break;
		
		case pAsyncSoundCompletionMessage:
			HUnlock(((pAsyncSoundCompletionDataPtr)sidrIN->messageData)->sound);
			ReleaseResource(((pAsyncSoundCompletionDataPtr)sidrIN->messageData)->sound);
			break;
		}
}