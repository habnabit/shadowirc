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

#include "Types.r"
#include "AEUserTermTypes.r"

resource 'open' (300, purgeable)
{
	'SIRC',
	{
		'TEXT'
	}
};

resource 'open' (301, purgeable)
{
	'SIRC',
	{
		'COLR'
	}
};

resource 'kind' (1000, purgeable)
{
	'SIRC',
	0,
	{
		'apnm', "ShadowIRC",
		
		'TEXT', "ShadowIRC text file",
		'COLR', "ShadowIRC color set",
		'pref', "ShadowIRC preference file",
		'PLUG', "ShadowIRC 68k plugin",
		'shlb', "ShadowIRC PPC plugin",
		'PPLG', "ShadowIRC plugin"
	}
};

resource 'scsz' (0)
{
	launchToGetTerminology,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	
	0, 0, 0, 0, 0, 0
};


data 'CRCt' (128, "CRC Magic Table", locked, preload) {
	$"0000 1021 2042 3063 4084 50A5 60C6 70E7"            /* ...! B0c@„P¥`Æpç */
	$"8108 9129 A14A B16B C18C D1AD E1CE F1EF"            /* .‘)¡J±kÁŒÑ­áÎñï */
	$"1231 0210 3273 2252 52B5 4294 72F7 62D6"            /* .1..2s"RRµB”r÷bÖ */
	$"9339 8318 B37B A35A D3BD C39C F3FF E3DE"            /* “9ƒ.³{£ZÓ½ÃœóÿãŞ */
	$"2462 3443 0420 1401 64E6 74C7 44A4 5485"            /* $b4C. ..dætÇD¤T… */
	$"A56A B54B 8528 9509 E5EE F5CF C5AC D58D"            /* ¥jµK…(•ÆåîõÏÅ¬Õ */
	$"3653 2672 1611 0630 76D7 66F6 5695 46B4"            /* 6S&r...0v×föV•F´ */
	$"B75B A77A 9719 8738 F7DF E7FE D79D C7BC"            /* ·[§z—.‡8÷ßçş×Ç¼ */
	$"48C4 58E5 6886 78A7 0840 1861 2802 3823"            /* HÄXåh†x§.@.a(.8# */
	$"C9CC D9ED E98E F9AF 8948 9969 A90A B92B"            /* ÉÌÙíéù¯‰H™i©Â¹+ */
	$"5AF5 4AD4 7AB7 6A96 1A71 0A50 3A33 2A12"            /* ZõJÔz·j–.qÂP:3*. */
	$"DBFD CBDC FBBF EB9E 9B79 8B58 BB3B AB1A"            /* ÛıËÜû¿ë›y‹X»;«. */
	$"6CA6 7C87 4CE4 5CC5 2C22 3C03 0C60 1C41"            /* l¦|‡Lä\Å,"<..`.A */
	$"EDAE FD8F CDEC DDCD AD2A BD0B 8D68 9D49"            /* í®ıÍìİÍ­*½.hI */
	$"7E97 6EB6 5ED5 4EF4 3E13 2E32 1E51 0E70"            /* ~—n¶^ÕNô>..2.Q.p */
	$"FF9F EFBE DFDD CFFC BF1B AF3A 9F59 8F78"            /* ÿŸï¾ßİÏü¿.¯:ŸYx */
	$"9188 81A9 B1CA A1EB D10C C12D F14E E16F"            /* ‘ˆ©±Ê¡ëÑ.Á-ñNáo */
	$"1080 00A1 30C2 20E3 5004 4025 7046 6067"            /* .€.¡0Â ãP.@%pF`g */
	$"83B9 9398 A3FB B3DA C33D D31C E37F F35E"            /* ƒ¹“˜£û³ÚÃ=Ó.ã.ó^ */
	$"02B1 1290 22F3 32D2 4235 5214 6277 7256"            /* .±."ó2ÒB5R.bwrV */
	$"B5EA A5CB 95A8 8589 F56E E54F D52C C50D"            /* µê¥Ë•¨…‰õnåOÕ,Å. */
	$"34E2 24C3 14A0 0481 7466 6447 5424 4405"            /* 4â$Ã. .tfdGT$D. */
	$"A7DB B7FA 8799 97B8 E75F F77E C71D D73C"            /* §Û·ú‡™—¸ç_÷~Ç.×< */
	$"26D3 36F2 0691 16B0 6657 7676 4615 5634"            /* &Ó6ò.‘.°fWvvF.V4 */
	$"D94C C96D F90E E92F 99C8 89E9 B98A A9AB"            /* ÙLÉmù.é/™È‰é¹Š©« */
	$"5844 4865 7806 6827 18C0 08E1 3882 28A3"            /* XDHex.h'.À.á8‚(£ */
	$"CB7D DB5C EB3F FB1E 8BF9 9BD8 ABBB BB9A"            /* Ë}Û\ë?û.‹ù›Ø«»»š */
	$"4A75 5A54 6A37 7A16 0AF1 1AD0 2AB3 3A92"            /* JuZTj7z.Âñ.Ğ*³:’ */
	$"FD2E ED0F DD6C CD4D BDAA AD8B 9DE8 8DC9"            /* ı.í.İlÍM½ª­‹èÉ */
	$"7C26 6C07 5C64 4C45 3CA2 2C83 1CE0 0CC1"            /* |&l.\dLE<¢,ƒ.à.Á */
	$"EF1F FF3E CF5D DF7C AF9B BFBA 8FD9 9FF8"            /* ï.ÿ>Ï]ß|¯›¿ºÙŸø */
	$"6E17 7E36 4E55 5E74 2E93 3EB2 0ED1 1EF0"            /* n.~6NU^t.“>².Ñ.ğ */
};

/*
	The tables to encode and decode text properly.
*/

data 'Tabl' (256, "Mac -> ISO", locked, preload) {
	$"0001 0203 0405 0607 0809 0A0B 0C0D 0E0F"            /* .........ÆÂ..... */
	$"1011 1213 1415 1617 1819 1A1B 1C1D 1E1F"            /* ................ */
	$"2021 2223 2425 2627 2829 2A2B 2C2D 2E2F"            /*  !"#$%&'()*+,-./ */
	$"3031 3233 3435 3637 3839 3A3B 3C3D 3E3F"            /* 0123456789:;<=>? */
	$"4041 4243 4445 4647 4849 4A4B 4C4D 4E4F"            /* @ABCDEFGHIJKLMNO */
	$"5051 5253 5455 5657 5859 5A5B 5C5D 5E5F"            /* PQRSTUVWXYZ[\]^_ */
	$"6061 6263 6465 6667 6869 6A6B 6C6D 6E6F"            /* `abcdefghijklmno */
	$"7071 7273 7475 7677 7879 7A7B 7C7D 7E7F"            /* pqrstuvwxyz{|}~. */
	$"C4C5 C7C9 D1D6 DCE1 E0E2 E4E3 E5E7 E9E8"            /* ÄÅÇÉÑÖÜáàâäãåçéè */
	$"EAEB EDEC EEEF F1F3 F2F4 F6F5 FAF9 FBFC"            /* êëíìîïñóòôöõúùûü */
	$"DDB0 A2A3 A780 B6DF AEA9 81B4 A882 C6D8"            /* İ°¢£§€¶ß®©´¨‚ÆØ */
	$"83B1 BE84 A5B5 8F85 BDBC 86AA BA87 E6F8"            /* ƒ±¾„¥µ…½¼†ªº‡æø */
	$"BFA1 AC88 9F89 90AB BB8A A0C0 C3D5 91A6"            /* ¿¡¬ˆŸ‰«»Š ÀÃÕ‘¦ */
	$"AD8B B3B2 8CB9 F7D7 FF8D 8EA4 D0F0 DEFE"            /* ­‹³²Œ¹÷×ÿ¤ĞğŞş */
	$"FDB7 9293 94C2 CAC1 CBC8 CDCE CFCC D3D4"            /* ı·’“”ÂÊÁËÈÍÎÏÌÓÔ */
	$"95D2 DADB D99E 9697 AF98 999A B89B 9C9D"            /* •ÒÚÛÙ–—¯˜™š¸›œ */
};

data 'Tabl' (257, "ISO -> Mac", locked, preload) {
	$"0001 0203 0405 0607 0809 0D0B 0C0D 0E0F"            /* .........Æ...... */
	$"1011 1213 1415 1617 1819 1A1B 1C1D 1E1F"            /* ................ */
	$"2021 2223 2425 2627 2829 2A2B 2C2D 2E2F"            /*  !"#$%&'()*+,-./ */
	$"3031 3233 3435 3637 3839 3A3B 3C3D 3E3F"            /* 0123456789:;<=>? */
	$"4041 4243 4445 4647 4849 4A4B 4C4D 4E4F"            /* @ABCDEFGHIJKLMNO */
	$"5051 5253 5455 5657 5859 5A5B 5C5D 5E5F"            /* PQRSTUVWXYZ[\]^_ */
	$"6061 6263 6465 6667 6869 6A6B 6C6D 6E6F"            /* `abcdefghijklmno */
	$"7071 7273 7475 7677 7879 7A7B 7C7D 7E7F"            /* pqrstuvwxyz{|}~. */
	$"A5AA ADB0 B3B7 BABD C3C5 C9D1 D4D9 DAB6"            /* ¥ª­°³·º½ÃÅÉÑÔÙÚ¶ */
	$"C6CE E2E3 E4F0 F6F7 F9FA FBFD FEFF F5C4"            /* ÆÎâãäğö÷ùúûışÿõÄ */
	$"CAC1 A2A3 DBB4 CFA4 ACA9 BBC7 C2D0 A8F8"            /* ÊÁ¢£Û´Ï¤¬©»ÇÂĞ¨ø */
	$"A1B1 D3D2 ABB5 A6E1 FCD5 BCC8 B9B8 B2C0"            /* ¡±ÓÒ«µ¦áüÕ¼È¹¸²À */
	$"CBE7 E5CC 8081 AE82 E983 E6E8 EDEA EBEC"            /* ËçåÌ€®‚éƒæèíêëì */
	$"DC84 F1EE EFCD 85D7 AFF4 F2F3 86A0 DEA7"            /* Ü„ñîïÍ…×¯ôòó† Ş§ */
	$"8887 898B 8A8C BE8D 8F8E 9091 9392 9495"            /* ˆ‡‰‹ŠŒ¾‘“’”• */
	$"DD96 9897 999B 9AD6 BF9D 9C9E 9FE0 DFD8"            /* İ–˜—™›šÖ¿œŸàßØ */
};

