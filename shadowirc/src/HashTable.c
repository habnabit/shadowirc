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

#include "inline.h"
#include "utils.h"

#include "HashTable.h"

static void _HTCopyList(HTPtr table, HTPtr srcTable);


HTPtr HTCreate(int hashSize)
{
	HTPtr ht = (HTPtr)NewPtr(sizeof(HashTable));
	
	HTInit(ht, hashSize);
	
	return ht;
}

HTPtr HTCreateDuplicate(HTPtr table)
{
	HTPtr ht = HTCreate(table->hashSize);
	
	_HTCopyList(ht, table);
	
	return ht;
}

void HTInit(HTPtr table, int hashSize)
{
	int i;
	
	table->hashSize = hashSize;
	table->hashes = (htEltPtr*)NewPtr(sizeof(htEltPtr) * hashSize);
	
	for(i=0; i < hashSize; i++)
		table->hashes[i] = NULL;
}

static void _HTClearList(htEltPtr e)
{
	linkfor(e, e)
	{
		DisposePtr((Ptr)e->name);
		if(e->type == htTypeString)
			DisposePtr((Ptr)e->data);
	}
}

void HTClear(HTPtr table)
{
	int i;

	for(i=0; i < table->hashSize; i++)
		if(table->hashes[i])
		{
			_HTClearList(table->hashes[i]);
			table->hashes[i] = 0;
		}
	
	DisposePtr((Ptr)table->hashes);
	table->hashSize = 0;
}

void HTDestroy(HTPtr table)
{
	if(table)
	{
		HTClear(table);
		DisposePtr((Ptr)table);
	}
}

static int _HTHash(HTPtr table, ConstStr255Param name)
{
	int x, max = name[0];
	int key = 0;
	
	for(x = 0; x < max; x++)
		key += name[x];
	
	return key % table->hashSize;
}

static htEltPtr _HTFindEltFromSublist(htEltPtr list, ConstStr255Param name)
{
	linkfor(list, list)
		if(pstrcmp(list->name, name))
			return list;
	
	return NULL;
}

static htEltPtr _HTFindEltAndPrevFromSublist(htEltPtr list, ConstStr255Param name, htEltPtr *prevElt)
{
	htEltPtr pe = NULL;
	
	linkfor(list, list)
		if(pstrcmp(list->name, name))
		{
			*prevElt = pe;
			return list;
		}
		else
			pe = list;
	
	return NULL;
}

static htEltPtr _HTFindElt(HTPtr table, ConstStr255Param name)
{
	return _HTFindEltFromSublist(table->hashes[_HTHash(table, name)], name);
}

char HTFind(HTPtr table, ConstStr255Param name, htDataType* type, void** found)
{
	htEltPtr elt = _HTFindElt(table, name);
	
	if(elt)
	{
		if(type)
			*type = elt->type;
		if(found)
			*found = elt->data;
		
		return true;
	}
	else
		return false;
}

char HTFindNumeric(HTPtr table, ConstStr255Param name, long *num)
{
	void* found;
	htDataType type;
	
	if(HTFind(table, name, &type, &found) && type == htTypeInt)
	{
		*num = (long)found;
		return true;
	}
	else
		return false;
}

long HTFindNumericDefault(HTPtr table, ConstStr255Param name, long def)
{
	void* found;
	htDataType type;
	
	if(HTFind(table, name, &type, &found) && type == htTypeInt)
		return (long)found;
	else
		return def;
}

static void _HTStrDup(ConstStringPtr str, StringPtr* dest)
{
	*dest = NewPString(str);
}

static void _HTSetElt(htEltPtr elt, void* newData, int type)
{
	elt->type = type;
	if(type == htTypeInt)
		elt->data = newData;
	else if(type == htTypeString)
		_HTStrDup(newData, (StringPtr*)&elt->data);
	else
		elt->data = 0;
}


static htEltPtr _HTNewElt(ConstStr255Param newName, void* newData, htDataType type, htEltPtr next)
{
	htEltPtr elt = (htEltPtr)NewPtr(sizeof(htElt));

	_HTStrDup(newName, &elt->name);
	_HTSetElt(elt, newData, type);
	elt->next = next;

	return elt;
}

static void _HTDisposeElt(htEltPtr elt)
{
	elt->next = NULL;
	
	DisposePtr(elt->name);
	elt->name = NULL;
	
	if(elt->type == htTypeString)
		DisposePtr(elt->data);
	elt->data = NULL;
	
	DisposePtr((Ptr)elt);
}

static void _HTModElt(htEltPtr elt, void* newData, htDataType type)
{
	if(elt->type == htTypeString)
		DisposePtr(elt->data);
	
	_HTSetElt(elt, newData, type);
}

static void _HTInsert(htEltPtr *first, ConstStr255Param newName, void* newData, htDataType type)
{
	htEltPtr list = _HTFindEltFromSublist(*first, newName);
	
	if(list) //modification of old element
	{
		_HTModElt(list, newData, type);
		return;
	}
	
	//Else new thing
	*first = _HTNewElt(newName, newData, type, *first);
}

static void _HTCopyList(HTPtr table, HTPtr srcTable)
{
	int x, max = srcTable->hashSize;
	htEltPtr list;
	
	for(x = 0; x < max; x++)
		linkfor(list, srcTable->hashes[x])
			table->hashes[x] = _HTNewElt(list->name, list->data, list->type, table->hashes[x]);
}

void HTAdd(HTPtr table, ConstStr255Param name, void* data, htDataType type)
{
	_HTInsert(&table->hashes[_HTHash(table, name)], name, data, type);
}

char HTDelete(HTPtr table, ConstStr255Param name)
{
	htEltPtr *list;
	htEltPtr elt, prevElt;
	
	list = &table->hashes[_HTHash(table, name)];
	
	elt = _HTFindEltAndPrevFromSublist(*list, name, &prevElt);
	
	if(elt)
	{
		//if prevElt is NULL, then elt is the first element
		if(!prevElt)
			*list = elt->next;
		else
			prevElt->next = elt->next;
		
		_HTDisposeElt(elt);
		
		return true;
	}
	else
		return false;
}

void HTCopyElt(HTPtr table, HTPtr srcTable, ConstStr255Param name)
{
	htEltPtr srcElt = _HTFindElt(srcTable, name);
	
	if(srcElt)
		HTAdd(table, name, srcElt->data, srcElt->type);
}

char HTIsSet(HTPtr table, ConstStr255Param name)
{
	return HTFind(table, name, NULL, NULL);
}
