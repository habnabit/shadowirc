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

#if TARGET_CPU_68K
#include "plugins68k.h"
#define _68kPlugs_ 1
#endif

#include <Navigation.h>

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

#if !__POWERPC__
#include "AppearanceHelp.h"
#include "IRCCommands.h"
#include "CTCP.h"
#include "IRCPreferences.h"
#include "IRCNotify.h"
#include "CMenus.h"
#include "OffscreenDrawing.h"
#include "MWPanes.h"
#include "Shortcuts.h"
#include "AEvents.h"
#endif

#include "plugins.h"

#include "IRCAux.h"
#include "Floaters.h"
#include "Libs.h"
#include "IRCNComm.h"
#include "IRCInput.h"
#include "InetConfig.h"
#include "DragDrop.h"
#include "ApplBase.h"

#if !__POWERPC__
#pragma dont_inline on
#endif

#pragma internal on

wMenuServicesHand wMenuServices = 0;

enum serviceStrings {
	sUserlist = 1,
	sFaces,
	sConnections,
	sDCCStatus
};

inline void callIndPlugin(plugsPtr ref, void* msgD, short msg);
inline char parsePlugFile(const FSSpec * spec, int isa);
static pascal void ProcessDirectory(FSSpec theSpec);
inline void initSIDR(void);
static pascal void AddService(FourCharCode serviceType, plugsPtr ref);
static pascal void InitPlugins(void);

pascal OSErr FSpGetDirectoryID(FSSpec*, long*, char*);
static pascal void CheckPluginPrefsFolder(void);
#pragma internal reset

#pragma lib_export on
#pragma export on 
pascal long _UndocumentedAPI(long type, long data);
#pragma export reset

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

#pragma internal on

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
			
			if(version == pVersion11OldMessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" is a ShadowIRC 1.1 development plugin and is incompatible with this version of ShadowIRC.", &ls);
			else if(version == pVersion10MessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" is a ShadowIRC 1.0 plugin and is incompatible with this version of ShadowIRC.", &ls);
			else if(version == pVersion6MessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" is a ShadowIRC 0.6.x or ShadowIRC 0.7.0 plugin and is incompatible with this version of ShadowIRC.", &ls);
			else if(version < pVersion6MessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" is a ShadowIRC 0.5.x plugin and is incompatible with this version of ShadowIRC", &ls);
			else if(version > pVersionCheckMessageReply)
				LSConcatStrAndStrAndStr("\p\"", p->pluginName, "\p\" can not run because it is too new for this version of ShadowIRC.", &ls);
			LineMsg(&ls);
		}
	}
}

inline char parsePlugFile(const FSSpec *spec, int isa)
{
	LongString ls;
	int x;
	plugsPtr thisPlug;
	
	if(isa == kPowerPCISA)
	{
		if(GetCurrentISA() != kPowerPCISA)
		{
			LSConcatStrAndStrAndStr("\p\"", spec->name, "\p\" is a PowerPC plugin, and can not be run with the 68k version of ShadowIRC.", &ls);
			LineMsg(&ls);
		}
		else
		{
		#if __POWERPC__
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
		#endif //__POWERPC__ : don't need to call if ShadowIRC-68k.
		}
	}
	else //if(isa == kM68kISA)
	{
		if(GetCurrentISA() != kM68kISA)
		{
			LSConcatStrAndStrAndStr("\p\"", spec->name, "\p\" is a 68k plugin, and can not be run with the PowerPC version of ShadowIRC.", &ls);
			LineMsg(&ls);
		}
		else
		{
			#if !__POWERPC__
			short rn;
			Handle h;

			rn=FSpOpenResFile(spec, fsRdPerm); //Open plugin resource fork read only
			if(!ResError())
			{
				UseResFile(rn);
				h=Get1Resource('PLUG', 10001);
				if(h)
				{
					HLockHi(h);
					HNoPurge(h);
					thisPlug = (plugsPtr)NewPtr(sizeof(plugsRec));

					thisPlug->proc=(pluginMain)*h;
					
					thisPlug->captureMessages[pInitMessage]=1;
					thisPlug->captureMessages[pQuitMessage]=1;
					x=2;
					do {
						thisPlug->captureMessages[x++]=0;
					} while(x<numMessages);
					
					pstrcpy(spec->name, thisPlug->pluginName);
					thisPlug->idleThreshold=0;
					thisPlug->lastIdleCall=0;
					thisPlug->xpluginRef=(long)thisPlug;
					thisPlug->timesCalled=0;

					thisPlug->resFileRefNum=rn;

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
					LSConcatStrAndStrAndStr("\pNo 68k Plugin in \"", spec->name, "\p\"", &ls);
					LineMsg(&ls);
				}
			}
			else
			{
				LSConcatStrAndStrAndStr("\pUnable to open \"", spec->name, "\p\".", &ls);
				LineMsg(&ls);
			}
		#endif
		}
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
	char ok;
	
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
						int x;
						
						if(fndrInfo.fdType=='shlb')
							x = kPowerPCISA;
						else if(fndrInfo.fdType=='PLUG')
							x = kM68kISA;
						else if(fndrInfo.fdType=='PPLG')
							x = GetCurrentISA();
						else x = -1;
					
						if(x>-1)
						{
							ok = parsePlugFile(&mySpec, x);
							
							if(!ok) //failed, due to a memory allocation error
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
	sidr.canAE=1; //assume have AppleEvents
	sidr.hasCM=hasCM;
	sidr.has75=has75;
	sidr._hasAppearance = hasAppearance;
	sidr.shasNav = hasNav;
	sidr.shasAppearance11 = hasAppearance11;
	sidr.hasDrag = hasDrag;
	sidr.shasWM11 = hasWM11;
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
	
	#if !__POWERPC__
		sidr.procs[procUndocumentedAPI]=(ProcPtr)&_UndocumentedAPI;
		sidr.procs[procLSDelete]=(ProcPtr)&LSDelete;
		sidr.procs[procLSCopy]=(ProcPtr)&LSCopy;
		sidr.procs[procLSCopyString]=(ProcPtr)&LSCopyString;
		sidr.procs[procLSPosCustom]=(ProcPtr)&LSPosCustom;
		sidr.procs[procLSPos]=(ProcPtr)&LSPos;
		sidr.procs[procLSPosChar]=(ProcPtr)&LSPosChar;
		sidr.procs[procLSNextArg]=(ProcPtr)&LSNextArg;
		sidr.procs[procLSNextArgND]=(ProcPtr)&LSNextArgND;
		sidr.procs[procLSStrLS]=(ProcPtr)&LSStrLS;
		sidr.procs[procLSConcatLSAndStr]=(ProcPtr)&LSConcatLSAndStr;
		sidr.procs[procLSConcatStrAndLS]=(ProcPtr)&LSConcatStrAndLS;
		sidr.procs[procLSConcatStrAndStr]=(ProcPtr)&LSConcatStrAndStr;
		sidr.procs[procLSConcatLSAndLS]=(ProcPtr)&LSConcatLSAndLS;
		sidr.procs[procLSConcatLSAndStrAndLS]=(ProcPtr)&LSConcatLSAndStrAndLS;
		sidr.procs[procLSConcatStrAndLSAndStr]=(ProcPtr)&LSConcatStrAndLSAndStr;
		sidr.procs[procLSConcatStrAndStrAndStr]=(ProcPtr)&LSConcatStrAndStrAndStr;
		sidr.procs[procLSStrCat]=(ProcPtr)&LSStrCat;
		sidr.procs[procLSInsertStr]=(ProcPtr)&LSInsertStr;
		sidr.procs[procSMPrefix]=(ProcPtr)&SMPrefix;
		sidr.procs[procSMPrefixColor]=(ProcPtr)&SMPrefixColor;
		sidr.procs[procLineMsg]=(ProcPtr)&LineMsg;
		sidr.procs[procUpdateStatusLine]=(ProcPtr)&UpdateStatusLine;
		sidr.procs[procSetInputLineCursorSelection]=(ProcPtr)&SetInputLineCursorSelection;
		sidr.procs[procGetInputLine]=(ProcPtr)&GetInputLine;
		sidr.procs[procGetLine]=(ProcPtr)&GetLine;
		sidr.procs[procSetInputLine]=(ProcPtr)&SetInputLine;
		sidr.procs[procChannelMsg]=(ProcPtr)&ChannelMsg;
		sidr.procs[procMessage]=(ProcPtr)&Message;
		sidr.procs[procNewPluginMWindow]=(ProcPtr)&NewPluginMWindow;
		sidr.procs[procMWSetDimen]=(ProcPtr)&MWSetDimen;
		sidr.procs[procMWSetFontSize]=(ProcPtr)&MWSetFontSize;
		sidr.procs[procMWDelete]=(ProcPtr)&MWDelete;
		sidr.procs[procMWMessage]=(ProcPtr)&MWMessage;
		sidr.procs[procDoModeLWindow]=(ProcPtr)&DoModeLWindow;
		sidr.procs[procEvtMW]=(ProcPtr)&MWFromWindow;
		sidr.procs[procMWColor]=(ProcPtr)&MWColor;
		sidr.procs[procPluginNewDialog]=(ProcPtr)&pluginNewDialog;
		sidr.procs[procPluginDisposeDialog]=(ProcPtr)&pluginDisposeDialog;
		sidr.procs[procIsChannel]=(ProcPtr)&IsChannel;
		sidr.procs[procIsDCCName]=(ProcPtr)&IsDCCName;
		sidr.procs[procMakeChannel]=(ProcPtr)&MakeChannel;
		sidr.procs[procChFind]=(ProcPtr)&ChFind;
		sidr.procs[procChFindBan]=(ProcPtr)&ChFindBan;
		sidr.procs[procChMatchBan]=(ProcPtr)&ChMatchBan;
		sidr.procs[procChGetBan]=(ProcPtr)&ChGetBan;
		sidr.procs[procMWIrcleColor]=(ProcPtr)&MWIrcleColor;
		sidr.procs[procChKillBan]=(ProcPtr)&ChKillBan;
		sidr.procs[procChKillAllBans]=(ProcPtr)&ChKillAllBans;
		sidr.procs[procULAddUser]=(ProcPtr)&ULAddUser;
		sidr.procs[procULDeleteUser]=(ProcPtr)&ULDeleteUser;
		sidr.procs[procULFindUserName]=(ProcPtr)&ULFindUserName;
		sidr.procs[procChannelWindow]=(ProcPtr)&ChannelWindow;
		sidr.procs[procChMsg]=(ProcPtr)&ChMsg;
		sidr.procs[procParamString]=(ProcPtr)&ParamString;
		sidr.procs[procSecsToHMS]=(ProcPtr)&SecsToHMS;
		sidr.procs[procDeleteConnection]=(ProcPtr)&deleteConnection;
		sidr.procs[procFindConnectionSock]=(ProcPtr)&findConnectionSock;
		sidr.procs[procCMAddSubmenu]=(ProcPtr)&CMAddSubmenu;
		sidr.procs[procPluginNewConnection]=(ProcPtr)&pluginNewConnection;
		sidr.procs[procPutServer]=(ProcPtr)&putServer;
		sidr.procs[procDoONotice]=(ProcPtr)&DoONotice;
		sidr.procs[procSendCTCPReply]=(ProcPtr)&SendCTCPReply;
		sidr.procs[procPMLAdd]=(ProcPtr)&PMLAdd;
		sidr.procs[procNextArg]=(ProcPtr)&NextArg;
		sidr.procs[procHandleCommand]=(ProcPtr)&HandleCommand;
		sidr.procs[procWriteMainPrefs]=(ProcPtr)&writeMainPrefs;
		sidr.procs[procWriteAllFiles]=(ProcPtr)&writeAllFiles;
		sidr.procs[procUlongval]=(ProcPtr)&ulongval;
		sidr.procs[procCMSetCheckmark]=(ProcPtr)&CMSetCheckmark;
		sidr.procs[procUlongstr]=(ProcPtr)&ulongstr;
		sidr.procs[procDccTypToStr]=(ProcPtr)&dccTypToStr;
		sidr.procs[procDccFlagTypToStr]=(ProcPtr)&dccFlagTypToStr;
		sidr.procs[procDccStrToTyp]=(ProcPtr)&DCCTypeFind;
		sidr.procs[procHostToIPStr]=(ProcPtr)&hostToIPStr;
		sidr.procs[procDCCFind]=(ProcPtr)&DCCFind;
		sidr.procs[procDCCClose]=(ProcPtr)&DCCClose;
		sidr.procs[procDCCWindowClose]=(ProcPtr)&DCCWindowClose;
		sidr.procs[procDCCCreate]=(ProcPtr)&DCCCreate;
		sidr.procs[procDCCRequest]=(ProcPtr)&DCCRequest;
		sidr.procs[procDCCOpen]=(ProcPtr)&DCCOpen;
		sidr.procs[procDCCCommand]=(ProcPtr)&DCCCommand;
		sidr.procs[procDCCSSay]=(ProcPtr)&DCCSSay;
		sidr.procs[procSendCommand]=(ProcPtr)&SendCommand;
		sidr.procs[procDNSLookup]=(ProcPtr)&DNSLookup;
		sidr.procs[procDoBroadcast]=(ProcPtr)&doBroadcast;
		sidr.procs[procListIgnores]=(ProcPtr)&ListIgnores;
		sidr.procs[procIsIgnored]=(ProcPtr)&IsIgnored;
		sidr.procs[procDoIgnore]=(ProcPtr)&DoIgnore;
		sidr.procs[procFindIgnore]=(ProcPtr)&findIgnore;
		sidr.procs[procProcessShortcutText]=(ProcPtr)&ProcessShortcutText;
		sidr.procs[procMBIdle]=(ProcPtr)&MBIdle;
		sidr.procs[procMBFindNick]=(ProcPtr)&MBFindNick;
		sidr.procs[procMBNewMessage]=(ProcPtr)&MBNewMessage;
		sidr.procs[procUpc]=(ProcPtr)&upc;
		sidr.procs[procUcase]=(ProcPtr)&ucase;
		sidr.procs[procPdelete]=(ProcPtr)&pdelete;
		sidr.procs[procPinsert]=(ProcPtr)&pinsert;
		sidr.procs[procPstrcpy]=(ProcPtr)&pstrcpy;
		sidr.procs[procPstrcmp]=(ProcPtr)&pstrcmp;
		sidr.procs[procExistsService]=(ProcPtr)&ExistsService;
		sidr.procs[procStandardDialogFilter]=(ProcPtr)&StandardDialogFilter;
		sidr.procs[procPadEnd]=(ProcPtr)&padEnd;
		sidr.procs[procPadBegin]=(ProcPtr)&padBegin;
		sidr.procs[procMaskMatch]=(ProcPtr)&maskMatch;
		sidr.procs[procMakeMask]=(ProcPtr)&makeMask;
		sidr.procs[procCountChar]=(ProcPtr)&countChar;
		sidr.procs[procRevPos]=(ProcPtr)&revPos;
		sidr.procs[procIsNumber]=(ProcPtr)&isNumber;
		sidr.procs[procIsIPNumber]=(ProcPtr)&isIPNumber;
		sidr.procs[procGetText]=(ProcPtr)&GetText;
		sidr.procs[procSetText]=(ProcPtr)&SetText;
		sidr.procs[procSetCheckBox]=(ProcPtr)&setCheckBox;
		sidr.procs[procGetCheckBox]=(ProcPtr)&getCheckBox;
		sidr.procs[procSetButtonEnable]=(ProcPtr)&setButtonEnable;
		sidr.procs[procGetControlHandle]=(ProcPtr)&GetControlHandle;
		sidr.procs[procGetFirstSelectedCell]=(ProcPtr)&getFirstSelectedCell;
		sidr.procs[procSelectOneCell]=(ProcPtr)&selectOneCell;
		sidr.procs[procDrawListBorder]=(ProcPtr)&drawListBorder;
		sidr.procs[procMakeCellVisible]=(ProcPtr)&makeCellVisible;
		sidr.procs[procOpenConnection]=(ProcPtr)&OpenConnection;
		sidr.procs[procDrawPlacard]=(ProcPtr)&DrawPlacard;
		sidr.procs[procSetTextColor]=(ProcPtr)&SetTextColor;
		sidr.procs[procSetDlogFont]=(ProcPtr)&SetDlogFont;
		sidr.procs[procGetControlMenu]=(ProcPtr)&GetControlMenu;
		sidr.procs[procLSPosCase]=(ProcPtr)&LSPosCase;
		sidr.procs[procPos]=(ProcPtr)&pos;
		sidr.procs[procShadowIRCVersion]=(ProcPtr)&ShadowIRCVersion;
		sidr.procs[procPluginNewWindow]=(ProcPtr)&pluginNewWindow;
		sidr.procs[procPluginDisposeWindow]=(ProcPtr)&pluginDisposeWindow;
		sidr.procs[procNewService]=(ProcPtr)&NewService;
		sidr.procs[procPFOpen]=(ProcPtr)&PFOpen;
		sidr.procs[procPFExists]=(ProcPtr)&PFExists;
		sidr.procs[procPFCreate]=(ProcPtr)&PFCreate;
		sidr.procs[procPFWrite]=(ProcPtr)&PFWrite;
		sidr.procs[procPFRead]=(ProcPtr)&PFRead;
		sidr.procs[procPFSize]=(ProcPtr)&PFSize;
		sidr.procs[procPFResize]=(ProcPtr)&PFResize;
		sidr.procs[procPFSetPos]=(ProcPtr)&PFSetPos;
		sidr.procs[procPFClose]=(ProcPtr)&PFClose;
		sidr.procs[procPFDelete]=(ProcPtr)&PFDelete;
		sidr.procs[procFrontNonFloater]=(ProcPtr)&FrontNonFloater;
		sidr.procs[procGetPluginWindowOwner]=(ProcPtr)&GetPluginWindowOwner;
		sidr.procs[procWIsFloater]=(ProcPtr)&WIsFloater;
		sidr.procs[procWMoveToFront]=(ProcPtr)&WMoveToFront;
		sidr.procs[procWSelect]=(ProcPtr)&WSelect;
		sidr.procs[procWDrag]=(ProcPtr)&WDrag;
		sidr.procs[procWShow]=(ProcPtr)&WShow;
		sidr.procs[procWHide]=(ProcPtr)&WHide;
		sidr.procs[procPstrcasecmp]=(ProcPtr)&pstrcasecmp;
		sidr.procs[procPstrcasecmp2]=(ProcPtr)&pstrcasecmp2;
		sidr.procs[procGetQD]=(ProcPtr)&GetQD;
		sidr.procs[procWMove]=(ProcPtr)&WMove;
		sidr.procs[procEnterModalDialog]=(ProcPtr)&EnterModalDialog;
		sidr.procs[procExitModalDialog]=(ProcPtr)&ExitModalDialog;
		sidr.procs[procLSConcatLSAndStrAndStr]=(ProcPtr)&LSConcatLSAndStrAndStr;
		sidr.procs[procIPStringToLong]=(ProcPtr)&IPStringToLong;
		sidr.procs[procSetSmartScrollInfo]=(ProcPtr)&SetSmartScrollInfo;
		sidr.procs[procSetSmartScrollProp]=(ProcPtr)&SetSmartScrollProp;
		sidr.procs[procGetSmartScrollProp]=(ProcPtr)&GetSmartScrollProp;
		sidr.procs[procLCAttach]=(ProcPtr)&LCAttach;
		sidr.procs[procLCDetach]=(ProcPtr)&LCDetach;
		sidr.procs[procLCSetValue]=(ProcPtr)&LCSetValue;
		sidr.procs[procLCSetMin]=(ProcPtr)&LCSetMin;
		sidr.procs[procLCSetMax]=(ProcPtr)&LCSetMax;
		sidr.procs[procLCGetValue]=(ProcPtr)&LCGetValue;
		sidr.procs[procLCGetMin]=(ProcPtr)&LCGetMin;
		sidr.procs[procLCGetMax]=(ProcPtr)&LCGetMax;
		sidr.procs[procLCSynch]=(ProcPtr)&LCSynch;
		sidr.procs[procIsChannelValid]=(ProcPtr)&IsChannelValid;
		sidr.procs[procIsUserValid]=(ProcPtr)&IsUserValid;
		sidr.procs[procConnPut]=(ProcPtr)&ConnPut;
		sidr.procs[procConnPutText]=(ProcPtr)&ConnPutText;
		sidr.procs[procConnPutLS]=(ProcPtr)&ConnPutLS;
		sidr.procs[procStackModes]=(ProcPtr)&StackModes;
		sidr.procs[procOpenPreferencesWindow]=(ProcPtr)&OpenPreferencesWindow;
		sidr.procs[procSMPrefixIrcleColor]=(ProcPtr)&SMPrefixIrcleColor;
		sidr.procs[procAsyncSoundPlay]=(ProcPtr)&AsyncSoundPlay;
		sidr.procs[procLSCmp]=(ProcPtr)&LSCmp;
		sidr.procs[procLSCaseCmp]=(ProcPtr)&LSCaseCmp;
		sidr.procs[procLSGetIndString]=(ProcPtr)&LSGetIndString;
		sidr.procs[procLSParamString]=(ProcPtr)&LSParamString;
		sidr.procs[procLSParamLS]=(ProcPtr)&LSParamLS;
		sidr.procs[procSMLink]=(ProcPtr)&SMLink;
		sidr.procs[procSMPrefixLink]=(ProcPtr)&SMPrefixLink;
		sidr.procs[procSMPrefixLinkColor]=(ProcPtr)&SMPrefixLinkColor;
		sidr.procs[procSMPrefixLinkIrcleColor]=(ProcPtr)&SMPrefixLinkIrcleColor;
		sidr.procs[procIPCRegister]=(ProcPtr)&IPCRegister;
		sidr.procs[procIPCExists]=(ProcPtr)&IPCExists;
		sidr.procs[procIPCCall]=(ProcPtr)&IPCSend;
		sidr.procs[procDoNotify]=(ProcPtr)&DoNotify;
		sidr.procs[procRunNotify]=(ProcPtr)&RunNotify;
		sidr.procs[procListNotify]=(ProcPtr)&ListNotify;
		sidr.procs[procfindNotify]=(ProcPtr)&findNotify;
		sidr.procs[procdeleteNotify]=(ProcPtr)&deleteNotify;
		sidr.procs[procaddNotify]=(ProcPtr)&addNotify;
		sidr.procs[procDrawMWinStatus]=(ProcPtr)&DrawMWinStatus;
		sidr.procs[procConnClose]=(ProcPtr)&ConnClose;
		sidr.procs[procConnAbort]=(ProcPtr)&ConnAbort;
		sidr.procs[procConnNewActive]=(ProcPtr)&ConnNewActive;
		sidr.procs[procConnNewPassive]=(ProcPtr)&ConnNewPassive;
		sidr.procs[procConnGetData]=(ProcPtr)&ConnGetData;
		sidr.procs[procConnCharsAvail]=(ProcPtr)&ConnCharsAvail;
		sidr.procs[procIWNewObject]=(ProcPtr)&IWNewWidget;
		sidr.procs[procIWRecalculateRects]=(ProcPtr)&IWRecalculateRects;
		sidr.procs[procIWPopUpMenu]=(ProcPtr)&IWPopUpMenu;
		sidr.procs[procIWDeleteObject]=(ProcPtr)&IWDeleteWidget;
		sidr.procs[procIWOverride]=(ProcPtr)&IWOverride;
		sidr.procs[procMWNewWidget]=(ProcPtr)&MWNewWidget;
		sidr.procs[procMWRecalculateRects]=(ProcPtr)&MWRecalculateRects;
		sidr.procs[procMWDestroyWidget]=(ProcPtr)&MWDestroyWidget;
		sidr.procs[procHMIAdd]=(ProcPtr)&HMIAdd;
		sidr.procs[procMWFindWidget]=(ProcPtr)&MWFindWidget;
		sidr.procs[procMWFindPane]=(ProcPtr)&MWFindPane;
		sidr.procs[procMWNewPane]=(ProcPtr)&MWNewPane;
		sidr.procs[procMWPaneRecalculate]=(ProcPtr)&MWPaneRecalculate;
		sidr.procs[procMWPaneResize]=(ProcPtr)&MWPaneResize;
		sidr.procs[procStartDrawingOffscreen]=(ProcPtr)&StartDrawingOffscreen;
		sidr.procs[procEndDrawingOffscreen]=(ProcPtr)&EndDrawingOffscreen;
		sidr.procs[procAbortDrawingOffscreen]=(ProcPtr)&AbortDrawingOffscreen;
		sidr.procs[procMWDestroyPane]=(ProcPtr)&MWDestroyPane;
		sidr.procs[procLSInsertChar]=(ProcPtr)&LSInsertChar;
		sidr.procs[procLSInsertShort]=(ProcPtr)&LSInsertShort;
		sidr.procs[procLSInsertLong]=(ProcPtr)&LSInsertLong;
		sidr.procs[procLightenColor]=(ProcPtr)&LightenColor;
		sidr.procs[procNavDialogFilter]=(ProcPtr)&NavDialogFilter;
		sidr.procs[procDirectorySelectButton]=(ProcPtr)&DirectorySelectButton;
		sidr.procs[procCleanFolderFSp]=(ProcPtr)&CleanFolderFSp;
		sidr.procs[procDoDCCSend]=(ProcPtr)&DoDCCSend;
		sidr.procs[procDoDCCSendFile]=(ProcPtr)&DoDCCSendFile;
		sidr.procs[procDCCGetAFile]=(ProcPtr)&DCCGetAFile;
		sidr.procs[procInputHandler]=(ProcPtr)&InputHandler;
		sidr.procs[procOpenURL]=(ProcPtr)&OpenURL;
		sidr.procs[procDCCTypeAdd]=(ProcPtr)&DCCTypeAdd;
		sidr.procs[procDCCGetStatus]=(ProcPtr)&DCCGetStatus;
		sidr.procs[procProcessLine]=(ProcPtr)&ProcessLine;
		sidr.procs[procProcessInputHunk]=(ProcPtr)&ProcessInputHunk;
		sidr.procs[procDragHilightRect]=(ProcPtr)&DragHilightRect;
		sidr.procs[procDragHilightRectBG]=(ProcPtr)&DragHilightRectBG;
		sidr.procs[procDragIsTypeAvail]=(ProcPtr)&DragIsTypeAvail;
		sidr.procs[procDragGetType]=(ProcPtr)&DragGetType;
		sidr.procs[procDragGetPtr]=(ProcPtr)&DragGetPtr;
		sidr.procs[procDragAddPtr]=(ProcPtr)&DragAddPtr;
		sidr.procs[procDCCSendCookie]=(ProcPtr)&DCCSendCookie;
		sidr.procs[procDCCSendCookieReply]=(ProcPtr)&DCCSendCookieReply;
		sidr.procs[procCMAdd]=(ProcPtr)&CMAdd;
		sidr.procs[procIPCReply]=(ProcPtr)&IPCReply;
		sidr.procs[procLSPosCaseCustom]=(ProcPtr)&LSPosCaseCustom;
		sidr.procs[procSetBackground]=(ProcPtr)&SetBackground;
		sidr.procs[procMySendControlMessage]=(ProcPtr)&MySendControlMessage;
		sidr.procs[procSMPrefixRGBColor]=(ProcPtr)&SMPrefixRGBColor;
		sidr.procs[procSMPrefixRGBLinkColor]=(ProcPtr)&SMPrefixRGBLinkColor;
		sidr.procs[procpluginSetWRefCon]=(ProcPtr)&pluginSetWRefCon;
		sidr.procs[procpluginGetWRefCon]=(ProcPtr)&pluginGetWRefCon;
		sidr.procs[procMWPaneUpdate]=(ProcPtr)&MWPaneUpdate;
		sidr.procs[procShadowIRCVersion2]=(ProcPtr)&ShadowIRCVersion2;
		sidr.procs[procSetDlogItemTextHdl]=(ProcPtr)&SetDlogItemTextHdl;
		sidr.procs[procDrawBorder]=(ProcPtr)&DrawBorder;
		sidr.procs[procFileAdd]=(ProcPtr)&FileAdd;
		sidr.procs[procFileClose]=(ProcPtr)&FileClose;
		sidr.procs[procIsIgnoredNickUser]=(ProcPtr)&IsIgnoredNickUser;
		sidr.procs[procWMSGetMenuItemNum]=(ProcPtr)&WMSGetMenuItemNum;
		sidr.procs[procGetInputLineCursorSelection]=(ProcPtr)&GetInputLineCursorSelection;
		sidr.procs[procLockInput]=(ProcPtr)&LockInput;
		sidr.procs[procUnlockInput]=(ProcPtr)&UnlockInput;
		sidr.procs[procDoServer]=(ProcPtr)&DoServer;
		sidr.procs[procDoSignoff]=(ProcPtr)&DoSignoff;
		sidr.procs[procMWGetName]=(ProcPtr)&MWGetName;
		sidr.procs[procMWGetChannel]=(ProcPtr)&MWGetChannel;
		sidr.procs[procMWGetDCC]=(ProcPtr)&MWGetDCC;
		sidr.procs[procDoModeKWindow]=(ProcPtr)&DoModeKWindow;
		sidr.procs[procConnGetLocalPort]=(ProcPtr)&ConnGetLocalPort;
		sidr.procs[procRegisterAETE]=(ProcPtr)&RegisterAETE;
		sidr.procs[procNickListAdd]=(ProcPtr)&NickListAdd;
		sidr.procs[procChannelListAdd]=(ProcPtr)&ChannelListAdd;
		sidr.procs[procGetDrawingState]=(ProcPtr)&GetDrawingState;
		sidr.procs[procSetDrawingState]=(ProcPtr)&SetDrawingState;
		sidr.procs[procNormalizeDrawingState]=(ProcPtr)&NormalizeDrawingState;
		sidr.procs[procGetAppearanceListBoxHandle]=(ProcPtr)&GetAppearanceListBoxHandle;
		sidr.procs[procCopyResource]=(ProcPtr)&CopyResource;
		sidr.procs[procPstrcat]=(ProcPtr)&pstrcat;
		sidr.procs[procMWPart]=(ProcPtr)&MWPart;
		sidr.procs[procWindowClose]=(ProcPtr)&WindowClose;
#endif
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
	
/*
	if(FSMakeFSSpec(pluginsFolderFSSpec.vRefNum, pluginsFolderFSSpec.parID,"\pPreferences", &pluginPrefsFSSpec))
	{
		//no prefs folder, so make it
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
	else
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
*/
}

pascal void makePlugsDB(void)
{
	FSSpec tempFSSpec;
	char isDir;
	LongString ls;
	long dirid;
	
	initSIDR();
	
	hmiList=(hmiListHand)NewHandleClear(sizeof(long));
	
	FindAppSpec(&pluginsFolderFSSpec); //Get the app apec
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

#pragma internal reset

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