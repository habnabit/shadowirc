/* ���Start Header��� */

/* File:         ICTypes.h
 * Generated by: 1.0d5
 * For:          IC 1.4
 * On:           Sunday, 14 September 1997, 21:17:55
 * 
 * This file is part of the Internet Configuration system and
 * is placed in the public domain for the benefit of all.
 */

/* ���End Header��� */

/*
	IMPORTANT NOTES ABOUT THE C CONVERSION
	--------------------------------------
	
	o Pascal "var" parameters are converted from "var x : y" to "y *x".  This
	  means that when you see the parameter "y *x" you should be aware that
		you *cannot pass in nil*.  In future this restriction may be eased,
		especially for the attr parameter to ICGetPref.  Parameters where nil
		is legal are declared using the explicit pointer type, ie "yPtr x".

	o Strings are *Pascal* strings.  This means that they must be word aligned.
		MPW and Think C do this automatically.  The last time I check, Metrowerks
		C does not.  If it still doesn't, then IMHO it's a bug in their compiler
		and you should report it to them.

	o The canonical Internet Config interface is defined in Pascal.  These
		headers have not been thoroughly tested.  If there is a conflict between
		these headers and the Pascal interfaces, the Pascal should take precedence.
*/

/* ///////////////////////////////////////////////////////////////////////////////// */

#ifndef __ICTYPES__
#define __ICTYPES__

#ifndef __TYPES__
#include <Types.h>
#endif

/* ///////////////////////////////////////////////////////////////////////////////// */

/* ShadowIRC: Not needed since we set this explicitly in the target settings
#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif
*/

enum {
	icPrefNotFoundErr = -666,		/* preference not found (duh!) */
	icPermErr = -667,						/* cannot set preference */
	icPrefDataErr = -668,				/* problem with preference data */
	icInternalErr = -669,				/* hmm, this is not good */
	icTruncatedErr = -670,			/* more data was present than was returned */
	icNoMoreWritersErr = -671,	/* you cannot begin a write session because someone else is already doing it */
	icNothingToOverrideErr = -672,	/* no component for the override component to capture */
	icNoURLErr = -673,						/* no URL found */
	icConfigNotFoundErr = -674,			/* no configuration was found */
	icConfigInappropriateErr = -675	/* incorrect manufacturer code */
};

enum {
	ICattr_no_change = 0xFFFFFFFFL,				/* supply this to ICSetPref to tell it not to change the attributes */
	ICattr_locked_bit = 0,								/* bits in the preference attributes */
	ICattr_locked_mask = 0x00000001L,			/* masks for the above */
	ICattr_volatile_bit = 1,
	ICattr_volatile_mask = 0x00000002L,
	icNoUserInteraction_bit = 0,
	icNoUserInteraction_mask = 0x00000001L
};

#define ICfiletype 'ICAp'
#define ICcreator 'ICAp'

#define ICdefault_file_name "\pInternet Preferences"	/* default file name, for internal use, overridden by a component resource */

struct ICDirSpec {											/* a record that specifies a folder */
	short vRefNum;
	long dirID;
};
typedef struct ICDirSpec ICDirSpec;

typedef ICDirSpec ICDirSpecArray[4];		/* an array of the above */
typedef ICDirSpec *ICDirSpecArrayPtr;	/* a pointer to that array */

typedef long ICAttr;										/* type for preference attributes */
typedef long ICError;										/* type for error codes */
typedef Ptr ICInstance;									/* opaque type for preference reference */
enum {
	icNoPerm = 0,
	icReadOnlyPerm = 1,
	icReadWritePerm = 2
};
typedef unsigned char ICPerm;

struct ICConfigRef {
	OSType manufacturer;
	/* other private data follows */
};
typedef struct ICConfigRef ICConfigRef, *ICConfigRefPtr, **ICConfigRefHandle;

/* ShadowIRC: Not needed since we set this explicitly in the target settings
#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif
*/

#endif
