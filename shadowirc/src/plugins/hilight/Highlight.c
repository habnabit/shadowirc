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

#include "ShadowIRC.h"

static ShadowIRCDataRecord* sidr;

typedef struct WordPref
{
	Str63 word;	//Size should to match length of nick
	char matchStrict;
	char unused[3];
} WordPref, *WordPrefPtr, **WordPrefHand;

typedef struct WordPrefs
{
	long numWords;
	WordPrefHand words;
} WordPrefs;

//Prefs keys
#define kHighlightPrefsDictName CFSTR("highlight")

#define kHighlightPrefsSubDict CFSTR("prefs")
#define kPrefBoldToken CFSTR("boldToken")
#define kPrefBoldNick CFSTR("boldNick")
#define kPrefAutoHilightCurNick CFSTR("autoHilightCurNick")

#define kHighlightPrefsWordsArr CFSTR("words")
#define kPrefWord CFSTR("word")
#define kPrefWordMatchStrict CFSTR("matchStrict")

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
#define preferencesDitl 4242

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
	iNickHilight,
	iStrictMatch
};

// Globals
static short prefPanel;
static ListHandle aliasList;

static WordPrefs wordPrefs;

static struct {
	char boldToken, boldNick;
	char autoHilightCurNick;
} prefs = {
	1, 1,
	0
};


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
		&& (end > text->len || IsWS(text->data[end+1]) || IsPunc(text->data[end+1]))); //end ok
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
		
		if(pstrcasecmp(wp->word, string) && (!wp->matchStrict || ValidPosition(text, i, i + x)))
		{
//if(wp->boldToken)
			if(prefs.boldToken)
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
	WordPrefPtr pp = *wordPrefs.words;
	int num = wordPrefs.numWords;
	int x;
	
	for(x=0; x < num; x++)
		if(DoSearch(text, &pp[x]))
			return &pp[x];
	
	if(prefs.autoHilightCurNick)
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
			if(prefs.boldNick)
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

		setCheckBox(p->PrefsDlg, iBoldText, prefs.boldToken);
		setCheckBox(p->PrefsDlg, iBoldNick, prefs.boldNick);
		//setCheckBox(p->PrefsDlg, iSoundCheck, (*prefs)->playSnd);
		setCheckBox(p->PrefsDlg, iNickHilight, prefs.autoHilightCurNick);
		
		setButtonEnable(p->PrefsDlg, iAddButton, false);
		setButtonEnable(p->PrefsDlg, iRemoveButton, (**aliasList).dataBounds.bottom >= 1);
		setButtonEnable(p->PrefsDlg, iStrictMatch, false);
	}
}

// Get our Prefs values
static void PrefsWindowGet(pPWGetWindowDataPtr p)
{
	if(p->id == prefPanel)
	{
		prefs.boldToken = getCheckBox(p->PrefsDlg, iBoldText);
		prefs.boldNick = getCheckBox(p->PrefsDlg, iBoldNick);
		prefs.autoHilightCurNick = getCheckBox(p->PrefsDlg, iNickHilight);
		//prefs.playSnd = getCheckBox(p->PrefsDlg, iSoundCheck);
	}
}

// Redraw our list
static void PrefWinRedraw(pPWRedrawDataPtr p)
{
#pragma unused(p)
	ThemeDrawingState ts;
	
	GetThemeDrawingState(&ts);
	NormalizeThemeDrawingState();
	
	SetThemeDrawingState(ts, true);
}

static void ResizePrefs(void)
{
	SetHandleSize((Handle)wordPrefs.words, wordPrefs.numWords * sizeof(WordPref));
}

static int PFindWord(ConstStr255Param st)
{
	int max = wordPrefs.numWords;
	int x;
	
	for(x = 0; x < max; x++)
		if(pstrcasecmp(st, (*wordPrefs.words)[x].word))
			return x;
	
	return -1;
}

static char DoWordMatchOnWord(ConstStr255Param inWord)
{
	int w = PFindWord(inWord);
	
	if(w != -1)
		return (*wordPrefs.words)[w].matchStrict;
	else
		return false;
}

static char AddWord(ConstStr255Param st)
{
	int w = PFindWord(st);
	
	if(w == -1)
	{
		WordPrefPtr p;
		int x = wordPrefs.numWords++;
		
		ResizePrefs();
		
		p = &(*wordPrefs.words)[x];
		
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
		
		HLock((Handle)wordPrefs.words);
		
		//Move everything above down
		a = *wordPrefs.words;
		siz = wordPrefs.numWords - delWord - 1;
		BlockMoveData(&a[delWord+1], &a[delWord], siz);
		
		HUnlock((Handle)wordPrefs.words);
		
		wordPrefs.numWords--;
		ResizePrefs();
	}
}


static void PrefWinHit(pPWItemHitDataPtr p)
{
	if(p->id == prefPanel)
	{
		Str255 st;
		ThemeDrawingState ts;
		
		SetPortWindowPort(GetDialogWindow(p->PrefsDlg));
		
		GetThemeDrawingState(&ts);
		NormalizeThemeDrawingState();
		
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
						(*wordPrefs.words)[w].matchStrict = getCheckBox(p->PrefsDlg, p->itemNum);
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
			
		SetThemeDrawingState(ts, true);
	}
}
	
// Make the list
static ListHandle BuildTheList(DialogPtr dlog)
{
	ListHandle textListHdl;
	ThemeDrawingState ts;
	StringPtr	theString;
	Cell cell;
	WordPrefPtr p;
	int x, max;
	
	GetThemeDrawingState(&ts);
	NormalizeThemeDrawingState();

	textListHdl = GetAppearanceListBoxHandle(dlog, iAliasList);
	(**textListHdl).selFlags = lOnlyOne + lNoNilHilite;

	HLock((Handle)wordPrefs.words);
	p = *wordPrefs.words;
	max = wordPrefs.numWords;
	
	LAddRow(max, (**textListHdl).dataBounds.bottom, textListHdl);
	
	SetPt(&cell,0,0);
	for(x = 0; x < max; x++)
	{
		theString = p[x].word;
		LSetCell(&theString[1], theString[0], cell, textListHdl);
		cell.v++;
	}
	
	SetPt(&cell,0,0);
	LSetSelect(true, cell, textListHdl);
	LSetDrawingMode(true, textListHdl);
	
	SetThemeDrawingState(ts, true);
	
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

static CFBooleanRef MakeCFBoolean(Boolean b)
{
	if(b)
		return kCFBooleanTrue;
	else
		return kCFBooleanFalse;
}

static Boolean ReadDictBoolean(CFDictionaryRef dict, CFStringRef key, Boolean *outValue)
{
	Boolean exists;
	CFBooleanRef value;
	
	value = CFDictionaryGetValue(dict, key);
	if(value)
		*outValue = CFBooleanGetValue(value);
	
	return exists;
}

static Boolean ReadDictStr(CFDictionaryRef dict, CFStringRef key, StringPtr outStr, int outLen)
{
	Boolean exists;
	CFStringRef value;
	
	value = CFDictionaryGetValue(dict, key);
	if(value)
		CFStringGetPascalString(value, outStr, outLen, kCFStringEncodingMacRoman);
	
	return exists;
}

static CFMutableArrayRef CreateWordsArr(void)
{
	CFMutableArrayRef wordsArr = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	CFMutableDictionaryRef word;
	CFStringRef str;
	WordPrefPtr p;
	int x, max;
	
	HLock((Handle)wordPrefs.words);
	p = *wordPrefs.words;
	max = wordPrefs.numWords;
	
	for(x = 0; x < max; x++)
	{
		word = CFDictionaryCreateMutable(NULL, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		
		str = CFStringCreateWithPascalString(NULL, p[x].word, kCFStringEncodingMacRoman);
		CFDictionarySetValue(word, kPrefWord, str);
		CFDictionarySetValue(word, kPrefWordMatchStrict, MakeCFBoolean(p[x].matchStrict));
		
		CFArrayAppendValue(wordsArr, word);
		CFRelease(word);
		CFRelease(str);
	}
	
	HUnlock((Handle)wordPrefs.words);
	
	return wordsArr;
}

static void ReadWordsArr(CFArrayRef wordsArr)
{
	CFDictionaryRef word;
	int numWords = CFArrayGetCount(wordsArr);
	int x;
	WordPrefPtr p;
	
	wordPrefs.words = (WordPrefHand)NewHandleClear(numWords * sizeof(WordPref));
	
	HLock((Handle)wordPrefs.words);
	p = *wordPrefs.words;
	
	for(x = 0; x < numWords; x++)
	{
		word = CFArrayGetValueAtIndex(wordsArr, x);
		
		ReadDictStr(word, kPrefWord, p[x].word, 64);
		ReadDictBoolean(word, kPrefWordMatchStrict, &p[x].matchStrict);
	}
	
	wordPrefs.numWords = numWords;
}

static void WritePrefs(void)
{
	CFMutableDictionaryRef highlightPrefsDict = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFMutableDictionaryRef prefsDict = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFMutableArrayRef wordsArr;
	
	//Set the prefs values
	CFDictionarySetValue(prefsDict, kPrefBoldToken, MakeCFBoolean(prefs.boldToken));
	CFDictionarySetValue(prefsDict, kPrefBoldNick, MakeCFBoolean(prefs.boldNick));
	CFDictionarySetValue(prefsDict, kPrefAutoHilightCurNick, MakeCFBoolean(prefs.autoHilightCurNick));
	
	CFDictionarySetValue(highlightPrefsDict, kHighlightPrefsSubDict, prefsDict);
	
	//Set the words values
	wordsArr = CreateWordsArr();
	CFDictionarySetValue(highlightPrefsDict, kHighlightPrefsWordsArr, wordsArr);
	
	CFPreferencesSetAppValue(kHighlightPrefsDictName, highlightPrefsDict, kCFPreferencesCurrentApplication);
	
	CFRelease(highlightPrefsDict);
	CFRelease(prefsDict);
	CFRelease(wordsArr);
}

//Open the prefs file and read in the prefs
static void ReadPrefs(void)
{
	Boolean ok = false;
	CFDictionaryRef highlightPrefsDict = CFPreferencesCopyAppValue(kHighlightPrefsDictName, kCFPreferencesCurrentApplication);
	
	if(highlightPrefsDict)
	{
		CFDictionaryRef prefsDict = CFDictionaryGetValue(highlightPrefsDict, kHighlightPrefsSubDict);
		CFArrayRef wordsArr = CFDictionaryGetValue(highlightPrefsDict, kHighlightPrefsWordsArr);
		
		if(prefsDict)
		{
			ReadDictBoolean(prefsDict, kPrefBoldToken, &prefs.boldToken);
			ReadDictBoolean(prefsDict, kPrefBoldNick, &prefs.boldNick);
			ReadDictBoolean(prefsDict, kPrefAutoHilightCurNick, &prefs.autoHilightCurNick);
		}
		
		if(wordsArr)
		{
			ReadWordsArr(wordsArr);
			ok = true;
		}
		
		CFRelease(highlightPrefsDict);
	}
	
	if(!ok)
		wordPrefs.words = (WordPrefHand)NewHandleClear(0);
}

// Turn on the messages we need
static void SetupMessages(char captureMessages[numMessages])
{
	captureMessages[pServerPRIVMSGMessage] = 
	captureMessages[pCTCPMessage] = 
	captureMessages[pSavePreferencesMessage] = 1;
}

static void displayOldVersionMsg(void)
{
	LongString ls;
	
	LSStrLS("\pThe highlight plugin requires ShadowIRC 2.0 or later.", &ls);
	if(!IsWindowVisible((*sidr->consoleWin)->w))
		ShowWindow((*sidr->consoleWin)->w);
	SMPrefixIrcleColor(&ls, dsConsole, '2');
}

// Entry point of our code
void pluginMain(ShadowIRCDataRecord* sidrIN)
{
	unsigned long l;
	
	switch (sidrIN->message)
	{
		case pVersionCheckMessage:
			sidr=sidrIN;
			l=((pVersionCheckDataPtr)sidrIN->messageData)->version;
			if(l<0x02000008) //we must have ShadowIRC 2.0d8 or later.
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
