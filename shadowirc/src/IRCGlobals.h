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

#ifndef _IRCGlobals
#define _IRCGlobals

#include <Carbon/Carbon.h>

typedef char Table[256], *TableChars;

enum stdgrey {
	kStandardGrey = 57097
};

enum {
	S_CONN,
	S_OFFLINE,
	S_LOOKUP,
	S_OPENING,
	S_CLOSING
};

enum colorMethod {
	cmNone,
	cmIrcle,
	cmMIRC,
	
	cmDefaultColorMethod = 254,
	cmWindowColorMethod = 255
};

enum autoQueryOpenFlags {
	aqAll,
	aqOutgoing,
	aqIncoming
};

enum quitActionFlags {
	qaConfirm,
	qaAutoSave,
	qaQuit
};

typedef struct linkPrefsRec {
	Str63 linkName;						//The name of the link.
	Str63 nick;								//User's nickname
	Str63 alterNick;						//User's alternate nick
	Str63 user, real;						//User's username and real name.
	char autoConnect;						//Should ShadowIRC connect on launch?
	char reconnect;							//Reconnect if server connection lost?
	char displayMOTD;					//Display MOTD on connect?
	char useConnectMacro;				//Use the connect macro
	Str255 connectMacro;				//The connect macro. Commands are semicolon delimeted.
	Str255 serverName;					//Server to connect to
	Str255 serverPass;					//Password for server.
	unsigned short serverPort;		//Port of server to connect to
	char modeI;								//Go +i on connect
	char modeW;								//Go +w on connect
	char modeS;								//Go +s on connect
char unusedX;
	Str63 recentNicks[10];
	Str255 recentChannels[10];
	short onoticeMethod;
	Rect windowLoc[10];

	short maxConnAtt;
	short retryDelay;
	Str255 signoffMessage;			//The signoff message
	Str255 fingerMessage;				//CTCP finger message
	Str255 userinfoMessage;			//CTCP userinfo message
	char regainNick;
	char isTalkCity;
	char unusedC[2];
	long unused[2];
	long networkID;
	
	Str255 kickMessage;
	long spare[763];
} linkPrefsRec, *linkPrefsPtr;

enum firewallType {
	fwNone,
	fwSOCKS5,
	fwSOCKS4A,
	fwSOCKS4
};

typedef struct prefsRec {
	Str255 serverMessagePrefix;	//Prefix for server messages
	Rect consoleLoc;
	Rect inputLoc;							//Position and size of inputline.
short unusedIW;
	char consoleOpen;						//Is console window open?
	char userListOpen;
	Rect userListRect;
	Str31 defaultFontName;			//Name of default font
	short defaultFontSize;				//Size of text
	char noFontsMenu;						//If true, the fonts menu isn't displayed (or fonts parsed at launched)
	char disableColor;						//Turns off processing of Ctrl-C colors
	short colorMethod;
	char disableStyles;					//Stops processing of ctrl-b, ctrl--, ctrl-v, and ctrl-n
	char squelchBeeps;					//Don't play ctrl-G beeps
	char textIndenting;
	char ircIIDisplay;
	char timestamp;						//Timestamp all text in message windows
	char timestampSeconds;			//Include seconds in timstamps
	char timestampWhenAway;		//Only display the timestamp when away
	char escClearsInputline;			//Hitting the esc key clears the inputline
	char cursorFocus;
	char cursorFocusDontActivate;
	char showEndMessages;			//Show end of command messages from server
	char showUserHostsWithMsgs;	//Show userhosts with messages
	char beepOnPrivmsgs;				//Do a system beep on private messages. **WILL go away with sound support.
	char wallopsToConsole;				//Send all wallops to the console
	char serverNoticesToConsole;	//Send all server notices to the console
	char privmsgsToConsole;			//Sends all private messages to console
	char displayPingsInConsole;
	char autoQuery;
	char autoRejoinAfterKick;			//Auto-rejoin a channel after being kicked
	char autoRejoinReconnect;			//Auto-rejoin open channels after reconnecting to the server
	char autoacceptInvites;				//Auto-accept channel invites
	char autoacceptInvitesWhenNotAway;	//Auto-accept invites only when not away
	char optionToMoveInputLine;		//Need to use option key to move up/down in inputline buffer. (If on, up/down moves up/down in current inputline text)
	char hideSplashScreen;
	char doWhowasIfFailedWhois;
	char dontActivateNewWindowsIfInputlineText;
	char quitAction;
	char dccWindowAutoOpen;
char unused4;
	char disableCTCP;						//Prevent responses to CTCP commands
	char noCTCPUnknownErrMsg;	//Stops the sending of CTCP ERRMSGs on unknown CTCP commands
	char disableCTCPMessages;		//Stop the display of CTCP messages.
	char enableDCC;						//Turns on or off DCC
	char autoAcceptDCCChat;			//Auto-accept DCC chats
	char autoAcceptDCCChatWhenNotAway;	//Auto-accept DCC Chats only when no away
	char autoLogging;
	char autoLogQueries;						//If true, automatically logs /query windows
	char autoLogDCCChat;					//If true, automatically logs dcc chats
	char autoQueryOpen;
	char nonGlobalInput;
char unused[68];
	Str255 shortcuts[30];				//The shortcuts
char unused2[70];
	char autoDCCGet;
	char autoSaveDCC;
	Str63 socksHost;
	unsigned short socksPort;
	char firewallType;
	char displayJoin;
	char displayPart;
	char displayQuit;
	char displayModes;
	char FastDCCSends;
long unused3[4];
	char displayNicks;
	char noModesWidget;
	char ddToSameWin;
	char ctcpToConsole;
	long dccSendPacketSize;
	char displayKicks;
	char dccReverseSends;
	char dccWindowOpen;
	char displayInvites;
	Str31 socksUser, socksPass;
	Rect dccWindowRect;
	long userlistNickWidth;
	char userlistSort;
	char userlistSortReverse;
	short userlistInWindowWidth;
	char userlistInWindow;				//True if the userlist should be displayed in channel windows.
	char resendAwayOnReconnect;
	char userlistInWindowRight;		//True if the in-window userlist should be on the right side of the window.
	char userlistScrollbarLeft;		//True if the scrollbar is on the left
	long spare[960];
} prefsRec, *prefsPtr;

typedef struct prefsStruct {
	short version;
	short prefsRecSize;
	short numColors;
	short linkPrefsSize;
	
	long prefsUpdate;
	long padding[19];
} prefsStruct;

/*	prefs structure is
prefsStruct
prefsRec size = prefsRecSize
RGBColor shadowircColors[numColors]
linkPrefs[10] size = linkPrefsSize
*/

enum menuList {
	menuBar = 256,
	appleMenu = 256,
	FileMenu = 257,
	EditMenu = 258,
	windowMenu = 260,
	shortcutsMenu = 261,
	commandsMenu = 262,
	fontsMenu = 263,
	helpMenu = 264,
	
	ConnectionListMenu = 205,
	SignoffConnectionListMenu = 206,
	SelectConnectionListMenu = 207,
	AppleURLMenu = 208,
	
	mHelpWidget = 202
};

enum {
	wConsoleItem = 7,
	wFirstServiceItem = 8
};

extern int wLastServiceItem;

#define wPrevWindowItem (wLastServiceItem + 1)
#define wNextWindowItem (wLastServiceItem + 2)
#define wCascadeItem (wLastServiceItem + 3)
#define wTileItem (wLastServiceItem + 4)
#define windowsStart (wLastServiceItem + 5)

#define creatorType 'SIRC'
#define prefsType 'pref'

#define CmdChar '/'

enum {
	ircDateModifier = 2082844800
};

extern prefsPtr mainPrefs;
extern linkPrefsRec *linkPrefsArray;
extern RGBColor *shadowircColors;
extern FSSpec logFolderFSp, dccFolderFSp;

extern short gApplResFork;

extern unsigned long now, launchTime;
enum {readTimeout = 20};
extern short fontsBegin;

extern Str63 CL_VERSION, cdt, shadowIRCDefaultSignoff;
extern char debugOn;
extern char QuitRequest;
extern unsigned long idleTime;
extern char anyAway;
extern char inBackground;

extern TableChars ISOEncode, ISODecode;

extern char hasAquaMenuMgr;

extern char gTalkCityPlug;

extern MenuHandle menuConnectionList, menuSignoffConnectionList, menuSelectConnectionList;
extern MenuHandle menuHelpWidget, gHelpMenu;
extern short defaultHelpItems, normHelpMenuItems;

extern ModalFilterUPP StdDlgFilter;

extern ProcessSerialNumber gPSN;
#endif