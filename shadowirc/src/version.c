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

unsigned char myVersion[] = "\p2.0a1";
#define SIVersion 0x02000001

#include "IRCGlobals.h"
#include "utils.h"
#include "Inline.h"

extern Str63 shadowIRCDefaultSignoff;

pascal long ShadowIRCVersion(StringPtr str);
pascal long ShadowIRCVersion2(StringPtr v, StringPtr date);
pascal long ShadowIRCVersion(StringPtr str){if(str)pstrcpy(CL_VERSION, str);return SIVersion;}
pascal long ShadowIRCVersion2(StringPtr v, StringPtr date){if(v) pstrcpy(myVersion,v); if(date) pstrcpy(cdt,date);return SIVersion;}

pascal void setVers(void);
pascal void setVers(void)
{
	short i;
	char date[]=__DATE__;
	char time[]=__TIME__;

	pstrcpy(myVersion, CL_VERSION);
	
	//expdate
	strc2p(date);
	strc2p(time);
	pstrcpy((unsigned char*)date, cdt);
	i=cdt[0]+1;
	pstrcpy((unsigned char*)time, &cdt[i]);
	cdt[0]+=cdt[i]+1;
	cdt[i]=' ';
	//shadowIRCDefaultSignoff
	pstrcpy("\pShadowIRC ", shadowIRCDefaultSignoff);
	i=CL_VERSION[0];
	pstrcpy(CL_VERSION, &shadowIRCDefaultSignoff[shadowIRCDefaultSignoff[0]]);
	shadowIRCDefaultSignoff[shadowIRCDefaultSignoff[0]]=' ';
	shadowIRCDefaultSignoff[0]+=i;
}
