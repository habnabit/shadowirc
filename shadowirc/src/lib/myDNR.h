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

#ifndef _myDNR_
#define _myDNR_

#ifndef __ADDRESSXLATION__
#include "AddressXlation.h"
#endif

#pragma internal on
pascal OSErr DNROpenResolver(void);
pascal void DNRCloseResolver(void);
pascal OSErr DNRStrToAddr(ConstStr255Param host, hostInfo *rtnStruct, UniversalProcPtr completion, Ptr userData);
pascal void DNRAddrToStr(long addr, Str255 s);
pascal OSErr DNRAddrToName(long addr, hostInfo *hi, UniversalProcPtr completion, Ptr userData);
pascal OSErr DNREnumCache(Ptr completion, Ptr userData);
pascal OSErr DNRHInfo(ConstStr255Param host, Ptr hi, Ptr completion, Ptr userData);
pascal OSErr DNRMXInfo(ConstStr255Param host, Ptr hi, Ptr completion, Ptr userData);
#pragma internal reset
#endif