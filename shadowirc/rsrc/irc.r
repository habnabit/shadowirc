#include <Carbon/Carbon.r>

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

resource 'ALRT' (130) {
	{54, 40, 156, 431},
	130,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	alertPositionMainScreen
};

resource 'ALRT' (136, preload) {
	{54, 40, 156, 431},
	136,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	alertPositionMainScreen
};

data 'CDEF' (900, "ColourBlock CDEF 1.1") {
	$"600A 0000 4344 4546 0384 0000 4EFA 0036"            /* `¬..CDEF.Ñ..N˙.6 */
	$"41FA FFEE D1FC 0000 851A 2008 A055 C18C"            /* A˙ˇÓ—¸..Ö. .†U¡å */
	$"4E75 4E75 200C 49FA 0006 2854 4E75 0000"            /* NuNu .I˙..(TNu.. */
	$"0000 41FA FFFA 208C 4E75 8753 6574 5570"            /* ..A˙ˇ˙ åNuáSetUp */
	$"4134 0000 4E56 0000 48E7 1C00 3A2E 000C"            /* A4..NV..HÁ..:... */
	$"7600 4EBA FFBC 2800 4EBA FFD8 396E 0012"            /* v.N∫ˇº(.N∫ˇÿ9n.. */
	$"8020 296E 000E 8022 3945 8026 296E 0008"            /* Ä )n..Ä"9EÄ&)n.. */
	$"8028 3005 48C0 0C80 0000 000B 6240 303B"            /* Ä(0.H¿.Ä....b@0; */
	$"0206 4EFB 0002 002C 0024 0032 0018 001E"            /* ..N˚...,.$.2.... */
	$"0038 0038 0038 0038 0038 0032 0032 4EBA"            /* .8.8.8.8.8.2.2N∫ */
	$"0038 601A 4EBA 010C 6014 4EBA 013A 2600"            /* .8`.N∫..`.N∫.:&. */
	$"600C 4EBA 0190 6006 4EBA 03B0 2608 2004"            /* `.N∫.ê`.N∫.∞&. . */
	$"C18C 2F43 0020 4CDF 0038 4E5E 4E74 000C"            /* ¡å/C. Lﬂ.8N^Nt.. */
	$"846D 6169 6E00 0000 4E56 FFFC 2F0A 4A2C"            /* Ñmain...NVˇ¸/¬J, */
	$"8000 6600 0088 554F 2F2C 8022 486E FFFC"            /* Ä.f..àUO/,Ä"Hnˇ¸ */
	$"AA44 101F 4AAE FFFC 6742 206E FFFC A029"            /* ™D..JÆˇ¸gB nˇ¸†) */
	$"206E FFFC 2050 2468 0008 2052 2968 000A"            /*  nˇ¸ P$h.. R)h.¬ */
	$"8002 3968 000E 8006 2052 2968 001A 8008"            /* Ä.9h..Ä. R)h..Ä. */
	$"3968 001E 800C 2052 2968 0012 800E 3968"            /* 9h..Ä. R)h..Ä.9h */
	$"0016 8012 206E FFFC A02A 602A 426C 8002"            /* ..Ä. nˇ¸†*`*BlÄ. */
	$"426C 8004 426C 8006 426C 8008 426C 800A"            /* BlÄ.BlÄ.BlÄ.BlÄ¬ */
	$"426C 800C 397C FFFF 800E 397C FFFF 8010"            /* BlÄ.9|ˇˇÄ.9|ˇˇÄ. */
	$"397C FFFF 8012 197C 0001 8000 7006 A122"            /* 9|ˇˇÄ..|..Ä.p.°" */
	$"2448 200A 6716 2052 30BC FFFF 2052 317C"            /* $H ¬g. R0ºˇˇ R1| */
	$"FFFF 0002 2052 317C FFFF 0004 206C 8022"            /* ˇˇ.. R1|ˇˇ.. lÄ" */
	$"2050 214A 001C 245F 4E5E 4E75 9349 6E69"            /*  P!J..$_N^NuìIni */
	$"7469 616C 6973 654D 7943 6F6E 7472 6F6C"            /* tialiseMyControl */
	$"0000 4E56 0000 206C 8022 2050 4AA8 001C"            /* ..NV.. lÄ" PJ®.. */
	$"670C 206C 8022 2050 2068 001C A023 4E5E"            /* g. lÄ" P h..†#N^ */
	$"4E75 9044 6973 706F 7365 4D79 436F 6E74"            /* NuêDisposeMyCont */
	$"726F 6C00 0000 4E56 FFFC 2F03 7600 206C"            /* rol...NVˇ¸/.v. l */
	$"8022 2050 7000 1028 0011 0C80 0000 00FF"            /* Ä" Pp..(...Ä...ˇ */
	$"6C2A 3D6C 802A FFFE 202C 8028 7210 E2A0"            /* l*=lÄ*ˇ˛ ,Ä(r.‚† */
	$"3D40 FFFC 554F 2F2E FFFC 206C 8022 2050"            /* =@ˇ¸UO/.ˇ¸ lÄ" P */
	$"4868 0008 A8AD 101F 6702 760B 2003 261F"            /* Hh..®≠..g.v. .&. */
	$"4E5E 4E75 8D54 6573 744D 7943 6F6E 7472"            /* N^NuçTestMyContr */
	$"6F6C 0000 4E56 FFF2 2F03 206C 8022 2050"            /* ol..NVˇÚ/. lÄ" P */
	$"4A28 0010 6700 00A0 306C 802A 2948 8028"            /* J(..g..†0lÄ*)HÄ( */
	$"206C 8022 2050 2968 0008 8014 2968 000C"            /*  lÄ" P)h..Ä.)h.. */
	$"8018 302C 8016 48C0 7215 D081 3940 801A"            /* Ä.0,Ä.H¿r.–Å9@Ä. */
	$"302C 8014 48C0 322C 8014 48C1 362C 8018"            /* 0,Ä.H¿2,Ä.H¡6,Ä. */
	$"48C3 9681 E283 D680 3003 48C0 5D80 3940"            /* H√ñÅ‚É÷Ä0.H¿]Ä9@ */
	$"8014 48C0 720C D081 3940 8018 302C 801A"            /* Ä.H¿r.–Å9@Ä.0,Ä. */
	$"48C0 5880 3940 801E 486E FFF2 A88B 3003"            /* H¿XÄ9@Ä.HnˇÚ®ã0. */
	$"48C0 322E FFF2 48C1 E281 D280 5381 3941"            /* H¿2.ˇÚH¡‚Å“ÄSÅ9A */
	$"801C 486E FFFA AA19 206C 8022 2050 2068"            /* Ä.Hnˇ˙™. lÄ" P h */
	$"0004 2F28 0018 487A 0024 42A7 42A7 ABCA"            /* ../(..Hz.$BßBß´  */
	$"486E FFFA AA14 261F 4E5E 4E75 8D44 7261"            /* Hnˇ˙™.&.N^NuçDra */
	$"774D 7943 6F6E 7472 6F6C 0000 4E56 FFEC"            /* wMyControl..NVˇÏ */
	$"48E7 1820 7600 4EBA FD0C 2800 202C 8028"            /* HÁ. v.N∫˝.(. ,Ä( */
	$"674E 5180 5780 670C 0480 0000 00F4 6740"            /* gNQÄWÄg..Ä...Ùg@ */
	$"6000 0116 206C 8022 2050 4A28 0011 6708"            /* `... lÄ" PJ(..g. */
	$"486C 8002 AA14 6006 486C 800E AA14 2D6C"            /* HlÄ.™.`.HlÄ.™.-l */
	$"8014 FFF8 2D6C 8018 FFFC 486E FFF8 2F3C"            /* Ä.ˇ¯-lÄ.ˇ¸Hnˇ¯/< */
	$"0001 0001 A8A9 486E FFF8 A8A1 6000 00DA"            /* ....®©Hnˇ¯®°`..⁄ */
	$"2D6C 8014 FFF8 2D6C 8018 FFFC 486C 8002"            /* -lÄ.ˇ¯-lÄ.ˇ¸HlÄ. */
	$"AA14 486E FFF8 A8A1 486C 800E AA14 486E"            /* ™.Hnˇ¯®°HlÄ.™.Hn */
	$"FFF8 2F3C 0001 0001 A8A9 486E FFF8 A8A1"            /* ˇ¯/<....®©Hnˇ¯®° */
	$"206C 8022 2050 7000 1028 0011 0C80 0000"            /*  lÄ" Pp..(...Ä.. */
	$"00FF 662A 2D6C 800E FFEC 3D6C 8012 FFF0"            /* .ˇf*-lÄ.ˇÏ=lÄ.ˇ */
	$"3D7C 8000 FFF2 3D7C 8000 FFF4 3D7C 8000"            /* =|Ä.ˇÚ=|Ä.ˇÙ=|Ä. */
	$"FFF6 0C6E 0001 0012 6626 7601 6022 206C"            /* ˇˆ.n....f&v.`" l */
	$"8022 2050 2468 001C 2052 2D50 FFEC 3D68"            /* Ä" P$h.. R-PˇÏ=h */
	$"0004 FFF0 2D6C 8002 FFF2 3D6C 8006 FFF6"            /* ..ˇ-lÄ.ˇÚ=lÄ.ˇˆ */
	$"486E FFEC AA14 486E FFF8 2F3C 0001 0001"            /* HnˇÏ™.Hnˇ¯/<.... */
	$"A8A9 486E FFF8 A8A2 486E FFF2 AA14 3F2C"            /* ®©Hnˇ¯®¢HnˇÚ™.?, */
	$"801E 3F2C 801C A893 4A03 6706 3F3C 0031"            /* Ä.?,Ä.®ìJ.g.?<.1 */
	$"A889 206C 8022 2050 4868 0028 A884 4A03"            /* ®â lÄ" PHh.(®ÑJ. */
	$"6706 3F3C 0001 A889 2004 C18C 4CDF 0418"            /* g.?<..®â .¡åLﬂ.. */
	$"4E5E 4E74 000C 9144 4556 4943 454C 4F4F"            /* N^Nt..ëDEVICELOO */
	$"5052 4F55 5449 4E45 0000 4E56 FFF8 48E7"            /* PROUTINE..NVˇ¯HÁ */
	$"1020 246C 8028 0C6C 0002 8026 661A 200A"            /* . $lÄ(.l..Ä&f. ¬ */
	$"0800 001F 6704 760B 6002 760A 200A 0280"            /* ....g.v.`.v¬ ¬.Ä */
	$"7FFF FFFF 2440 6004 362C 8026 206C 8022"            /* .ˇˇˇ$@`.6,Ä& lÄ" */
	$"2050 2D68 0008 FFF8 2D68 000C FFFC 0C43"            /*  P-h..ˇ¯-h..ˇ¸.C */
	$"000B 6622 206C 8022 2050 2D68 0008 FFF8"            /* ..f" lÄ" P-h..ˇ¯ */
	$"2D68 000C FFFC 302E FFFA 48C0 7215 D081"            /* -h..ˇ¸0.ˇ˙H¿r.–Å */
	$"3D40 FFFE 6012 206C 8022 2050 2D68 0008"            /* =@ˇ˛`. lÄ" P-h.. */
	$"FFF8 2D68 000C FFFC 2F0A 486E FFF8 A8DF"            /* ˇ¯-h..ˇ¸/¬Hnˇ¯®ﬂ */
	$"0C6C 0002 8026 6704 7001 2440 204A 4CDF"            /* .l..Ä&g.p.$@ JLﬂ */
	$"0408 4E5E 4E75 9143 616C 6375 6C61 7465"            /* ..N^NuëCalculate */
	$"4D79 5265 6769 6F6E 0000 0000 0000 0000"            /* MyRegion........ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000"                                     /* ...... */
};

resource 'CNTL' (128) {
	{118, 69, 389, 579},
	0,
	visible,
	0,
	0,
	160,
	0,
	""
};

resource 'CNTL' (503) {
	{8, 8, 203, 96},
	500,
	visible,
	0,
	0,
	360,
	0,
	""
};

resource 'CNTL' (504) {
	{8, 104, 203, 328},
	500,
	visible,
	0,
	0,
	360,
	0,
	""
};

resource 'CNTL' (1000, "sicServer") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Server"
};

resource 'CNTL' (1001, "sicStandard") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Normal"
};

resource 'CNTL' (1002, "sicPrivmsg") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Private Messages"
};

resource 'CNTL' (1003, "sicCTCP") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"CTCP"
};

resource 'CNTL' (1004, "sicOut") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Your text"
};

resource 'CNTL' (1005, "sicNotice") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Notice"
};

resource 'CNTL' (1006, "sicWindowBG") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Message Window Background"
};

resource 'CNTL' (1007, "sicChannelOp") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Channel Operator"
};

resource 'CNTL' (1008, "sicVoice") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"User with Voice"
};

resource 'CNTL' (1009, "sicOper") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"IRC Operator"
};

resource 'CNTL' (1010, "sicRegularUser") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Regular User"
};

resource 'CNTL' (1011, "sicStatusLine") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Status Line text"
};

resource 'CNTL' (1012, "sicUserIgnored") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Ignored User"
};

resource 'CNTL' (1013, "sicNotifyColor") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Notify"
};

resource 'CNTL' (1014, "sicTimestampColor") {
	{0, 0, 12, 160},
	0,
	visible,
	1,
	0,
	14400,
	0,
	"Timestamp Color"
};

resource 'CNTL' (10000, "Prefs") {
	{1, 2, 350, 524},
	0,
	visible,
	0,
	-12345,
	162,
	0,
	"P"
};

resource 'CNTL' (10001, "Prefs.onotice-method") {
	{27, 25, 48, 275},
	0,
	visible,
	90,
	10001,
	409,
	0,
	"ONotice method:"
};

resource 'CNTL' (10002, "Prefs.Font") {
	{27, 25, 48, 225},
	0,
	visible,
	70,
	10002,
	412,
	1179602516,
	"Default Font:"
};

resource 'CNTL' (10003, "Prefs.ColorMethod") {
	{27, 25, 48, 225},
	0,
	visible,
	110,
	203,
	409,
	0,
	"Color Processing:"
};

resource 'CNTL' (10005, "DCCBufSiz") {
	{56, 251, 77, 510},
	0,
	visible,
	125,
	10005,
	409,
	0,
	"DCC Send Packet Size:"
};

resource 'CNTL' (10006, "firewallType") {
	{27, 25, 48, 240},
	0,
	visible,
	110,
	10006,
	409,
	0,
	"Firewall Type:"
};

resource 'CNTL' (10100, "Server Options") {
	{0, 0, 115, 390},
	0,
	visible,
	100,
	0,
	164,
	0,
	"Server Options"
};

resource 'CNTL' (10124) {
	{0, 0, 130, 101},
	10124,
	visible,
	0,
	0,
	360,
	0,
	""
};

resource 'CNTL' (10201) {
	{0, 0, 74, 220},
	0,
	visible,
	100,
	0,
	164,
	0,
	"When quitting ShadowIRC"
};

resource 'CNTL' (10720) {
	{0, 0, 82, 220},
	0,
	visible,
	100,
	0,
	164,
	0,
	"Show"
};

resource 'DITL' (-301) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{135, 252, 155, 332},
		Button {
			enabled,
			"Send"
		},
		/* [2] */
		{104, 252, 124, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		HelpItem {
			disabled,
			HMScanhdlg {
				-6042
			}
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 159, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{6, 12, 25, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 251, 92, 333},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{165, 12, 183, 118},
		RadioButton {
			enabled,
			"Binary"
		},
		/* [11] */
		{165, 124, 183, 230},
		RadioButton {
			enabled,
			"MacBinary"
		}
	}
};

resource 'DITL' (-300) {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{135, 252, 155, 332},
		Button {
			enabled,
			"Open"
		},
		/* [2] */
		{104, 252, 124, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		HelpItem {
			disabled,
			HMScanhdlg {
				-6042
			}
		},
		/* [4] */
		{8, 235, 24, 337},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 252, 52, 332},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{60, 252, 80, 332},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{29, 12, 159, 230},
		UserItem {
			enabled
		},
		/* [8] */
		{6, 12, 25, 230},
		UserItem {
			enabled
		},
		/* [9] */
		{91, 251, 92, 333},
		Picture {
			disabled,
			11
		},
		/* [10] */
		{164, 12, 184, 230},
		Button {
			enabled,
			""
		}
	}
};

resource 'DITL' (130) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{77, 166, 97, 224},
		Button {
			enabled,
			"Quit"
		},
		/* [2] */
		{2, 4, 34, 36},
		Icon {
			disabled,
			0
		},
		/* [3] */
		{3, 39, 70, 388},
		StaticText {
			disabled,
			"^0 ^1\n^2"
		}
	}
};

resource 'DITL' (136, "nonfatal error") {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{77, 166, 97, 224},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{2, 4, 34, 36},
		Icon {
			disabled,
			0
		},
		/* [3] */
		{3, 39, 70, 388},
		StaticText {
			disabled,
			"^0 ^1\n^2"
		}
	}
};

resource 'DITL' (141, "Font Size") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{62, 125, 82, 183},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{62, 7, 82, 65},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{3, 2, 33, 187},
		StaticText {
			disabled,
			"Set font size to:"
		},
		/* [4] */
		{39, 66, 53, 125},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (500, "Servers") {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{222, 345, 242, 413},
		Button {
			enabled,
			"Select"
		},
		/* [2] */
		{222, 265, 242, 329},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{8, 8, 203, 96},
		UserItem {
			enabled
		},
		/* [4] */
		{8, 104, 203, 328},
		UserItem {
			enabled
		},
		/* [5] */
		{32, 347, 52, 411},
		Button {
			enabled,
			"Edit"
		},
		/* [6] */
		{182, 347, 202, 411},
		Button {
			enabled,
			"Defaults"
		},
		/* [7] */
		{54, 347, 74, 411},
		Button {
			enabled,
			"Delete"
		},
		/* [8] */
		{10, 347, 30, 411},
		Button {
			enabled,
			"Add"
		},
		/* [9] */
		{87, 347, 107, 411},
		Button {
			enabled,
			"Export"
		}
	}
};

resource 'DITL' (501) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{86, 225, 106, 283},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{86, 148, 106, 206},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{3, 5, 35, 37},
		Icon {
			disabled,
			0
		},
		/* [4] */
		{4, 40, 68, 283},
		StaticText {
			disabled,
			"Resetting the server list will delete al"
			"l servers you added.\n\nThis operation can"
			" not be undone."
		}
	}
};

resource 'DITL' (502) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{84, 227, 104, 285},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{84, 133, 104, 191},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{12, 13, 25, 81},
		StaticText {
			disabled,
			"Server Name:"
		},
		/* [4] */
		{39, 54, 52, 81},
		StaticText {
			disabled,
			"Port:"
		},
		/* [5] */
		{13, 90, 26, 284},
		EditText {
			enabled,
			""
		},
		/* [6] */
		{41, 90, 54, 164},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (601, "DCCSendNav") {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{6, 7, 24, 113},
		RadioButton {
			enabled,
			"Binary"
		},
		/* [2] */
		{6, 119, 24, 225},
		RadioButton {
			enabled,
			"MacBinary"
		}
	}
};

resource 'DITL' (1001, "Prefs.Conn") {
	{	/* array DITLarray: 32 elements */
		/* [1] */
		{33, 288, 46, 413},
		EditText {
			enabled,
			""
		},
		/* [2] */
		{56, 208, 69, 429},
		EditText {
			enabled,
			""
		},
		/* [3] */
		{56, 467, 69, 515},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{79, 208, 92, 334},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{102, 208, 115, 306},
		EditText {
			enabled,
			""
		},
		/* [6] */
		{125, 208, 138, 306},
		EditText {
			enabled,
			""
		},
		/* [7] */
		{125, 379, 138, 477},
		EditText {
			enabled,
			""
		},
		/* [8] */
		{148, 208, 161, 515},
		EditText {
			enabled,
			""
		},
		/* [9] */
		{33, 192, 45, 282},
		StaticText {
			disabled,
			"Connection Name:"
		},
		/* [10] */
		{33, 11, 163, 112},
		Control {
			enabled,
			10124
		},
		/* [11] */
		{176, 121, 297, 511},
		Control {
			enabled,
			10100
		},
		/* [12] */
		{198, 128, 213, 295},
		CheckBox {
			enabled,
			"Auto-connect on startup"
		},
		/* [13] */
		{55, 136, 67, 203},
		StaticText {
			disabled,
			"Server Name:"
		},
		/* [14] */
		{79, 340, 91, 409},
		StaticText {
			disabled,
			"(optional)"
		},
		/* [15] */
		{215, 128, 230, 278},
		CheckBox {
			enabled,
			"Reconnect to Server"
		},
		/* [16] */
		{102, 152, 114, 203},
		StaticText {
			disabled,
			"Nickname:"
		},
		/* [17] */
		{198, 339, 213, 507},
		CheckBox {
			enabled,
			"Display Server MOTD"
		},
		/* [18] */
		{250, 339, 265, 507},
		CheckBox {
			enabled,
			"Show server messages"
		},
		/* [19] */
		{232, 339, 248, 439},
		CheckBox {
			enabled,
			"Show Wallops"
		},
		/* [20] */
		{125, 152, 137, 203},
		StaticText {
			disabled,
			"Alternate:"
		},
		/* [21] */
		{125, 318, 137, 374},
		StaticText {
			disabled,
			"User Name:"
		},
		/* [22] */
		{56, 434, 68, 461},
		StaticText {
			disabled,
			"Port:"
		},
		/* [23] */
		{148, 150, 160, 203},
		StaticText {
			disabled,
			"Real name:"
		},
		/* [24] */
		{215, 339, 230, 507},
		CheckBox {
			enabled,
			"Go \"Invisible\""
		},
		/* [25] */
		{78, 116, 90, 204},
		StaticText {
			disabled,
			"Server Password:"
		},
		/* [26] */
		{232, 135, 245, 289},
		StaticText {
			disabled,
			"Maximum Connection Attempts:"
		},
		/* [27] */
		{252, 170, 265, 289},
		StaticText {
			disabled,
			"Connection Retry Delay:"
		},
		/* [28] */
		{232, 294, 245, 324},
		EditText {
			enabled,
			""
		},
		/* [29] */
		{252, 294, 265, 324},
		EditText {
			enabled,
			""
		},
		/* [30] */
		{272, 128, 287, 312},
		CheckBox {
			enabled,
			"Attempt to Regain Lost Nicks"
		},
		/* [31] */
		{76, 407, 96, 510},
		Button {
			enabled,
			"Select Server"
		}
	}
};

resource 'DITL' (1002, "Prefs.Misc") {
	{	/* array DITLarray: 17 elements */
		/* [1] */
		{35, 15, 117, 235},
		Control {
			disabled,
			10201
		},
		/* [2] */
		{53, 23, 69, 223},
		RadioButton {
			enabled,
			"Ask to save prefs before quitting"
		},
		/* [3] */
		{70, 23, 86, 223},
		RadioButton {
			enabled,
			"Save prefs and quit"
		},
		/* [4] */
		{87, 23, 103, 223},
		RadioButton {
			enabled,
			"Quit without saving prefs"
		},
		/* [5] */
		{120, 15, 136, 248},
		CheckBox {
			enabled,
			"Beep on private messages"
		},
		/* [6] */
		{137, 15, 153, 248},
		CheckBox {
			enabled,
			"Auto-rejoin after kick"
		},
		/* [7] */
		{154, 15, 170, 248},
		CheckBox {
			enabled,
			"Auto-rejoin channels on reconnect"
		},
		/* [8] */
		{171, 15, 187, 248},
		CheckBox {
			enabled,
			"Auto-accept invites"
		},
		/* [9] */
		{188, 25, 204, 258},
		CheckBox {
			enabled,
			"but not when away"
		},
		/* [10] */
		{35, 290, 51, 504},
		CheckBox {
			enabled,
			"Escape clears inputline"
		},
		/* [11] */
		{205, 15, 221, 248},
		CheckBox {
			enabled,
			"Require option key to scroll inputline b"
			"uffer"
		},
		/* [12] */
		{52, 290, 68, 504},
		CheckBox {
			enabled,
			"No control-G beeps"
		},
		/* [13] */
		{69, 290, 85, 504},
		CheckBox {
			enabled,
			"Hide splash screen"
		},
		/* [14] */
		{86, 290, 102, 504},
		CheckBox {
			enabled,
			"Do /whowas if /whois fails"
		},
		/* [15] */
		{103, 290, 119, 504},
		CheckBox {
			enabled,
			"Don't load fonts menu"
		},
		/* [16] */
		{120, 290, 136, 504},
		CheckBox {
			enabled,
			"Reset away message on reconnect"
		},
		/* [17] */
		{257, 14, 278, 264},
		Control {
			enabled,
			10001
		}
	}
};

resource 'DITL' (1004, "Prefs.MAIN") {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{355, 461, 375, 519},
		Button {
			enabled,
			"Done"
		},
		/* [2] */
		{355, 3, 375, 67},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{1, 2, 350, 524},
		Control {
			enabled,
			10000
		}
	}
};

resource 'DITL' (1005, "Prefs.DCC") {
	{	/* array DITLarray: 12 elements */
		/* [1] */
		{51, 15, 66, 248},
		CheckBox {
			enabled,
			"Auto-Accept DCC Chat"
		},
		/* [2] */
		{67, 24, 82, 248},
		CheckBox {
			enabled,
			"but not when away"
		},
		/* [3] */
		{35, 15, 50, 248},
		CheckBox {
			enabled,
			"Enable DCC"
		},
		/* [4] */
		{142, 14, 158, 94},
		StaticText {
			disabled,
			"DCC save folder:"
		},
		/* [5] */
		{140, 97, 160, 155},
		Button {
			enabled,
			"Select…"
		},
		/* [6] */
		{142, 160, 177, 360},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{115, 15, 130, 248},
		CheckBox {
			enabled,
			"Automatically save DCC files"
		},
		/* [8] */
		{99, 15, 114, 248},
		CheckBox {
			enabled,
			"Automatically accept DCC files"
		},
		/* [9] */
		{36, 251, 51, 471},
		CheckBox {
			enabled,
			"Use Fast(er) DCC Sends"
		},
		/* [10] */
		{56, 251, 77, 510},
		Control {
			enabled,
			10005
		},
		/* [11] */
		{99, 251, 114, 484},
		CheckBox {
			enabled,
			"Use reversed sends (ShadowIRC only)"
		},
		/* [12] */
		{187, 15, 202, 248},
		CheckBox {
			enabled,
			"Auto-open DCC status window"
		}
	}
};

resource 'DITL' (1006, "Prefs.CTCP") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{35, 15, 50, 248},
		CheckBox {
			enabled,
			"Disable CTCP (doesn't disable DCC)"
		},
		/* [2] */
		{50, 15, 65, 248},
		CheckBox {
			enabled,
			"Don't display CTCP requests"
		},
		/* [3] */
		{65, 15, 80, 248},
		CheckBox {
			enabled,
			"Don't return unknown CTCP errors"
		},
		/* [4] */
		{80, 15, 95, 248},
		CheckBox {
			enabled,
			"Display all CTCPs in console"
		}
	}
};

resource 'DITL' (1007, "Prefs.Text") {
	{	/* array DITLarray: 27 elements */
		/* [1] */
		{35, 15, 50, 248},
		CheckBox {
			enabled,
			"Show end of command messages"
		},
		/* [2] */
		{51, 15, 66, 248},
		CheckBox {
			enabled,
			"Show user@host with incoming/msgs"
		},
		/* [3] */
		{136, 14, 151, 247},
		CheckBox {
			enabled,
			"Timestamp messages"
		},
		/* [4] */
		{152, 25, 167, 258},
		CheckBox {
			enabled,
			"Show seconds in timestamps"
		},
		/* [5] */
		{193, 15, 208, 248},
		CheckBox {
			enabled,
			"Display Server notices in console"
		},
		/* [6] */
		{209, 15, 224, 248},
		CheckBox {
			enabled,
			"Display Wallops in console"
		},
		/* [7] */
		{58, 259, 74, 492},
		CheckBox {
			enabled,
			"Disable Color"
		},
		/* [8] */
		{74, 259, 89, 492},
		CheckBox {
			enabled,
			"Disable Styles"
		},
		/* [9] */
		{77, 15, 90, 136},
		StaticText {
			disabled,
			"Server Message Prefix:"
		},
		/* [10] */
		{77, 140, 90, 215},
		EditText {
			enabled,
			""
		},
		/* [11] */
		{168, 25, 183, 258},
		CheckBox {
			enabled,
			"Show timestamps only when away"
		},
		/* [12] */
		{225, 15, 240, 248},
		CheckBox {
			enabled,
			"Display private messages in console"
		},
		/* [13] */
		{92, 269, 113, 469},
		Control {
			enabled,
			10002
		},
		/* [14] */
		{118, 287, 131, 338},
		StaticText {
			disabled,
			"Font Size:"
		},
		/* [15] */
		{118, 346, 131, 381},
		EditText {
			enabled,
			""
		},
		/* [16] */
		{115, 14, 130, 247},
		CheckBox {
			enabled,
			"Text indenting"
		},
		/* [17] */
		{241, 15, 256, 248},
		CheckBox {
			enabled,
			"Display server pings in console"
		},
		/* [18] */
		{35, 259, 56, 459},
		Control {
			enabled,
			10003
		},
		/* [19] */
		{99, 14, 114, 247},
		CheckBox {
			enabled,
			"IrcII text display"
		},
		/* [20] */
		{178, 268, 260, 488},
		Control {
			disabled,
			10720
		},
		/* [21] */
		{193, 274, 208, 374},
		CheckBox {
			enabled,
			"Channel Joins"
		},
		/* [22] */
		{209, 274, 224, 374},
		CheckBox {
			enabled,
			"Channel Parts"
		},
		/* [23] */
		{193, 376, 208, 476},
		CheckBox {
			enabled,
			"Signoffs"
		},
		/* [24] */
		{209, 376, 224, 476},
		CheckBox {
			enabled,
			"Mode Changes"
		},
		/* [25] */
		{225, 274, 240, 374},
		CheckBox {
			enabled,
			"Nick changes"
		},
		/* [26] */
		{225, 376, 240, 476},
		CheckBox {
			enabled,
			"Kicks"
		},
		/* [27] */
		{241, 274, 256, 374},
		CheckBox {
			enabled,
			"Invites"
		}
	}
};

resource 'DITL' (1008, "Prefs.Color") {
	{	/* array DITLarray: 19 elements */
		/* [1] */
		{38, 40, 50, 200},
		Control {
			enabled,
			1000
		},
		/* [2] */
		{51, 40, 63, 200},
		Control {
			enabled,
			1001
		},
		/* [3] */
		{64, 40, 76, 200},
		Control {
			enabled,
			1002
		},
		/* [4] */
		{77, 40, 89, 200},
		Control {
			enabled,
			1003
		},
		/* [5] */
		{90, 40, 102, 200},
		Control {
			enabled,
			1004
		},
		/* [6] */
		{103, 40, 115, 200},
		Control {
			enabled,
			1005
		},
		/* [7] */
		{116, 40, 128, 200},
		Control {
			enabled,
			1006
		},
		/* [8] */
		{170, 40, 182, 200},
		Control {
			enabled,
			1007
		},
		/* [9] */
		{183, 40, 195, 200},
		Control {
			enabled,
			1008
		},
		/* [10] */
		{196, 40, 208, 200},
		Control {
			enabled,
			1009
		},
		/* [11] */
		{209, 40, 221, 200},
		Control {
			enabled,
			1010
		},
		/* [12] */
		{38, 201, 50, 361},
		Control {
			enabled,
			1011
		},
		/* [13] */
		{222, 40, 234, 200},
		Control {
			enabled,
			1012
		},
		/* [14] */
		{129, 40, 141, 200},
		Control {
			enabled,
			1013
		},
		/* [15] */
		{51, 201, 67, 301},
		Control {
			enabled,
			1014
		},
		/* [16] */
		{153, 40, 169, 162},
		StaticText {
			disabled,
			"Userlist Colors:"
		},
		/* [17] */
		{40, 441, 60, 516},
		Button {
			enabled,
			"Export"
		},
		/* [18] */
		{63, 441, 83, 516},
		Button {
			enabled,
			"Import"
		},
		/* [19] */
		{257, 15, 277, 90},
		Button {
			enabled,
			"Defaults"
		}
	}
};

resource 'DITL' (1009, "Prefs.Logging") {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{35, 15, 50, 235},
		CheckBox {
			enabled,
			"Automatically log channel text"
		},
		/* [2] */
		{87, 14, 103, 89},
		StaticText {
			disabled,
			"Log folder:"
		},
		/* [3] */
		{85, 92, 105, 150},
		Button {
			enabled,
			"Select…"
		},
		/* [4] */
		{87, 155, 122, 355},
		StaticText {
			disabled,
			""
		},
		/* [5] */
		{51, 15, 66, 235},
		CheckBox {
			enabled,
			"Automatically log /queries"
		},
		/* [6] */
		{67, 15, 82, 235},
		CheckBox {
			enabled,
			"Automatically log DCC chat"
		}
	}
};

resource 'DITL' (1010, "Prefs.SOCKS") {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{47, 21, 68, 236},
		Control {
			enabled,
			10006
		},
		/* [2] */
		{77, 52, 91, 78},
		StaticText {
			disabled,
			"Host:"
		},
		/* [3] */
		{101, 51, 115, 78},
		StaticText {
			disabled,
			"Port:"
		},
		/* [4] */
		{78, 87, 92, 235},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{102, 87, 116, 137},
		EditText {
			enabled,
			""
		},
		/* [6] */
		{131, 25, 145, 78},
		StaticText {
			disabled,
			"Username:"
		},
		/* [7] */
		{154, 27, 168, 78},
		StaticText {
			disabled,
			"Password:"
		},
		/* [8] */
		{132, 87, 146, 235},
		EditText {
			enabled,
			""
		},
		/* [9] */
		{155, 87, 169, 235},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (1011, "Prefs.Messages") {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{35, 240, 80, 495},
		EditText {
			enabled,
			""
		},
		/* [2] */
		{91, 240, 136, 495},
		EditText {
			enabled,
			""
		},
		/* [3] */
		{147, 240, 192, 495},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{32, 153, 46, 234},
		StaticText {
			disabled,
			"Signoff message:"
		},
		/* [5] */
		{88, 127, 102, 234},
		StaticText {
			disabled,
			"CTCP Finger message:"
		},
		/* [6] */
		{144, 118, 158, 234},
		StaticText {
			disabled,
			"CTCP userinfo message:"
		},
		/* [7] */
		{209, 165, 223, 234},
		StaticText {
			disabled,
			"Kick Message:"
		},
		/* [8] */
		{203, 240, 248, 495},
		EditText {
			disabled,
			""
		},
		/* [9] */
		{210, 56, 242, 88},
		UserItem {
			disabled
		},
		/* [10] */
		{33, 11, 163, 112},
		Control {
			enabled,
			10124
		}
	}
};

resource 'DITL' (1012, "Prefs.Windows") {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{35, 15, 51, 229},
		CheckBox {
			enabled,
			"Auto-open query windows for /msgs"
		},
		/* [2] */
		{52, 25, 68, 239},
		RadioButton {
			enabled,
			"for all messages"
		},
		/* [3] */
		{69, 25, 85, 239},
		RadioButton {
			enabled,
			"only for outgoing messages"
		},
		/* [4] */
		{86, 25, 102, 239},
		RadioButton {
			enabled,
			"only for incoming messages"
		},
		/* [5] */
		{137, 15, 153, 229},
		CheckBox {
			enabled,
			"Don't show channel modes display"
		},
		/* [6] */
		{154, 15, 170, 229},
		CheckBox {
			enabled,
			"Allow Drag and Drop to same window"
		},
		/* [7] */
		{171, 15, 187, 229},
		CheckBox {
			enabled,
			"Put inputline in message windows"
		},
		/* [8] */
		{188, 15, 204, 300},
		CheckBox {
			enabled,
			"Don't activate new windows if inputline "
			"contains text"
		}
	}
};

resource 'DITL' (1013, "Prefs.Startup Actions") {
	{	/* array DITLarray: 10 elements */
		/* [1] */
		{169, 129, 184, 289},
		CheckBox {
			enabled,
			"Use Connect Script"
		},
		/* [2] */
		{188, 175, 224, 515},
		EditText {
			enabled,
			""
		},
		/* [3] */
		{189, 129, 221, 170},
		StaticText {
			disabled,
			"Connect Script:"
		},
		/* [4] */
		{261, 203, 293, 235},
		UserItem {
			disabled
		},
		/* [5] */
		{271, 213, 303, 245},
		UserItem {
			disabled
		},
		/* [6] */
		{31, 255, 161, 356},
		UserItem {
			enabled
		},
		/* [7] */
		{34, 373, 54, 431},
		UserItem {
			enabled
		},
		/* [8] */
		{57, 373, 77, 431},
		UserItem {
			enabled
		},
		/* [9] */
		{33, 131, 49, 248},
		UserItem {
			disabled
		},
		/* [10] */
		{33, 11, 163, 112},
		Control {
			enabled,
			10124
		}
	}
};

resource 'DITL' (10001, "+l") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{62, 125, 82, 183},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{62, 7, 82, 65},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{3, 2, 33, 187},
		StaticText {
			disabled,
			"Set user limit on ^0 to:"
		},
		/* [4] */
		{39, 66, 53, 125},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (10002, "+k") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{62, 125, 82, 183},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{62, 7, 82, 65},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{3, 2, 33, 187},
		StaticText {
			disabled,
			"Set channel key for ^0 to:"
		},
		/* [4] */
		{39, 45, 53, 145},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DLOG' (-301) {
	{0, 0, 188, 344},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	-301,
	"",
	noAutoCenter
};

resource 'DLOG' (-300) {
	{0, 0, 186, 344},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	-300,
	"",
	noAutoCenter
};

data 'DLOG' (130, "Err") {
	$"0036 0028 009C 01AF 0001 0100 0100 0000"            /* .6.(.ú.Ø........ */
	$"0000 0082 00"                                       /* ...Ç. */
};

resource 'DLOG' (132, "Quit/Save Prefs") {
	{73, 90, 161, 410},
	dBoxProc,
	visible,
	noGoAway,
	0x0,
	132,
	"Quit",
	alertPositionMainScreen
};

resource 'DLOG' (141, "Font Size") {
	{180, 266, 267, 456},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	141,
	"Font Size",
	alertPositionMainScreen
};

resource 'DLOG' (300) {
	{0, 0, 221, 348},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	300,
	"",
	noAutoCenter
};

resource 'DLOG' (500) {
	{117, 160, 367, 585},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	500,
	"Server List",
	centerMainScreen
};

resource 'DLOG' (501) {
	{75, 166, 191, 456},
	dBoxProc,
	visible,
	goAway,
	0x0,
	501,
	"Reset to Defaults?",
	alertPositionParentWindow
};

resource 'DLOG' (502, "EditServer") {
	{84, 133, 195, 427},
	movableDBoxProc,
	visible,
	goAway,
	0x0,
	502,
	"Edit Server",
	centerParentWindow
};

resource 'DLOG' (1004, "Prefs MAIN") {
	{66, 48, 444, 577},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	1004,
	"ShadowIRC Preferences",
	centerMainScreen
};

resource 'DLOG' (10001, "mode +l") {
	{-666, -666, -579, -476},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	10001,
	"Channel Limit",
	alertPositionParentWindow
};

resource 'DLOG' (10002, "mode +k") {
	{-666, -666, -579, -476},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	10002,
	"Channel Key",
	alertPositionParentWindow
};

resource 'ICN#' (129) {
	{	/* array: 2 elements */
		/* [1] */
		$"BC00 0000 4200 1FFE 2D00 1FDE 9D00 1FF6"
		$"FF00 1FDE 0300 1FF6 FFC0 1FDE 003C 1776"
		$"FFD7 1FEA FFFE 9DD6 FFFF 9FEA FFFF 1756"
		$"FF5C 18AA F360 120A CBE0 18C2 E760 1216"
		$"FFC0 18A2 FFC0 1FFE FFC0 0000 FE80 0000"
		$"FD00 0000 FB00 0000 FF00 0000 FDC0 0000"
		$"FD78 0000 FBD4 0000 FFAB 0000 FFFF 8000"
		$"FFB6 8000 FF6F 8000 FF8F 8000 FF7B 80",
		/* [2] */
		$"7800 0000 FC00 1FFE FC00 1FFE FE00 1FFE"
		$"FE00 1FFE FE00 1FFE FE00 1FFE FFF0 1FFE"
		$"FFFC 1FFE FFFE 1FFE FFFF 1FFE FFFF 1FFE"
		$"FFFE 1FFE FFFC 1FFE FFF0 1FFE FFC0 1FFE"
		$"FFC0 1FFE FFC0 1FFE FFC0 0000 FF80 0000"
		$"FF80 0000 FF00 0000 FF00 0000 FF00 0000"
		$"FFE0 0000 FFFC 0000 FFFE 0000 FFFF 0000"
		$"FFFF 0000 FFFF 0000 FFFF 0000 FFFF"
	}
};

resource 'ICN#' (130) {
	{	/* array: 2 elements */
		/* [1] */
		$"BC00 0000 4200 0000 0900 0000 9D00 0000"
		$"FF00 0000 0300 0000 FFC0 0000 003C 0000"
		$"FFC3 0000 FFFC 8000 FFFF 8000 FFFF 0000"
		$"FF5C 0000 F360 7000 CBE0 8800 E761 2400"
		$"FFC2 1400 FFC2 8400 FFC2 4800 FE81 1200"
		$"FD00 E140 FB00 08C0 FF00 05E0 FDC0 03E0"
		$"FD38 07E0 FB44 01F0 FFAB 0018 FFFB 8008"
		$"FFB2 8008 FF4F 80F0 FF07 8300 FE6B FC",
		/* [2] */
		$"7800 0000 FC00 0000 FC00 0000 FE00 0000"
		$"FE00 0000 FE00 0000 FE00 0000 FFF0 0000"
		$"FFFC 0000 FFFE 0000 FFFF 0000 FFFF 0000"
		$"FFFE 0000 FFFC 7000 FFF0 F800 FFC1 FC00"
		$"FFC3 FC00 FFC3 FC00 FFC3 F800 FF81 F200"
		$"FF80 E140 FF00 08C0 FF00 05E0 FF00 03E0"
		$"FFE0 07E0 FFFC 01F0 FFFE 0018 FFFF 0008"
		$"FFFF 0008 FFFF 00F0 FFFF 0300 FFFF FC"
	}
};

resource 'MENU' (100) {
	100,
	textMenuProc,
	0x7FFFFFFC,
	enabled,
	"iw.Target.RecentChannels",
	{	/* array: 3 elements */
		/* [1] */
		"Recent Channels", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Clear List", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (101) {
	101,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"iw.Links",
	{	/* array: 3 elements */
		/* [1] */
		"Open Connection", noIcon, hierarchicalMenu, "Õ", plain,
		/* [2] */
		"Close Connection", noIcon, hierarchicalMenu, "Œ", plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (203, "colorMethod") {
	203,
	textMenuProc,
	allEnabled,
	enabled,
	"Color Method",
	{	/* array: 2 elements */
		/* [1] */
		"Macintosh", noIcon, noKey, noMark, plain,
		/* [2] */
		"mIRC", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (1001, "iw.Target") {
	1001,
	63,
	0x7FFFFFFD,
	enabled,
	"iw.Target",
	{	/* array: 2 elements */
		/* [1] */
		"Console", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (1002) {
	1002,
	63,
	0x7FFFFFFE,
	enabled,
	"iw.RecentNicks",
	{	/* array: 3 elements */
		/* [1] */
		"-", noIcon, noKey, noMark, plain,
		/* [2] */
		"Clear List", noIcon, noKey, noMark, plain,
		/* [3] */
		"Other…", noIcon, noKey, noMark, 2
	}
};

resource 'MENU' (10001) {
	10001,
	textMenuProc,
	allEnabled,
	enabled,
	"/onotice method",
	{	/* array: 3 elements */
		/* [1] */
		"Message each user", noIcon, noKey, noMark, plain,
		/* [2] */
		"/notice @#channel", noIcon, noKey, noMark, plain,
		/* [3] */
		"/wallchops", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (10002) {
	10002,
	textMenuProc,
	allEnabled,
	enabled,
	"Font",
	{	/* array: 0 elements */
	}
};

resource 'MENU' (10005) {
	10005,
	textMenuProc,
	allEnabled,
	enabled,
	"DCCBufSiz",
	{	/* array: 6 elements */
		/* [1] */
		"16k (Fastest)", noIcon, noKey, noMark, plain,
		/* [2] */
		"8k", noIcon, noKey, noMark, plain,
		/* [3] */
		"4k (Reccommended)", noIcon, noKey, noMark, plain,
		/* [4] */
		"2k", noIcon, noKey, noMark, plain,
		/* [5] */
		"1k", noIcon, noKey, noMark, plain,
		/* [6] */
		".5k (Slow)", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (10006) {
	10006,
	textMenuProc,
	allEnabled,
	enabled,
	"firewallType",
	{	/* array: 4 elements */
		/* [1] */
		"None", noIcon, noKey, noMark, plain,
		/* [2] */
		"SOCKS 5", noIcon, noKey, noMark, plain,
		/* [3] */
		"SOCKS 4A", noIcon, noKey, noMark, plain,
		/* [4] */
		"SOCKS 4", noIcon, noKey, noMark, plain
	}
};

resource 'SICN' (128) {
	{	/* array: 1 elements */
		/* [1] */
		$"07E0 0FF0 0FF0 7FFE FFFF FFFF 7FFE 1DB8"
		$"1FF8 1FF8 0FF0 0FF8 3FFE FFFF FFFF FFFF"
	}
};

data 'Tabl' (256, "Mac -> ISO", locked, preload) {
	$"0001 0203 0405 0607 0809 0A0B 0C0D 0E0F"            /* .........∆¬..... */
	$"1011 1213 1415 1617 1819 1A1B 1C1D 1E1F"            /* ................ */
	$"2021 2223 2425 2627 2829 2A2B 2C2D 2E2F"            /*  !"#$%&'()*+,-./ */
	$"3031 3233 3435 3637 3839 3A3B 3C3D 3E3F"            /* 0123456789:;<=>? */
	$"4041 4243 4445 4647 4849 4A4B 4C4D 4E4F"            /* @ABCDEFGHIJKLMNO */
	$"5051 5253 5455 5657 5859 5A5B 5C5D 5E5F"            /* PQRSTUVWXYZ[\]^_ */
	$"6061 6263 6465 6667 6869 6A6B 6C6D 6E6F"            /* `abcdefghijklmno */
	$"7071 7273 7475 7677 7879 7A7B 7C7D 7E7F"            /* pqrstuvwxyz{|}~. */
	$"C4C5 C7C9 D1D6 DCE1 E0E2 E4E3 E5E7 E9E8"            /* ƒ≈«…—÷‹·‡‚‰„ÂÁÈË */
	$"EAEB EDEC EEEF F1F3 F2F4 F6F5 FAF9 FBFC"            /* ÍÎÌÏÓÔÒÛÚÙˆı˙˘˚¸ */
	$"DDB0 A2A3 A780 B6DF AEA9 81B4 A882 C6D8"            /* ›∞¢£ßÄ∂ﬂÆ©Å¥®Ç∆ÿ */
	$"83B1 BE84 A5B5 8F85 BDBC 86AA BA87 E6F8"            /* É±æÑ•µèÖΩºÜ™∫áÊ¯ */
	$"BFA1 AC88 9F89 90AB BB8A A0C0 C3D5 91A6"            /* ø°¨àüâê´ªä†¿√’ë¶ */
	$"AD8B B3B2 8CB9 F7D7 FF8D 8EA4 D0F0 DEFE"            /* ≠ã≥≤åπ˜◊ˇçé§–ﬁ˛ */
	$"FDB7 9293 94C2 CAC1 CBC8 CDCE CFCC D3D4"            /* ˝∑íìî¬ ¡À»ÕŒœÃ”‘ */
	$"95D2 DADB D99E 9697 AF98 999A B89B 9C9D"            /* ï“⁄€ŸûñóØòôö∏õúù */
};

data 'Tabl' (257, "ISO -> Mac", locked, preload) {
	$"0001 0203 0405 0607 0809 0D0B 0C0D 0E0F"            /* .........∆...... */
	$"1011 1213 1415 1617 1819 1A1B 1C1D 1E1F"            /* ................ */
	$"2021 2223 2425 2627 2829 2A2B 2C2D 2E2F"            /*  !"#$%&'()*+,-./ */
	$"3031 3233 3435 3637 3839 3A3B 3C3D 3E3F"            /* 0123456789:;<=>? */
	$"4041 4243 4445 4647 4849 4A4B 4C4D 4E4F"            /* @ABCDEFGHIJKLMNO */
	$"5051 5253 5455 5657 5859 5A5B 5C5D 5E5F"            /* PQRSTUVWXYZ[\]^_ */
	$"6061 6263 6465 6667 6869 6A6B 6C6D 6E6F"            /* `abcdefghijklmno */
	$"7071 7273 7475 7677 7879 7A7B 7C7D 7E7F"            /* pqrstuvwxyz{|}~. */
	$"A5AA ADB0 B3B7 BABD C3C5 C9D1 D4D9 DAB6"            /* •™≠∞≥∑∫Ω√≈…—‘Ÿ⁄∂ */
	$"C6CE E2E3 E4F0 F6F7 F9FA FBFD FEFF F5C4"            /* ∆Œ‚„‰ˆ˜˘˙˚˝˛ˇıƒ */
	$"CAC1 A2A3 DBB4 CFA4 ACA9 BBC7 C2D0 A8F8"            /*  ¡¢£€¥œ§¨©ª«¬–®¯ */
	$"A1B1 D3D2 ABB5 A6E1 FCD5 BCC8 B9B8 B2C0"            /* °±”“´µ¶·¸’º»π∏≤¿ */
	$"CBE7 E5CC 8081 AE82 E983 E6E8 EDEA EBEC"            /* ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ */
	$"DC84 F1EE EFCD 85D7 AFF4 F2F3 86A0 DEA7"            /* ‹ÑÒÓÔÕÖ◊ØÙÚÛÜ†ﬁß */
	$"8887 898B 8A8C BE8D 8F8E 9091 9392 9495"            /* àáâãäåæçèéêëìíîï */
	$"DD96 9897 999B 9AD6 BF9D 9C9E 9FE0 DFD8"            /* ›ñòóôõö÷øùúûü‡ﬂÿ */
};

resource 'aete' (0) {
	0x0,
	0x0,
	english,
	roman,
	{	/* array Suites: 1 elements */
		/* [1] */
		"Required Apple Events",
		"",
		'aevt',
		0,
		0,
		{	/* array Events: 0 elements */
		},
		{	/* array Classes: 0 elements */
		},
		{	/* array ComparisonOps: 0 elements */
		},
		{	/* array Enumerations: 0 elements */
		}
	}
};

resource 'alrx' (130) {
	versionZero {
		13,
		0,
		kUseThemeWindow,
		""
	}
};

resource 'alrx' (136) {
	versionZero {
		13,
		0,
		kUseThemeWindow,
		""
	}
};

data 'cicn' (128, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....Ä........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ˇˇˇˇˇˇˇˇˇˇˇˇˇˇ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 8007"            /* ˇˇˇˇˇˇˇˇˇˇˇˇÄ.Ä. */
	$"BFF7 A027 9047 8887 8707 8207 8007 8007"            /* ø˜†'êGàáá.Ç.Ä.Ä. */
	$"FFFF 0000 0000 0000 0008 0000 FFFF FFFF"            /* ˇˇ..........ˇˇˇˇ */
	$"FFFF 0001 CCCC CCCC CCCC 0002 AAAA AAAA"            /* ˇˇ..ÃÃÃÃÃÃ..™™™™ */
	$"AAAA 0003 CCCC CCCC FFFF 0004 9999 9999"            /* ™™..ÃÃÃÃˇˇ..ôôôô */
	$"FFFF 0005 6666 6666 CCCC 0006 BBBB BBBB"            /* ˇˇ..ffffÃÃ..ªªªª */
	$"BBBB 0007 8888 8888 8888 000F 0000 0000"            /* ªª..àààààà...... */
	$"0000 FFFF FFFF FFFF FF00 F000 0000 0000"            /* ..ˇˇˇˇˇˇˇ...... */
	$"1F00 F011 1111 1111 2F00 F0FF FFFF FFFF"            /* ......./.ˇˇˇˇˇ */
	$"2F00 F0F3 3333 34F7 2F00 F01F 4444 5F76"            /* /.Û334˜/..DD_v */
	$"2F00 F011 F445 F761 2F00 F011 1F5F 7611"            /* /..ÙE˜a/..._v. */
	$"2F00 F011 11F7 6111 2F00 F011 1116 1111"            /* /...˜a./...... */
	$"2F00 F122 2222 2222 2F00 FFFF FFFF FFFF"            /* /.Ò"""""/.ˇˇˇˇˇˇ */
	$"FF00"                                               /* ˇ. */
};

resource 'cicn' (129, locked, preload) {
	4,
	{0, 0, 12, 14},
	2,
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF",
	$"FFFF 8007 8007 9557 AAA7 9547 8A87 8507"
	$"8207 8007 8007 FFFF",
	{	/* array ColorSpec: 4 elements */
		/* [1] */
		52428, 52428, 52428,
		/* [2] */
		39321, 39321, 65535,
		/* [3] */
		34952, 34952, 34952,
		/* [4] */
		35939, 35939, 35939
	},
	$"FFFF FFF0 C000 0030 C000 0030 CAAA AA30"
	$"C955 5630 C255 5830 C095 6030 C025 8030"
	$"C00A 0030 C000 0030 C000 0030 FFFF FFF0"
};

data 'cicn' (130, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....Ä........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ˇˇˇˇˇˇˇˇˇˇˇˇˇˇ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 8007"            /* ˇˇˇˇˇˇˇˇˇˇˇˇÄ.Ä. */
	$"BFF7 BFE7 9FC7 8F87 8707 8207 8007 8007"            /* ø˜øÁü«èáá.Ç.Ä.Ä. */
	$"FFFF 0000 0000 0000 0008 0000 FFFF FFFF"            /* ˇˇ..........ˇˇˇˇ */
	$"FFFF 0001 CCCC CCCC CCCC 0002 AAAA AAAA"            /* ˇˇ..ÃÃÃÃÃÃ..™™™™ */
	$"AAAA 0003 CCCC CCCC FFFF 0004 9999 9999"            /* ™™..ÃÃÃÃˇˇ..ôôôô */
	$"FFFF 0005 6666 6666 CCCC 0006 BBBB BBBB"            /* ˇˇ..ffffÃÃ..ªªªª */
	$"BBBB 0007 8888 8888 8888 000F 0000 0000"            /* ªª..àààààà...... */
	$"0000 FFFF FFFF FFFF FF00 F222 2222 2222"            /* ..ˇˇˇˇˇˇˇ.Ú""""" */
	$"1F00 F211 1111 1111 0F00 F2FF FFFF FFFF"            /* ..Ú.......Úˇˇˇˇˇ */
	$"0F00 F2F5 5555 54F7 0F00 F21F 4444 3F76"            /* ..ÚıUUT˜..Ú.DD?v */
	$"0F00 F211 F443 F761 0F00 F211 1F3F 7611"            /* ..Ú.ÙC˜a..Ú..?v. */
	$"0F00 F211 11F7 6111 0F00 F211 1116 1111"            /* ..Ú..˜a...Ú..... */
	$"0F00 F100 0000 0000 0F00 FFFF FFFF FFFF"            /* ..Ò.......ˇˇˇˇˇˇ */
	$"FF00"                                               /* ˇ. */
};

data 'cicn' (136, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....Ä........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ˇˇˇˇˇˇˇˇˇˇˇˇˇˇ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 800F"            /* ˇˇˇˇˇˇˇˇˇˇˇˇÄ.Ä. */
	$"884F 9E6F B87F 9E6F 884F 800F 800F BFFF"            /* àOûo∏.ûoàOÄ.Ä.øˇ */
	$"FFFF 0000 0000 0000 0005 0000 FFFF FFFF"            /* ˇˇ..........ˇˇˇˇ */
	$"FFFF 0001 5555 5555 5555 0002 DDDD DDDD"            /* ˇˇ..UUUUUU..›››› */
	$"DDDD 0003 EEEE 0000 0000 0004 8888 8888"            /* ››..ÓÓ......àààà */
	$"8888 000F 0000 0000 0000 FFFF FFFF FFFF"            /* àà........ˇˇˇˇˇˇ */
	$"FF00 F000 0000 0000 2F00 F022 2222 2222"            /* ˇ....../.""""" */
	$"1F00 F022 4222 2422 1F00 F024 F332 2F12"            /* .."B"$"..$Û2/. */
	$"1F00 F0FF F222 2FFF 1F00 F024 F332 2F12"            /* ..ˇÚ"/ˇ..$Û2/. */
	$"1F00 F022 4222 2422 1F00 F022 2222 2222"            /* .."B"$"..""""" */
	$"1F00 F022 2222 2222 1F00 F211 1111 1111"            /* .."""""..Ú..... */
	$"1F00 FFFF FFFF FFFF FF00"                           /* ..ˇˇˇˇˇˇˇ. */
};

resource 'cicn' (137, locked, preload) {
	8,
	{0, 0, 12, 14},
	4,
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF",
	$"FFFF 8007 8007 8847 9627 A857 9627 8847"
	$"8007 8007 8007 FFFF",
	{	/* array ColorSpec: 6 elements */
		/* [1] */
		34952, 34952, 34952,
		/* [2] */
		43690, 43690, 43690,
		/* [3] */
		35939, 35939, 35939,
		/* [4] */
		52851, 52851, 52851,
		/* [5] */
		60947, 15238, 15238,
		/* [6] */
		16383, 16383, 16383
	},
	$"2222 2222 2222 2200 2333 3333 3333 3200"
	$"2333 3333 3333 3200 2333 1333 3133 3200"
	$"2331 5443 3503 3200 2355 5333 3555 3200"
	$"2331 5443 3503 3200 2333 1333 3133 3200"
	$"2333 3333 3333 3200 2333 3333 3333 3200"
	$"2333 3333 3333 3200 2222 2222 2222 2200"
};

data 'cicn' (138, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....Ä........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ˇˇˇˇˇˇˇˇˇˇˇˇˇˇ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇ */
	$"FFFF F7BF E19F C78F E19F F7BF FFFF FFFF"            /* ˇˇ˜ø·ü«è·ü˜øˇˇˇˇ */
	$"FFFF 0000 0000 0000 0005 0000 FFFF FFFF"            /* ˇˇ..........ˇˇˇˇ */
	$"FFFF 0001 5555 5555 5555 0002 DDDD DDDD"            /* ˇˇ..UUUUUU..›››› */
	$"DDDD 0003 EEEE 0000 0000 0004 8888 8888"            /* ››..ÓÓ......àààà */
	$"8888 000F 0000 0000 0000 FFFF FFFF FFFF"            /* àà........ˇˇˇˇˇˇ */
	$"FF00 F111 1111 1111 2F00 F122 2222 2222"            /* ˇ.Ò...../.Ò""""" */
	$"0F00 F122 2222 2222 0F00 F122 4222 2422"            /* ..Ò"""""..Ò"B"$" */
	$"0F00 F124 F332 2F12 0F00 F1FF F222 2FFF"            /* ..Ò$Û2/...ÒˇÚ"/ˇ */
	$"0F00 F124 F332 2F12 0F00 F122 4222 2422"            /* ..Ò$Û2/...Ò"B"$" */
	$"0F00 F122 2222 2222 0F00 F200 0000 0000"            /* ..Ò"""""..Ú..... */
	$"0F00 FFFF FFFF FFFF FF00"                           /* ..ˇˇˇˇˇˇˇ. */
};

resource 'dftb' (500) {
	versionZero {
		{	/* array FontStyle: 4 elements */
			/* [1] */
			skipItem {

			}			,
			/* [2] */
			skipItem {

			}			,
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (502) {
	versionZero {
		{	/* array FontStyle: 6 elements */
			/* [1] */
			skipItem {

			}			,
			/* [2] */
			skipItem {

			}			,
			/* [3] */
			skipItem {

			}			,
			/* [4] */
			skipItem {

			}			,
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1001) {
	versionZero {
		{	/* array FontStyle: 31 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [9] */
			skipItem {

			},
			/* [10] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [11] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [12] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [13] */
			skipItem {

			},
			/* [14] */
			skipItem {

			},
			/* [15] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [16] */
			skipItem {

			},
			/* [17] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [18] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [19] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [20] */
			skipItem {

			},
			/* [21] */
			skipItem {

			},
			/* [22] */
			skipItem {

			},
			/* [23] */
			skipItem {

			},
			/* [24] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [25] */
			skipItem {

			},
			/* [26] */
			skipItem {

			},
			/* [27] */
			skipItem {

			},
			/* [28] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [29] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [30] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1002) {
	versionZero {
		{	/* array FontStyle: 17 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [9] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [10] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [11] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [12] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [13] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [14] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [15] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [16] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [17] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1005) {
	versionZero {
		{	/* array FontStyle: 12 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			skipItem {

			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [9] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [10] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [11] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [12] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1006) {
	versionZero {
		{	/* array FontStyle: 4 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1007) {
	versionZero {
		{	/* array FontStyle: 27 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [9] */
			skipItem {

			},
			/* [10] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [11] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [12] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [13] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [14] */
			skipItem {

			},
			/* [15] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [16] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [17] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [18] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [19] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [20] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [21] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [22] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [23] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [24] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [25] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [26] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [27] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1009) {
	versionZero {
		{	/* array FontStyle: 6 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			skipItem {

			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1010) {
	versionZero {
		{	/* array FontStyle: 9 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [9] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1011) {
	versionZero {
		{	/* array FontStyle: 8 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			skipItem {

			},
			/* [5] */
			skipItem {

			},
			/* [6] */
			skipItem {

			},
			/* [7] */
			skipItem {

			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1012) {
	versionZero {
		{	/* array FontStyle: 10 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (1013) {
	versionZero {
		{	/* array FontStyle: 3 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (10001) {
	versionZero {
		{	/* array FontStyle: 4 elements */
			/* [1] */
			skipItem {

			}			,
			/* [2] */
			skipItem {

			}			,
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (10002) {
	versionZero {
		{	/* array FontStyle: 4 elements */
			/* [1] */
			skipItem {

			}			,
			/* [2] */
			skipItem {

			}			,
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (10004) {
	versionZero {
		{	/* array FontStyle: 6 elements */
			/* [1] */
			skipItem {

			}			,
			/* [2] */
			skipItem {

			}			,
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (10011) {
	versionZero {
		{	/* array FontStyle: 31 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [5] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [6] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [7] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [9] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [10] */
			skipItem {

			},
			/* [11] */
			skipItem {

			},
			/* [12] */
			skipItem {

			},
			/* [13] */
			skipItem {

			},
			/* [14] */
			skipItem {

			},
			/* [15] */
			skipItem {

			},
			/* [16] */
			skipItem {

			},
			/* [17] */
			skipItem {

			},
			/* [18] */
			skipItem {

			},
			/* [19] */
			skipItem {

			},
			/* [20] */
			skipItem {

			},
			/* [21] */
			skipItem {

			},
			/* [22] */
			skipItem {

			},
			/* [23] */
			skipItem {

			},
			/* [24] */
			skipItem {

			},
			/* [25] */
			skipItem {

			},
			/* [26] */
			skipItem {

			},
			/* [27] */
			skipItem {

			},
			/* [28] */
			skipItem {

			},
			/* [29] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [30] */
			skipItem {

			},
			/* [31] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dftb' (10012) {
	versionZero {
		{	/* array FontStyle: 8 elements */
			/* [1] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [2] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [3] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			},
			/* [4] */
			skipItem {

			},
			/* [5] */
			skipItem {

			},
			/* [6] */
			skipItem {

			},
			/* [7] */
			skipItem {

			},
			/* [8] */
			dataItem {
				1,
				-2,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				""
			}
		}
	}
};

resource 'dlgx' (-3010) {
	versionZero {
		15
	}
};

resource 'dlgx' (-3000) {
	versionZero {
		15
	}
};

resource 'dlgx' (141) {
	versionZero {
		15
	}
};

resource 'dlgx' (500) {
	versionZero {
		15
	}
};

resource 'dlgx' (501) {
	versionZero {
		15
	}
};

resource 'dlgx' (502) {
	versionZero {
		15
	}
};

resource 'dlgx' (1004) {
	versionZero {
		15
	}
};

resource 'dlgx' (10001) {
	versionZero {
		15
	}
};

resource 'dlgx' (10002) {
	versionZero {
		15
	}
};

resource 'icl4' (129) {
	$"0FFF E000 0000 0000 0000 0000 0000 0000"
	$"FDDD DE00 0000 0000 000F FFFF FFFF FFF0"
	$"DEDD DF00 0000 0000 000F 3333 3222 22F0"
	$"EEED DDF0 0000 0000 000F 3333 3222 22F0"
	$"EEEE DDF0 0000 0000 000F 3333 3222 22F0"
	$"CCCD DDF0 0000 0000 000F 3333 3222 22F0"
	$"CCDD DDF0 0000 0000 000F 3333 3222 22F0"
	$"DDDD DDDF FFFE 0000 000F 1111 1888 88F0"
	$"DDDE DDDD DDDD FE00 000F 1111 1888 88F0"
	$"EEED DDDD DDDD DDE0 000F 1111 1888 88F0"
	$"EEEE EDDD DDDD DDDF 000F 1111 1888 88F0"
	$"EEEE EEDD DDDD DDCF 000F 1111 1888 88F0"
	$"EEEE EEEE DDDD DCE0 000F 6666 6444 44F0"
	$"FFFF FFFE DDDD FE00 000F 6666 6444 44F0"
	$"FFAB BAFE DDFE 0000 000F 6666 6444 44F0"
	$"F33C BAFE DF00 0000 000F 6666 6444 44F0"
	$"FA3B BFFE DF00 0000 000F 6666 6444 44F0"
	$"FFAA FFFD DF00 0000 000F FFFF FFFF FFF0"
	$"FFFF FFED DE00 0000 0000 0000 0000 0000"
	$"FFFF FFDD F000 0000 0000 0000 0000 0000"
	$"FFFF FEDD E000 0000 0000 0000 0000 0000"
	$"FFFF FDDF 0000 0000 0000 0000 0000 0000"
	$"FFFF ADDF 0000 0000 0000 0000 0000 0000"
	$"FFFF ADDF 0000 0000 0000 0000 0000 0000"
	$"FFFF EDDD FFE0 0000 0000 0000 0000 0000"
	$"FFFF ADDD DEEF FE00 0000 0000 0000 0000"
	$"FFFF FDDD DEEE AEE0 0000 0000 0000 0000"
	$"FFFF FFFE DEEE AFFF 0000 0000 0000 0000"
	$"FFFF FFFF DEEE AFFE 0000 0000 0000 0000"
	$"FFFF FFFA DEEE FFFE 0000 0000 0000 0000"
	$"FFFF FFFD DEEA AFFE 0000 0000 0000 0000"
	$"FFFF FFDD EEEE FFFE"
};

resource 'icl4' (130) {
	$"0FFF E000 0000 0000 0000 0000 0000 0000"
	$"FDDD DE00 0000 0000 0000 0000 0000 0000"
	$"DEDD DF00 0000 0000 0000 0000 0000 0000"
	$"EEED DDF0 0000 0000 0000 0000 0000 0000"
	$"EEEE DDF0 0000 0000 0000 0000 0000 0000"
	$"CCCD DDF0 0000 0000 0000 0000 0000 0000"
	$"CCDD DDF0 0000 0000 0000 0000 0000 0000"
	$"DDDD DDDF FFFE 0000 0000 0000 0000 0000"
	$"DDDE DDDD DDDD FE00 0000 0000 0000 0000"
	$"EEED DDDD DDDD DDE0 0000 0000 0000 0000"
	$"EEEE EDDD DDDD DDDF 0000 0000 0000 0000"
	$"EEEE EEDD DDDD DDCF 0000 0000 0000 0000"
	$"EEEE EEEE DDDD DCE0 0000 0000 0000 0000"
	$"FFFF FFFE DDDD FE00 0FFF 0000 0000 0000"
	$"FFAB BAFE DDFE 0000 FCCC F000 0000 0000"
	$"F33C BAFE DF00 000F CCFC CF00 0000 0000"
	$"FA3B BFFE DF00 00FC CCCF CF00 0000 0000"
	$"FFAA FFFD DF00 00FC FCCC CF00 0000 0000"
	$"FFFF FFED DE00 00FC CFCC F000 0000 0000"
	$"FFFF FFDD F000 000F CCCF 00F0 0000 0000"
	$"FFFF FEDD E000 0000 FFF0 000F 0F00 0000"
	$"FFFF FDDF 0000 0000 0000 F000 FF00 0000"
	$"FFFF ADDF 0000 0000 0000 0F0F FFF0 0000"
	$"FFFF ADDF 0000 0000 0000 00FF FFF0 0000"
	$"FFFF EDDD FFE0 0000 0000 0FFF FFF0 0000"
	$"FFFF ADDD DEEF FE00 0000 000F FFFF 0000"
	$"FFFF FDDD DEEE AEE0 0000 0000 000F F000"
	$"FFFF FFFE DEEE AFFF 0000 0000 0000 F000"
	$"FFFF FFFF DEEE AFFE 0000 0000 0000 F000"
	$"FFFF FFFA DEEE FFFE 0000 0000 FFFF 0000"
	$"FFFF FFFD DEEA AFFE 0000 00FF 0000 0000"
	$"FFFF FFDD EEEE FFFE FFFF FF"
};

resource 'icl8' (129) {
	$"00FF FFFF FB00 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FF56 81FA F9FB 0000 0000 0000 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FF00"
	$"81FB 8181 FAFF 0000 0000 0000 0000 0000"
	$"0000 00FF D8D8 D8D8 D816 1616 1616 FF00"
	$"FBFC FB81 81FA FF00 0000 0000 0000 0000"
	$"0000 00FF D8D8 D8D8 D816 1616 1616 FF00"
	$"FCFB FCFB 81FA FF00 0000 0000 0000 0000"
	$"0000 00FF D8D8 D8D8 D816 1616 1616 FF00"
	$"F7F8 F856 F9FA FF00 0000 0000 0000 0000"
	$"0000 00FF D8D8 D8D8 D816 1616 1616 FF00"
	$"F7F8 5656 56FA FF00 0000 0000 0000 0000"
	$"0000 00FF D8D8 D8D8 D816 1616 1616 FF00"
	$"8181 81FA 81FA F9FF FFFF FFFB 0000 0000"
	$"0000 00FF 0505 0505 05E3 E3E3 E3E3 FF00"
	$"8181 81FB 8181 FA81 FAFA F956 FFFB 0000"
	$"0000 00FF 0505 0505 05E3 E3E3 E3E3 FF00"
	$"FBFB FB81 8181 81FA FAF9 F9F9 F956 FB00"
	$"0000 00FF 0505 0505 05E3 E3E3 E3E3 FF00"
	$"FBFC FBFB FB81 8181 FAFA F9F9 5656 56FF"
	$"0000 00FF 0505 0505 05E3 E3E3 E3E3 FF00"
	$"FCFC FCFC FBFB 8181 81FA FAF9 56F9 F8FF"
	$"0000 00FF 0505 0505 05E3 E3E3 E3E3 FF00"
	$"ACAC FCAC ACFC FCFB FAFA FAF9 FAF7 FB00"
	$"0000 00FF ECEC ECEC EC69 6969 6969 FF00"
	$"FFFF FFFF FFFF FFAC F9FA FAF9 FFFB 0000"
	$"0000 00FF ECEC ECEC EC69 6969 6969 FF00"
	$"F4E0 DD5E 64DD FFAC F9F9 FFFB 0000 0000"
	$"0000 00FF ECEC ECEC EC69 6969 6969 FF00"
	$"FFDB 402D 34DC FFFC F9FF 0000 0000 0000"
	$"0000 00FF ECEC ECEC EC69 6969 6969 FF00"
	$"F48E 463A 5EE0 FFFB FAFF 0000 0000 0000"
	$"0000 00FF ECEC ECEC EC69 6969 6969 FF00"
	$"FFFF DEDD E0FF F4FA FAFF 0000 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FF00"
	$"FFFF FFFF FFFF ACF9 F9FB 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF 81F9 FF00 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFAC 5656 FB00 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFA 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FD56 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FD56 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF AC56 F9FA FFFF FB00 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FD56 F9F9 81FB FCFF FFFB 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF EAFA FAFA FAFB FCAC FDFB FB00"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FFFC 81FB FCAC FDFF FFFF"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FFE0 81FB FCAC FDFF FFFC"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FFFD 81FB FCAC FEFE FFFC"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FEFA 81FB FCFD FDFF FFFC"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFE 81FA FBFB ACAC FEFF FFAC"
};

resource 'icl8' (130) {
	$"00FF FFFF FB00 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FF56 81FA F9FB 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"81FB 8181 FAFF 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FBFC FB81 81FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FCFB FCFB 81FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"F7F8 F856 F9FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"F7F8 5656 56FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"8181 81FA 81FA F9FF FFFF FFFB 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"8181 81FB 8181 FA81 FAFA F956 FFFB 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FBFB FB81 8181 81FA FAF9 F9F9 F956 FB00"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FBFC FBFB FB81 8181 FAFA F9F9 5656 56FF"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FCFC FCFC FBFB 8181 81FA FAF9 56F9 F8FF"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"ACAC FCAC ACFC FCFB FAFA FAF9 FAF7 FB00"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FFAC F9FA FAF9 FFFB 0000"
	$"00FF FFFF 0000 0000 0000 0000 0000 0000"
	$"F4E0 DD5E 64DD FFAC F9F9 FFFB 0000 0000"
	$"FF2B 2B2B FF00 0000 0000 0000 0000 0000"
	$"FFDB 402D 34DC FFFC F9FF 0000 0000 00FF"
	$"2B2B FF2B 2BFF 0000 0000 0000 0000 0000"
	$"F48E 463A 5EE0 FFFB FAFF 0000 0000 FF2B"
	$"2B2B 2BFF 2BFF 0000 0000 0000 0000 0000"
	$"FFFF DEDD E0FF F4FA FAFF 0000 0000 FF2B"
	$"FF2B 2B2B 2BFF 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF ACF9 F9FB 0000 0000 FF2B"
	$"2BFF 2B2B FF00 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF 81F9 FF00 0000 0000 00FF"
	$"2B2B 2BFF 0000 FF00 0000 0000 0000 0000"
	$"FFFF FFFF FFAC 5656 FB00 0000 0000 0000"
	$"FFFF FF00 0000 00FF 00FF 0000 0000 0000"
	$"FFFF FFFF FFFA 56FF 0000 0000 0000 0000"
	$"0000 0000 FF00 0000 FFFF 0000 0000 0000"
	$"FFFF FFFF FD56 56FF 0000 0000 0000 0000"
	$"0000 0000 00FF 00FF FFFF FF00 0000 0000"
	$"FFFF FFFF FD56 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 FFFF FFFF FF00 0000 0000"
	$"FFFF FFFF AC56 F9FA FFFF FB00 0000 0000"
	$"0000 0000 00FF FFFF FFFF FF00 0000 0000"
	$"FFFF FFFF FD56 F9F9 81FB FCFF FFFB 0000"
	$"0000 0000 0000 00FF FFFF FFFF 0000 0000"
	$"FFFF FFFF EAFA FAFA FAFB FCAC FDFB FB00"
	$"0000 0000 0000 0000 0000 00FF FF00 0000"
	$"FFFF FFFF FFFF FFFC 81FB FCAC FDFF FFFF"
	$"0000 0000 0000 0000 0000 0000 FF00 0000"
	$"FFFF FFFF FFFF FFE0 81FB FCAC FDFF FFFC"
	$"0000 0000 0000 0000 0000 0000 FF00 0000"
	$"FFFF FFFF FFFF FFFD 81FB FCAC FEFE FFFC"
	$"0000 0000 0000 0000 FFFF FFFF 0000 0000"
	$"FFFF FFFF FFFF FEFA 81FB FCFD FDFF FFFC"
	$"0000 0000 0000 FFFF 0000 0000 0000 0000"
	$"FFFF FFFF FFFE 81FA FBFB ACAC FEFF FFAC"
	$"FFFF FFFF FFFF"
};

resource 'ics#' (129) {
	{	/* array: 2 elements */
		/* [1] */
		$"E07F F079 F079 FC7F FF4F FF4F FF7F BC7F"
		$"F87F F800 F800 F000 FE00 FF00 FF00 FF",
		/* [2] */
		$"E000 F078 F07C FC7E FF7E FF7E FF7E FC7E"
		$"F87E F800 F800 F000 FE00 FF00 FF00 FF"
	}
};

resource 'ics#' (130) {
	{	/* array: 2 elements */
		/* [1] */
		$"E000 F000 F000 FC00 FF00 FF00 FF00 BC14"
		$"F814 F83E F83E F01C FE08 FF08 FF08 FFF0",
		/* [2] */
		$"E000 F000 F000 FC00 FF00 FF00 FF00 FC14"
		$"F814 F83E F83E F01C FE08 FF08 FF08 FFF0"
	}
};

resource 'ics4' (129) {
	$"FFE0 0000 0FFF FFFF EEFF 0000 0F33 322F"
	$"EEDF 0000 0F33 322F CDDF FF00 0F33 888F"
	$"EEDD DDFE 0F11 888F EEED DDDF 0F11 888F"
	$"FFFF DDFE 0F66 644F FAAF FF00 0F66 644F"
	$"FAFF F000 0FFF FFFF FFFD F000 0000 0000"
	$"FFFF E000 0000 0000 FFAF 0000 0000 0000"
	$"FFAD FFF0 0000 0000 FFFF EEFF 0000 0000"
	$"FFFF EEFF 0000 0000 FFFF EAFF"
};

resource 'ics4' (130) {
	$"FFE0 0000 0000 0000 EEFF 0000 0000 0000"
	$"EEDF 0000 0000 0000 CDDF FF00 0000 0000"
	$"EEDD DDFE 0000 0000 EEED DDDF 0000 0000"
	$"FFFF DDFE 0000 0000 FAAF FF00 000F 0F00"
	$"FAFF F000 000F 0F00 FFFD F000 00FF FFF0"
	$"FFFF E000 00FF FFF0 FFAF 0000 000F FF00"
	$"FFAD FFF0 0000 F000 FFFF EEFF 0000 F000"
	$"FFFF EEFF 0000 F000 FFFF EAFF FFFF"
};

resource 'ics8' (129) {
	$"FFFF FB00 0000 0000 00FF FFFF FFFF FFFF"
	$"FCFB FFFF 0000 0000 00FF D8D8 D816 16FF"
	$"FCFC FAFF 0000 0000 00FF D8D8 D816 16FF"
	$"F8FA FAFF FFFF 0000 00FF D8D8 E3E3 E3FF"
	$"FBFB 81FA FAF9 FFFB 00FF 0505 E3E3 E3FF"
	$"FCFC FB81 FAFA F9FF 00FF 0505 E3E3 E3FF"
	$"FFFF FFFF FAFA FFFB 00FF ECEC EC69 69FF"
	$"FFDD DDFF FFFF 0000 00FF ECEC EC69 69FF"
	$"FFDE FFFF FF00 0000 00FF FFFF FFFF FFFF"
	$"FFFF FFF9 FF00 0000 0000 0000 0000 0000"
	$"FFFF FFFF FB00 0000 0000 0000 0000 0000"
	$"FFFF FDFF 0000 0000 0000 0000 0000 0000"
	$"FFFF FDFA FFFF FF00 0000 0000 0000 0000"
	$"FFFF FFFF FBFC FFFF 0000 0000 0000 0000"
	$"FFFF FFFF FBFC FFFF 0000 0000 0000 0000"
	$"FFFF FFFE FBFD FFFF"
};

resource 'ics8' (130) {
	$"FFFF FB00 0000 0000 0000 0000 0000 0000"
	$"FCFB FFFF 0000 0000 0000 0000 0000 0000"
	$"FCFC FAFF 0000 0000 0000 0000 0000 0000"
	$"F8FA FAFF FFFF 0000 0000 0000 0000 0000"
	$"FBFB 81FA FAF9 FFFB 0000 0000 0000 0000"
	$"FCFC FB81 FAFA F9FF 0000 0000 0000 0000"
	$"FFFF FFFF FAFA FFFB 0000 0000 0000 0000"
	$"FFDD DDFF FFFF 0000 0000 00FF 00FF 0000"
	$"FFDE FFFF FF00 0000 0000 00FF 00FF 0000"
	$"FFFF FFF9 FF00 0000 0000 FFFF FFFF FF00"
	$"FFFF FFFF FB00 0000 0000 FFFF FFFF FF00"
	$"FFFF FDFF 0000 0000 0000 00FF FFFF 0000"
	$"FFFF FDFA FFFF FF00 0000 0000 FF00 0000"
	$"FFFF FFFF FBFC FFFF 0000 0000 FF00 0000"
	$"FFFF FFFF FBFC FFFF 0000 0000 FF00 0000"
	$"FFFF FFFE FBFD FFFF FFFF FFFF"
};

resource 'ldes' (500) {
	versionZero {
		0,
		1,
		0,
		0,
		hasVertScroll,
		noHorizScroll,
		0,
		noGrowSpace
	}
};

resource 'ldes' (10124) {
	versionZero {
		0,
		1,
		0,
		0,
		hasVertScroll,
		noHorizScroll,
		0,
		noGrowSpace
	}
};
