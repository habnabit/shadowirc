ShadowIRC v1.1
© 1996-2000 John Bafford
All Rights Reserved

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


Software Requirments:
	The CodeWarrior project that comes with ShadowIRC is for CodeWarrior Pro 5.3.
	ShadowIRC and the plugins are compiled against Universal Headers version 3.3.2.

ShadowIRC Carbon
	The Carbon ShadowIRC target will not compile at present. There are four issues preventing this:
	* CW Pro 5 doesn't build precompiled headers for Carbon, so the carbon stuff builds even less than it did before. CW Pro 6 fixes this, though.
	* Still using MacTCP(!!!) instead of OT
	* Scrap Manager stuff needs to be updated
	* Need to convert the floating window managment stuff to work with the new APIs in Mac OS 8.6
	* And a few miscelaneous things here and there. (Did I say "four" things?)

Once this is done, the next step in getting a working ShadowIRC carbon target is to make sure that the functions I wrote glue code for (in BlueLibPrefix.h) work the way I think they do. I'm probably wrong about something.

The #if TARGET_CARBON / #include <...> ... /#endif stuff sprinked through the code is an artifact of my previous configuration: precompiled UI 3.2 for 68k/PPC and #include'd UI 3.3.1 for the carbon target. This will go away when ShadowIRC is updated to CW Pro 6.

ShadowIRC uses the following components, included with the distribution:
	ColourBlock CDEF 1.1 (located in misc folder)
	WASTE 1.3 (modified; see note below)
	MoreFiles 1.5 (required portions only)
	SmartScroll
	Internet Config 1.4

NOTE REGARDING WASTE 1.3:
	ShadowIRC uses a *MODIFIED* version of WASTE 1.3 by Marco Piovanelli <marco.piovanelli@pobox.com>.
	The modifications are CLEARLY DOCUMENTED in the WASTE source.
	These MODIFICATIONS are licensed under the GPL and are owned by me.
	All ORIGINAL WASTE CODE that has been left unmodified is licensed under the ORIGINAL WASTE LICENSE and is owned by Marco Piovanelli.
	The original WASTE distribution can be downloaded from <http://www.boingo.com/waste/>. The ShadowIRC source distribution includes only the parts that it needs.