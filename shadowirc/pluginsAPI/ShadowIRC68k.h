/*	ShadowIRC Plugins Header File - 68k Interfaces
		Version 1.1
		© John Bafford 1997-2000. All Rights Reserved.
		dshadow@shadowirc.com
		http://www.shadowirc.com

		This file contains the necessary prototypes and glue for 68k plugins to work.
		You shouldn't ever need to see anything in this file. You do not need to include it.

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

#ifndef __ShadowIRC_68k_Headers__
#define __ShadowIRC_68k_Headers__
#if !__POWERPC__
#ifndef __A4STUFF__
#include "A4Stuff.h"
#endif
#pragma mark proclist
#define procUndocumentedAPI 0
#define procLSDelete 1
#define procLSCopy 2
#define procLSCopyString 3
#define procLSPosCustom 4
#define procLSPos 5
#define procLSPosChar 6
#define procLSNextArg 7
#define procLSNextArgND 8
#define procLSStrLS 9
#define procLSConcatLSAndStr 10
#define procLSConcatStrAndLS 11
#define procLSConcatStrAndStr 12
#define procLSConcatLSAndLS 13
#define procLSConcatLSAndStrAndLS 14
#define procLSConcatStrAndLSAndStr 15
#define procLSConcatStrAndStrAndStr 16
#define procLSStrCat 17
#define procLSInsertStr 18
#define procSMPrefix 19
#define procSMPrefixColor 20
#define procLineMsg 21
#define procUpdateStatusLine 22
#define procSetInputLineCursorSelection 23
#define procGetInputLine 24
#define procGetLine 25
#define procSetInputLine 26
#define procChannelMsg 27
#define procMessage 28
#define procNewPluginMWindow 29
#define procMWSetDimen 30
#define procMWSetFontSize 31
#define procMWDelete 32
#define procMWMessage 33
#define procDoModeLWindow 34
#define procEvtMW 35
#define procMWColor 36
#define procPluginNewDialog 37
#define procPluginDisposeDialog 38
#define procIsChannel 39
#define procIsDCCName 40
#define procMakeChannel 41
#define procChFind 42
#define procChFindBan 43
#define procChMatchBan 44
#define procChGetBan 45
#define procMWIrcleColor 46
#define procChKillBan 47
#define procChKillAllBans 48
#define procULAddUser 49
#define procULDeleteUser 50
#define procULFindUserName 51
#define procChannelWindow 52
#define procChMsg 53
#define procParamString 54
#define procSecsToHMS 55
#define procDeleteConnection 56
#define procFindConnectionSock 57
#define procCMAddSubmenu 58
#define procPluginNewConnection 59
#define procPutServer 60
#define procDoONotice 61
#define procSendCTCPReply 62
#define procPMLAdd 63
#define procNextArg 64
#define procHandleCommand 65
#define procWriteMainPrefs 66
#define procWriteAllFiles 67
#define procUlongval 68
#define procCMSetCheckmark 69
#define procUlongstr 70
#define procDccTypToStr 71
#define procDccFlagTypToStr 72
#define procDccStrToTyp 73
#define procHostToIPStr 74
#define procDCCFind 75
#define procDCCClose 76
#define procDCCWindowClose 77
#define procDCCCreate 78
#define procDCCRequest 79
#define procDCCOpen 80
#define procDCCCommand 81
#define procDCCSSay 82
#define procSendCommand 83
#define procDNSLookup 84
#define procDoBroadcast 85
#define procListIgnores 86
#define procIsIgnored 87
#define procDoIgnore 88
#define procFindIgnore 89
#define procProcessShortcutText 90
#define procMBIdle 91
#define procMBFindNick 92
#define procMBNewMessage 93
#define procUpc 94
#define procUcase 95
#define procPdelete 96
#define procPinsert 97
#define procPstrcpy 98
#define procPstrcmp 99
#define procExistsService 100
#define procStandardDialogFilter 101
#define procPadEnd 102
#define procPadBegin 103
#define procMaskMatch 104
#define procMakeMask 105
#define procCountChar 106
#define procRevPos 107
#define procIsNumber 108
#define procIsIPNumber 109
#define procGetText 110
#define procSetText 111
#define procSetCheckBox 112
#define procGetCheckBox 113
#define procSetButtonEnable 114
#define procGetControlHandle 115
#define procGetFirstSelectedCell 116
#define procSelectOneCell 117
#define procDrawListBorder 118
#define procMakeCellVisible 119
#define procOpenConnection 120
#define procDrawPlacard 121
#define procSetTextColor 122
#define procSetDlogFont 123
#define procGetControlMenu 124
#define procLSPosCase 125
#define procPos 126
#define procShadowIRCVersion 127
#define procPluginNewWindow 128
#define procPluginDisposeWindow 129
#define procNewService 130
#define procPFOpen 131
#define procPFExists 132
#define procPFCreate 133
#define procPFWrite 134
#define procPFRead 135
#define procPFSize 136
#define procPFResize 137
#define procPFSetPos 138
#define procPFClose 139
#define procPFDelete 140
#define procFrontNonFloater 141
#define procGetPluginWindowOwner 142
#define procWIsFloater 143
#define procWMoveToFront 144
#define procWSelect 145
#define procWDrag 146
#define procWShow 147
#define procWHide 148
#define procPstrcasecmp 149
#define procPstrcasecmp2 150
#define procGetQD 151
#define procWMove 152
#define procEnterModalDialog 153
#define procExitModalDialog 154
#define procLSConcatLSAndStrAndStr 155
#define procIPStringToLong 156
#define procSetSmartScrollInfo 157
#define procSetSmartScrollProp 158
#define procGetSmartScrollProp 159
#define procLCAttach 160
#define procLCDetach 161
#define procLCSetValue 162
#define procLCSetMin 163
#define procLCSetMax 164
#define procLCGetValue 165
#define procLCGetMin 166
#define procLCGetMax 167
#define procLCSynch 168
#define procIsChannelValid 169
#define procIsUserValid 170
#define procConnPut 171
#define procConnPutText 172
#define procConnPutLS 173
#define procStackModes 174
#define procOpenPreferencesWindow 175
#define procSMPrefixIrcleColor 176
#define procAsyncSoundPlay 177
#define procLSCmp 178
#define procLSCaseCmp 179
#define procLSGetIndString 180
#define procLSParamString 181
#define procLSParamLS 182
#define procSMLink 183
#define procSMPrefixLink 184
#define procSMPrefixLinkColor 185
#define procSMPrefixLinkIrcleColor 186
#define procIPCRegister 187
#define procIPCExists 188
#define procIPCCall 189
#define procDoNotify 190
#define procRunNotify 191
#define procListNotify 192
#define procfindNotify 193
#define procdeleteNotify 194
#define procaddNotify 195
#define procDrawMWinStatus 196
#define procConnClose 197
#define procConnAbort 198
#define procConnNewActive 199
#define procConnNewPassive 200
#define procConnGetData 201
#define procConnCharsAvail 202
#define procIWNewObject 203
#define procIWRecalculateRects 204
#define procIWPopUpMenu 205
#define procIWDeleteObject 206
#define procIWOverride 207
#define procMWNewWidget 208
#define procMWRecalculateRects 209
#define procMWDestroyWidget 210
#define procHMIAdd 211
#define procMWFindWidget 212
#define procMWFindPane 213
#define procMWNewPane 214
#define procMWPaneRecalculate 215
#define procMWPaneResize 216
#define procStartDrawingOffscreen 217
#define procEndDrawingOffscreen 218
#define procAbortDrawingOffscreen 219
#define procMWDestroyPane 220
#define procLSInsertChar 221
#define procLSInsertShort 222
#define procLSInsertLong 223
#define procLightenColor 224
#define procNavDialogFilter 225
#define procDirectorySelectButton 226
#define procCleanFolderFSp 227
#define procDoDCCSend 228
#define procDoDCCSendFile 229
#define procDCCGetAFile 230
#define procInputHandler 231
#define procOpenURL 232
#define procDCCTypeAdd 233
#define procDCCGetStatus 234
#define procProcessLine 235
#define procProcessInputHunk 236
#define procDragHilightRect 237
#define procDragHilightRectBG 238
#define procDragIsTypeAvail 239
#define procDragGetType 240
#define procDragGetPtr 241
#define procDragAddPtr 242
#define procDCCSendCookie 243
#define procDCCSendCookieReply 244
#define procCMAdd 245
#define procIPCReply 246
#define procLSPosCaseCustom 247
#define procSetBackground 248
#define procMySendControlMessage 249
#define procSMPrefixRGBColor 250
#define procSMPrefixRGBLinkColor 251
#define procpluginSetWRefCon 252
#define procpluginGetWRefCon 253
#define procMWPaneUpdate 254
#define procShadowIRCVersion2 255
#define procSetDlogItemTextHdl 256
#define procDrawBorder 257
#define procFileAdd 258
#define procFileClose 259
#define procIsIgnoredNickUser 260
#define procWMSGetMenuItemNum 261
#define procGetInputLineCursorSelection 262
#define procLockInput 263
#define procUnlockInput 264
#define procDoServer 265
#define procDoSignoff 266
#define procWMSListAdd 267
#define procILGetLine 268
#define procILSetLine 269
#define procMWGetName 270
#define procMWGetChannel 271
#define procMWGetDCC 272
#define procDoModeKWindow 273
#define procConnGetLocalPort 274
#define procRegisterAETE 275
#define procNickListAdd 276
#define procChannelListAdd 277
#define procGetDrawingState 278
#define procSetDrawingState 279
#define procNormalizeDrawingState 280
#define procGetAppearanceListBoxHandle 281
#define procCopyResource 282
#define procPstrcat 283
#define procMWPart 284
#define procWindowClose 285

#pragma mark fnlist
typedef pascal long (*UndocumentedAPIPtr)(long, long);
#define _UndocumentedAPI ((UndocumentedAPIPtr)(sidr->procs[procUndocumentedAPI]))
typedef pascal void (*LSDeletePtr)(LongString*, short, short);
#define LSDelete ((LSDeletePtr)(sidr->procs[procLSDelete]))
typedef pascal void (*LSCopyPtr)(const LongString*, short, short, LongString*);
#define LSCopy ((LSCopyPtr)(sidr->procs[procLSCopy]))
typedef pascal void (*LSCopyStringPtr)(const LongString*, short, short, StringPtr);
#define LSCopyString ((LSCopyStringPtr)(sidr->procs[procLSCopyString]))
typedef pascal short (*LSPosCustomPtr)(StringPtr, LongString*, short);
#define LSPosCustom ((LSPosCustomPtr)(sidr->procs[procLSPosCustom]))
typedef pascal short (*LSPosPtr)(StringPtr, LongString*);
#define LSPos ((LSPosPtr)(sidr->procs[procLSPos]))
typedef pascal short (*LSPosCharPtr)(short, LongString*);
#define LSPosChar ((LSPosCharPtr)(sidr->procs[procLSPosChar]))
typedef pascal void (*LSNextArgPtr)(LongString*, StringPtr);
#define LSNextArg ((LSNextArgPtr)(sidr->procs[procLSNextArg]))
typedef pascal void (*LSNextArgNDPtr)(LongString*, StringPtr arg);
#define LSNextArgND ((LSNextArgNDPtr)(sidr->procs[procLSNextArgND]))
typedef pascal void (*LSStrLSPtr)(StringPtr, LongString*);
#define LSStrLS ((LSStrLSPtr)(sidr->procs[procLSStrLS]))
typedef pascal void (*LSConcatLSAndStrPtr)(const LongString*, StringPtr, LongString*);
#define LSConcatLSAndStr ((LSConcatLSAndStrPtr)(sidr->procs[procLSConcatLSAndStr]))
typedef pascal void (*LSConcatStrAndLSPtr)(StringPtr, const LongString*, LongString*);
#define LSConcatStrAndLS ((LSConcatStrAndLSPtr)(sidr->procs[procLSConcatStrAndLS]))
typedef pascal void (*LSConcatStrAndStrPtr)(StringPtr, StringPtr, LongString*);
#define LSConcatStrAndStr ((LSConcatStrAndStrPtr)(sidr->procs[procLSConcatStrAndStr]))
typedef pascal void (*LSConcatLSAndLSPtr)(LongString*, LongString*, LongString*);
#define LSConcatLSAndLS ((LSConcatLSAndLSPtr)(sidr->procs[procLSConcatLSAndLS]))
typedef pascal void (*LSConcatLSAndStrAndLSPtr)(LongString*, StringPtr, LongString*, LongString*);
#define LSConcatLSAndStrAndLS ((LSConcatLSAndStrAndLSPtr)(sidr->procs[procLSConcatLSAndStrAndLS]))
typedef pascal void (*LSConcatStrAndLSAndStrPtr)(StringPtr, LongString*, StringPtr, LongString*);
#define LSConcatStrAndLSAndStr ((LSConcatStrAndLSAndStrPtr)(sidr->procs[procLSConcatStrAndLSAndStr]))
typedef pascal void (*LSConcatStrAndStrAndStrPtr)(StringPtr, StringPtr, StringPtr, LongString*);
#define LSConcatStrAndStrAndStr ((LSConcatStrAndStrAndStrPtr)(sidr->procs[procLSConcatStrAndStrAndStr]))
typedef void (*LSStrCatPtr)(short,LongString*,...);
#define LSStrCat ((LSStrCatPtr)(sidr->procs[procLSStrCat]))
typedef pascal void (*LSInsertStrPtr)(ConstStr255Param, short, LongString*);
#define LSInsertStr ((LSInsertStrPtr)(sidr->procs[procLSInsertStr]))
typedef pascal void (*SMPrefixPtr)(LongString*, short);
#define SMPrefix ((SMPrefixPtr)(sidr->procs[procSMPrefix]))
typedef pascal void (*SMPrefixColorPtr)(LongString*, short, short);
#define SMPrefixColor ((SMPrefixColorPtr)(sidr->procs[procSMPrefixColor]))
typedef pascal void (*LineMsgPtr)(LongString*);
#define LineMsg ((LineMsgPtr)(sidr->procs[procLineMsg]))
typedef pascal void (*UpdateStatusLinePtr)(void);
#define UpdateStatusLine ((UpdateStatusLinePtr)(sidr->procs[procUpdateStatusLine]))
typedef pascal void (*SetInputLineCursorSelectionPtr)(long, long);
#define SetInputLineCursorSelection ((SetInputLineCursorSelectionPtr)(sidr->procs[procSetInputLineCursorSelection]))
typedef pascal void (*GetInputLinePtr)(LongString*);
#define GetInputLine ((GetInputLinePtr)(sidr->procs[procGetInputLine]))
typedef pascal void (*GetLinePtr)(char, MWPtr);
#define GotLine ((GotLinePtr)(sidr->procs[procGotLine]))
typedef pascal void (*SetInputLinePtr)(LongString*);
#define SetInputLine ((SetInputLinePtr)(sidr->procs[procSetInputLine]))
typedef pascal void (*ChannelMsgPtr)(linkPtr, StringPtr, LongString*);
#define ChannelMsg ((ChannelMsgPtr)(sidr->procs[procChannelMsg]))
typedef pascal void (*MessagePtr)(LongString*);
#define Message ((MessagePtr)(sidr->procs[procMessage]))
typedef pascal MWPtr (*NewPluginMWindowPtr)(StringPtr title);
#define NewPluginMWindow ((NewPluginMWindowPtr)(sidr->procs[procNewPluginMWindow]))
typedef pascal void (*MWSetDimenPtr)(MWPtr, short, short, short, short);
#define MWSetDimen ((MWSetDimenPtr)(sidr->procs[procMWSetDimen]))
typedef pascal void (*MWSetFontSizePtr)(MWPtr, short, short);
#define MWSetFontSize ((MWSetFontSizePtr)(sidr->procs[procMWSetFontSize]))
typedef pascal void (*MWDeletePtr)(MWPtr);
#define MWDelete ((MWDeletePtr)(sidr->procs[procMWDelete]))
typedef pascal void (*MWMessagePtr)(MWPtr, LongString*);
#define MWMessage ((MWMessagePtr)(sidr->procs[procMWMessage]))
typedef pascal void (*DoModeLWindowPtr)(channelPtr, LongString*);
#define DoModeLWindow ((DoModeLWindowPtr)(sidr->procs[procDoModeLWindow]))
typedef pascal MWPtr (*EvtMWPtr)(WindowPtr);
#define MWFromWindow ((EvtMWPtr)(sidr->procs[procEvtMW]))
typedef pascal void (*MWColorPtr)(LongString*, short);
#define MWColor ((MWColorPtr)(sidr->procs[procMWColor]))
typedef pascal DialogPtr (*pluginNewDialogPtr)(short);
#define pluginNewDialog ((pluginNewDialogPtr)(sidr->procs[procPluginNewDialog]))
typedef pascal void (*pluginDisposeDialogPtr)(DialogPtr);
#define pluginDisposeDialog ((pluginDisposeDialogPtr)(sidr->procs[procPluginDisposeDialog]))
typedef pascal WindowPtr (*pluginNewWindowPtr)(Rect*, StringPtr, short, long);
#define pluginNewWindow ((pluginNewWindowPtr)(sidr->procs[procPluginNewWindow]))
typedef pascal void (*pluginDisposeWindowPtr)(WindowPtr);
#define pluginDisposeWindow ((pluginDisposeWindowPtr)(sidr->procs[procPluginDisposeWindow]))
typedef pascal char (*IsChannelPtr)(StringPtr);
#define IsChannel ((IsChannelPtr)(sidr->procs[procIsChannel]))
typedef pascal char (*IsDCCNamePtr)(StringPtr);
#define IsDCCName ((IsDCCNamePtr)(sidr->procs[procIsDCCName]))
typedef pascal void (*MakeChannelPtr)(StringPtr);
#define MakeChannel ((MakeChannelPtr)(sidr->procs[procMakeChannel]))
typedef pascal channelPtr (*ChFindPtr)(StringPtr, linkPtr);
#define ChFind ((ChFindPtr)(sidr->procs[procChFind]))
typedef pascal bansP (*ChFindBanPtr)(StringPtr, channelPtr, char);
#define ChFindBan ((ChFindBanPtr)(sidr->procs[procChFindBan]))
typedef pascal bansP (*ChMatchBanPtr)(channelPtr, StringPtr, char);
#define ChMatchBan ((ChMatchBanPtr)(sidr->procs[procChMatchBan]))
typedef pascal char (*ChGetBanPtr)(channelPtr, StringPtr, bansP*, char);
#define ChGetBan ((ChGetBanPtr)(sidr->procs[procChGetBan]))
typedef pascal void (*MWIrcleColorPtr)(LongString*, short);
#define MWIrcleColor ((MWIrcleColorPtr)(sidr->procs[procMWIrcleColor]))
typedef pascal void (*ChKillBanPtr)(channelPtr, bansP*, char);
#define ChKillBan ((ChKillBanPtr)(sidr->procs[procChKillBan]))
typedef pascal void (*ChKillAllBansPtr)(channelPtr, char);
#define ChKillAllBans ((ChKillAllBansPtr)(sidr->procs[procChKillAllBans]))
typedef pascal UserListPtr (*ULAddUserPtr)(channelPtr, StringPtr, StringPtr);
#define ULAddUser ((ULAddUserPtr)(sidr->procs[procULAddUser]))
typedef pascal void (*ULDeleteUserPtr)(UserListPtr);
#define ULDeleteUser ((ULDeleteUserPtr)(sidr->procs[procULDeleteUser]))
typedef pascal UserListPtr (*ULFindUserNamePtr)(channelPtr, StringPtr);
#define ULFindUserName ((ULFindUserNamePtr)(sidr->procs[procULFindUserName]))
typedef pascal MWPtr (*ChannelWindowPtr)(linkPtr, StringPtr);
#define ChannelWindow ((ChannelWindowPtr)(sidr->procs[procChannelWindow]))
typedef void (*ChMsgPtr)(channelPtr, LongString*);
#define ChMsg ((ChMsgPtr)(sidr->procs[procChMsg]))
typedef pascal OSErr (*ParamStringPtr)(StringPtr, StringPtr, StringPtr, StringPtr, StringPtr);
#define ParamString ((ParamStringPtr)(sidr->procs[procParamString]))
typedef pascal void (*SecsToHMSPtr)(long, LongString*);
#define SecsToHMS ((SecsToHMSPtr)(sidr->procs[procSecsToHMS]))
typedef pascal void (*deleteConnectionPtr)(connectionPtr*);
#define deleteConnection ((deleteConnectionPtr)(sidr->procs[procDeleteConnection]))
typedef pascal connectionPtr (*findConnectionSockPtr)(long);
#define findConnectionSock ((findConnectionSockPtr)(sidr->procs[procFindConnectionSock]))
typedef pascal void (*CMAddSubmenuPtr)(pCMPopupsDataPtr, Str255, short*, MenuHandle*);
#define CMAddSubmenu ((CMAddSubmenuPtr)(sidr->procs[procCMAddSubmenu]))
typedef pascal connectionPtr (*pluginNewConnectionPtr)(char);
#define pluginNewConnection ((pluginNewConnectionPtr)(sidr->procs[procPluginNewConnection]))
typedef pascal void (*putServerPtr)(linkPtr, LongString*);
#define putServer ((putServerPtr)(sidr->procs[procPutServer]))
typedef pascal void (*DoONoticePtr)(channelPtr, LongString*);
#define DoONotice ((DoONoticePtr)(sidr->procs[procDoONotice]))
typedef pascal void (*SendCTCPReplyPtr)(linkPtr, ConstStr255Param, ConstStr255Param, LongString*);
#define SendCTCPReply ((SendCTCPReplyPtr)(sidr->procs[procSendCTCPReply]))
typedef pascal short (*PMLAddPtr)(StringPtr);
#define PMLAdd ((PMLAddPtr)(sidr->procs[procPMLAdd]))
typedef pascal void (*NextArgPtr)(StringPtr, StringPtr);
#define NextArg ((NextArgptr)(sidr->procs[procNextArg]))
typedef pascal void (*HandleCommandPtr)(linkPtr, LongString*);
#define HandleCommand ((HandleCommandPtr)(sidr->procs[procHandleCommand]))
typedef pascal void (*writeMainPrefsPtr)(void);
#define writeMainPrefs ((writeMainPrefsPtr)(sidr->procs[procWriteMainPrefs]))
typedef pascal void (*writeAllFilesPtr)(void);
#define writeAllFiles ((writeAllFilesPtr)(sidr->procs[procWriteAllFiles]))
typedef pascal long (*ulongvalPtr)(StringPtr);
#define ulongval ((ulongvalPtr)(sidr->procs[procUlongval]))
typedef pascal void (*CMSetCheckmarkPtr)(pCMPopupsDataPtr d, long id, char checked);
#define CMSetCheckmark ((CMSetCheckmarkPtr)(sidr->procs[procCMSetCheckmark]))
typedef pascal void (*ulongstrPtr)(long, StringPtr);
#define ulongstr ((ulongstrPtr)(sidr->procs[procUlongstr]))
typedef pascal ConstStringPtr (*dccTypToStrPtr)(short);
#define dccTypToStr ((dccTypToStrPtr)(sidr->procs[procDccTypToStr]))
typedef pascal ConstStringPtr (*dccFlagTypToStrPtr)(short);
#define dccFlagTypToStr ((dccFlagTypToStrPtr)(sidr->procs[procDccFlagTypToStr]))
typedef pascal short (*dccStrToTypPtr)(StringPtr);
#define DCCTypeFind ((dccStrToTypPtr)(sidr->procs[procDccStrToTyp]))
typedef pascal void (*hostToIPStrPtr)(long, StringPtr);
#define hostToIPStr ((hostToIPStrPtr)(sidr->procs[procHostToIPStr]))
typedef pascal connectionPtr (*DCCFindPtr)(linkPtr, short, ConstStr255Param);
#define DCCFind ((DCCFindPtr)(sidr->procs[procDCCFind]))
typedef pascal void (*DCCClosePtr)(connectionPtr*, char);
#define DCCClose ((DCCClosePtr)(sidr->procs[procDCCClose]))
typedef pascal void (*DCCWindowClosePtr)(Ptr);
#define DCCWindowClose ((DCCWindowClosePtr)(sidr->procs[procDCCWindowClose]))
typedef pascal char (*DCCCreatePtr)(linkPtr, short, StringPtr, connectionPtr*);
#define DCCCreate ((DCCCreate)(sidr->procs[procDCCCreate]))
typedef pascal void (*DCCRequestPtr)(linkPtr, StringPtr, StringPtr, StringPtr);
#define DCCRequest ((DCCRequestPtr)(sidr->procs[procDCCRequest]))
typedef pascal void (*DCCOpenPtr)(connectionPtr*);
#define DCCOpen ((DCCOpenPtr)(sidr->procs[procDCCOpen]))
typedef pascal void (*DCCCommandPtr)(linkPtr, StringPtr);
#define DCCCommand ((DCCCommandPtr)(sidr->procs[procDCCCommand]))
typedef pascal void (*DCCSSayPtr)(connectionPtr*, LongString*);
#define DCCSSay ((DCCSSayPtr)(sidr->procs[procDCCSSay]))
typedef pascal void (*SendCommandPtr)(linkPtr, LongString*);
#define SendCommand ((SendCommandPtr)(sidr->procs[procSendCommand]))
typedef pascal void (*DNSLookupPtr)(StringPtr, long);
#define DNSLookup ((DNSLookupPtr)(sidr->procs[procDNSLookup]))
typedef pascal void (*doBroadcastPtr)(linkPtr, LongString*, char);
#define doBroadcast ((doBroadcastPtr)(sidr->procs[procDoBroadcast]))
typedef pascal void (*ListIgnoresPtr)(void);
#define ListIgnores ((ListIgnoresPtr)(sidr->procs[procListIgnores]))
typedef pascal char (*IsIgnoredPtr)(Str255);
#define IsIgnored ((IsIgnoredPtr)(sidr->procs[procIsIgnored]))
typedef pascal void (*DoIgnorePtr)(LongString *);
#define DoIgnore ((DoIgnorePtr)(sidr->procs[procDoIgnore]))
typedef pascal ignorePtr (*findIgnorePtr)(Str255, ignorePtr);
#define findIgnore ((findIgnorePtr)(sidr->procs[procFindIgnore]))
typedef pascal void (*ProcessShortcutTextPtr)(LongString*, short*, short*);
#define ProcessShortcutText ((ProcessShortcutTextPtr)(sidr->procs[procProcessShortcutText]))
typedef pascal void (*MBIdlePtr)(void);
#define MBIdle ((MBIdlePtr)(sidr->procs[procMBIdle]))
typedef pascal messageBufferPtr (*MBFindNickPtr)(linkPtr, StringPtr);
#define MBFindNick ((MBFindNickPtr)(sidr->procs[procMBFindNick]))
typedef pascal void (*MBNewMessagePtr)(linkPtr, StringPtr);
#define MBNewMessage ((MBNewMessagePtr)(sidr->procs[procMBNewMessage]))
typedef pascal char (*upcPtr)(char);
#define upc ((upcPtr)(sidr->procs[procUpc]))
typedef pascal void (*ucasePtr)(StringPtr);
#define ucase ((ucasePtr)(sidr->procs[procUcase]))
typedef pascal void (*pdeletePtr)(StringPtr, short, short);
#define pdelete ((pdeletePtr)(sidr->procs[procPdelete]))
typedef pascal char (*pinsertPtr)(StringPtr,StringPtr,short);
#define pinsert ((pinsertPtr)(sidr->procs[procPinsert]))
typedef pascal char (*pstrcpyPtr)(ConstStringPtr,StringPtr);
#define pstrcpy ((pstrcpyPtr)(sidr->procs[procPstrcpy]))
typedef pascal char (*pstrcmpPtr)(ConstStringPtr,StringPtr);
#define pstrcmp ((pstrcmpPtr)(sidr->procs[procPstrcmp]))
typedef pascal char (*ExistsServicePtr)(long);
#define ExistsService ((ExistsServicePtr)(sidr->procs[procExistsService]))
typedef pascal char (*StandardDialogFilterPtr)(DialogPtr, EventRecord*, short*);
#define StandardDialogFilter ((StandardDialogFilterPtr)(sidr->procs[procStandardDialogFilter]))
typedef pascal void (*padEndPtr)(StringPtr, short, StringPtr);
#define padEnd ((padEndPtr)(sidr->procs[procPadEnd]))
typedef pascal void (*padBeginPtr)(StringPtr, short, StringPtr);
#define padBegin ((padBeginPtr)(sidr->procs[procPadBegin]))
typedef pascal char (*maskMatchPtr)(StringPtr, StringPtr);
#define maskMatch ((maskMatchPtr)(sidr->procs[procMaskMatch]))
typedef pascal void (*makeMaskPtr)(StringPtr, StringPtr);
#define makeMask ((makeMaskPtr)(sidr->procs[procMakeMask]))
typedef pascal short (*countCharPtr)(short,StringPtr);
#define countChar ((countCharPtr)(sidr->procs[procCountChar]))
typedef pascal short (*revPosPtr)(short, StringPtr);
#define revPos ((revPosPtr)(sidr->procs[procRevPos]))
typedef pascal char (*isNumberPtr)(StringPtr);
#define isNumber ((isNumberPtr)(sidr->procs[procIsNumber]))
typedef pascal char (*isIPNumberPtr)(StringPtr);
#define isIPNumber ((isIPNumberPtr)(sidr->procs[procIsIPNumber]))
typedef pascal void (*GetTextPtr)(DialogPtr, short, StringPtr);
#define GetText ((GetTextPtr)(sidr->procs[procGetText]))
typedef pascal void (*SetTextPtr)(DialogPtr, short, StringPtr);
#define SetText ((SetTextPtr)(sidr->procs[procSetText]))
typedef pascal void (*setCheckBoxPtr)(DialogPtr, short, char);
#define setCheckBox ((setCheckBoxPtr)(sidr->procs[procSetCheckBox]))
typedef pascal char (*getCheckBoxPtr)(DialogPtr, short);
#define getCheckBox ((getCheckBoxPtr)(sidr->procs[procGetCheckBox]))
typedef pascal void (*setButtonEnablePtr)(DialogPtr, short, char);
#define setButtonEnable ((setButtonEnablePtr)(sidr->procs[procSetButtonEnable]))
typedef pascal ControlHandle(*GetControlHandlePtr)(DialogPtr, short);
#define GetControlHandle ((GetControlHandlePtr)(sidr->procs[procGetControlHandle]))
typedef pascal char(*getFirstSelectedCellPtr)(ListHandle, Cell*);
#define getFirstSelectedCell ((getFirstSelectedCellPtr)(sidr->procs[procGetFirstSelectedCell]))
typedef pascal void(*selectOneCellPtr)(ListHandle, Cell);
#define selectOneCell ((selectOneCellPtr)(sidr->procs[procSelectOneCell]))
typedef pascal void(*drawListBorderPtr)(ListHandle);
#define drawListBorder ((drawListBorderPtr)(sidr->procs[procDrawListBorder]))
typedef pascal void (*makeCellVisiblePtr)(ListHandle, Cell);
#define makeCellVisible ((makeCellVisiblePtr)(sidr->procs[procMakeCellVisible]))
typedef pascal char (*OpenConnectionPtr)(linkPtr link);
#define OpenConnection ((OpenConnectionPtr)(sidr->procs[procOpenConnection]))
typedef pascal void (*DrawPlacardPtr)(Rect*, long);
#define DrawPlacard ((DrawPlacardPtr)(sidr->procs[procDrawPlacard]))
typedef pascal void (*SetTextColorPtr)(short);
#define SetTextColor ((SetTextColorPtr)(sidr->procs[procSetTextColor]))
typedef pascal void (*SetDlogFontPtr)(DialogPtr);
#define SetDlogFont ((SetDlogFontPtr)(sidr->procs[procSetDlogFont]))
typedef pascal MenuHandle(*GetControlMenuPtr)(ControlHandle);
#define GetControlMenu ((GetControlMenuPtr)(sidr->procs[procGetControlMenu]))
typedef pascal short (*LSPosCasePtr)(StringPtr, LongString*);
#define LSPosCase ((LSPosCasePtr)(sidr->procs[procLSPosCase]))
typedef pascal short (*posPtr)(short, StringPtr);
#define pos ((posPtr)(sidr->procs[procPos]))
typedef pascal long (*ShadowIRCVersionPtr)(StringPtr);
#define ShadowIRCVersion ((ShadowIRCVersionPtr)(sidr->procs[procShadowIRCVersion]))
typedef pascal char (*NewServicePtr)(FourCharCode);
#define NewService ((NewServicePtr)(sidr->procs[procNewService]))
typedef pascal OSErr (*PFOpenPtr)(StringPtr, char, short*, FSSpec*);
#define PFOpen ((PFOpenPtr)(sidr->procs[procPFOpen]))
typedef pascal char (*PFExistsPtr)(StringPtr);
#define PFExists ((PFExistsPtr)(sidr->procs[procPFExists]))
typedef pascal OSErr (*PFCreatePtr)(StringPtr, FourCharCode, FourCharCode, char);
#define PFCreate ((PFCreatePtr)(sidr->procs[procPFCreate]))
typedef pascal OSErr (*PFWritePtr)(short , void*, long*);
#define PFWrite ((PFWritePtr)(sidr->procs[procPFWrite]))
typedef pascal OSErr (*PFReadPtr)(short, void*, long*);
#define PFRead ((PFReadPtr)(sidr->procs[procPFRead]))
typedef pascal OSErr (*PFSizePtr)(short, long*);
#define PFSize ((PFSizePtr)(sidr->procs[procPFSize]))
typedef pascal OSErr (*PFResizePtr)(short,  long);
#define PFResize ((PFResizePtr)(sidr->procs[procPFResize]))
typedef pascal OSErr (*PFSetPosPtr)(short, long);
#define PFSetPos ((PFSetPosPtr)(sidr->procs[procPFSetPos]))
typedef pascal OSErr (*PFClosePtr)(short);
#define PFClose ((PFClosePtr)(sidr->procs[procPFClose]))
typedef pascal OSErr (*PFDeletePtr)(StringPtr);
#define PFDelete ((PFDeletePtr)(sidr->procs[procPFDelete]))
typedef pascal WindowPtr (*FrontNonFloaterPtr)(void);
#define FrontNonFloater ((FrontNonFloaterPtr)(sidr->procs[procFrontNonFloater]))
typedef pascal void* (*GetPluginWindowOwnerPtr)(WindowPtr);
#define GetPluginWindowOwner ((GetPluginWindowOwnerPtr)(sidr->procs[procGetPluginWindowOwner]))
typedef pascal char (*WIsFloaterPtr)(WindowPtr);
#define WIsFloater ((WIsFloaterPtr)(sidr->procs[procWIsFloater]))
typedef pascal void (*WMoveToFrontPtr)(WindowPtr);
#define WMoveToFront ((WMoveToFrontPtr)(sidr->procs[procWMoveToFront]))
typedef pascal void (*WSelectPtr)(WindowPtr);
#define WSelect ((WSelectPtr)(sidr->procs[procWSelect]))
typedef pascal void (*WDragPtr)(WindowPtr, Point, Rect*);
#define WDrag ((WDragPtr)(sidr->procs[procWDrag]))
typedef pascal void (*WShowPtr)(WindowPtr);
#define WShow ((WShowPtr)(sidr->procs[procWShow]))
typedef pascal void (*WHidePtr)(WindowPtr);
#define WHide ((WHidePtr)(sidr->procs[procWHide]))
typedef pascal char (*pstrcasecmpPtr)(ConstStr255Param s1, ConstStr255Param s2);
#define pstrcasecmp ((pstrcasecmpPtr)(sidr->procs[procPstrcasecmp]))
typedef pascal char (*pstrcasecmp2Ptr)(ConstStr255Param s1, ConstStr255Param s2);
#define pstrcasecmp2 ((pstrcasecmp2Ptr)(sidr->procs[procPstrcasecmp2]))
typedef pascal void (*GetQDPtr)(QDGlobals**);
#define GetQD ((GetQDPtr)(sidr->procs[procGetQD]))
typedef pascal void (*WindowMovePtr)(WindowPtr, short, short, char);
#define WMove ((WindowMovePtr)(sidr->procs[procWMove]))
typedef pascal void (*EnterModalDialogPtr)(void);
#define EnterModalDialog ((EnterModalDialogPtr)(sidr->procs[procEnterModalDialog]))
typedef pascal void (*ExitModalDialogPtr)(void);
#define ExitModalDialog ((ExitModalDialogPtr)(sidr->procs[procExitModalDialog]))
typedef pascal void (*LSConcatLSAndStrAndStrPtr)(LongString*, StringPtr, StringPtr, LongString*);
#define LSConcatLSAndStrAndStr ((LSConcatLSAndStrAndStrPtr)(sidr->procs[procLSConcatLSAndStrAndStr]))
typedef pascal long (*IPStringToLongPtr)(StringPtr);
#define IPStringToLong ((IPStringToLongPtr)(sidr->procs[procIPStringToLong]))
typedef pascal void (*SetSmartScrollInfoPtr)(ControlRef, long, long);
#define SetSmartScrollInfo ((SetSmartScrollInfoPtr)(sidr->procs[procSetSmartScrollInfo]))
typedef pascal void (*SetSmartScrollPropPtr)(ControlRef, Fract);
#define SetSmartScrollProp ((SetSmartScrollInfoPtr)(sidr->procs[procSetSmartScrollProp]))
typedef pascal Fract (*GetSmartScrollPropPtr)(ControlRef);
#define GetSmartScrollProp ((GetSmartScrollPropPtr)(sidr->procs[procGetSmartScrollProp]))
typedef OSErr (*LCAttachPtr)(ControlRef);
#define LCAttach ((LCAttachPtr)(sidr->procs[procLCAttach]))
typedef void (*LCDetachPtr)(ControlRef);
#define LCDetach ((LCDetachPtr)(sidr->procs[procLCDetach]))
typedef void (*LCSetValuePtr)(ControlRef, long);
#define LCSetValue ((LCSetValuePtr)(sidr->procs[procLCSetValue]))
typedef void (*LCSetMinPtr)(ControlRef, long);
#define LCSetMin ((LCSetMinPtr)(sidr->procs[procLCSetMin]))
typedef void (*LCSetMaxPtr)(ControlRef, long);
#define LCSetMax ((LCSetMaxPtr)(sidr->procs[procLCSetMax]))
typedef long (*LCGetValuePtr)(ControlRef);
#define LCGetValue ((LCGetValuePtr)(sidr->procs[procLCGetValue]))
typedef long (*LCGetMinPtr)(ControlRef);
#define LCGetMin ((LCGetMinPtr)(sidr->procs[procLCGetMin]))
typedef long (*LCGetMaxPtr)(ControlRef);
#define LCGetMax ((LCGetMaxPtr)(sidr->procs[procLCGetMax]))
typedef void (*LCSynchPtr)(ControlRef);
#define LCSynch ((LCSynchPtr)(sidr->procs[procLCSynch]))
typedef pascal char (*IsChannelValidPtr)(channelPtr);
#define IsChannelValid ((IsChannelValidPtr)(sidr->procs[procIsChannelValid]))
typedef pascal char (*IsUserValidPtr)(UserListPtr);
#define IsUserValid ((IsUserValidPtr)(sidr->procs[procIsUserValid]))
typedef pascal void (*ConnPutPtr)(connectionPtr*, void*, long);
#define ConnPut ((ConnPutPtr)(sidr->procs[procConnPut]))
typedef pascal void (*ConnPutTextPtr)(connectionPtr*, void*, long);
#define ConnPutText ((ConnPutPtr)(sidr->procs[procConnPutText]))
typedef pascal void (*ConnPutLSPtr)(connectionPtr*, LongString *);
#define ConnPutLS ((ConnPutLSPtr)(sidr->procs[procConnPutLS]))
typedef pascal void (*StackModesPtr)(LongString*, channelPtr, StringPtr, StringPtr);
#define StackModes ((StackModesPtr)(sidr->procs[procStackModes]))
typedef pascal void (*OpenPreferencesWindowPtr)(short);
#define OpenPreferencesWindow ((OpenPreferencesWindowPtr)(sidr->procs[procOpenPreferencesWindow]))
typedef pascal void (*SMPrefixIrcleColorPtr)(LongString*, short, short);
#define SMPrefixIrcleColor ((SMPrefixIrcleColorPtr)(sidr->procs[procSMPrefixIrcleColor]))
typedef pascal OSErr (*AsyncSoundPlayPtr)(Handle, long, Ptr*);
#define AsyncSoundPlay ((AsyncSoundPlayPtr)(sidr->procs[procAsyncSoundPlay]))
typedef pascal char (*LSCmpPtr)(LongString*, LongString*);
#define LSCmp ((LSCmpPtr)(sidr->procs[procLSCmp]))
#define LSCaseCmp ((LSCmpPtr)(sidr->procs[procLSCaseCmp]))
typedef pascal void (*LSGetIndStringPtr)(LongString*, short, short);
#define LSGetIndString ((LSGetIndStringPtr)(sidr->procs[procLSGetIndString]))
typedef pascal OSErr (*LSParamStringPtr)(LongString*, StringPtr, StringPtr, StringPtr, StringPtr, StringPtr);
#define LSParamString ((LSParamStringPtr)(sidr->procs[procLSParamString]))
typedef pascal OSErr (*LSParamLSPtr)(LongString*, StringPtr, StringPtr, StringPtr, StringPtr);
#define LSParamLS ((LSParamLSPtr)(sidr->procs[procLSParamLS]))
typedef pascal char (*SMLinkPtr)(linkPtr, LongString*);
#define SMLink ((SMLinkPtr)(sidr->procs[procSMLink]))
typedef pascal char (*SMPrefixLinkPtr)(linkPtr, LongString*, short);
#define SMPrefixLink ((SMPrefixLinkPtr)(sidr->procs[procSMPrefixLink]))
typedef pascal char (*SMPrefixLinkColorPtr)(linkPtr, LongString*, short, short);
#define SMPrefixLinkColor ((SMPrefixLinkColorPtr)(sidr->procs[procSMPrefixLinkColor]))
typedef pascal char (*SMPrefixLinkIrcleColorPtr)(linkPtr, LongString*, short, short);
#define SMPrefixLinkIrcleColor ((SMPrefixLinkIrcleColorPtr)(sidr->procs[procSMPrefixLinkIrcleColor]))
typedef pascal char (*IPCRegisterPtr)(FourCharCode, long, long);
#define IPCRegister ((IPCRegisterPtr)(sidr->procs[procIPCRegister]))
typedef pascal char (*IPCExistsPtr)(FourCharCode, long*, long*);
#define IPCExists ((IPCExistsPtr)(sidr->procs[procIPCExists]))
typedef pascal short (*IPCSendPtr)(FourCharCode, long, long, long*);
#define IPCSend ((IPCSendPtr)(sidr->procs[procIPCCall]))
typedef pascal void (*DoNotifyPtr)(linkPtr link, LongString *parse);
#define DoNotify ((DoNotifyPtr)(sidr->procs[procDoNotify]))
typedef pascal void (*RunNotifyPtr)(void);
#define RunNotify ((RunNotifyPtr)(sidr->procs[procRunNotify]))
typedef pascal void (*ListNotifyPtr)(linkPtr link);
#define ListNotify ((ListNotifyPtr)(sidr->procs[procListNotify]))
typedef pascal notifyPtr (*findNotifyPtr)(linkPtr link, StringPtr nick);
#define findNotify ((findNotifyPtr)(sidr->procs[procfindNotify]))
typedef pascal void (*deleteNotifyPtr)(linkPtr link, StringPtr nick);
#define deleteNotify ((deleteNotifyPtr)(sidr->procs[procdeleteNotify]))
typedef pascal void (*addNotifyPtr)(linkPtr link, StringPtr nick, char);
#define addNotify ((addNotifyPtr)(sidr->procs[procaddNotify]))
typedef pascal void (*DrawMWinStatusPtr)(MWPtr);
#define DrawMWinStatus ((DrawMWinStatusPtr)(sidr->procs[procDrawMWinStatus]))
typedef pascal void (*ConnClosePtr)(connectionPtr);
#define ConnClose ((ConnClosePtr)(sidr->procs[procConnClose]))
#define ConnAbort ((ConnClosePtr)(sidr->procs[procConnAbort]))
typedef pascal char (*ConnNewActivePtr)(connectionPtr);
#define ConnNewActive ((ConnNewActivePtr)(sidr->procs[procConnNewActive]))
#define ConnNewPassive ((ConnNewActivePtr)(sidr->procs[procConnNewPassive]))
typedef pascal OSErr (*ConnGetDataPtr)(connectionPtr, Ptr, short);
#define ConnGetData ((ConnGetDataPtr)(sidr->procs[procConnGetData]))
typedef pascal long (*ConnCharsAvailPtr)(connectionPtr);
#define ConnCharsAvail ((ConnCharsAvailPtr)(sidr->procs[procConnCharsAvail]))
typedef pascal iwWidgetPtr (*IWNewObjectPtr)(long, short, short);
#define IWNewWidget ((IWNewObjectPtr)(sidr->procs[procIWNewObject]))
typedef pascal void (*IWRecalculateRectsPtr)(void);
#define IWRecalculateRects ((IWRecalculateRectsPtr)(sidr->procs[procIWRecalculateRects]))
typedef pascal long (*IWPopUpMenuPtr)(Point, MenuHandle, long);
#define IWPopUpMenu ((IWPopUpMenuPtr)(sidr->procs[procIWPopUpMenu]))
typedef pascal char (*IWDeleteObjectPtr)(iwWidgetPtr);
#define IWDeleteWidget ((IWDeleteObjectPtr)(sidr->procs[procIWDeleteObject]))
typedef pascal short (*IWOverridePtr)(long, iwWidgetPtr*);
#define IWOverride ((IWOverridePtr)(sidr->procs[procIWOverride]))
typedef pascal mwWidgetPtr (*MWNewWidgetPtr)(MWPtr, long, short, short);
#define MWNewWidget ((MWNewWidgetPtr)(sidr->procs[procMWNewWidget]))
typedef pascal void (*MWRecalculateRectsPtr)(MWPtr mw);
#define MWRecalculateRects ((MWRecalculateRectsPtr)(sidr->procs[procMWRecalculateRects]))
typedef pascal void (*MWDestroyWidgetPtr)(mwWidgetPtr);
#define MWDestroyWidget ((MWDestroyWidgetPtr)(sidr->procs[procMWDestroyWidget]))
typedef pascal short (*HMIAddPtr)(StringPtr name);
#define HMIAdd ((HMIAddPtr)(sidr->procs[procHMIAdd]))
typedef pascal mwWidgetPtr (*MWFindWidgetPtr)(MWPtr, long);
#define MWFindWidget ((MWFindWidgetPtr)(sidr->procs[procMWFindWidget]))
typedef pascal mwPanePtr (*MWFindPanePtr)(MWPtr mw, long type);
#define MWFindPane ((MWFindPanePtr)(sidr->procs[procMWFindPane]))
typedef pascal mwPanePtr (*MWNewPanePtr)(MWPtr mw, long type, short align, short height, short width);
#define MWNewPane ((MWNewPanePtr)(sidr->procs[procMWNewPane]))
typedef pascal void (*MWPaneRecalculatePtr)(MWPtr mw);
#define MWPaneRecalculate ((MWPaneRecalculatePtr)(sidr->procs[procMWPaneRecalculate]))
typedef pascal void (*MWPaneResizePtr)(MWPtr mw);
#define MWPaneResize ((MWPaneResizePtr)(sidr->procs[procMWPaneResize]))
typedef pascal char (*StartDrawingOffscreenPtr)(Ptr *data, RectPtr bounds, Boolean copyDest);
#define StartDrawingOffscreen ((StartDrawingOffscreenPtr)(sidr->procs[procStartDrawingOffscreen]))
typedef pascal void (*EndDrawingOffscreenPtr)(Ptr data);
#define EndDrawingOffscreen ((EndDrawingOffscreenPtr)(sidr->procs[procEndDrawingOffscreen]))
typedef pascal void (*AbortDrawingOffscreenPtr)(Ptr data);
#define AbortDrawingOffscreen ((AbortDrawingOffscreenPtr)(sidr->procs[procAbortDrawingOffscreen]))
typedef pascal char (*MWDestroyPanePtr)(mwPanePtr o);
#define MWDestroyPane ((MWDestroyPanePtr)(sidr->procs[procMWDestroyPane]))
typedef pascal char (*LSInsertCharPtr)(char, short, LongString*);
#define LSInsertChar ((LSInsertCharPtr)(sidr->procs[procLSInsertChar]))
typedef pascal char (*LSInsertShortPtr)(short, short, LongString*);
#define LSInsertShort ((LSInsertShortPtr)(sidr->procs[procLSInsertShort]))
typedef pascal char (*LSInsertLongPtr)(long, short, LongString*);
#define LSInsertLong ((LSInsertLongPtr)(sidr->procs[procLSInsertLong]))
typedef pascal void (*LightenColorPtr)(RGBColor*);
#define LightenColor ((LightenColorPtr)(sidr->procs[procLightenColor]))
typedef pascal void (*NavDialogFilterPtr)(const long, Ptr, long);
#define NavDialogFilter ((NavDialogFilterPtr)(sidr->procs[procNavDialogFilter]))
typedef pascal char (*DirectorySelectButtonPtr)(FSSpec *fss);
#define DirectorySelectButton ((DirectorySelectButtonPtr)(sidr->procs[procDirectorySelectButton]))
typedef pascal void (*CleanFolderFSpPtr)(FSSpec *fss);
#define CleanFolderFSp ((CleanFolderFSpPtr)(sidr->procs[procCleanFolderFSp]))
typedef pascal char (*DoDCCSendPtr)(linkPtr, Str255, char);
#define DoDCCSend ((DoDCCSendPtr)(sidr->procs[procDoDCCSend]))
typedef pascal char (*DoDCCSendFilePtr)(linkPtr, Str255, FSSpec*, char, char);
#define DoDCCSendFile ((DoDCCSendFilePtr)(sidr->procs[procDoDCCSendFile]))
typedef pascal char (*DCCGetAFilePtr)(FSSpec*, char*);
#define DCCGetAFile ((DCCGetAFilePtr)(sidr->procs[procDCCGetAFile]))
typedef pascal void (*InputHandlerPtr)(LongString*, targetPtr);
#define InputHandler ((InputHandlerPtr)(sidr->procs[procInputHandler]))
typedef pascal OSErr (*OpenURLPtr)(Str255);
#define OpenURL ((OpenURLPtr)(sidr->procs[procOpenURL]))
typedef pascal short (*DCCTypeAddPtr)(Str255);
#define DCCTypeAdd ((DCCTypeAddPtr)(sidr->procs[procDCCTypeAdd]))
typedef pascal void (*DCCGetStatusPtr)(dccPtr, dccStatus*);
#define DCCGetStatus ((DCCGetStatusPtr)(sidr->procs[procDCCGetStatus]))
typedef pascal void (*ProcessLinePtr)(LongString*, char, char, MWPtr);
#define ProcessLine ((ProcessLinePtr)(sidr->procs[procProcessLine]))
typedef pascal char (*ProcessInputHunkPtr)(Ptr, long, MWPtr, char, long*);
#define ProcessInputHunk ((ProcessInputHunkPtr)(sidr->procs[procProcessInputHunk]))
typedef pascal void (*DragHilightRectPtr)(Rect *re, DragReference drag);
#define DragHilightRect ((DragHilightRectPtr)(sidr->procs[procDragHilightRect]))
typedef pascal void (*DragHilightRectBGPtr)(Rect *re, DragReference drag, char hilite);
#define DragHilightRectBG ((DragHilightRectBGPtr)(sidr->procs[procDragHilightRectBG]))
typedef pascal char (*DragIsTypeAvailPtr)(DragReference drag, long type);
#define DragIsTypeAvail ((DragIsTypeAvailPtr)(sidr->procs[procDragIsTypeAvail]))
typedef pascal char (*DragGetTypePtr)(DragReference drag, long type, void* *data, long *size);
#define DragGetType ((DragGetTypePtr)(sidr->procs[procDragGetType]))
typedef pascal char (*DragGetPtrPtr)(DragReference drag, long type, void* *data);
#define DragGetPtr ((DragGetPtrPtr)(sidr->procs[procDragGetPtr]))
typedef pascal char (*DragAddPtrPtr)(DragReference drag, ItemReference item, long type, void* data, long flags);
#define DragAddPtr ((DragAddPtrPtr)(sidr->procs[procDragAddPtr]))
typedef pascal void (*DCCSendCookiePtr)(connectionPtr*, long, LongString*);
#define DCCSendCookie ((DCCSendCookiePtr)(sidr->procs[procDCCSendCookie]))
typedef pascal void (*DCCSendCookieReplyPtr)(connectionPtr*, long, LongString*);
#define DCCSendCookieReply ((DCCSendCookieReplyPtr)(sidr->procs[procDCCSendCookieReply]))
typedef pascal void (*CMAddPtr)(pCMPopupsDataPtr m, Str255 item, long id);
#define CMAdd ((CMAddPtr)(sidr->procs[procCMAdd]))
typedef pascal char (*IPCReplyPtr)(void*, FourCharCode, long, long, long*);
#define IPCReply ((IPCReplyPtr)(sidr->procs[procIPCReply]))
typedef pascal short (*LSPosCaseCustomPtr)(StringPtr, LongString*, short);
#define LSPosCaseCustom ((LSPosCaseCustomPtr)(sidr->procs[procLSPosCaseCustom]))
typedef pascal void (*SetBackgroundPtr)(short);
#define SetBackground ((SetBackgroundPtr)(sidr->procs[procSetBackground])) 
typedef pascal long (*MySendControlMessagePtr)(ControlHandle, short, long);
#define MySendControlMessage ((MySendControlMessagePtr)(sidr->procs[procMySendControlMessage]))
typedef pascal void (*SMPrefixRGBColorPtr)(LongString*, short, RGBColor*);
#define SMPrefixRGBColor ((SMPrefixRGBColorPtr)(sidr->procs[procSMPrefixRGBColor]))
typedef pascal char (*SMPrefixRGBLinkColorPtr)(linkPtr, LongString*, short, RGBColor*);
#define SMPrefixRGBLinkColor ((SMPrefixRGBLinkColorPtr)(sidr->procs[procSMPrefixLinkRGBColor]))
typedef pascal void (*pluginSetWRefConPtr)(WindowPtr, long);
#define pluginSetWRefCon ((pluginSetWRefConPtr)(sidr->procs[procpluginSetWRefCon]))
typedef pascal long (*pluginGetWRefConPtr)(WindowPtr);
#define pluginGetWRefCon ((pluginGetWRefConPtr)(sidr->procs[procpluginGetWRefCon]))
typedef pascal void (*MWPaneUpdatePtr)(MWPtr);
#define MWPaneUpdate ((MWPaneUpdatePtr)(sidr->procs[procMWPaneUpdate]))
typedef pascal long (*ShadowIRCVersion2Ptr)(StringPtr, StringPtr);
#define ShadowIRCVersion2 ((ShadowIRCVersion2Ptr)(sidr->procs[procShadowIRCVersion2]))
typedef pascal void (*SetDlogItemTextHdlPtr)(DialogPtr, short, char*, long);
#define SetDlogItemTextHdl ((SetDlogItemTextHdlPtr)(sidr->proc[procSetDlogItemTextHdl]))
typedef pascal void (*DrawBorderPtr)(Rect*, long, char);
#define DrawBorder ((DrawBorderPtr)(sidr->procs[procDrawBorder]))
typedef pascal void (*FileAddPtr)(short, char);
#define FileAdd ((FileAddPtr)(sidr->procs[procFileAdd]))
typedef pascal OSErr (*FileClosePtr)(short);
#define FileClose ((FileClosePtr)(sidr->procs[procFileClose]))
typedef pascal char (*IsIgnoredNickUserPtr)(ConstStr255Param, ConstStr255Param);
#define IsIgnoredNickUser ((IsIgnoredNickUserPtr)(sidr->procs[procIsIgnoredNickUser]))
typedef pascal short (*WMSGetMenuItemNumPtr)(FourCharCode);
#define WMSGetMenuItemNum ((WMSGetMenuItemNumPtr)(sidr->procs[procWMSGetMenuItemNum]))
typedef pascal void (*GetInputLineCursorSelectionPtr)(long*, long*);
#define GetInputLineCursorSelection ((GetInputLineCursorSelectionPtr)(sidr->procs[procGetInputLineCursorSelection]))
typedef pascal void (*LockInputPtr)(void);
#define LockInput ((LockInputPtr)(sidr->procs[procLockInput]))
typedef pascal void (*UnlockInputPtr)(void);
#define UnlockInput ((UnlockInputPtr)(sidr->procs[procUnlockInput]))
typedef pascal void (*DoServerPtr)(linkPtr link, LongString *rest, const LongString* reason);
#define DoServer ((DoServerPtr)(sidr->procs[procDoServer]))
typedef pascal void (*DoSignoffPtr)(linkPtr link, const LongString *reason);
#define DoSignoff ((DoSignoffPtr)(sidr->procs[procDoSignoff]))
typedef pascal short (*WMSListAddPtr)(FourCharCode serviceType, ConstStr255Param name);
#define WMSListAdd ((WMSListAddPtr)(sidr->procs[procWMSListAdd]))
typedef pascal char (*ILGetLinePtr)(MWPtr mw, LongString *ls);
#define ILGetLine ((ILGetLinePtr)(sidr->procs[procILGetLine]))
typedef pascal char (*ILSetLinePtr)(MWPtr mw, LongString *ls);
#define ILSetLine ((ILSetLinePtr)(sidr->procs[procILSetLine]))
typedef pascal char (*MWGetNamePtr)(MWPtr mw, Str255 s);
#define MWGetName ((MWGetNamePtr)(sidr->procs[procMWGetName]))
typedef pascal channelPtr (*MWGetChannelPtr)(MWPtr mw);
#define MWGetChannel ((MWGetChannelPtr)(sidr->procs[procMWGetChannel]))
typedef pascal connectionPtr (*MWGetDCCPtr)(MWPtr mw);
#define MWGetDCC ((MWGetDCCPtr)(sidr->procs[procMWGetDCC]))
typedef pascal void (*DoModeKWindowPtr)(channelPtr, LongString*);
#define DoModeKWindow ((DoModeKWindowPtr)(sidr->procs[procDoModeKWindow]))
typedef pascal unsigned short (*ConnGetLocalPortPtr)(connectionPtr);
#define ConnGetLocalPort ((ConnGetLocalPortPtr)(sidr->procs[procConnGetLocalPort]))
typedef pascal long (*RegisterAETEPtr)(Handle aete);
#define RegisterAETE ((RegisterAETEPtr)(sidr->procs[procRegisterAETE]))
typedef pascal void (*NickListAddPtr)(linkPtr link, ConstStr255Param nick);
#define NickListAdd ((NickListAddPtr)(sidr->procs[procNickListAdd]))
typedef pascal void (*ChannelListAddPtr)(linkPtr link, ConstStr255Param channel);
#define ChannelListAdd ((ChannelListAdd)(sidr->procs[procChannelListAdd]))
typedef pascal void (*GetDrawingStatePtr)(DrawingState*);
#define GetDrawingState ((GetDrawingStatePtr)(sidr->procs[procGetDrawingState]))
typedef pascal void (*SetDrawingStatePtr)(DrawingState);
#define SetDrawingState ((SetDrawingStatePtr)(sidr->procs[procSetDrawingState]))
typedef pascal void (*NormalizeDrawingStatePtr)();
#define NormalizeDrawingState ((NormalizeDrawingStatePtr)(sidr->procs[procNormalizeDrawingState]))
typedef pascal ListHandle (*GetAppearanceListBoxHandlePtr)(DialogPtr theDialog, short itemNum);
#define GetAppearanceListBoxHandle ((GetAppearanceListBoxHandlePtr)(sidr->procs[procGetAppearanceListBoxHandle]))
typedef pascal OSErr (*CopyResourcePtr)(ResType type, short id, short src, short des);
#define CopyResource ((CopyResourcePtr)(sidr->procs[procCopyResource]))
typedef pascal void (*pstrcatPtr)(ConstStr255Param src1, ConstStr255Param src2, StringPtr des);
#define pstrcat ((pstrcatPtr)(sidr->procs[procPstrcat]))

typedef pascal void (*MWPartPtr)(MWPtr mw);
#define MWPart ((MWPartPtr)(sidr->procs[procMWPart]))
typedef pascal void (*WindowClosePtr)(WindowPtr wp);
#define WindowClose ((WindowClosePtr)(sidr->procs[procWindowClose]))
#endif
#endif
