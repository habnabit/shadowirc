/*	ShadowIRC Plugins Header File
		Version 2.0-preliminary
		й John Bafford 1997-2002. All Rights Reserved.
		dshadow@shadowirc.com
		http://www.shadowirc.com
		
		It is not advisable to use these header files with any other version of ShadowIRC.

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



#ifndef __ShadowIRC_Headers__
#define __ShadowIRC_Headers__

#define _ShadowIRC_API_Version_ 0x02000006

#include <Carbon/Carbon.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#pragma mark еее ShadowIRC Message Definitions
/*	------------------------------------------------------------------------------------------
		ShadowIRC Message Definitions
		
		Unless otherwise stated, all messages have a pointer to data in sidr->messageData. See below for
		the definitions of those structs.
		------------------------------------------------------------------------------------------
*/
enum messagesList
{
	numMessages		=41,					//To set the size of CMArray...
// ShadowIRC plugin messages
	pVersionCheckMessage			=-1,	//You recieve this message before pInitMessage. Respond to it with pVersionCheckMessageReply.
	pInitMessage							=0,	//Init time message. You always get this. This is the first message your plugin receives.
	pQuitMessage						=1,	//Exit time message. You always get this. This is the last message your plugin receives.
	pUserCommandMessage			=2,	//User entered a command. You get this message before ShadowIRC checks it.
	pInputMessage						=4,	//User hit return after inputting text.
	pKeyDownMessage				=5,	//User pressed a key.
	pNumericCommandMessage	=6,	//Numeric server command.
	pServerJOINMessage			=7,	//JOIN message from the server.
	pServerPARTMessage			=8,	//PART message from the server.
	pServerQUITMessage			=9,	//QUIT message from server. NOTE: Since you wouldn't get a QUIT message when you sign off, it's faked for your signoffs.
	pServerTOPICMessage			=10,	//TOPIC message from server.
	pServerNICKMessage			=11,	//NICK message from server.
	pServerPINGMessage			=12,	//PING message from server.
	pServerPONGMessage			=13,	//PONG message from server.
	pServerKICKMessage			=14,	//KICK message form server.
	pServerINVITEMessage			=15,	//INVITE message form server.
	pServerWALLOPSMessage		=16,	//WALLOPS message form server.
	pServerERRORMessage			=17,	//ERROR message form server.
	pServerNOTICEMessage			=18,	//NOTICE message form server.
	pServerPRIVMSGMessage		=19,	//PRIVMSG message form server. (Channel and private messages.)
	pCTCPMessage						=20,	//Message for CTCP requests or replies.
	pDCCIncomingChatMessage	=21,	//Incoming DCC data.
	pMWTextMessage					=22, //Message window text
	pContextSwitchMessage		=23,	//Sent if ShadowIRC is switched from fg to bg, or bg to fg.
	pSavePreferencesMessage	=24,	//Sent to your plugin when the user saves the prefs. You should save yours also.
	pServerRPONGMessage			=25,	//RPONG message from server. (UnderNet opers only)
	pServerSILENCEMessage		=26,	//SILENCE message reply for /silence command
	pServerMODEMessage			=27,	//MODE message from server
	pMWNewMessage					=28, //Sent after a new message window is created
	pMWDestroyMessage				=29, //Sent before a message window is destroyed.
	pDCCCreateMessage				=30,	//A DCC connection was created
	pDCCOpenedMessage				=31,	//A DCC connection was opened
	pDCCClosedMessage				=32,	//A DCC connection was closed
	pInputTextDisplayMessage		=33,	//Allows your plugin to modify text displayed in the window after the user hits return
	pNOTIFYMessage					=34,	//Allows your plugin to watch notify events
	pDCCConnOpenedMessage		=35,	//A DCC connection has been established.
	pDCCChatCookieMessage		=36,	//A DCC chat cookie was received
	pCMPopupsMessage				=37, //Contextual menus message
	pShortcutProcessMessage		=38,	//Process shortcuts for $tokens.
	pPWClosedMessage				=39,	//Preferences window closed
	pPWOpenedMessage				=40, //Preferences window opened (no messageData)


/*	<blink><b><font size=100>
		Messages below here are recieved because your plugin had to do something to get them. They do not have
		values in sidr->yourInfo->captureMessage[]. DO NOT ATTEMPT TO TURN THEM ON. You WILL mess
		something up if you do.
		You don't need to set sidr->completelyProcessed, since you are the only plugin recieving these messages.
		</font></b></blink>
*/

//Here, "window" means a WindowPtr, unless stated.
	pUIWindowCloseMessage			=51,		//User attempted to close one of your windows.
	pMWWindowMoveMessage			=52,		//User moved one of your message windows.
	pUIMWGotTextMessage				=54,		//User typed text to one of your plugin's message windows
	pUIActivateMessage					=55,		//A window has become active or deactive.
	pUIMouseUpDownMessage			=56,		//A user clicked in your window somewhere.
	pUIMWInfoRefresh						=252,	//ShadowIRC is refreshing your Message Window's status bar. Tell it what you want it to do.

//Drag and Drop
	pUIDragTrackMessage				=253,	//Drag and Drop drag tracking handler
	pUIDragReceiveMessage			=254,	//Drag and Drop drag receive handler
	pMWPaneDragTrackMessage		=255,	//MW Pane D&D drag tracking message
	pMWWidgetDragTrackMessage	=256,	//MW Widget D&D drag tracking message
	pMWPaneDragReceiveMessage	=257,	//MW Pane D&D drag receive message
	pMWWidgetDragReceiveMessage=258,	//MW Widget D&D drag receive message
	
//Prefs Window
	pPWActivateMessage				=60,		//Activate event for a prefs pane set up by you. (Return the id of your DITL)
	pPWSetWindowMessage				=61,		//Set Data event for a prefs pane set up by you. (Set the value of the items in the window)
	pPWGetWindowMessage				=62,		//Get Data event for a prefs pane set up by you. (Read your items; your prefs window is going away)
	pPWItemHitMessage					=63,		//Item hit in one of your prefs panes.
	pPWRedrawMessage					=65,		//You need to redraw non-controls in your pane.

//Connections
	pDNSLookupMessage					=70,		//Results from a DNSLookup() with saveReply != 0.
	pDCCStatusMessage					=75,		//ShadowIRC or a plugin is requesting the status of a DCC.
	pTextConnectionMessage			=90,			//Plugin-created connection message. *text* connections only
	pDataConnectionMessage			=91,			//Plugin-created connection message. *data* connections only

//Inputline Widgets
	pIWUpdateMessage						=141,		//Status line update message
	pIWClickMessage							=142,		//Status line click message

//Message Window Widgets
	pMWWidgetUpdateMessage				=143,		//You are being asked to update your widget
	pMWWidgetClickMessage				=144,		//User lcicked on your message window widget
	pMWWidgetDestroyMesage			=145,		//Your message window widget is being destroyed. Perform cleanup first.

//Message Window Panes
	pMWPaneDestroyMessage				=146,		//Your panel is being destroyed. Perform cleanup.
	pMWPaneActivateMessage				=147,		//The message window was activated or deacvitated. Perform necessary tasks.
	pMWPaneClickMessage					=148,		//Your panel has been clicked.
	pMWPaneUpdateMessage				=149,		//The window is being updated. Draw your panel.
	pMWPaneResizeMessage				=150,		//The window was resized, or a pane was deleted and a plugin forced a resize. You only get this if the window is visible.

//Misc
	pCMPopupsReturnMessage			=259,	//User has selected one of your items from a contextual menu managed by ShadowIRC.
	pHelpMenuMessage					=64,		//User selected something from the help menu that your plugin put in.
	pAsyncSoundCompletionMessage=80,	//Completion message from AsyncSoundPlay(). The sound has finished playing.

/* Services messages
*/

//General service messages - all services receive these
	pServiceActivateWin				=101,		//Activation/deactivation of a window. You will sometimes get this twice in a row for the same window.
	pServiceWindowMenu			=114,		//User selected your service from the window menu. Act as appropriate.

//These messages apply only to a userlist service or a faces service.
	pServiceULUserhosts			=111,		//ShadowIRC received a userhosts message and has parsed it.
	pServiceULNamesEnd				=112,		//ShadowIRC received the end of the first /names message.
	pServiceULTrashChannel		=115,		//The userlist of a channel is being trashed. Walk its userlist and do any memory disposals you have to.
	pServiceULNickChange			=116,		//A user has changed their nick. You receive the *pServerNICKMessage* *AFTER* the channel's userlist has been updated.

//These messages are only for a connection window service
	pServiceCWLinkStateChange	=131,		//The connection state of a link has changed.

//This is the sound service message
	pSoundServiceMessage			=161,		//Play a sound.

//This is for the user registration service
	pServiceUserRegMessage		=500,

/*	Inter-Plugin Communication messages
*/
	pIPCMessage							=300,		//Your plugin is beging called with an IPC message
	pIPCReplyMessage					=301		//Your plugin is being called by an IPC service
};

/*	Services List
		The list of valid service types. Register with the functions in the plugin services section.
		If you need a service type that is not listed here, please let me know.
*/
enum servicesList {
	genericServiceType					='****',	//Generic service. Use this to receive generic service messages.
	userlistServiceType					='ulst',		//Userlist
	facesServiceType						='face',		//Faces
	connectionWindowServiceType	='conw',	//Connection window
	dccWindowServiceType				='dccw'	,	//DCC window
	soundServiceType						='sond',	//Sound service
	userRegistrationType				='ureg'		//User Registration to the server
};

/*	Sound service sound types
		The sound service uses these types to indicate what sound should be played.
*/
enum soundServiceSounds {
	sndServerPing		='sPNG',
	sndNotice				='NOTC',
	sndChanMsg			='cMSG',
	sndPrivMsg			='pMSG',
	sndMiscMsg			='?MSG',
	sndJoin					='JOIN',
	sndPart					='PART',
	sndQuit					='QUIT',
	sndTopic				='TOPC',
	sndNick					='NICK',
	sndKill					='KILL',
	sndMode				='MODE',
	sndUMode				='UMOD',
	sndKick					='KICK',
	sndInvite				='INVT',
	sndRPong				='rPNG',
	sndSilence				='SILN',
	sndWallops			='WALL',
	sndGotPing			='cPNG',
	sndCPing				='CPNG',
	sndNotify				='NOTY',
	sndUserHitReturn	='RETN',
	sndDCCChat			='DCcS',
	sndDCCChatOpen	='DCcO',
	sndDCCChatClose	='DCcC',
	sndDCCFileOpen		='DCfO',
	sndDCCFileClose	='DCfC'
};

/* plugsPtr
*/
typedef char CMArray[numMessages];
typedef struct plugsRec {
	const ProcPtr yourPlugin;			//Your main function.
	long pluginRef;							//A reference to your plugin.
	CFStringRef pluginName;			//The name of your plugin's file
	const long timesCalled;				//The number of times your plugin has been called.
	const short resFileRefNum;		//The File Manager refnum of your resource fork.
	short idleThreshold;					//If you're recieving idle messages, how often to get them (minimally), in ticks.
	CMArray captureMessages;		//The messages your plugin listens for.
} plugsRec, *plugsPtr;


/*	The LongString
*/
enum {
	maxLSlen = 600 //maximum length of a LongString.
};

typedef struct LongString {
	short len;											//Length of a longstring. Functions as a C-string strlen() or a pascal string length byte would.
	unsigned char data[maxLSlen + 1];	//The string. data[1] is the _first_ character of the string. data[0] has no specific value.
} LongString;

/*	Forward type definitions
*/
typedef struct Link Link, *linkPtr;
typedef struct Channel Channel, *channelPtr;
typedef struct dccRec dccRec, *dccPtr;

#pragma mark еее Message Windows
/*	Message Window types
*/
typedef struct MessageWindow MessageWindow, *MWPtr;

enum mwTypes {
	conWin=0,		//Console
	chanWin,		//Channel
	queryWin,		//query
	pluginWin,		//window generated from a plugin
	dccWin,			//dcc chat window
};

enum mwDefaultWidgets
{
	mwShadowIRCWidget = 'SIRC',	//ShadowIRC created the widget
	mwPluginWidget = 'PLUG',			//A plugin created the widget
	
	mwTopicWidget = 'topc',			//Widget is the topic area
	mwModesWidget = 'mode'			//Widget is mode switches
};

enum mwWidgetAlign {
	mwAlignLeft = 0,			//display before the topic
	mwAlignRight = 1,		//display at the right edge of the window
	mwPlaceholder = 255
};

typedef struct mwWidgetRec mwWidgetRec, *mwWidgetPtr;
struct mwWidgetRec {
	const mwWidgetPtr next;		//Next widget
	const mwWidgetPtr prev;		//Previous widget
	const MWPtr mw;					//The message window for this widget.
	const long reserved;				//Don't touch
	const long type;					//Type you passed to MWNewWidget()
	long data;								//Data area for you
	const Rect drawArea;			//The rect you are permitted to draw in
	short askedWidth;					//The width of space you asked for.
	const short givenWidth;			//The width of space you were given.
};

enum mwPaneAlign {
	mwPaneTop,				//Align along the top of the window
	mwPaneBottom,		//Align along the bottom of the window
	mwPaneLeft,				//Align along the left side of the window
	mwPaneRight			//Align along the right side of the window
};


typedef struct mwPaneRec mwPaneRec, *mwPanePtr;
struct mwPaneRec {
	const mwPanePtr next;			//The previous pane for this window
	const mwPanePtr prev;			//The next pane for this window
	const MWPtr mw;					//The message window
	const long reserved;
	const long type;								
	long data;
	const Rect drawArea;
	short askedHeight, askedWidth;
	const short givenHeight, givenWidth;
};

/*	Message Window record
*/
struct MessageWindow {
	const OSType magic;						//'MWIN'. DO NOT EVER CHANGE THIS.
	const WindowPtr w;						//The window.
	const short unused;
	const short winType;					//The type of window.
	const ControlHandle vscr;				//The vertical scroll bar.
	const Ptr we;								//A WASTE WEReference to the text.
	const short vislines;						//Number of visible lines in the text field
	const short scrpHeight;					//height of one line in the text field
	const short font,size;					//Font number and size
	const Handle sty;							//Style template
	short logRefNum;							//The log file to write to. if zero, not logging.
	short colorMethod;						//The color style for this window.
	const linkPtr link;							//The link for this window, or nil.
	const mwWidgetPtr widgetList;		//The widget list for this window.
	const mwPanePtr paneList;			//The pane list for this window.
	long refCon;									//Reference constant for the MessageWindow.
	const MWPtr next, prev;				//The next and previous message windows.
	const plugsRec*const pluginRef;	//A reference to the plugin that created this window. Nil if no plugin.
};

//Constants used by ShadowIRC for Navigation Services' preferenceKey.
enum navPrefKey { 
	kNavGetFile,	
	kNavGetFolder,
	kNavPutFile,
	kNavGetDCCFile
};

#pragma mark еее Connections
/*		------------------------------------------------------------------------------------------
			Connections
			
			Data types for connections.
			------------------------------------------------------------------------------------------
*/

enum connTypeRec {
	connNIL,				//no connection
	connIRC,				//IRC server connection
	connIDENTD,
	connDCC,				//DCC connection
	connDNSIP,			//DNS name -> IP lookup
	connDNSNAME,		//DNS ip -> name lookup
	connPLUGIN,			//Text connection owned by a plugin
	connSOCKS			//a connection to a SOCKS server before SOCKS completes the connection 
};

enum dccTypes {dccNIL, dccCHAT, dccGET, dccSEND, dccRESUME, dccREVERSE};
enum dccFlags {closed, waiting, offered, opening, open, waitingRev};
enum dccSendStage {sendNIL, sendHeader, sendDF, sendRF, sendDone};

typedef struct dccCHATData {
	MWPtr dccWindow;						//The Message Window for this DCC
	long remoteShadowIRCVersion;		//The version of the remote ShadowIRC, as reported by ShadowIRCVersion()
	char remoteIsShadowIRC;				//True if the remote client is ShadowIRC >= 0.7.1
	char sentReplyCookie;					//
	char closeChatWhenConnDies;		//Automatically close the message window when the chat closes.
} dccCHATData, *dccCHATDataPtr;

typedef struct dccGETData {
	FSSpec gfile;					//FSSpec to the file
	short gfref;					//The file's reference number
	long gotten;					//How much of the file we've gotten
	long getmax;					//File size, or -1 if we don't know.
	short getpacketlen;		//Packet size, or -1 if we don't know.
	long padLong;				
	Ptr macB;						//Ptr to a MacBinary header, or nil if this isn't a MacBinary file (or we haven't gotten the header yet)
	short pad;						
	char inRF;						//Receiving the resource fork now
} dccGETData, *dccGETDataPtr;

typedef struct dccSENDData {
	FSSpec sfile;					//FSSpec to the file
	short sfref;					//The file's reference number
	long sent;						//How much of the file we've sent
	long sendmax;				//File size
	long sendpacketlen;		//Packet size
	long dfSize;					//If stage == sendRF, the size of the data fork + padding + MB Header (if it's a MB send)
	Ptr macB;						//Ptr to a MacBinary header, or nil if this isn't a MacBinary send.
	short stage;					//enum dccSendStage
	char MBSend;				//true if MacBinary send, false otherwise.
	char pad;
	long acked;					//The amount of data the other side has acknowledged.
} dccSENDData, *dccSENDDataPtr;

struct dccRec {
	short dccType;				//The type of dcc. (A dccTypeRec)
	short dccFlags;				//DCC connection flags. (A dccFlagsRec)
	Ptr DCCInputFunc;
	Str255 dccUserName;	//The name of the user we're chatting with
	Ptr dccData;					//Pointer to dcc data. If the dcc is a plugin dcc type, this is free for your own use.
	long timeOpened;			//The time the connection was opened
	char failedDCC;				//True if the reason the dcc is being closed is because it failed.
};

typedef struct dccStatus {
	Str255 status;							//String representation of the status
	long progress, progressMax;		//Current/maximum progress. Set both to -1 if not applicable.
} dccStatus, *dccStatusPtr;

enum connectStage {
	csNil = 0,								//No connection in progress
	csStartingToConnect,			//Starting to connect
	csLookingUp,							//Looking up IP
	csOpeningConnection,			//Trying to open the connection (found IP)
	csConnected,							//Data transfer is now possible
	csSentRegistration,				//Send USER/NICK/PASS
	csOnline,								//Server has sent numeric 001 - we are online
	
	csSOCKSNegotiatingMethod,		//Negotiating an authentication method with SOCKS server
	csSOCKSSendingAuthRequest,	//Attempting authentication with SOCKS server
	csSOCKSSendingRequest,			//Sending connection request to SOCKS server

	csFailedToLookup = -1,			//IP lookup failed
	csFailedToConnect = -3,		//connect failed
	csSOCKSAuthFailed = -4,		//SOCKS authentication failed
	csSOCKSConnectFailed = -5	//SOCKS connection failed
};

enum lineTermList {
	eolLF,
	eolCR,
	eolCRLF
};

/*	Connection Record
*/
typedef struct Connection Connection, *connectionPtr;
struct Connection {
	const connectionPtr next;			//Pointer to next connection
	const long socket;						//The connection number
	const short connType;				//Type of connection
	unsigned short port;				//The port we're connecting on
	
	const dccPtr dcc;						//Ptr to DCC information, if this is a DCC, else nil.
	void* refCon;							//Reference constant.
	const linkPtr link;						//The link for this connection, or nil.
	
	struct in_addr ip, localip;
	Str255 name;							//name of connection
	long lastData;							//The time the last data came in.

	char textOrBinary;					//True if text, false if not. Determines how it's processed on input and output.
	char lineTerm;							//See enum lineTermList. Valid only for ConnPutLS().
};

/*	Notify List Record
*/

typedef struct notifyRec notifyRec, *notifyPtr;
struct notifyRec {
	const notifyPtr next, back;			//Next and previous notify entries.
	const Str63 nick;							//The nickname
	const char waitingForResponse;		//True if we sent out an /ison
	const char signedon;						//True if the user is signed on, false if not.
	const char changed;						//True if we need to inform the user
};

enum connectionStatus {
	C_NoEvent,					// No network event.
	C_Found,						// Name -> IP search successful
	C_SearchFailed,			// Name -> IP search failed
	C_Established,				// Connection established.
	C_FailedToOpen,			// 
	C_Closed,						// Connection closed.
	C_CharsAvailable			// Data is available.
};

enum linkStatus {
	S_CONN,		//Connected
	S_OFFLINE,	//Offline
	S_LOOKUP,	//Lookup in progress
	S_OPENING,	//Opening the connection
	S_CLOSING	//Closing the connection
};

/*	Link Record
*/
struct Link {
	connectionPtr conn;					//The IRC connection for this link, or nil
	connectionPtr identConn;
	long connectedTime;					//The time the connection was established, in seconds.
	
	short serverStatus;					//Server status. see enum linkStatus
	short connectingNickAttempt;	//A number ShadowIRC uses to set the nickname in case the nickname is taken
	char neverConnected;				//
	char UserRegistered;				//True if we have sent the NICK and USER commands.
	char gotMOTD;							//True if we have recieved any of the MOTD
	char finishedMOTD;					//True if we have recieved the end of the MOTD
	char alreadyFinished;				//
	char isAway;							//True if away
	short lastNumerics[10];			//The last ten numeric commands for this link. lastNumerics[0] == most rcent
	short linkNum;							//The link number.
	Str63 CurrentNick;					//The current nickname
	Str255 CurrentServer;				//The name of the server
	Str255 serverTryingName;		//The name of the server to try when we (re)connect
	short serverTryingPort;			//The port to try when we (re)connect
	short numOutstandingISON;		//Number of ISONs to wait for before displaying notify results.
	notifyPtr notifyList;					//The notify list for this link.
	Ptr linkPrefs;							//Ptr to the preferences for this link. (linkPrefsPtr; typecast yourself)
	MWPtr windows[10];				//The first ten windows for this link.
	long lastConnAttempt;				//Time of last connection attempt
	short connectionAttempts;		//Number of (unsuccessful) connetion attempts
	char waitingToRetry;				//True if we're waiting to retry the connection.
	char needUserHosts;					//True if we're in the process of getting needed userhost queries.
	StringPtr umodes;						//The possible usermodes as reported by the server
	StringPtr yourUmodes;				//The usermodes you have. Character matching umode is 0 if down, the umode if up.
	StringPtr chModes;					//The possible channel modes as reported by the server
	StringPtr awayString;				//The away message, or nil if none. DO NOT modify this string.
	channelPtr channelList;				//The channel list for this link.
	
	linkPtr next, prev;					//Next/previous link.

	short outstandingUSERHOST;		//The number of outstanding USERHOST queries
	short connectStage;
};


#pragma mark еее Channels
/*		------------------------------------------------------------------------------------------
			Channel Records
			------------------------------------------------------------------------------------------
*/

typedef struct bansRec {	//A linked list of bans.
	struct bansRec *const next;	//The next ban in the list. If this is the last ban, next = nil.
	Str255 ban;							//The ban.
	Str63 setBy;						//The nick that set the ban.
	unsigned long time;				//The time the ban was made.
} bansRec, *bansP;

typedef struct UserListRec {	//A linked list of users.
	struct UserListRec *const next;		//The next user.
	Str63 nick;										//The user's nickname. BE CAREFUL, this is only a 63-character string.
	Str255 userhost;								//The user's user@host.
	unsigned long lastUHUpdate;				//The last time /userhost info was recieved for this user.
	char isOper;										//True if the user is an Oper.
	char isAway;									//True if the user is away.
	char isOp;											//True if the user is an op on this channel.
	char hasVoice;									//True if the user has voice on this channel.
	const channelPtr channel;					//channelPtr for the channel for this user record.
	long userlistServiceRefCon;				//RefCon for userlist service **ONLY**. If you're not a userlist, DON'T EVER touch this.
	long userNum;									//A unique user number generated for each UserListRec when it's created. You can use it to verify two users are the same.
															//Invalid if zero, although not necessarally valid if non-zero, either.
	char userlistIsSelected;					//Set by the userlist service. True if the user is selected, false if not. DON'T CHANGE THIS unless you're the userlist service.
	char isHalfOp;									//True is the user is a half-op. (mode +h on TS4 servers)
	char isIgnored;
	char pad;											//Padding to even data struct
	long facesServiceRefCon;					//RefCon for faces service **ONLY**. If you're not a faces service, DON'T EVER touch this.
} UserListRec, *UserListPtr;

enum chModes {
	modeT, modeN, modeI, modeM, modeP, modeS, modeL, modeK
};

/*	Channel list
*/
struct Channel {
	const channelPtr next;			//Pointer to next channel
	char PAD;
	const char active;					//(used internally)
	const char hasOps, hasVoice;		//True if you have ops or voice.
	const Str255 chName;				//The name of the channel.
	Str255 topic;						//The topic.
	char modes[8];						//Mode flags. True if on, false if not.
	long limit;							//If +l, the limit.
	Str255 key;							//If +k, the key.
	const bansP bans;					//Pointer to first ban, or nil if no bans.
	const UserListPtr userlist;	//Pointer to first entry in this channel's userlist.
	const short numUsers;				//Number of users in channel.
	Str63 topicSetBy;					//Nickname of the person who set the topic.
	long topicSetOn;					//The date and time the topic was set.
	const MWPtr window;			//Window for this channel.
	const linkPtr link;					//The link for this channel
	Str63 lastJoiner;					//The last person to join this channel.
	const bansP exceptions;		//Exceptions list for TS4.
	char hasHalfOps;					//True if +h. (TS4 servers)
	char gotNames;						//True if the channel has gotten it's entire names list.
};

	/*	A note about the userlist: the userlist is updated before plugins are called for
			join and nick messages, and afterwards for kicks, part and signoffs.
	*/

/*	Target Record
*/ 
enum targtype {
	targInvalid=-1,		//Invalid target
	targConsole=0,		//target is console
	targChannel,			//channel
	targQuery,			//query window
	targPlugin,			//plugin window
	targDCC				//DCC Chat window
};

typedef struct target {
	MWPtr mw;			//The window for the target
	linkPtr link;			//The link the window was created for.
	short type;			//The type of target this is. (values are in the enum targtype, listed above)
	char inactive;		//for targChannel, targDCC. If true, then the dcc/channel isn't active. (you aren't really in it)
	char bad;				//If true, this is an invalid target (i.e. target is bad, or console, or inactive channel, etc)
	
	channelPtr chan;		//For targChannel && !inactive, a channelPtr. otherwise nil
	connectionPtr dcc;	//For targDCC && !inactive, the connectionPtr for the dcc. otherwise nil
} target, *targetPtr;

/*	/ignore command data
*/
typedef struct ignoreRec {
	struct ignoreRec *next, *back;	//pointers to the next and previous ignore
	Str255 mask;							//mask to ignore
	long time;									//time ignore was placed
	long lastIgnored;						//last time this ignore was used (set by IsIgnored())
} ignoreRec, *ignorePtr;

/*	Message Buffer Data
*/
typedef struct messageBufferRec {
	Str63 nick;				//Their nickname
	long lastmessage;		//The time of the last message to/from them
	linkPtr link;				
	long unused[2];
} messageBufferRec, *messageBufferPtr;


#pragma mark еее Input Window

enum iwAlign {
	iwAlignLeft = 0,	//display before the status line
	iwAlignRight = 1	//display after the status line (before the memory counter)
};

enum iwOverrideErrors {
	iwOverrideNoErr = 0,			//IWOverride() successful
	iwOverrideNotFound,				//No such ShadowIRC input window widget found
	iwOverridePluginObject,		//The widget was a plugin widget. You can't have it.
	iwOverrideAlready				//That ShadowIRC widget is already overridden.
};

enum iwShadowIRCWidgets {
	iwStatusLine = 'stat'
};

typedef struct iwWidgetRec iwWidgetRec, *iwWidgetPtr;
struct iwWidgetRec {
	const iwWidgetPtr next;		//Next widget
	const iwWidgetPtr prev;		//Previous widget
	const long reserved;				//Reserved. Don't touch.
	const long type;					//Type you passed to IWNewObject()
	long data;								//Data area for you
	const Rect drawArea;			//The rect you are permitted to draw in
	short askedWidth;					//The width of space you asked for.
	const short givenWidth;			//The width of space you were given.
};

enum statuslineFlags {
	kUmodeFlagsOn = 1 << 0,		//Display the user mode
	kUmodeFlagsPlus = 1 << 1,	//Put a + in front of the modes
	kBoldedPopups = 1 << 2			//Bold the nick/channel/server name
};

typedef struct inputLineRec {
	const WindowPtr window;				//The inputwindow. If nil, it means there's no global floating inputline.
	const CharsHandle history;			//The text buffer
	const FontInfo fi;							//The status line font.
	const short fontnum;						//The status line font number
	const short fontsize;						//The status line font size
	
	const iwWidgetPtr widgetList;		//Status line widget list
	const Ptr reserved2;
	const Ptr inputline;						//The WEReference for the inputline text field.

	const short statusLinePos;
	const short statusLineHeight;		//The height of the status line
	const short hpos;							//The position in the inputline history
	
	char statuslineFlags;						//umodeFlags
} inputLineRec, *inputLinePtr;


#pragma mark еее Drag and Drop

/*	When drags are initiated from ShadowIRC, ShadowIRC may add the following sender-only
		drag flavors as approprate.
*/

enum flavors	 {
	kTextFlavor		= 'TEXT',
	kMWinFlavor		= 'MWIN',	//MWPtr of source, if source is message window.
	kUserFlavor		= 'USER'	//UserPtr if source is a channel window and the single selected word is a user on that channel
};

#pragma mark еее Menus

enum cmPopupsTypes {
	cmMW = 'cmmw',			//Message window
	cmInputline = 'cmil',		//Inputline
	cmPlugin = 'cmpl'			//Plugin window
};

#pragma mark еее Color List
/*	Internal ShadowIRC color numbers
		Internal ShadowIRC color codes are similar to Ircle color codes. A hex character 0x08 followed
		by a color code determines the color the text will be displayed in. (0x08 is control-h). The color codes
		here are the actual byte values. For example, to set the text color to the notice color, you would insert
		0x0805 into your string.
		
		See MWColor(), MSIrcleColor(), SMPrefix(), SMPrefixColor(), SMPrefixIrcleColor().
*/
enum siColors {
	numSIColors = 15,
	sicServer=0,				//Server message color
	sicStandard=1,				//Standard text color
	sicPrivmsg=2,				//Private message color
	sicCTCP=3,					//CTCP and DCC color
	sicOutgoing=4,				//Outgoing text color (stuff the user typed)
	sicNotice=5,					//Notice color
	sicWindowBG=6,			//Channel window background
	sicUserChannelOp=7,		//color for channel ops in userlists
	sicUserVoice=8,			//color for +v users in userlists
	sicUserOper=9,				//color for opers in userlists
	sicUserRegular=10,		//color for regular users in userlist
	sicStatusLine=11,			//color of the statusline text
	sicUserIgnored=12,		//color of ignored users in the userlist
	sicNotifyColor=13,		//color of notify messages
	sicTimestampColor=14,	//color of timestamps

	//These "color codes" are commands to instruct ShadowIRC to do different things
	sicNoCR=246,							//Inserts text into the window without the newline infront of it.
	sicDontLog=247,						//If this is anywhere in text being added to a message window, it is not logged.
	sicCustomColorBG=248,			//Specifies an RGB foreground and background color. Next 6 bytes are RGB values for front and back.
	sicCustomColor=249,				//Specifies an RGB color. Next 3 bytes are red, green, and blue values.
	sicSaveStyle = 250,					//Saves current text style.
	sicRestoreStyle = 251,				//Restores saved style
	sicSetColorMethod = 252,			//Next byte is number for colorMethod to use
	sicSaveColorMethod = 253,		//Saves the current colorMethod
	sicRestoreColorMethod = 254,	//Restores the saved colorMethod
	sicDontProcess=255					//any styles after this aren't processed.
};

enum colorMethod {
	cmNone,		//don't show colors. Assume default processing for stripping color codes.
	cmIrcle,		//process colors as Ircle colors
	cmMIRC,		//process colors as mIRC colors (eeeewwww)

	cmDefaultColorMethod = 254,	//The default color method
	cmWindowColorMethod = 255		//The selected color method for this window
};

#pragma mark еее Preferences

enum userlistSorts {
	kSortNick,
	kSortUserhost
};

/*	prefsRec
		NOTE:	It's a Bad Idea to change the user's preferences unless you absolutely have to. Use these
					so your plugin doesn't have to ask for preferences that the user would already have set.
					UNDER ABSOLUTELY NO CIRCUMSTANCES SHOULD YOUR PLUGIN *EVER* TOUCH spareBytes, unused, or reserved
					FIELDS. They're reserved (*WILL BE USED*) for additional preferences items. spareBytes and
					moreSpareBytes should ALWAYS be filled with zeros.
*/

typedef struct linkPrefsRec {	//One of these for each link. There are ten links total.
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
	char unused;
	Str63 recentNicks[10];				//Ten most recently used nicks
	Str255 recentChannels[10];		//Ten most recently joined channels
	short onoticeMethod;					//Onotice method
	Rect windowLoc[10];				//Window locations

	short maxConnAtt;					//Maximum number of connection attempts.
	short retryDelay;						//Retry delay, in seconds.
	Str255 signoffMessage;			//The signoff message
	Str255 fingerMessage;				//CTCP finger message
	Str255 userinfoMessage;			//CTCP userinfo message

	char regainNick;
	char unusedC[3];
	long unused1[2];
	long networkID;							//Network ID for server list
	
	Str255 kickMessage;
	long spare[763];
} linkPrefsRec, *linkPrefsPtr;

enum firewallType {
	fwNone,
	fwSOCKS5,
	fwSOCKS4A,
	fwSOCKS4
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

typedef struct prefsRec {
	Str255 serverMessagePrefix;	//Prefix for server messages
	Rect consoleLoc;						//Position and size of console window
	Rect inputLoc;							//Position and size of inputline.
	short unusedIW;
	char consoleOpen;						//Is console window open?
	char userListOpen;					//Is the userlist open?
	Rect userListRect;					//Size and position of the userlist
	Str31 defaultFontName;			//Name of default font
	short defaultFontSize;				//Size of text
	char noFontsMenu;					//If true, the fonts menu isn't displayed (or fonts parsed at launched)
	char disableColor;						//Turns off processing of Ctrl-C colors. (see enum colorMethod)
	short colorMethod;					//How ShadowIRC should display colors
	char disableStyles;					//Stops processing of ctrl-b, ctrl--, ctrl-v, and ctrl-n
	char squelchBeeps;					//Don't play ctrl-G beeps
	char textIndenting;					//Text indenting
	char ircIIDisplay;						//Display text in ircII format
	char timestamp;						//Timestamp all text in message windows
	char timestampSeconds;			//Include seconds in timstamps
	char timestampWhenAway;		//Only display the timestamp when away
	char escClearsInputline;			//Hitting the esc key clears the inputline
	short unused5;
	char showEndMessages;			//Show end of command messages from server
	char showUserHostsWithMsgs;	//Show userhosts with messages
	char beepOnPrivmsgs;				//Do a system beep on private messages. **WILL go away with sound support.
	char wallopsToConsole;				//Send all wallops to the console
	char serverNoticesToConsole;	//Send all server notices to the console
	char privmsgsToConsole;			//Sends all private messages to console
	char displayPingsInConsole;		//Turns on or off the display of server pings
	char autoQuery;						//Automatically open /query windows when you recieve a private message
	char autoRejoinAfterKick;			//Auto-rejoin a channel after being kicked
	char autoRejoinReconnect;			//Auto-rejoin open channels after reconnecting to the server
	char autoacceptInvites;				//Auto-accept channel invites
	char autoacceptInvitesWhenNotAway;	//Auto-accept invites only when not away
	char optionToMoveInputLine;		//Need to use option key to move up/down in inputline buffer. (If on, up/down moves up/down in current inputline text)
	char hideSplashScreen;				//Prevent splash screen from being shown during launch
	char doWhowasIfFailedWhois;	//Do a /whowas if /whois returns nick not found
	char dontActivateNewWindowsIfInputlineText;
	char quitAction;						//What do do when the user asks ShadowIRC to quit.
	char dccWindowAutoOpen;			//Whether or not the dcc window is open.
	char dccUsePortRange;
	char disableCTCP;						//Prevent responses to CTCP commands
	char noCTCPUnknownErrMsg;	//Stops the sending of CTCP ERRMSGs on unknown CTCP commands
	char disableCTCPMessages;		//Stop the display of CTCP messages.
	char enableDCC;						//Turns on or off DCC
	char autoAcceptDCCChat;			//Auto-accept DCC chats
	char autoAcceptDCCChatWhenNotAway;	//Auto-accept DCC Chats only when no away
	char autoLogging;						//If true, automatically log channel windows
	char autoLogQueries;					//If true, automatically logs /query windows
	char autoLogDCCChat;					//If true, automatically logs dcc chats
	char autoQueryOpen;
	char nonGlobalInput;
	short dccPortRangeLow, dccPortRangeHigh;
char unused[64];
	Str255 shortcuts[30];				//The shortcuts
char unused2[70];
	char autoDCCGet;						//Automatically accept dcc gets?
	char autoSaveDCC;					//Automatically save dcc gets?
	Str63 socksHost;						//Host for SOCKS server
	unsigned short socksPort;			//Port to connect to for SOCKS server
	char firewallType;					//type of firewall to use
	char displayJoin;						//Display join messages
	char displayPart;						//Display part messages
	char displayQuit;						//Display quit messages
	char displayModes;					//Display mode change messages
	char FastDCCSends;					//Faster DCC sends
long unused3[4];
	char displayNicks;						//Display nick change messages
	char noModesWidget;					//show modes widget?
	char ddToSameWin;					//Allow Drag and Drop to same window
	char ctcpToConsole;					//put all ctcps in console
	long dccSendPacketSize;			//DCC send packet size
	char displayKicks;						//Display kick messages
	char dccReverseSends;				//Enable reversed dcc sends
	char dccWindowOpen;				//True if the dcc status window is open, false if not.
	char displayInvites;					//Display invite messages
	Str31 socksUser;						//username for SOCKS user/pass authentication
	Str31 socksPass;						//password for SOCKS user/pass authentication
	Rect dccWindowRect;				//The location of the dcc window rect.
	long userlistNickWidth;				//The width of the nickname field in the userlist. (Default == 80)
	char userlistSort;						//Sorting method used by the userlist
	char userlistSortReverse;			//True if the userlist should be sorted in reverse order
	short userlistInWindowWidth;	//Width of default in-window userlist
	char userlistInWindow;				//True if the userlist should be displayed in channel windows.
	char resendAwayMessage;			//Resets the away message on reconnect.
	char userlistInWindowRight;		//True if the in-window userlist should be on the right side of the window.
	char userlistScrollbarLeft;		//True if the scrollbar is on the left
} prefsRec, *prefsPtr;

#pragma mark еее Miscelaneous


#pragma mark -
#pragma mark еее ShadowIRC Data Record

/*	ShadowIRCDataRecord
		The source of all information passed to your plugin.
*/
typedef struct ShadowIRCDataRecord {
	plugsPtr yourInfo; 					//Datarec for your plugin.
	void* messageData;						//Data for the message. If no data, nil. Usually a pointer to a struct for each message.
	short message;							//The message. See the top of this file for a list of possible messages. If you get a message you don't understand, IGNORE IT!
	char unused2;
	char completelyProcessed;					//If this is true, ShadowIRC stops calling plugins for this message. Ignored for init and idle messages.

	const prefsPtr mainPrefs;								//Pointer to the prefs.
	const linkPrefsPtr linkPrefs;							//Pointer to an array of ten link prefs
	RGBColor *const shadowircColors;				//The list of ShadowIRC internal colors.

	linkPtr *const firstLink;								//Pointer to the first link.
	
	ignorePtr *const ignores;								//Pointer to ignore list
	messageBufferPtr *const messageBuffers;	//Pointer to a time ordered list of 10 messageBuffers.
	connectionPtr *const connections;					//Pointer to the list of connections.
	const long *const curUserNum;						//The userNum given to the most recently created user. Anything larger than this is invalid.

	const long *const idleTime;					//Time when last command sent. (GetDateTime())
	const char *const debugOn;					//Is debug mode on or off?
	const long *const launchTime;				//Time when ShadowIRC was launched. (GetDateTime())
	const char *const inBackground;			//True if ShadowIRC is in the background, false if it's in the foreground.
	const long *const lastInput;					//The time the user last hit return in the inputline. (GetDateTime())
	const long *const lastKeyTime;				//The time the user last input any text to the inputline. (GetDateTime())

	targetPtr CurrentTarget;						//The active target
	inputLinePtr inputLine;							//Pointer to the inputline data.

	const MWPtr *const consoleWin;			//A pointer to the console.
	const MWPtr *const mwList;				//A pointer to the message window list.
	
	const Ptr internetConfig;						//The Internet Config component. Nil if IC is not installed. Typecast to internetConfigurationComponent.
} ShadowIRCDataRecord;


enum pVersionCheckMessageReply {
	pVersionShadowIRCTooOld = -1, 	//Pass this if you have determined that this version of ShadowIRC is too old.
	pVersionPluginTooOld = -2,			//Pass this if your plugin is too old to run on this version of ShadowIRC.
	pVersionCheckMessageReply = 5,	//This is the current version check reply.
	pUndocumentedAPIReply = 0x42414421 //This reply is undocumented. Do not use it.
};


#pragma mark еее Message Data structures
/*			------------------------------------------------------------------------------------------
				Message Data structures
				------------------------------------------------------------------------------------------
*/

/*	pVersionCheckData
		If you do not repsond to this message with pVersionCheckMessageReply, you will not get your
		init message. 
		When your plugin is called, version contains the version number from ShadowIRCVersion(). If you
		determine that this is too old a version for your plugin to run, set version to pVersionShadowIRCTooOld and
		LineMsg() an error message to the console. ShadowIRC will open the console if it's not already open.
*/
typedef struct pVersionCheckData {
	long version;	
} pVersionCheckData, *pVersionCheckDataPtr;

/*	pUserCommandDataRec
*/
typedef struct pCommandDataRec {
	linkPtr link;					//The link the command will go into
	StringPtr com;				//The command.
	LongString *rest;			//Any arguments the user may have typed after the command.
	LongString *outgoing;	//Text you want HandleCommand() to send to the server.
	char dontProcess;			//Set to true if you don't want ShadowIRC to process the command.
} pCommandDataRec, *pCommandDataPtr;

/*	pInputMessageRec
*/
typedef struct pInputMessageDataRec {
	linkPtr link;						//The link
	LongString *inputText;		//Text the user typed. When your plugin exits, the text to process.
	MWPtr mw;						//The target of the input.
	char action;						//On entry, true if the option key was down. On exit, process as action or not.
} pInputMessageDataRec, *pInputMessageDataPtr;

/*	pKeyDownDataRec
		Note: sidr->lastKey is equal to the time the _previous_ key was pressed.
*/
typedef struct pKeyDownDataRec {
	char *character;					//The character pressed. Set to 0 to stop ShadowIRC from processing it.

	EventRecord *e;					//The event record for the keypress.
} pKeyDownDataRec, *pKeyDownDataPtr;

/*	pNumericCommandDataRec
		ееWARNINGее
			You receive this message before ShadowIRC processes the numeric.
			If you set DONT_PROCESS, you *MUST* do *ALL* processing ShadowIRC would have done.
			There is not a dontDisplay variable. There may be in the future.
		ееWARNINGее
*/
typedef struct pNumericCommandDataRec {
	linkPtr link;							//The link
	StringPtr from;					//The server the message came from.
	short command;					//Serer command. Server commands are documented in RFC1459.
	LongString *incoming;			//A pointer to a *COPY* of the data from the server.
	char *commandProcessed;	//True if ShadowIRC procesed the message, false if not. On exit, set this to true if
												//you process the message. (You don't necessarally have to set sidr->completelyProcessed.)
	short *lastNumerics;			//10 most recent numerics; lastNumerics[0] is this numeric.
	char DONT_PROCESS;			//Prevents ShadowIRC from processing this numeric.
} pNumericCommandDataRec, *pNumericCommandDataPtr;

/*	pServerJOINDataRec
*/
typedef struct pServerJOINDataRec {
	linkPtr link;							//The link
	StringPtr channelName;			//The name of the channel that was joined or parted.
	StringPtr username;				//The name of the user.
	StringPtr userhost;				//The user@host of the user.
	const UserListPtr userPtr;	//The UserListPtr of this user.
	const channelPtr channel;		//The channel the user joined.
	const char isMe;					//True if the user is me, false if not.
	char dontDisplay;					//Don't display the join message. Defaults to setting in prefs.
	char dontSound;					//Don't play the sound. Defaults to false.
} pServerJOINDataRec, *pServerJOINDataPtr;

/*	pServerPARTDataRec
*/
typedef struct pServerPARTDataRec {
	linkPtr link;					//The link
	StringPtr channelName;			//The name of the channel that was joined or parted.
	StringPtr username;				//The name of the user.
	StringPtr userhost;				//The user@host of the user.
	const UserListPtr userPtr;	//The UserListPtr of this user.
	const channelPtr channel;		//The channel the user left.
	LongString *partReason;		//The user's part reason (for DalNet)
	const char isMe;					//True if the user is me, false if not.
	char dontDisplay;					//Don't display the part message. Defaults to setting in prefs.
	char dontSound;					//Don't play the sound. Defaults to false.
} pServerPARTDataRec, *pServerPARTDataPtr;

/*	pServerQUITDataRec
*/
typedef struct pServerQUITDataRec {
	linkPtr link;								//The link
	StringPtr username;					//The name of the user who signed off.
	StringPtr userhost;					//If it's another user signing off, their userhost.
													//If it's you, this is a boolean value; true if a signoff message was specified, false if the default one was used.
	LongString *signoffMessage;		//The user's signoff message.
	const char isMe;						//True if you, false if not.
	char dontDisplay;						//Don't display the quit message. Defaults to setting in prefs.
	char dontSound;						//Don't play the sound. Defaults to false.
} pServerQUITDataRec, *pServerQUITDataPtr;

/*	pServerTOPICDataRec
*/
typedef struct pServerTOPICDataRec {
	linkPtr link;						//The link
	StringPtr username;			//The name of the user changing the topic.
	StringPtr userhost;			//The userhost of the user.
	StringPtr channelName;		//The channel whose topic is being changed.
	LongString *newtopic;		//The new topic.
	channelPtr channel;			//The channel whose topic was changed.
	const char isMe;				//True if I'm changing the topic, false if not.
	char dontDisplay;				//Don't display the topic change in the channel.
	char dontSound;				//Don't play the sound. Defaults to false.
} pServerTOPICDataRec, *pServerTOPICDataPtr;

/*	pServerNICKDataRec
		Note: this message is sent to plugins *BEFORE* channel userlists are updated.
*/
typedef struct pServerNICKDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user changing nicks.
	StringPtr userhost;		//The userhost of the user.
	StringPtr newnick;		//Their new nickname.
	const char isMe;			//True if I'm changing nicks.
	char dontDisplay;			//Don't display the nick change message.
	char dontSound;			//Don't play the sound. Defaults to false.
} pServerNICKDataRec, *pServerNICKDataPtr;

/*	pServerPINGDataRec
*/
typedef struct pServerPINGDataRec {
	linkPtr link;					//The link
	LongString *source;		//The source of the ping.
} pServerPINGDataRec, *pServerPINGDataPtr;

/*	pServerPONGDataRec
*/
typedef struct pServerPONGDataRec {
	linkPtr link;				//The link
	StringPtr source;		//the source of the pong.
} pServerPONGDataRec, *pServerPONGDataPtr;

/*	pServerKICKDataRec
*/
typedef struct pServerKICKDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user kicking.
	StringPtr userhost;		//The userhost of the user.
	StringPtr channelName;	//The channel.
	StringPtr kickedNick;	//The name of the user getting kicked.
	LongString *message;	//The kick message.
	channelPtr channel;		//The channel that the user was kicked from.
	const char isMe;			//True if I was kicked.
	char autoRejoin;			//On entry, if ShadowIRC will autorejoin. On exit, whether ShadowIRC should autorejoin.
	char dontDisplay;			//Don't display the kick message.
	char dontSound;			//Don't play the sound. Defaults to false.
} pServerKICKDataRec, *pServerKICKDataPtr;

/*	pServerINVITEDataRec
*/
typedef struct pServerINVITEDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user inviting you.
	StringPtr userhost;		//The userhost of the user.
	StringPtr channel;			//The channel you are being invited to.
	char autojoin;				//On entry, true if ShadowIRC will join the channel. On exit, whether it will or not.
	char ignored;				//True if the person is being ignored (in which case, autojoin will always be false)
	char dontDisplay;			//If false, ignore message is displayed
	char dontSound;			//If false, ignore sound is played.
} pServerINVITEDataRec, *pServerINVITEDataPtr;

/*	pServerWALLOPSDataRec
*/
typedef struct pServerWALLOPSDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user sending a WALLOPS.
	StringPtr userhost;		//The userhost of the user.
	LongString *message;			//The message.
	char ignore;							//Set to true if you want ShadowIRC to not display the wallop.
} pServerWALLOPSDataRec, *pServerWALLOPSDataPtr;

/*	pServerERRORDataRec
*/
typedef struct pServerERRORDataRec {
	linkPtr link;					//The link
	LongString *message;	//the text of the error message.
} pServerERRORDataRec, *pServerERRORDataPtr;

/*	pServerPRIVMSGDataRec
*/
typedef struct pServerPRIVMSGDataRec {
	linkPtr link;							//The link
	StringPtr username;				//The nick of the user who sent the message.
	StringPtr userhost;				//The userhost of the user.
	StringPtr target;					//The target of the message.
	channelPtr chan;					//If the trget is a channel, the channel.
	LongString *message;			//The message.
	char ignored;						//If true, the message will be ignored.
	const char targMe;				//If true, the message was going to me.
	const char targChan;				//If true, the message was going to a channel.
	const char targOther;			//If true, the message wasn't targeted at me or a channel.
	const char fromServer;		//If true, the message came from the server.
	char autoQuery;					//If true, ShadowIRC intends to auto-open a /query window. Set to false to keep the window from opening.
	char displayUserHost;			//If true, displays the person's userhost. defaults to targMe && mainPrefs->showUserHostsWithMsgs
	char dontSound;					//If false, doesn't play sound.
	MWPtr redirect;					//If not nil, redirect output to this message window.

	unsigned char *nickStyle;		//Pascal string representing color/style to be used for the nickname display. (max length = 9)
	unsigned char *textStyle;		//Pascal string representing color/style to be used for text. (max length = 9)
} pServerPRIVMSGDataRec, *pServerPRIVMSGDataPtr;

/*	pServerNOTICEDataRec
*/
typedef struct pServerNOTICEDataRec {
	linkPtr link;							//The link
	StringPtr username;				//The nick of the user who sent the message.
	StringPtr userhost;				//The userhost of the user.
	StringPtr target;					//The target of the message.
	channelPtr chan;					//If the target is a channel, the channel.
	LongString *message;			//The message.
	char ignored;						//If true, the message will be ignored. You shouldn't need to set this to false.
	const char targMe;				//If true, the message was going to me.
	const char targChan;				//If true, the message was going to a channel.
	const char targOther;			//If true, the message wasn't targeted at me or a channel.
	const char fromServer;		//If true, the message came from the server.
	char autoQuery;					//If true, ShadowIRC intends to auto-open a /query window. Set to false to keep the window from opening.
	char displayUserHost;			//If true, displays the person's userhost. defaults to targMe && mainPrefs->showUserHostsWithMsgs
	char toConsole;						//Force to console
	char dontSound;					//If true, don't play a sound.

char unused[3];	

	MWPtr redirect;					//If not nil, redirect output to this message window.
}  pServerNOTICEDataRec, *pServerNOTICEDataPtr;

/*	pServerRPONGData
*/
typedef struct pServerRPONGData {
	linkPtr link;						//The link
	StringPtr server;				//Server that replied to the /rping
	StringPtr num;					//Some seemingly random number the server spits out. If you know what this means, please let me know.
	long pingTime;					//The time, in milliseconds, the server took to reply
	char dontDisplay;				//Set to true if you don't want ShadowIRC to display the rping reply message
	char dontSound;				//Don't play the sound
} pServerRPONGData, *pServerRPONGDataPtr;

/*	pServerSILENCEData
*/
typedef struct pServerSILENCEData {
	linkPtr link;							//The link
	StringPtr mask;					//Mask that has been silenced or unsilenced
	char add;								//True if the mask is now being silenced, false if not
	char completelyProcessed;	//Set to true if you don't want ShadowIRC to display the notification.
} pServerSILENCEData, *pServerSILENCEDataPtr;

/*	pServerMODEData
		Note: You get one of these for each mode change on a channel. You may wish to cache them and react when
		you get a message where done=1.
*/
typedef struct pServerMODEData {
	linkPtr link;			//The link
	channelPtr ch;		//Channel for the mode change. nil if usermode
	char up;				//true = mode +, false = mode -
	char mode;			//The mode changed, i.e. 'v', 'o', 'k', etc. 'е' if usermode.
	long data;				/*	If mode v, o or h, UserListPtr to user. If mode b, StringPtr to ban change.
										If mode k, StringPtr to channel key. If mode +l, new channel limit.
										If mode == 'е', StringPtr to unprocessed usermode change.
										If done, StringPtr to full modechange. Otherwise, nil.
								*/
	char done;				//If true, finished processing the MODE command from the server. (Always true for umode changes.)
	char dontDisplay;	//If true, ShadowIRC won't display the message
	char dontSound;	//If true, ShadowIRC won't ask the sound service to play the mode sound
} pServerMODEData, *pServerMODEDataPtr;

/*	pCTCPDataRec
*/
typedef struct pCTCPDataRec {
	linkPtr link;							//The link
	connectionPtr dcc;				//If this was a DCC CTCP, this is a pointer to the dcc, else nil.
	StringPtr username;				//The name of the user who did the ctcp.
	StringPtr userhost;				//The userhost of the user. 
	StringPtr target;					//The target. (Nick or channel.)
	StringPtr cmd;						//The CTCP command.
	StringPtr rest;						//Any parameters for the CTCP message. (Action text, CPING time, etc)
	char *ignored;						//If true, the CTCP will be ignored.
	char isReply;						//If true, this is a reply to a CTCP we did.
	char completelyProcessed;	//Set this to true to keep ShadowIRC from processing this CTCP message.
} pCTCPDataRec, *pCTCPDataPtr;

/*	pDCCIncomingChatDataRec
		Note:	If you delete the DCC connection, make SURE you set completelyProcessed and
					sidr->completelyProcessed.
*/
typedef struct pDCCIncomingChatDataRec {
	connectionPtr conn;				//The connection.
	LongString *text;					//The received text
	char completelyProcessed;	//Set this to true to keep ShadowIRC from displaying.
} pDCCIncomingChatDataRec, *pDCCIncomingChatDataPtr;

/*	pMWTextMessage
*/
typedef struct pMWTextData {
	MWPtr mw;							//The message window
	LongString *ls;						//The text that will go in it.
} pMWTextDataRec, *pMWTextDataPtr;

/*	pDCCChatCookieData
*/
typedef struct pDCCChatCookieData {
	connectionPtr conn;				//The DCC connection
	long cookie;							//The cookie
	LongString *data;					//The cookie data.
	char isReply;						//True if this cookie was a reply
	char processed;					//If isReply, true if it was processed by the other end, false if not.
												//if not isReply, set to true if you process, leave alone if you don't.
												//If you reply to this cookie, do it through DCCSendCookieReply()
} pDCCChatCookieData, *pDCCChatCookieDataPtr;

/*	pShortcutProcessData
*/
typedef struct pShortcutProcessData {
	Handle textH;						//An unlocked handle to shortcut text. (Parse your $tokens with Munger()).
} pShortcutProcessData, *pShortcutProcessDataPtr;


/*	pCMPopupsData
		Use CMAdd() to add items to the contextual menu. As odd as it may seem, pass this struct to it.
*/
typedef struct pCMPopupsData {
	long type;					//Type of window message is for (see enum cmPopupsTypes)
	
	WindowPtr w;			//The window
	MWPtr mw;				//If a message window, the one being clicked on
	mwPanePtr pane;		//If a message window, the pane being clicked on

	Point where;				//Point in the window where user clicked.
	
	long s0, s1;				//Selection range in message window
	char noSelection;		//True if no selection
	char oneWord;			//True if only one word is slected
	short unused;
	Str255 theWord;		//If oneWord, the word selected.
} pCMPopupsData, *pCMPopupsDataPtr;

/*	pCMPopupsReturnData
*/
typedef struct pCMPopupsReturnData {
	long type;					//Type of window message is for (see enum cmPopupsTypes)
	
	WindowPtr w;			//The window
	MWPtr mw;				//If a message window, the one being clicked on
	mwPanePtr pane;		//If a message window, the pane being clicked on
	
	Point where;				//Point in the window where user clicked.

	long s0, s1;				//Selection start in message window
	char noSelection;		//True if no selection
	char oneWord;			//True if only one word is slected
	short unused;
	Str255 theWord;		//If oneWord, the word selected.
	
	long id;						//The selected ID of the menu item the user chose. If menuID != -1, this is the item number of the menu selection
	short menuID;			//The menu ID if a submenu, or -1.
} pCMPopupsReturnData, *pCMPopupsReturnDataPtr;

/*	pContextSwitchDataRec
*/
typedef struct pContextSwitchDataRec {
	const char inBackground ;	//If false, ShadowIRC is now the foreground application. If true, it's now in the bg.
} pContextSwitchDataRec, *pContextSwitchDataPtr;

/*	pMWNewData
		NOTE: logRefNum, chan, dcc, and link fields are nil at this point, because ShadowIRC has not
					had a chance to fill them in yet. *DO NOT* rely on the contents of these fields at this point.
					If you are using this message to create a new message window widget, make sure to
					allow for this. You can still get the window's type, and by the time you need to draw,
					ShadowIRC will have filled in the rest of the MWPtr as appropriate.
*/
typedef struct pMWNewData {
	const MWPtr mw;		//Message window being created
} pMWNewData, *pMWNewDataPtr;

/*	pMWDestroyData
		NOTE:	At this point, ShadowIRC has not closed the log file, so logRefNum is still valid. You can't
						count on the accuracy of chan, dcc, or the link fields, although the window type is still
						valid. *DON'T* use this message to dispose of message window widgets or panes; there's
						other messages specifically for that.
*/
typedef struct pMWDestroyData {
	const MWPtr mw;		//Message window being destroyed
} pMWDestroyData, *pMWDestroyDataPtr;

/*	pDCCCreateData
*/
typedef struct pDCCCreateData {
	connectionPtr conn;	//The connection created.
} pDCCCreateData, *pDCCCreateDataPtr;

/*	pDCCOpenedData
*/
typedef struct pDCCOpenedData {
	connectionPtr conn;	//ShadowIRC is attempting to open a DCC connection. The connection has not yet been established.
} pDCCOpenedData, *pDCCOpenedDataPtr;

/*	pDCCClosedData
*/
typedef struct pDCCClosedData {
	connectionPtr conn;	//The connection to be closed. (It will not have been destroyed yet.)
	char silent;				//If true, you shouldn't output any messages.
} pDCCClosedData, *pDCCClosedDataPtr;

/*	pInputTextDisplayData
*/
typedef struct pInputTextDisplayData {
	targetPtr targ;					//The target text was typed into.
	LongString *input;			//The text the user typed. (Could be modified previously by a pInputMessage)
	LongString *display;			//The text to display after ShadowIRC has done its processing.
	char textIndenting;			//True if text indenting is on
	char ircIIDisplay;				//True if ircII formatting is on
} pInputTextDisplayData, *pInputTextDisplayDataPtr;

/*	pNOTIFYData
		NOTE:	To find people in the notifyList who signed on, look for (n->signedon && n->changed).
					To find people who signed off, look for (n->waitingForResponse)
*/
typedef struct pNOTIFYData {
	linkPtr link;					//The link
	notifyPtr notifyList;		//The link's notify list. (link->notifyList). Provided for convienence.
	const short signedOn;	//The number of users that signed on since the last notify period.
	const short signedOff;	//The number of users that signed off since the last notify period.
} pNOTIFYData, *pNOTIFYDataPtr;

/*	pDCCConnOpenedData
*/
typedef struct pDCCConnOpenedData{
	connectionPtr conn;		//The DCC connection that was established. The DCC connection is fully open.
} pDCCConnOpenedData, *pDCCConnOpenedDataPtr;

/*	pUIWindowCloseDataRec
*/
typedef struct pUIWindowCloseDataRec {
	WindowPtr w;	//a WindowPtr to the window that was a target of a close action.
	MWPtr mw;		//A MWPtr to the window closed if the window is a message window. Otherwise nil.
} pUIWindowCloseDataRec, *pUIWindowCloseDataPtr;

/*	pUIDragTrackData
*/
typedef struct pUIDragTrackData {
	WindowPtr window;						//The window
	DragReference drag;						//The DragReference
	DragTrackingMessage message;		//the DragTrackingMessage
} pUIDragTrackData, *pUIDragTrackDataPtr;

/*	pUIDragReceiveData
*/
typedef struct pUIDragReceiveData {
	WindowPtr window;	//The window
	DragReference drag;	//The DragReference
	OSErr returnVal;		//Receive Handler error code
} pUIDragReceiveData, *pUIDragReceiveDataPtr;

/*	pMWPaneDragTrackData
*/
typedef struct pMWPaneDragTrackData {
	mwPanePtr pane;							//The pane
	DragReference drag;						//The drag
	DragTrackingMessage message;		//The DragTrackingMessage, as approprate for this pane
	Point mouse;									//The mouse location, in local coords
} pMWPaneDragTrackData, *pMWPaneDragTrackDataPtr;

/*	pMWWidgetDragTrackData
*/
typedef struct pMWWidgetDragTrackData {
	mwWidgetPtr widget;					//The widget
	DragReference drag;						//The drag
	DragTrackingMessage message;		//The DragTrackingMessage, as approprate for this widget
	Point mouse;									//The mouse location, in local coords
} pMWWidgetDragTrackData, *pMWWidgetDragTrackDataPtr;

/*	pMWPaneDragReceiveData
*/
typedef struct pMWPaneDragReceiveData {
	mwPanePtr pane;					//The pane
	DragReference drag;				//The drag
	Point mouse;							//The mouse location, in local coords
	char received;						//True if you accepted the drag, false if not.
} pMWPaneDragReceiveData, *pMWPaneDragReceiveDataPtr;

/*	pMWWidgetDragReceiveData
*/
typedef struct pMWWidgetDragReceiveData {
	mwWidgetPtr widget;			//The widget
	DragReference drag;				//The drag
	Point mouse;							//The mouse location, in local coords
	char received;						//True if you accepted the drag, false if not.
} pMWWidgetDragReceiveData, *pMWWidgetDragReceiveDataPtr;

/* pMWWindowMoveDataRec
*/
typedef struct pMWWindowMoveDataRec {
	MWPtr mw;	//the window that was moved.
	Rect newpos;	//where the window was moved to
} pMWWindowMoveDataRec, *pMWWindowMoveDataPtr;


/*	pUIMWGotTextDataRec
*/
typedef struct pUIMWGotTextDataRec {
	MWPtr window;		//The message window that got text
	LongString *text;		//The text the user typed into it
} pUIMWGotTextDataRec, *pUIMWGotTextDataPtr;

/*	pPWActivateData
		NOTE:	If you set hasConnectionsList to true, make *SURE* that the list is
					item 10 in your DITL. You should use the list control item found in the connection prefs DITL in ShadowIRC,
					and it should be the same size and position.
*/
typedef struct pPWActivateData {
	short id;							//id of pane to activate
	short ditlNum;					//set this to the id of your DITL resource
	char hasConnectionsList;	//set to true if you want the connections list.
} pPWActivateData, *pPWActivateDataPtr;

/*	pPWSetWindowData
*/
typedef struct pPWSetWindowData {
	DialogPtr PrefsDlg;	//The Prefs Dialog
	short id;					//id of pane to set up
	short connListNum;	//The connection (0-9) that you are setting data for. (Ignore if you didn't specify hasConnectionsList = true)
} pPWSetWindowData, *pPWSetWindowDataPtr;

/*	pPWGetWindowData
*/
typedef struct pPWGetWindowData {
	DialogPtr PrefsDlg;	//The Prefs Dialog
	short id;					//id of pane to deactivate
	char dontClose;			//Set to true if you want to prevent the prefs window from closing.
	char padding;
	short connListNum;	//The connection (0-9) that you are reading data for. (Ignore if you didn't specify hasConnectionsList = true)
} pPWGetWindowData, *pPWGetWindowDataPtr;

/*	pPWItemHitData
*/
typedef struct pPWItemHitData {
	DialogPtr PrefsDlg;	//The Prefs Dialog
	short id;					//id of pane that got hit
	short itemNum;			//item number that got hit.
	Point pt;					//Location in window of click
	EventRecord *e;		//The event record of the action.
} pPWItemHitData, *pPWItemHitDataPtr;

/*	pPWRedrawData
*/
typedef struct pPWRedrawData {
	DialogPtr PrefsDlg;	//The Prefs Dialog
	short id;					//id of your pane.
} pPWRedrawData, *pPWRedrawDataPtr;

/*	pPWClosedData
*/
typedef struct pPWClosedData {
	char canceled;		//True if prefs changes were canceled.
} pPWClosedData, *pPWClosedDataPtr;

/*	pUIActivateData
*/
typedef struct pUIActivateData {
	WindowPtr window;	//The window being activated or deactivated
	char activate;			//True if being activated, false if being deactivated.
} pUIActivateData, *pUIActivateDataPtr;

/*	pUIMouseUpDownData
*/
typedef struct pUIMouseUpDownData {
	WindowPtr window;	//The window that was clicked on, or nil if a mouseUp event.
	EventRecord *e;		//The full event record
	char down;				//True if the mouse was pressed, false if it was released.
} pUIMouseUpDownData, *pUIMouseUpDownDataPtr;

/*	pUIMWInfoRefreshData
*/
typedef struct pUIMWInfoRefreshData {
	MWPtr mw;					//The message window that's getting refreshed
	Str255 displayStr;		//If you want ShadowIRC to put in a string, set this. Otherwise, leave it unchanged and do your own drawing
	Rect rect;						//The rect for the status area, in the window's local coordinates.
	char active;					//True if the window is active, false if not.
} pUIMWInfoRefreshData, *pUIMWInfoRefreshDataPtr;

/*	pHelpMenuData
		Note: text and style must *both* be set, and non-nil. Both will be released from memory when
					ShadowIRC is done with them.
*/
typedef struct pHelpMenuData {
	short reference;			//The return value from HMIAdd()
	Handle text;					//Resource handle to your 'TEXT' resource.
	Handle style;					//Resource handle to your 'styl' resource.
} pHelpMenuData, *pHelpMenuDataPtr;

/*	pDNSLookupData
*/
typedef struct pDNSLookupData {
	long refCon;				//The number you passed to DNSLookup()
	long ip;						//The IP searched/found as four 8-bit values
	StringPtr search;		//What you searched for
	StringPtr reply;		//The results of the search. Length of 0 if search not successful.
	char nameToIP;			//True if this was a name to IP search, false if an IP to name search
	char successful;		//True if the IP/name could be resolved, false if not.
} pDNSLookupData, *pDNSLookupPtr;

/*	pAsyncSoundCompletionData
*/
typedef struct pAsyncSoundCompletionData {
	long refcon;			//The refcon you passed to AsyncSoundPlay()
	Handle sound;		//The sound handle you passed to AsyncSoundPlay()
	Ptr soundChannel;	//The sound channel AsyncSoundPlay() created and returned to you. Don't dispose of it.
} pAsyncSoundCompletionData, *pAsyncSoundCompletionDataPtr;

/*	pServiceActivateWinData
		NOTE: You will sometimes get thhis message twice in a row for the same window.
*/
typedef struct pServiceActivateWinData {
	channelPtr ch;			//Channel that was activated. Nil if the window wasn't a channel
	char activate;			//True if activated, false if deactivated
	char padding[3];
	WindowPtr w;			//The window that was activated/deactivated.
	MWPtr mw;				//If the window is a message window, the MWPtr. Nil otherwise.
} pServiceActivateWinData, *pServiceActivateWinDataPtr;

/*	pServiceWindowMenuData
*/
typedef struct pServiceWindowMenuData {
	long key;				//The value selected
	short menuItem;	//The menu item number
} pServiceWindowMenuData, *pServiceWindowMenuPtr;

/*	pServiceULUserhostsData
*/
typedef struct pServiceULUserhostsData {
	linkPtr link;			//The link receiving the userhosts.
} pServiceULUserhostsData, *pServiceULUserhostsPtr;

/*	pServiceULNamesEndData
*/
typedef struct pServiceULNamesEndData {
	channelPtr channel;
} pServiceULNamesEndData, *pServiceULNamesEndDataPtr;

/*	pServiceULTrashChannelData
*/
typedef struct pServiceULTrashUsersData {
	channelPtr channel;	//The channel being disposed of.
} pServiceULTrashChannelData, *pServiceULTrashChannelDataPtr;

/*	pServiceCWLinkStateChangeData
*/
typedef struct pServiceCWLinkStateChangeData
{
	linkPtr link;				//The link whose connection state has changed. link->conn is not guaranteed to be non-nil
	short connectStage; 	//same as link->conn->connectStage. Values are in enum connectStage.
} pServiceCWLinkStateChangeData, *pServiceCWLinkStateChangeDataPtr;

/*	pServiceUserRegData
*/
typedef struct pServiceUserRegData {
	linkPtr link;
	char registered;
} pServiceUserRegData, *pServiceUserRegPtr;

/*	pIPCMessageData
*/
typedef struct pIPCMessageData
{
	long type;					//The IPC type registered with IPCRegister()
	long message;			//A message number.
	long data;					//Data for the message.
	long result;				//A result code to pass back to the calling plugin. Initially zero.
	void* client;				//A reference to the plugin that sent a message with IPCSend(). Use with IPCReply()
	char ack;					//Set to true if the IPC service processes this message, leave alone if you don't.
} pIPCMessageData, *pIPCMessageDataPtr;

/*	pIPCReplyMessageData
*/
typedef struct pIPCReplyMessageData
{
	long type;					//IPC class
	long message;			//A message number
	long data;					//Data for the message
	long result;				//Your reply to the IPC service. Initially zero.
	char ack;					//Set to true if your plugin processes this message, leave alone if you don't.
} pIPCReplyMessageData, *pIPCReplyMessageDataPtr;

/*	pConnectionData
*/
typedef struct pConnectionData {
	connectionPtr conn;	//The connection
	char event;				//The event. see enum connectionStatus
	char sendEvent;		//If true, the event is the result of a ConnPut*()
	LongString *data;		//If event==C_CharsAvailable, and this is a text connection, ptr to data recieved.
} pConnectionData, *pConnectionDataPtr, 
	pTextConnectionData, *pTextConnectionDataPtr, pDataConnectionData, *pDataConnectionDataPtr;

/*	pIWUpdateData
*/
typedef struct pIWUpdateData
{
	iwWidgetPtr widget;		//Inputwindow widget
} pIWUpdateData, *pIWUpdateDataPtr;

/*	pIWClickData
*/
typedef struct pIWClickData
{
	iwWidgetPtr widget;		//Inputwindow widget
	Point where;					//Location the mouse was pressed
} pIWClickData, *pIWClickDataPtr;

/*	pMWWidgetUpdateData
*/
typedef struct pMWWidgetUpdateData {
	mwWidgetPtr widget;
	char winActive;
} pMWWidgetUpdateData, *pMWWidgetUpdateDataPtr;

/*	pMWWidgetClickData
*/
typedef struct pMWWidgetClickData
{
	mwWidgetPtr widget;
	Point where;
} pMWWidgetClickData, *pMWWidgetClickDataPtr;

/*	pMWWidgetDestroyData
*/
typedef struct pMWWidgetDestroyData {
	mwWidgetPtr widget;
} pMWWidgetDestroyData, *pMWWidgetDestroyDataPtr;

/*	pMWPaneDestroyData
*/
typedef struct pMWPaneDestroyData {
	mwPanePtr pane;
} pMWPaneDestroyData, *pMWPaneDestroyDataPtr;

/*	pMWPaneActivateData
*/
typedef struct pMWPaneActivateData {
	mwPanePtr pane;
	char activate;
} pMWPaneActivateData, *pMWPaneActivateDataPtr;

/*	pMWPaneClickData
*/
typedef struct pMWPaneClickData {
	mwPanePtr pane;
	EventRecord *e;
} pMWPaneClickData, *pMWPaneClickDataPtr;

/*	pMWPaneUpdateData
*/
typedef struct pMWPaneUpdateData {
	mwPanePtr pane;			//The pane to update
} pMWPaneUpdateData, *pMWPaneUpdateDataPtr;

/*	pMWPaneResizeData
*/
typedef struct pMWPaneResizeData {
	mwPanePtr pane;
} pMWPaneResizeData, *pMWPaneResizeDataPtr;

/*	pSoundServiceData
*/
typedef struct pSoundServiceData {
	long sound;		//The sound type. see enum soundServiceSounds
	long data;			//Data.
} pSoundServiceData, *pSoundServiceDataPtr;

/*	Values for pSoundServiceData.data:
	sndNotice:	true == from server; false == from user
	sndJoin:	true == you joined; false == someone else joined
	sndPart:	true == you left; false == someone else left
	sndNick:	true == you changed nicks; false == someone else
	sndMode:	number of mode changes
	sndKick:	true == you were kicked; false == someone else was kicked
	sndRPong:	number of milliseconds the server took to reply
	sndCPing:	number of milliseconds the /cping took
	sndNotify:	HiWord = num new online; LoWord = num new offline
*/

/*	pDCCStatusData
*/
typedef struct pDCCStatusData {
	dccPtr dcc;				//The dcc you should return status for
	dccStatus *status;	//Your status
} pDCCStatusData, *pDCCStatusDataPtr;


/*	where codes for SMPrefix and SMPrefixColor
*/
enum displaySelection {
	dsNowhere,	//Don't display
	dsConsole,	//Display in console
	dsFrontWin	//Display in front window
};

#pragma mark -
#pragma mark ShadowIRC Functions
/*	------------------------------------------------------------------------------------------
		ShadowIRC Functions
		
		Unless otherwise specified, all character pointers are pointers to 255-character Pascal strings.
		------------------------------------------------------------------------------------------
*/

#pragma mark е LongStrings
/*	------------------------------------------------------------------------------------------
		LongStrings

		A LongString is a structure that implements a pascal style "long" string. Although
		technically, this string could be 32k in size, it's maximum size has been defined as
		600 since IRC protocol only allows for a maximum line length of 512 characters. This gives us
		a buffer to handle internal colors, timestamps, etc.
		
		Please note, LongString.data[1] is the first character of a LongString. LongString.data[0]
		is unused, and is because it allows for more efficient accessing and more easily understandable
		code. It makes sense that LongString.data[n] is the nth character, not the n+1th character.
		
		You can use LongString.data as a Str255 if you set LongString.data[0] to correct length. This can
		come in useful sometimes.
		
		NOTE: ALL Character pointers are pascal strings.
		------------------------------------------------------------------------------------------
*/

pascal char LSCmp(const LongString *ls1, const LongString *ls2);
pascal char LSCaseCmp(const LongString *ls1, const LongString *ls2);
/*	Compares two LongStrings. LSCaseCmp is case insensitive.
		Input:	ls1, ls2 - two longstrings
		Output:	true if ls1 == ls2, false if ls1 != ls2 or you pass nil as either parameter
*/

pascal void LSDelete(LongString *ls, short firstpos, short lastpos);
/*	Deletes text from a LongString
		Input:	ls - LongString
					firstpos - First character to delete
					lastpos - Last character to delete
		Output:	ls - Modified LongString
*/

pascal void LSCopy(const LongString *ls, short firstpos, short len, LongString *out);
/*	Copies a portion of a LongString into another.
		Input:	ls - LongString
					firstpos - First character to copy. If <= 0, copies the entire LongString.
					len - number of characters to copy
		Output:	out - LongString copied from ls. out can be ls.
*/

pascal void LSCopyString(const LongString *ls, short firstpos, short len, StringPtr str);
/*	Copies a portion of a LongString into a pascal string.
		Input:	ls - LongString
					firstpos - First character to copy
					len - number of characters to copy
		Output:	str - Pascal string copied from ls
*/

pascal short LSPosCustom(ConstStr255Param s, const LongString *ls, short start);
pascal short LSPos(ConstStr255Param s, const LongString *ls);
pascal short LSPosCase(ConstStr255Param s, const LongString *ls);
pascal short LSPosCaseCustom(ConstStr255Param s, const LongString *ls, short start);
/*	Finds the first location of a substring in a LongString. LSPosCase and LSPosCaseCustom are case insensitive.
		Input:	s - Pascal substring
					ls - LongString to search
					start - in LSPosCustom and LSPosCaseCustom, the first character to start searching at
		Output:	return value - Location of the first character found, or 0 if not found.
*/

pascal short LSPosChar(short c, const LongString *ls);
/*	Finds the first location of a character in a LongString.
		Input:	c - A character
					ls - LongString to search
		Output:	return value - Location of the first character found, or 0 if not found
*/

pascal void LSNextArg(LongString *ls, StringPtr arg);
/*	Gets the next argument in a LongString and deletes it.
		Input:	ls - LongString to operate on.
					arg - Address to copy argment to. If arg = 0, the argument is discarded.
		Output:	ls - LongString, less the text of the first argument.
					arg - Pascal string of the first argument.
*/

pascal void LSNextArgND(LongString *ls, StringPtr arg);
/*	Gets the next argument in a LongString and preserves the LongString.
		Input:	ls - LongString to operate on.
		Output:	arg - Pascal string of the first argument.
*/

pascal void LSStrLS(ConstStr255Param s, LongString *ls);
/*	Converts a pascal string to a LongString.
		Input:	s - Pascal string.
		Output:	ls - A pointer to a longstring containing s.
*/

pascal void LSConcatLSAndStr(const LongString *ls, StringPtr s, LongString *out);
/*	Concatenates a LongString and a string. More efficient if out = ls.
		Input:	ls - LongString
					s - string
		Output:	out - ls + s. out can be ls.
*/

pascal void LSConcatStrAndLS(ConstStr255Param s, const LongString *ls, LongString *out);
/*	Concatenates a string and a LongString
		Input:	s - string
					ls - LongString
		Output:	out - s + ls. out can be ls.
*/

pascal void LSConcatStrAndStr(ConstStr255Param s1, ConstStr255Param s2, LongString *out);
/*	Concatenates two strings into a LongString.
		Input:	s1, s2 - string
		Output:	out - s1 + s2
*/

pascal void LSConcatLSAndLS(const LongString *ls, const LongString *ls2, LongString *out);
/*	Concatenates two LongStrings. More efficient if out = ls.
		Input:	ls, ls2 - LongStrings
		Output:	out - ls + ls2
*/

pascal void LSConcatLSAndStrAndLS(const LongString *ls1, ConstStr255Param s, const LongString *ls2, LongString *out);
/*	Concatenates two longstrings and a string together. More efficient if out = ls1.
		Input:	ls1, ls2 - LongStrings
					s - String
		Output:	out - ls1 + s + ls2
*/

pascal void LSConcatLSAndStrAndStr(const LongString *ls1, ConstStr255Param s, ConstStr255Param s2, LongString *out);
/*	Concatenates a LongString and two strings. More efficient if out = ls1.
		Input:	ls1 - LongString
					s, s2 - Strings
		Output:	out - ls1 + s + s2
*/

pascal void LSConcatStrAndLSAndStr(ConstStr255Param s1, const LongString *ls, ConstStr255Param s2, LongString *out);
/*	Concatenates two strings and a LongString.
		Input:	s1, s2 - strings
					ls - LongString
		Output:	out - s1 + ls + s2
*/

pascal void LSConcatStrAndStrAndStr(ConstStr255Param s1, ConstStr255Param s2, ConstStr255Param s3, LongString *out);
/*	Concatenates three strings.
		Input:	s1, s2, s3: strings
		Output:	out - s1 + s2 + s3
*/

void LSStrCat(short n,LongString *out, ...);
/*	Concatenates many pascal strings together. Not as optimal for small numbers of strings as existing functions.
		Input:	n - number of strings to concatenate
					out - LongString to concatenate into
					╔ - list of pascal strings. MAKE SURE YOU PASS STRINGS AND NOTHING ELSE, OR YOU WILL HAVE PROBLEMS.
		Output:	out - pascal strings concatenated
*/

pascal void LSInsertStr(ConstStr255Param s, short insat, LongString *ls);
/*	Inserts a string into a LongString
		Input:	s - string to insert
					insat - position to insert at
					ls - LongString
		Output:	ls - LongString
*/

pascal char LSInsertChar(char c, short p, LongString *ls);
pascal char LSInsertShort(short s, short p, LongString *ls);
pascal char LSInsertLong(long l, short p, LongString *ls);
/*	Inserts one/two/four characters into a LongString.
		Input:	c, l, s - one/two/four characters to insert.
					p - position to insert at
					ls - LongString
		Output:	return value: true if character(s) succesfully inserted into LongString, false if not.
*/


pascal void LSGetIndString(LongString *ls, short id, short num);
/*	LongString equivilant of GetIndString(). Retrieves a STR# string.
		Input:	ls - LongString to write into
					id - resource ID
					num - string index number
		Output:	ls - LongString representation of STR# string
*/

pascal OSErr LSParamString(LongString *ls, ConstStr255Param str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3);
/*	Takes a string with tokens and replaces them with strings, and dumps the result into a LongString.
		Input:	ls - LongString
					str - String with tokens. String may contain up to four tokens, ^0, ^1, ^2, and ^3. It is not necessary to use all of them.
					r0, r1, r2, r3 - replacment strings for ^0, ^1, ^2, and ^3. You may pass nil if that token does not appear in your string.
		Output:	ls - LongString representation of str with tokens replaced.
					return value: Error condition. 0 = no error.
		Note:	This function assumes str is a Str255.
*/

pascal OSErr LSParamLS(LongString *ls, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3);
/*	Same as LSParmString(), except ls is the input and output string.
*/

#define LSAppend1(ls,ch){(ls).len++;(ls).data[(ls).len]=ch;}
#define LSAppend2(ls,sh){(ls).len+=2;*(short*)&(ls).data[(ls).len-1]=sh;}
#define LSAppend3(ls,lo){(ls).len+=3;*(long*)&(ls).data[(ls).len-2]=lo;}
#define LSAppend4(ls,lo){(ls).len+=4;*(long*)&(ls).data[(ls).len-3]=lo;}
/*	Appends 1, 2, 3, or 4 characters to a longstring.
		Input:	ls - LongString
					ch/sh/lo - characters to append
		Output:	ls - LongString with ch/sh/lo appended
		Notes:	е LSAppend3 appends four characters, but only increases length by three
					е MAKE SURE you don't overrun the end of the LongString BEFORE you use these, although
						with 600 characters of storage space, this isn't too likely.
					е This is the fastest way to append 1-4 characters to a LongString
*/

#define LSMakeStr(ls) {if((ls).len>255) (ls).data[0]=255; else (ls).data[0]=(ls).len;}
/*	Given a LongString ls, makes ls.data a valid Str255.
		Input:	ls - LongString
		Output:	ls - LongString with data[0] set to min(ls.data, 255)
*/

#pragma mark е Plugin Services
/*	------------------------------------------------------------------------------------------
		Plugin Services

		
		------------------------------------------------------------------------------------------
*/

pascal char NewService(FourCharCode serviceType);
/*	Sets up your plugin as a particular service. Only one plugin can register for any one service.
		Input:	serviceType - the type of service
		Output:	return value: 1 if the service was registered, 0 if not.
*/

pascal char ExistsService(FourCharCode serviceType);
/*	Checks to see if a service exists.
		Input:	serviceType - the type of service
		Output:	return value: 1 if the service was registered, 0 if not.
*/

pascal short HMIAdd(ConstStr63Param name);
/*	Adds an item to the help menu
		Input:	name - string to add
		Output:	return value: reference number.
*/

pascal short WMSGetMenuItemNum(long key);
/*	Returns the item number for a menu item in the Windows menu.
		Input:	key - a unique value
		Output:	-1 if not in menu, otherwise, the menu item number
*/

pascal short WMSListAdd(long key, ConstStr255Param name);
/*	Adds a menu item to the services section of the Window menu
		Input:	key - a unique value
					name - The name of the menu item
		Output:	return value: The item number of the menu item. Use to set cmd-key shortcuts, etc.
		Note:	Do not add more than one menu for any one key.
*/

pascal long RegisterAETE(Handle aete);
/*	Registers your plugin's aete resource (if any) with ShadowIRC for AppleScripting.
		Input: 	aete - a handle to your plugin's aete resource.
		Output:	return value: >= 0 if successful, -1 if not.
		Note:	Your aete resource must be loaded into memory and market as non-purgeable. Do not release the resource.
*/

#pragma mark е Inter-Plugin Communication
/*	------------------------------------------------------------------------------------------
		Inter-Plugin Communication

		These plugins allow simple data transfer between plugins.
		------------------------------------------------------------------------------------------
*/

pascal short IPCRegister(FourCharCode IPCType, long version, long data);
/*	Registers your plugin to recieve inter-plugin communication messages.
		Input:	IPCType - unique code for message classes
					version - a version number for the class
					data - any data you wish. You cannot modify it once you've called IPCRegister().
		Output:	return value: 0 if successfully registered. 1 if the same version is registered. 2 if a newer version is registered. 3 if an older version is registered.
*/

pascal char IPCExists(FourCharCode IPCType, long *version, long *data);
/*	Checks to see if a plugin is listening for a given IPC message class.
		Input:	IPCType - unique code for message classes
					version - ptr to a long, or nil if you don't want the version number
					data- ptr to a long, or nil if you don't want the data.
		Output:	return value: true if a plugin is registered for these messages, false if not.
					version, data - if a plugin is regstered, the version and data from IPCRegister(). If not, version and data are unchanged.
*/

pascal short IPCSend(FourCharCode IPCType, long message, long data, long *result);
/*	Sends an IPC message to the plugin listening for a IPC class.
		Input:	IPCType - the message class
					message - the message identifier
					data - the message's data
		Output:	result - a return value from the plugin handling the IPC message.
					return value: 1 if no error. 0 if the IPC type isn't registered. -1 if the IPC service didn't process the message.
*/

pascal char IPCReply(void* client, FourCharCode IPCType, long message, long data, long *result);
/*	Allows an IPC service to send a message to its clients.
		Input:	client - client to send to. Use the client field from pIPCMessageData.client.
					IPCType - the message class
					message - the message identifier
					data - the message's data
		Output:	result - return value from client plugin you are calling, or nil if the message was not received
					return value: true if message was sent, false if not.
*/

#pragma mark е Message Window Display Functions
/*	------------------------------------------------------------------------------------------
		Message Window Display Functions

		These functions handle displaying text in message windows.
		------------------------------------------------------------------------------------------
*/

pascal void SMPrefix(LongString *ls, short where);
/*	Adds the server message prefix to the string, and optionaly displays it.
		Input:	ls - the text
					where - what to do with the text
		Output:	ls - The text with color and server message prefix added
*/

pascal void SMPrefixColor(LongString *ls, short where, short color);
pascal void SMPrefixIrcleColor(LongString *ls, short where, short color);
pascal void SMPrefixRGBColor(LongString *ls, short where, RGBColor *rgb);
/*	Displays a message with the server message prefix and ShadowIRC or Ircle color you specify.
		Input:	ls - the text
					where - what to do with the text
					color - which color to use
					rgb - an RGB color to use.
		Output:	The text with color and server message prefix added
		Note:	е SMPrefix() is functionally equivilant to SMPrefixColor() with a color of sicServer.
					е For SMPrefixIrcleColor(), the Ircle color should be a character, i.e. '0', '1', ':', etc.
					е SMPrefixIrcleColor() temporarally sets mainPrefs->colorMethod to cmIrcle when it displays
						the message, as long as where != dsNowhere.
					е SMPrefixRGBColor() uses the ShadowIRC color code for custom RGB values (sicCustomColor).
						Only the high eight bytes of the red, green, and blue values are significant.
*/

pascal char SMLink(linkPtr link, LongString *ls);
/*	Checks link against the current target and prepends "[server name]" to the LongString if the links are different.
		Input:	link - link the message is for
					ls - LongString with message
		Output:	ls - LongString
					return value: true if [servername] was prepended, false if not.
*/
 
pascal char SMPrefixLink(linkPtr link, LongString *ls, short where);
pascal char SMPrefixLinkColor(linkPtr link, LongString *ls, short where, short color);
pascal char SMPrefixLinkIrcleColor(linkPtr link, LongString *ls, short where, short color);
pascal char SMPrefixRGBLinkColor(linkPtr link, LongString *ls, short where, RGBColor *rgb);
/*	Same as SMPrefix*() functions, but with an added call to SMLink().
		return value:	return value from SMLink().
*/

pascal void LineMsg(LongString *ls);
/*	Displays a message in console window
		Input:	ls - LongString to display in console window
		Output:	none
*/

pascal void ChMsg(channelPtr ch, LongString *msg);
/*	Displays a message in a channel.
		Input:	ch - Channel to display to
					msg - LongString to display
		Output:	none
*/

pascal void ChannelMsg(linkPtr link, ConstStr255Param windowName, LongString *msg);
/*	Finds the "best" window to display a message to.
		Input:	link - link message is for
					windowName - Window to display to. Either a channel, or a query window name.
					msg - LongString to display
		Output:	none
*/

pascal void Message(LongString *msg);
/*	Displays a message in the current window.
		Input:	msg - LongString to display.
		Output:	none
*/


#pragma mark е Text Input
/*	------------------------------------------------------------------------------------------
		Text Input Functions

		These functions handle various ways of getting ShadowIRC to process text.
		------------------------------------------------------------------------------------------
*/

pascal void GetLine(char action, MWPtr window);
/*	Processes the text in the inputline as if the user hit return.
		Input:	action - True if opt-return was held down and we're turning the text into an action
					window - The Message Window to direct the text to. nil == sidr->CurrentTarget->mw
		Output:	none
		Note:	DON'T call GetLine() while you're handling an pInputMessage. GetLine() calls ProcessLine(),
					which calls plugins immediately before it does anything else. Calling GetLine() or ProcessLine()
					while processing an pInputMessage will lead to infinite recursion unless you know what you're
					doing.
*/

pascal void InputHandler(LongString *ls, targetPtr targ);
/*	Processes a string as input for the current target. Can be a command.
		Input:	ls - Input text
					targ - the target to input to
		Output:	none
*/

pascal void ProcessLine(LongString* line, char stackup, char action, MWPtr mw);
/*	Processes a line of text. Can be a command.
		Input:	line - text to process.
					stackup - if true, adds to inputline history unless line begins with "//"
					action - if true, treat as an action
					mw - target of the input
		Output:	none
		Note:	DON'T call while you're handling an pInputMessage. ProcessLine() calls plugins immediately before
					it does anything else. Calling ProcessLine() while processing an pInputMessage will lead to infinite
					recursion unless you know what you're doing.
*/

pascal char ProcessInputHunk(Ptr data, long length, MWPtr mw, char clearEnd, long *lastCR);
/*	Processes a quantity of <cr> delimited text lines.
		Input:	data - pointer to text to process
					length - the length of the data
					mw - window so send input to
					clearEnd - if false and the last line of text doesn't have a terminating <cr>, it is put in the inputline;
									otherwise, it is processed.
		Output:	lastCR - if not nil, the offset of the last return, or -1 if no returns found.
					return value: true if all text processed, false if not.
*/

pascal void LockInput(void);
/*	Prevents the user from inputting text to the inputline or any message window.
		Input:	none
		Output:	none
*/

pascal void UnlockInput(void);
/*	Allows the user to input text to the inputline or message windows.
		Input:	none
		Output:	none
*/

#pragma mark е Inputline
/*	------------------------------------------------------------------------------------------
		Inputline Functions

		These functions handle various inputline related tasks.
		------------------------------------------------------------------------------------------
*/

pascal iwWidgetPtr IWNewWidget(long type, short align, short width);
/*	Creates a new inputline widget, adds it to the list, and calls IWRecalculateRects().
		Input:	type - a value of your choosing
					align - see enum iwAlign
					width - number of pixels to request
		Output:	return value: the inputline widget you created
*/

pascal char IWDeleteWidget(iwWidgetPtr widget);
/*	Deletes an inputline widget
		Input:	widget - widget to delete
		Output:	true if the widget was deleted, false if not.
*/

pascal void IWRecalculateRects(void);
/*	Recalculates the displayArea of each inputline widget based on the inputline width and givenWidth in each inputline widget.
		Input:	none
		Output:	none
*/

pascal long IWPopUpMenu(Point pt, MenuHandle m, long curItem);
/*	Handles popup menus on the inputline.
		Input:	pt - top left corner of the menu
					m - the menu to display
					curItem - item number to select
		Output:	return value: high two bytes- menu ID. low two bytes- item number
*/

pascal short IWOverride(long type, iwWidgetPtr *widget);
/*	Allows you to override a ShadowIRC inputline widget.
		Input:	type - type of widget to override
		Output:	widget - if IWOverride() successful, the widget you overrode. If not, nil.
					return value: error condition. See enum iwOverrideErrors
*/

pascal char ILGetLine(MWPtr mw, LongString *ls);
/*	Gets the inputline of a given window.
		Input:	mw - The message window to get the input from, or nil for the frontmost input.
		Output:	ls - The text of the inputfield.
					return value: true if an input field was found, false if not.
*/

pascal char ILSetLine(MWPtr mw, LongString *ls);
/*	Sets the inputline of a given window.
		Input:	mw - The message window to set the input for, or nil for the frontmost input.
		Output:	ls - The text to set the inputfield to.
					return value: true if an input field was found, false if not.
*/

/*	Depreciated Functions:
		These function still work now, however their use is depreciated since ShadowIRC now has the ability
		to put the inputline inside message windows.
*/

pascal void UpdateStatusLine(void);
/*	Updates the status line
		Input:	none
		Output:	none
*/

pascal void SetInputLineCursorSelection(long start, long finish);
/*	Sets the position of the selection on the input line.
		Input:	start - The start of the selection.
					finish - The end of the selection. If start == finish, insertion point is placed at start.
		Output:	none
*/

pascal void GetInputLineCursorSelection(long *start, long *finish);
/*	Gets the selection in the inputline.
		Input:	none
		Output:	start - The start of the selection.
					finish - The end of the selection.
*/

pascal void GetInputLine(LongString *line);
/*	Gets the text in the input line.
		Input:	none
		Output:	line - LongString containing the text of the input line.
*/

pascal void SetInputLine(LongString *ls);
/*	Sets the text in the input line.
		Input:	ls - LongString for inputline.
		Output:	none
*/


#pragma mark е Windows
/*	------------------------------------------------------------------------------------------
		Windows
		
		е Dialogs are bad, m'kay? Use windows instead.
		
		е NEVER create dialogs or windows behind ShadowIRC's back, since the only way you
		can get events from it is through ShadowIRC.
		
		е DO NOT change the a dialog or window's refCon, since it's a pointer to a struct that tells ShadowIRC
		where the window came from. Use GetWindowProperty()/SetWindowProperty() instead.
		
		е NEVER delete dialogs or windows behind ShadowIRC's back, since the refCon is a pointer
		to a struct containing information about where the window came from. Although small,
		disposing the dialog on your own will leave that memory allocated. (DON'T call DisposeDialog,
		for example.)
		
		е List of Window Manager functions to avoid:
			NewWindow() and variants, DisposeWindow(),	SelectWindow()
*/

WindowPtr pluginNewWindow(const Rect *boundsRect, ConstStr255Param title, WindowAttributes flags, char isFloater);
/*	Creates a new window for a plugin.
		Input:	boundsRect - rectangle for window
					title - name of window
					flags - flags used to specify appearance of window
					isFloater - true if a floating window, false if not
					
		Output:	return value: WindowPtr from CreateWindow()
		Note:	е The window is not displayed until you explicitly do so.
*/

pascal void pluginDisposeWindow(WindowPtr w);
/*	Deletes a window created by a plugin.
		Input:	w - window to delete
		Output:	none
*/

pascal void* GetPluginWindowOwner(WindowPtr w);
/*	Gets the plugin owner of a window.
		Input:	w- window to get owner of
		Output:	return value: pluginRef of owner, or nil if none.
*/

pascal char WIsFloater(WindowPtr w);
/*	Determines if a window is a floating window or not.
		Input:	w - WindowPtr of window to test
		Output:	return value: true if the window floats, false if not
*/

pascal void WSelect(WindowPtr w);
/*	Brings a window to the front and activates it. (Replacment for SelectWindow())
		Input:	w - WindowPtr of window to activate
		Output:	none
*/

Rect WGetBBox(WindowPtr w);
/*	Gets the bounding rectangle for a valid window, including titlebar
		Input:	w - window
		Output:	return value: Rect of window
*/

pascal char StandardDialogFilter(DialogPtr d, EventRecord *e, short *item);
/*	ShadowIRC's standard ModalDialog() filter. Call directly from your filter, or as a
		UPP in a ModalDialog(). Provides update event support for other windows,
		network activity, and maps enter/return to the default dialog item (or item #1) and esc/cmd-period
		to the default cancel item (or item #2).
		Output:	true if the event was completely processed, false if not. If calling from your dialog filter,
					you should call this first, and process only if it returns false.
*/ 

//pascal void NavDialogFilter(const NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, NavCallBackUserData callBackUD)
pascal void NavDialogFilter(const long callBackSelector, Ptr callBackParms, long callBackUD);
/*	ShadowIRC's standard Navigation Services dialog filter. Call directly from your filter, or as a
		UPP from a Navigation Services API call.
		Provides update event support for other windows. Does NOT allow network activity.
*/

pascal void MWPart(MWPtr mw);
/*	Closes a message window. Same as hitting cmd-w on a window.
		Input:	mw - The message window to close
		Output:	none
*/

#pragma mark е Appearance Manager
/*	------------------------------------------------------------------------------------------
		Appearance Manager abstractions.

		------------------------------------------------------------------------------------------
*/

pascal void SetTextColor(short color);
/*	Emulates SetThemeTextColor() if Appearance is not present, or calls through if it is.
		Input:	color - Appearance Manager text color
		Output:	none
*/

pascal void SetBackground(short color);
/*	Emlates SetThemeBackground() if Appearance is not present, or calls through if it is.
		Input:	color - Appearance Manager theme pen
		Output:	none
*/

pascal void DrawBorder(const Rect *r, long state, char draggable);
/*	Draws a border.
		Input:	r - Rect to draw in
					state - kThemeStateActive, kThemeStatePressed, kThemeStateInactive
					draggable - if true, draw border with dragging indicator
		Output:	none
*/

pascal ListHandle GetAppearanceListBoxHandle(DialogPtr theDialog, short itemNum);
/*	Gets the ListHandle of an Appearance list control.
		Input:	theDialog - the dialog of the list control
					itemNum - the item number of the list control
		Output:	the ListHandle, or null if failed
*/

#pragma mark е Menus
/*	------------------------------------------------------------------------------------------
		Menus

		------------------------------------------------------------------------------------------
*/

pascal void CMAdd(pCMPopupsDataPtr m, ConstStr255Param item, long id);
/*	Adds an item to a contextual menu before displaying it.
		Input:	m - sidr->msgData.
					item - the name of the menu item
					id - a unique id that will be returned to you if the user selects an item.
		Output:	none
*/

pascal void CMAddSubmenu(pCMPopupsDataPtr d, ConstStr255Param item, short *id, MenuHandle *m);
/*	Adds a submenu to a contextual menu.
		Input:	d - sidr->msgData.
					item - Name of the menu item.
					id - 
					m - 
		Output:	id - 
					m - 
*/

pascal void CMSetCheckmark(pCMPopupsDataPtr d, long id, char checked);
/*	Sets/removes the checkmark of an item the contextual menu.
		Input:	d - sidr->msgData.
					id - The id of the item you want to change
					checked - True if checked, false if not.
		Output:	none
*/

#pragma mark е Drag and Drop
/*	------------------------------------------------------------------------------------------
		Drag and Drop

		These functions are helper functions for implementing Drag and Drop in plugins.
		------------------------------------------------------------------------------------------
*/

pascal void DragHilightRect(const Rect *rect, DragReference drag);
/*	Draws the drag hilite for a rect. Hide th HideDragHilite().
		Input:	rect - Rectangle to hilite.
					drag - the drag reference
		Output:	none
*/

pascal void DragHilightRectBG(const Rect *rect, DragReference drag, char hilite);
/*	Draws or hides the drag hilite for a rect on a standard grey background.
		Input:	rect - Rectangle to hilite.
					drag - the drag reference
					hilite - true if show hilite, false if remove it
		Output:	none
*/

pascal char DragIsTypeAvail(DragReference drag, long type);
/*	Checks to see if a particular drag flavor is in the drag.
		Input:	drag - the drag
					type - the flavor to search for
		Output:	return value: true if the flavor is in the drag, false if not.
*/

pascal char DragGetType(DragReference drag, long type, void* *data, long *size);
/*	Retrieves a drag flavor.
		Input:	drag - the drag
					type - the flavor to get
					data - the address of a pointer.
		Output:	data - a new pointer to the data extracted from the drag
					size - the size of the pointer created
					return value: true if flavor found, false if not.
		Note:	е You MUST DisposePtr() the data you get from this function, or you will have a memory leak.
*/

pascal char DragGetPtr(DragReference drag, long type, void* *data);
/*	Retrieves a drag flavor that contains (only) a pointer.
		Input:	drag - the drag
					type - the drag flavor
		Output:	data - the contents of the drag flavor.
					return value: true if successful, false if not.
		Note:	е Use this for drag flavors that contain only pointers, i.e. kMWinFlavor, kUserFlavor
*/

pascal char DragAddPtr(DragReference drag, ItemReference item, long type, void* data, long flags);
/*	Adds a drag flavor that contains (only) a pointer.
		Input:	drag - the drag
					item - the drag item
					type - the flavor type
					data - the pointer to add
					flags - DragManager flags.
		Output:	return value: true if successful, false if not.
		Note:	е It is *HIGHLY* reccommended that you set this item's flags to flavorSenderOnly,
						since it is generally a Bad Idea to pass pointers to other processes.
*/


#pragma mark е Message Windows
/*	------------------------------------------------------------------------------------------
		Message Window Functions

		
		------------------------------------------------------------------------------------------
*/

pascal MWPtr NewPluginMWindow(ConstStr255Param title);
/*	Creates a new message window. Window is NOT automatically visible. (Use WShow())
		Input:	title - name of window. Title is quoted with underscores.
		Output:	return value: MWPtr for created message window.
		Notes:	е Don't EVER change the 'magic' variable in a MWPtr. If you do, ShadowIRC will not be able
					to tell that the window is a message window.
					е Don't EVER change the window's refCon, since it contains the MWPtr for the window.
*/

void MWSetDimen(MWPtr window, short left, short top, short width, short height);
/*	Sets a message window's size and position.
		Input:	window - the message window
					left - left coord
					top - top coord
					width - width of window
					height - height of window
		Output:	none
*/

pascal void MWSetFontSize(MWPtr window, short font, short size);
/*	Sets the font and size of a message window.
		Input:	window - the window
					font - a font number, or -1 to leave unchanged.
					size - a font size, or -1 to leave unchagned.
		Output:	none
*/

pascal void MWDelete(MWPtr window);
/*	Deletes a message window.
		Input:	window - MWPtr to close and destroy.
		Note:	е Don't use this on any message window other than the ones you've created with
					NewPluginMWindow(), or Bad Things can happen.
*/

pascal void MWMessage(MWPtr window, LongString *msg);
/*	Puts text into a message window.
		Input:	window - message window to put text into.
		Output:	msg - text
*/

pascal MWPtr MWFromWindow(WindowPtr w);
/*	Takes a window, returns the message window.
		Input:	w - window
		Output:	return value: The MWPtr for w, or nil.
*/

pascal void MWColor(LongString *text, short colorNum);
pascal void MWIrcleColor(LongString *text, short colorNum);
/*	Sets the primary color of a LongString by prepending a ShadowIRC internal color code or an Ircle color code.
		Input:	text - LongString
					colorNum - the color number
		Output:	text - colorized LongString.
		Note:	е╩Passing a LongString to something that calls MWMessage() without colorizing it sets the
					primary color to black.
					е For MWIrcleColor(), the color should be a character, i.e. '0', '1', ':', etc.
*/

pascal void DrawMWinStatus(MWPtr mw);
/*	Forces a message window to update its status line.
		Input:	mw - Message window to update
		Output:	none
		Note:	е Be careful calling this from handling pMWWidgetUpdateMessage. See the notes in the sample code for numusers.
*/

pascal mwWidgetPtr MWNewWidget(MWPtr mw, long type, short align, short width);
/*	Creates a new message window widget, adds it to the window, and calls MWRecalculateRects().
		Input:	mw - The message window
					type - a value of your choosing
					align - see enum mwWidgetAlign
					width - number of pixels to request
		Output:	return value: the message window widget you created, or nil if a widget of that type was already created.
*/

pascal void MWRecalculateRects(MWPtr mw);
/*	Recalculates the widths of Message Window widgets.
		Input:	mw - The message window
		Output:	none
*/

pascal void MWDestroyWidget(mwWidgetPtr widget);
/*	Deletes a message window widget.
		Input:	widget - widget to destroy
		Output:	none
*/

pascal mwWidgetPtr MWFindWidget(MWPtr mw, long type);
/*	Finds a message window widget.
		Input:	mw - message window to search
					type - widget type to search for
		Output:	return value: the widget found, or nil.
*/

pascal mwPanePtr MWNewPane(MWPtr mw, long type, short align, short height, short width);
/*	Creates a new message window pane
		Input:	mw  - the message window
					type- a type for the pane. Types must be unique.
					align - alignment for the pane (see enum mwPaneAlign)
					height - requested height for top/bottom aligned panes, or -1.
					width - requested width for left/right aligned panes, or -1.
		Output:	return value: the created pane, or nil (type already existed)
*/

pascal mwPanePtr MWFindPane(MWPtr mw, long type);
/*	Finds a message window pane of a given type.
		Input:	mw - the message window
					type - the type to search for
		Output:	return value: the found pane, or nil
*/

pascal void MWPaneRecalculate(MWPtr mw);
/*	Recalculates message window pane sizes. Call this if you change the height/width your pane is asking for. Then, call MWPaneResize()
		Input:	mw - message window
		Output:	none
		Note:	е You MUST call MWPaneResize() *IMMEDIATELY* after calling this function.
*/

pascal void MWPaneResize(MWPtr mw);
/*	Calls each pane's resize function. You MUST call this *IMMEDIATELY* after calling MWPaneRecalculate().
		Input:	mw - message window
		Output:	none
*/

pascal void MWPaneUpdate(MWPtr mw);
/*	Calls each pane's update function. Forces the message window to redraw. This is not usually necessary.
		Input:	mw - message window
		Output:	none
*/

pascal char MWDestroyPane(mwPanePtr pane);
/*	Destroys a pane.
		Input:	pane - the pane to destroy
		Output:	return value: true if pane was destroyed. False if not.
		Note:	е You can only destroy a pane you created.
*/

pascal ConstStringPtr MWGetName(MWPtr mw, Str255 s);
/*	Gets the target name of a message window.
		Input:	mw - the mesage window
		Output:	s - if not nil, copies the name into it.
					return value: a pointer to the name. If nil, couldn't find name.
*/

pascal channelPtr MWGetChannel(MWPtr mw);
/*	Gets the channel of a message window, or returns nil if no channel.
		Input:	mw - the window
		Output:	return value: channel of window, or nil
*/

pascal connectionPtr MWGetDCC(MWPtr mw);
/*	Gets the DCC connection of a message window, or returns nil if no DCC.
		Input:	mw - the window
		Output:	return value: dcc connection of window, or nil
*/

MWPtr GetActiveMW(void);
/*	Gets the active message window
		Input:	none
		Output:	return value: the active message window, or nil if no active message window.
*/

MWPtr GetFrontMW(void);
/*	Gets the frontmost message window.
		Input:	none
		Output:	return value: the frontmost message weindow.
*/

#pragma mark е Channels
/*	------------------------------------------------------------------------------------------
		Channel Functions

		
		------------------------------------------------------------------------------------------
*/

pascal char IsChannel(ConstStr255Param s);
/*	Returns true if the string is a channel. False if not.
		Input:	s - Pascal string
		Output:	return value - 1 if s is a channel. 0 if not.
*/

pascal char IsDCCName(ConstStr255Param s);
/*	Returns true if the string is a DCC window name ("=nickname="), false if not.
		Input:	s - pascal string
		Output:	return value: true if dcc window name, false if not.
*/

pascal void MakeChannel(Str255 s);
/*	Makes the string a channel by adding '#' if necessary.
		Input:	s - Pascal string
		Output: s - Pascal string
*/

pascal channelPtr ChFind(ConstStr255Param name, linkPtr link);
/*	Finds the channel information for a channel name.
		Input:	name - Pascal string.
					link - the link to search in. You can pass nil to have it search the current link.
		Output:	return value: channelPtr to channel information
*/

pascal bansP ChFindBan(channelPtr ch, ConstStr255Param n, char exceptionList);
/*	Finds a ban in a channel's banlist.
		Input:	ch - A channel.
					n - Pascal string - The ban to find.
					exceptionList - searches the exception list, rather than the bans list.
		Output:	return value - A pointer to a bansRec if found, or null if not.
*/

pascal bansP ChMatchBan(channelPtr ch, ConstStr255Param n, char exceptionList);
/*	Finds a ban matching a mask in a channel's banlist.
		Input:	ch - A channel
					n - Pascal string - The mask to match against.
					exceptionList - searches the exception list, rather than the bans list.
		Output:	return value - A pointer to a bansRec if matched, or null if not.
*/

pascal char ChGetBan(channelPtr ch, ConstStr255Param from, bansP *f, char exceptionList);
/*	Search for and create, if necessary, a ban in a channel.
		Input:	ch - A channel
					from - Pascal string - The ban to find/add.
					exceptionList - searches the exception list, rather than the bans list.
		Output:	f - If f!=0, on input, a pointer to a bansRec for the matching/added ban.
					return value - True if ban was already in banlist. False if not.
*/

pascal void ChKillBan(channelPtr ch, bansP *theBan, char exceptionList);
/*	Delete a ban in a channel's banlist.
		Input:	ch - A channel
					theBan - a pointer to the bansP you want to delete.
					exceptionList - searches the exception list, rather than the bans list.
		Output:	theBan - nil
*/

pascal void ChKillAllBans(channelPtr ch, char exceptionList);
/*	Deletes all bans in a channel's banlist.
		Input:	ch - A channel
					exceptionList - searches the exception list, rather than the bans list.
		Output:	none
*/

pascal UserListPtr ULAddUser(channelPtr channel, ConstStr255Param nick, ConstStr255Param userhost);
/*	Adds a user to a channel's userlist. It's not reccommended you call this.
		Input:	channel - A channel
					nick - User's name
					userhost - User's user@host
		Output:	return value: a pointer to the userlist entry.
*/

pascal void ULDeleteUser(UserListPtr user);
/*	Deletes a user from a channel's userlist. It's not reccommended you call this.
		Input:	user - Pointer to the user's userlist entry.
		Output:	none
*/

pascal UserListPtr ULFindUserName(channelPtr channel, ConstStr255Param nick);
/*	Finds a user in a channel.
		Input:	channel - A channel
					nick - The name of the user.
		Output:	return value: Pointer to userlist record, or nil if not found.
*/

pascal MWPtr ChannelWindow(linkPtr, ConstStr255Param name);
/*	Gets the message window for a channel/query window name.
		Input:	link - link to search
					name - Name of the window.
		Output:	return value: Message Window pointer.
*/

pascal char IsChannelValid(channelPtr ch);
/*	Checks to see if a channel exists.
		Input:	ch - channelPtr to channel.
		Output:	return value: true if ch is in the current channel list, false if not.
*/

pascal char IsUserValid(UserListPtr u);
/*	Checks to see if a user exists.
		Input:	u - UserListPtr for user.
		Output:	return value: true if u exists, false if not.
*/

pascal void DoModeLWindow(channelPtr ch, LongString *ls);
/*	Opens a dialog to set the channel limit.
		Input:	ch - channel
					ls - LongString with "MODE +l" already in it.
		Output:	ls - empty string if dialog canceled, or new limit appended.
*/

pascal void DoModeKWindow(channelPtr ch, LongString *ls);
/*	Opens a dialog to set the channel key.
		Input:	ch - channel
					ls - LongString with "MODE +k" already in it.
		Output:	ls - empty string if dialog canceled, or new key appended.
*/


#pragma mark е Connections
/*	------------------------------------------------------------------------------------------
		Connection Functions
		
		Network-related control functions.
		------------------------------------------------------------------------------------------
*/

pascal connectionPtr pluginNewConnection(char textOrBinary);
/*	Creates a new plugin TCP/IP connection.
		Input:	textOrBinary - true if text, false if binary.
		Output:	return value: connection, or nil if an error occured in creation.
		Notes:	е If you create a text connection, you send and recieve data as "text"; the text
						is run through an encoder/decoder as necessary.
					е If you create a data plugin, you are responsible for recieving your own data using the
						ConnCharsAvail() and ConnGetData() functions.
*/

pascal void deleteConnection(connectionPtr *conn);
/*	Deletes a connection and, if necessary, closes the TCP connection.
		Input:	conn - connection to close
		Output:	conn - nil
		Note:	е You should ONLY use this function on connections you created.
*/

pascal connectionPtr findConnectionSock(long sock);

pascal void ConnClose(connectionPtr conn);
/*	Attempts an orderly close of a TCP/IP connection.
		Input:	conn - connection to close
		Output:	none
*/

pascal char ConnNewActive(connectionPtr conn);
/*	Attempts to open a TCP/IP connection. Uses the ip and port values in the connection.
		Input:	conn - connection to open
		Output:	return value: true if the attempt to open was successful, false if the attempt failed for one reason or another.
*/

pascal char ConnNewListen(connectionPtr conn);
/*	Attempts to open a passive (incoming) TCP/IP connection. Uses the ip and port values in the connection.
		Input:	conn - connection to open
		Output:	return value: true if the attempt to open was successful, false if the attempt failed for one reason or another.
		Note:	е Set conn->ip to a specific IP to listen for connection attempts from a specific IP, or set it to zero to
					listen for connection attempts from any IP.
*/

pascal void ConnPut(connectionPtr *conn, const void* p, long len);
/*	Puts raw data over a connection.
		Input:	conn - ptr to conection to send over
					p - pointer to data to send. (If dereferencing a handle, make *SURE* it's locked)
					len - the length of the data to send
		Output:	none
*/

pascal void ConnPutText(connectionPtr *conn, void* p, long len);
/*	Sends data through a text connection after encoding it. Use this for text connection.
		Input:	conn - ptr to conection to send over
					p - pointer to data to send. (If dereferencing a handle, make *SURE* it's locked)
					len - the length of the data to send
		Output:	p - A character-translated version of the outgoing string.
		Note:	You may need to append a linefeed to your text so that it is processed by the remote server.
*/

pascal void ConnPutLS(connectionPtr *conn, LongString *ls);
/*	Sends a LongString through a text connection. Appends a linefeed and encodes text before sending.
		Input:	conn - ptr to conection to send over
					ls - LongString containing text to send
		Output:	none
*/

pascal OSErr ConnGetData(connectionPtr conn, Ptr data, short len);
/*	Get data from a connection.
		Input:	conn - The connection
					data - Ptr to memory where data will be recieved to.
					len - number of bytes to recieve
		Output:	return value: MacTCP error
*/

pascal unsigned short ConnGetLocalPort(connectionPtr conn);
/*	Gets the local port for a connection.
		Input:	conn - the connection.
		Output:	return value: the local port.
*/

pascal void putServer(linkPtr link, LongString *ls);
/*	Sends a line to the IRC server.
		Input:	ls - LongString to send to server.
		Output:	none
*/

pascal void SendCTCPReply(linkPtr link, ConstStr255Param target, ConstStr255Param command, LongString *args);
/*	Sends a CTCP reply.
		Input:	link - the link to send the CTCP through
					target - Where to send the reply.
					command - The CTCP command you are replying to.
					args - Additional information, for example, version reply, CPING response time, etc.
		Output:	none
*/

pascal char OpenConnection(linkPtr link);
/*	Opens a connection to an IRC server.
		Input:	link - link to connect to
		Output:	return value: true if ShadowIRC started to open the connection, false if not
*/

pascal void DoServer(linkPtr link, LongString *rest, const LongString* reason);
/*	Changes servers. Equivilant to the /server command.
		Input:	link - The link to change servers on
					rest - a string containing the servername, and optionally the port
					reason - pointer to the reason to use if connected. If nil, uses the "Changing Servers" default reason.
		Output:	none
*/

pascal void DoSignoff(linkPtr link, const LongString* reason);
/*	Disconnects a connection. Equivilant to the /signoff command.
		Input:	link - The link to disconnect from
					reason - the reason to use. If nil, uses the default signoff reason for this link.
		Output:	none
*/

pascal void NickListAdd(linkPtr link, ConstStr255Param nick);
/*	Adds a nick to the link's nick list.
		Input:	link - The link to add the nick to.
					nick - The nick to add.
		Output:	none
*/

pascal void ChannelListAdd(linkPtr link, ConstStr255Param channel);
/*	Adds a channel to the link's channel list.
		Input:	link - The link to add the channel to.
					channel - The channel to add.
		Output:	none
*/

#pragma mark е DCC
/*	------------------------------------------------------------------------------------------
		DCC
		
		
		------------------------------------------------------------------------------------------
*/


pascal ConstStringPtr dccTypToStr(short d);
/*	Takes a DCC Type and returns a string representation.
		Input:	d - The DCC type (dccTypeRec)
		Output:	return value: String
*/

pascal ConstStringPtr dccFlagTypToStr(short d);
/*	Takes a DCC Flag and returns a string representation.
		Input:	d - The DCC flag (dccFlagsRec)
		Output:	return value: string
*/

pascal short DCCTypeFind(Str255 s);
/*	Changes a string into a DCC type.
		Input:	s - string
		Output:	return value: (dccTypeRec)
*/

pascal connectionPtr DCCFind(linkPtr link, short typ, ConstStr255Param from);
/*	Finds a DCC connection
		Input:	link - the link to search on, or nil to search all links. (ShadowIRC uses this to check for DCC chat collisions)
					typ - a dccTypeRec (dccCHAT, dccSEND, etc)
					from - the user name to search for
		Output:	return value:  connectionPtr of DCC found, or nil if not found
*/

pascal void DCCClose(connectionPtr *conn, char silent);
/*	Closes a DCC connection
		Input:	conn - pointer to connectionPtr of DCC connection
					silent - true if you want output messages disabled, false if not. Default is false.
		Output:	conn is set to nil 
*/

pascal void DCCWindowClose(Ptr mw);
/*	Closes a DCC connection and its associated window
		Input:	mw - the DCC connection's message window
		Output:	none
*/

pascal char DCCCreate(linkPtr link, short typ, ConstStr255Param from, connectionPtr *c);
/*	Creates a DCC connection. Does NOT open a DCC connection. Not reccommended that you use this.
		Input:	link - the link the user is on
					typ - The type of DCC (dccTypeRec). You can also use responses you get back from DCCTypeAdd().
					from - the nick receiving the DCC.
		Output:	c - ConnectionPtr for the DCC.
					return value: true if the DCC was created, false if it already existed.
*/

pascal void DCCRequest(linkPtr link, ConstStr255Param from, ConstStr255Param uah, Str255 s);
/*	Handles a DCC request from another user and sets it up. Use this instead of DCCCreate().
		Input:	link - the link the user is on
					from - name of user
					uah - user@host of user
					s - their DCC request
		Output:	none
*/

pascal void DCCOpen(connectionPtr *c);
/*	Opens a DCC connection.
		Input:	c - connectionPtr of DCC
		Output:	c - nil if connection closed
*/

pascal void DCCCommand(linkPtr link, Str255 s);
/*	Handles a DCC command as if typed into the inputwindow.
		Input:	link - link to use
					s - string to process
		Output:	none
*/

pascal void DCCSSay(connectionPtr *c, LongString *ls);
/*	Sends text through a DCC connection.
		Input:	c - connection to send text through.
					ls - Text to send
		Output:	c - c if no errors, nil if the connection was closed.
*/

pascal char DoDCCSend(linkPtr link, ConstStr255Param name, char sendRev);
/*	Functions as if the user typed /dcc send name. Displays a file selection dialog and attempts to send the file.
		Input:	link - link name is on
					name - name of user to dcc to
					sendRev - false for normal DCC send, true for reversed send
		Output:	return value: true if user selected a file to send, false if not.
*/

pascal char DoDCCSendFile(linkPtr link, ConstStr255Param name, const FSSpec *file, char macbinary, char sendRev);
/*	Allows you to DCC send a file by specifying the file.
		Input:	link - link the person you're sending to is on
					name - name of user to dcc to
					file - the file to send
					macbinary - true if file should be sent with MacBinary encoding, false if it should be sent straight binary.
					sendRev - false for normal DCC send, true for reversed send
		Output:	return value: true if file open and dcc creation was successful, false if not
*/

pascal char DCCGetAFile(FSSpec *file, char *macbinary);
/*	Opens a DCC file send dialog.
		Input:	none
		Output:	file - the file that was selected
					macbinary - true if macbinary xfer was selected, false if not
					return value: true if file was selected, false if not.
*/

pascal short DCCTypeAdd(ConstStringPtr type);
/*	Adds a dcc type.
		Input:	type - ALL CAPS dcc type name. This MUST be a global variable or string constant. It *CAN NOT* be a local variable.
		Output:	return value: reference number for this dcc type, or -1 if the type has already been added.
*/

pascal void DCCGetStatus(dccPtr dcc, dccStatus *status);
/*	Gets the status for a DCC
		Input:	dcc - a DCC record
		Output:	status - the DCC's status
*/

pascal void DCCSendCookie(connectionPtr *conn, long cookie, LongString *ls);
pascal void DCCSendCookieReply(connectionPtr *conn, long cookie, LongString *ls);
/*	Sends a cookie or a reply to a cookie.
		Input:	conn - a Ptr to a dcc connectionPtr
					cookie - the cookie type
					ls - any additional cookie data
		Output:	conn == nil if send failed.
*/

#pragma mark е Preferences Dialog
/*	------------------------------------------------------------------------------------------
		Preferences Dialog
		
		
		------------------------------------------------------------------------------------------
*/

pascal short PMLAdd(ConstStr63Param name);
/*	Adds an entry into the preferences window menu.
		Input:	name - Name of menu item
		Output:	return value: id passed by the prefs menu events, else -1 if error
		Note:	е╩Because there are already three items in the dialog - the OK, Cancel, and
						prefs selection popup menu, the actual item number of things in the dialog
						are going to be 3 larger than their item numbers in your DITL. That is, item
						number 1 in your DITL will be item number 4 in the dialog, and you should treat
						it as such.
*/

pascal void OpenPreferencesWindow(short panelID);
/*	Opens the preferences window to a plugins's prefernces.
		Input:	panelID - a panel ID you get back from PMLAdd(), or -1 to open preferences as if the user chose it from the menu.
		Output:	none
*/


#pragma mark е Preferences Files
/*	------------------------------------------------------------------------------------------
		Preferences Files
		
		Use these functions to manipulate plugin prefs files. They go in the 'Preferences' folder in the plugins
		folder.
		
		Most of these functions are simple abstractions of file manager calls. These are noted in the
		descriptions of the functions. If you're comfortable with the File Manager, you may wish to use the
		File Manager directly.
		------------------------------------------------------------------------------------------
*/

pascal OSErr PFOpen(ConstStr255Param name, char resFork, short *refNum, FSSpec *fsp);
/*	Opens a preferences file.
		Input:	name - the name of the prefs file
					resFork - true if you want to open the resource fork, false if you want the data fork
		Output:	refNum - a reference number for the file
					fsp - if not nil, and the file was opened, the FSSpec to the file.
					return value: File Manager/Resource Manager error code
*/

pascal char PFExists(ConstStr255Param name);
/*	Checks to see if a file with a given name exists in the plugins preferences folder.
		Input:	name - the name of the prefs file
		Output:	return value - true if the file exists, false if not
*/

pascal OSErr PFCreate(ConstStr255Param name, FourCharCode type, FourCharCode creator, char resFork);
/*	Creates a preferences file in the plugins preferences folder
		Input:	name - name of the prefs file
					type - file type of file
					creator - creator of file
					resFork - true if you want to create a resource fork, false if you want a data fork
		Output:	return value: File Manager/Resource Manager error code
*/

pascal OSErr PFClose(short refNum);
/*	Closes a file.
		Input:	refNum - file reference number
		Output:	return value: File Manager error code
		Notes:	е Equivilant to err = FSClose(refNum);
					е Don't use on resource files.
					е You probably don't need this function. Use FileClose() instead.
*/

pascal OSErr PFDelete(ConstStr255Param name);
/*	Deletes a plugin preferences file
		Input:	name - name of prefs file to delete
		Output:	return value: File Manager error code
*/

#pragma mark е File Managment
/*	------------------------------------------------------------------------------------------
		File Managment
		
		Whenever you open or close files, use these functions to make sure ShadowIRC does the Right Thing
		if the file is opened or closed multiple times. DO NOT call these functions if you use PFOpen() or PFClose().
		------------------------------------------------------------------------------------------
*/

pascal void FileAdd(short fref, char res);
/*	Adds a file to ShadowIRC's file list. If the file is alread in the list, increment the access counter.
		Input:	fref - file reference number	
					res - true if resource fork (OpenResFile(), FSpOpenResFile()), false if not.
		Output:	none
		Note:	Pass false to res if you get a resource fork with FSpOpenRF().
*/

pascal OSErr FileClose(short fref);
/*	Decrements a files access count, and closes the file if the access count is zero.
		Input:	fref - file reference number
		Output:	return value: error code from either FSClose() or CloseResFile().
		Note:	FileClose() uses the res setting given in FileAdd() to properly close the file.
*/

#pragma mark е Message Buffer
/*	------------------------------------------------------------------------------------------
		Message Buffer functions

		These functions deal with the private message nickname buffer that is used by the tab key in the
		inputline.
		------------------------------------------------------------------------------------------
*/

pascal void MBIdle(void);
/*	Scans the message buffer list and removes entries that are more than ten minutes old.
		Input:	none
		Output:	none
*/

pascal messageBufferPtr MBFindNick(linkPtr link, ConstStr255Param nick);
/*	Finds a nickname message buffer list.
		Input:	link - the link
					nick - nickname
		Output:	return value: pointer to a message buffer entry.
*/

pascal void MBNewMessage(linkPtr link, ConstStr255Param nick);
/*	Updates the message buffer list.
		Input:	link - the link
					nick - nickname
		Output:	none
*/

#pragma mark е Miscelaneous
/*	------------------------------------------------------------------------------------------
		Miscelaneous
		
		
		------------------------------------------------------------------------------------------
*/

long ShadowIRCVersion(StringPtr versionString);
/*	Returns the version of ShadowIRC being used.
		Input:	versionString - a pointer to where you want a copy of the string representation of ShadowIRC's
					version, or nil, if you don't want a copy.
		Output:	versionString - ShadowIRC's version string is copied here (i.e. "ShadowIRC 1.1 PPC")
					return value: ShadowIRC version encoded in hex - first three bytes are version, major revision,
					and minor revision number. Fourth byte indicates release stage and pre-release number. In version
					checking, larger numbers are always newer versions.
*/

long ShadowIRCVersion2(StringPtr version, StringPtr date);
/*	Returns version information.
		Input:	version - pointer to a string, or nil
					date - pointer to a string, or nil
		Output:	version - if version != nil, the version number of ShadowIRC (i.e. "1.0.2")
					date - if date != nil, the date ShadowIRC was compiled
					return value: ShadowIRC version encoded in hex
*/

pascal void NextArg(Str255 str, StringPtr arg);
/*	Returns the next argument of a string. (Arguments are space delimted.)
		Input:	str - Pascal string.
		Output:	arg - Next argument of str
					str - input, less the string return as arg
*/

pascal void HandleCommand(linkPtr link, LongString *cmd);
/*	Processes a command.
		Input:	link - Link to send data through
					cmd - LongString to process
		Output:	none
		Note:	е HandleCommand() calls plugins with the pUserCommandMessage. You probabally don't
						want to call HandleCommand() while processing a pUserCommandMessage unless you take
						special care to ensure that you won't wind up causing infinite recursion. Use SendCommand()
						here approprate.
*/

pascal void SendCommand(linkPtr link, LongString *ls);
/*	Sends a command to the IRC server without the processing HandleCommand() does
		Input:	link - link to send data through
					ls - LongString to send
		Output:	none
*/

pascal void writeMainPrefs(void);
/*	Saves the Settings file.
		Input:	none
		Output:	none
*/

pascal void writeAllFiles(void);
/*	Saves all data files and asks plugins to write their settings.
		Input:	none
		Output:	none
*/

pascal long ulongval(ConstStr255Param s);
/*	Turns a 10-digit string into a unsigned longint.
		Input:	s - string
		Output:	return value: (unsigned) long.
*/

pascal void ulongstr(long l, Str255 s);
/*	Turns an (unsigned) long into a character string. Use NumToString() for signed values.
		Input:	l - (unsigned) long.
		Output:	s - string
*/

pascal void hostToIPStr(long l, Str255 s);
/*	Changes an IP into xxx.xxx.xxx.xxx format.
		Input:	l - longint representing an IP
		Output:	s - string representation of l
*/

pascal long IPStringToLong(ConstStr255Param s);
/*	Changes the string representation of an IP into a longint.
		Input:	s - string representation of IP
		Output:	return value: IP packed into a longint
*/

pascal void DNSLookup(Str255 addr, long saveReply);
/*	Equivilant to the /dns command. Creates a new connection that looks up the address/IP you pass.
		Input:	addr - nickname, or string in xxx.xxx.xxx.xxx or somemachine.com form
					saveReply - Non-zero if you want a message with the result, zero if you want it displayed like
										a /dns command.
		Output:	addr - if addr is a nickname, then if the user exists on any channel the user is on, addr is the
								machine name portion of their userhost. If the user does not exist or ShadowIRC has not
								recieved the user's userhost, a zero-length string. If addr is an IP or machine name,
								it is unchanged.
		Note:	When possible, make sure before you call DNSLookup() that ShadowIRC has recieved the user's
					userhost by checking to see if the userhost length is greater than zero. ShadowIRC displays a
					can't lookup error message in this condition, but only if saveReply == 0.
*/

pascal void doBroadcast(linkPtr link, const LongString *text, char action);
/*	Equivilant to the /broadcast and /bract commands.
		Input:	link - The link to broadcst on, or nil for all links.
					text - the text to broadcast
					action - true if an action, false if not
		Output:	None
*/

pascal void DoONotice(channelPtr ch, const LongString *text);
/*	Does the /onotice command.
		Input:	targ - target of /onotice.
					text - the text to send
		Output:	none
*/

pascal void ListIgnores(void);
/*	Outputs the list of ignores to the front window.
		Input:	none
		Output:	none
*/

void DoIgnore(LongString  *in, char silent);
/*	Does ignore list matinence. Equivilant to /ignore command.
		Input: 	in - parameters for /ignore.
				silent - if true, ShadowIRC will not output the ignore list.
		Output:	none
*/

pascal char IsIgnored(ConstStr255Param uah);
/*	Checks to see if someone is ignored, and if they are, sets lastIgnored time for the matched ignore mask to the current time.
		Input:	uah - nick!user@host of user
		Output:	return value: true if found, false if not
		Note:	Use this function if you're looking to see if someone's ignored so you can ignore their text, or something
					along those lines.
*/

pascal char IsIgnoredNickUser(ConstStr255Param nick, ConstStr255Param user);
/*	Checks to see if someone is ignored. Calls IsIgnored() with a nick!user@host.
		Input:	nick - nick of user
					user - username@host of user
		Output:	return value: return value of IsIgnored(nick!user)
*/

pascal ignorePtr findIgnore(ConstStr255Param mask, ignorePtr start);
/*	Finds a mask in the ignore list.
		Input:	mask - mask to mach against ignore list. Must pass a nick!user@host, or maskMatch() will always fail.
					start - first ignore entry to scan from. 0 = start of ignore list
		Output:	return value: first matching ignore entry, or 0 if none found.
		Note:	Use this function if you're trying to find out if someone's ignored.
*/

pascal void DoNotify(linkPtr link, LongString *parse);
/*	Does notify list matinence for a given link. Equivilant to /notify command.
		Input:	link - link to operate on.
					parse - LongString containing list of nicks to add/remove from list
		Output:	none
*/

pascal void RunNotify(void);
/*	Sends out ISON queries to the server for each connected link.
		Input:	none
		Output:	none
*/

pascal void ListNotify(linkPtr link);
/*	Lists the notify list for a link in the front window.
		Input:	link - link to list the notify list for
		Output:	none
*/

pascal notifyPtr findNotify(linkPtr link, ConstStr255Param nick);
/*	Finds an entry in the notify list for a link.
		Input:	link - link to search in
					nick - nickname to search for. (Case insensitive)
		Output:	return value: notifyPtr if found, nil if not.
*/

pascal void deleteNotify(linkPtr link, ConstStr255Param nick);
/*	Removes an entry from a link's notify list.
		Input:	link - link to remove from
					nick - nickname to remove
		Output:	none
*/

pascal notifyPtr addNotify(linkPtr link, ConstStr63Param nick, char ison);
/*	Adds an entry to a link's notify list.
		Input:	link - link to add to
					nick - nickname to add
					ison - true if /ison, false if /notify
		Output:	return value: the notifyPtr added.
*/

pascal void ProcessShortcutText(LongString *ls, short *cursorPos, short *endPos);
/*	Runs text through the same conversion as it would go through if it were a shortcut.
		Input:	ls - string to process
		Output:	ls - processed string
					cursorPos - if cursorPos is not nil, where to insert the cursor in the inputlinefrom '$->' token. (-1 means at the end)
					endPos - if endPos is not nil, the end of a selection created by a $-> ... $<- sequence, or -1.
*/

pascal void StackModes(LongString *theMode, channelPtr channel, ConstStr255Param mode, ConstStr255Param arg);
/*	Stacks multiple mode changes into one command, and sends it to the server when necessary.
		Input:	theMode - LongString containing the current mode string.
					channel - channel mode changes are for
					mode - the mode to add, i.e. +t, or +o, or -v. If mode == nil, finishes up the mode command and sends
								it to the server.
					arg - Arguments for mode changes, i.e. channel limit, or the name of the user to op/deop/voice/devoice
							if arg == nil, no argument is used.
		Output:	theMode - contains the updated mode string, or has a length of zero if the string was sent to the server.
		Usage:	See StackModes.c in the Sample Code folder for a sample function that utilizes StackModes.
		Note:	StackModes will combine a maximum of four mode changes unless the server informs the client
					it can do more.
*/

pascal void SecsToHMS(long secs, LongString *ls);
/*	Takes a number of seconds and returns a LongString in the form of [h hours, [m minutes, [s seconds]]].
		Input:	secs - number of seconds
		Output:	ls - LongString
*/

OSErr OpenURL(Str255 url);
/*	Opens a URL with Internet Config
		Input:	url - url
		Output:	return value: -1 if Internet Config not installed, 0 if no error, or Internet Config error value.
*/


#pragma mark е String Maniuplation
/*	------------------------------------------------------------------------------------------
		String Maniuplation

		These functions do various string manipulations
		------------------------------------------------------------------------------------------
*/

pascal char upc(unsigned char c);
/*	Takes a character and makes it uppercase.
		Input:	c - a character
		Output:	return value - a character. if c in ['a'..'z'], return value is uppercase. Otherwise, return value = c.
*/

pascal void ucase(Str255 str);
/*	Takes a pascal string and makes it uppercase.
		Input:	str - Pascal string
		Output:	str - Uppercase version of input.
*/

pascal void pdelete(Str255 s, short start, short len);
/*	Deletes text in a pascal string.
		Input:	s - Pascal string
					start - first character to delete
					len - number of characters to delete
		Output:	s - Pascal string
*/

pascal void pinsert(ConstStr255Param ins, Str255 s, short insat);
/*	Inserts a pascal string into another.
		Input:	ins - string to be insterted
					s - string receiving the insert
					insat - the position where the first character of ins should go
		Output:	s - string with ins inserted
*/

pascal void pstrcpy(ConstStr255Param src, Str255 dest);
/*	Duplicates a pascal string.
		Input:	src - Pascal string
		Output:	dest - a copy of src
*/

pascal char pstrcmp(ConstStr255Param s1, ConstStr255Param s2);
/*	Compares two pascal strings.
		Input:	s1 - Pascal string
					s2 - Pascal string
		Output:	return value - 1 if s1 and s2 are exactly the same. 0 if not.
*/

pascal char pstrcasecmp(ConstStr255Param s1, ConstStr255Param s2);
/*	Compares two pascal strings. Not case sensitive.
		Input:	s1 - Pascal string
					s2 - Pascal string
		Output:	return value - 1 if s1 and s2 are the same. 0 if not.
		Note:	This is a good function to use if you need to test two strings
					case insensitively, and don't plan on using either of the strings in other
					string compares.
*/
pascal char pstrcasecmp2(ConstStr255Param s1, ConstStr255Param s2);
/*	Compares two pascal strings. Assumes that s1 is already uppercase.
		Input:	s1 - Pascal string (UPPERCASE)
					s2 - Pascal string
		Output:	return value - 1 if s1 and s2 are the same. 0 if not.
		Note:	This is a good function to use when comparing one string to many others,
					and you want the search to be case insensitive. By calling ucase() on s1
					in advance, you reduce the amount of work pstrcasecmp2() does compared
					with pstrcasecmp(), which yields faster execution times.
*/

pascal void padEnd(ConstStr255Param s, short num, Str255 out);
/*	Pads the end of a string with spaces. More efficient if out = s.
		Input:	s - string
					num - minimum length of out
		Output:	out - string padded with (num-length(s)) spaces. If length(s)>num, nothing is added.
*/

pascal void padBegin(ConstStr255Param s, short num, Str255 out);
/*	Pads the begining of a string with spaces. More efficient if out = s.
		Input:	s - string
					num - minimum length of out
		Output:	out - string padded with (num-length(s)) spaces. If length(s)>num, nothing is added.
*/

pascal OSErr ParamString(Str255 str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3);
/*	Takes a string with tokens and replaces the tokens.
		Input:	str - string with tokens (^0, ^1, ^2, ^3). You may use as many or as few tokens as you wish.
					r0, r1, r2, r3 - replacment strings for tokens. You may pass nil if a token does not appear in your string.
		Output:	str - string with tokens replaced
					return value: error condition. 0 == no error
*/

pascal char maskMatch(ConstStr255Param s, ConstStr255Param mask);
/*	Matches a string with a mask. (Uses the wildcards '*' and '?'.)
		Input:	s - Pascal string
					mask - The mask (Pascal string)
		Output:	return value - 1 if s matches mask. 0 if not.
*/

pascal void makeMask(Str255 uah, Str255 mask);
/*	Makes a hostmask, given a nick!user@host or user@host
		Input:	uah - nick!user@host or user@host
		Output:	mask - hostmask for uah. If it can't generate a hostmask, mask is empty.
*/

pascal short countChar(short c, ConstStr255Param s);
/*	Counts the number of times a character appears in a string.
		Input:	c - A character
					s - Pascal string
		Output:	return value - Number of times c appears in s.
*/

pascal short pos(short c, ConstStr255Param s);
/*	Finds the first occurance of a character in a string.
		Input:	c - A character
					s - Pascal string
		Output:	return value - The position of the first occurance of c in s, or 0 if c is not in s.
*/

pascal short revPos(short c, ConstStr255Param s);
/*	Finds the last occurance of a character in a string.
		Input:	c - A character
					s - Pascal string
		Output:	return value - The position of the last occurance of c in s, or 0 if c is not in s.
*/

pascal char isNumber(ConstStr255Param s);
/*	Checks to see if a string is a number.
		Input:	s - Pascal string
		Output:	return value - 1 if s is a number, 0 if not. (Returns 0 if any character other than a numeric digit is in the string.
*/

pascal char isIPNumber(ConstStr255Param s);
/*	Checks to see if a string is a valid IP in nnn.nnn.nnn.nnn form.
		Input:	s - Pascal string
		Output:	return value - true if valid, false if not
*/

pascal void pstrcat(ConstStr255Param src1, ConstStr255Param src2, StringPtr des);
/*	Concatenates src1 + src2 into des
		Input:	src1 - First source string
					src2 - Second source string
					des - Destination string
		Output:	des contains src1 + src2
*/

#pragma mark е Utilities
/*	------------------------------------------------------------------------------------------
		Utility Functions

		Miscelaneous utility functions. Provided so you don't have to reinvent the wheel.
		------------------------------------------------------------------------------------------
*/

pascal void GetText(DialogPtr dlg, short item, Str255 into);
/*	Gets the text of a dialog item.
		Input:	dlg - The dialog.
					item - The item number.
		Output:	into - Pascal string containing the item's text.
*/

pascal void SetText(DialogPtr dlg, short item, ConstStr255Param text);
/*	Sets the text of a dialog item.
		Input:	dlg - The dialog.
					item - The item number.
					text - Pascal string containing the text to set.
		Output:	none
*/

pascal void setCheckBox(DialogPtr dlg, short item, char up);
/*	Sets the state of a check box.
		Input:	dlg - The dialog.
					item - The item number.
					up - True if on, false if off.
		Output:	none
*/

pascal char getCheckBox(DialogPtr dlg, short item);
/*	Gets the state of a check box.
		Input:	dlg - The dialog.
					item - The item number.
		Output:	return value - True if on, false if off.
*/

pascal void setButtonEnable(DialogPtr dlg, short item, char on);
/*	Enables or disables (dims) a dialog item. (Can be used on anything).
		Input:	dlg - The dialog.
					item - The item number.
					on - True if enabled, false if disabled.
		Output:	none
*/

pascal ControlHandle GetControlHandle(DialogPtr d, short item);
/*	Gets the control handle of a dialog item.
		Input:	d - The dialog.
					item - The item number.
		Output:	none
*/

pascal char getFirstSelectedCell(ListHandle l, Cell *c);
/*	Gets the first selected cell in a list.
		Input:	l - A List.
		Output:	c - The first selected cell.
					return value - True if a cell is selected, false if not.
*/

pascal void selectOneCell(ListHandle l, Cell c);
/*	Selects a cell in a list.
		Input:	l - A list.
					c - The cell to select.
		Output:	none
*/

pascal void makeCellVisible(ListHandle l, Cell c);
/*	Makes the specified cell in the list visible.
		Input:	l - a list
					c - the cell to view
		Output:	none
*/

pascal void drawListBorder(ListHandle theList);
/*	Draws the border around a list.
		Input:	theList - The list to border.
		Output:	none
*/

pascal void SetDlogFont(DialogPtr d);
/*	Sets the text in a dialog to Geneva 10.
		Input:	d - The dialog.
		Output:	none.
*/

pascal OSErr CopyResource(ResType type, short id, short src, short des);
/*	Copies a resource from one resource file to another.
		Input:	type - the resource type
					id - the resource id
					src - source res file
					des - destination res file
		Output:	zero if succesful, otherwise a resource manager error.
*/

pascal OSErr AsyncSoundPlay(Handle sound, long refcon, Ptr *channel);
/*	Plays a sound asynchronously and calls you with a pAsyncSoundCompletionMessage when the sound is finished.
		Input:	sound - The handle to the sound you wish to play
					refcon - a value that will be returned to you in the pAsyncSoundCompletionMessage message.
					channel - a pointer to a SndChannelPtr for the sound channel to play on, a pointer to nil to create a new channel,
									or nil if you don't want the sound channel back.
		Output:	channel - the sound channel the sound will play on
					return value: any error, or 0 == noErr.
		Notes:	е If you do not specify a sound channel, it will be disposed at the end of sound play. If you provide your own sound channel, it
						will NOT be disposed of.
					е Your sound's handle must be locked. AsyncSoundPlay() does not do this for you.
					е You must dispose of the sound yourself.
*/

pascal void LightenColor(RGBColor *rgb);
/*	Lightens a color so it looks "dimmed."
		Input:	rgb - a color
		Output:	rgb - lightened color
*/

pascal void CleanFolderFSp(FSSpec *fss);
/*	"Cleans" up a folder FSSpec
		Input:	fss - FSSpec for a folder, with it's name in the name field and it's parent directory in the parID field.
		Output:	fss - FSSpec for folder with it's id in the parID field and the name field blank.
*/

pascal char DirectorySelectButton(FSSpec *fss);
/*	Asks the user to select a directory.
		Input:	fss - FSSpec to write to
		Output:	fss - FSSpec the user selected, or unchanged.
					return value: true if the user selected a directory, false if canceled.
*/

pascal long _UndocumentedAPI(long, long);
/*	This function is undocumented, and should never be called.
*/

#endif
