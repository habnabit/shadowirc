/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2003 John Bafford
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

extern STRnPtr spFiles, spCM, spError, spInfo, spDCC, spTopic, spServices, spSOCKS, spServer, spFile, spWhois;

enum stringResources {
	srPreferencesItems = 129,
	srFiles = 130,
	
	srCM = 200,
	srServices = 201,
	srServer = 253,
	srSOCKS = 255,
	srError = 256,
	srInfo = 257,
	srDCC = 258,
	srTopic = 260,
	srFile = 262,
	srWhois = 263
};

enum filesStrings {
	sPreferencesFolder = 1,
	sPreferencesFile,
};

enum errorStrings {
	sCClosing = 6,
	sCClosed,
	sOldOrDamagedPrefs,
	sUnrecognizedPrefs,
	sPrefsVersionError, 
	sCChangingServers,
	sCRegistering,
	sServerRefusesConnection,
	sOpeningIdentd,
	sCIdentdCalled,
	sPrefNoServerSpecified,
	sPrefNoServerPort,
	sPrefNoNick,
	sPrefNoUsername,
	sPrefNoRealname,
	sLogsFolderInvalid,
	sErrorLoadingPrefs,
	sPrefsError,
	sNoColonInLinkName,
	sRegistrationFailed,
	sCantMakeLogFolderThisConn,
	sLogFolderThisConnIsFile,
	sNoTopic,
	sNoOpsCantTopic,
	sNoLinkName,
	sCIdentdClosed,
	sDNSCantLookupYet,
	sDNSNotInUserlist,
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
	sAddedToSilence,
	sRemovedFromSilence,
	sAlreadyOnNotify,
	sNotifyList,
	spNooneOnNotify,
	sUsersOnNotify,
	sSignedOn,
	sSignedOff,
	sOnline,
	sLookup,
	sOpening,
	sClosing,
	sAway,
	sNoTopicSet,
	sTopicStr,
	sTopicSetBy,
	sInactiveChannel,
	sClosingLogFile,
	sOpeningLogFile,
	sChannelCreatedTime,
	sTopicSetBy2,
	sRecentChannels,
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
	sTopicLength,
	sServerTime
};

enum topicStrings {
	sConsole = 1,
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
	sAlreadyOnChannel,
	sNickJoinedChannel,
	sModeChange,
	sGotPongFor,
	sInvitesYouToChannel,
	sReceivedChannelCTCP,
	sReceivedCTCP,
	sCPINGTime,
	sCTCPReply,
	sUserHasSignedOff
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
	sIdleFor,
	sIsLoggedInAs
};

#endif