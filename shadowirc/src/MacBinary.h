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

#ifndef _MacBinary
#define _MacBinary

enum {
	PT_None, PT_File, PT_StartBlock, PT_EndBlock
}; //for pnl

typedef struct MBIIStartHeader {
	Str63 name;						//file name
	OSType ftype;					//type
	OSType fcreator;				//creator
	UInt8 flags_high;
	UInt8 zero1;
	Point flocation;
	short windowID;
	UInt8 fileLocked;
	UInt8 zero2;
	long dlen, rlen, create_date, mod_date;
	short clen;
	UInt8 flags_low;
	char sig[4];
	char script;
	char flags_x;
} MBIIStartHeader;

typedef struct MBIIHeader {
	SInt8 version;
	SInt8 MBIIStart;					//struct MBIIStartHeader starts here
	UInt8 space[114];
	long total_unpack_len;
	short second_header_len;
	UInt8 versionII;
	UInt8 miniversionII;
	short crc;
	short processorID;
} MBIIHeader, *MBIIHeaderPtr;

#pragma internal on
pascal short ValidateMBHeader(MBIIHeaderPtr header, char handle2plus);
pascal void CatInfo2MBHeader(CInfoPBRec *pb, MBIIHeaderPtr header);
#pragma internal reset
#endif