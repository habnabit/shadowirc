/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2004 John Bafford
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

#include <stdlib.h>

#include "CircularList.h"

typedef struct CLData {
	void* data;
	int size;
} CLData;

struct CircularList {
	CLData *data;
	int size;
	
	int head, tail;
	int view;
};

static int _CLFull(CLPtr list)
{
	//if the tail is one node less than the head
	if(list->head == 0)
	{
		if(list->tail == list->size - 1)
			return true;
	}
	else
	{
		if(list->tail == list->head - 1)
			return true;
	}
		
	return false;
}

static void CLDequeue(CLPtr list)
{
	CLData *cld = &list->data[list->head];
	
	if(cld->data)
	{
		free(cld->data);
		cld->data = 0;
		cld->size = 0;
		if(++list->head >= list->size)
			list->head = 0;
	}
}

void CLEnqueue(CLPtr list, void* item, int size)
{
	CLData *cld;
	
	if(!item || !size)
		return;
	
	if(_CLFull(list))
		CLDequeue(list);
	
	list->view = list->tail;
	cld = &list->data[list->tail++];
	
	cld->data = malloc(size);
	cld->size = size;
	memcpy(cld->data, item, size);
	
	if(list->tail >= list->size)
		list->tail = 0;
}

static void CLRetrieve(CLPtr list, int itemID, void* *data, int *size)
{
	*data = list->data[itemID].data;
	*size = list->data[itemID].size;
}

/*
	WARNING: This does not properly handle the case where the user scrolls to the end,
	and the rewinds one spot - the last item in the list doesn't get shown.
*/
void CLWalk(CLPtr list, int direction, void* *data, int *size)
{
	//In case of error, clear the data and size.
	*data = 0;
	*size = 0;
	
	if(direction == -1)
	{
		//go backwards.
		if(list->view == list->head) //if we're at the head, we can't go back further
			CLRetrieve(list, list->view, data, size);
		else //not at the head; go back
		{
			list->view--;
			if(list->view < 0)
				list->view = list->size - 1;
			
			CLRetrieve(list, list->view, data, size);
		}
	}
	else //we're going forward.
	{
		int newView = list->view + 1;
		
		if(newView >= list->size)
			newView = 0;
		
		if(newView == list->tail)
			;
		else
		{
			list->view = newView;
			
			CLRetrieve(list, list->view, data, size);
		}
	}
}

CLPtr CLNew(int size)
{
	//If we want N lines of storage, we need N+1 slots in the array
	size++;
	
	CLPtr list = malloc(sizeof(struct CircularList));
	
	list->data = calloc(sizeof(CLData), size);
	list->size = size;
	
	list->head = 0;
	list->tail = 0;
	list->view = 0;
	
	return list;
}

void CLDispose(CLPtr list)
{
	//Iterate through the data and dispose of all the data
	int x;
	for(x = 0; x < list->size; x++)
		if(list->data[x].data)
			free(list->data[x].data);
	
	free(list->data);
	free(list);
}

