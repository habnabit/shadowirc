/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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

#ifndef _HashTable_
#define _HashTable_

#define DEFAULT_HASHSIZE 31

enum dataType
{
	htTypeNull,
	htTypeInt,
	htTypeString
};

typedef int htDataType;

typedef struct htElt
{
	struct htElt *next;
	
	StringPtr name;
	void* data;
	
	htDataType type;
} htElt, *htEltPtr;

typedef struct HashTable
{
	int hashSize;
	
	htEltPtr* hashes;
} HashTable, *HTPtr;

HTPtr HTCreate(int hashSize);
HTPtr HTCreateDuplicate(HTPtr table);
void HTInit(HTPtr table, int hashSize);
void HTClear(HTPtr table);
void HTDestroy(HTPtr table);

void HTAdd(HTPtr table, ConstStr255Param name, void* data, htDataType type);
void HTCopyElt(HTPtr table, HTPtr srcTable, ConstStr255Param name);
char HTDelete(HTPtr table, ConstStr255Param name);

char HTFind(HTPtr table, ConstStr255Param name, htDataType* type, void** found);
char HTFindNumeric(HTPtr table, ConstStr255Param name, long *num);
long HTFindNumericDefault(HTPtr table, ConstStr255Param name, long def);
char HTIsSet(HTPtr table, ConstStr255Param name);

#endif