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

#include "IRCGlobals.h"

#include "AEvents.h"
#include "MsgWindows.h"
#include "TextWindows.h"
#include "ApplBase.h"

#include "inline.h"
#include "plugins.h"

typedef struct AETEItem {
	Handle aete;
} AETEItem, *AETEItemPtr;

typedef struct AETEList {
	long num;
	AETEItem list[1];
} AETEList, *AETEListPtr, **AETEListHand;

static AETEListHand aeteList;

#pragma internal reset

pascal long RegisterAETE(Handle aete)
{
	int i;
	AETEItemPtr pp;
	
	if(aete)
	{
		if(!aeteList)
			aeteList = (AETEListHand)NewHandleClear(sizeof(AETEList));
		i=(**aeteList).num++;
		SetHandleSize((Handle)aeteList, sizeof(long)+((**aeteList).num*sizeof(AETEItem)));
		pp = &(**aeteList).list[i];
		
		pp->aete = aete;
		return i;
	}
	else
		return -1;
}

#pragma internal on

static pascal OSErr aeOApp(const AppleEvent *theAEvent, AppleEvent *theReply, unsigned long refCon);
static pascal OSErr aeQuit(const AppleEvent *theAEvent, AppleEvent *theReply, unsigned long refCon);
static pascal OSErr aeODoc(const AppleEvent *theAEvent, AppleEvent *theReply, unsigned long refCon);
static pascal OSErr aePDoc(const AppleEvent *theAEvent, AppleEvent *theReply, unsigned long refCon);
static pascal OSErr aeGetAETE(const AppleEvent *theAEvent, AppleEvent *theReply, unsigned long refCon);

static pascal char GetAETEResource(Handle *res);
static pascal OSErr EasyHasUnusedParameters(const AppleEvent *theAEvent);

static pascal OSErr aeOApp(const AppleEvent *, AppleEvent *, unsigned long)
{
	return noErr;
}

static pascal OSErr aeQuit(const AppleEvent *, AppleEvent *, unsigned long)
{
	doQuit(false, 0);
	return noErr;
}

static pascal OSErr aeODoc(const AppleEvent *theAEvent, AppleEvent *, unsigned long)
{
	OSErr error = noErr, firstError = noErr;
	short numErrors = 0;
	AEDesc theList;
	long numFiles, fileCount;
	
/* the direct object parameter is the list of aliases to files (one or more) */
	error = AEGetParamDesc(theAEvent, keyDirectObject, typeAEList, &theList);
	if (error)
		return(error);

/* that should be all, but in case something is screwy... */
	error = EasyHasUnusedParameters(theAEvent);
	if (error)
		return(error);

/* get number of files in list */
	error = AECountItems(&theList, &numFiles);
	if (error)
		return(error);

/* open each file - keep track of errors	 */
	for(fileCount = 1; fileCount <= numFiles; fileCount++)
	{
		FSSpec fileToOpen;
		long actualSize;
		AEKeyword dummyKeyword;
		DescType dummyType;
		FInfo f;
		
		/* get the alias for the nth file, convert to an FSSpec */
		error = AEGetNthPtr(&theList, fileCount, typeFSS, &dummyKeyword, &dummyType, (Ptr)&fileToOpen, sizeof(FSSpec), &actualSize);
		if(error)
			return(error);

		/* open the file (also creates window) */
		FSpGetFInfo(&fileToOpen, &f);
		if(f.fdType == 'TEXT')
			TWOpen(&fileToOpen);
		else
			error = -1;

		/* count file opening errors */
		if(error)
		{
			++numErrors;	/* keep count */
			if(numErrors == 1)				
				firstError = error;	/* save first error */
		}
	}

/* set proper error message */
	if(numErrors)	/* at least one happend */
	{
		if (numErrors > 1)	/* had more than one,  */
			error = -1;
		else
			error = firstError;
	}
	
	AEDisposeDesc(&theList); /* dispose what we allocated */
	return(error);
}

static pascal OSErr aePDoc(const AppleEvent *, AppleEvent *, unsigned long)
{
	return errAEEventNotHandled;
}

static pascal OSErr aeGetAETE(const AppleEvent *theAEvent, AppleEvent *theReply, unsigned long)
{
	OSErr err;
	long actualSize;
	short languageCode;
	DescType returnedType;
	AEDescList theList;
	AEDesc	aeteDesc;
	int x, num;
	AETEItemPtr list;
	
	//directObject is an integer -- language code
	//reply is a typeAEList or typeAETE.
	if(!theReply->dataHandle) //If no reply requested, then don't handle.
		return errAEEventNotHandled;
	
	err = AEGetParamPtr(theAEvent, keyDirectObject, typeInteger, &returnedType, &languageCode, sizeof(short), &actualSize);
	if(err)
		return err;
	
	//Create a list
	err = AECreateList(0, 0, false, &theList);
	if(err)
		return err;
	
	if(aeteList)
	{
		HLock((Handle)aeteList);
		num = (**aeteList).num;
		list = (**aeteList).list;
		
		for(x = 0; x < num; x++)
		{
			err = AECreateDesc(kAETerminologyExtension, *(list[x].aete), GetHandleSize(list[x].aete), &aeteDesc);
			if(!err)
			{
				AEPutDesc(&theList, 0, &aeteDesc);
				AEDisposeDesc(&aeteDesc);
			}
		}
		HUnlock((Handle)aeteList);
	}
	aeteDesc.descriptorType = kAETerminologyExtension;
	
	//Add list to reply event
	err = AEPutParamDesc(theReply, keyDirectObject, &theList);
	err = AEDisposeDesc(&theList);
	return err;
}

#pragma mark -

static pascal OSErr EasyHasUnusedParameters(const AppleEvent *theAEvent)
{
	OSErr		error;
	long		actualSize;
	DescType	dummyType;
	AEKeyword	missedKeyword;
		
/* Get the "missed keyword" attribute from the AppleEvent. */
	error = AEGetAttributePtr(theAEvent, keyMissedKeywordAttr, typeKeyword, &dummyType, (Ptr)&missedKeyword, sizeof(missedKeyword), &actualSize);

/* If the descriptor isn't found, then we got the required parameters. */
	if(error == errAEDescNotFound)
		return noErr;
	else
		return errAEEventNotHandled;
}

#define InstHand(aeClass, aeType, proc) do {error = AEInstallEventHandler(aeClass, aeType, NewAEEventHandlerProc(proc), 0, false); if(error) return error;}while(0)

pascal OSErr InstallAEHandlers(void)
{
	OSErr error;
	
	InstHand(kCoreEventClass, kAEOpenApplication, aeOApp);
	InstHand(kCoreEventClass, kAEQuitApplication, aeQuit);
	InstHand(kCoreEventClass, kAEOpenDocuments, aeODoc);
	InstHand(kCoreEventClass, kAEPrintDocuments, aePDoc);

	InstHand(kASAppleScriptSuite, kGetAETE, aeGetAETE);
	
	RegisterAETE(Get1Resource(kAETerminologyExtension, 0));
	
	return error;
}