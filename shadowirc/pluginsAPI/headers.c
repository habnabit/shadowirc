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

#include "ShadowIRC.h"
#pragma lib_export on
#pragma export on
pascal long _UndocumentedAPI(long, long){}
pascal void LSDelete(LongString *ls, short firstpos, short lastpos){}
pascal void LSCopy(const LongString *ls, short firstpos, short len, LongString *out){}
pascal void LSCopyString(const LongString *ls, short firstpos, short len, StringPtr str){}
pascal short LSPosCustom(ConstStr255Param s, const LongString *ls, short start){}
pascal short LSPos(ConstStr255Param s, const LongString *ls){}
pascal short LSPosCase(ConstStr255Param s, const LongString *ls){}
pascal short LSPosChar(short c, const LongString *ls){}
pascal void LSNextArg(LongString *ls, StringPtr arg){}
pascal void LSNextArgND(LongString *ls, StringPtr arg){}
pascal void LSStrLS(ConstStr255Param s, LongString *ls){}
pascal void LSConcatLSAndStr(const LongString *ls, StringPtr s, LongString *out){}
pascal void LSConcatStrAndLS(ConstStr255Param s, const LongString *ls, LongString *out){}
pascal void LSConcatStrAndStr(ConstStr255Param s1, ConstStr255Param s2, LongString *out){}
pascal void LSConcatLSAndLS(const LongString *ls, const LongString *ls2, LongString *out){}
pascal void LSConcatLSAndStrAndStr(const LongString *ls1, ConstStr255Param s, ConstStr255Param s2, LongString *out){}
pascal void LSConcatLSAndStrAndLS(const LongString *ls1, ConstStr255Param s, const LongString *ls2, LongString *out){}
pascal void LSConcatStrAndLSAndStr(ConstStr255Param s1, const LongString *ls, ConstStr255Param s2, LongString *out){}
pascal void LSConcatStrAndStrAndStr(ConstStr255Param s1, ConstStr255Param s2, ConstStr255Param s3, LongString *out){}
void LSStrCat(short n,LongString *out,...){}
pascal void LSInsertStr(ConstStr255Param s, short insat, LongString *ls){}
pascal char NewService(FourCharCode serviceType){}
pascal void SMPrefix(LongString *ls, short where){}
pascal void SMPrefixColor(LongString *ls, short where, short color){}
pascal void LineMsg(LongString *ls){}
pascal void UpdateStatusLine(void){}
pascal void SetInputLineCursorSelection(long start, long finish){}
pascal void GetInputLine(LongString *line){}
pascal void GetLine(char, MWPtr){}
pascal void SetInputLine(LongString *ls){}
pascal void ChMsg(channelPtr ch, LongString *msg){}
pascal void ChannelMsg(linkPtr, ConstStr255Param ch, LongString *msg){}
pascal void Message(LongString *msg){}
pascal DialogPtr pluginNewDialog(short dialogID){}
pascal void pluginDisposeDialog(DialogPtr d){}
pascal WindowPtr pluginNewWindow(const Rect *boundsRect, ConstStr255Param title, short theProc, long flags){}
pascal void pluginDisposeWindow(WindowPtr w){}
pascal WindowPtr FrontNonFloater(void){}
pascal char WIsFloater(WindowPtr w){}
pascal void WMoveToFront(WindowPtr w){}
pascal void WSelect(WindowPtr w){}
pascal void WDrag(WindowPtr w, Point startPoint, Rect *boundsRect){}
pascal void WShow(WindowPtr w){}
pascal void WHide(WindowPtr w){}
pascal MWPtr NewPluginMWindow(ConstStr255Param title){}
pascal void MWSetDimen(MWPtr window, short left, short top, short width, short height){}
pascal void MWSetFontSize(MWPtr window, short font, short size){}
pascal void MWDelete(MWPtr window){}
pascal void MWMessage(MWPtr window, LongString *msg){}
pascal MWPtr MWFromWindow(WindowPtr){}
pascal void MWColor(LongString *text, short colorNum){}
pascal void MWIrcleColor(LongString *text, short colorNum){}
pascal char IsChannel(ConstStr255Param s){}
pascal char IsDCCName(ConstStr255Param s){}
pascal void MakeChannel(Str255){}
pascal channelPtr ChFind(ConstStr255Param name, linkPtr){}
pascal bansP ChFindBan(channelPtr ch, ConstStr255Param n, char){}
pascal bansP ChMatchBan(channelPtr ch, ConstStr255Param n, char){}
pascal char ChGetBan(channelPtr ch, ConstStr255Param from, bansP *f, char){}
pascal void ChKillBan(channelPtr ch, bansP *theBan, char){}
pascal void ChKillAllBans(channelPtr ch, char){}
pascal UserListPtr ULAddUser(channelPtr channel, ConstStr255Param nick, ConstStr255Param userhost){}
pascal UserListPtr ULFindUserName(channelPtr channel, ConstStr255Param nick){}
pascal MWPtr ChannelWindow(linkPtr, ConstStr255Param name){}
pascal void deleteConnection(connectionPtr *c){}
pascal connectionPtr pluginNewConnection(char){}
pascal void ConnPut(connectionPtr*, const void*, long){}
pascal void ConnPutText(connectionPtr*, void*, long){}
pascal void ConnPutLS(connectionPtr*, LongString*){}
pascal void putServer(linkPtr, LongString *ls){}
pascal void SendCTCPReply(linkPtr, ConstStr255Param target, ConstStr255Param command, LongString *args){}
pascal short PMLAdd(ConstStr255Param name){}
pascal OSErr PFOpen(ConstStr255Param, char, short*, FSSpec*){}
pascal char PFExists(ConstStr255Param name){}
pascal OSErr PFCreate(ConstStr255Param name, FourCharCode type, FourCharCode creator, char resFork){}
pascal OSErr PFWrite(short, const void*, long*){}
pascal OSErr PFRead(short, void*, long*){}
pascal OSErr PFSize(short refNum, long *size){}
pascal OSErr PFResize(short refNum,  long newSize){}
pascal OSErr PFSetPos(short refNum, long pos){}
pascal OSErr PFClose(short refNum){}
pascal OSErr PFDelete(ConstStr255Param name){}
pascal long ShadowIRCVersion(StringPtr versionString){}
pascal void NextArg(StringPtr str, StringPtr arg){}
pascal void HandleCommand(linkPtr, LongString *cmd){}
pascal void writeMainPrefs(void){}
pascal void writeAllFiles(void){}
pascal long ulongval(ConstStr255Param s){}
pascal void ulongstr(long l, StringPtr s){}
pascal ConstStringPtr dccTypToStr(short d){}
pascal ConstStr255Param dccFlagTypToStr(short d){}
pascal short DCCTypeFind(StringPtr s){}
pascal void hostToIPStr(long l, StringPtr s){}
pascal connectionPtr DCCFind(linkPtr link, short typ, ConstStr255Param from){}
pascal void DCCClose(connectionPtr*, char){}
pascal void DCCWindowClose(Ptr mw){}
pascal char DCCCreate(linkPtr, short typ, ConstStr255Param from, connectionPtr *c){}
pascal void DCCRequest(linkPtr, ConstStr255Param from, ConstStr255Param uah, StringPtr s){}
pascal void DCCOpen(connectionPtr*){}
pascal void DCCCommand(linkPtr, StringPtr){}
pascal void DCCSSay(connectionPtr *c, LongString *ls){}
pascal void DNSLookup(StringPtr addr, long saveReply){}
pascal void doBroadcast(linkPtr, const LongString *text, char action){}
pascal void DoONotice(channelPtr, const LongString *text){}
pascal void ListIgnores(void){}
pascal char IsIgnored(ConstStr255Param uah){}
pascal void DoIgnore(LongString  *in){}
pascal ignorePtr findIgnore(ConstStr255Param mask, ignorePtr start){}
pascal void ProcessShortcutText(LongString *ls, short *cursorPos, short*){}
pascal void MBIdle(void){}
pascal messageBufferPtr MBFindNick(linkPtr, ConstStr255Param){}
pascal void MBNewMessage(linkPtr, ConstStr255Param){}
pascal char upc(unsigned char c){}
pascal void ucase(StringPtr str){}
pascal void pdelete(StringPtr s, short start, short len){}
pascal void pinsert(ConstStr255Param ins, StringPtr s, short insat){}
pascal void pstrcpy(ConstStr255Param f, StringPtr dest){}
pascal char pstrcmp(ConstStr255Param s1, ConstStr255Param s2){}
pascal char pstrcasecmp(ConstStr255Param s1, ConstStr255Param s2){}
pascal char pstrcasecmp2(ConstStr255Param s1, ConstStr255Param s2){}
pascal void padEnd(ConstStr255Param s, short num, StringPtr out){}
pascal void padBegin(ConstStr255Param s, short num, StringPtr out){}
pascal char maskMatch(ConstStr255Param s, ConstStr255Param mask){}
pascal void makeMask(StringPtr uah, StringPtr mask){}
pascal short countChar(short c, ConstStr255Param s){}
pascal short pos(short c, ConstStr255Param s){}
pascal short revPos(short c, ConstStr255Param s){}
pascal char isNumber(ConstStr255Param s){}
pascal char isIPNumber(ConstStr255Param s){}
pascal void GetText(DialogPtr dlg, short item, StringPtr into){}
pascal void SetText(DialogPtr dlg, short item, ConstStr255Param text){}
pascal void setCheckBox(DialogPtr dlg, short item, char up){}
pascal char getCheckBox(DialogPtr dlg, short item){}
pascal void setButtonEnable(DialogPtr dlg, short item, char on){}
pascal ControlHandle GetControlHandle(DialogPtr d, short item){}
pascal char getFirstSelectedCell(ListHandle l, Cell *c){}
pascal void selectOneCell(ListHandle l, Cell c){}
pascal void drawListBorder(ListHandle theList){}
pascal void SetDlogFont(DialogPtr d){}
pascal MenuHandle GetControlMenu(ControlHandle c){}
pascal void WMove(WindowPtr w, short h, short v, char front){}
pascal void EnterModalDialog(void){}
pascal void ExitModalDialog(void){}
OSErr LCAttach (ControlRef inControl){}
void LCDetach (ControlRef inControl){}
void LCSetValue (ControlRef inControl, SInt32 inValue){}
void LCSetMin (ControlRef inControl, SInt32 inValue){}
void LCSetMax (ControlRef inControl, SInt32 inValue){}
SInt32 LCGetValue (ControlRef inControl){}
SInt32 LCGetMin (ControlRef inControl){}
SInt32 LCGetMax (ControlRef inControl){}
void LCSynch (ControlRef inControl){}
pascal char IsChannelValid(channelPtr ch){}
pascal char IsUserValid(UserListPtr u){}
pascal void StackModes(LongString *theMode, channelPtr channel, ConstStr255Param mode, ConstStr255Param arg){}
pascal void OpenPreferencesWindow(short){}
pascal void SMPrefixIrcleColor(LongString *ls, short where, short color){}
pascal OSErr AsyncSoundPlay(Handle sound, long refcon, Ptr *channel){}
pascal char LSCmp(const LongString *ls1, const LongString *ls2){}
pascal char LSCaseCmp(const LongString *ls1, const LongString *ls2){}
pascal void LSGetIndString(LongString *ls, short id, short num){}
pascal OSErr LSParamString(LongString *ls, ConstStr255Param str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3){}
pascal OSErr LSParamLS(LongString *ls, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3){}
pascal OSErr ParamString(StringPtr str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3){}
pascal char SMLink(linkPtr link, LongString *ls){}
pascal char SMPrefixLink(linkPtr link, LongString *ls, short where){}
pascal char SMPrefixLinkColor(linkPtr link, LongString *ls, short where, short color){}
pascal char SMPrefixLinkIrcleColor(linkPtr link, LongString *ls, short where, short color){}
pascal short IPCRegister(FourCharCode IPCType, long version, long data){}
pascal char IPCExists(FourCharCode IPCType, long *version, long *data){}
pascal short IPCSend(FourCharCode IPCType, long message, long data, long *result){}
pascal void SecsToHMS(long secs, LongString *ls){}
pascal char IPCReply(void*, FourCharCode, long, long data, long*){}
pascal void DoNotify(linkPtr link, LongString *parse){}
pascal void RunNotify(void){}
pascal void ListNotify(linkPtr link){}
pascal notifyPtr findNotify(linkPtr link, ConstStr255Param nick){}
pascal void deleteNotify(linkPtr link, ConstStr255Param nick){}
pascal notifyPtr addNotify(linkPtr, ConstStr255Param, char){}
pascal void SendCommand(linkPtr link, LongString *ls){}
pascal void DrawMWinStatus(MWPtr mw){}
pascal void ConnAbort(connectionPtr conn){}
pascal void ConnClose(connectionPtr conn){}
pascal char ConnNewActive(connectionPtr conn){}
pascal char ConnNewPassive(connectionPtr c){}
pascal OSErr ConnGetData(connectionPtr c, Ptr d, short len){}
pascal long ConnCharsAvail(connectionPtr conn){}
pascal iwWidgetPtr IWNewWidget(long type, short align, short width){}
pascal void IWRecalculateRects(void){}
pascal long IWPopUpMenu(Point p, MenuHandle m, long curItem){}
pascal char IWDeleteWidget(iwWidgetPtr o){}
pascal short IWOverride(long type, iwWidgetPtr *object){}
pascal mwWidgetPtr MWNewWidget(MWPtr mw, long type, short align, short width){}
pascal void MWRecalculateRects(MWPtr mw){}
pascal void MWDestroyWidget(mwWidgetPtr w){}
pascal short HMIAdd(ConstStr255Param){}
pascal mwWidgetPtr MWFindWidget(MWPtr, long){}
pascal mwPanePtr MWFindPane(MWPtr mw, long type){}
pascal mwPanePtr MWNewPane(MWPtr mw, long type, short align, short height, short width){}
pascal void MWPaneRecalculate(MWPtr mw){}
pascal void MWPaneResize(MWPtr mw){}
pascal char StartDrawingOffscreen(Ptr *data, RectPtr bounds, Boolean copyDest){}
pascal void EndDrawingOffscreen(Ptr data){}
pascal void AbortDrawingOffscreen(Ptr data){}
pascal char MWDestroyPane(mwPanePtr o){}
pascal char LSInsertChar(char c, short p, LongString *ls){}
pascal char LSInsertShort(short s, short p, LongString *ls){}
pascal char LSInsertLong(long l, short p, LongString *ls){}
pascal void LightenColor(RGBColor *rgb){}
pascal char DirectorySelectButton(FSSpec *fss){}
pascal void CleanFolderFSp(FSSpec *fss){}
pascal char DoDCCSend(linkPtr, ConstStr255Param, char){}
pascal char DoDCCSendFile(linkPtr, ConstStr255Param, const FSSpec*, char, char){}
pascal char DCCGetAFile(FSSpec *f, char *macbinary){}
pascal void InputHandler(LongString*, targetPtr){}
pascal OSErr OpenURL(Str255){}
pascal short DCCTypeAdd(ConstStringPtr type){}
pascal void DCCGetStatus(dccPtr d, dccStatus *status){}
pascal void ProcessLine(LongString* line, char stackup, char action, MWPtr mw){}
pascal char ProcessInputHunk(Ptr data, long length, MWPtr mw, char, long*){}
pascal void DragHilightRect(const Rect *re, DragReference drag){}
pascal void DragHilightRectBG(const Rect *re, DragReference drag, char hilite){}
pascal char DragIsTypeAvail(DragReference drag, long type){}
pascal char DragGetType(DragReference drag, long type, void* *data, long *size){}
pascal char DragGetPtr(DragReference drag, long type, void* *data){}
pascal char DragAddPtr(DragReference drag, ItemReference item, long type, void* data, long flags){}
pascal void CMAdd(pCMPopupsDataPtr m, ConstStr255Param item, long id){}
pascal short LSPosCaseCustom(ConstStr255Param s, const LongString *ls, short start){}
pascal void CMAddSubmenu(pCMPopupsDataPtr, ConstStr255Param, short*, MenuHandle*){}
pascal char ExistsService(FourCharCode){}
pascal void makeCellVisible(ListHandle l, Cell c){}
pascal char StandardDialogFilter(DialogPtr, EventRecord*, short*){}
pascal void NavDialogFilter(const long callBackSelector, Ptr callBackParms, long callBackUD){}
pascal char OpenConnection(linkPtr){};
pascal void DrawPlacard(Rect *r, long state){}
pascal void SetTextColor(short){}
pascal void SetBackground(short){}
pascal long MySendControlMessage(ControlHandle, short, long){}
pascal void SMPrefixRGBColor(LongString *ls, short where, RGBColor *rgb){}
pascal char SMPrefixRGBLinkColor(linkPtr link, LongString *ls, short where, RGBColor *rgb){}
pascal void pluginSetWRefCon(WindowPtr w, long l){}
pascal long pluginGetWRefCon(WindowPtr w){}
pascal void MWPaneUpdate(MWPtr){}
pascal long ShadowIRCVersion2(StringPtr, StringPtr){}
pascal void SetDlogItemTextHdl(DialogPtr dp, short item, const char *buf, long bufLen){}
pascal void GetQD(QDGlobals**){}
pascal void DrawBorder(const Rect*, long, char){}
pascal void FileAdd(short fref, char res){}
pascal OSErr FileClose(short fref){}
pascal char IsIgnoredNickUser(ConstStr255Param nick, ConstStr255Param user){}
pascal short WMSGetMenuItemNum(long){}
pascal void GetInputLineCursorSelection(long *start, long *finish){}
pascal long IPStringToLong(ConstStr255Param){}
pascal void CMSetCheckmark(pCMPopupsDataPtr, long, char){}
pascal void LockInput(void){}
pascal void DoServer(linkPtr link, LongString *rest, const LongString* reason){}
pascal void DoSignoff(linkPtr link, const LongString *reason){}
pascal void UnlockInput(void){}
pascal short WMSListAdd(long, ConstStr255Param name){}
pascal char ILGetLine(MWPtr mw, LongString *ls){}
pascal char ILSetLine(MWPtr mw, LongString *ls){}
pascal channelPtr MWGetChannel(MWPtr mw){}
pascal ConstStringPtr MWGetName(MWPtr mw, Str255 s){}
pascal connectionPtr MWGetDCC(MWPtr mw){}
pascal void* GetPluginWindowOwner(WindowPtr){}
pascal void DoModeLWindow(channelPtr ch, LongString *ls){}
pascal void DoModeKWindow(channelPtr ch, LongString *ls){}
pascal unsigned short ConnGetLocalPort(connectionPtr conn){}
pascal long RegisterAETE(Handle aete){}
pascal void NickListAdd(linkPtr link, ConstStr255Param nick){}
pascal void ChannelListAdd(linkPtr link, ConstStr255Param channel){}
pascal void GetDrawingState(DrawingState* state){}
pascal void SetDrawingState(DrawingState state){}
pascal void NormalizeDrawingState(){}
pascal ListHandle GetAppearanceListBoxHandle(DialogPtr theDialog, short itemNum){}
pascal OSErr CopyResource(ResType type, short id, short src, short des){}
pascal void pstrcat(ConstStr255Param src1, ConstStr255Param src2, StringPtr des){}
pascal void MWPart(MWPtr mw){}
pascal void WindowClose(WindowPtr w){}

#pragma export off
#pragma lib_export off
void sirclib(void);
void sirclib(void){}