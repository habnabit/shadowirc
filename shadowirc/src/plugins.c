/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2001 John Bafford
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

#include "StringList.h"
#include "IRCGlobals.h"
#include "utils.h"
#include "MsgBuffer.h"
#include "IRCIgnore.h"
#include "Inputline.h"
#include "MsgWindows.h"
#include "IRCChannels.h"
#include "channels.h"
#include "WindowList.h"
#include "DCC.h"
#include "connections.h"
#include "TextManip.h"
#include "filesMan.h"

#include "plugins.h"

#include "IRCAux.h"
#include "Floaters.h"
#include "IRCNComm.h"
#include "IRCInput.h"
#include "InetConfig.h"
#include "DragDrop.h"
#include "MenuCommands.h"
#include "ApplBase.h"

wMenuServicesHand wMenuServices = 0;

enum serviceStrings {
	sUserlist = 1,
	sFaces,
	sConnections,
	sDCCStatus
};

inline void callIndPlugin(plugsPtr ref, void* msgD, short msg);
inline char parsePlugFile(const FSSpec * spec);
static pascal void ProcessDirectory(FSSpec theSpec);
inline void initSIDR(void);
static pascal void AddService(FourCharCode serviceType, plugsPtr ref);
static pascal void InitPlugins(void);

pascal OSErr FSpGetDirectoryID(FSSpec*, long*, char*);
static pascal void CheckPluginPrefsFolder(void);

pascal long _UndocumentedAPI(long type, long data);

static pascal plugsPtr IPCFind(FourCharCode IPCType, long *version, long *data);

//For Inlined crap
#define linkfor(list, init) for((list) = (init); (list); (list)=(list)->next)
pascal char IsChannel(ConstStr255Param s);
pascal char IsDCCName(ConstStr255Param s);
pascal void pstrcpy(ConstStr255Param f, ConstStr255Param dest);
pascal void LineMsg(const LongString *ls);
pascal void Message(const LongString *msg);

pascal long ShadowIRCVersion(StringPtr);
pascal long ShadowIRCVersion2(StringPtr, StringPtr);

FSSpec pluginsFolderFSSpec, pluginPrefsFSSpec;

hmiListHand hmiList;

static plugsPtr firstPlugin, lastPlugin;

ShadowIRCDataRecord sidr;

static slPtr ServiceList=0;
static IPCPtr IPCList = 0;

pascal short WMSGetMenuItemNum(FourCharCode serviceType)
{
	int x, m;
	
	m = (**wMenuServices).numMenus;
	for(x=0;x<m;x++)
		if((**wMenuServices).service[x].key == serviceType)
			return x + wFirstServiceItem;
	
	return -1;
}

pascal short WMSListAdd(FourCharCode serviceType, ConstStr255Param name)
{
	int x;

	if(!wMenuServices)
	{
		wMenuServices = (wMenuServicesHand)NewHandleClear(sizeof(long)+sizeof(long));
		(**wMenuServices).numMenus = 1;
		x=0;
	}
	else
	{
		x = (**wMenuServices).numMenus++;
		SetHandleSize((Handle)wMenuServices, sizeof(long)+(sizeof(wMSData)*(x+1)));
	}
	
	(**wMenuServices).service[x].pluginRef = sidr.yourInfo;
	(**wMenuServices).service[x].key = serviceType;
	InsertMenuItem(gWindowMenu, name, wLastServiceItem++);
	return wLastServiceItem;
}

static pascal void AddService(FourCharCode serviceType, plugsPtr ref)
{
	slPtr p=(slPtr)NewPtr(sizeof(serviceListRec));
	
	p->next=ServiceList;
	p->serviceType=serviceType;
	p->pluginRef=ref;
	
	if(serviceType == facesServiceType)
		p->serviceClass=userlistServiceType;
	else
		p->serviceClass=serviceType;

	ServiceList=p;
}

pascal long FindService(FourCharCode serviceType)
{
	slPtr p;
	
	linkfor(p, ServiceList)
		if(p->serviceType==serviceType)
			return (long)p->pluginRef;

	return 0;
}

pascal char ExistsService(FourCharCode serviceType)
{
	return FindService(serviceType) != 0;
}

pascal char NewService(FourCharCode serviceType)
{
	short i;
	
	switch(serviceType)
	{
		case userlistServiceType:
			if(!FindService(userlistServiceType))
			{
				AddService(userlistServiceType, sidr.yourInfo);
				i = WMSListAdd(userlistServiceType, GetIntStringPtr(spServices, sUserlist));
				SetItemCmd(gWindowMenu, i, 'U');
				return 1;
			}
			break;
		
		case facesServiceType:
			if(!FindService(facesServiceType))
			{
				AddService(facesServiceType, sidr.yourInfo);
				WMSListAdd(facesServiceType, GetIntStringPtr(spServices, sFaces));
				return 1;
			}
			break;
		
		case connectionWindowServiceType:
			if(!FindService(connectionWindowServiceType))
			{
				AddService(connectionWindowServiceClass, sidr.yourInfo);
				WMSListAdd(connectionWindowServiceClass, GetIntStringPtr(spServices, sConnections));
				return 1;
			}
			break;
		
		case dccWindowServiceType:
			if(!FindService(dccWindowServiceType))
			{
				AddService(dccWindowServiceClass, sidr.yourInfo);
				i = WMSListAdd(dccWindowServiceClass, GetIntStringPtr(spServices, sDCCStatus));
				SetItemCmd(gWindowMenu, i, 'D');
				return 1;
			}
			break;
			
		case soundServiceType:
			if(!FindService(soundServiceType))
			{
				AddService(soundServiceClass, sidr.yourInfo);
				return 1;
			}
			break;
			
		case userRegistrationType:
			AddService(userRegistrationClass, sidr.yourInfo);
			return 1;
			
		case genericServiceType:
			AddService(genericServiceType, sidr.yourInfo);
			return 1;
		
		default:
			if(!FindService(serviceType))
			{
				AddService(serviceType, sidr.yourInfo);
				return 1;
			}
	}
	
	return 0;
}

#pragma mark -

pascal char IPCExists(FourCharCode IPCType, long *version, long *data)
{
	IPCPtr p;
	
	linkfor(p, IPCList)
		if(p->type==IPCType)
		{
			if(version)
				*version=p->version;
			if(data)
				*data=p->data;
			return 1;
		}

	return 0;
}

static pascal plugsPtr IPCFind(FourCharCode IPCType, long *version, long *data)
{
	IPCPtr p;
	
	linkfor(p, IPCList)
		if(p->type==IPCType)
		{
			if(version)
				*version=p->version;
			if(data)
				*data=p->data;
			return p->pluginRef;
		}

	return 0;
}

pascal short IPCSend(FourCharCode IPCType, long message, long data, long *result)
{
	plugsPtr p=IPCFind(IPCType, 0, 0);
	
	if(p)
	{
		pIPCMessageData ipcd;
		ipcd.type=IPCType;
		ipcd.message=message;
		ipcd.data=data;
		ipcd.result=0;
		ipcd.client=(long)sidr.yourInfo;
		ipcd.ack = 0;
		runIndPlugin(p, pIPCMessage, &ipcd);
		*result=ipcd.result;
		if(ipcd.ack)
			return 1;
		else
			return -1;
	}

	return 0;
}

pascal char IPCReply(plugsPtr client, FourCharCode IPCType, long message, long data, long *result)
{
	pIPCReplyMessageData p;
	
	if(client)
	{
		p.type=IPCType;
		p.message=message;
		p.data=data;
		p.result=0;
		p.ack = 0;
		
		runIndPlugin(client, pIPCReplyMessage, &p);
		
		if(!p.ack)
			*result=p.result;
		else
			*result = 0;
		return p.ack;
	}
	else
	{
		*result = 0;
		return 0;
	}
}

pascal short IPCRegister(FourCharCode IPCType, long version, long data)
{
	long v;
	IPCPtr p;
	plugsPtr pnum=IPCFind(IPCType, &v, 0);
	
	if(pnum)
	{
		if(version>v)
			return 3;
		else if(version<v)
			return 2;
		else
			return 1;
	}
	else
	{
		p=(IPCPtr)NewPtr(sizeof(IPCListRec));
		p->next=IPCList;
		p->type=IPCType;
		p->pluginRef=sidr.yourInfo;
		p->version=version;
		p->data=data;
		IPCList=p;
		return 0;
	}
}

#pragma mark -

pascal short HMIAdd(ConstStr63Param name)
{
	int i;
	
	if(name[0])
	{
		i=(**hmiList).num++;
		SetHandleSize((Handle)hmiList, sizeof(long)+((**hmiList).num*sizeof(hmItem)));
		(**hmiList).list[i].pluginRef=sidr.yourInfo;
		pstrcpy(name, (**hmiList).list[i].name);
		
		i=CountMenuItems(helpMenu);
		if(i<normHelpMenuItems)
		{
			AppendMenu(helpMenu, "\p-");
			AppendMenu(menuHelpWidget, "\p-");
		}
		
		AppendMenu(helpMenu, name);
		AppendMenu(menuHelpWidget, name);
		return i;
	}
	else
		return -1;
}

#pragma mark -

#define SavePluginState() \
	plugsPtr prevInfo = sidr.yourInfo; \
	void* prevMsgD = sidr.messageData; \
	short prevMsg = sidr.message;

#define RestorePluginState() \
	sidr.yourInfo=prevInfo; \
	sidr.messageData=prevMsgD; \
	sidr.message=prevMsg;

#define SetSIDR() \
	sidr.messageData=msgD; \
	sidr.message=msg; \
	sidr.yourInfo=ref; \
	ref->timesCalled++;

#define MakePluginCall() \
	UseResFile(ref->resFileRefNum); \
	ref->proc(&sidr);

inline void callIndPlugin(plugsPtr ref, void* msgD, short msg)
{
	short rf;
	
	SetSIDR();
	
	rf=CurResFile();

	MakePluginCall();

//Restore sidr state.
	UseResFile(rf);
}

inline void callIndPluginRF(plugsPtr ref, void* msgD, short msg)
{
	SetSIDR();
	MakePluginCall();
}

pascal void runIndPlugin(plugsPtr ref, short message, void* messageData)
{
	SavePluginState();

	callIndPlugin(ref, messageData, message);

	RestorePluginState();
}

pascal void runService(short message, void* messageData) //messages to any service
{
	slPtr p;
	
	SavePluginState();
	
	linkfor(p, ServiceList)
		callIndPlugin(p->pluginRef, messageData, message);

	RestorePluginState();
}

pascal void runIndService(long serviceType, short message, void* messageData) //messages to a specific service
{
	slPtr p;
	
	SavePluginState();
	
	linkfor(p, ServiceList)
		if(p->serviceClass==serviceType)
			callIndPlugin(p->pluginRef, messageData, message);

	RestorePluginState();
}

pascal void SoundService(long sound, long data)
{
	long soundService = FindService(soundServiceType);
	
	if(soundService)
	{
		pSoundServiceData p;
		p.sound = sound;
		p.data = data;
		runIndPlugin((plugsPtr)soundService, pSoundServiceMessage, &p);
	}
	else //internal sound
		;
}

pascal void runPlugins(short message, void* messageData)
{
	plugsPtr p;
	char k = sidr.completelyProcessed;
	short rf = CurResFile();
	
	SavePluginState();
	
	sidr.completelyProcessed=0;
	linkfor(p, firstPlugin)
		if(p->captureMessages[message])
		{
			callIndPluginRF(p, messageData, message);
			if(sidr.completelyProcessed)
				break;
		}
	
	sidr.completelyProcessed=k;
	RestorePluginState();
	UseResFile(rf);
}

pascal void runAllPlugins(short message, void* messageData)
{
	plugsPtr p;
	short rf = CurResFile();
	
	SavePluginState();

	linkfor(p, firstPlugin)
		if(p->captureMessages[message])
			callIndPluginRF(p, messageData, message);

	RestorePluginState();
	UseResFile(rf);
}

pascal void idlePlugins(EventRecord *e)
{
	plugsPtr plug;
	long curTime;
	pIdleMessageData p;

	SavePluginState();
	
	curTime=TickCount();
	p.e = e;
	linkfor(plug, firstPlugin)
		if(plug->captureMessages[pIdleMessage])
			if((curTime-plug->lastIdleCall)>plug->idleThreshold)
			{
				callIndPlugin(plug, &p, pIdleMessage);
				plug->lastIdleCall=curTime;
			}

	RestorePluginState();
}

#define procUndocumentedAPI 0
enum {
	pUndocumentedAPIReply = 'BAD!'
};

pascal long _UndocumentedAPI(long type, long data)
{
	#pragma unused(data)
	LongString ls;
	int x;
	
	switch(type)
	{
		case 'tlkc':
			gTalkCityPlug = 1;
			return 0;
		
		case 'mail':
			if(data < 5)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pThe version of mailcheck installed is too old. Please install the most recent version.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return pVersionPluginTooOld;
			}
			else if(data < 4)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pYou are not using the most recent version of the mailcheck plugin.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return 0;
			}
			else
				return 0;
			break;

		case 'numu':
			if(data < 2)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pThe version of numusers installed is too old. Please install the most recent version.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return pVersionPluginTooOld;
			}
			else
				return 0;
			break;
		
		case 'othl':
			return 0;
		
		case 'homr':
			return 0;
		
		case 'hlgt':
			return 0;
		
		case 'ulst':
			if(data < 11)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pThe Userlist plugin is too old. Please install the most recent version.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return pVersionPluginTooOld;
			}
			else if(data < 9)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pYou are not using the most recent version of the userlist plugin.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return 0;
			}
			else
				return 0;
			break;
		
		case 'srvl':
			if(!IsVisible(consoleWin->w))
				WShow(consoleWin->w);
			LSStrLS("\pThe ServerList plugin is no longer necessary. Please delete it.", &ls);
			SMPrefixIrcleColor(&ls, dsConsole, '2');
			return pVersionPluginTooOld;
			break;
		
		case 'popu':
			if(data < 7)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pThe popups plugin is too old. Please install the most recent version.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return pVersionPluginTooOld;
			}
			else if(data < 4)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
				LSStrLS("\pYou are not using the most recent version of the Popups plugin.", &ls);
				SMPrefixIrcleColor(&ls, dsConsole, '2');
				return 0;
			}
			else
				return 0;
			break;
		
		default:
			if(!IsVisible(consoleWin->w))
				WShow(consoleWin->w);
			LSConcatStrAndStrAndStr("\pPlugin \"", sidr.yourInfo->pluginName, "\p\" can not run.", &ls);
			SMPrefixIrcleColor(&ls, dsConsole, '2');
			for(x=0;x<numMessages;x++)
				sidr.yourInfo->captureMessages[x] = 0;
	}

	return pUndocumentedAPIReply;
}

static pascal void InitPlugins(void)
{
	plugsPtr p;
	int y;
	long version, v;
	LongString ls;
	char b=0;
	
	_UndocumentedAPI;
	v = ShadowIRCVersion(0);
	
	linkfor(p, firstPlugin)
	{
		version=v;
		runIndPlugin(p, pVersionCheckMessage, &version);
		if(version == pVersionCheckMessageReply)
			runIndPlugin(p, pInitMessage, 0);
		else if(version == pUndocumentedAPIReply)
		{
			for(y=0;y<numMessages;y++)
				p->captureMessages[y] = 0;
		}
		else if(version == pVersionShadowIRCTooOld || version == pVersionPluginTooOld)
		{
			for(y=0;y<numMessages;y++)
				p->captureMessages[y] = 0;

			if(!b)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
			}
			b=1;
		}
		else
		{
			for(y=0;y<numMessages;y++)
				p->captureMessages[y] = 0;

			if(!b)
			{
				if(!IsVisible(consoleWin->w))
					WShow(consoleWin->w);
			}
			b=1;
			
			if(version <= pVersion11MessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" is too old, and is incompatible with this version of ShadowIRC.", &ls);
			else if(version > pVersionCheckMessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" can not run because it is too new for this version of ShadowIRC.", &ls);
			LineMsg(&ls);
		}
	}
}

inline char parsePlugFile(const FSSpec *spec)
{
	LongString ls;
	int x;
	plugsPtr thisPlug;
	
	CFragConnectionID cID; //save this in case 
	Ptr cfgUPP;
	Str255 errMsg;

	if(!GetDiskFragment(spec, 0, 0, "\p", kLoadCFrag, &cID, &cfgUPP, errMsg))
	{
		thisPlug = (plugsPtr)NewPtr(sizeof(plugsRec));
		thisPlug->proc = (pluginMain)cfgUPP;
		thisPlug->captureMessages[pInitMessage]=1;
		thisPlug->captureMessages[pQuitMessage]=1;
		x=2;
		do {
			thisPlug->captureMessages[x++]=0;
		} while(x<numMessages);
		
		pstrcpy(spec->name, thisPlug->pluginName);
		thisPlug->idleThreshold=0;
		thisPlug->lastIdleCall=0;
		thisPlug->xpluginRef=(long)&thisPlug;
		thisPlug->timesCalled=0;
		thisPlug->resFileRefNum=FSpOpenResFile(spec, fsRdPerm);
		
		if(!firstPlugin)
		{
			firstPlugin = lastPlugin = thisPlug;
			thisPlug->next = 0;
		}
		else
		{
			lastPlugin->next = thisPlug;
			lastPlugin = thisPlug;
			thisPlug->next = 0;
		}
	}
	else
	{
		LSConcatStrAndStrAndStr("\pError loading PowerPC plugin \"", spec->name, "\p\": ", &ls);
		LSConcatLSAndStr(&ls, errMsg, &ls);
		LineMsg(&ls);
	}
	
	return true;
}

static pascal void ProcessDirectory(FSSpec mySpec)
{
	int i;
	long dir;
	CInfoPBRec paramBlock;
	Str255 name;
	FInfo fndrInfo;
	short numFiles;
	LongString ls;
	
	paramBlock.hFileInfo.ioCompletion=0;
	paramBlock.hFileInfo.ioNamePtr=mySpec.name;
	paramBlock.hFileInfo.ioVRefNum=mySpec.vRefNum;
	paramBlock.hFileInfo.ioDirID=mySpec.parID;
	paramBlock.hFileInfo.ioFDirIndex=-1;
	
	i=PBGetCatInfoSync(&paramBlock);
	if(i)
	{
		LSGetIntString(&ls, spError, sNoPluginsFolder);
		LineMsg(&ls);
		return;
	}
	
	dir=paramBlock.dirInfo.ioDrDirID;
	if(paramBlock.dirInfo.ioDrNmFls==0)
	{
		LSGetIntString(&ls, spError, sPlugsFolderEmpty);
		LineMsg(&ls);
	}
	else
	{
		numFiles=paramBlock.dirInfo.ioDrNmFls;
		for(i=1;i<=numFiles;i++)
		{
			paramBlock.hFileInfo.ioNamePtr=name;
			paramBlock.hFileInfo.ioVRefNum=mySpec.vRefNum;
			paramBlock.hFileInfo.ioDirID=dir;
			paramBlock.hFileInfo.ioFDirIndex=i;
			if(PBGetCatInfoSync(&paramBlock))
				return;
			if(!(paramBlock.hFileInfo.ioFlAttrib & 16))
			{
				if(!FSMakeFSSpec(mySpec.vRefNum, mySpec.parID, name, &mySpec))
				{
					FSpGetFInfo(&mySpec, &fndrInfo);
					if(fndrInfo.fdCreator=='SIRC')
					{
						if(fndrInfo.fdType=='shlb' || fndrInfo.fdType=='PPLG')
						{
							if(!parsePlugFile(&mySpec)) //failed, due to a memory allocation error
							{
								LSGetIntString(&ls, spError, sPlugsMemErr);
								LineMsg(&ls);
								break;
							}
						}
					}
				}
			}
		}
		UseResFile(gApplResFork);
	}
}

inline void initSIDR(void)
{
	sidr.yourInfo=0; //If yourInfo == 0, there's nothing running
	sidr.mainPrefs=mainPrefs;
	sidr.linkPrefs=linkPrefsArray;
	sidr.shadowircColors=shadowircColors;
	sidr.ignores=&ignores;
	sidr.messageBuffers=messagebuffers;
	sidr.launchTime=&launchTime;
	sidr.idleTime=&idleTime;
	sidr.debugOn=&debugOn;
	sidr.inBackground=&inBackground;
	sidr.lastInput=&lastInput;
	sidr.lastKey=&lastKey;
	sidr.connections=&fConn;
	sidr.allowConnections=&allowConnections;
	sidr.curUserNum=&userNum;

	sidr.CurrentTarget=&CurrentTarget;

	sidr.firstLink = &firstLink;

	sidr.MWActive=&MWActive;
	sidr.inputLine=&inputLine;
	sidr.consoleWin = &consoleWin;
	sidr.mwList = &mwl;
	
	sidr.internetConfig = (Ptr)internetConfig;
	sidr.ContextWindow = &ContextWindow;
}

static pascal void CheckPluginPrefsFolder(void)
{
	char isDir;
	LongString ls;
	long dirid;
	OSErr err;
	short vref;
	FSSpec ShadowIRCFolder;
	long ShadowIRCFolderDirID, PreferencesFolderDirID;
	
	//Get preferences folder
	err = FindFolder(kOnSystemDisk, 'pref', false, &vref, &PreferencesFolderDirID);
	//Get ShadowIRC Ä
	err = FSMakeFSSpec(vref, PreferencesFolderDirID, GetIntStringPtr(spFiles, sPreferencesFolder), &ShadowIRCFolder);
	
	//Assume ShadowIRC Ä Exists. Get its id.
	err = FSpGetDirectoryID(&ShadowIRCFolder, &ShadowIRCFolderDirID, &isDir);
	
	//Make the FSp for the Plugins dir
	err = FSMakeFSSpec(vref, ShadowIRCFolderDirID, GetIntStringPtr(spFiles, sPluginsFolder), &pluginPrefsFSSpec);

	if(err) //this means that there's no preferences folder
	{
		if(FSpDirCreate(&pluginPrefsFSSpec, 0, &dirid))
		{
			LSGetIntString(&ls, spError, spCantCreatPluginPrefsFldr);
			pluginPrefsFSSpec.parID=pluginPrefsFSSpec.vRefNum=*(short*)pluginPrefsFSSpec.name[0]=0xFDDF;
			LineMsg(&ls);
		}
		else
		{
			pluginPrefsFSSpec.parID=dirid;
			pluginPrefsFSSpec.name[0]=0;
		}
	}
	else //it exists
	{
		FSpGetDirectoryID(&pluginPrefsFSSpec, &pluginPrefsFSSpec.parID, &isDir);
		if(!isDir)
		{
			LSGetIntString(&ls, spError, sPlugsPrefsFolderIsFile);
			pluginPrefsFSSpec.parID=pluginPrefsFSSpec.vRefNum=*(short*)pluginPrefsFSSpec.name[0]=0xFDDF;
			LineMsg(&ls);
		}
		//else it's ok, do nothing.
	}
}

pascal void makePlugsDB(void)
{
	FSSpec tempFSSpec;
	char isDir;
	LongString ls;
	long dirid;
	
	initSIDR();
	
	hmiList=(hmiListHand)NewHandleClear(sizeof(long));
	
	FindAppSpec(&pluginsFolderFSSpec); //Get the app spec
	if(FSMakeFSSpec(pluginsFolderFSSpec.vRefNum, pluginsFolderFSSpec.parID, GetIntStringPtr(spFiles, sPluginsFolder), &pluginsFolderFSSpec))
	{
		//no plugins folder, so make it
		if(FSpDirCreate(&pluginsFolderFSSpec, 0, &dirid))
		{
			LSGetIntString(&ls, spError, sNoPluginsFolder);
			pluginsFolderFSSpec.parID=pluginsFolderFSSpec.vRefNum=*(short*)pluginsFolderFSSpec.name[0]=0xFDDF;
			LineMsg(&ls);
			isDir=0;
		}
		else
		{
			pluginsFolderFSSpec.parID=dirid;
			pluginsFolderFSSpec.name[0]=0;
			isDir=1;
		}
	}
	else
	{
		FSpGetDirectoryID(&pluginsFolderFSSpec, &pluginsFolderFSSpec.parID, &isDir);
		if(!isDir)
		{
			LSGetIntString(&ls, spError, sPlugsFldrIsFile);
			pluginsFolderFSSpec.parID=pluginsFolderFSSpec.vRefNum=*(short*)pluginsFolderFSSpec.name[0]=0xFDDF;
			LineMsg(&ls);
		}
	}
	
	if(isDir)
	{
		tempFSSpec=pluginsFolderFSSpec;
		CheckPluginPrefsFolder();
		ProcessDirectory(tempFSSpec);
	}

	InitPlugins();
}

pascal void pluginCloseWindow(WindowPtr win, pluginDlgInfoPtr p)
{
	pUIWindowCloseDataRec pp;
	pp.w=win;
	pp.mw=0;
	runIndPlugin(p->pluginRef, pUIWindowCloseMessage, &pp);
}

pascal void pluginHitDialog(DialogPtr win, pluginDlgInfoPtr p, short i)
{
	pUIDialogItemHitDataRec pl;
	
	pl.dlg=win;
	pl.item=i;
	runIndPlugin(p->pluginRef, pUIDialogItemHitMessage, &pl);
}

pascal DialogPtr pluginNewDialog(short dialogID)
{
	DialogPtr d;
	pluginDlgInfoPtr p;
	
	d=GetNewDialog(dialogID, 0, (WindowPtr)-1);
	if(d)
	{
		p=(pluginDlgInfoPtr)NewPtr(sizeof(pluginDlgInfo));
		p->magic=PLUGIN_MAGIC;
		p->pluginRef=sidr.yourInfo;
		SetWRefCon(GetDialogWindow(d), (long)p);
	}
	return d;
}

pascal void pluginDisposeDialog(DialogPtr d)
{
	pluginDlgInfoPtr p;
	
	if(d)
	{
		p=(pluginDlgInfoPtr)GetWRefCon(GetDialogWindow(d));
		DisposePtr((Ptr)p);
		DisposeDialog(d);
	}
}

enum {
	pnwHasCloseBox = 1L << 0,		//Does the window have a close box?
	pnwHasGrowBox = 1L << 1,
	pnwHasZoomBox = 1L << 2,
	pnwDocumentWindow = 1L << 10,	//Display using a standard document window
	pnwFloaterWindow = 1L << 11,		//Display using a utility window
	pnwSideFloaterWindow = 1L << 12,	//Display using a utility window with the titlebar on the side
	pnwFIsFloater = 1L << 31			//Will the window float?
};

pascal WindowPtr pluginNewWindow(const Rect *boundsRect, ConstStr255Param title, short theProc, long flags)
{
	pluginDlgInfoPtr p;
	char hasGrowBox = !!(flags & pnwHasGrowBox);
	char goAwayFlag = !!(flags & pnwHasCloseBox);
	char hasZoomBox = !!(flags & pnwHasZoomBox);
	char sideFloaterWindow = !!(flags & pnwSideFloaterWindow);
	char floats = !!(flags & pnwFIsFloater);
	short whichWDEF;
	WindowPtr w;
	
	if(theProc==-1)
	{
		if(floats)
			whichWDEF = (hasGrowBox*2) + (hasZoomBox*4) + (sideFloaterWindow*8) + kWindowFloatProc;
		else if(flags & pnwDocumentWindow)
			whichWDEF = kWindowDocumentProc + (hasGrowBox) + (hasZoomBox * 6); //zoom is vert*2 + horiz*4
	}
	else
		whichWDEF = theProc;
	
	p=(pluginDlgInfoPtr)NewPtr(sizeof(pluginDlgInfo));
	p->magic=PLUGIN_MAGIC;
	p->pluginRef = sidr.yourInfo;
	p->refcon = 0;
	
	w = WCreate(boundsRect, title, whichWDEF, goAwayFlag, (long)p, floats);
	if(!w) //error creating window, kill the data ptr
		DisposePtr((Ptr)p);
	return w;
}

pascal plugsPtr GetPluginWindowOwner(WindowPtr w)
{
	if(w)
	{
		pluginDlgInfoPtr p = (pluginDlgInfoPtr)GetWRefCon(w);
		if(p)
			return p->pluginRef;
	}
	
	return 0;
}

pascal void pluginSetWRefCon(WindowPtr w, long l)
{
	if(w)
	{
		pluginDlgInfoPtr p = (pluginDlgInfoPtr)GetWRefCon(w);
		if(p)
			p->refcon = l;
	}
}

pascal long pluginGetWRefCon(WindowPtr w)
{
	if(w)
	{
		pluginDlgInfoPtr p = (pluginDlgInfoPtr)GetWRefCon(w);
		
		if(p)
			return p->refcon;
		else
			return 0;
	}
	else
		return 0;
}

pascal void pluginDisposeWindow(WindowPtr d)
{
	if(d)
	{
		DisposePtr((Ptr)GetWRefCon(d));
		DisposeWindow(d);
	}
}