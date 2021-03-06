/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2005 John Bafford
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

const unsigned char CL_VERSION[] = "\p2.0a14";
#define SIVersion 0x0200000E

#include "IRCGlobals.h"
#include "utils.h"
#include "Inline.h"

long ShadowIRCVersion(StringPtr str);
long ShadowIRCVersion2(StringPtr v, StringPtr date);
long ShadowIRCVersion(StringPtr str){if(str)pstrcpy(CL_VERSION, str);return SIVersion;}
long ShadowIRCVersion2(StringPtr v, StringPtr date){if(v) pstrcpy(CL_VERSION,v); if(date) pstrcpy(cdt,date);return SIVersion;}

void setVers(void);
void setVers(void)
{
	//compile date & time
	CopyCStringToPascal(__DATE__  " " __TIME__, cdt);
	
	//shadowIRCDefaultSignoff
	pstrcat("\pShadowIRC ", CL_VERSION, shadowIRCDefaultSignoff);
}
