/*
 	File:		SmartScrollAPI.h
 
 	Contains:	Smart Scroll Application Programming Interface
 
 	Version:	1.4
 
 	Copyright:	© 1996, 1997 by Marc Moini, portions by Marc Menschenfreund,
				Alessandro Levi Montalcini, Marco Piovanelli and Mark Shirley (Thanks!)
 				All rights reserved.
 
 	Bugs?:		If you find a problem with this file, please email Marc@Kagi.com
 
 	Version
	History:	1.4		24sep97	fixes by Marco Piovanelli
				1.3		21feb97	Same as 1.2, except a couple of "ControlHandle" definitions changed to "ControlRef"
*/

#ifndef __SMARTSCROLLAPI__
#define __SMARTSCROLLAPI__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern pascal void SetSmartScrollInfo (ControlRef theScrollBar, long amountVisible , long amountTotal);
extern pascal void SetSmartScrollProp (ControlRef theScrollBar, Fract proportion);
extern pascal Fract GetSmartScrollProp (ControlRef theScrollBar);
extern pascal void DisposeAllSmartScrolls (void);


#ifdef __cplusplus
}
#endif

#endif	/* __SMARTSCROLLAPI__ */
