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

#ifndef _Plugins
#define _Plugins

#ifndef _TCP
#include "TCP.h"
#endif

#define PLUGIN_MAGIC 'PLUG'

enum servicesList {
	genericServiceType					='****',	//Generic service. Use this to receive generic service messages.
	userlistServiceType					='ulst',		//Userlist
	facesServiceType						='face',		//Faces
	connectionWindowServiceType	='conw',	//Connection window
	dccWindowServiceType				='dccw',	//DCC window
	soundServiceType						='sond',	//Sound service
	userRegistrationType				='ureg'
};

enum serviceClassList {
	genericServiceClass				 	='****',
	userlistServiceClass 				='ulst',
	connectionWindowServiceClass	='conw',
	dccWindowServiceClass			='dccw',
	soundServiceClass					='sond',
	userRegistrationClass				='ureg'
};

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

enum pVersionCheckMessageReply {
	pVersionShadowIRCTooOld = -1,
	pVersionPluginTooOld = -2,
	
	pVersion6MessageReply = 3,
	pVersion10MessageReply = 4,
	pVersion11MessageReply = 5,
	pVersionCheckMessageReply = 5
};

enum messagesList
{
	numMessages		=41,					//To set the size of CMArray...
// ShadowIRC plugin messages
	pVersionCheckMessage			=-1,	//You recieve this message before pInitMessage. Respond to it with pVersionCheckMessageReply.
	pInitMessage							=0,	//Init time message. You always get this. This is the first message your plugin receives.
	pQuitMessage						=1,	//Exit time message. You always get this. This is the last message your plugin receives.
	pUserCommandMessage			=2,	//User entered a command. You get this message before ShadowIRC checks it.
	pIdleMessage							=3,	//Idle message.
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

//Here, "window" means a DialogPtr or WindowPtr, unless stated.
	pUIWindowCloseMessage			=51,		//User attempted to close one of your windows.
	pUIWindowMoveMessage			=52,		//User moved one of your windows.
	pUIDialogItemHitMessage			=53,		//User clicked a button in one of your windows.
	pUIMWGotTextMessage				=54,		//User typed text to one of your plugin's message windows
	pUIActivateMessage					=55,		//A window has become active or deactive.
	pUIMouseUpDownMessage			=56,		//A user clicked in your window somewhere.
	pUIUpdateMessage						=57,		//Update event from OS
	pUIInGrowMessage					=58,		//User clicked on your grow box (if one exists).
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
	pTextConnectionMessage			=90,		//Plugin-created connection message. *text* connections only
	pDataConnectionMessage			=91,		//Plugin-created connection message. *data* connections only

//Inputline Widgets
	pIWUpdateMessage					=141,		//Status line update message
	pIWClickMessage						=142,		//Status line click message

//Message Window Widgets
	pMWWidgetUpdateMessage			=143,		//You are being asked to update your widget
	pMWWidgetClickMessage			=144,		//User lcicked on your message window widget
	pMWWidgetDestroyMesage		=145,		//Your message window widget is being destroyed. Perform cleanup first.

//Message Window Panes
	pMWPaneDestroyMessage			=146,		//Your panel is being destroyed. Perform cleanup.
	pMWPaneActivateMessage			=147,		//The message window was activated or deacvitated. Perform necessary tasks.
	pMWPaneClickMessage				=148,		//Your panel has been clicked.
	pMWPaneUpdateMessage			=149,		//The window is being updated. Draw your panel.
	pMWPaneResizeMessage			=150,		//The window was resized, or a pane was deleted and a plugin forced a resize. You only get this if the window is visible.

//Misc
	pCMPopupsReturnMessage			=259,	//User has selected one of your items from a contextual menu managed by ShadowIRC.
	pHelpMenuMessage					=64,		//User selected something from the help menu that your plugin put in.
	pAsyncSoundCompletionMessage=80,	//Completion message from AsyncSoundPlay(). The sound has finished playing.
	pMenuItemSelectedMessage		=59,		//A menu item was selected

/* Services messages
*/

//General service messages - all services receive these
	pServiceActivateWin				=101,		//Activation/deactivation of a window. You will sometimes get this twice in a row for the same window.
	pServiceWindowMenu			=114,		//User selected your service from the window menu. Act as appropriate.

//These messages apply only to a userlist service or a faces service.
	pServiceULUserhosts			=111,		//ShadowIRC received a userhosts message and has parsed it.
	pServiceULNamesEnd				=112,		//ShadowIRC received the end of the first /names message.
	pServiceULTrashChannel		=115,		//The userlist of a channel is being trashed. Walk its userlist and do any memory disposals you have to.
	pServiceULNickChange			=116,		//A user has changed their nick. You receive the *pServerNICKMessage* struct *AFTER* the channel's userlist has been updated.

//These messages are only for a connection window service
	pServiceCWLinkStateChange	=131,		//The connection state of a link has changed.

//This is the sound service message
	pSoundServiceMessage			=161,		//Play a sound.

//This is for the user registration message
	pServiceUserRegMessage		=500,

/*	Inter-Plugin Communication messages
*/
	pIPCMessage							=300,		//Your plugin is beging called with an IPC message
	pIPCReplyMessage					=301		//Your plugin is being called by an IPC service
};

struct ShadowIRCDataRecord;

/* plugsPtr
*/
typedef pascal void(*pluginMain)(struct ShadowIRCDataRecord*);
typedef char CMArray[numMessages + (4 - (numMessages%4))]; //keeps array size divisible by four.
typedef struct plugsRec plugsRec, *plugsPtr;
struct plugsRec {
	pluginMain proc;
	long xpluginRef;
	Str31 pluginName;				//The name of your plugin's file
	long timesCalled;					//The number of times your plugin has been called.
	long lastIdleCall;					//The last time your plugin was called at idle time.
	short resFileRefNum;			//The File Manager refnum of your resource fork.
	short idleThreshold;				//If you're recieving idle messages, how often to get them (minimally), in ticks.
	CMArray captureMessages;	//The messages your plugin listens for.
	
	plugsPtr next;
};

typedef struct pluginDlgInfo {
	OSType magic;
	plugsPtr pluginRef;
} pluginDlgInfo, *pluginDlgInfoPtr;

/*	ShadowIRCDataRecord
*/
typedef struct ShadowIRCDataRecord {
	plugsPtr yourInfo; 					//Datarec for your plugin.
	void* messageData;					//Data for the message. If no data, nil.
	short message;							//The message.
	char unused2;
	char completelyProcessed;					//If this is true, ShadowIRC stops calling plugins for this message. Ignored for init and idle messages.

	prefsPtr mainPrefs;								//Pointer to the prefs.
	linkPrefsPtr linkPrefs;							//Pointer to an array of ten link prefs
	RGBColor *shadowircColors;					//The list of ShadowIRC internal colors.

	linkPtr *firstLink;
	
#ifndef _IRCIgnore
	Ptr *ignores;
#else
	ignorePtr *ignores;								//Pointer to ignore list
#endif

#ifndef _MsgBuffer_
	Ptr *messageBuffers;
#else
	messageBufferPtr *messageBuffers;	//Pointer to a time ordered list of 10 messageBuffers.
#endif
	connectionPtr *connections;					//Pointer to the list of connections.
	long *curUserNum;								//The userNum given to the most recently created user. Anything larger than this is invalid.

	unsigned long *idleTime;						//Time when last command sent. (GetDateTime() based)
	char *debugOn;										//Is debug mode on or off?
	unsigned long *launchTime;					//Time when ShadowIRC was launched. (GetDateTime() based)
	char *inBackground;								//True if ShadowIRC is in the background, false if it's in the foreground.
	long *lastInput;										//The time the user last hit return in the inputline, in seconds
	long *lastKey;										//The time the user last input any text to the inputline, in seconds
	
#ifndef _IRCChannels
	Ptr CurrentTarget;
#else
	targetPtr CurrentTarget;						//The active target
#endif
#ifndef _InputLine_
	Ptr inputLine;
#else
	inputLinePtr inputLine;					 		//Pointer to the inputline data.
#endif

#ifndef _MsgWindows_
	Ptr *consoleWin;
#else
	MWPtr *consoleWin;								//A pointer to the console.
#endif
#ifndef _MsgWindows_
	Ptr *mwList;
#else
	MWPtr *mwList;					//A pointer to the (message) window list.
#endif
	
	Ptr internetConfig;								//The Internet Config component. Nil if IC is not installed. Typecast to internetConfigurationComponent.
	WindowPtr* ContextWindow;
} ShadowIRCDataRecord;

#pragma mark ¥ Structs
/*			------------------------------------------------------------------------------------------
				Message Data structures
				------------------------------------------------------------------------------------------
*/

/*	pVersionCheckData
		If you do not repsond to this message with pVersionCheckMessageReply, you will not get your
		init message. 
*/
typedef struct pVersionCheckData {
	long reply;
} pVersionCheckData, *pVersionCheckDataPtr;

/*	pCommandDataRec
*/
typedef struct pCommandDataRec {
	linkPtr link;					//The link the command will apply to
	StringPtr com;				//The command.
	LongString *rest;			//Any arguments the user may have typed after the command.
	LongString *outgoing;	//Text you want HandleCommand() to send to the server.
	char dontProcess;			//Set to true if you don't want ShadowIRC to process the command.
} pCommandDataRec, *pCommandDataPtr;

/*	pIdleMessageData
*/
typedef struct pIdleMessageData {
	EventRecord *e;			//The event record
} pIdleMessageData, *pIdleMessageDataPtr;

#ifdef _MsgWindows_

/*	pInputMessageRec
*/
typedef struct pInputMessageDataRec{
	linkPtr link;						//The link the text will go into
	LongString *inputText;		//Text the user typed. When your plugin exits, the text to process.
	MWPtr mw;						//The target of the input.
	char action;						//On entry, true if the option key was down. On exit, process as action or not.
} pInputMessageDataRec, *pInputMessageDataPtr;

#endif

/*	pKeyDownDataRec
*/
typedef struct pKeyDownDataRec {
	char *character;					//The character pressed. Set to 0 to stop ShadowIRC from processing it.
	
	EventRecord *e;					//The event record for the keypress.
} pKeyDownDataRec, *pKeyDownDataPtr;

/*	pNumericCommandDataRec
*/

typedef struct pNumericCommandDataRec {
	linkPtr link;					//The link
	StringPtr from;					//The server the message came from.
	short command;					//Serer command. Server commands are documented in RFC1459.
	LongString *incoming;			//The text for the parameters the server gave.
	char *commandProcessed;	//True if ShadowIRC procesed the message, false if not. On exit, set this to true if
												//you process the message. (You don't necessarally have to set sidr->completelyProcessed.)
	short *lastNumerics;			//10 most recent numerics; lastNumerics[0] is this numeric.
	char DONT_PROCESS;
} pNumericCommandDataRec, *pNumericCommandDataPtr;

#ifdef _channels

/*	pServerJOINDataRec
*/
typedef struct pServerJOINDataRec {
	linkPtr link;					//The link
	StringPtr channelName;	//The name of the channel that was joined or parted.
	StringPtr username;		//The name of the user.
	StringPtr userhost;		//The user@host of the user.
	UserListPtr userPtr;		//The UserListPtr of this user.
	channelPtr channel;		//The channel
	char isMe;					//True if the user is me, false if not.
	char dontDisplay;			//Don't display the join message. Defaults to setting in prefs.
	char dontSound;			//Don't play the sound. Defaults to false.
} pServerJOINDataRec, *pServerJOINDataPtr;

/*	pServerPARTDataRec
*/
typedef struct pServerPARTDataRec {
	linkPtr link;					//The link
	StringPtr channelName;	//The name of the channel that was joined or parted.
	StringPtr username;	//The name of the user.
	StringPtr userhost;	//The user@host of the user.
	UserListPtr userPtr;		//The UserListPtr of this user.
	channelPtr channel;
	LongString *partReason;
	char isMe;						//True if the user is me, false if not.
	char dontDisplay;					//Don't display the part message. Defaults to setting in prefs.
	char dontSound;					//Don't play the sound. Defaults to false.
} pServerPARTDataRec, *pServerPARTDataPtr;

#endif

/*	pServerQUITDataRec
*/
typedef struct pServerQUITDataRec {
	linkPtr link;					//The link
	StringPtr username;			//The name of the user who signed off.
	StringPtr userhost;			//The userhost of the user. If it's me, the userhost is a null string. Sorry.
	LongString *signoffMessage;		//The user's signoff message.
	char isMe;								//True if me, false if not.
	char dontDisplay;					//Don't display the quit message. Defaults to setting in prefs.
	char dontSound;					//Don't play the sound. Defaults to false.
} pServerQUITDataRec, *pServerQUITDataPtr;

#ifdef _channels

/*	pServerTOPICDataRec
*/
typedef struct pServerTOPICDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user changing the topic.
	StringPtr userhost;		//The userhost of the user.
	StringPtr channelName;		//The channel whose topic is being changed.
	LongString *newtopic;			//The new topic.
	channelPtr channel;
	char isMe;							//True if I'm changing the topic, false if not.
	char dontDisplay;
	char dontSound;
} pServerTOPICDataRec, *pServerTOPICDataPtr;

#endif

/*	pServerNICKDataRec
*/
typedef struct pServerNICKDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user changing nicks.
	StringPtr userhost;		//The userhost of the user.
	StringPtr newnick;		//Their new nickname.
	char isMe;					//True if I'm changing nicks.
	char dontDisplay;			//If true, don't display this message.
	char dontSound;			
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
	linkPtr link;					//The link
	StringPtr source;	//the source of the pong.
} pServerPONGDataRec, *pServerPONGDataPtr;

#ifdef _channels

/*	pServerKICKDataRec
*/
typedef struct pServerKICKDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user kicking.
	StringPtr userhost;		//The userhost of the user.
	StringPtr channelName;		//The channel.
	StringPtr kickedNick;	//The name of the user getting kicked.
	LongString *message;			//The kick message.
	channelPtr channel;
	char isMe;							//True if I was kicked.
	char autoRejoin;		//On entry, if ShadowIRC will autorejoin. On exit, whether ShadowIRC should autorejoin.
	char dontDisplay;
	char dontSound;
} pServerKICKDataRec, *pServerKICKDataPtr;

#endif

/*	pServerINVITEDataRec
*/
typedef struct pServerINVITEDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The name of the user inviting you.
	StringPtr userhost;		//The userhost of the user.
	StringPtr channel;			//The channel you are being invited to.
	char autojoin;				//On entry, true if ShadowIRC will join the channel. On exit, whether it will or not.
	char ignored;				//True if the person is being ignored (in which case, autojoin will always be false)
	char dontDisplay;			//If false
	char dontSound;
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

#ifdef _channels

/*	pServerPRIVMSGDataRec;
*/
typedef struct pServerPRIVMSGDataRec {
	linkPtr link;					//The link
	StringPtr username;		//The nick of the user who sent the message.
	StringPtr userhost;		//The userhost of the user.
	StringPtr target;			//The target of the message.
	channelPtr chan;			//If the trget is a channel, the channel.
	LongString *message;	//The message.
	char ignored;				//If true, the message will be ignored.
	char targMe;					//If true, the message was going to me.
	char targChan;				//If true, the message was going to a channel.
	char targOther;				//If true, the message wasn't targeted at me or a channel.
	char fromServer;			//If true, the message came from the server.
	char autoQuery;			//If true, ShadowIRC intends to auto-open a /query window. Set to false to keep the window from opening.
	char displayUserHost;	//If true, displays the person's userhost. defaults to targMe && mainPrefs->showUserHostsWithMsgs
	char dontSound;			//If false, doesn't play sound.
	MWPtr redirect;			//If not nil, redirect output to this message window.
	
	unsigned char *nickStyleP;		//Pascal string representing color/style to be used for the nickname display. (max length = 9)
	unsigned char *textStyleP;		//Pascal string representing color/style to be used for text. (max length = 9)
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
	char targMe;							//If true, the message was going to me.
	char targChan;						//If true, the message was going to a channel.
	char targOther;						//If true, the message wasn't targeted at me or a channel.
	char fromServer;					//If true, the message came from the server.
	char autoQuery;					//If true, ShadowIRC intends to auto-open a /query window. Set to false to keep the window from opening.
	char displayUserHost;			//If true, displays the person's userhost. defaults to targMe && mainPrefs->showUserHostsWithMsgs
	char toConsole;						//Force to console
	char dontSound;					//If true, don't play a sound.

char unused[3];	
	
	MWPtr redirect;
}  pServerNOTICEDataRec, *pServerNOTICEDataPtr;

#endif

/*	pServerRPONGData
*/
typedef struct pServerRPONGData {
	linkPtr link;					//The link
	StringPtr server;
	StringPtr num;
	long pingTime;
	char dontDisplay;
	char dontSound;				//Don't play the sound
} pServerRPONGData, *pServerRPONGDataPtr;

/*	pServerSILENCEData
*/
typedef struct pServerSILENCEData {
	linkPtr link;					//The link
	StringPtr mask;
	char add;
	char completelyProcessed;
} pServerSILENCEData, *pServerSILENCEDataPtr;

#ifdef _channels

/*	pServerMODEData
		Note: You get one of these for each mode change on a channel. You may wish to cache them and react when
		you get a message where done=1.
*/
typedef struct pServerMODEData {
	linkPtr link;			//The link
	channelPtr ch;		//Channel for the mode change
	char up;				//true = mode +, false = mode -
	char mode;			//The mode changed, i.e. 'v', 'o', 'k', etc
	long data;				/*	If mode v or o, UserListPtr to user. If mode b, StringPtr to ban change.
										If mode+ k, StringPtr to channel key. If mode +l, new channel limit.
										If done is true, StringPtr to the entire mode change. Otherwise, nil.
								*/
	char done;				//If true, finished processing the MODE command from the server.
	char dontDisplay;	//If true, ShadowIRC won't display the message
	char dontSound;	//If true, ShadowIRC won't ask the sound service to play the mode sound
} pServerMODEData, *pServerMODEDataPtr;

#endif

/*	pCTCPDataRec
*/
typedef struct pCTCPDataRec {
	linkPtr link;									//The link
	connectionPtr dcc;						//If this was a DCC CTCP, this is a pointer to the dcc, else nil.
	ConstStr255Param username;		//The name of the user who did the ctcp.
	ConstStr255Param userhost;		//The userhost of the user. 
	ConstStr255Param target;			//The target. (Nick or channel.)
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

#ifdef _MsgWindows_
/*	pMWTextMessage
*/
typedef struct pMWTextData {
	MWPtr mw;							//The message window
	LongString *ls;						//The text that will go in it.
} pMWTextDataRec, *pMWTextDataPtr;
#endif

/*	pDCCChatCookieData
*/
typedef struct pDCCChatCookieData {
	connectionPtr conn;				//The DCC connection
	long cookie;							//The cookie
	LongString *text;					//The cookie data.
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

/*	pContextSwitchDataRec
*/
typedef struct pContextSwitchDataRec {
	char inBackground ;	//If false, ShadowIRC is now the foreground application. If true, it's now in the bg.
} pContextSwitchDataRec, *pContextSwitchDataPtr;

#ifdef _MsgWindows_

/*	pMWNewData
*/
typedef struct pMWNewData {
	MWPtr mw;		//Message window being created
} pMWNewData, pMWNewDataPtr;

/*	pMWDestroyData
*/
typedef struct pMWDestroyData {
	MWPtr mw;		//Message window being destroyed
} pMWDestroyData, pMWDestroyDataPtr;

#endif

/*	pDCCCreateData
*/
typedef struct pDCCCreateData {
	connectionPtr connection;	//The connection created.
} pDCCCreateData, *pDCCCreateDataPtr;

/*	pDCCOpenedData
*/
typedef struct pDCCOpenedData {
	connectionPtr connection;	//The connection opened. No data transactions have been made yet.
} pDCCOpenedData, *pDCCOpenedDataPtr;

/*	pDCCClosedData
*/
typedef struct pDCCClosedData {
	connectionPtr connection;	//The connection to be closed. (It will not have been destroyed yet.)
	char silent;
} pDCCClosedData, *pDCCClosedDataPtr;

/*	pInputTextDisplayData
*/
typedef struct pInputTextDisplayData {
#ifdef _IRCChannels
	targetPtr targ;					//The target text was typed into.
#else
	Ptr targ;
#endif
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
#ifdef _IRCNotify_
	notifyPtr notifyList;		//The link's notify list. (link->notifyList). Provided for convienence.
#else
	Ptr notifyList;
#endif
	short signedOn;				//The number of users that signed on since the last notify period.
	short signedOff;			//The number of users that signed off since the last notify period.
} pNOTIFYData, *pNOTIFYDataPtr;

/*	pDCCConnOpenedData
*/
typedef struct pDCCConnOpenedData {
	connectionPtr conn;		//The DCC connection that was established. The DCC connection is fully open.
} pDCCConnOpenedData, *pDCCConnOpenedDataPtr;

#ifdef _MsgWindows_

/*	pUIWindowCloseDataRec
*/
typedef struct pUIWindowCloseDataRec {
	WindowPtr w;	//a WindowPtr to the window that was a target of a close action.
	MWPtr mw;		//A MWPtr to the window closed if the window is a message window. Otherwise nil.
} pUIWindowCloseDataRec, pUIWindowCloseDataPtr;

#endif

/*	pUIDragTrackData
*/
typedef struct pUIDragTrackData {
	WindowPtr window;	//The window
	long drag;					//The DragReference
	short message;			//the DragTrackingMessage
} pUIDragTrackData, *pUIDragTrackDataPtr;

/*	pUIDragReceiveData
*/
typedef struct pUIDragReceiveData {
	WindowPtr window;	//The window
	long drag;					//The DragReference
	OSErr returnVal;		//Receive Handler error code
} pUIDragReceiveData, *pUIDragReceiveDataPtr;

#ifdef _MsgWindows_

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

#endif

/* pUIWindowMoveDataRec
*/
typedef struct pUIWindowMoveDataRec {
	WindowPtr w;	//a WindowPtr, *NOT* a MWPtr, to the window that was moved.
	Rect newpos;	//where the window was moved to
} pUIWindowMoveDataRec, *pUIWindowMoveDataPtr;

/* pUIDialogItemHitDataRec
*/
typedef struct pUIDialogItemHitDataRec {
	DialogPtr dlg;	//The dialog that was hit
	short item;		//The item that was hit.
} pUIDialogItemHitDataRec, *pUIDialogItemHitDataPtr;

#ifdef _MsgWindows_

/*	pUIMWGotTextDataRec
*/
typedef struct pUIMWGotTextDataRec {
	MWPtr window;
	LongString *text;
} pUIMWGotTextDataRec, *pUIMWGotTextDataPtr;

#endif

/*	pPWActivateData
*/
typedef struct pPWActivateData {
	short id;							//id of pane to activate
	short ditlNum;					//set this to the id of your DITL resource
	char hasConnectionsList;	//set to true if you want the connections list. (Don't forget to put it in the ditl as item 10)
} pPWActivateData, *pPWActivateDataPtr;

/*	pPWSetWindowData
*/
typedef struct pPWSetWindowData {
	DialogPtr PD;	//The Prefs Dialog
	short id;					//id of pane to set up
	short connListNum;	//The connection (0-9) that you are setting data for. (Ignore if you didn't specify hasConnectionsList = true)
} pPWSetWindowData, *pPWSetWindowDataPtr;

/*	pPWGetWindowData
*/
typedef struct pPWGetWindowData {
	DialogPtr PD;	//The Prefs Dialog
	short id;					//id of pane to deactivate
	char dontClose;			//Set to true if you want to prevent the prefs window from closing.
	char padding;
	short connListNum;	//The connection (0-9) that you are reading data for. (Ignore if you didn't specify hasConnectionsList = true)
} pPWGetWindowData, *pPWGetWindowDataPtr;

/*	pPWItemHitData
*/
typedef struct pPWItemHitData {
	DialogPtr PD;	//The Prefs Dialog
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

/*	pUIUpdateData
*/
typedef struct pUIUpdateData {
	EventRecord *e;
} pUIUpdateData, *pUIUpdateDataPtr;

/*	pUIInGrowData
*/
typedef struct pUIInGrowData {
	WindowPtr window;				//window user is trying to resize
	const EventRecord *e;			//The event record;
} pUIInGrowData, *pUIInGrowDataPtr;

#ifdef _MsgWindows_

/*	pUIMWInfoRefreshData
*/
typedef struct pUIMWInfoRefreshData {
	MWPtr mw;					//The message window that's getting refreshed
	Str255 displayStr;		//If you want ShadowIRC to put in a string, set this. Otherwise, leave it unchanged and do your own drawing
	Rect rect;						//The rect for the status area, in the window's local coordinates.
	char active;					//True if the window is active, false if not.
} pUIMWInfoRefreshData, *pUIMWInfoRefreshDataPtr;

#endif

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
	long refCon;					//The number you passed to DNSLookup()
	struct in_addr ip;
	StringPtr search;
	StringPtr reply;
	char nameToIP;
	char successful;
} pDNSLookupData, *pDNSLookupPtr;

#ifdef _MsgWindows_

#ifdef _channels

/*	pServiceActivateWinData
*/
typedef struct pServiceActivateWinData {
	channelPtr ch;			//Channel that was activated. Nil if the window wasn't a channel
	char activate;			//True if activated, false if deactivated
	char padding[3];
	WindowPtr w;				//The window that was activated/deactivated.
	MWPtr mw;				//If the window is a message window, the MWPtr. Nil otherwise.
} pServiceActivateWinData, *pServiceActivateWinDataPtr;

#endif

#endif

/*	pServiceWindowMenuData
*/
typedef struct pServiceWindowMenuData {
	long key;				//The value selected
	short menuItem;	//The menu item number
} pServiceWindowMenuData, *pServiceWindowMenuPtr;


/*	pServiceULUserhostsData
*/
typedef struct pServiceULUserhostsData {
	linkPtr link;
} pServiceULUserhostsData, *pServiceULUserhostsPtr;

/*	pServiceCWLinkStateChangeData
*/
typedef struct pServiceCWLinkStateChangeData {
	linkPtr link;				//The link whose connection state has changed
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
typedef struct pIPCMessageData {
	long type;					//The IPC type registered with IPCRegister()
	long message;			//A message number.
	long data;					//Data for the message.
	long result;				//A result code to pass back to the calling plugin.
	long client;				//A reference to the plugin that sent a message with IPCSend(). Use with IPCReply()
	char ack;
} pIPCMessageData, *pIPCMessageDataPtr;

/*	pIPCReplyMessageData
*/
typedef struct pIPCReplyMessageData {
	long type;					//IPC class
	long message;			//A message number
	long data;					//Data for the message
	long result;				//Your reply to the IPC service
	char ack;
} pIPCReplyMessageData, *pIPCReplyMessageDataPtr;

/*	pConnectionData
*/
typedef struct pConnectionData {
	connectionPtr conn;	//The connection
	char event;				//The event. see enum connectionStatus
	char sendEvent;		//If true, the event is the result of a ConnPut*()
	LongString *data;		//If event==C_CharsAvailable, and this is a text connection, ptr to data recieved.
} pConnectionData, *pConnectionDataPtr;

#ifdef _InputLine_

typedef struct pIWUpdateData {
	iwWidgetPtr widget;
} pIWUpdateData, *pIWUpdateDataPtr;

/*	pIWClickData
*/
typedef struct pIWClickData {
	iwWidgetPtr widget;
	Point where;
} pIWClickData, *pIWClickDataPtr;

#endif

#ifdef _MsgWindows_

/*	pMWWidgetUpdateData
*/
typedef struct pMWWidgetUpdateData {
	mwWidgetPtr widget;
	char winActive;
} pMWWidgetUpdateData, *pMWWidgetUpdateDataPtr;

/*	pMWWidgetClickData
*/
typedef struct pMWWidgetClickData {
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
	mwPanePtr pane;
} pMWPaneUpdateData, *pMWPaneUpdateDataPtr;

/*	pMWPaneResizeData
*/
typedef struct pMWPaneResizeData {
	mwPanePtr pane;
} pMWPaneResizeData, *pMWPaneResizeDataPtr;

#endif

/*	pSoundServiceData
*/
typedef struct pSoundServiceData {
	long sound;		//The sound type. see enum soundServiceSounds
	long data;			//Data.
} pSoundServiceData, *pSoundServiceDataPtr;

#ifdef _DCC

/*	pDCCStatusData
*/
typedef struct pDCCStatusData {
	dccPtr dcc;				//The dcc you should return status for
	Ptr status;				//Your status
} pDCCStatusData, *pDCCStatusDataPtr;

#endif

/*	pMenuItemSelectedData
*/
typedef struct pMenuItemSelectedData {
	WindowPtr contextWindow;
	short menuID;
	short itemID;
} pMenuItemSelectedData, *pMenuItemSelectedDataPtr;

#pragma mark ¥ misc
/* misc
*/

typedef struct serviceListRec {
	struct serviceListRec *next;
	long serviceClass;		//the actual class of the service
	long serviceType;		//the service registered
	plugsPtr pluginRef;
} serviceListRec, *slPtr;

typedef struct IPCListRec {
	struct IPCListRec *next;
	plugsPtr pluginRef;
	long type;
	long version;
	long data;
} IPCListRec, *IPCPtr;

typedef struct hmItem {
	plugsPtr pluginRef;
	Str63 name;
} hmItem;

typedef struct {
	long num;
	hmItem list[1];
} hmiListRec, *hmiListPtr, **hmiListHand;

typedef plugsRec plugsArray[1];
typedef plugsArray *plugsArrayPtr, **plugsArrayH;

typedef struct wMSData {
	plugsPtr pluginRef;
	long key;
} wMSData, *wMSDataPtr;

typedef struct wMenuServicesRec {
	long numMenus;
	wMSData service[1];
} wMenuServicesRec, *wMenuServicesPtr, **wMenuServicesHand;

extern wMenuServicesHand wMenuServices;

extern FSSpec pluginsFolderFSSpec, pluginPrefsFSSpec;
extern hmiListHand hmiList;
extern ShadowIRCDataRecord sidr;

pascal void makePlugsDB(void);
pascal void runIndPlugin(plugsPtr ref, short message, void* messageData);
pascal void runPlugins(short message, void* messageData);
pascal void runAllPlugins(short message, void* messageData);
pascal void idlePlugins(EventRecord *e);
pascal void runService(short message, void* messageData); //messages to any service
pascal void runIndService(long serviceType, short message, void* messageData); //messages to a specific service

pascal void pluginCloseWindow(WindowPtr win, pluginDlgInfoPtr p);

pascal long FindService(FourCharCode serviceType);
pascal void SoundService(long sound, long data);

pascal short WMSGetMenuItemNum(FourCharCode serviceType);

pascal short HMIAdd(ConstStr63Param name);
pascal DialogPtr pluginNewDialog(short dialogID);
pascal void pluginDisposeDialog(DialogPtr d);
pascal WindowPtr pluginNewWindow(const Rect *boundsRect, ConstStr255Param title, short theProc, long flags);
pascal void pluginDisposeWindow(WindowPtr d);
pascal plugsPtr GetPluginWindowOwner(WindowPtr w);

pascal char NewService(FourCharCode serviceType);
pascal char ExistsService(FourCharCode serviceType);
pascal short IPCRegister(FourCharCode IPCType, long version, long data);
pascal char IPCExists(FourCharCode IPCType, long *version, long *data);
pascal short IPCSend(FourCharCode IPCType, long message, long data, long *result);
pascal char IPCReply(plugsPtr client, FourCharCode IPCType, long message, long data, long *result);
pascal short WMSListAdd(FourCharCode serviceType, ConstStr255Param name);

#endif