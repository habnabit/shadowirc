#include <Carbon/Carbon.h>

//	Universal Headers #defines

#define		OLDROUTINENAMES					0
#define		OLDROUTINELOCATIONS				0
#define		CGLUESUPPORTED					0
#define		SystemSixOrLater				1
#define		SystemSevenOrLater				1
#define		SystemSevenFiveOrLater			0

//	WASTE #defines

#define		WASTE_SHARED_LIBRARY			0

#define		WASTE_DEBUG						0
#define		WASTE_OBJECTS					0
#define		WASTE_REDRAW_SPEED				0
#define 	WASTE_NO_SYNCH					0
#define 	WASTE_NO_RO_CARET				1
#define 	WASTE_IC_SUPPORT				1
#define 	WASTE_OBJECTS_ARE_GLYPHS		0
#define 	WASTE_RESOLVE_FONT_DESIGNATORS	0
#define 	WASTE_WECALTEXT_DOES_REDRAW		0
#define 	WASTE_KURTHS_OPTION_ARROWS		1
#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
  #define 	WASTE_TRANSLUCENT_DRAGS			( TARGET_API_MAC_CARBON || TARGET_CPU_PPC || ( ! TARGET_RT_MAC_CFM ) )
#else
  #define 	WASTE_TRANSLUCENT_DRAGS			( TARGET_API_MAC_CARBON || GENERATINGPOWERPC || ( ! GENERATINGCFM ) )
 #endif