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

	Note: This code based on code by Peter N. Lewis.
*/

#if TARGET_CARBON
#include <Files.h>
#endif

#include "MacBinary.h"
#include "MyMemory.h"
#include "CalcCRC.h"

#include "utils.h"
#include "inline.h"

enum {
	macbin_folder_ftype = 'fold',
	macbin_folder_creator_start = -1,
	macbin_folder_creator_end = -2
};

//typedef UInt8 MBpacket[128];
typedef char* MBpacket;

pascal short ValidateMBHeader(MBIIHeaderPtr header, char handle2plus)
{
	short ocrc, typ;
	MBIIStartHeader start;
	short i;
	
	BlockMoveData(&header->MBIIStart, &start, sizeof(start));
	typ = PT_None;
	
	if(header->version <= handle2plus && ((MBpacket)header)[74] == 0)
	{
		ocrc = 0;
		for(i=0;i<=123;i++)
			CalcMBCRC(&ocrc, ((MBpacket)header)[i]);
		if(ocrc == header->crc)
		{
			if(header->version == 1 && start.ftype == macbin_folder_ftype && (start.fcreator == macbin_folder_creator_start) || start.fcreator == macbin_folder_creator_end)
			{
				if(start.fcreator == macbin_folder_creator_start)
					typ = PT_StartBlock;
				else
					typ = PT_EndBlock;
			}
			else
				typ = PT_File;
		}
		else //assume MB1 file
		{
			if(header->version == 0)
			{
				((MBpacket)header)[101] = 0; //low_byte flags
				header->total_unpack_len = 0;
				header->second_header_len = 0;
				header->versionII = 130; //MB3
				header->miniversionII = 129;
				header->crc = 0;
				header->processorID = 0;
			}
		}
	}

	return typ;
}

pascal void CatInfo2MBHeader(CInfoPBRec *pb, MBIIHeaderPtr header)
{
	MBIIStartHeader start;
	char folder;
	int i;
	
	folder = (pb->hFileInfo.ioFlAttrib & 0x10) != 0;
	MFillLong(header, sizeof(MBIIHeader), 0);
	MFill(&start, sizeof(start), 0);
	header->version = folder;
	header->versionII = 130 + folder;
	header->miniversionII = 129 + folder;
	
	pstrcpy(pb->hFileInfo.ioNamePtr, start.name);
	start.flags_high = (pb->hFileInfo.ioFlFndrInfo.fdFlags >> 8) & 0xFF;
	start.flags_low = (pb->hFileInfo.ioFlFndrInfo.fdFlags) & 0xFF;
	start.flocation = pb->hFileInfo.ioFlFndrInfo.fdLocation;
	start.windowID = pb->hFileInfo.ioFlFndrInfo.fdFldr;
	start.create_date = pb->hFileInfo.ioFlCrDat;
	start.mod_date = pb->hFileInfo.ioFlMdDat;
	*(long*)&start.sig = 'mBIN'; //MB3
	start.script = pb->hFileInfo.ioFlXFndrInfo.fdScript;
	start.flags_x = pb->hFileInfo.ioFlXFndrInfo.fdXFlags;
	
	if(folder)
	{
		start.ftype = macbin_folder_ftype;
		start.fcreator = macbin_folder_creator_start;
		start.dlen = 0;
		start.rlen = 0;
	}
	else
	{
		start.ftype = pb->hFileInfo.ioFlFndrInfo.fdType;
		start.fcreator = pb->hFileInfo.ioFlFndrInfo.fdCreator;
		start.dlen = pb->hFileInfo.ioFlLgLen;
		start.rlen = pb->hFileInfo.ioFlRLgLen;
	}
	
	start.clen = 0;
	BlockMoveData(&start, &header->MBIIStart, sizeof(start));
	header->crc = 0;
	for(i=0;i<124;i++)
		CalcMBCRC(&header->crc, ((MBpacket)header)[i]);
}

