/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2000 John Bafford
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

#ifndef _stringlist_
#define _stringlist_

typedef struct STRnList {
	const short num;
	const unsigned char list[1];
} STRnList, *STRnPtr, **STRnHand;

extern STRnPtr spFiles, spCM, spError, spInfo, spDCC, spHelp, spTopic, spAppleURL, spServices, spSOCKS, spServer, spFile, spWhois;

enum stringResources {
	srAboutBox = 128,
	srPreferencesItems = 129,
	srFiles = 130,
	
	srCM = 200,
	srServices = 201,
	srServer = 253,
	srSOCKS = 255,
	srError = 256,
	srInfo = 257,
	srDCC = 258,
	srHelp = 259,
	srTopic = 260,
	srAppleURL = 261,
	srFile = 262,
	srWhois = 263
};

enum filesStrings {
	sPreferencesFolder = 1,
	sPreferencesFile,
	sPluginsFolder
};

enum errorStrings {
	sCClosing = 6,
	sCClosed,
	sOldOrDamagedPrefs,
	sUnrecognizedPrefs,
	sPrefsVersionError, 
	sCChangingServers,
	sCRegistering,
	
	sOpeningIdentd = 14,
	sCIdentdCalled,
	
	sLogsFolderInvalid = 21,
	sErrorLoadingPrefs,
	sPrefsError,
	sNoColonInLinkName,
	
	spCantCreatPluginPrefsFldr=25,
	sPlugsPrefsFolderIsFile,
	sNoPluginsFolder,
	sPlugsFolderEmpty,
	sRegistrationFailed,
	
	sPlugsFldrIsFile = 31,
	sCantMakeLogFolderThisConn,
	sLogFolderThisConnIsFile,
	sShadowIRCLowMem,
	sShadowIRCLowMem2,
	sOnlyOneShadowIRC,
	sNoTopic,
	sNoOpsCantTopic,
	sLowOnMem,
	sNoLinkName,
	sCIdentdClosed,
	sPlugsMemErr,
	sDNSCantLookupYet,
	sDNSNotInUserlist,
	sRanOutOfMemory,
	sNotOnChannel,
	sServerNickMismatch
};

enum InfoStrings {
	sLookingUpIP = 1,
	sIPForIs,
	sIgnoreList,
	sNotIgnoringAnyone,
	sAlreadyIgnoring,
	sIgnoring,
	
	sOnoticeRequiresChannel = 9,
	sCantMakeQuery,
	
	sAlreadyOnNotify = 13,
	sNotifyList,
	spNooneOnNotify,
	sUsersOnNotify,
	sSignedOn,
	sSignedOff,
	sOnline,
	sLookup,
	sOpening,
	sClosing,
	
	sNoTopicSet = 24,
	sTopicStr,
	sTopicSetBy,
	sInactiveChannel,
	sClosingLogFile,
	sOpeningLogFile,
	sChannelCreatedTime,
	sTopicSetBy2,
	sRecentChannels,
	sInputline,
	sNobody,
	sStatsNTimeDelta,
	sBanSetBy,
	sNoBans,
	sNoBans2,
	sListedBans,
	sListedBans2,
	sNoBansMatching1,
	sNoBansMatching2,
	sNoBansMatching3,
	sNoBansMatching4,
	sReconnectAway,
	sSetTopicOn,
	sIsNowKnownAs,
	sRejected,
	sKickedOffBy,
	sTopicLength
};

enum topicStrings {
	sConsole = 1,
	sHelp,
	sQuery,
	sDCCChat,
	sInactiveDCCChat,
	sModelessChannel
};

enum serverStrings {
	
	sRPINGReply = 1,
	sNickInUse,
	sChPasswd,
	sChPasswd2,
	sUserModeIs,
	sChannelURLIs,
	sIsOnChannels,
	sInvitingToChannel,
	sTopicFor,
	sAlreadyOnChannel
};

enum fileStrings {
	sSelectColorFileGetLoc = 1,
	sSelectColorFileSaveLoc,
	sPleaseChooseAFolder,
	sSend
};

enum whoisStrings {
	sWhSignedOnAt = 1,
	sOnIRCVia,
	sWasOnIRCVia,
	sIdleFor
};

#endif