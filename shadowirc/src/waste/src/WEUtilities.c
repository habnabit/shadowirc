/*
 *	WEUtilities.c
 *
 *	WASTE PROJECT
 *  Miscellaneous General-Purpose Utilities
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

pascal Boolean _WEBlockCmp ( const void * block1, const void * block2, Size blockSize )
{
	const UInt8 * p1 = ( const UInt8 * ) block1 ;
	const UInt8 * p2 = ( const UInt8 * ) block2 ;

	for ( ; blockSize > 0 ; blockSize -- )
	{
		if ( * p1 ++ != * p2 ++ ) return false ;
	}

	return true ;
}

pascal void _WEBlockClr ( void * block, Size blockSize )
{
	UInt8 * p = ( UInt8 * ) block ;

	if ( blockSize >= 16 )
	{
		//	advance block pointer to next even address
		if ( ( UInt32 ) p & 1 )
		{
			* p ++ = 0 ;
			blockSize -- ;
		}

		//	advance block pointer to next address divisible by four
		if ( ( UInt32 ) p & 2 )
		{
			* ( UInt16 * ) p = 0 ;
			p += 2 ;
			blockSize -= 2 ;
		}

		//	clear 16-byte blocks
		for ( ; blockSize >= 16 ; blockSize -= 16 )
		{
			* ( ( UInt32 * ) p     ) = 0 ;
			* ( ( UInt32 * ) p + 1 ) = 0 ;
			* ( ( UInt32 * ) p + 2 ) = 0 ;
			* ( ( UInt32 * ) p + 3 ) = 0 ;
			p += 16 ;
		}
	}

	//	clear remaining bytes
	for ( ; blockSize > 0 ; blockSize -- )
	{
		* p ++ = 0 ;
	}
}

pascal void _WEForgetHandle ( Handle * h )
{
	Handle theHandle ;

	if ( ( theHandle = * h ) != nil )
	{
		* h = nil ;
		DisposeHandle ( theHandle ) ;
	}
}

pascal Boolean _WESetHandleLock ( Handle h, Boolean lock )
{
	Boolean oldLock = ( HGetState ( h ) & ( 1 << 7 ) ) != 0 ;

	if ( lock != oldLock )
	{
		if ( lock )
		{
			HLock ( h ) ;
		}
		else
		{
			HUnlock ( h ) ;
		}
	}

	return oldLock ;
}

pascal void _WEReorder ( SInt32 * a, SInt32 * b )
{
	if ( * a > * b )
	{
		SInt32 temp = * a ;
		* a = * b ;
		* b = temp ;
	}
}

pascal OSErr _WEAllocate ( Size blockSize, UInt32 allocFlags, Handle * h )
{
	// Allocate a new relocatable block.
	// AllocFlags may specify whether the block should be cleared and whether
	// temporary memory should be used.

	Handle theHandle = nil ;
	OSErr err ;

	// if kAllocTemp is specified, try tapping temporary memory
	if ( allocFlags & kAllocTemp )
	{
		theHandle = TempNewHandle ( blockSize, & err ) ;
	}

	// if kAllocTemp isn't specified, or TempNewHandle failed, try with current heap
	if ( theHandle == nil )
	{
		theHandle = NewHandle ( blockSize ) ;
		err = MemError ( ) ;
	}

	// if kAllocClear is specified, zero the block
	if ( ( allocFlags & kAllocClear ) && ( theHandle != nil ) )
	{
		_WEBlockClr ( * theHandle, blockSize ) ;
	}

	* h = theHandle ;
	return err ;
}

pascal OSErr _WESplice ( Handle h, const void * blockPtr, SInt32 blockSize, SInt32 offset )
{
	SInt32 oldSize, newSize ;
	OSErr err ;

	//	get handle size
	oldSize = GetHandleSize ( h ) ;

	//	calculate new size
	newSize = oldSize + blockSize ;

	if ( blockSize < 0 )
	{
		//	negative blockSize means we must remove a portion of the block
		//	offset == -1 means remove trailing portion
		if ( offset == -1 )
		{
			offset = newSize ;
		}

		//	sanity checks
		err = paramErr ;
		if ( ( offset < 0 ) || ( offset > newSize ) )
		{
			goto cleanup ;
		}

		//	compact the handle
		BlockMoveData ( * h + ( offset - blockSize ), * h + offset, newSize - offset ) ;
	}

	//	resize the handle
	SetHandleSize ( h, newSize ) ;
	if ( ( err = MemError ( ) ) != noErr )
	{
		goto cleanup ;
	}

	if ( blockSize > 0 )
	{
		//	positive blockSize means make room within the handle
		//	offset == -1 means append new portion at the end
		if ( offset == -1 )
		{
			offset = oldSize ;
		}

		//	sanity checks
		err = paramErr ;
		if ( ( offset < 0 ) || ( offset > oldSize ) )
		{
			goto cleanup ;
		}

		//	make some room at offset
		BlockMoveData ( * h + offset, * h + offset + blockSize, oldSize - offset ) ;

		//	insert blockPtr into gap (unless blockPtr == nil)
		if ( blockPtr != nil )
		{
			BlockMoveData ( blockPtr, * h + offset, blockSize ) ;
		}
	}

	//	clear result code
	err = noErr ;

cleanup :
	return err ;
}
