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

#ifndef _TextManip_
#define _TextManip_

enum {
	dsNowhere, dsConsole, dsFrontWin
};

//Colors for MWColor
enum siColors {
	numSIColors=15,
	
	sicServer=0,		//Server message color
	sicStandard=1,		//Standard text color
	sicPrivmsg=2,		//Private message color
	sicCTCP=3,			//CTCP and DCC color
	sicOutgoing=4,		//Outgoing text color (stuff the user typed)
	sicNotice=5,			//Notice color
	sicWindowBG=6,
	sicUserChannelOp=7,
	sicUserVoice=8,
	sicUserOper=9,
	sicUserRegular=10,
	sicStatusLine=11,
	sicUserIgnored=12,
	sicNotifyColor=13,
	sicTimestampColor=14,
	
	sicNoCR=246,
	sicDontLog=247,
	sicCustomColorBG=248,
	sicCustomColor=249,
	sicSaveStyle = 250,					//Saves current text style.
	sicRestoreStyle = 251,				//Restores saved style
	sicSetColorMethod = 252,			//Next byte is number for colorMethod to use
	sicSaveColorMethod = 253,		//Saves the current colorMethod
	sicRestoreColorMethod = 254,	//Restores the saved colorMethod
	sicDontProcess=255					//any styles after this color token aren't processed.
};

enum nickFormats
{
	//Source. One must be selected, or normal is assumed.
	kNickNormal = (1 << 0),
	kNickNotice = (1 << 1),
	kNickPrivmsg = (1 << 2),
	kNickOther = (1 << 3),
	
	//Display modifications
	kNickAction = (1 << 4),
	kNickSend = (1 << 5)
};


pascal void FormatNick(ConstStr255Param nick, LongString *ls, ConstStringPtr nickC, int type);
pascal void FormatMessage(ConstStr255Param nick, const LongString *text, ConstStringPtr nickC, ConstStringPtr textC, int type, LongString *out);

pascal void MWColor(LongString *ls, short colorNum);
pascal void MWIrcleColor(LongString *ls, short colorNum);

pascal void SMPrefix(LongString *ls, short where);
pascal void SMPrefixColor(LongString *ls, short where, short color);
pascal void SMPrefixIrcleColor(LongString *ls, short where, short color);
pascal void SMPrefixRGBColor(LongString *ls, short where, const RGBColor *rgb);

#ifdef _connections
pascal char SMLink(linkPtr link, LongString *ls);
pascal char SMPrefixLink(linkPtr link, LongString *ls, short where);
pascal char SMPrefixLinkColor(linkPtr link, LongString *ls, short where, short color);
pascal char SMPrefixLinkIrcleColor(linkPtr link, LongString *ls, short where, short color);
pascal char SMPrefixRGBLinkColor(linkPtr link, LongString *ls, short where, const RGBColor *rgb);
#endif

#endif