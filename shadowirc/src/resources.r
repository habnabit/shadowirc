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

resource 'scsz' (0)
{
	launchToGetTerminology,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	
	0, 0, 0, 0, 0, 0
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

