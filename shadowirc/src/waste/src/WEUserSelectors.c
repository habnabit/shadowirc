/*
 *	WEUserSelectors.c
 *
 *	WASTE PROJECT
 *  Routines for hanging user-defined tagged data to a WASTE instance
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

pascal OSErr WESetUserInfo ( WESelector tag, SInt32 userInfo, WEHandle hWE )
{
	Handle hUserInfo = ( * hWE ) -> hUserInfo ;
	WEUserInfoEntry * pEntry ;
	WEUserInfoEntry entry ;
	OSErr err = noErr ;

	//	look for the specified tag in the user info array
	for (	pEntry = ( WEUserInfoEntry * ) * hUserInfo ;
			pEntry -> tag != 0 ;
			pEntry ++ )
	{
		if ( pEntry -> tag == tag )
		{
			//	tag found: save the user info and return
			pEntry -> info = userInfo ;
			goto cleanup ;
		}
	}

	//	unknown tag: add a new entry at the end of the array
	entry . tag = tag ;
	entry . info = userInfo ;
	if ( ( err = _WESplice ( hUserInfo, & entry, sizeof ( entry ), GetHandleSize ( hUserInfo ) - sizeof ( WEUserInfoEntry ) ) ) != noErr )
	{
		goto cleanup ;
	}

cleanup :
	return err ;
}

pascal OSErr WEGetUserInfo ( WESelector tag, SInt32 * userInfo, WEHandle hWE )
{
	Handle hUserInfo = ( * hWE ) -> hUserInfo ;
	WEUserInfoEntry * pEntry ;
	OSErr err = weUndefinedSelectorErr ;

	//	look for the specified tag in the user info array
	for (	pEntry = ( WEUserInfoEntry * ) * hUserInfo ;
			pEntry -> tag != 0 ;
			pEntry ++ )
	{
		if ( pEntry -> tag == tag )
		{
			//	tag found: set result code to noErr and break out of loop
			err = noErr ;
			break ;
		}
	}

	if ( userInfo != nil )
	{
		* userInfo = pEntry -> info ;
	}

	return err ;
}
