/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2002 John Bafford
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

#include <Carbon/Carbon.r>

resource 'STR#' (128, "About Box Strings", locked, preload)
{
	{
		"version "
	};
};

resource 'STR#' (130, "Files", locked, preload)
{
	{
		"ShadowIRC",
		"ShadowIRC Preferences",
		"Plugins"
	};
};

resource 'STR#' (200, "Menu Strings", locked, preload)
{
	{
		"Close Window",
		"Copy Selection",
		"Copy to Inputline",
		"Clear Selection",
		"Clear All",
		"Stop Logging",
		"Copy All",
		"Cut All",
		"Color Processing",
		"Start Logging",
		"Save Selection to File",
		"Save Window Text to File",
		"Connect",
		"Disconnect",
		"Connect to ^0",
		"Disconnect from ^0",
		"Close DCC Connection",
		"None",
		"Save",
		"Save As…",
		"Cut Selection",
		"Copy Selection",
		"Clear Selection",
		"Paste",
		"Save Preferences",
		"Save Text File"
	}
};

resource 'STR#' (201, "Windows Menu Service Names", locked, preload)
{
	{
		"Userlist",
		"Faces",
		"Connections",
		"DCC Status"
	};
};

resource 'STR#' (253, "Server Messages", locked, preload)
{
	{
		"RPING reply from ^0: ^1^2 seconds",
		"Nickname ^0 is in use. Please choose another.",
		"Channel password is ^1",
		"Channel password for ^0 is ^1",
		"User mode is: ",
		"Channel URL is: ",
		"^0 is on channels ",
		"Inviting ^0 to channel ^1",
		"Topic for ^0: ",
		"^0 is already on channel ^1",
		"^0 (^1) joined ^2",
		"Mode change \"^0\" on ^1 by ^2",
		"Got PONG from ^0",
		"^0 invites you to channel ^1",
		"Received channel CTCP \"^0\" for ^2 from ^1",
		"Received CTCP \"^0\" from ^1",
		"CPING time for ^0: ^1^2 seconds",
		"CTCP ^0 reply from ^1: ^2",
		"^0 has signed off"
	};
};

resource 'STR#' (255, "SOCKS Errors", locked, preload)
{
	{
		"General SOCKS server failure.",
		"Connection not allowed by ruleset.",
		"Network unreachable.",
		"Host unreachable.",
		"Connection refused.",
		"TTL expired.",
		"Command not supported.",
		"Address not supported.",
		"Unknown SOCKS error.",
		"SOCKS Error: ^0",
		"Authentication failed.",
		"Server did not like authentication methods.",
		"Request rejected or failed.",
		"Request rejected because SOCKS server cannot connect to identd.",
		"Request rejected because identd failed."
	};
};

resource 'STR#' (256, "Error Strings", locked, preload)
{
	{
		"You are not connected to a server",
		"You are not talking to anybody",
		"Search failed",
		"DNS Name search failed",
		"Unable to open connection",
		"Closing Connection",
		"Connection Closed",
		"The ShadowIRC Preferences file in the preferences folder is old or damaged. Please remove it and relaunch ShadowIRC.",
		"ShadowIRC does not recognize the format of the ShadowIRC preferences file in the preferences folder. It may be too old or too new. Please remove it and relaunch ShadowIRC.",
		"The preferences file is from ShadowIRC 1.2 or later, and can not be used with this version of ShadowIRC.",
		"Changing Servers",
		"Registering with server...",
		"^0 refuses connection",
		"Opening Identd",
		"Identd called",
		"No server specified in Servers window",
		"Invalid server port specified",
		"No nickname specified in Connection Settings",
		"No username specified in Connection Settings",
		"No real name specified in Connection Settings",
		"The logs folder is invalid. Check your settings. Auto-logging is temporarally disabled.",
		"Error loading preferences.",
		"Preferences Error.",
		"You can not use a colon in the link name.",
		"ERROR: Unable to create plugin preferences folder!",
		"ERROR: Plugins preferences folder is a file!",
		"ERROR: No Plugins Folder",
		"Plugins folder is empty",
		"Registration failed, trying again...",
		"ERROR: Plugins folder is a file!",
		"ERROR: Can't create log folder for this connection!",
		"ERROR: The log folder for this connection is a file!",
		"ShadowIRC is running low on memory.",
		"Please close some windows or extra IRC connections to free up memory.",
		"ShadowIRC requires System 8.1 or later to run.",
		"There is no topic set.",
		"You do not have ops, so you can not change the topic.",
		"WARNING: ShadowIRC is running low on memory.",
		"You must give this connection a name.",
		"Identd closed",
		"Unable to load one or more plugins: not enough memory.",
		"DNS ^0: Can't lookup yet.",
		"DNS ^0: Not in userlist; can't lookup.",
		"ShadowIRC ran out of memory.",
		"^0 is not on channel ^1",
		"The server thinks your nick is something other than what I think it is. Check your settings."
	};
};

resource 'STR#' (257, "Informative Strings", locked, preload)
{
	{
		"Looking up IP for ^0.",
		"IP for ^0 is ^1.",
		"Ignore List: ",
		"You are not ignoring anyone.",
		"You are already ignoring ^0",
		"Ignoring ^0 ^1",
		"mask.",
		"masks.",
		"The /onotice command can only be used in a channel.",
		"Can't make query window. Illegal character in nickname.",
		"^0: Added to silence list",
		"^0: Removed from silence list",
		"You already have ^0 on your notify list.",
		"Notify List: ",
		"You do not have anyone on your notify list.",
		"You have ^0 users on your notify list.",
		"Signed on: ",
		"Signed off: ",
		" (Offline)",
		" (lookup)",
		" (opening)",
		" (closing)",
		" (Away)",
		"(no topic set)",
		"Topic: ",
		" set by ",
		"Inactive Channel",
		"Closing log file for ^0 at ^1^2",
		"Opening log file for ^0 at ^1^2",
		"Channel created on ^0 at ^1",
		"Topic set by ^0 on ^1 at ^2",
		"Recent Channels",
		"Inputline",
		"(nobody)",
		"Time Delta: ^0",
		"^0 set by ^1 on ^2 @ ^3", //^b around set by and on.
		"No bans for ^0",
		"No exceptions for ^0",
		"Listed ^2 bans for ^0",
		"Listed ^2 exceptions for ^0",
		"No bans for ^0 matching \"^1\"",
		"No exceptions for ^0 matching \"^1\"",
		"Listed ^2 bans for ^0 matching \"^1\"",
		"Listed ^2 exceptions for ^0 matching \"^1\"",
		"Resetting away message (^0)",
		"^0 has set the topic on ^1 to ",
		"^0 is now known as ^1",
		"You have been rejected by ^0: ",
		"^0 was kicked off ^1 by ^2",
		"Length: ^0; Maximum Length: ^1",
		"Time for ^0: ^1 (^2)"
	};
};

resource 'STR#' (258, "DCC Strings", locked, preload)
{
	{
		"No DCC connections are open.",
		"Listed ^0 DCC connections.",
		"Couldn't find DCC ^0 for ^1.",
		"Syntax: /DCC CLOSE <CHAT|GET|SEND> <nick>",
		"Syntax: /DCC CHAT nickname",
		"Syntax: /DCC GET nickname",
		"Syntax: /DCC SEND nickname",
		"^0: No such DCC GET nickname.",
		"Requesting DCC ^0 connection with ^1.",
		"Opening DCC connection to ^0 (^1:^2).",
		"Closed DCC ^0 connection to ^1.",
		"Failed to create.",
		"Lost (send error)",
		"Lost (file read error)",
		"Lost (Couldn't create file)",
		"Lost (acknowledge error)",
		"Lost (file write error)",
		"Lost (recieve error)",
		"Failed to open",
		"You can't DCC a channel.",
		"Select a location to save to:",
		"DCC ^0 request from ^1 ^2 (^3)",
		"DCC Resume requested from ^0 (^1) at ^2 bytes",
		"DCC Resume failed: tried to reposition past end of file",
		"DCC Resume failed: could not find DCC to resume",
		"DCC collision for ^0 request from ^1",
		"DCC SENDREV error: ^0 asked for a send which does not exist.",
		"Requesting reverse DCC ^0 connection with ^1.",
		"DCC reverse ^0 request from ^1 ^2",
		"Unsupported DCC ^0 request from ^1",
		"Unsupported reverse DCC ^0 request from ^1",
		"You are receiving a DCC file request from ^0 (\"^1\").",
		"You are receiving a DCC file request from ^0 for a file that currently exists (\"^1\"). Please choose ShadowIRC from the Applications menu and save the file under a different name.",
		"DCC ^0 request from ^1. Ignoring."
	};
};

resource 'STR#' (260, "Topic Widget", locked, preload)
{
	{
		"Console",
		"Help: ",
		"Query",
		"DCC Chat",
		"Inactive DCC Chat",
		"(modeless channel)"
	};
};

resource 'STR#' (261, "URL Menu", locked, preload)
{
	{
		"ShadowIRC Home Page", "http://www.shadowirc.com/",
		"SourceForge Project Page", "http://sourceforge.net/projects/shadowirc/",
//		"Comments and Suggestions", "http://www.shadowirc.com/comment.phtml",
		"ShadowIRC Support", "http://www.shadowirc.com/support.phtml",
		"ShadowIRC Plugins", "http://plugins.shadowirc.com/",
		"-", "-",
		"GNU GPL License", "http://www.gnu.org/copyleft/gpl.html",
		"-", "-",
		"Email ShadowIRC's Author", "mailto:dshadow@shadowirc.com"
	};
};

resource 'STR#' (262, "File Dialog Strings", locked, preload)
{
	{
		"Please select a color set.",
		"Please select a location to save this color set.",
		"Please choose a folder.",
		"Send"
	};
};


resource 'STR#' (263, "WHOIS Strings", locked, preload)
{
	{
		"^0 signed on at ^1, ^2",
		"^0 is on IRC via ^1 (^2)",
		"^0 was on IRC via ^1 (^2)",
		"^0 has been idle for ^1"
	};
};

resource 'STR#' (259, "Help Menu Items", locked, preload)
{
	{
		"Credits",
		"Command Reference",
		"Colors",
		"Manual"
	};
};
