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

	Note: Based on Peter N. Lewis's MacTCP code.
*/

#include "inline.h"
#include "myDNR.h"

#pragma internal on

#pragma dont_inline off

inline pascal void GetSystemFolder(short *vrn, long *dirID);
inline pascal void GetCPanelFolder(short *vrn, long *dirID);
static pascal Handle SearchFolderForDNRP(OSType type, OSType creator, short vrn, long dirID);
static pascal Handle SearchForDNRP(void);

#if __POWERPC__
#define New68kProc(proc, type) NewRoutineDescriptor(proc, type, kM68kISA)
#else
#define New68kProc(proc, type) (proc)
#endif

static pascal void InitDNRUPP(void);
static pascal void KillDNRUPP(void);

enum {
	kOPENRESOLVER = 1,
	kCLOSERESOLVER = 2,
	kSTRTOADDR = 3,
	kADDRTOSTR = 4,
	kENUMCACHE = 5,
	kADDRTONAME = 6,
	kHINFO = 7,
	kMXINFO = 8
};

static Handle code;
static OpenResolverUPP uppOpenResolverProc;
static CloseResolverUPP uppCloseResolverProc;
static AddrToStrUPP uppAddrToStrProc;
static AddrToNameUPP uppAddrToNameProc;
static StrToAddrUPP uppStrToAddrProc;
//EnumCacheProcPtr uppEnumCacheProc;
//HInfoProcPtr uppHInfoProc;
//MXInfoProcPtr uppMXInfoProc;

inline pascal void GetSystemFolder(short *vrn, long *dirID)
{
	if(FindFolder(kOnSystemDisk, kSystemFolderType, false, vrn, dirID))
	{
		*vrn = 0;
		*dirID = 0;
	}
}

inline pascal void GetCPanelFolder(short *vrn, long *dirID)
{
	if(FindFolder(kOnSystemDisk, kControlPanelFolderType, false, vrn, dirID))
	{
		*vrn = 0;
		*dirID = 0;
	}
}

static pascal Handle SearchFolderForDNRP(OSType type, OSType creator, short vrn, long dirID)
{
	HParamBlockRec pb;
	Str63 fname;
	short refNum;
	short i;
	Handle h;
	OSErr err;
	
	h = 0;
	i = 1;
	do {
		pb.ioParam.ioNamePtr = fname;
		pb.ioParam.ioVRefNum = vrn;
		pb.fileParam.ioDirID = dirID;
		pb.fileParam.ioFDirIndex = i;
		i++;
		err = PBHGetFInfoSync(&pb);
		if(!err)
		{
			if(pb.fileParam.ioFlFndrInfo.fdType == type & pb.fileParam.ioFlFndrInfo.fdCreator == creator)
			{
				SetResLoad(false);
				refNum = HOpenResFile(vrn, dirID, fname, fsRdPerm);
				SetResLoad(true);
				if(refNum != -1)
				{
					h = Get1IndResource('dnrp', 1);
					if(h)
						DetachResource(h);
					CloseResFile(refNum);
				}
			}
		}
	} while(!err && !h);
	
	return h;
}


static pascal Handle SearchForDNRP(void)
{
	Handle h;
	short vrn;
	long dirIDC, dirIDS;
	
	//first search Control Panels for MacTCP 1.1
	GetCPanelFolder(&vrn, &dirIDC);
	h = SearchFolderForDNRP('cdev', 'ztcp', vrn, dirIDC);
	
	//next search System Folder for MacTCP 1.0.x
	if(!h)
	{
		GetSystemFolder(&vrn, &dirIDS);
		h = SearchFolderForDNRP('cdev', 'mtcp', vrn, dirIDS);
	
		//then search Control Panels for MacTCP 1.0.x
		if(!h)
		{
			h = SearchFolderForDNRP('cdev', 'mtcp', vrn, dirIDC);

			//finally, look in any open resource file
			if(!h)
			{
				h = Get1IndResource('dnrp', 1);
				if(h)
					DetachResource(h);
			}
		}
	}
	
	return h;
}


static pascal void InitDNRUPP(void)
{
	uppCloseResolverProc = (CloseResolverUPP)New68kProc((ProcPtr)*code, uppCloseResolverProcInfo);
	uppOpenResolverProc = (OpenResolverUPP)New68kProc((ProcPtr)*code, uppOpenResolverProcInfo);
	uppAddrToStrProc = (AddrToStrUPP)New68kProc((ProcPtr)*code, uppAddrToStrProcInfo);
	uppAddrToNameProc = (AddrToNameUPP)New68kProc((ProcPtr)*code, uppAddrToNameProcInfo);
	uppStrToAddrProc = (StrToAddrUPP)New68kProc((ProcPtr)*code, uppStrToAddrProcInfo);
//	uppEnumCacheProc = New68kProc(*code, uppEnumCacheProcInfo);
//	uppHInfoProc = New68kProc(*code, uppHInfoProcInfo);
//	uppMXInfoProc = New68kProc(*code, uppMXInfoProcInfo);
}

static pascal void KillDNRUPP(void)
{
	DisposeRoutineDescriptor(uppCloseResolverProc);
	DisposeRoutineDescriptor(uppOpenResolverProc);
	DisposeRoutineDescriptor(uppAddrToNameProc);
	DisposeRoutineDescriptor(uppAddrToStrProc);
	DisposeRoutineDescriptor(uppStrToAddrProc);
//	DisposeRoutineDescriptor(uppEnumCacheProc);
//	DisposeRoutineDescriptor(uppHInfoProc);
//	DisposeRoutineDescriptor(uppMXInfoProc);
}

pascal OSErr DNROpenResolver(void)
{
	OSErr err;
	
	code = SearchForDNRP();
	
	if(code)
	{
		HLock(code);
		InitDNRUPP();
		err = CallOpenResolverProc(uppOpenResolverProc, kOPENRESOLVER, 0);
		if(err)
		{
			DisposeHandle(code);
			code = 0;
			KillDNRUPP();
		}
		
		return err;
	}
	else
		return resNotFound;
}

pascal void DNRCloseResolver(void)
{
	if(code)
	{
		CallCloseResolverProc(uppCloseResolverProc, kCLOSERESOLVER);
		DisposeHandle(code);
		code = 0;
		KillDNRUPP();
	}
}

pascal OSErr DNRStrToAddr(ConstStr255Param host, hostInfo* rtnStruct, UniversalProcPtr completion, Ptr userData)
{
	if(!code)
		return notOpenErr;
	else
	{
		char s[256];
		BlockMoveData(&host[1], s, host[0]);
		s[host[0]] = 0;
		return CallStrToAddrProc(uppStrToAddrProc, kSTRTOADDR, s, rtnStruct, (ResultUPP)completion, userData);
	}
}

pascal void DNRAddrToStr(long addr, Str255 s)
{
	int len;
	
	if(code)
	{
		CallAddrToStrProc(uppAddrToStrProc, kADDRTOSTR, addr, (char*)s);
		len = 0;
		while(s[len] && len<255)
			len++;
		BlockMoveData(s, &s[1], len);
		s[0] = len;
	}
}

pascal OSErr DNRAddrToName(long addr, hostInfo *hi, UniversalProcPtr completion, Ptr userData)
{
	if(!code)
		return notOpenErr;
	else
		return CallAddrToNameProc(uppAddrToNameProc, kADDRTONAME, addr, hi, (ResultUPP)completion, userData);
}
/*
pascal OSErr EnumCache(Ptr completion, Ptr userData)
{
	if(!code)
		return notOpenErr;
	else
		return CallEnumCacheProc(uppEnumCacheProc, kENUMCACHE, completion, userData);
}

pascal OSErr HInfo(ConstStr255Param host, Ptr hi, Ptr completion, Ptr userData)
{
	if(!code)
		return notOpenErr;
	else
	{
		Str255 s;
		pstrcpy(host, s);
		P2CStr(s);
		return CallHInfoProc(uppHInfoProc, kHINFO, s, hi, completion, userData);
	}
}

pascal OSErr MXInfo(ConstStr255Param host, Ptr hi, Ptr completion, Ptr userData)
{
	if(!code)
		return notOpenErr;
	else
	{
		Str255 s;
		pstrcpy(host, s);
		P2CStr(s);
		return CallMXInfoProc(uppMXInfoProc, kMXINFO, s, hi, completion, userData);
	}
}
*/