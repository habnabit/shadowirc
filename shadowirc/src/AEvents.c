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

#include "IRCGlobals.h"

#include "AEvents.h"
#include "MsgWindows.h"
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

static OSErr aeOApp(const AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
static OSErr aeQuit(const AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
static OSErr aePDoc(const AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
static OSErr aeGetAETE(const AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
static OSErr aeGURL(const AppleEvent *event, AppleEvent *reply, long refcon);

static OSErr EasyHasUnusedParameters(const AppleEvent *theAEvent);

static OSErr aeOApp(const AppleEvent * ae, AppleEvent *reply, long refCon)
{
	#pragma unused(ae, reply, refCon)
	return noErr;
}

static OSErr aeQuit(const AppleEvent *ae, AppleEvent *reply, long refCon)
{
	#pragma unused(ae, reply, refCon)
	doQuit(0);
	return noErr;
}

static OSErr aePDoc(const AppleEvent *ae, AppleEvent *reply, long refCon)
{
	#pragma unused(ae, reply, refCon)
	return errAEEventNotHandled;
}

static OSErr aeGetAETE(const AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
	#pragma unused(refCon)
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

enum {
	kGetURLEventClass = 'GURL',
	kGetURLEventID	 = 'GURL',
	kGetURLToFileKeyword = 'dest'
};

#include "inputline.h"

static OSErr OpenURLString(LongString *url)
{
	SetInputLine(url);
	
	return paramErr;
}

static OSErr aeGURL(const AppleEvent *event, AppleEvent *reply, long refCon)
{
	#pragma unused(refCon)
	LongString url;
	DescType typeCode;
	Size actualSize;
	OSErr err = noErr;
	OSErr result = noErr;

	//if (!gStartupOK) return userCanceledErr;
	
	err = AEGetParamPtr(event, keyDirectObject, typeChar, &typeCode, &url.data[1], maxLSlen, &actualSize);
	if(err != noErr)
		return err;
	url.len = actualSize;
	
	err = EasyHasUnusedParameters(event);
	if(err != noErr)
		return err;
	
	err = result = OpenURLString(&url);
	if(reply->dataHandle != nil)
	{
		err = AEPutParamPtr(reply, keyDirectObject, typeShortInteger, &result, sizeof(result));
		if(err != noErr)
			return err;
	}
	
	return noErr;
}

#pragma mark -

static OSErr EasyHasUnusedParameters(const AppleEvent *theAEvent)
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

#define InstHand(aeClass, aeType, proc) do {error = AEInstallEventHandler(aeClass, aeType, NewAEEventHandlerUPP(proc), 0, false); if(error) return error;}while(0)

pascal OSErr InstallAEHandlers(void)
{
	OSErr error;
	
	InstHand(kCoreEventClass, kAEOpenApplication, aeOApp);
	InstHand(kCoreEventClass, kAEQuitApplication, aeQuit);
	InstHand(kCoreEventClass, kAEPrintDocuments, aePDoc);
	
	InstHand(kGetURLEventClass, kGetURLEventID, aeGURL);

	InstHand(kASAppleScriptSuite, kGetAETE, aeGetAETE);
	
	RegisterAETE(Get1Resource(kAETerminologyExtension, 0));
	
	return error;
}