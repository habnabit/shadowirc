#include <Carbon/Carbon.r>

resource 'ALRT' (129, "TCPERR") {
	{56, 74, 158, 465},
	129,
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
	$"600A 0000 4344 4546 0384 0000 4EFA 0036"            /* `Â..CDEF.„..Nú.6 */
	$"41FA FFEE D1FC 0000 851A 2008 A055 C18C"            /* AúÿîÑü..…. . UÁŒ */
	$"4E75 4E75 200C 49FA 0006 2854 4E75 0000"            /* NuNu .Iú..(TNu.. */
	$"0000 41FA FFFA 208C 4E75 8753 6574 5570"            /* ..Aúÿú ŒNu‡SetUp */
	$"4134 0000 4E56 0000 48E7 1C00 3A2E 000C"            /* A4..NV..Hç..:... */
	$"7600 4EBA FFBC 2800 4EBA FFD8 396E 0012"            /* v.Nºÿ¼(.NºÿØ9n.. */
	$"8020 296E 000E 8022 3945 8026 296E 0008"            /* € )n..€"9E€&)n.. */
	$"8028 3005 48C0 0C80 0000 000B 6240 303B"            /* €(0.HÀ.€....b@0; */
	$"0206 4EFB 0002 002C 0024 0032 0018 001E"            /* ..Nû...,.$.2.... */
	$"0038 0038 0038 0038 0038 0032 0032 4EBA"            /* .8.8.8.8.8.2.2Nº */
	$"0038 601A 4EBA 010C 6014 4EBA 013A 2600"            /* .8`.Nº..`.Nº.:&. */
	$"600C 4EBA 0190 6006 4EBA 03B0 2608 2004"            /* `.Nº.`.Nº.°&. . */
	$"C18C 2F43 0020 4CDF 0038 4E5E 4E74 000C"            /* ÁŒ/C. Lß.8N^Nt.. */
	$"846D 6169 6E00 0000 4E56 FFFC 2F0A 4A2C"            /* „main...NVÿü/ÂJ, */
	$"8000 6600 0088 554F 2F2C 8022 486E FFFC"            /* €.f..ˆUO/,€"Hnÿü */
	$"AA44 101F 4AAE FFFC 6742 206E FFFC A029"            /* ªD..J®ÿügB nÿü ) */
	$"206E FFFC 2050 2468 0008 2052 2968 000A"            /*  nÿü P$h.. R)h.Â */
	$"8002 3968 000E 8006 2052 2968 001A 8008"            /* €.9h..€. R)h..€. */
	$"3968 001E 800C 2052 2968 0012 800E 3968"            /* 9h..€. R)h..€.9h */
	$"0016 8012 206E FFFC A02A 602A 426C 8002"            /* ..€. nÿü *`*Bl€. */
	$"426C 8004 426C 8006 426C 8008 426C 800A"            /* Bl€.Bl€.Bl€.Bl€Â */
	$"426C 800C 397C FFFF 800E 397C FFFF 8010"            /* Bl€.9|ÿÿ€.9|ÿÿ€. */
	$"397C FFFF 8012 197C 0001 8000 7006 A122"            /* 9|ÿÿ€..|..€.p.¡" */
	$"2448 200A 6716 2052 30BC FFFF 2052 317C"            /* $H Âg. R0¼ÿÿ R1| */
	$"FFFF 0002 2052 317C FFFF 0004 206C 8022"            /* ÿÿ.. R1|ÿÿ.. l€" */
	$"2050 214A 001C 245F 4E5E 4E75 9349 6E69"            /*  P!J..$_N^Nu“Ini */
	$"7469 616C 6973 654D 7943 6F6E 7472 6F6C"            /* tialiseMyControl */
	$"0000 4E56 0000 206C 8022 2050 4AA8 001C"            /* ..NV.. l€" PJ¨.. */
	$"670C 206C 8022 2050 2068 001C A023 4E5E"            /* g. l€" P h.. #N^ */
	$"4E75 9044 6973 706F 7365 4D79 436F 6E74"            /* NuDisposeMyCont */
	$"726F 6C00 0000 4E56 FFFC 2F03 7600 206C"            /* rol...NVÿü/.v. l */
	$"8022 2050 7000 1028 0011 0C80 0000 00FF"            /* €" Pp..(...€...ÿ */
	$"6C2A 3D6C 802A FFFE 202C 8028 7210 E2A0"            /* l*=l€*ÿş ,€(r.â  */
	$"3D40 FFFC 554F 2F2E FFFC 206C 8022 2050"            /* =@ÿüUO/.ÿü l€" P */
	$"4868 0008 A8AD 101F 6702 760B 2003 261F"            /* Hh..¨­..g.v. .&. */
	$"4E5E 4E75 8D54 6573 744D 7943 6F6E 7472"            /* N^NuTestMyContr */
	$"6F6C 0000 4E56 FFF2 2F03 206C 8022 2050"            /* ol..NVÿò/. l€" P */
	$"4A28 0010 6700 00A0 306C 802A 2948 8028"            /* J(..g.. 0l€*)H€( */
	$"206C 8022 2050 2968 0008 8014 2968 000C"            /*  l€" P)h..€.)h.. */
	$"8018 302C 8016 48C0 7215 D081 3940 801A"            /* €.0,€.HÀr.Ğ9@€. */
	$"302C 8014 48C0 322C 8014 48C1 362C 8018"            /* 0,€.HÀ2,€.HÁ6,€. */
	$"48C3 9681 E283 D680 3003 48C0 5D80 3940"            /* HÃ–âƒÖ€0.HÀ]€9@ */
	$"8014 48C0 720C D081 3940 8018 302C 801A"            /* €.HÀr.Ğ9@€.0,€. */
	$"48C0 5880 3940 801E 486E FFF2 A88B 3003"            /* HÀX€9@€.Hnÿò¨‹0. */
	$"48C0 322E FFF2 48C1 E281 D280 5381 3941"            /* HÀ2.ÿòHÁâÒ€S9A */
	$"801C 486E FFFA AA19 206C 8022 2050 2068"            /* €.Hnÿúª. l€" P h */
	$"0004 2F28 0018 487A 0024 42A7 42A7 ABCA"            /* ../(..Hz.$B§B§«Ê */
	$"486E FFFA AA14 261F 4E5E 4E75 8D44 7261"            /* Hnÿúª.&.N^NuDra */
	$"774D 7943 6F6E 7472 6F6C 0000 4E56 FFEC"            /* wMyControl..NVÿì */
	$"48E7 1820 7600 4EBA FD0C 2800 202C 8028"            /* Hç. v.Nºı.(. ,€( */
	$"674E 5180 5780 670C 0480 0000 00F4 6740"            /* gNQ€W€g..€...ôg@ */
	$"6000 0116 206C 8022 2050 4A28 0011 6708"            /* `... l€" PJ(..g. */
	$"486C 8002 AA14 6006 486C 800E AA14 2D6C"            /* Hl€.ª.`.Hl€.ª.-l */
	$"8014 FFF8 2D6C 8018 FFFC 486E FFF8 2F3C"            /* €.ÿø-l€.ÿüHnÿø/< */
	$"0001 0001 A8A9 486E FFF8 A8A1 6000 00DA"            /* ....¨©Hnÿø¨¡`..Ú */
	$"2D6C 8014 FFF8 2D6C 8018 FFFC 486C 8002"            /* -l€.ÿø-l€.ÿüHl€. */
	$"AA14 486E FFF8 A8A1 486C 800E AA14 486E"            /* ª.Hnÿø¨¡Hl€.ª.Hn */
	$"FFF8 2F3C 0001 0001 A8A9 486E FFF8 A8A1"            /* ÿø/<....¨©Hnÿø¨¡ */
	$"206C 8022 2050 7000 1028 0011 0C80 0000"            /*  l€" Pp..(...€.. */
	$"00FF 662A 2D6C 800E FFEC 3D6C 8012 FFF0"            /* .ÿf*-l€.ÿì=l€.ÿğ */
	$"3D7C 8000 FFF2 3D7C 8000 FFF4 3D7C 8000"            /* =|€.ÿò=|€.ÿô=|€. */
	$"FFF6 0C6E 0001 0012 6626 7601 6022 206C"            /* ÿö.n....f&v.`" l */
	$"8022 2050 2468 001C 2052 2D50 FFEC 3D68"            /* €" P$h.. R-Pÿì=h */
	$"0004 FFF0 2D6C 8002 FFF2 3D6C 8006 FFF6"            /* ..ÿğ-l€.ÿò=l€.ÿö */
	$"486E FFEC AA14 486E FFF8 2F3C 0001 0001"            /* Hnÿìª.Hnÿø/<.... */
	$"A8A9 486E FFF8 A8A2 486E FFF2 AA14 3F2C"            /* ¨©Hnÿø¨¢Hnÿòª.?, */
	$"801E 3F2C 801C A893 4A03 6706 3F3C 0031"            /* €.?,€.¨“J.g.?<.1 */
	$"A889 206C 8022 2050 4868 0028 A884 4A03"            /* ¨‰ l€" PHh.(¨„J. */
	$"6706 3F3C 0001 A889 2004 C18C 4CDF 0418"            /* g.?<..¨‰ .ÁŒLß.. */
	$"4E5E 4E74 000C 9144 4556 4943 454C 4F4F"            /* N^Nt..‘DEVICELOO */
	$"5052 4F55 5449 4E45 0000 4E56 FFF8 48E7"            /* PROUTINE..NVÿøHç */
	$"1020 246C 8028 0C6C 0002 8026 661A 200A"            /* . $l€(.l..€&f. Â */
	$"0800 001F 6704 760B 6002 760A 200A 0280"            /* ....g.v.`.vÂ Â.€ */
	$"7FFF FFFF 2440 6004 362C 8026 206C 8022"            /* .ÿÿÿ$@`.6,€& l€" */
	$"2050 2D68 0008 FFF8 2D68 000C FFFC 0C43"            /*  P-h..ÿø-h..ÿü.C */
	$"000B 6622 206C 8022 2050 2D68 0008 FFF8"            /* ..f" l€" P-h..ÿø */
	$"2D68 000C FFFC 302E FFFA 48C0 7215 D081"            /* -h..ÿü0.ÿúHÀr.Ğ */
	$"3D40 FFFE 6012 206C 8022 2050 2D68 0008"            /* =@ÿş`. l€" P-h.. */
	$"FFF8 2D68 000C FFFC 2F0A 486E FFF8 A8DF"            /* ÿø-h..ÿü/ÂHnÿø¨ß */
	$"0C6C 0002 8026 6704 7001 2440 204A 4CDF"            /* .l..€&g.p.$@ JLß */
	$"0408 4E5E 4E75 9143 616C 6375 6C61 7465"            /* ..N^Nu‘Calculate */
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

resource 'CNTL' (10004, "Shortcuts.Set") {
	{2, 196, 23, 371},
	0,
	visible,
	105,
	10004,
	409,
	0,
	"Shortcuts Set:"
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

resource 'CNTL' (10909) {
	{36, 251, 51, 471},
	0,
	visible,
	1,
	0,
	checkBoxProcUseWFont,
	0,
	"Use Fast(er) DCC Sends"
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

resource 'DITL' (128, "About") {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{185, 151, 204, 209},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{0, 0, 144, 360},
		Picture {
			disabled,
			128
		},
		/* [3] */
		{66, 147, 80, 307},
		UserItem {
			disabled
		},
		/* [4] */
		{148, 2, 162, 359},
		UserItem {
			disabled
		},
		/* [5] */
		{163, 2, 177, 359},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (129) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{77, 163, 97, 228},
		Button {
			enabled,
			"Continue"
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
			"MacTCP error: Could not open TCP driver."
			" (id=^0)."
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

resource 'DITL' (131, "Quit.Save") {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{60, 248, 80, 306},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{60, 159, 80, 217},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{5, 46, 48, 307},
		StaticText {
			disabled,
			"Do you want to save preferences before q"
			"uitting?"
		},
		/* [4] */
		{5, 12, 37, 44},
		Icon {
			disabled,
			0
		},
		/* [5] */
		{60, 12, 80, 70},
		Button {
			enabled,
			"Cancel"
		}
	}
};

resource 'DITL' (135, "Find") {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{98, 259, 118, 317},
		Button {
			enabled,
			"Find"
		},
		/* [2] */
		{98, 10, 118, 68},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{1, 16, 17, 130},
		StaticText {
			disabled,
			"Search String:"
		},
		/* [4] */
		{20, 13, 67, 314},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{73, 13, 91, 146},
		CheckBox {
			enabled,
			"Case Sensitive"
		},
		/* [6] */
		{73, 173, 91, 306},
		CheckBox {
			enabled,
			"Search Backwards"
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

resource 'DITL' (137, "TW.SaveChanges?") {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{87, 284, 107, 344},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{87, 211, 107, 271},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{87, 70, 107, 155},
		Button {
			enabled,
			"DonÕt Save"
		},
		/* [4] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			2
		},
		/* [5] */
		{10, 72, 76, 344},
		StaticText {
			disabled,
			"Save changes to the text file Ò^0Ó befor"
			"e closing?"
		}
	}
};

resource 'DITL' (138, "TW.SaveChangesLowMem") {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{87, 284, 107, 344},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{87, 211, 107, 271},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{87, 70, 107, 155},
		Button {
			enabled,
			"DonÕt Save"
		},
		/* [4] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			2
		},
		/* [5] */
		{10, 72, 76, 344},
		StaticText {
			disabled,
			"ShadowIRC is running low on memory. Save"
			" changes to the text file Ò^0Ó and close"
			" to free up memory?"
		}
	}
};

resource 'DITL' (139, "TW.NoFreeMem") {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{87, 284, 107, 344},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			2
		},
		/* [3] */
		{10, 72, 76, 344},
		StaticText {
			disabled,
			"ShadowIRC does not have enough memory to"
			" open the text file Ò^0.Ó"
		}
	}
};

resource 'DITL' (140) {
	{	/* array DITLarray: 1 elements */
		/* [1] */
		{2, 2, 46, 207},
		StaticText {
			disabled,
			"Please wait while ShadowIRC closes TCP/I"
			"P connectionsÉ"
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

resource 'DITL' (260) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{141, 337, 161, 395},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{3, 38, 136, 400},
		StaticText {
			disabled,
			""
		},
		/* [3] */
		{3, 4, 35, 36},
		Icon {
			disabled,
			2
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

resource 'DITL' (1000, "Splash") {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{0, 0, 144, 360},
		Picture {
			disabled,
			128
		},
		/* [2] */
		{66, 144, 80, 304},
		UserItem {
			disabled
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
		{102, 379, 115, 477},
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
		{176, 121, 291, 511},
		Control {
			enabled,
			10100
		},
		/* [12] */
		{198, 128, 213, 278},
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
		{213, 128, 228, 278},
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
		{247, 339, 262, 507},
		CheckBox {
			enabled,
			"Show server messages"
		},
		/* [19] */
		{231, 339, 247, 439},
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
		{101, 318, 113, 374},
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
		{213, 339, 228, 507},
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
		{229, 135, 243, 289},
		StaticText {
			disabled,
			"Maximum Connection Attempts:"
		},
		/* [27] */
		{252, 170, 266, 289},
		StaticText {
			disabled,
			"Connection Retry Delay:"
		},
		/* [28] */
		{229, 294, 242, 324},
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
		{272, 339, 287, 439},
		CheckBox {
			enabled,
			"TalkCity Server"
		},
		/* [32] */
		{76, 421, 96, 518},
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

resource 'DITL' (1003, "Shortcuts") {
	{	/* array DITLarray: 25 elements */
		/* [1] */
		{321, 346, 341, 404},
		Button {
			enabled,
			"Done"
		},
		/* [2] */
		{321, 27, 341, 85},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{5, 24, 21, 107},
		StaticText {
			disabled,
			"Shortcuts"
		},
		/* [4] */
		{28, 3, 43, 21},
		StaticText {
			disabled,
			"1:"
		},
		/* [5] */
		{52, 3, 67, 21},
		StaticText {
			disabled,
			"2:"
		},
		/* [6] */
		{76, 3, 91, 21},
		StaticText {
			disabled,
			"3:"
		},
		/* [7] */
		{100, 3, 115, 21},
		StaticText {
			disabled,
			"4:"
		},
		/* [8] */
		{124, 3, 139, 21},
		StaticText {
			disabled,
			"5:"
		},
		/* [9] */
		{148, 3, 163, 21},
		StaticText {
			disabled,
			"6:"
		},
		/* [10] */
		{172, 3, 187, 21},
		StaticText {
			disabled,
			"7:"
		},
		/* [11] */
		{196, 3, 211, 21},
		StaticText {
			disabled,
			"8:"
		},
		/* [12] */
		{220, 3, 235, 21},
		StaticText {
			disabled,
			"9:"
		},
		/* [13] */
		{244, 3, 259, 21},
		StaticText {
			disabled,
			"10:"
		},
		/* [14] */
		{28, 26, 42, 426},
		EditText {
			enabled,
			""
		},
		/* [15] */
		{52, 26, 66, 426},
		EditText {
			enabled,
			""
		},
		/* [16] */
		{76, 26, 90, 426},
		EditText {
			enabled,
			""
		},
		/* [17] */
		{100, 26, 114, 426},
		EditText {
			enabled,
			""
		},
		/* [18] */
		{124, 26, 138, 426},
		EditText {
			enabled,
			""
		},
		/* [19] */
		{148, 26, 162, 426},
		EditText {
			enabled,
			""
		},
		/* [20] */
		{172, 26, 186, 426},
		EditText {
			enabled,
			""
		},
		/* [21] */
		{196, 26, 210, 426},
		EditText {
			enabled,
			""
		},
		/* [22] */
		{220, 26, 234, 426},
		EditText {
			enabled,
			""
		},
		/* [23] */
		{244, 26, 258, 426},
		EditText {
			enabled,
			""
		},
		/* [24] */
		{265, 6, 317, 428},
		StaticText {
			disabled,
			""
		},
		/* [25] */
		{2, 196, 23, 371},
		Control {
			enabled,
			10004
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
		{355, 3, 375, 61},
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
			"SelectÉ"
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
		Control {
			enabled,
			10909
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
			"SelectÉ"
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
		{103, 15, 119, 229},
		CheckBox {
			enabled,
			"Cursor Focusing"
		},
		/* [6] */
		{120, 25, 136, 239},
		CheckBox {
			enabled,
			"but don't activate the window"
		},
		/* [7] */
		{137, 15, 153, 229},
		CheckBox {
			enabled,
			"Don't show channel modes display"
		},
		/* [8] */
		{154, 15, 170, 229},
		CheckBox {
			enabled,
			"Allow Drag and Drop to same window"
		},
		/* [9] */
		{171, 15, 187, 229},
		CheckBox {
			enabled,
			"Put inputline in message windows"
		},
		/* [10] */
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

resource 'DITL' (10003, "topic") {
	{	/* array DITLarray: 7 elements */
		/* [1] */
		{155, 273, 175, 331},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{155, 6, 175, 64},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{4, 6, 19, 330},
		StaticText {
			disabled,
			"Topic for ^0:"
		},
		/* [4] */
		{26, 28, 91, 309},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{100, 8, 133, 329},
		StaticText {
			disabled,
			"Topic set by ^1 on ^2 at ^3."
		},
		/* [6] */
		{135, 8, 151, 329},
		StaticText {
			disabled,
			""
		},
		/* [7] */
		{0, 0, 0, 0},
		HelpItem {
			enabled,
			HMScanhdlg {
				10003
			}
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

resource 'DLOG' (128, "About") {
	{73, 53, 282, 413},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	128,
	"About ShadowIRC",
	alertPositionMainScreen
};

data 'DLOG' (130, "Err") {
	$"0036 0028 009C 01AF 0001 0100 0100 0000"            /* .6.(.œ.¯........ */
	$"0000 0082 00"                                       /* ...‚. */
};

resource 'DLOG' (131, "Quit") {
	{73, 90, 161, 410},
	dBoxProc,
	visible,
	noGoAway,
	0x0,
	131,
	"Quit",
	alertPositionMainScreen
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

resource 'DLOG' (135, "Find") {
	{73, 88, 196, 417},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	135,
	"Find Text",
	alertPositionMainScreen
};

resource 'DLOG' (137) {
	{62, 41, 179, 394},
	dBoxProc,
	visible,
	goAway,
	0x0,
	137,
	"",
	alertPositionMainScreen
};

resource 'DLOG' (138) {
	{62, 41, 179, 394},
	dBoxProc,
	visible,
	goAway,
	0x0,
	138,
	"",
	alertPositionMainScreen
};

resource 'DLOG' (139) {
	{62, 41, 179, 394},
	dBoxProc,
	visible,
	goAway,
	0x0,
	139,
	"",
	alertPositionMainScreen
};

resource 'DLOG' (140, "Close Connections") {
	{83, 133, 132, 343},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	140,
	"Closing Connections",
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

resource 'DLOG' (260, "Kill") {
	{83, 88, 249, 492},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	260,
	"Connection Terminated",
	alertPositionParentWindowScreen
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

resource 'DLOG' (1000, "Splash") {
	{107, 141, 251, 501},
	dBoxProc,
	visible,
	noGoAway,
	0x0,
	1000,
	"ShadowIRC",
	alertPositionMainScreen
};

resource 'DLOG' (1003, "Shortcuts") {
	{63, 35, 408, 468},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	1003,
	"Shortcuts",
	alertPositionMainScreen
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

resource 'DLOG' (10003, "topic") {
	{58, 41, 238, 378},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	10003,
	"Channel Topic",
	alertPositionParentWindow
};

resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"001F 7800 0020 8400 0044 5200 004F 3A00"
		$"007F FE00 004E 0600 01FF FF80 1E00 0078"
		$"61FF FF86 FFFF FFF9 DFFF FFFF 6FFF FFFE"
		$"1D7F FEB8 03E7 E6C0 0369 97C0 0377 EEC0"
		$"017F FF80 017F FF80 017F FF80 00BF FD00"
		$"005F FA00 006F F600 007F FE00 00DF FB80"
		$"035F FA70 05AF F688 3B77 FF56 56E9 FF75"
		$"976F FF64 7977 FE9E E0BF FE0F F6FF FCD3",
		/* [2] */
		$"000F 7800 001F FC00 003F FC00 003F FE00"
		$"003F FE00 003F FE00 003F FE00 07FF FFE0"
		$"3FFF FFFC 7FFF FFFE FFFF FFFF FFFF FFFF"
		$"7FFF FFFE 1FFF FFF8 07FF FFE0 03FF FFC0"
		$"03FF FFC0 01FF FFC0 01FF FF80 01FF FF80"
		$"00FF FF80 007F FF00 007F FF00 00FF FF00"
		$"01FF FFE0 07FF FFF8 3FFF FFFE 7FFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
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

resource 'ICN#' (131) {
	{	/* array: 2 elements */
		/* [1] */
		$"BC00 0000 4200 0000 2D00 1FF8 9D00 100C"
		$"FF00 100A 0300 100F FFC0 1001 003C 1001"
		$"FFD7 12D9 FFFE 9001 FFFF 93D9 FFFF 1001"
		$"FF5C 1369 F360 1001 CBE0 1001 E760 1001"
		$"FFC0 1001 FFC0 1FFF FFC0 0000 FE80 0000"
		$"FD00 0000 FB00 0000 FF00 0000 FDC0 0000"
		$"FD78 0000 FBD4 0000 FFAB 0000 FFFF 8000"
		$"FFB6 8000 FF6F 8000 FF8F 8000 FF7B 80",
		/* [2] */
		$"7800 0000 FC00 0000 FC00 1FF8 FE00 1FFC"
		$"FE00 1FFE FE00 1FFF FE00 1FFF FFF0 1FFF"
		$"FFFC 1FFF FFFE 1FFF FFFF 1FFF FFFF 1FFF"
		$"FFFE 1FFF FFFC 1FFF FFF0 1FFF FFC0 1FFF"
		$"FFC0 1FFF FFC0 1FFF FFC0 0000 FF80 0000"
		$"FF80 0000 FF00 0000 FF00 0000 FF00 0000"
		$"FFE0 0000 FFFC 0000 FFFE 0000 FFFF 0000"
		$"FFFF 0000 FFFF 0000 FFFF 0000 FFFF"
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
		"Open Connection", noIcon, hierarchicalMenu, "Í", plain,
		/* [2] */
		"Close Connection", noIcon, hierarchicalMenu, "Î", plain,
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
		"OtherÉ", noIcon, noKey, noMark, 2
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

resource 'MENU' (10004, "Shortcuts.Set") {
	10004,
	textMenuProc,
	allEnabled,
	enabled,
	"Shortcuts Set",
	{	/* array: 3 elements */
		/* [1] */
		"Set 1", noIcon, noKey, noMark, plain,
		/* [2] */
		"Set 2", noIcon, noKey, noMark, plain,
		/* [3] */
		"Set 3", noIcon, noKey, noMark, plain
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

data 'SIRC' (0, "Owner resource") {
	$"2353 6861 646F 7749 5243 2C20 A920 3139"            /* #ShadowIRC, © 19 */
	$"3936 2D32 3030 3220 4A6F 686E 2042 6166"            /* 96-2002 John Baf */
	$"666F 7264"                                          /* ford */
};

data 'TEXT' (127, "Shortcut text") {
	$"5C6E 203D 2072 6574 7572 6E3B 2024 6368"            /* \n = return; $ch */
	$"616E 6E65 6C20 3D20 6375 7272 656E 7420"            /* annel = current  */
	$"6368 616E 6E65 6C3B 2024 6C61 7374 6A6F"            /* channel; $lastjo */
	$"696E 6572 203D 206C 6173 7420 7065 7273"            /* iner = last pers */
	$"6F6E 2074 6F20 6A6F 696E 2074 6869 7320"            /* on to join this  */
	$"6368 616E 6E65 6C3B 2024 6E69 636B 203D"            /* channel; $nick = */
	$"2063 7572 7265 6E74 206E 6963 6B3B 2024"            /*  current nick; $ */
	$"7365 7276 6572 203D 2063 7572 7265 6E74"            /* server = current */
	$"2073 6572 7665 723B 2024 7365 6C65 6374"            /*  server; $select */
	$"6564 7573 6572 203D 2066 6972 7374 2075"            /* eduser = first u */
	$"7365 7220 7365 6C65 6374 6564 2069 6E20"            /* ser selected in  */
	$"7573 6572 6C69 7374 3B20 242D 3E20 3D20"            /* userlist; $-> =  */
	$"7365 7420 6375 7273 6F72 2070 6F73 6974"            /* set cursor posit */
	$"696F 6E2F 6265 6769 6E20 7365 6C65 6374"            /* ion/begin select */
	$"696F 6E3B 2024 3C2D 203D 2065 6E64 2073"            /* ion; $<- = end s */
	$"656C 6563 7469 6F6E 3B20 2469 6C20 3D20"            /* election; $il =  */
	$"696E 7075 746C 696E 6520 7465 7874 3B20"            /* inputline text;  */
	$"2464 6174 652C 2024 6461 7465 2E6C 2C20"            /* $date, $date.l,  */
	$"2474 696D 652C 2024 7469 6D65 2E73 2C20"            /* $time, $time.s,  */
	$"2476 6572 7369 6F6E"                                /* $version */
};

data 'TEXT' (128, "Credits") {
	$"5368 6164 6F77 4952 4320 312E 320D A920"            /* ShadowIRC 1.2.©  */
	$"3139 3936 2D32 3030 3220 4A6F 686E 2042"            /* 1996-2002 John B */
	$"6166 666F 7264 0D41 6C6C 2072 6967 6874"            /* afford.All right */
	$"7320 7265 7365 7276 6564 0D0D 5368 6164"            /* s reserved..Shad */
	$"6F77 4952 4320 6973 2066 7265 6520 736F"            /* owIRC is free so */
	$"6674 7761 7265 2C20 6469 7374 7269 6275"            /* ftware, distribu */
	$"7465 6420 756E 6465 7220 7468 6520 474E"            /* ted under the GN */
	$"5520 4765 6E65 7261 6C20 5075 626C 6963"            /* U General Public */
	$"204C 6963 656E 7365 2E20 466F 7220 6C69"            /*  License. For li */
	$"6365 6E73 6520 7465 726D 732C 2070 6C65"            /* cense terms, ple */
	$"6173 6520 7365 6520 7468 6520 224C 6963"            /* ase see the "Lic */
	$"656E 7365 2220 6669 6C65 2074 6861 7420"            /* ense" file that  */
	$"6361 6D65 2077 6974 6820 5368 6164 6F77"            /* came with Shadow */
	$"4952 432C 206F 7220 7669 7369 7420 3C68"            /* IRC, or visit <h */
	$"7474 703A 2F2F 7777 772E 676E 752E 6F72"            /* ttp://www.gnu.or */
	$"672F 636F 7079 6C65 6674 2F67 706C 2E68"            /* g/copyleft/gpl.h */
	$"746D 6C3E 0D0D 4F66 6669 6369 616C 2049"            /* tml>..Official I */
	$"5243 2063 6861 6E6E 656C 3A20 2353 6861"            /* RC channel: #Sha */
	$"646F 7749 5243 206F 6E20 556E 6465 726E"            /* dowIRC on Undern */
	$"6574 0D0D 4372 6564 6974 730D 0D53 6861"            /* et..Credits..Sha */
	$"646F 7749 5243 2075 7365 7320 6120 6D6F"            /* dowIRC uses a mo */
	$"6469 6669 6564 2076 6572 7369 6F6E 206F"            /* dified version o */
	$"6620 7468 6520 5741 5354 4520 7465 7874"            /* f the WASTE text */
	$"2065 6E67 696E 652E 0D57 4153 5445 2074"            /*  engine..WASTE t */
	$"6578 7420 656E 6769 6E65 20A9 2031 3939"            /* ext engine © 199 */
	$"332D 3139 3938 204D 6172 636F 2050 696F"            /* 3-1998 Marco Pio */
	$"7661 6E65 6C6C 692E 0D0D 5368 6164 6F77"            /* vanelli...Shadow */
	$"4952 4320 7761 7320 6261 7365 6420 6F6E"            /* IRC was based on */
	$"2049 7263 6C65 2031 2E35 2E31 2062 7920"            /*  Ircle 1.5.1 by  */
	$"4F6C 6166 2054 6974 7A2C 2061 6E64 2068"            /* Olaf Titz, and h */
	$"6173 2062 6565 6E20 636F 6D70 6C65 7465"            /* as been complete */
	$"6C79 2072 6577 7269 7474 656E 2069 6E20"            /* ly rewritten in  */
	$"432E 2049 7263 6C65 2031 2E35 2E31 2069"            /* C. Ircle 1.5.1 i */
	$"7320 6C69 6365 6E73 6564 2075 6E64 6572"            /* s licensed under */
	$"2074 6865 2047 4E55 2047 656E 6572 616C"            /*  the GNU General */
	$"2050 7562 6C69 6320 4C69 6365 6E73 652E"            /*  Public License. */
	$"0D0D 4869 6768 6C69 6768 7420 706C 7567"            /* ..Highlight plug */
	$"696E 2062 6173 6564 206F 6E20 7468 6520"            /* in based on the  */
	$"2254 6572 6D42 6F6C 6422 2070 6C75 6769"            /* "TermBold" plugi */
	$"6E20 6279 2048 3230 2E0D 0D54 6861 6E6B"            /* n by H20...Thank */
	$"7320 746F 204D 6963 6861 656C 204C 6564"            /* s to Michael Led */
	$"666F 7264 2066 6F72 2064 6562 7567 6769"            /* ford for debuggi */
	$"6E67 2061 6E64 2064 6576 656C 6F70 6D65"            /* ng and developme */
	$"6E74 2061 7373 6973 7461 6E63 652E 0D0D"            /* nt assistance... */
	$"5370 6563 6961 6C20 7468 616E 6B73 2067"            /* Special thanks g */
	$"6F65 7320 746F 2042 6174 6D61 6E50 5043"            /* oes to BatmanPPC */
	$"2C20 4E69 6768 7477 696E 672C 2061 6E64"            /* , Nightwing, and */
	$"204B 6F72 656E 6469 6C20 666F 7220 7468"            /*  Korendil for th */
	$"6569 7220 776F 726B 206F 6E20 7468 6520"            /* eir work on the  */
	$"5368 6164 6F77 4952 4320 7765 6270 6167"            /* ShadowIRC webpag */
	$"6573 2E0D 0D54 6861 6E6B 7320 676F 6573"            /* es...Thanks goes */
	$"206F 7574 2074 6F20 7468 6520 616C 7068"            /*  out to the alph */
	$"6120 7465 7374 6572 732C 2070 6172 7469"            /* a testers, parti */
	$"6375 6172 616C 6C79 2054 7769 7374 2C20"            /* cuarally Twist,  */
	$"4861 6C6C 6A65 2C20 6B65 762C 204C 6F72"            /* Hallje, kev, Lor */
	$"6457 7572 6D2C 2065 4C69 5865 522C 204D"            /* dWurm, eLiXeR, M */
	$"6163 2D61 7265 6E61 2C20 546F 6F6C 426F"            /* ac-arena, ToolBo */
	$"792C 206B 6F72 656E 6469 6C2C 2043 616D"            /* y, korendil, Cam */
	$"6572 6F6F 6E2C 2043 6872 6973 432C 206D"            /* eroon, ChrisC, m */
	$"6C65 6466 6F72 642C 2061 6E64 2074 6865"            /* ledford, and the */
	$"206F 7468 6572 2072 6567 756C 6172 7320"            /*  other regulars  */
	$"6F66 2055 6E64 6572 6E65 7420 236D 6163"            /* of Undernet #mac */
	$"696E 746F 7368 2061 6E64 2023 6D61 6364"            /* intosh and #macd */
	$"6576 2066 6F72 2066 696E 6469 6E67 206D"            /* ev for finding m */
	$"616E 7920 6275 6773 2E0D 0D46 7265 6E63"            /* any bugs...Frenc */
	$"6820 6C61 6E67 7561 6765 2070 6F72 7420"            /* h language port  */
	$"6279 204A 6561 6E2D 5069 6572 7265 204B"            /* by Jean-Pierre K */
	$"7579 7065 7273 2E0D 0D0D 486F 7720 746F"            /* uypers....How to */
	$"2063 6F6E 7461 6374 2074 6865 2061 7574"            /*  contact the aut */
	$"686F 723A 0D45 6D61 696C 3A20 6473 6861"            /* hor:.Email: dsha */
	$"646F 7740 7368 6164 6F77 6972 632E 636F"            /* dow@shadowirc.co */
	$"6D0D 4952 433A 2055 6E64 6572 6E65 743A"            /* m.IRC: Undernet: */
	$"2044 5368 6164 6F77 2C20 236D 6163 696E"            /*  DShadow, #macin */
	$"746F 7368 2C20 236D 6163 6465 762C 2061"            /* tosh, #macdev, a */
	$"6E64 2023 5368 6164 6F77 4952 430D 0D54"            /* nd #ShadowIRC..T */
	$"6865 2046 696E 6520 5072 696E 743A 0D54"            /* he Fine Print:.T */
	$"6869 7320 7072 6F67 7261 6D20 6973 2066"            /* his program is f */
	$"7265 6520 736F 6674 7761 7265 3B20 796F"            /* ree software; yo */
	$"7520 6361 6E20 7265 6469 7374 7269 6275"            /* u can redistribu */
	$"7465 2069 7420 616E 642F 6F72 0D6D 6F64"            /* te it and/or.mod */
	$"6966 7920 6974 2075 6E64 6572 2074 6865"            /* ify it under the */
	$"2074 6572 6D73 206F 6620 7468 6520 474E"            /*  terms of the GN */
	$"5520 4765 6E65 7261 6C20 5075 626C 6963"            /* U General Public */
	$"204C 6963 656E 7365 0D61 7320 7075 626C"            /*  License.as publ */
	$"6973 6865 6420 6279 2074 6865 2046 7265"            /* ished by the Fre */
	$"6520 536F 6674 7761 7265 2046 6F75 6E64"            /* e Software Found */
	$"6174 696F 6E3B 2065 6974 6865 7220 7665"            /* ation; either ve */
	$"7273 696F 6E20 320D 6F66 2074 6865 204C"            /* rsion 2.of the L */
	$"6963 656E 7365 2C20 6F72 2028 6174 2079"            /* icense, or (at y */
	$"6F75 7220 6F70 7469 6F6E 2920 616E 7920"            /* our option) any  */
	$"6C61 7465 7220 7665 7273 696F 6E2E 0D0D"            /* later version... */
	$"5468 6973 2070 726F 6772 616D 2069 7320"            /* This program is  */
	$"6469 7374 7269 6275 7465 6420 696E 2074"            /* distributed in t */
	$"6865 2068 6F70 6520 7468 6174 2069 7420"            /* he hope that it  */
	$"7769 6C6C 2062 6520 7573 6566 756C 2C0D"            /* will be useful,. */
	$"6275 7420 5749 5448 4F55 5420 414E 5920"            /* but WITHOUT ANY  */
	$"5741 5252 414E 5459 3B20 7769 7468 6F75"            /* WARRANTY; withou */
	$"7420 6576 656E 2074 6865 2069 6D70 6C69"            /* t even the impli */
	$"6564 2077 6172 7261 6E74 7920 6F66 0D4D"            /* ed warranty of.M */
	$"4552 4348 414E 5441 4249 4C49 5459 206F"            /* ERCHANTABILITY o */
	$"7220 4649 544E 4553 5320 464F 5220 4120"            /* r FITNESS FOR A  */
	$"5041 5254 4943 554C 4152 2050 5552 504F"            /* PARTICULAR PURPO */
	$"5345 2E20 2053 6565 2074 6865 0D47 4E55"            /* SE.  See the.GNU */
	$"2047 656E 6572 616C 2050 7562 6C69 6320"            /*  General Public  */
	$"4C69 6365 6E73 6520 666F 7220 6D6F 7265"            /* License for more */
	$"2064 6574 6169 6C73 2E0D"                           /*  details.. */
};

data 'TEXT' (129, "Command Reference") {
	$"5368 6164 6F77 4952 4320 312E 320D 436F"            /* ShadowIRC 1.2.Co */
	$"6D6D 616E 6420 5265 6665 7265 6E63 650D"            /* mmand Reference. */
	$"0D50 6172 616D 6574 6572 7320 696E 205B"            /* .Parameters in [ */
	$"6272 6163 6B65 7473 5D20 6172 6520 6F70"            /* brackets] are op */
	$"7469 6F6E 616C 2E0D 0D2F 6261 6E73 205B"            /* tional.../bans [ */
	$"3C6D 6173 6B3E 5D20 2D20 6C69 7374 7320"            /* <mask>] - lists  */
	$"6261 6E73 206F 6E20 6375 7272 656E 7420"            /* bans on current  */
	$"6368 616E 6E65 6C2C 206F 7074 696F 6E61"            /* channel, optiona */
	$"6C6C 7920 6D61 7463 6869 6E67 206D 6173"            /* lly matching mas */
	$"6B2E 0D2F 6272 6163 7420 2D20 6272 6F61"            /* k../bract - broa */
	$"6463 6173 7420 6163 7469 6F6E 0D2F 6272"            /* dcast action./br */
	$"6F61 6463 6173 7420 2D20 6272 6F61 6463"            /* oadcast - broadc */
	$"6173 7420 7465 7874 2074 6F20 616C 6C20"            /* ast text to all  */
	$"6368 616E 6E65 6C73 0D2F 6279 6520 5B3C"            /* channels./bye [< */
	$"7369 676E 6F66 6620 7265 6173 6F6E 3E5D"            /* signoff reason>] */
	$"202D 2064 6973 636F 6E6E 6563 7473 2066"            /*  - disconnects f */
	$"726F 6D20 616C 6C20 7365 7276 6572 732E"            /* rom all servers. */
	$"0D2F 6320 3C63 6F6E 6E65 6374 696F 6E4E"            /* ./c <connectionN */
	$"756D 6265 7220 7C20 636F 6E6E 6563 7469"            /* umber | connecti */
	$"6F6E 4E61 6D65 3E20 5B3C 7365 7276 6572"            /* onName> [<server */
	$"3E20 5B3C 706F 7274 3E20 5B3C 7061 7373"            /* > [<port> [<pass */
	$"776F 7264 3E5D 5D5D 2D20 6F70 656E 7320"            /* word>]]]- opens  */
	$"6120 636F 6E6E 6563 7469 6F6E 0D2F 636C"            /* a connection./cl */
	$"6561 7220 2D20 636C 6561 7273 2061 6C6C"            /* ear - clears all */
	$"2074 6865 2074 6578 7420 696E 2074 6865"            /*  the text in the */
	$"2063 7572 7265 6E74 206D 6573 7367 6520"            /*  current messge  */
	$"7769 6E64 6F77 0D2F 6370 696E 6720 3C6E"            /* window./cping <n */
	$"6963 6B3E 202D 2064 6F65 7320 6120 6374"            /* ick> - does a ct */
	$"6370 2070 696E 6720 6F6E 2073 6F6D 656F"            /* cp ping on someo */
	$"6E65 2061 6E64 2064 6973 706C 6179 7320"            /* ne and displays  */
	$"7468 6569 7220 7069 6E67 2074 696D 6520"            /* their ping time  */
	$"696E 2074 656E 7468 7320 6F66 2073 6563"            /* in tenths of sec */
	$"6F6E 6473 0D2F 6374 6370 203C 6E69 636B"            /* onds./ctcp <nick */
	$"3E20 3C63 6F6D 6D61 6E64 3E0D 2F64 203C"            /* > <command>./d < */
	$"636F 6E6E 6563 7469 6F6E 4E75 6D62 6572"            /* connectionNumber */
	$"207C 2063 6F6E 6E65 6374 696F 6E4E 616D"            /*  | connectionNam */
	$"653E 202D 2063 6C6F 7365 7320 6120 636F"            /* e> - closes a co */
	$"6E6E 6563 7469 6F6E 0D2F 6463 6320 3C5B"            /* nnection./dcc <[ */
	$"6C69 7374 5D7C 636C 6F73 657C 6368 6174"            /* list]|close|chat */
	$"7C73 656E 647C 6765 743E 0D2F 6465 6275"            /* |send|get>./debu */
	$"6720 2D20 7475 726E 7320 6F6E 2064 6562"            /* g - turns on deb */
	$"7567 206D 6F64 650D 2F64 6E73 203C 4950"            /* ug mode./dns <IP */
	$"7C6D 6163 6869 6E65 4E61 6D65 3E0D 2F65"            /* |machineName>./e */
	$"7863 6570 7469 6F6E 7320 5B3C 6D61 736B"            /* xceptions [<mask */
	$"3E5D 202D 206C 6973 7473 2065 7863 6570"            /* >] - lists excep */
	$"7469 6F6E 7320 6F6E 2063 7572 7265 6E74"            /* tions on current */
	$"2063 6861 6E6E 656C 2C20 6F70 7469 6F6E"            /*  channel, option */
	$"616C 6C79 206D 6174 6368 696E 6720 6D61"            /* ally matching ma */
	$"736B 2E0D 2F65 7869 740D 2F69 676E 6F72"            /* sk../exit./ignor */
	$"6520 5B2D 5D3C 6E69 636B 207C 2075 7365"            /* e [-]<nick | use */
	$"7240 686F 7374 207C 206E 6963 6B21 7573"            /* r@host | nick!us */
	$"6572 4068 6F73 743E 205B 2E2E 2E5D 0D2F"            /* er@host> [...]./ */
	$"6A20 3D20 2F6A 6F69 6E0D 2F6A 6F69 6E20"            /* j = /join./join  */
	$"3C63 6861 6E6E 656C 3E20 5B3C 6368 616E"            /* <channel> [<chan */
	$"6E65 6C20 6B65 793E 5D0D 2F6B 203D 202F"            /* nel key>]./k = / */
	$"6B69 636B 0D2F 6B69 636B 203C 6368 616E"            /* kick./kick <chan */
	$"6E65 6C3E 203C 6E69 636B 6E61 6D65 3E20"            /* nel> <nickname>  */
	$"5B3C 7265 6173 6F6E 3E5D 202D 204B 6963"            /* [<reason>] - Kic */
	$"6B73 2061 2075 7365 7220 6672 6F6D 2061"            /* ks a user from a */
	$"2063 6861 6E6E 656C 2E0D 2F6B 696C 6C20"            /*  channel../kill  */
	$"3C6E 6963 6B6E 616D 653E 205B 3C72 6561"            /* <nickname> [<rea */
	$"736F 6E3E 5D0D 2F6C 6561 7665 203D 202F"            /* son>]./leave = / */
	$"7061 7274 0D2F 6D20 3D20 2F6D 7367 0D2F"            /* part./m = /msg./ */
	$"6D65 202D 2061 6374 696F 6E0D 2F6D 7367"            /* me - action./msg */
	$"203C 6E69 636B 6E61 6D65 3E20 3C6D 6573"            /*  <nickname> <mes */
	$"7361 6765 3E0D 2F6D 7367 7120 2D20 4C69"            /* sage>./msgq - Li */
	$"6B65 202F 6D73 672C 2062 7574 2064 6F65"            /* ke /msg, but doe */
	$"736E 2774 2064 6973 706C 6179 2079 6F75"            /* sn't display you */
	$"7220 6D65 7373 6167 6520 6F6E 2074 6865"            /* r message on the */
	$"2073 6372 6565 6E0D 2F6D 6F64 6520 2D20"            /*  screen./mode -  */
	$"4469 7370 6C61 7973 2061 6E64 2073 6574"            /* Displays and set */
	$"7320 7573 6572 2061 6E64 2063 6861 6E6E"            /* s user and chann */
	$"656C 206D 6F64 6573 2E0D 0943 6861 6E6E"            /* el modes..ÆChann */
	$"656C 204D 6F64 6573 3A0D 0909 2B74 202D"            /* el Modes:.ÆÆ+t - */
	$"204F 6E6C 7920 6368 616E 6E65 6C20 6F70"            /*  Only channel op */
	$"6572 6174 6F72 7320 6361 6E20 6368 616E"            /* erators can chan */
	$"6765 2074 6F70 6963 0D09 092B 6E20 2D20"            /* ge topic.ÆÆ+n -  */
	$"596F 7520 6D75 7374 2062 6520 696E 2074"            /* You must be in t */
	$"6865 2063 6861 6E6E 656C 2074 6F20 7365"            /* he channel to se */
	$"6E64 2061 206D 6573 7361 6765 2074 6F20"            /* nd a message to  */
	$"6974 0D09 092B 6920 2D20 496E 7669 7465"            /* it.ÆÆ+i - Invite */
	$"2D6F 6E6C 792E 204F 6E6C 7920 6368 616E"            /* -only. Only chan */
	$"6E65 6C20 6F70 6572 6174 6F72 7320 6361"            /* nel operators ca */
	$"6E20 696E 7669 7465 2075 7365 7273 2074"            /* n invite users t */
	$"6F20 7468 6973 2063 6861 6E6E 656C 2E0D"            /* o this channel.. */
	$"0909 2B6D 202D 204D 6F64 6572 6174 6564"            /* ÆÆ+m - Moderated */
	$"2E20 4F6E 6C79 2075 7365 7273 2077 6974"            /* . Only users wit */
	$"6820 766F 6963 6520 2863 6861 6E6E 656C"            /* h voice (channel */
	$"206D 6F64 6520 2B76 2920 6D61 7920 7370"            /*  mode +v) may sp */
	$"6561 6B2E 0D09 092B 7020 2D20 5072 6976"            /* eak..ÆÆ+p - Priv */
	$"6174 652E 0D09 092B 7320 2D20 5365 6372"            /* ate..ÆÆ+s - Secr */
	$"6574 2E0D 0909 2B6C 202D 2041 206C 696D"            /* et..ÆÆ+l - A lim */
	$"6974 2069 7320 706C 6163 6564 206F 6E20"            /* it is placed on  */
	$"7468 6520 6E75 6D62 6572 206F 6620 7065"            /* the number of pe */
	$"6F70 6C65 2077 686F 206D 6179 206A 6F69"            /* ople who may joi */
	$"6E20 7468 6973 2063 6861 6E6E 656C 2E0D"            /* n this channel.. */
	$"0909 2B6B 202D 2053 6574 7320 7468 6520"            /* ÆÆ+k - Sets the  */
	$"6368 616E 6E65 6C20 7061 7373 776F 7264"            /* channel password */
	$"2E20 596F 7520 6D75 7374 206B 6E6F 7720"            /* . You must know  */
	$"7468 6973 2070 6173 7377 6F72 6420 746F"            /* this password to */
	$"206A 6F69 6E20 7468 6520 6368 616E 6E65"            /*  join the channe */
	$"6C2E 0D09 092B 6F20 2D20 4368 616E 6E65"            /* l..ÆÆ+o - Channe */
	$"6C20 6F70 6572 6174 6F72 2E20 4769 7665"            /* l operator. Give */
	$"7320 796F 7520 7468 6520 6162 696C 6974"            /* s you the abilit */
	$"7920 746F 2073 6574 2063 6861 6E6E 656C"            /* y to set channel */
	$"206D 6F64 6573 2061 6E64 206B 6963 6B20"            /*  modes and kick  */
	$"7065 6F70 6C65 2066 726F 6D20 7468 6520"            /* people from the  */
	$"6368 616E 6E65 6C2E 0D09 092B 7620 2D20"            /* channel..ÆÆ+v -  */
	$"566F 6963 652E 2041 6C6C 6F77 7320 796F"            /* Voice. Allows yo */
	$"7520 746F 2073 7065 616B 206F 6E20 6120"            /* u to speak on a  */
	$"6D6F 6465 7261 7465 6420 6368 616E 6E65"            /* moderated channe */
	$"6C2E 0D09 092B 6220 2D20 4261 6E6E 6564"            /* l..ÆÆ+b - Banned */
	$"2E20 4966 2079 6F75 2061 7265 2062 616E"            /* . If you are ban */
	$"6E65 642C 2079 6F75 2061 7265 2070 7265"            /* ned, you are pre */
	$"7665 6E74 6564 2066 726F 6D20 6A6F 696E"            /* vented from join */
	$"696E 6720 7468 6520 6368 616E 6E65 6C2E"            /* ing the channel. */
	$"0D2F 6E61 6D65 7320 5B3C 6368 616E 6E65"            /* ./names [<channe */
	$"6C3E 5D0D 2F6E 6963 6B20 5B3C 6E65 776E"            /* l>]./nick [<newn */
	$"6963 6B3E 5D0D 2F6E 6F74 6963 6520 3C6E"            /* ick>]./notice <n */
	$"6963 6B6E 616D 657C 6368 616E 6E65 6C6E"            /* ickname|channeln */
	$"616D 653E 203C 6D65 7373 6167 653E 0D2F"            /* ame> <message>./ */
	$"6E6F 7469 6365 7120 2D20 4C69 6B65 202F"            /* noticeq - Like / */
	$"6E6F 7469 6365 2C20 6275 7420 646F 6573"            /* notice, but does */
	$"6E27 7420 6469 7370 6C61 7920 796F 7572"            /* n't display your */
	$"206D 6573 7361 6765 206F 6E20 7468 6520"            /*  message on the  */
	$"7363 7265 656E 0D2F 6E6F 7469 6679 205B"            /* screen./notify [ */
	$"5B2D 5D3C 6E69 636B 3E5D 202D 2041 6464"            /* [-]<nick>] - Add */
	$"7320 6F72 2072 656D 6F76 6573 2073 6F6D"            /* s or removes som */
	$"656F 6E65 2066 726F 6D20 7468 6520 6E6F"            /* eone from the no */
	$"7469 6679 206C 6973 7420 666F 7220 7468"            /* tify list for th */
	$"6520 6375 7272 656E 7420 636F 6E6E 6563"            /* e current connec */
	$"7469 6F6E 2E0D 2F6F 6E6F 7469 6365 202D"            /* tion../onotice - */
	$"2053 656E 6473 2061 206E 6F74 6963 6520"            /*  Sends a notice  */
	$"746F 2061 6C6C 2074 6865 206F 7073 206F"            /* to all the ops o */
	$"6620 7468 6520 6375 7272 656E 7420 6368"            /* f the current ch */
	$"616E 6E65 6C2E 0D2F 7061 7274 205B 3C23"            /* annel../part [<# */
	$"6368 616E 6E65 6C3E 5D20 5B3C 7265 6173"            /* channel>] [<reas */
	$"6F6E 3E5D 202D 206C 6561 7665 7320 6120"            /* on>] - leaves a  */
	$"6368 616E 6E65 6C2C 206F 7220 7468 6520"            /* channel, or the  */
	$"6375 7272 656E 7420 6368 616E 6E65 6C20"            /* current channel  */
	$"6966 206E 6F6E 6520 7370 6563 6966 6965"            /* if none specifie */
	$"642E 0D2F 7069 6E67 203D 202F 6374 6370"            /* d../ping = /ctcp */
	$"2070 696E 670D 2F71 203D 202F 7175 6572"            /*  ping./q = /quer */
	$"790D 2F71 7565 7279 203C 6E69 636B 3E20"            /* y./query <nick>  */
	$"5B3C 6D65 7373 6167 653E 5D0D 2F71 7569"            /* [<message>]./qui */
	$"7420 3D20 2F65 7869 740D 2F71 756F 7465"            /* t = /exit./quote */
	$"203C 7465 7874 3E20 2D20 5365 6E64 7320"            /*  <text> - Sends  */
	$"7468 6520 7465 7874 2074 6F20 7468 6520"            /* the text to the  */
	$"7365 7276 6572 2077 6974 686F 7574 2070"            /* server without p */
	$"7265 7072 6F63 6573 7369 6E67 2E20 0D2F"            /* reprocessing. ./ */
	$"7261 7720 3D20 2F71 756F 7465 0D2F 7361"            /* raw = /quote./sa */
	$"7920 3C74 6578 743E 202D 2053 656E 6473"            /* y <text> - Sends */
	$"2074 6865 2074 6578 7420 746F 2074 6865"            /*  the text to the */
	$"2063 7572 7265 6E74 2077 696E 646F 772E"            /*  current window. */
	$"2028 536F 2079 6F75 2063 616E 2074 7970"            /*  (So you can typ */
	$"6520 7465 7874 2074 6861 7420 6265 6769"            /* e text that begi */
	$"6E73 2077 6974 6820 7468 6520 636F 6D6D"            /* ns with the comm */
	$"616E 6420 6368 6172 6163 7465 722E 290D"            /* and character.). */
	$"2F73 6572 7665 720D 2F73 6967 6E6F 6666"            /* /server./signoff */
	$"0D2F 7469 6D65 205B 3C73 6572 7665 726E"            /* ./time [<servern */
	$"616D 653E 5D20 2D20 7265 7475 726E 7320"            /* ame>] - returns  */
	$"7468 6520 6C6F 6361 6C20 7469 6D65 206F"            /* the local time o */
	$"6620 7468 6520 7365 7276 6572 2079 6F75"            /* f the server you */
	$"2061 7265 2063 6F6E 6E65 6374 6564 2074"            /*  are connected t */
	$"6F2C 206F 7220 7468 6520 7365 7276 6572"            /* o, or the server */
	$"2079 6F75 2073 7065 6369 6679 2E0D 2F74"            /*  you specify../t */
	$"6F70 6963 0D2F 756D 6F64 6520 2D20 7361"            /* opic./umode - sa */
	$"6D65 2061 7320 2F6D 6F64 6520 4375 7272"            /* me as /mode Curr */
	$"656E 744E 6963 6B0D 2F75 6E69 7874 696D"            /* entNick./unixtim */
	$"6520 7469 6D65 202D 2043 6F6E 7665 7274"            /* e time - Convert */
	$"7320 6120 7469 6D65 2069 6E20 756E 6978"            /* s a time in unix */
	$"2066 6F72 6D61 7420 746F 2061 2068 756D"            /*  format to a hum */
	$"616E 2D72 6561 6461 626C 6520 666F 726D"            /* an-readable form */
	$"6174 0D2F 7665 7273 696F 6E0D 2F77 686F"            /* at./version./who */
	$"0D2F 7768 6F69 730D 2F77 686F 7761 730D"            /* ./whois./whowas. */
	$"2F77 6820 3D20 2F77 686F 6973 0D2F 7777"            /* /wh = /whois./ww */
	$"203D 202F 7768 6F77 6173 0D0D 2F2F 636F"            /*  = /whowas..//co */
	$"6D6D 616E 6420 646F 6573 6E27 7420 7361"            /* mmand doesn't sa */
	$"7665 2069 6E20 696E 7075 746C 696E 6520"            /* ve in inputline  */
	$"7363 726F 6C6C 6261 636B 2E20 476F 6F64"            /* scrollback. Good */
	$"2075 7365 733A 202F 2F6D 7367 7120 7840"            /*  uses: //msgq x@ */
	$"6368 616E 6E65 6C73 2E75 6E64 6572 6E65"            /* channels.underne */
	$"742E 6F72 6720 6C6F 6769 6E20 2379 6F75"            /* t.org login #you */
	$"7263 6861 6E6E 656C 2079 6F75 7270 6173"            /* rchannel yourpas */
	$"730D 0D53 686F 7274 6375 7473 2074 6F6B"            /* s..Shortcuts tok */
	$"656E 733A 0D5C 6E20 3D20 7265 7475 726E"            /* ens:.\n = return */
	$"2028 746F 2073 6570 6172 6174 6520 636F"            /*  (to separate co */
	$"6D6D 616E 6473 290D 2463 6861 6E6E 656C"            /* mmands).$channel */
	$"203D 2063 7572 7265 6E74 2074 6172 6765"            /*  = current targe */
	$"740D 2469 6C20 3D20 696E 7075 746C 696E"            /* t.$il = inputlin */
	$"6520 7405 6578 740D 246C 6173 746A 6F69"            /* e t.ext.$lastjoi */
	$"6E65 7220 3D20 6C61 7374 2070 6572 736F"            /* ner = last perso */
	$"6E20 746F 206A 6F69 6E20 6375 7272 656E"            /* n to join curren */
	$"7420 6368 616E 6E65 6C0D 246E 6963 6B20"            /* t channel.$nick  */
	$"3D20 6375 7272 656E 7420 6E69 636B 0D24"            /* = current nick.$ */
	$"7365 7276 6572 203D 2063 7572 7265 6E74"            /* server = current */
	$"2073 6572 7665 720D 2473 656C 6563 7465"            /*  server.$selecte */
	$"6475 7365 7220 3D20 6669 7273 7420 7573"            /* duser = first us */
	$"6572 2073 656C 6563 7465 6420 696E 2074"            /* er selected in t */
	$"6865 2075 7365 726C 6973 740D 2476 6572"            /* he userlist.$ver */
	$"7369 6F6E 203D 2076 6572 7369 6F6E 206F"            /* sion = version o */
	$"6620 5368 6164 6F77 4952 4320 796F 7520"            /* f ShadowIRC you  */
	$"6172 6520 7573 696E 670D 2464 6174 6520"            /* are using.$date  */
	$"3D20 6375 7272 656E 7420 6461 7465 0D24"            /* = current date.$ */
	$"6461 7465 2E6C 203D 2063 7572 7265 6E74"            /* date.l = current */
	$"2064 6174 6520 286C 6F6E 6720 666F 726D"            /*  date (long form */
	$"6174 290D 2474 696D 6520 3D20 6375 7272"            /* at).$time = curr */
	$"656E 7420 7469 6D65 0D24 7469 6D65 2E73"            /* ent time.$time.s */
	$"203D 2063 7572 7265 6E74 2074 696D 6520"            /*  = current time  */
	$"7769 7468 2073 6563 6F6E 6473 0D24 2D3E"            /* with seconds.$-> */
	$"203D 2073 6574 2074 6865 2063 7572 736F"            /*  = set the curso */
	$"7220 6F6E 2074 6865 2069 6E70 7574 6C69"            /* r on the inputli */
	$"6E65 2061 7420 7468 6973 2070 6F73 6974"            /* ne at this posit */
	$"696F 6E20 2865 2E67 2E20 222F 6374 6370"            /* ion (e.g. "/ctcp */
	$"2024 2D3E 2076 6572 7369 6F6E 2220 776F"            /*  $-> version" wo */
	$"756C 6420 6C65 7420 796F 7520 6869 7420"            /* uld let you hit  */
	$"6120 7368 6F72 7463 7574 2C20 7479 7065"            /* a shortcut, type */
	$"2069 6E20 736F 6D65 6F6E 6527 7320 6E69"            /*  in someone's ni */
	$"636B 6E61 6D65 2C20 616E 6420 6869 7420"            /* ckname, and hit  */
	$"7265 7475 726E 2074 6F20 7665 7273 696F"            /* return to versio */
	$"6E20 7468 656D 2C20 7769 7468 6F75 7420"            /* n them, without  */
	$"6861 7669 6E67 2074 6F20 706F 7369 7469"            /* having to positi */
	$"6F6E 2074 6865 2063 7572 736F 7220 6265"            /* on the cursor be */
	$"7477 6565 6E20 2F63 7463 7020 616E 6420"            /* tween /ctcp and  */
	$"7665 7273 696F 6E2E 290D 243C 2D20 3D20"            /* version.).$<- =  */
	$"656E 6420 7365 6C65 6374 696F 6E0D"                 /* end selection. */
};

data 'TEXT' (130, "Colors") {
	$"5368 6164 6F77 4952 4320 312E 320D 436F"            /* ShadowIRC 1.2.Co */
	$"6C6F 7273 2052 6566 6572 656E 6365 0D0D"            /* lors Reference.. */
	$"4D61 6369 6E74 6F73 6820 636F 6C6F 7220"            /* Macintosh color  */
	$"636F 6465 733A 0D0D 436F 6E74 726F 6C2D"            /* codes:..Control- */
	$"4320 2066 6F6C 6C6F 7765 6420 6279 2061"            /* C  followed by a */
	$"2063 6861 7261 6374 6572 3A0D 0D43 7472"            /*  character:..Ctr */
	$"6C2D 4320 310D 4374 726C 2D43 2032 0D43"            /* l-C 1.Ctrl-C 2.C */
	$"7472 6C2D 4320 330D 4374 726C 2D43 2034"            /* trl-C 3.Ctrl-C 4 */
	$"0D43 7472 6C2D 4320 350D 4374 726C 2D43"            /* .Ctrl-C 5.Ctrl-C */
	$"2036 0D43 7472 6C2D 4320 370D 4374 726C"            /*  6.Ctrl-C 7.Ctrl */
	$"2D43 2038 0D43 7472 6C2D 4320 390D 4374"            /* -C 8.Ctrl-C 9.Ct */
	$"726C 2D43 2030 2028 4374 726C 2D43 2030"            /* rl-C 0 (Ctrl-C 0 */
	$"203D 2077 6869 7465 290D 4374 726C 2D43"            /*  = white).Ctrl-C */
	$"203A 0D43 7472 6C2D 4320 3B0D 4374 726C"            /*  :.Ctrl-C ;.Ctrl */
	$"2D43 203C 0D43 7472 6C2D 4320 3D0D 4374"            /* -C <.Ctrl-C =.Ct */
	$"726C 2D43 203E 0D43 7472 6C2D 4320 3F0D"            /* rl-C >.Ctrl-C ?. */
	$"4374 726C 2D43 2040 0D43 7472 6C2D 4320"            /* Ctrl-C @.Ctrl-C  */
	$"410D 4374 726C 2D43 2042 0D43 7472 6C2D"            /* A.Ctrl-C B.Ctrl- */
	$"4320 430D 4374 726C 2D43 2044 0D43 7472"            /* C C.Ctrl-C D.Ctr */
	$"6C2D 4320 450D 4374 726C 2D43 2046 0D43"            /* l-C E.Ctrl-C F.C */
	$"7472 6C2D 4320 470D 4374 726C 2D43 2048"            /* trl-C G.Ctrl-C H */
	$"0D43 7472 6C2D 4320 490D 4374 726C 2D43"            /* .Ctrl-C I.Ctrl-C */
	$"204A 0D43 7472 6C2D 4320 4B0D 0D0D 6D49"            /*  J.Ctrl-C K...mI */
	$"5243 2063 6F6C 6F72 2063 6F64 6573 3A0D"            /* RC color codes:. */
	$"0D43 6F6E 7472 6F6C 2D43 2066 6F6C 6C6F"            /* .Control-C follo */
	$"7765 6420 6279 2061 206E 756D 6265 722C"            /* wed by a number, */
	$"2030 2D31 352C 2061 6E64 2061 6E20 6F70"            /*  0-15, and an op */
	$"7469 6F6E 616C 2063 6F6D 6D61 2061 6E64"            /* tional comma and */
	$"2062 6163 6B67 726F 756E 6420 636F 6C6F"            /*  background colo */
	$"722E 0D0D 4374 726C 2D43 2031 0D7F 4374"            /* r...Ctrl-C 1..Ct */
	$"726C 2D43 2032 0D43 7472 6C2D 4320 330D"            /* rl-C 2.Ctrl-C 3. */
	$"4374 726C 2D43 2034 0D43 7472 6C2D 4320"            /* Ctrl-C 4.Ctrl-C  */
	$"350D 4374 726C 2D43 2036 0D43 7472 6C2D"            /* 5.Ctrl-C 6.Ctrl- */
	$"4320 370D 4374 726C 2D43 2038 0D43 7472"            /* C 7.Ctrl-C 8.Ctr */
	$"6C2D 4320 390D 4374 726C 2D43 2030 2028"            /* l-C 9.Ctrl-C 0 ( */
	$"4374 726C 2D43 2030 203D 2077 6869 7465"            /* Ctrl-C 0 = white */
	$"290D 4374 726C 2D43 2031 300D 4374 726C"            /* ).Ctrl-C 10.Ctrl */
	$"2D43 2031 310D 4374 726C 2D43 2031 320D"            /* -C 11.Ctrl-C 12. */
	$"4374 726C 2D43 2031 330D 4374 726C 2D43"            /* Ctrl-C 13.Ctrl-C */
	$"2020 3134 0D43 7472 6C2D 4320 3135"                 /*   14.Ctrl-C 15 */
};

data 'TEXT' (131, "Manual") {
	$"5368 6164 6F77 4952 4320 312E 320D 4D61"            /* ShadowIRC 1.2.Ma */
	$"6E75 616C 2F52 6566 6572 656E 6365 0D0D"            /* nual/Reference.. */
	$"5468 6520 5368 6164 6F77 4952 4320 5175"            /* The ShadowIRC Qu */
	$"6963 6B20 5374 6172 7420 6775 6964 652C"            /* ick Start guide, */
	$"2061 6E20 696E 7472 6F64 7563 7469 6F6E"            /*  an introduction */
	$"2074 6F20 4952 432C 2061 6E64 2074 6865"            /*  to IRC, and the */
	$"2053 6861 646F 7749 5243 2046 4151 2061"            /*  ShadowIRC FAQ a */
	$"7265 2061 7661 696C 6162 6C65 2061 7420"            /* re available at  */
	$"3C68 7474 703A 2F2F 7777 772E 7368 6164"            /* <http://www.shad */
	$"6F77 6972 632E 636F 6D2F 7375 7070 6F72"            /* owirc.com/suppor */
	$"742E 6874 6D6C 3E2E"                                /* t.html>. */
};

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

resource 'alrx' (129) {
	versionZero {
		13,
		0,
		kUseThemeWindow,
		""
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
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....€........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 8007"            /* ÿÿÿÿÿÿÿÿÿÿÿÿ€.€. */
	$"BFF7 A027 9047 8887 8707 8207 8007 8007"            /* ¿÷ 'Gˆ‡‡.‚.€.€. */
	$"FFFF 0000 0000 0000 0008 0000 FFFF FFFF"            /* ÿÿ..........ÿÿÿÿ */
	$"FFFF 0001 CCCC CCCC CCCC 0002 AAAA AAAA"            /* ÿÿ..ÌÌÌÌÌÌ..ªªªª */
	$"AAAA 0003 CCCC CCCC FFFF 0004 9999 9999"            /* ªª..ÌÌÌÌÿÿ..™™™™ */
	$"FFFF 0005 6666 6666 CCCC 0006 BBBB BBBB"            /* ÿÿ..ffffÌÌ..»»»» */
	$"BBBB 0007 8888 8888 8888 000F 0000 0000"            /* »»..ˆˆˆˆˆˆ...... */
	$"0000 FFFF FFFF FFFF FF00 F000 0000 0000"            /* ..ÿÿÿÿÿÿÿ.ğ..... */
	$"1F00 F011 1111 1111 2F00 F0FF FFFF FFFF"            /* ..ğ...../.ğÿÿÿÿÿ */
	$"2F00 F0F3 3333 34F7 2F00 F01F 4444 5F76"            /* /.ğó334÷/.ğ.DD_v */
	$"2F00 F011 F445 F761 2F00 F011 1F5F 7611"            /* /.ğ.ôE÷a/.ğ.._v. */
	$"2F00 F011 11F7 6111 2F00 F011 1116 1111"            /* /.ğ..÷a./.ğ..... */
	$"2F00 F122 2222 2222 2F00 FFFF FFFF FFFF"            /* /.ñ"""""/.ÿÿÿÿÿÿ */
	$"FF00"                                               /* ÿ. */
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
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....€........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 8007"            /* ÿÿÿÿÿÿÿÿÿÿÿÿ€.€. */
	$"BFF7 BFE7 9FC7 8F87 8707 8207 8007 8007"            /* ¿÷¿çŸÇ‡‡.‚.€.€. */
	$"FFFF 0000 0000 0000 0008 0000 FFFF FFFF"            /* ÿÿ..........ÿÿÿÿ */
	$"FFFF 0001 CCCC CCCC CCCC 0002 AAAA AAAA"            /* ÿÿ..ÌÌÌÌÌÌ..ªªªª */
	$"AAAA 0003 CCCC CCCC FFFF 0004 9999 9999"            /* ªª..ÌÌÌÌÿÿ..™™™™ */
	$"FFFF 0005 6666 6666 CCCC 0006 BBBB BBBB"            /* ÿÿ..ffffÌÌ..»»»» */
	$"BBBB 0007 8888 8888 8888 000F 0000 0000"            /* »»..ˆˆˆˆˆˆ...... */
	$"0000 FFFF FFFF FFFF FF00 F222 2222 2222"            /* ..ÿÿÿÿÿÿÿ.ò""""" */
	$"1F00 F211 1111 1111 0F00 F2FF FFFF FFFF"            /* ..ò.......òÿÿÿÿÿ */
	$"0F00 F2F5 5555 54F7 0F00 F21F 4444 3F76"            /* ..òõUUT÷..ò.DD?v */
	$"0F00 F211 F443 F761 0F00 F211 1F3F 7611"            /* ..ò.ôC÷a..ò..?v. */
	$"0F00 F211 11F7 6111 0F00 F211 1116 1111"            /* ..ò..÷a...ò..... */
	$"0F00 F100 0000 0000 0F00 FFFF FFFF FFFF"            /* ..ñ.......ÿÿÿÿÿÿ */
	$"FF00"                                               /* ÿ. */
};

data 'cicn' (132, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....€........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 8787"            /* ÿÿÿÿÿÿÿÿÿÿÿÿ€.‡‡ */
	$"8FC7 8CC7 8187 8307 8007 8307 8307 8007"            /* ÇŒÇ‡ƒ.€.ƒ.ƒ.€. */
	$"FFFF 0000 0000 0000 0004 0000 FFFF FFFF"            /* ÿÿ..........ÿÿÿÿ */
	$"FFFF 0001 5555 5555 5555 0002 DDDD DDDD"            /* ÿÿ..UUUUUU..İİİİ */
	$"DDDD 0003 8888 8888 8888 000F 0000 0000"            /* İİ..ˆˆˆˆˆˆ...... */
	$"0000 FFFF FFFF FFFF FF00 F000 0000 0000"            /* ..ÿÿÿÿÿÿÿ.ğ..... */
	$"2F00 F022 2FFF F222 1F00 F022 FFFF FF22"            /* /.ğ"/ÿò"..ğ"ÿÿÿ" */
	$"1F00 F022 FF22 FF22 1F00 F022 223F F222"            /* ..ğ"ÿ"ÿ"..ğ""?ò" */
	$"1F00 F022 22FF 3222 1F00 F022 2222 2222"            /* ..ğ""ÿ2"..ğ""""" */
	$"1F00 F022 22FF 2222 1F00 F022 22FF 2222"            /* ..ğ""ÿ""..ğ""ÿ"" */
	$"1F00 F211 1111 1111 1F00 FFFF FFFF FFFF"            /* ..ò.......ÿÿÿÿÿÿ */
	$"FF00"                                               /* ÿ. */
};

resource 'cicn' (133, locked, preload) {
	4,
	{0, 0, 12, 14},
	2,
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF",
	$"FFFF 8007 8207 8507 8087 8107 8207 8007"
	$"8207 8007 8007 FFFF",
	{	/* array ColorSpec: 3 elements */
		/* [1] */
		21845, 21845, 21845,
		/* [2] */
		35939, 35939, 35939,
		/* [3] */
		52851, 52851, 52851
	},
	$"5555 5550 6AAA AA90 6AA0 AA90 6A80 2A90"
	$"6A8A 2A90 6AA8 2A90 6AA0 AA90 6AAA AA90"
	$"6AA0 AA90 6AAA AA90 6AAA AA90 5555 5550"
};

data 'cicn' (134, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....€........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF F037 C787"            /* ÿÿÿÿÿÿÿÿÿÿÿÿğ7Ç‡ */
	$"CFC7 CCC7 C187 C307 C007 C307 C307 8007"            /* ÏÇÌÇÁ‡Ã.À.Ã.Ã.€. */
	$"FFFF 0000 0000 0000 0004 0000 FFFF FFFF"            /* ÿÿ..........ÿÿÿÿ */
	$"FFFF 0001 5555 5555 5555 0002 DDDD DDDD"            /* ÿÿ..UUUUUU..İİİİ */
	$"DDDD 0003 8888 8888 8888 000F 0000 0000"            /* İİ..ˆˆˆˆˆˆ...... */
	$"0000 FFFF FFFF FFFF FF00 F111 3333 3311"            /* ..ÿÿÿÿÿÿÿ.ñ.333. */
	$"2F00 F122 2FFF F222 0F00 F122 FFFF FF22"            /* /.ñ"/ÿò"..ñ"ÿÿÿ" */
	$"0F00 F122 FF22 FF22 0F00 F122 223F F222"            /* ..ñ"ÿ"ÿ"..ñ""?ò" */
	$"0F00 F122 22FF 3222 0F00 F122 2222 2222"            /* ..ñ""ÿ2"..ñ""""" */
	$"0F00 F122 22FF 2222 0F00 F122 22FF 2222"            /* ..ñ""ÿ""..ñ""ÿ"" */
	$"0F00 F200 0000 0000 0F00 FFFF FFFF FFFF"            /* ..ò.......ÿÿÿÿÿÿ */
	$"FF00"                                               /* ÿ. */
};

data 'cicn' (136, locked, preload) {
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....€........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 8007 800F"            /* ÿÿÿÿÿÿÿÿÿÿÿÿ€.€. */
	$"884F 9E6F B87F 9E6F 884F 800F 800F BFFF"            /* ˆOo¸.oˆO€.€.¿ÿ */
	$"FFFF 0000 0000 0000 0005 0000 FFFF FFFF"            /* ÿÿ..........ÿÿÿÿ */
	$"FFFF 0001 5555 5555 5555 0002 DDDD DDDD"            /* ÿÿ..UUUUUU..İİİİ */
	$"DDDD 0003 EEEE 0000 0000 0004 8888 8888"            /* İİ..îî......ˆˆˆˆ */
	$"8888 000F 0000 0000 0000 FFFF FFFF FFFF"            /* ˆˆ........ÿÿÿÿÿÿ */
	$"FF00 F000 0000 0000 2F00 F022 2222 2222"            /* ÿ.ğ...../.ğ""""" */
	$"1F00 F022 4222 2422 1F00 F024 F332 2F12"            /* ..ğ"B"$"..ğ$ó2/. */
	$"1F00 F0FF F222 2FFF 1F00 F024 F332 2F12"            /* ..ğÿò"/ÿ..ğ$ó2/. */
	$"1F00 F022 4222 2422 1F00 F022 2222 2222"            /* ..ğ"B"$"..ğ""""" */
	$"1F00 F022 2222 2222 1F00 F211 1111 1111"            /* ..ğ"""""..ò..... */
	$"1F00 FFFF FFFF FFFF FF00"                           /* ..ÿÿÿÿÿÿÿ. */
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
	$"0000 0000 8008 0000 0000 000C 000E 0000"            /* ....€........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0004 0001 0004 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0002 0000 0000 000C 000E"            /* ................ */
	$"0000 0000 0002 0000 0000 000C 000E 0000"            /* ................ */
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ..ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF F7BF E19F C78F E19F F7BF FFFF FFFF"            /* ÿÿ÷¿áŸÇáŸ÷¿ÿÿÿÿ */
	$"FFFF 0000 0000 0000 0005 0000 FFFF FFFF"            /* ÿÿ..........ÿÿÿÿ */
	$"FFFF 0001 5555 5555 5555 0002 DDDD DDDD"            /* ÿÿ..UUUUUU..İİİİ */
	$"DDDD 0003 EEEE 0000 0000 0004 8888 8888"            /* İİ..îî......ˆˆˆˆ */
	$"8888 000F 0000 0000 0000 FFFF FFFF FFFF"            /* ˆˆ........ÿÿÿÿÿÿ */
	$"FF00 F111 1111 1111 2F00 F122 2222 2222"            /* ÿ.ñ...../.ñ""""" */
	$"0F00 F122 2222 2222 0F00 F122 4222 2422"            /* ..ñ"""""..ñ"B"$" */
	$"0F00 F124 F332 2F12 0F00 F1FF F222 2FFF"            /* ..ñ$ó2/...ñÿò"/ÿ */
	$"0F00 F124 F332 2F12 0F00 F122 4222 2422"            /* ..ñ$ó2/...ñ"B"$" */
	$"0F00 F122 2222 2222 0F00 F200 0000 0000"            /* ..ñ"""""..ò..... */
	$"0F00 FFFF FFFF FFFF FF00"                           /* ..ÿÿÿÿÿÿÿ. */
};

data 'ckid' (128, "Projector Data") {
	$"7818 E6E6 3FE5 3230 0004 0000 0000 0000"            /* x.ææ?å20........ */
	$"0000 B820 4385 B7E9 3C30 0000 0000 0000"            /* ..¸ C…·é<0...... */
	$"0000 0000 0000 0000 0000 0764 7368 6164"            /* ...........dshad */
	$"6F77 0000 0008 6972 632E 7273 7263 0000"            /* ow....irc.rsrc.. */
	$"0000 0000"                                          /* .... */
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

resource 'dftb' (1003) {
	versionZero {
		{	/* array FontStyle: 24 elements */
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

resource 'dftb' (10003) {
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

resource 'dlgx' (128) {
	versionZero {
		15
	}
};

resource 'dlgx' (131) {
	versionZero {
		15
	}
};

resource 'dlgx' (135) {
	versionZero {
		15
	}
};

resource 'dlgx' (137) {
	versionZero {
		15
	}
};

resource 'dlgx' (138) {
	versionZero {
		15
	}
};

resource 'dlgx' (139) {
	versionZero {
		15
	}
};

resource 'dlgx' (140) {
	versionZero {
		15
	}
};

resource 'dlgx' (141) {
	versionZero {
		15
	}
};

resource 'dlgx' (260) {
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

resource 'dlgx' (1003) {
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

resource 'dlgx' (10003) {
	versionZero {
		15
	}
};

resource 'icl4' (128) {
	$"0000 0000 0000 FFFF 0FFF E000 0000 0000"
	$"0000 0000 000E DDDD FDDD DE00 0000 0000"
	$"0000 0000 00FD DDDD DEDD DF00 0000 0000"
	$"0000 0000 00FD DDDE EEED DDF0 0000 0000"
	$"0000 0000 00FD DDDE EEEE DDF0 0000 0000"
	$"0000 0000 00E0 CCCC CCCD DDF0 0000 0000"
	$"0000 0000 00E0 CCCC CCDD DDF0 0000 0000"
	$"0000 0EFF EEDD DDDD DDDD DDDF FFFE 0000"
	$"00EF FDDD DDDD DDDE DDDE DDDD DDDD FE00"
	$"0ECD DDDD DDDD DEEE EEED DDDD DDDD DDE0"
	$"FDDD DDDD DDDE EEEE EEEE EDDD DDDD DDDF"
	$"ECDD DDDD DDDE EEEE EEEE EEDD DDDD DDCF"
	$"0EFD DDDD EEEE EEEE EEEE EEEE DDDD DCE0"
	$"000F FDEE EFFF FFFF FFFF FFFE DDDD FE00"
	$"0000 0EDE EFFA BAFF FFAB BAFE DDFE 0000"
	$"0000 00FE EAF3 BC3A F33C BAFE DF00 0000"
	$"0000 00FE EAFF BD3A FA3B BFFE DF00 0000"
	$"0000 000F EEFF FAFF FFAA FFFD DF00 0000"
	$"0000 000F EEFF FFFF FFFF FFED DE00 0000"
	$"0000 000F DEAF FFFF FFFF FFDD F000 0000"
	$"0000 0000 FEEF FFFF FFFF FEDD E000 0000"
	$"0000 0000 0FDA FFFF FFFF FDDF 0000 0000"
	$"0000 0000 0FDD FFFF FFFF ADDF 0000 0000"
	$"0000 0000 FDDD FFFF FFFF ADDF 0000 0000"
	$"0000 000F DEDD FFFF FFFF EDDD FFE0 0000"
	$"0000 0FFE EEDD EFFF FFFF ADDD DEEF FE00"
	$"00FF FAEE EEDD DAFF FFFF FDDD DEEE AEE0"
	$"FFFF FAAE EEDD DDDE FFFF FFFE DEEE AFFF"
	$"FFFF FAAE EEDD DDDA FFFF FFFF DEEE AFFE"
	$"FFFF FFEE EEED DEFF FFFF FFFA DEEE FFFE"
	$"FFFF FAAE EEDE AFFF FFFF FFFD DEEA AFFE"
	$"FFFF FFAE EEEF FFFF FFFF FFDD EEEE FFFE"
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

resource 'icl4' (131) {
	$"0FFF E000 0000 0000 0000 0000 0000 0000"
	$"FDDD DE00 0000 0000 0000 0000 0000 0000"
	$"DEDD DF00 0000 0000 000F FFFF FFFF F000"
	$"EEED DDF0 0000 0000 000F 0000 0000 FF00"
	$"EEEE DDF0 0000 0000 000F 0000 0000 FCF0"
	$"CCCD DDF0 0000 0000 000F 0000 0000 FFFF"
	$"CCDD DDF0 0000 0000 000F 0000 0000 000F"
	$"DDDD DDDF FFFE 0000 000F 0000 0000 000F"
	$"DDDE DDDD DDDD FE00 000F 00F0 FF0F F00F"
	$"EEED DDDD DDDD DDE0 000F 0000 0000 000F"
	$"EEEE EDDD DDDD DDDF 000F 00FF FF0F F00F"
	$"EEEE EEDD DDDD DDCF 000F 0000 0000 000F"
	$"EEEE EEEE DDDD DCE0 000F 00FF 0FF0 F00F"
	$"FFFF FFFE DDDD FE00 000F 0000 0000 000F"
	$"FFAB BAFE DDFE 0000 000F 0000 0000 000F"
	$"F33C BAFE DF00 0000 000F 0000 0000 000F"
	$"FA3B BFFE DF00 0000 000F 0000 0000 000F"
	$"FFAA FFFD DF00 0000 000F FFFF FFFF FFFF"
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

resource 'icl8' (128) {
	$"0000 0000 0000 0000 0000 0000 FFFF FFFF"
	$"00FF FFFF FB00 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 00FB 56F9 FAFA"
	$"FF56 81FA F9FB 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 FF56 F9F9 FA81"
	$"81FB 8181 FAFF 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 FFF9 F981 81FB"
	$"FBFC FB81 81FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 FFF9 F9FA 81FB"
	$"FCFB FCFB 81FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 FBF5 F6F6 2BF7"
	$"F7F8 F856 F9FA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 FBF5 F62B 2BF7"
	$"F7F8 5656 56FA FF00 0000 0000 0000 0000"
	$"0000 0000 00FB FFFF FBFB F9FA FAFA 8181"
	$"8181 81FA 81FA F9FF FFFF FFFB 0000 0000"
	$"0000 FBFF FFF9 F9FA FA81 FA81 8181 81FB"
	$"8181 81FB 8181 FA81 FAFA F956 FFFB 0000"
	$"00FB F856 F956 F9FA FAFA 8181 81FB FBFB"
	$"FBFB FB81 8181 81FA FAF9 F9F9 F956 FB00"
	$"FF56 5656 56F9 FAF9 8181 81FB FBFB FBFC"
	$"FBFC FBFB FB81 8181 FAFA F9F9 5656 56FF"
	$"FBF8 5656 F9FA F981 FA81 81FB FBFC FCFC"
	$"FCFC FCFC FBFB 8181 81FA FAF9 56F9 F8FF"
	$"00FB FFF9 F9F9 8181 FBFB FCFC ACFC ACAC"
	$"ACAC FCAC ACFC FCFB FAFA FAF9 FAF7 FB00"
	$"0000 00FF FFFA FCFC FBFE FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFAC F9FA FAF9 FFFB 0000"
	$"0000 0000 00FB 81FC FBEA DFDC 6489 B3EA"
	$"F4E0 DD5E 64DD FFAC F9F9 FFFB 0000 0000"
	$"0000 0000 0000 FFAC FBFD FF46 3A2D 40DD"
	$"FFDB 402D 34DC FFFC F9FF 0000 0000 0000"
	$"0000 0000 0000 FFFC FBFD FFB3 3A33 40DD"
	$"F48E 463A 5EE0 FFFB FAFF 0000 0000 0000"
	$"0000 0000 0000 00FF FBFC FFF4 E08F B3FF"
	$"FFFF DEDD E0FF F4FA FAFF 0000 0000 0000"
	$"0000 0000 0000 00FF FCFB FFFF FFFF FFFF"
	$"FFFF FFFF FFFF ACF9 F9FB 0000 0000 0000"
	$"0000 0000 0000 00FF 81FB FDFF FFFF FFFF"
	$"FFFF FFFF FFFF 81F9 FF00 0000 0000 0000"
	$"0000 0000 0000 0000 FFFB FCFF FFFF FFFF"
	$"FFFF FFFF FFAC 5656 FB00 0000 0000 0000"
	$"0000 0000 0000 0000 00FF 81FD FFFF FFFF"
	$"FFFF FFFF FFFA 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 0000 00FF 81FA F4FF FFFF"
	$"FFFF FFFF FD56 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 0000 FFFA 8181 E0FF FFFF"
	$"FFFF FFFF FD56 56FF 0000 0000 0000 0000"
	$"0000 0000 0000 00FF 81FB 8181 EAFF FFFF"
	$"FFFF FFFF AC56 F9FA FFFF FB00 0000 0000"
	$"0000 0000 00FF FFFB FCFB 81FA ACFF FFFF"
	$"FFFF FFFF FD56 F9F9 81FB FCFF FFFB 0000"
	$"0000 FFFF FFFD ACAC FCFB 81FA FAFD FFFF"
	$"FFFF FFFF EAFA FAFA FAFB FCAC FDFB FB00"
	$"FFFF FEFF FFFD FDFC FCFB 81FA F9F9 FAAC"
	$"FFFF FFFF FFFF FFFC 81FB FCAC FDFF FFFF"
	$"FFFF FFFF FFFD FDAC FBFB 8181 FA56 FAFD"
	$"FFFF FFFF FFFF FFE0 81FB FCAC FDFF FFFC"
	$"FFFF FFFF F4FE ACFC FCFB FBFA F9FC FFFF"
	$"FFFF FFFF FFFF FFFD 81FB FCAC FEFE FFFC"
	$"FFFF FFFF EAFD FDAC FCFB 81FB FDFF FFFF"
	$"FFFF FFFF FFFF FEFA 81FB FCFD FDFF FFFC"
	$"FFFF FFFF E0FE FDFC FCFB FCFF FFFF FFFF"
	$"FFFF FFFF FFFE 81FA FBFB ACAC FEFF FFAC"
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

resource 'icl8' (131) {
	$"00FF FFFF FB00 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FF56 81FA F9FB 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"81FB 8181 FAFF 0000 0000 0000 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FF00 0000"
	$"FBFC FB81 81FA FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 FFFF 0000"
	$"FCFB FCFB 81FA FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 FF2B FF00"
	$"F7F8 F856 F9FA FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 FFFF FFFF"
	$"F7F8 5656 56FA FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"8181 81FA 81FA F9FF FFFF FFFB 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"8181 81FB 8181 FA81 FAFA F956 FFFB 0000"
	$"0000 00FF F5F5 FFF5 FFFF F5FF FFF5 F5FF"
	$"FBFB FB81 8181 81FA FAF9 F9F9 F956 FB00"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FBFC FBFB FB81 8181 FAFA F9F9 5656 56FF"
	$"0000 00FF F5F5 FFFF FFFF F5FF FFF5 F5FF"
	$"FCFC FCFC FBFB 8181 81FA FAF9 56F9 F8FF"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"ACAC FCAC ACFC FCFB FAFA FAF9 FAF7 FB00"
	$"0000 00FF F5F5 FFFF F5FF FFF5 FFF5 F5FF"
	$"FFFF FFFF FFFF FFAC F9FA FAF9 FFFB 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"F4E0 DD5E 64DD FFAC F9F9 FFFB 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFDB 402D 34DC FFFC F9FF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"F48E 463A 5EE0 FFFB FAFF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFFF DEDD E0FF F4FA FAFF 0000 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
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

resource 'ics#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"07E0 0FF0 0FF0 7FFE FFFF FFFF 7FFE 1DB8"
		$"1FF8 1FF8 0FF0 0FF8 3FFE FFFF FFFF FFFF",
		/* [2] */
		$"07E0 0FF0 0FF0 7FFE FFFF FFFF 7FFE 1FF8"
		$"1FF8 1FF8 0FF0 0FF8 3FFE FFFF FFFF FFFF"
	}
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

resource 'ics#' (131) {
	{	/* array: 2 elements */
		/* [1] */
		$"E000 F078 F044 FC42 FF5A FF42 FF5A BC42"
		$"F87E F800 F800 F000 FE00 FF00 FF00 FF",
		/* [2] */
		$"E000 F078 F07C FC7E FF7E FF7E FF7E FC7E"
		$"F87E F800 F800 F000 FE00 FF00 FF00 FF"
	}
};

resource 'ics4' (128) {
	$"0000 0FFF FFF0 0000 0000 FCFF DFFF 0000"
	$"0000 FFFF FFFF 0000 0FFF FFFF FFFF FFF0"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"0FFF FFFF FFFF FFF0 000F FF3F F3FF F000"
	$"000F FFFF FFFF F000 000F FFFF FFFF F000"
	$"0000 FFFF FFFF 0000 0000 FFFF FFFF F000"
	$"00FF FFFF FFFF FFF0 FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
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

resource 'ics4' (131) {
	$"FFE0 0000 0000 0000 EEFF 0000 0FFF F000"
	$"EEDF 0000 0F00 0F00 CDDF FF00 0F00 00F0"
	$"EEDD DDFE 0F0F F0F0 EEED DDDF 0F00 00F0"
	$"FFFF DDFE 0F0F F0F0 FAAF FF00 0F00 00F0"
	$"FAFF F000 0FFF FFF0 FFFD F000 0000 0000"
	$"FFFF E000 0000 0000 FFAF 0000 0000 0000"
	$"FFAD FFF0 0000 0000 FFFF EEFF 0000 0000"
	$"FFFF EEFF 0000 0000 FFFF EAFF"
};

resource 'ics8' (128) {
	$"0000 0000 00FF FFFF FFFF FF00 0000 0000"
	$"0000 0000 FFF8 FFFF F9FF FFFF 0000 0000"
	$"0000 0000 FFFF FFFF FFFF FFFF 0000 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FF00"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FF00"
	$"0000 00FF FFFF D7FF FFD7 FFFF FF00 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FF00 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FF00 0000"
	$"0000 0000 FFFF FFFF FFFF FFFF 0000 0000"
	$"0000 0000 FFFF FFFF FFFF FFFF FF00 0000"
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FF00"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
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

resource 'ics8' (131) {
	$"FFFF FB00 0000 0000 0000 0000 0000 0000"
	$"FCFB FFFF 0000 0000 00FF FFFF FF00 0000"
	$"FCFC FAFF 0000 0000 00FF 0000 00FF 0000"
	$"F8FA FAFF FFFF 0000 00FF 0000 0000 FF00"
	$"FBFB 81FA FAF9 FFFB 00FF 00FF FF00 FF00"
	$"FCFC FB81 FAFA F9FF 00FF 0000 0000 FF00"
	$"FFFF FFFF FAFA FFFB 00FF 00FF FF00 FF00"
	$"FFDD DDFF FFFF 0000 00FF 0000 0000 FF00"
	$"FFDE FFFF FF00 0000 00FF FFFF FFFF FF00"
	$"FFFF FFF9 FF00 0000 0000 0000 0000 0000"
	$"FFFF FFFF FB00 0000 0000 0000 0000 0000"
	$"FFFF FDFF 0000 0000 0000 0000 0000 0000"
	$"FFFF FDFA FFFF FF00 0000 0000 0000 0000"
	$"FFFF FFFF FBFC FFFF 0000 0000 0000 0000"
	$"FFFF FFFF FBFC FFFF 0000 0000 0000 0000"
	$"FFFF FFFE FBFD FFFF"
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

data 'styl' (128) {
	$"0014 0000 0000 0013 000E 0003 0100 000E"            /* ................ */
	$"0000 0000 0000 0000 003A 000C 000A 0003"            /* .........:...Â.. */
	$"0100 0009 0000 0000 0000 0000 003C 000D"            /* ...Æ.........<.. */
	$"000A 0003 0000 000A 0000 0000 0000 0000"            /* .Â.....Â........ */
	$"00DF 000D 000A 0003 0400 000A 0000 0000"            /* .ß...Â.....Â.... */
	$"FFFF 0000 0103 000D 000A 0003 0000 000A"            /* ÿÿ.......Â.....Â */
	$"0000 0000 0000 0000 0106 000D 000A 0003"            /* .............Â.. */
	$"0100 000A 0000 0000 0000 0000 011A 000D"            /* ...Â............ */
	$"000A 0003 0000 000A 0000 0000 0000 0000"            /* .Â.....Â........ */
	$"0134 0010 000C 0003 0100 000C 0000 0000"            /* .4.............. */
	$"0000 0000 013B 000D 000A 0003 0000 000A"            /* .....;...Â.....Â */
	$"0000 0000 0000 0000 043A 000D 000A 0003"            /* .........:...Â.. */
	$"0100 000A 0000 0000 0000 0000 0454 000D"            /* ...Â.........T.. */
	$"000A 0003 0000 000A 0000 0000 0000 0000"            /* .Â.....Â........ */
	$"0455 000D 000A 0003 0100 000A 0000 0000"            /* .U...Â.....Â.... */
	$"0000 0000 045A 000D 000A 0003 0000 000A"            /* .....Z...Â.....Â */
	$"0000 0000 0000 0000 0472 000D 000A 0003"            /* .........r...Â.. */
	$"0100 000A 0000 0000 0000 0000 0475 000D"            /* ...Â.........u.. */
	$"000A 0003 0000 000A 0000 0000 0000 0000"            /* .Â.....Â........ */
	$"0481 000D 000A 0003 0100 000A 0000 0000"            /* ....Â.....Â.... */
	$"0000 0000 0488 000D 000A 0003 0000 000A"            /* .....ˆ...Â.....Â */
	$"0000 0000 0000 0000 04AE 000D 000A 0003"            /* .........®...Â.. */
	$"0100 000A 0000 0000 0000 0000 04BE 000D"            /* ...Â.........¾.. */
	$"000A 0003 0000 000A 0000 0000 0000 0000"            /* .Â.....Â........ */
	$"04BF 000D 000A 0004 0000 000A 0000 0000"            /* .¿...Â.....Â.... */
	$"0000"                                               /* .. */
};

data 'styl' (129) {
	$"0003 0000 0000 0013 000E 0003 0100 000E"            /* ................ */
	$"0000 0000 0000 0000 001F 0010 000C 0003"            /* ................ */
	$"0000 000C 0000 0000 0000 0000 0021 000C"            /* .............!.. */
	$"000A 0003 0000 0009 0000 0000 0000"                 /* .Â.....Æ...... */
};

data 'styl' (130, "Colors") {
	$"0032 0000 0000 0013 000E 0003 0100 000E"            /* .2.............. */
	$"0000 0000 0000 0000 001E 0010 000C 0003"            /* ................ */
	$"0000 000C 0000 0000 0000 0000 0020 000C"            /* ............. .. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .Â.....Æ........ */
	$"0066 000C 000A 0003 0000 0009 FFFF 0000"            /* .f...Â.....Æÿÿ.. */
	$"0000 0000 006F 000C 000A 0003 0000 0009"            /* .....o...Â.....Æ */
	$"FFFF 8421 0000 0000 0078 000C 000A 0003"            /* ÿÿ„!.....x...Â.. */
	$"0000 0009 FFFF FFFF 0000 0000 0081 000C"            /* ...Æÿÿÿÿ....... */
	$"000A 0003 0000 0009 8421 FFFF 0000 0000"            /* .Â.....Æ„!ÿÿ.... */
	$"008A 000C 000A 0003 0000 0009 0000 FFFF"            /* .Š...Â.....Æ..ÿÿ */
	$"0000 0000 0093 000C 000A 0003 0000 0009"            /* .....“...Â.....Æ */
	$"0000 FFFF 8421 0000 009C 000C 000A 0003"            /* ..ÿÿ„!...œ...Â.. */
	$"0000 0009 0000 FFFF FFFF 0000 00A5 000C"            /* ...Æ..ÿÿÿÿ...¥.. */
	$"000A 0003 0000 0009 0000 8421 FFFF 0000"            /* .Â.....Æ..„!ÿÿ.. */
	$"00AE 000C 000A 0003 0000 0009 FFFF FFFF"            /* .®...Â.....Æÿÿÿÿ */
	$"FFFF 0000 00B7 000C 000A 0003 0000 0009"            /* ÿÿ...·...Â.....Æ */
	$"0000 0000 0000 0000 00C9 000C 000A 0003"            /* .........É...Â.. */
	$"0000 0009 FFFF FFFF FFFF 0000 00CA 000C"            /* ...Æÿÿÿÿÿÿ...Ê.. */
	$"000A 0003 0000 0009 0000 0000 FFFF 0000"            /* .Â.....Æ....ÿÿ.. */
	$"00D3 000C 000A 0003 0000 0009 8421 0000"            /* .Ó...Â.....Æ„!.. */
	$"FFFF 0000 00DC 000C 000A 0003 0000 0009"            /* ÿÿ...Ü...Â.....Æ */
	$"FFFF 0000 FFFF 0000 00E5 000C 000A 0003"            /* ÿÿ..ÿÿ...å...Â.. */
	$"0000 0009 FFFF 0000 8421 0000 00EE 000C"            /* ...Æÿÿ..„!...î.. */
	$"000A 0003 0000 0009 AD6B AD6B AD6B 0000"            /* .Â.....Æ­k­k­k.. */
	$"00F7 000C 000A 0003 0000 0009 5294 5294"            /* .÷...Â.....ÆR”R” */
	$"5294 0000 0100 000C 000A 0003 0000 0009"            /* R”.......Â.....Æ */
	$"8421 0000 0000 0000 0109 000C 000A 0003"            /* „!.......Æ...Â.. */
	$"0000 0009 8421 4210 0000 0000 0112 000C"            /* ...Æ„!B......... */
	$"000A 0003 0000 0009 8421 8421 0000 0000"            /* .Â.....Æ„!„!.... */
	$"011B 000C 000A 0003 0000 0009 4210 8421"            /* .....Â.....ÆB.„! */
	$"0000 0000 0124 000C 000A 0003 0000 0009"            /* .....$...Â.....Æ */
	$"0000 8421 0000 0000 012D 000C 000A 0003"            /* ..„!.....-...Â.. */
	$"0000 0009 0000 8421 4210 0000 0136 000C"            /* ...Æ..„!B....6.. */
	$"000A 0003 0000 0009 0000 8421 8421 0000"            /* .Â.....Æ..„!„!.. */
	$"013F 000C 000A 0003 0000 0009 0000 4210"            /* .?...Â.....Æ..B. */
	$"8421 0000 0148 000C 000A 0003 0000 0009"            /* „!...H...Â.....Æ */
	$"0000 0000 8421 0000 0151 000C 000A 0003"            /* ....„!...Q...Â.. */
	$"0000 0009 4210 0000 8421 0000 015A 000C"            /* ...ÆB...„!...Z.. */
	$"000A 0003 0000 0009 8421 0000 8421 0000"            /* .Â.....Æ„!..„!.. */
	$"0163 000C 000A 0003 0000 0009 8421 0000"            /* .c...Â.....Æ„!.. */
	$"4210 0000 016B 000C 000A 0003 0000 0009"            /* B....k...Â.....Æ */
	$"0000 0000 0000 0000 01DE 000C 000A 0003"            /* .........Ş...Â.. */
	$"0000 0009 0000 0000 8421 0000 01E7 000C"            /* ...Æ....„!...ç.. */
	$"000A 0003 0000 0009 0000 8421 0000 0000"            /* .Â.....Æ..„!.... */
	$"01F0 000C 000A 0003 0000 0009 FFFF 0000"            /* .ğ...Â.....Æÿÿ.. */
	$"0000 0000 01F9 000C 000A 0003 0000 0009"            /* .....ù...Â.....Æ */
	$"8421 0000 0000 0000 0202 000C 000A 0003"            /* „!...........Â.. */
	$"0000 0009 8421 0000 8421 0000 020B 000C"            /* ...Æ„!..„!...... */
	$"000A 0003 0000 0009 8421 8421 0000 0000"            /* .Â.....Æ„!„!.... */
	$"0214 000C 000A 0003 0000 0009 FFFF FFFF"            /* .....Â.....Æÿÿÿÿ */
	$"0000 0000 021D 000C 000A 0003 0000 0009"            /* .........Â.....Æ */
	$"0000 FFFF 0000 0000 0226 000C 000A 0003"            /* ..ÿÿ.....&...Â.. */
	$"0000 0009 FFFF FFFF FFFF 0000 022F 000C"            /* ...Æÿÿÿÿÿÿ.../.. */
	$"000A 0003 0000 0009 0000 0000 0000 0000"            /* .Â.....Æ........ */
	$"0241 000C 000A 0003 0000 0009 FFFF FFFF"            /* .A...Â.....Æÿÿÿÿ */
	$"FFFF 0000 0242 000C 000A 0003 0000 0009"            /* ÿÿ...B...Â.....Æ */
	$"0000 8421 8421 0000 024C 000C 000A 0003"            /* ..„!„!...L...Â.. */
	$"0000 0009 0000 FFFF FFFF 0000 0256 000C"            /* ...Æ..ÿÿÿÿ...V.. */
	$"000A 0003 0000 0009 0000 0000 FFFF 0000"            /* .Â.....Æ....ÿÿ.. */
	$"0260 000C 000A 0003 0000 0009 FFFF 0000"            /* .`...Â.....Æÿÿ.. */
	$"FFFF 0000 026A 000C 000A 0003 0000 0009"            /* ÿÿ...j...Â.....Æ */
	$"8421 8421 8421 0000 0275 000C 000A 0003"            /* „!„!„!...u...Â.. */
	$"0000 0009 C631 C631 C631"                           /* ...ÆÆ1Æ1Æ1 */
};

data 'styl' (131, "Manual") {
	$"0004 0000 0000 0013 000E 0003 0100 000E"            /* ................ */
	$"0000 0000 0000 0000 0020 0010 000C 0003"            /* ......... ...... */
	$"0000 000C 0000 0000 0000 0000 0081 0010"            /* ............... */
	$"000C 0003 0000 000C 0000 0000 FFFF 0000"            /* ............ÿÿ.. */
	$"00A6 0010 000C 0003 0000 000C 0000 0000"            /* .¦.............. */
	$"0000"                                               /* .. */
};

data 'PICT' (128, purgeable) {
	$"3D14 0000 0000 0090 0168 0011 02FF 0C00"            /* =.......h...ÿ.. */
	$"FFFE 0000 0048 0000 0048 0000 0000 0000"            /* ÿş...H...H...... */
	$"0090 0168 0000 0000 00A1 01F2 0016 3842"            /* ..h.....¡.ò..8B */
	$"494D 0000 0000 0000 0090 0168 4772 8970"            /* IM........hGr‰p */
	$"68AF 626A 00A1 00E0 0218 0000 0000 0000"            /* h¯bj.¡.à........ */
	$"0214 4144 4245 0210 0000 6D6E 7472 5247"            /* ..ADBE....mntrRG */
	$"4220 5859 5A20 07CE 000A 0019 0012 0039"            /* B XYZ .Î.Â.....9 */
	$"0016 6163 7370 4150 504C 0000 0000 6E6F"            /* ..acspAPPL....no */
	$"6E65 0000 0000 0000 0000 0000 0000 0000"            /* ne.............. */
	$"0000 0000 F6D6 0001 0000 0000 D32D 4144"            /* ....öÖ......Ó-AD */
	$"4245 0000 0000 0000 0000 0000 0000 0000"            /* BE.............. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000A 6370 7274 0000 00FC 0000 0024 6465"            /* .Âcprt...ü...$de */
	$"7363 0000 0120 0000 0060 7774 7074 0000"            /* sc... ...`wtpt.. */
	$"0180 0000 0014 626B 7074 0000 0194 0000"            /* .€....bkpt...”.. */
	$"0014 7254 5243 0000 01A8 0000 000E 6754"            /* ..rTRC...¨....gT */
	$"5243 0000 01B8 0000 000E 6254 5243 0000"            /* RC...¸....bTRC.. */
	$"01C8 0000 000E 7258 595A 0000 01D8 0000"            /* .È....rXYZ...Ø.. */
	$"0014 6758 595A 0000 01EC 0000 0014 6258"            /* ..gXYZ...ì....bX */
	$"595A 0000 0200 0000 0014 7465 7874 0000"            /* YZ........text.. */
	$"0000 2863 2920 3139 3938 2041 646F 6265"            /* ..(c) 1998 Adobe */
	$"2053 7973 7465 6D73 2049 6E63 2E00 6465"            /*  Systems Inc..de */
	$"7363 0000 0000 0000 0005 626C 6168 0000"            /* sc........blah.. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 5859"            /* ..............XY */
	$"5A20 0000 0000 0000 F31D 0001 0000 0001"            /* Z ......ó....... */
	$"67F2 5859 5A20 0000 0000 0000 0000 0000"            /* gòXYZ .......... */
	$"0000 0000 0000 6375 7276 0000 0000 0000"            /* ......curv...... */
	$"0001 01CD 0000 6375 7276 0000 0000 0000"            /* ...Í..curv...... */
	$"0001 01CD 0000 6375 7276 0000 0000 0000"            /* ...Í..curv...... */
	$"0001 01CD 0000 5859 5A20 0000 0000 0000"            /* ...Í..XYZ ...... */
	$"6D29 0000 39E2 0000 02F9 5859 5A20 0000"            /* m)..9â...ùXYZ .. */
	$"0000 0000 5E9A 0000 AFBF 0000 185C 5859"            /* ....^š..¯¿...\XY */
	$"5A20 0000 0000 0000 2B12 0000 165F 0000"            /* Z ......+...._.. */
	$"B7D7 00A1 00E0 0004 0000 0002 0001 000A"            /* ·×.¡.à.........Â */
	$"0000 0000 0090 0168 8200 0000 3886 0000"            /* ......h‚...8†.. */
	$"0001 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0001 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"4000 0000 0000 0000 0000 0000 0000 0000"            /* @............... */
	$"0040 0000 0000 0090 0168 0000 0300 0000"            /* .@......h...... */
	$"0000 0000 0056 6A70 6567 0000 0000 0000"            /* .....Vjpeg...... */
	$"0000 0001 0001 6170 706C 0000 0000 0000"            /* ......appl...... */
	$"0300 0168 0090 0048 0000 0048 0000 0000"            /* ...h..H...H.... */
	$"37EB 0001 0C50 686F 746F 202D 204A 5045"            /* 7ë...Photo - JPE */
	$"4700 0000 0000 0000 0000 0000 0000 0000"            /* G............... */
	$"0000 0000 0018 FFFF FFD8 FFE0 0010 4A46"            /* ......ÿÿÿØÿà..JF */
	$"4946 0001 0101 0048 0048 0000 FFFE 000C"            /* IF.....H.H..ÿş.. */
	$"4170 706C 654D 6172 6B0A FFDB 0084 0003"            /* AppleMarkÂÿÛ.„.. */
	$"0202 0202 0203 0202 0203 0303 0304 0604"            /* ................ */
	$"0404 0404 0705 0605 0609 0809 0908 0808"            /* .........Æ.ÆÆ... */
	$"0809 0A0D 0B09 0A0D 0A08 080C 100C 0D0E"            /* .ÆÂ..ÆÂ.Â....... */
	$"0E0F 0F0F 090B 1011 100E 110D 0E0F 0E01"            /* ....Æ........... */
	$"0303 0304 0304 0704 0407 0E0A 080A 0E0E"            /* ...........Â.Â.. */
	$"0E0E 0E0E 0E0E 0E0E 0E0E 0E0E 0E0E 0E0E"            /* ................ */
	$"0E0E 0E0E 0E0E 0E0E 0E0E 0E0E 0E0E 0E0E"            /* ................ */
	$"0E0E 0E0E 0E0E 0E0E 0E0E 0E0E 0E0E 0E0E"            /* ................ */
	$"FFC4 01A2 0000 0105 0101 0101 0101 0000"            /* ÿÄ.¢............ */
	$"0000 0000 0000 0102 0304 0506 0708 090A"            /* ..............ÆÂ */
	$"0B01 0003 0101 0101 0101 0101 0100 0000"            /* ................ */
	$"0000 0001 0203 0405 0607 0809 0A0B 1000"            /* ...........ÆÂ... */
	$"0201 0303 0204 0305 0504 0400 0001 7D01"            /* ..............}. */
	$"0203 0004 1105 1221 3141 0613 5161 0722"            /* .......!1A..Qa." */
	$"7114 3281 91A1 0823 42B1 C115 52D1 F024"            /* q.2‘¡.#B±Á.RÑğ$ */
	$"3362 7282 090A 1617 1819 1A25 2627 2829"            /* 3br‚ÆÂ.....%&'() */
	$"2A34 3536 3738 393A 4344 4546 4748 494A"            /* *456789:CDEFGHIJ */
	$"5354 5556 5758 595A 6364 6566 6768 696A"            /* STUVWXYZcdefghij */
	$"7374 7576 7778 797A 8384 8586 8788 898A"            /* stuvwxyzƒ„…†‡ˆ‰Š */
	$"9293 9495 9697 9899 9AA2 A3A4 A5A6 A7A8"            /* ’“”•–—˜™š¢£¤¥¦§¨ */
	$"A9AA B2B3 B4B5 B6B7 B8B9 BAC2 C3C4 C5C6"            /* ©ª²³´µ¶·¸¹ºÂÃÄÅÆ */
	$"C7C8 C9CA D2D3 D4D5 D6D7 D8D9 DAE1 E2E3"            /* ÇÈÉÊÒÓÔÕÖ×ØÙÚáâã */
	$"E4E5 E6E7 E8E9 EAF1 F2F3 F4F5 F6F7 F8F9"            /* äåæçèéêñòóôõö÷øù */
	$"FA11 0002 0102 0404 0304 0705 0404 0001"            /* ú............... */
	$"0277 0001 0203 1104 0521 3106 1241 5107"            /* .w.......!1..AQ. */
	$"6171 1322 3281 0814 4291 A1B1 C109 2333"            /* aq."2..B‘¡±ÁÆ#3 */
	$"52F0 1562 72D1 0A16 2434 E125 F117 1819"            /* Rğ.brÑÂ.$4á%ñ... */
	$"1A26 2728 292A 3536 3738 393A 4344 4546"            /* .&'()*56789:CDEF */
	$"4748 494A 5354 5556 5758 595A 6364 6566"            /* GHIJSTUVWXYZcdef */
	$"6768 696A 7374 7576 7778 797A 8283 8485"            /* ghijstuvwxyz‚ƒ„… */
	$"8687 8889 8A92 9394 9596 9798 999A A2A3"            /* †‡ˆ‰Š’“”•–—˜™š¢£ */
	$"A4A5 A6A7 A8A9 AAB2 B3B4 B5B6 B7B8 B9BA"            /* ¤¥¦§¨©ª²³´µ¶·¸¹º */
	$"C2C3 C4C5 C6C7 C8C9 CAD2 D3D4 D5D6 D7D8"            /* ÂÃÄÅÆÇÈÉÊÒÓÔÕÖ×Ø */
	$"D9DA E2E3 E4E5 E6E7 E8E9 EAF2 F3F4 F5F6"            /* ÙÚâãäåæçèéêòóôõö */
	$"F7F8 F9FA FFC0 0011 0800 9001 6803 0121"            /* ÷øùúÿÀ.....h..! */
	$"0002 1101 0311 01FF DA00 0C03 0100 0211"            /* .......ÿÚ....... */
	$"0311 003F 00E7 7F64 CFD9 0BE0 B7C5 DF82"            /* ...?.ç.dÏÙ.à·Åß‚ */
	$"1A1F 8C7C 57E1 A89F 5295 7C99 648D 1479"            /* ..Œ|Wá¨ŸR•|™d.y */
	$"9B55 0866 C8FB DF37 27BE 3D73 5EC7 FF00"            /* ›U.fÈûß7'¾=s^Çÿ. */
	$"0EF3 FD9C FF00 E858 FD13 FF00 89A0 03FE"            /* .óıœÿ.èXı.ÿ.‰ .ş */
	$"1DE7 FB39 FF00 D0B1 FA27 FF00 1347 FC3B"            /* .çû9ÿ.Ğ±ú'ÿ..Gü; */
	$"CFF6 73FF 00A1 63F4 4FFE 2680 192F FC13"            /* Ïösÿ.¡côOş&€./ü. */
	$"DFF6 7189 7737 863F 44FF 00E2 6B91 F127"            /* ßöq‰w7†?Dÿ.âk‘ñ' */
	$"EC65 FB34 6868 C5BC 38A0 8CF5 09FE 1401"            /* ìeû4hhÅ¼8 ŒõÆş.. */
	$"E55A D7C0 8FD9 C2C2 5648 BC3B 1719 EC9F"            /* åZ×ÀÙÂÂVH¼;..ìŸ */
	$"E153 693F B35F C16D 4905 C2F8 0E63 09C1"            /* áSi?³_ÁmI.Âø.cÆÁ */
	$"0E62 0A18 1EE3 2BC8 FA50 0755 6BFB 2A7E"            /* .bÂ..ã+ÈúP.Ukû*~ */
	$"CE4D 06FB 8F0A 8460 0E41 0991 F863 38AE"            /* ÎM.ûÂ„`.AÆ‘øc8® */
	$"675E FD9F FF00 66ED 3988 8BC3 B171 FEE7"            /* g^ıŸÿ.fí9ˆ‹Ã±qşç */
	$"F850 052D 23F6 7CF8 11A9 9124 5E0F 2F0F"            /* øP.-#ö|ø.©‘$^./. */
	$"3F38 450A 7070 7048 E4FD 2BD1 FC2F FB20"            /* ?8EÂpppHäı+Ñü/û  */
	$"FECD 7AC9 48E7 F0EC 6923 71B0 EC0C 7F0C"            /* şÍzÉHçğìi#q°ì... */
	$"6680 3BE8 3FE0 9F3F B38C E81D 7C31 D7D9"            /* f€;è?àŸ?³Œè.|1×Ù */
	$"3FF8 9A93 FE1D E7FB 39FF 00D0 B1FA 27FF"            /* ?øš“ş.çû9ÿ.Ğ±ú'ÿ */
	$"0013 4007 FC3B CFF6 73FF 00A1 63F4 4FFE"            /* ..@.ü;Ïösÿ.¡côOş */
	$"268F F877 9FEC E7FF 0042 C7E8 9FFC 4D00"            /* &øwŸìçÿ.BÇèŸüM. */
	$"1FF0 EF3F D9CF FE85 8FD1 3FF8 9A3F E1DE"            /* .ğï?ÙÏş…Ñ?øš?áŞ */
	$"7FB3 9FFD 0B1F A27F F134 007F C3BC FF00"            /* .³Ÿı..¢.ñ4..Ã¼ÿ. */
	$"673F FA16 3F44 FF00 E268 FF00 8779 FECE"            /* g?ú.?Dÿ.âhÿ.‡yşÎ */
	$"7FF4 2C7E 89FF 00C4 D001 FF00 0EF3 FD9C"            /* .ô,~‰ÿ.ÄĞ.ÿ..óıœ */
	$"FF00 E858 FD13 FF00 89A3 FE1D E7FB 39FF"            /* ÿ.èXı.ÿ.‰£ş.çû9ÿ */
	$"00D0 B1FA 27FF 0013 4007 FC3B CFF6 73FF"            /* .Ğ±ú'ÿ..@.ü;Ïösÿ */
	$"00A1 63F4 4FFE 268F F877 9FEC E7FF 0042"            /* .¡côOş&øwŸìçÿ.B */
	$"C7E8 9FFC 4D00 1FF0 EF3F D9CF FE85 8FD1"            /* ÇèŸüM..ğï?ÙÏş…Ñ */
	$"3FF8 9A3F E1DE 7FB3 9FFD 0B1F A27F F134"            /* ?øš?áŞ.³Ÿı..¢.ñ4 */
	$"007F C3BC FF00 673F FA16 3F44 FF00 E268"            /* ..Ã¼ÿ.g?ú.?Dÿ.âh */
	$"FF00 8779 FECE 7FF4 2C7E 89FF 00C4 D001"            /* ÿ.‡yşÎ.ô,~‰ÿ.ÄĞ. */
	$"FF00 0EF3 FD9C FF00 E858 FD13 FF00 89A3"            /* ÿ..óıœÿ.èXı.ÿ.‰£ */
	$"FE1D E7FB 39FF 00D0 B1FA 27FF 0013 4007"            /* ş.çû9ÿ.Ğ±ú'ÿ..@. */
	$"FC3B CFF6 73FF 00A1 63F4 4FFE 268F F877"            /* ü;Ïösÿ.¡côOş&øw */
	$"9FEC E7FF 0042 C7E8 9FFC 4D00 1FF0 EF3F"            /* Ÿìçÿ.BÇèŸüM..ğï? */
	$"D9CF FE85 8FD1 3FF8 9A3F E1DE 7FB3 9FFD"            /* ÙÏş…Ñ?øš?áŞ.³Ÿı */
	$"0B1F A27F F134 007F C3BC FF00 673F FA16"            /* ..¢.ñ4..Ã¼ÿ.g?ú. */
	$"3F44 FF00 E268 FF00 8779 FECE 7FF4 2C7E"            /* ?Dÿ.âhÿ.‡yşÎ.ô,~ */
	$"89FF 00C4 D001 FF00 0EF3 FD9C FF00 E858"            /* ‰ÿ.ÄĞ.ÿ..óıœÿ.èX */
	$"FD13 FF00 89A3 FE1D E7FB 39FF 00D0 B1FA"            /* ı.ÿ.‰£ş.çû9ÿ.Ğ±ú */
	$"27FF 0013 4007 FC3B CFF6 73FF 00A1 63F4"            /* 'ÿ..@.ü;Ïösÿ.¡cô */
	$"4FFE 268F F877 9FEC E7FF 0042 C7E8 9FFC"            /* Oş&øwŸìçÿ.BÇèŸü */
	$"4D00 1FF0 EF3F D9CF FE85 8FD1 3FF8 9A3F"            /* M..ğï?ÙÏş…Ñ?øš? */
	$"E1DE 7FB3 9FFD 0B1F A27F F134 007F C3BC"            /* áŞ.³Ÿı..¢.ñ4..Ã¼ */
	$"FF00 673F FA16 3F44 FF00 E268 FF00 8779"            /* ÿ.g?ú.?Dÿ.âhÿ.‡y */
	$"FECE 7FF4 2C7E 89FF 00C4 D007 8E7E D67F"            /* şÎ.ô,~‰ÿ.ÄĞ.~Ö. */
	$"B217 C16F 845F 0435 CF18 F853 C351 26A5"            /* ².Áo„_.5Ï.øSÃQ&¥ */
	$"12F9 3149 2229 F2F7 2B1D CB81 F7BE 5E0F"            /* .ù1I")ò÷+.Ë÷¾^. */
	$"6CFA D7E6 6500 7ECE 7FC1 3F3F E4D9 F40F"            /* lú×æe.~Î.Á??äÙô. */
	$"FAE8 FF00 FA2E 3AFA 4680 0A28 0398 F17E"            /* úèÿ.ú.:úF€Â(.˜ñ~ */
	$"B6BA 658C 9217 C601 AF8D 7E31 FC52 B837"            /* ¶ºeŒ’.Æ.¯~1üR¸7 */
	$"12C1 0CE7 A91C 1A00 D8F8 67F0 DA2D 3B4D"            /* .Á.ç©...ØøgğÚ-;M */
	$"8BC5 9E32 845C EA97 6A25 86DA 5195 B553"            /* ‹Å2„\ê—j%†ÚQ•µS */
	$"C8CA 9E0B 9E0E 7B74 1DEB AAD5 75A5 8C15"            /* ÈÊ..{t.ëªÕu¥Œ. */
	$"5200 1D00 E00A 00E3 F52F 1105 CE5E B88D"            /* R...àÂ.ãõ/..Î^¸ */
	$"662D 3355 D462 D42E 6493 F767 2F10 3849"            /* f-3UÔbÔ.d“÷g/.8I */
	$"7D37 0EFF 00D7 BE68 036A D3C4 9144 1638"            /* }7.ÿ.×¾h.jÓÄ‘D.8 */
	$"F08A 0600 1C00 2BA1 D37C 441C 8C49 9A00"            /* ğŠ....+¡Ó|D.ŒIš. */
	$"FA03 E0BF 8F25 D5BC CF0E EA13 9925 8A3F"            /* ú.à¿%Õ¼Ï.ê.™%Š? */
	$"36D9 D8E4 9404 6E42 7A9C 6411 ED9E C057"            /* 6ÙØä”.nBzœd.íÀW */
	$"ADD0 0145 0014 5001 4500 1450 0145 0014"            /* ­Ğ.E..P.E..P.E.. */
	$"5001 4500 1450 0145 0014 5001 4500 1450"            /* P.E..P.E..P.E..P */
	$"0145 0014 5001 4500 7CDD FF00 0502 FF00"            /* .E..P.E.|İÿ...ÿ. */
	$"9367 D7FF 00EB A2FF 00E8 B92B F18E 803F"            /* “g×ÿ.ë¢ÿ.è¹+ñ€? */
	$"673F E09F 9FF2 6CFA 07FD 747F FD17 1D7D"            /* g?àŸŸòlú.ıt.ı..} */
	$"2340 0523 1C29 A00F 1FF8 C7A8 4B6F A6CD"            /* #@.#.) ..øÇ¨Ko¦Í */
	$"B588 F94D 7C75 E1AD 217C 67F1 6F4F D3EF"            /* µˆùM|uá­!|gñoOÓï */
	$"D3CD B4B7 91EE E752 7AAC 632A 083D 46FD"            /* ÓÍ´·‘îçRz¬c*.=Fı */
	$"808F 4CD0 07D0 7AF6 A3B4 360F 0381 5E6F"            /* €LĞ.Ğzö£´6..^o */
	$"AF6B 0537 1DD4 01E7 BABF 880A 963E 67EB"            /* ¯k.7.Ô.çº¿ˆÂ–>gë */
	$"5CBD CF89 B6B1 1E67 EB40 0EB4 F136 E603"            /* \½Ï‰¶±.gë@.´ñ6æ. */
	$"CCFD 6BAA D1F5 F259 7E7F D680 3D77 E177"            /* ÌıkªÑõòY~.Ö€=wáw */
	$"8A5B 4CF1 5E8F 7DE6 602D D246 E73C 6C73"            /* Š[Lñ^}æ`-ÒFç<ls */
	$"B1FF 00F1 D635 F65A 9CA8 3400 B450 0145"            /* ±ÿ.ñÖ5öZœ¨4.´P.E */
	$"0014 5001 4500 1450 0145 0014 5001 4500"            /* ..P.E..P.E..P.E. */
	$"1450 0145 0014 5001 4500 1450 0145 0014"            /* .P.E..P.E..P.E.. */
	$"5007 CDDF F050 2FF9 367D 7FFE BA2F FE8B"            /* P.ÍßğP/ù6}.şº/ş‹ */
	$"92BF 18E8 03F6 73FE 09FA 40FD 99F4 0CFF"            /* ’¿.è.ösşÆú@ı™ô.ÿ */
	$"00CF 47FF 00D1 71D7 D1CD 2A2F 5614 010B"            /* .ÏGÿ.Ñq×ÑÍ*.V... */
	$"EA16 F1F5 907E 7552 7D76 C914 E655 FCE8"            /* ê.ñõ~uR}vÉ.æUüè */
	$"03C6 3E31 EB56 53E9 D305 954F CA7B D794"            /* .Æ>1ëVSéÓ.•OÊ{×” */
	$"7EC7 3E1C D17C 65FB 426A 3A26 B50B CD66"            /* ~Ç>.Ñ|eûBj:&µ.Íf */
	$"FA0D DCA5 6390 C6DB 9668 31F3 0E7B 9A00"            /* ú.Ü¥cÆÛ–h1ó.{š. */
	$"FB57 56FD 9C3E 154D B83E 917D FF00 81F2"            /* ûWVıœ>.M¸>‘}ÿ.ò */
	$"FF00 8D71 5AAF ECC7 F075 C112 787A E9FD"            /* ÿ.qZ¯ìÇğuÁ.xzéı */
	$"DB50 9F3F A350 0713 AB7E CABF 04DF 3BBC"            /* ÛPŸ?£P..«~Ê¿.ß;¼ */
	$"3173 DFFE 6237 1FFC 5571 BA9F ECA5 F049"            /* 1sßşb7.üUqºŸì¥ğI */
	$"4923 C317 439F FA09 5C7F F174 01CE DD7E"            /* I#Ã.CŸúÆ\.ñt.Îİ~ */
	$"CDBF 08EC B26D FC3D 72A4 7ADF CE7F 9B56"            /* Í¿.ì²mü=r¤zßÎ.›V */
	$"6DC7 C28F 02E9 A316 9A2B 26DE 99B9 94FF"            /* mÇÂ.é£.š+&Ş™¹”ÿ */
	$"0036 A00C C9F4 9B1D 2B9B 084C 457E EFCE"            /* .6 .Éô›.+›.LE~ïÎ */
	$"C71F 99AF AEFC 27E2 A8F5 6F0F 6997 F733"            /* Ç.™¯®ü'â¨õo.i—÷3 */
	$"AB4F 7169 14B2 1036 E58A 824E 3B73 401B"            /* «Oqi.².6åŠ‚N;s@. */
	$"A97F 6F27 DD90 54CB 2A37 4228 01F9 CF4A"            /* ©.o'İTË*7B(.ùÏJ */
	$"2800 A280 0A28 00A2 800A 2800 A280 0A28"            /* (.¢€Â(.¢€Â(.¢€Â( */
	$"00A2 800A 2800 A280 0A28 00A2 800A 2800"            /* .¢€Â(.¢€Â(.¢€Â(. */
	$"A280 3E6E FF00 8281 7FC9 B3EB FF00 F5D1"            /* ¢€>nÿ.‚.É³ëÿ.õÑ */
	$"7FF4 5C95 F8C7 401F AD9F B137 8E2C B40F"            /* .ô\•øÇ@.­Ÿ±7,´. */
	$"D9BF 43B7 9A65 5657 6382 7FD8 8EBD 1F5C"            /* Ù¿C·šeVWc‚.Ø½.\ */
	$"F8E7 616B B82D CAF1 EF40 1C3D F7C7 B9AE"            /* øçak¸-Êñï@.=÷Ç¹® */
	$"E468 6C04 B3BF 5DB1 2973 8FA0 ACE9 BC7D"            /* ähl.³¿]±)s ¬é¼} */
	$"E3AB FC79 3A55 C46A C48C CCC2 3C63 D413"            /* ã«üy:UÄjÄŒÌÂ<cÔ. */
	$"91F9 5007 2FAF C5E3 0D75 3CBB ABEB 5855"            /* ‘ùP./¯Åã.u<»«ëXU */
	$"C1DD F333 B2FD 4600 3F81 AB7F 0035 1BCF"            /* Áİó3²ıF.?«..5.Ï */
	$"809F 116E 7E22 CA21 D6E6 974E 9F4F 5B4C"            /* €Ÿ.n~"Ê!Öæ—NŸO[L */
	$"9814 091E 36DE 5F93 91E5 E36E DEF9 CF6A"            /* ˜.Æ.6Ş_“‘åãnŞùÏj */
	$"00F7 6D57 F6DF F12C E5BE CBE0 6D22 1CF4"            /* .÷mWößñ,å¾Ëàm".ô */
	$"DF75 34BF C82F BD72 7A87 ED8B E3AB 8276"            /* ßu4¿È/½rz‡í‹ã«‚v */
	$"681E 1F88 67B4 73B1 C7E3 2500 73F7 7FB5"            /* h..ˆg´s±Çã%.s÷.µ */
	$"478E EE18 9363 A2A8 3D85 BBFF 0057 ACA9"            /* Gî.“c¢¨=…»ÿ.W¬© */
	$"FF00 68CF 185C 93E6 5B69 5CFA 40C3 FF00"            /* ÿ.hÏ.\“æ[i\ú@Ãÿ. */
	$"67A0 0A72 7C6D F105 C8C4 B69A 7FB9 11B8"            /* g Âr|mñ.ÈÄ¶š.¹.¸ */
	$"FF00 D9AA BBFC 47BE BC3F BDB2 B5E7 FBBB"            /* ÿ.Ùª»üG¾¼?½²µçû» */
	$"87F3 2680 2B49 AD2D FF00 DF8D 509F 439A"            /* ‡ó&€+I­-ÿ.ßPŸCš */
	$"EAB4 7F1E 6AB6 56F6 F6B1 5C15 8ADE 08E1"            /* ê´..j¶Vöö±\.ŠŞ.á */
	$"4006 49D8 817D 47A5 0059 BFF8 F973 E1D8"            /* @.IØ}G¥.Y¿øùsáØ */
	$"D5EE E78C 9C12 46E3 C7E6 3F96 6A3D 13F6"            /* ÕîçŒœ.FãÇæ?–j=.ö */
	$"B47B EBA5 B7B7 D3EE 2E5B A958 2332 9C7D"            /* ´{ë¥··Óî.[©X#2œ} */
	$"1734 01EC FE0A F8CD A7F8 88A4 1731 CB6D"            /* .4.ìşÂøÍ§øˆ¤.1Ëm */
	$"33F4 4991 A33F 9301 5D5F 88FC 71A7 787A"            /* 3ôI‘£?“.]_ˆüq§xz */
	$"C0DF 5C48 0281 9CE6 8032 BC1B F157 48F1"            /* Àß\H.œæ€2¼.ñWHñ */
	$"7CC6 1B29 5588 38EB 5DD8 607A 1A00 5A32"            /* |Æ.)Uˆ8ë]Ø`z..Z2 */
	$"3D45 0026 47A8 A5A0 0320 7535 C7F8 D3E2"            /* =E.&G¨¥ . u5ÇøÓâ */
	$"2699 E0E8 FCCB D902 8F73 4012 782F C7FA"            /* &™àèüËÙ.s@.x/Çú */
	$"778C 60F3 ACA4 0C3D 8D75 9400 991E A296"            /* wŒ`ó¬¤.=u”.™.¢– */
	$"800C 8A28 00A3 2280 20BC B94B 4B76 9DCF"            /* €.Š(.£"€ ¼¹KKvÏ */
	$"0A09 AF32 BCF8 EFE1 FB4D 67FB 1DE7 5126"            /* ÂÆ¯2¼øïáûMgû.çQ& */
	$"EDB8 CD00 777F F091 DAFF 0064 7F6B 6E1E"            /* í¸Í.w.ğ‘Úÿ.d.kn. */
	$"5EDD D5E5 FAA7 ED27 E19D 36ED ECE4 B84D"            /* ^İÕåú§í'á6íìä¸M */
	$"E871 D680 2183 F69C F0B4 8E15 A755 CF7C"            /* èqÖ€!ƒöœğ´.§UÏ| */
	$"D7A4 784F C79A 478A E012 D8DC 23E7 D0E6"            /* ×¤xOÇšGŠà.ØÜ#çĞæ */
	$"8036 355D 62CF 48B6 6B9B A955 1146 724D"            /* €65]bÏH¶k›©U.FrM */
	$"791F 88BF 693F 0CE9 172D 6C93 A315 2470"            /* y.ˆ¿i?.é.-l“£.$p */
	$"6802 3F0B 7ED4 7E04 D7EF A3D3 BFB7 6C56"            /* h.?.~Ô~.×ï£Ó¿·lV */
	$"791B 62C7 F684 DC4F A019 CE6B D9AC 2FE0"            /* y.bÇö„ÜO .ÎkÙ¬/à */
	$"D420 5B8B 770C AC32 08A0 0F9D FF00 E0A0"            /* Ô [‹w.¬2. .ÿ.à  */
	$"5FF2 6CFA FF00 FD74 5FFD 1725 7E31 D007"            /* _òlúÿ.ıt_ı.%~1Ğ. */
	$"DD9F B2E6 89E3 CF18 7C35 D3B4 ED15 0DA6"            /* İŸ²æ‰ãÏ.|5Ó´í..¦ */
	$"990B 307B E9F2 23C8 0A08 41FC 6DEC 38F5"            /* ™.0{éò#ÈÂ.Aümì8õ */
	$"22BE 85D3 FE10 F86F 4B0B 36AD 34FA BDC8"            /* "¾…Óş.øoK.6­4ú½È */
	$"E59A E1B1 1E78 E918 E31F 5CD0 06AB 4163"            /* åšá±.xé.ã.\Ğ.«Ac */
	$"631F 9365 690C 11AF 458D 028E 98ED EC05"            /* c.“ei..¯E.˜íì. */
	$"63DF 5DF5 00D0 0605 EDEE 33CD 605E 5FE3"            /* cß]õ.Ğ..íî3Í`^_ã */
	$"3F35 0062 DD6A 18CF CD59 53EA 3FED 5005"            /* ?5.bİj.ÏÍYSê?íP. */
	$"27D4 FF00 DAA8 FF00 B4CE 7EF1 A009 63D4"            /* 'Ôÿ.Ú¨ÿ.´Î~ñ ÆcÔ */
	$"B9FB D57A DF52 E986 A00D 6B5D 4338 F9AB"            /* ¹ûÕzßRé† .k]C8ù« */
	$"66DA FCED FBDD A803 CEB5 6D0A FBC7 9F13"            /* fÚüíûİ¨.ÎµmÂûÇŸ. */
	$"3C35 E0B9 6F1E D6D7 5BD5 ADAC 6598 1E51"            /* <5à¹o.Ö×[Õ­¬e˜.Q */
	$"2491 5588 FF00 6B04 E077 38AF BE7C 586C"            /* $‘Uˆÿ.k.àw8¯¾|Xl */
	$"7E03 FC36 C7C3 6F87 4D7F 0D83 4712 69DA"            /* ~.ü6ÇÃo‡M..ƒG.iÚ */
	$"7A10 D83C 34D2 1556 77C6 32CD 82C4 9C92"            /* z.Ø<4Ò.VwÆ2Í‚Äœ’ */
	$"3920 0303 C1BF 1ABC 1DF1 0FC3 893F 8812"            /* 9 ..Á¿.¼.ñ.Ã‰?ˆ. */
	$"DF48 D47C D686 E74B B993 7C91 B8E4 32E5"            /* ßHÔ|Ö†çK¹“|‘¸ä2å */
	$"4360 8208 6C0C 1C8E D9AF 2FF8 E7E2 75B5"            /* C`‚.l..Ù¯/øçâuµ */
	$"D0AE AC60 B932 5BAE E10C CA73 95ED 9FF1"            /* Ğ®¬`¹2[®á.Ês•íŸñ */
	$"A00E 9BF6 76B9 6BBF 82DE 1DB8 67DC 58DE"            /*  .›öv¹k¿‚Ş.¸gÜXŞ */
	$"73D7 38BB 9856 E781 3E2D 78A3 5E93 ED32"            /* s×8»˜Vç>-x£^“í2 */
	$"E817 49A7 6F2A 9712 008A EA0E 32A1 882C"            /* è.I§o*—..Šê.2¡ˆ, */
	$"38EA 01A0 0F5E 7D72 11A4 7F69 A306 4DA4"            /* 8ê. .^}r.¤.i£.M¤ */
	$"F15E 17E2 5F8F DAAA 6B4D A368 BA7C D733"            /* ñ^.â_ÚªkM£hº|×3 */
	$"0270 91A9 6381 D4FD 3DE8 02D6 89F1 7FC6"            /* .p‘©cÔı=è.Ö‰ñ.Æ */
	$"2970 A756 D0E5 8E13 D5BC D8C9 03E8 1B3F"            /* )p§VĞå.Õ¼ØÉ.è.? */
	$"A57B 2E81 E26B 2D73 4BFE D0B7 9919 547C"            /* ¥{.âk-sKşĞ·™.T| */
	$"E01E 54FA 11D8 D007 907C 4EFD A1EC FC25"            /* à.Tú.ØĞ.|Nı¡ìü% */
	$"7DFD 9F6C DBE5 2DB4 2AF2 49AF 06F8 BBE3"            /* }ıŸlÛå-´*òI¯.ø»ã */
	$"5F19 78DF 4BF3 E1D1 6EB6 81B8 8046 FC7F"            /* _.xßKóáÑn¶¸€Fü. */
	$"B99D DFA5 006C 7C33 2C7C 49E0 F67C EE37"            /* ¹ß¥.l|3,|Iàö|î7 */
	$"DA79 39F5 F323 AFA7 3E23 7C51 D33C 0F68"            /* Úy9õó#¯§>#|QÓ<.h */
	$"D25C 4CA1 C0E0 6680 3C47 FE1A 92FA 7B93"            /* Ò\L¡Ààf€<Gş.’ú{“ */
	$"25BD ABB4 3BBE F01C 57A9 7C3B F8EF E1FF"            /* %½«´;¾ğ.W©|;øïáÿ */
	$"0017 CABA 6497 D143 7CE3 E486 43B1 9CFA"            /* ..Êºd—ÑC|ãä†C±œú */
	$"2E7E F1FA 5006 778F 7E39 43E0 DD55 6D6E"            /* .~ñúP.w~9CàİUmn */
	$"8ED8 F3C9 F6A8 EEBF 691F 0A49 A249 7BA6"            /* ØóÉö¨î¿i.ÂI¢I{¦ */
	$"EAD0 5C4D 0206 9514 9CAE 7A67 3401 8DE1"            /* êĞ\M..•.œ®zg4.á */
	$"8FDA C3C1 B74E EBAD 6B30 59E3 A190 9C1F"            /* ÚÃÁ·Në­k0Yã¡œ. */
	$"D2AC EB9F B4AE 8BA3 EACD 0DCD F451 5AC6"            /* Ò¬ëŸ´®‹£êÍ.ÍôQZÆ */
	$"4179 8B7C A012 0039 1DB9 1F9D 0077 5A47"            /* Ay‹| ..9.¹..wZG */
	$"C50F 07FC 46F0 CEA5 2F84 FC41 69A9 C963"            /* Å..üFğÎ¥/„üAi©Éc */
	$"1AFD A85B B16F 28B8 6DA1 8E3B ED6F C8D7"            /* .ı¨[±o(¸m¡;íoÈ× */
	$"E73F ED03 7334 BE25 BA89 E462 A2E3 2013"            /* ç?í.s4¾%º‰äb¢ã . */
	$"DF0D 401E A1FB 10BB 2786 7E29 1538 3E5E"            /* ß.@.¡û.»'†~).8>^ */
	$"93FF 00B7 B5F3 CFC7 39E5 9BC4 A449 216D"            /* “ÿ.·µóÏÇ9å›Ä¤I!m */
	$"B24D 8C9E 9CAD 0072 FA17 C34D 43C4 FE1A"            /* ²MŒœ­.rú.ÃMCÄş. */
	$"9FC4 D6BA AD8C 31C3 74F6 8609 4BF9 8CCA"            /* ŸÄÖº­Œ1Ãtö†ÆKùŒÊ */
	$"88FB 8614 8C7E F00E BD8D 50F0 C78F 7E20"            /* ˆû†.Œ~ğ.½PğÇ~  */
	$"7C17 F174 5AAF 853C 4175 A6EA 1A74 CACC"            /* |.ñtZ¯…<Au¦ê.tÊÌ */
	$"914A C609 870D B248 F3B6 4461 8CAB 0FC8"            /* ‘JÆÆ‡.²Hó¶DaŒ«.È */
	$"F400 FD1E F8F1 F172 EEEB C01A 46B5 688F"            /* ô.ı.øññrîëÀ.Fµh */
	$"6A75 9D3A DEF4 445B 98FC D8D5 F69F A6EC"            /* ju:ŞôD[˜üØÕöŸ¦ì */
	$"57E6 7FC4 6F88 1E22 F186 B936 9369 7772"            /* Wæ.Äoˆ."ñ†¹6“iwr */
	$"F6DE 7791 1C10 962D 70F9 DBC8 1CB6 4F01"            /* öŞw‘..–-pùÛÈ.¶O. */
	$"7E9C 6680 3D27 5CFF 0082 7EFE D01A 2784"            /* ~œf€='\ÿ.‚~şĞ.'„ */
	$"DBC4 D73A 6E9B 25D4 76E6 E64D 261B 867B"            /* ÛÄ×:n›%ÔvææM&.†{ */
	$"A540 3257 EE79 6D20 1FC2 18E7 A024 F15E"            /* ¥@2Wîym .Â.ç $ñ^ */
	$"EFFF 0004 DAF8 A5F1 C2DF C403 E1BF 897C"            /* ïÿ..Úø¥ñÂßÄ.á¿‰| */
	$"2FE2 7D53 C097 B6F2 4965 AB5C 58DC 3DBE"            /* /â}SÀ—¶òIe«\XÜ=¾ */
	$"953A A965 4FB4 32ED 1148 14A8 4CF0 FB4A"            /* •:©eO´2í.H.¨LğûJ */
	$"E017 C807 D19F F050 2FF9 367D 7FFE BA2F"            /* à.È.ÑŸğP/ù6}.şº/ */
	$"FE8B 92BF 18E8 03F6 7FC1 4FE0 C87C 11A3"            /* ş‹’¿.è.ö.ÁOàÈ|.£ */
	$"45F0 F25B 393C 391D 9A26 9CD6 B831 B440"            /* Eğò[9<9.š&œÖ¸1´@ */
	$"6011 8EFE B9E7 39CF 34DD 427E BCD0 0739"            /* `.ş¹ç9Ï4İB~¼Ğ.9 */
	$"7F71 B41E 6B9B BFB9 C679 1401 CE5F DDF5"            /* .q´.k›¿¹Æy..Î_İõ */
	$"19AE 72FA F7AF 3401 8377 7FC9 C1AC 7B9D"            /* .®rú÷¯4.ƒw.ÉÁ¬{ */
	$"400F E2A0 0CF9 7543 9FBC 2A2F ED43 9FBD"            /* @.â .ùuCŸ¼*.íCŸ½ */
	$"4013 45AA 13DC 568D B6A0 0E30 D401 B367"            /* @.Eª.ÜV¶ .0Ô.³g */
	$"7F9C 735B B677 D85C 93D2 8039 AD5B 599A"            /* .œs[¶wØ\“Ò€9­[Yš */
	$"6D7E C61D 16DE EA4D 5E2B 8496 CBEC 91B4"            /* m~Æ..ŞêM^+„–Ëì‘´ */
	$"9309 90EE 568D 5412 5815 CF1E 95F5 BF87"            /* “ÆîVT.X.Ï.•õ¿‡ */
	$"FF00 6ABB BD23 C396 D7FF 0018 7C05 AF68"            /* ÿ.j»½#Ã–×ÿ..|.¯h */
	$"8491 01D4 A2B0 916D A693 04FD C902 946C"            /* „‘.Ô¢°‘m¦“.ıÉ.”l */
	$"0276 A97E 84F0 3800 1DF6 89E2 2F86 1F1A"            /* .v©~„ğ8..ö‰â/†.. */
	$"B43F EDFF 000F 5CE9 FAED A239 85A6 1195"            /* ´?íÿ..\éúí¢9…¦.• */
	$"9A07 C03B 5B70 1244 D820 E38E 0E7A 1AF9"            /* š.À;[p.DØ ã.z.ù */
	$"D3F6 8DD2 BFB0 6CAE B4C5 90BC 71AE E8D8"            /* ÓöÒ¿°l®´Å¼q®èØ */
	$"F52A 4646 7DFB 7E14 01DE 7ECC 6853 E047"            /* õ*FF}û~..Ş~ÌhSàG */
	$"8614 823F E3F4 F3EF 7939 FEB5 4FC3 3F14"            /* †.‚?ãôóïy9şµOÃ?. */
	$"F53D 77E2 56A9 E06B AD32 D22B 2B6B 7926"            /* õ=wâV©àk­2Ò++ky& */
	$"B596 2DC2 41B1 9570 F938 390D 9E00 C631"            /* µ–-ÂA±•pù89..Æ1 */
	$"CF60 0F57 B5D5 61D3 FC2D A8BD D37E E91C"            /* Ï`.WµÕaÓü-¨½Ó~é. */
	$"900F 6CAF 3FCA BE76 D3BC 6B71 7FE2 6D46"            /* .l¯?Ê¾vÓ¼kq.âmF */
	$"0F0A 6849 7AE4 813C 8410 2319 3805 F202"            /* .ÂhIzä<„.#.8.ò. */
	$"E79E BD71 ED40 1627 B5F8 8B71 E25B 4D52"            /* ç½qí@.'µø‹qâ[MR */
	$"FF00 5BD3 B4BD 1D19 637B 05C4 AD31 2391"            /* ÿ.[Ó´½..c{.Ä­1#‘ */
	$"B8A8 C127 A618 E3D3 D7D5 7E1C 5D5C C77D"            /* ¸¨Á'¦.ãÓ×Õ~.]\Ç} */
	$"AB5B 42E7 649A 7B48 57D5 95D0 29FF 00C7"            /* «[Bçdš{HWÕ•Ğ)ÿ.Ç */
	$"DBF3 A00F 9DAE ED5F 55F8 C574 FA80 2E2C"            /* Ûó .®í_UøÅtú€., */
	$"6096 E220 DD37 6E55 07F0 DF9F C2AD F8F3"            /* `–â İ7nU.ğßŸÂ­øó */
	$"C453 786F 4C5B DB6F 25A5 2F8D 9202 411D"            /* ÄSxoL[Ûo%¥/’.A. */
	$"FA11 401D 1780 AE45 DF8A FC31 768A 544D"            /* ú.@..€®EßŠü1vŠTM */
	$"A8D9 4814 F38C C887 1FAD 47FB 50CF A95D"            /* ¨ÙH.óŒÈ‡.­GûPÏ©] */
	$"7896 D74F 2EEB 0CD3 AC64 8F42 C050 0715"            /* x–×O.ë.Ó¬dBÀP.. */
	$"6961 6CFB E169 1608 6085 DC63 D870 07E3"            /* ialûái..`…ÜcØp.ã */
	$"8FD6 B95D 3752 B99D 5AE0 48C9 2DBC E447"            /* Ö¹]7R¹ZàHÉ-¼äG */
	$"221D A78C 1560 4742 3DBD 2803 DA7E 2858"            /* ".§Œ.`GB=½(.Ú~(X */
	$"5DF8 C3E1 B685 E389 A326 E67B 50D3 B01F"            /* ]øÃá¶…ã‰£&æ{PÓ°. */
	$"79D4 9476 FC4A 93F8 D7CF 8B34 CCB2 4104"            /* yÔ”vüJ“ø×Ï‹4Ì²A. */
	$"A545 CA6C 6E7B 641F E940 1853 4612 3C2F"            /* ¥EÊln{d.é@.SF.</ */
	$"DD5E 326A CEB5 AB4D 79A0 41E7 4A5A 5924"            /* İ^2jÎµ«My AçJZY$ */
	$"F249 2739 58D4 75FF 00BE 9280 3EDF FD9A"            /* òI'9XÔuÿ.¾’€>ßıš */
	$"BE1F 1F03 7ECF B0DE 5D42 52FF 00C4 9BF5"            /* ¾...~Ï°Ş]BRÿ.Ä›õ */
	$"79F2 3044 6EA1 605E 4671 E52A 37D5 DABE"            /* yò0Dn¡`^Fqå*7ÕÚ¾ */
	$"25F8 FBFF 0023 45D7 FD77 3FD6 803D 4FF6"            /* %øûÿ.#E×ıw?Ö€=Oö */
	$"23FF 0091 63E2 97FD 73D2 7FF6 F6BE 77F8"            /* #ÿ.‘câ—ısÒ.öö¾wø */
	$"DE31 E256 3EB2 4BFF 00B2 D007 1D69 178C"            /* Ş1âV>²Kÿ.²Ğ..i.Œ */
	$"ECFC 3136 B9A5 4973 1E8E 974D 0CAF 138C"            /* ìü16¹¥Is.—M.¯.Œ */
	$"2CDB 509C 8EBF 7593 9AEE 3F66 1F85 3E06"            /* ,ÛPœ¿u“šî?f.…>. */
	$"F8DD F156 D3C3 BF10 FC6A FA70 91FE D0B6"            /* øİñVÓÃ¿.üjúp‘şĞ¶ */
	$"6509 9755 2BF3 3429 3138 4620 1CF5 6201"            /* eÆ—U+ó4)18F .õb. */
	$"DBCF 2003 ED1F DAD7 C32E 9A74 5F60 B558"            /* ÛÏ .í.Ú×Ã.št_`µX */
	$"EDAD D024 71C6 B854 4518 5500 7400 0031"            /* í­Ğ$qÆ¸TE.U.t..1 */
	$"5F9B 7F08 AEAC B49F 8D1E 07BD D7CA 8B3B"            /* _›..®¬´Ÿ..½×Ê‹; */
	$"3F14 E992 DEF9 9D3C A4BB 8CC9 BBDB 01B3"            /* ?.é’Şù<¤»ŒÉ»Û.³ */
	$"401F AC3E 36FD A3B4 0D17 5058 2196 2619"            /* @.¬>6ı£´..PX!–&. */
	$"E5B2 3007 724D 6F7C 15FD A7FE 11FC 628E"            /* å²0.rMo|.ı§ş.üb */
	$"3B3F 0C78 EF48 9F52 6C63 4F92 616F 747B"            /* ;?.xïHŸRlcO’aot{ */
	$"7CB0 C9B5 DBEA 011D 3D45 0072 7FF0 502F"            /* |°ÉµÛê..=E.r.ğP/ */
	$"F936 7D7F FEBA 2FFE 8B92 BF18 E803 F497"            /* ù6}.şº/ş‹’¿.è.ô— */
	$"F64D D03C 4169 F047 47D7 7469 5CC7 231F"            /* öMĞ<AiğGG×ti\Ç#. */
	$"3216 C947 C2A1 E9D8 F279 AF5B 3AD7 DA98"            /* 2.ÉGÂ¡éØòy¯[:×Ú˜ */
	$"4175 0BDB DCF3 98D8 1EDE 87BF 0280 3235"            /* Au.ÛÜó˜Ø.Ş‡¿.€25 */
	$"09F7 6483 5E61 E3FF 001F 49E1 4BFB 3B41"            /* Æ÷dƒ^aãÿ..IáKû;A */
	$"A60B A4BB 8DDF 7F9B B369 5206 3A1C F5A0"            /* ¦.¤»ß.›³iR.:.õ  */
	$"0C28 BC6B 16A8 3FE3 CA48 89FF 006C 3579"            /* .(¼k.¨?ãÊH‰ÿ.l5y */
	$"46B7 FB47 784A C356 BBD1 EE74 DD55 64B3"            /* F·ûGxJÃV»ÑîtİUd³ */
	$"9E4B 791D 638C A965 62A4 8F9F 38C8 FD68"            /* Ky.cŒ©eb¤Ÿ8Èıh */
	$"0316 EFF6 86F0 6B9C 456F A930 23AF 92A3"            /* ..ïö†ğkœEo©0#¯’£ */
	$"1FF8 F550 97E3 8784 A500 87BD 527B 183A"            /* .øõP—ã‡„¥.‡½R{.: */
	$"7EB4 0152 5F8C 3E16 6E97 1727 FED8 9A8D"            /* ~´.R_Œ>.n—.'şØš */
	$"7E2D F860 9E6E 6718 F589 A802 74F8 BFE1"            /* ~-ø`ng.õ‰¨.tø¿á */
	$"5880 26F2 73CF 410B 1A9B FE17 AF85 AD8A"            /* X€&òsÏA..›ş.¯…­Š */
	$"EC8E F66F 52B1 EDC7 E668 02E4 1FB4 7785"            /* ìöoR±íÇæh.ä.´w… */
	$"A119 6D33 5324 0E30 A9C9 FF00 BEAB B6F8"            /* ¡.m3S$.0©Éÿ.¾«¶ø */
	$"6BF1 B3C3 FF00 10FC 6BA5 F817 4CD3 B51B"            /* kñ³Ãÿ..ük¥ø.LÓµ. */
	$"79F5 4778 D2E2 709B 10AA 33E4 80C4 F217"            /* yõGxÒâp›.ª3ä€Äò. */
	$"1F8D 007B 0DA7 87B5 3F87 3E3C D13E 2168"            /* ..{.§‡µ?‡><Ñ>!h */
	$"F38B 9BBD 16ED 67F2 F6EC 12C7 CAC9 1E73"            /* ó‹›½.ígòöì.ÇÊÉ.s */
	$"C6E4 675C FBD7 D6BE 26D4 3E0C FED1 7E06"            /* Æäg\û×Ö¾&Ô>.şÑ~. */
	$"3E1F D635 C896 DEE4 A5C0 85EE 56D2 F6D2"            /* >.Ö5È–Şä¥À…îVÒöÒ */
	$"6507 E601 FF00 8806 65CE 1948 2DD4 1A00"            /* e.æ.ÿ.ˆ.eÎ.H-Ô.. */
	$"4F84 DF0B FE1E 7C20 D0EE 3C35 E01B B96F"            /* O„ß.ş.| Ğî<5à.¹o */
	$"7ED5 37DA 6E25 96E5 2E6E 257C 6D52 C502"            /* ~Õ7Ún%–å.n%|mRÅ. */
	$"AA80 0607 0A3A F735 97F1 37E0 ECDE 30B7"            /* ª€..Â:÷5—ñ7àìŞ0· */
	$"B9D4 2F8F 9D73 30C4 7101 FBB8 940C 01FE"            /* ¹Ô/s0Äq.û¸”..ş */
	$"D1EF 93DC F028 027F 86DE 0CBD F07F 81B4"            /* Ñï“Üğ(..†Ş.½ğ.´ */
	$"DF0C B5BC ACF6 7E7E 76A9 3F7E 677F FD9E"            /* ß.µ¼¬ö~~v©?~g.ı */
	$"B96D 23E1 26A3 A0FC 44BB F160 8247 5B8B"            /* ¹m#á&£ üD»ñ`‚G[‹ */
	$"7921 0154 9396 653F FB2D 007A 2A78 52EF"            /* y!.T“–e?û-.z*xRï */
	$"57F0 D6A3 A7DC 5B4B 0B4A DF28 7429 918E"            /* WğÖ£§Ü[K.Jß(t)‘ */
	$"D9EB 5E53 E15F 86D7 FE05 8F53 B36B 191B"            /* Ùë^Sá_†×ş.S³k.. */
	$"ED57 66E7 CD54 2491 B554 29C7 61B4 9FF8"            /* íWfçÍT$‘µT)Ça´Ÿø */
	$"11A0 0C9B AF84 375A E78B E1F1 6CA9 A8DC"            /* . .›¯„7Zç‹áñl©¨Ü */
	$"BD9B 7990 4263 2123 6038 C13D BBE0 639F"            /* ½›yBc!#`8Á=»àcŸ */
	$"5AF5 0F85 DA0E AB6D E21B 93A8 E917 D043"            /* Zõ.…Ú.«mâ.“¨é.ĞC */
	$"3D8C 912C 925B BAA6 EDE8 7058 8C74 5340"            /* =Œ‘,’[º¦íèpXŒtS@ */
	$"1C87 8FBE 0BEA 36FE 2AFF 0084 9B46 80B9"            /* .‡¾.ê6ş*ÿ.„›F€¹ */
	$"6568 E441 FC4A 7FFA E01F C2BC EF5F F86B"            /* ehäAüJ.úà.Â¼ï_øk */
	$"79E2 8962 D3EF FC3F A8CB 2C4C 76A2 44EA"            /* yâ‰bÓïü?¨Ë,Lv¢Dê */
	$"7B67 381D 38EB FAD0 074B E12F 01F8 AF4E"            /* {g8.8ëúĞ.Ká/.ø¯N */
	$"F1AE 84AF E18D 4D6D ACF5 4B5F 3254 B490"            /* ñ®„¯áMm¬õK_2T´ */
	$"C488 B2A6 4EF0 B8DA 00EB D302 BD53 E317"            /* Äˆ²¦Nğ¸Ú.ëÓ.½Sã. */
	$"C1E8 BC69 0FDA ADD7 13A1 DCA4 0E41 1C8A"            /* Áè¼i.Ú­×.¡Ü¤.A.Š */
	$"00F9 B7C5 3F0D 7C5F A334 96F7 7A25 EC91"            /* .ù·Å?.|_£4–÷z%ì‘ */
	$"3702 6B78 99D4 8FAA 8383 EC6A AF83 FE0C"            /* 7.kx™Ôªƒƒìj¯ƒş. */
	$"78DF C5D7 B0E9 5A37 872E EDAD CB62 4BBB"            /* xßÅ×°éZ7‡.í­ËbK» */
	$"A85E 2863 1DD9 9D87 27D8 649F 4A00 FADF"            /* ¨^(c.Ù‡'ØdŸJ.úß */
	$"5DF8 5B60 BF0F ECFC 1762 0C91 69F6 8B6C"            /* ]ø[`¿.ìü.b.‘iö‹l */
	$"8EC3 E672 072C 7DC9 C9FC 6BE3 9F88 1F00"            /* Ãær.,}ÉÉükãŸˆ.. */
	$"FC73 E1BD 4238 F45F 0D6A 9AAD BDCE F61F"            /* üsá½B8ô_.jš­½Îö. */
	$"61B4 96E4 C641 190D B14E DCEE 18CF 5E7D"            /* a´–äÆA..±NÜî.Ï^} */
	$"2803 6FC3 5FB3 1789 F5AF 007F 686A 5A5D"            /* (.oÃ_³.‰õ¯..hjZ] */
	$"D69D A9DE 5C48 C96F 7503 412C 689F 2A6E"            /* Ö©Ş\HÉou.A,hŸ*n */
	$"4601 8658 31E4 7208 3DEB CE3C 1DFB 3FFC"            /* F.†X1är.=ëÎ<.û?ü */
	$"4BF1 378C B48F 0F6A 9E05 F11E 9FA7 DD5F"            /* Kñ7Œ´.j.ñ.Ÿ§İ_ */
	$"4715 E5E4 FA6C F1C3 043B 879A E642 9B7E"            /* G.åäúlñÃ.;‡šæB›~ */
	$"E838 E704 E067 9A00 FD25 D434 A87F B13F"            /* è8ç.àgš.ı%Ô4¨.±? */
	$"B26C A258 A18E 110C 51A8 C055 030A 07B0"            /* ²l¢X¡..Q¨ÀU.Â.° */
	$"0057 C19F B4BF C00F 182A 0D73 C3BE 1DD4"            /* .WÁŸ´¿À..*.sÃ¾.Ô */
	$"F589 EE35 058C C1A7 DA49 7522 A149 0962"            /* õ‰î5.ŒÁ§ÚIu"¡IÆb */
	$"B1A9 2172 1467 18C9 1EB4 01D3 7EC5 1F0C"            /* ±©!r.g.É.´.Ó~Å.. */
	$"3C71 A268 FF00 116C FC59 E0FD 7343 3A82"            /* <q¢hÿ..lüYàısC:‚ */
	$"6962 DBFB 4B4F 9ACF CFDB F6CD FB3C C51B"            /* ibÛûKOšÏÏÛöÍû<Å. */
	$"B6EE 4CE3 A6E1 9EA2 BC97 F69A FD9E 3C7F"            /* ¶îLã¦á¢¼—öšı<. */
	$"67AB 6977 5E17 F06E B9AE FDB1 EECC C34C"            /* g«iw^.ğn¹®ı±îÌÃL */
	$"D3E7 BCF2 B1E5 6DDF E52B 6DCE E6C6 719D"            /* Óç¼ò±åmßå+mÎæÆq */
	$"A71D 0D00 76DF B3DF ECE1 E24D 73E0 4EB3"            /* §...vß³ßìáâMsàN³ */
	$"A3F8 D3C2 5AAE 8D7D 2788 2E4A 5B6A 9632"            /* £øÓÂZ®}'ˆ.J[j–2 */
	$"DA4B 2406 D6D4 0709 22A9 2848 71B8 0C65"            /* ÚK$.ÖÔ.Æ"©(Hq¸.e */
	$"48EC 6BE6 DF11 FECE DF1E FE19 FC42 75F0"            /* Hìkæß.şÎß.ş.üBuğ */
	$"AFC3 FF00 16EA 1368 B791 DD69 DAA6 97A4"            /* ¯Ãÿ..ê.h·‘İiÚ¦—¤ */
	$"5CDD 46DB 4892 2757 8D19 770E 3233 C104"            /* \İFÛH’'W.w.23Á. */
	$"5007 E9AE 8D67 77F1 C3E1 2E8F AEF8 A7C2"            /* P.é®gwñÃá.®ø§Â */
	$"BA8E 81AC 5ED9 817F A6EA 3652 D9CB 6F72"            /* º¬^Ù.¦ê6RÙËor */
	$"BF2C A047 2A86 D858 128D 8E54 A9AF CF8F"            /* ¿, G*†ØX.T©¯Ï */
	$"DA43 F622 F893 E19D 7AE7 C55F 0FFC 3D77"            /* ÚCö"ø“ázçÅ_.ü=w */
	$"AE69 B792 34B3 DA59 4664 B8B7 90F2 4AC6"            /* ®i·’4³ÚYFd¸·òJÆ */
	$"3E67 563C 8DA0 904E 318C 1A00 F16B D3FB"            /* >gV< N1Œ..ñkÓû */
	$"44EB 36FF 00F0 8E4D E1AF 17DD 492A FD9C"            /* Dë6ÿ.ğMá¯.İI*ıœ */
	$"A0D1 A76B 8914 8C6C 2C22 DE72 3F13 9E6B"            /*  Ñ§k‰.Œl,"Şr?.k */
	$"EC4F F827 FF00 EC55 E35F 0678 C21F 8E1F"            /* ìOø'ÿ.ìUã_.xÂ.. */
	$"16F4 4934 696C 2091 742D 2AE4 0173 E6C8"            /* .ôI4il ‘t-*ä.sæÈ */
	$"A637 B89E 3EB1 0546 7554 6C3E 5B71 0BB5"            /* ¦7¸>±.FuTl>[q.µ */
	$"7700 7BEF FC14 0BFE 4D9F 5FFF 00AE 8BFF"            /* w.{ïü..şMŸ_ÿ.®‹ÿ */
	$"00A2 E4AF C63A 00FD 8DFD 8334 AB7B FF00"            /* .¢ä¯Æ:.ııƒ4«{ÿ. */
	$"D99B 41F3 501C C8FD 47FD 338E BD57 C4BF"            /* Ù›AóP.ÈıGı3½WÄ¿ */
	$"0C2C 3505 6610 2E7B 1028 03C4 3C7B E19D"            /* .,5.f..{.(.Ä<{á */
	$"4BC2 EAF3 4686 78D7 2486 3862 3FDE FEA4"            /* KÂêóF†x×$†8b?Şş¤ */
	$"1AF9 BFE2 6EB7 69AF 2DA1 8619 A3B9 D3E4"            /* .ù¿ân·i¯-¡†.£¹Óä */
	$"6F39 5D38 08F8 1904 1208 0C14 7383 961C"            /* o9]8.ø......sƒ–. */
	$"5006 5E8A DC0F C2BE 5AF8 A968 6CBE 22F8"            /* P.^ŠÜ.Â¾Zø©hl¾"ø */
	$"8602 319B E925 FF00 BECE FF00 FD9A 8037"            /* †.1›é%ÿ.¾Îÿ.ıš€7 */
	$"3C39 F083 FE12 1D3E CB50 875E 07ED AAA5"            /* <9ğƒş..>ËP‡^.íª¥ */
	$"238E DF71 0C4E 36E4 B750 78E9 5EC1 6FFB"            /* #ßq.N6ä·Pxé^Áoû */
	$"0278 CB50 D186 AB67 E2AB 6490 AEEF 2A6B"            /* .xËPÑ†«gâ«d®ï*k */
	$"46DA 3DB7 AB1F FD06 BE6F 33E2 3A79 64A3"            /* FÚ=·«.ı.¾o3â:yd£ */
	$"19D3 6EED F5ED D4FD 571B E1C6 1F05 9761"            /* .ÓníõíÔıW.áÆ..—a */
	$"F193 C72E 6ACA 328A E4D2 D249 AD79 AFD7"            /* ñ“Ç.jÊ2ŠäÒÒI­y¯× */
	$"7B7C 8F08 F1DF C29F 187C 3ABE 9ECF 5EB1"            /* {|.ñßÂŸ.|:¾Ï^± */
	$"5648 1B6B 4F03 6F4E 7A13 C020 1F71 5C85"            /* VH.kO.oNz.À .q\… */
	$"7BB8 6C4D 3C5D 28D6 A4F4 6AE7 C067 392E"            /* {¸lM<](Ö¤ôjçÀg9. */
	$"2F22 C4FD 5717 1B36 9493 5AA9 45ED 24FB"            /* /"ÄıW..6”“Z©Eí$û */
	$"7E29 A69E A77F E05F 825E 30F1 E5F5 AD9D"            /* ~)¦§.à_‚^0ñåõ­ */
	$"AC71 592D D91B 1EE0 9DDB 7AEE D806 718F"            /* ¬qY-Ù..àÛzîØ.q */
	$"5C66 BE83 BAFF 0082 79EA 96BA 12EA 67C6"            /* \f¾ƒºÿ.‚yê–º.êgÆ */
	$"72BC E10B 30FB 1054 27B0 1F39 23F5 AF9D"            /* r¼á.0û.T'°.9#õ¯ */
	$"CDF8 9A9E 595A 3463 0E6B EFAD ACBC 8FAC"            /* ÍøšYZ4c.kï­¬¼¬ */
	$"A3C0 B2C2 D3A1 5337 ADEC 9D54 9C62 95E4"            /* £À²ÂÓ¡S7­ìTœb•ä */
	$"93DB 9AF6 49B5 D356 BADB 63E7 BF1E FC2C"            /* “ÛšöIµÓVºÛcç¿.ü, */
	$"5F03 5A4D 3DC6 B05E 68A7 FB3F 90F0 ED66"            /* _.ZM=Æ°^h§û?ğíf */
	$"7C9C E086 EC01 3D2B A3FD 92AC 9AF3 E3CF"            /* |œà†ì.=+£ı’¬šóãÏ */
	$"8718 0C8B 65BB B86F 60B6 D2E3 F523 F3AF"            /* ‡..‹e»¸o`¶Òãõ#ó¯ */
	$"A1C2 D7FA CD35 512D 19E3 7187 0ED3 E18C"            /* ¡Â×úÍ5Q-.ãq‡.ÓáŒ */
	$"C560 A956 F6A9 C632 BDB9 6DCD 7B26 AEFA"            /* Å`©Vö©Æ2½¹mÍ{&®ú */
	$"59DF CCFB BB5B CCCD E52A EE67 3B40 F524"            /* YßÌû»[ÌÍå*îg;@õ$ */
	$"E057 B478 3FF6 69D2 ECFC BBAF 136A 525E"            /* àW´x?öiÒìü»¯.jR^ */
	$"B75F 2205 F263 1F53 CB37 E1B6 BA0F 963D"            /* ·_".òc.SË7á¶º.–= */
	$"9B48 D074 AD0A D12C 74AB 082D 604F BA91"            /* ›HĞt­ÂÑ,t«.-`Oº‘ */
	$"2041 9F5E 3BFB D5F2 AA46 0814 0082 345F"            /*  AŸ^;ûÕòªF...‚4_ */
	$"BAA0 51E5 479C EC19 A007 6063 18A8 CDB4"            /* º QåGœì. .`c.¨Í´ */
	$"0C72 6352 7E94 00BE 4C5F F3CD 7F2A 7050"            /* .rcR~”.¾L_óÍ.*pP */
	$"BC01 8A00 6BC5 1BFD F406 9041 0818 11AF"            /* ¼.Š.kÅ.ıô.A...¯ */
	$"E540 0F54 55FB AA05 0406 1823 3401 1FD9"            /* å@.TUûª....#4..Ù */
	$"60CE 7CA5 CFD2 A455 5518 518A 0052 01EA"            /* `Î|¥ÏÒ¤UU.QŠ.R.ê */
	$"29A2 28C1 C841 9A00 0C68 DD54 1A55 555F"            /* )¢(ÁÈAš..hİT.UU_ */
	$"BA00 A005 A679 5193 9D83 3400 EDA3 18C5"            /* º. .¦yQ“ƒ4.í£.Å */
	$"37CA 8C1C EC19 A007 1552 3040 A411 4639"            /* 7ÊŒ.ì. ..R0@¤.F9 */
	$"0805 003A A37B 785F EF46 A7EA 2800 4821"            /* ...:£{x_ïF§ê(.H! */
	$"8FEE 46A3 E82A 4A00 F9BB FE0A 05FF 0026"            /* îF£è*J.ù»şÂ.ÿ.& */
	$"CFAF FF00 D745 FF00 D172 57E3 1D00 7ECE"            /* Ï¯ÿ.×Eÿ.ÑrWã..~Î */
	$"7FC1 3F3F E4D9 F40F FAE8 FF00 FA2E 3AFA"            /* .Á??äÙô.úèÿ.ú.:ú */
	$"3DD1 5970 4500 7987 C55D 052F 34D9 888C"            /* =ÑYpE.y‡Å]./4ÙˆŒ */
	$"1F94 D7C1 9F10 3459 2D35 B9ED 550F FA52"            /* .”×ÁŸ.4Y-5¹íU.úR */
	$"B441 47F1 3F0D 18FF 00BF 8A9F 9500 60E8"            /* ´AGñ?..ÿ.¿ŠŸ•.`è */
	$"6E19 548E 8402 2BE7 FF00 DA0F 4D36 3F11"            /* n.T„.+çÿ.Ú.M6?. */
	$"66BA 0B85 BFB4 82E0 1F53 B761 FD50 D003"            /* fº.…¿´‚à.S·aıPĞ. */
	$"BE0B FC47 B5F0 96B3 6B63 AE3E 2C16 E566"            /* ¾.üGµğ–³kc®>,.åf */
	$"8E46 E913 646E 07D0 1FD0 FD4D 7E9C F857"            /* Fé.dn.Ğ.ĞıM~œøW */
	$"E307 84CF 83D4 C573 6D24 7244 1848 1811"            /* ã.„ÏƒÔÅsm$rD.H.. */
	$"8C7A D7E7 3C63 84A8 AB53 AD15 78BB AF99"            /* Œz×ç<c„¨«S­.x»¯™ */
	$"FAFE 0675 38AF 87B0 986A 52FD EE1A 5C92"            /* úş.u8¯‡°˜jRıî.\’ */
	$"5D79 6F78 3F46 BDDF FB75 9F18 FC7E F1BE"            /* ]yox?F½ßûuŸ.ü~ñ¾ */
	$"817F A86A DAB3 3C6D 6B24 0D6D 1A9C 7EFD"            /* .¨jÚ³<mk$.m.œ~ı */
	$"883C 28EF C9EB ED9A F93A C278 AD6F ADAE"            /* ˆ<(ïÉëíšù:Âx­o­® */
	$"678F CC8A 2991 DD3F BCA1 8123 F102 BEAB"            /* gÌŠ)‘İ?¼¡#ñ.¾« */
	$"22A1 3A38 28C2 5BD9 7E45 789F 5E86 1B17"            /* "¡:8(Â[Ù~ExŸ^†.. */
	$"96E1 27AD 4A30 4E6B C9B4 D45F 9E8D DBB3"            /* –á'­J0NkÉ´Ô_Û³ */
	$"BF53 EB7F 85DE 34D2 B4ED 7AC7 C4B0 CCB3"            /* ¿Së.…Ş4Ò´ízÇÄ°Ì³ */
	$"D8BF CACE 9CED 0707 91D4 1040 C8EB 5F59"            /* Ø¿ÊÎœí..‘Ô.@Èë_Y */
	$"F887 F686 F05D 8F82 1E6B DD5E CA1B 68A2"            /* ø‡ö†ğ]‚.kİ^Ê.h¢ */
	$"24C8 D228 EDFA 9F6E B5F1 19F6 02BD 5C54"            /* $ÈÒ(íúŸnµñ.ö.½\T */
	$"5D38 B775 CBF3 B9F7 DC6B 94AC F278 4CD2"            /* ]8·uËó¹÷Ük”¬òxLÒ */
	$"84EF 4525 3E6E 96DE EDF6 B6A7 E63F C5FF"            /* „ïE%>n–Şíö¶§æ?Åÿ */
	$"0088 BFF0 B1BC 5D77 AA59 A3C5 A709 5CDB"            /* .ˆ¿ğ±¼]wªY£Å§Æ\Û */
	$"2370 581F E361 D890 071D 87E3 5EBD FB0C"            /* #pX.ãaØ..‡ã^½û. */
	$"7875 EF3C 77AF 78A5 97F7 5A4E 95F6 7071"            /* xuï<w¯x¥—÷ZN•öpq */
	$"FF00 2D67 7007 FE3B 1C95 FA56 0E87 D5B0"            /* ÿ.-gp.ş;.•úV.‡Õ° */
	$"F0A3 FCA9 23F9 EF88 B35F EDAC CEB6 317C"            /* ğ£ü©#ùïˆ³_í¬Î¶1| */
	$"3276 8FF8 6294 63FF 0092 A57F 33EC CF0F"            /* 2vøb”cÿ.’¥.3ìÏ. */
	$"69AF AEF8 DB43 D251 4B79 FA84 3B80 EBB1"            /* i¯®øÛCÒQKyú„;€ë± */
	$"583B 9FC1 558F E15F 64A8 0071 5D27 882D"            /* X;ŸÁUá_d¨.q]'ˆ- */
	$"1400 5140 0514 0051 4005 1400 5140 0514"            /* ..Q@...Q@...Q@.. */
	$"0051 4005 1400 5140 0514 0051 4005 1400"            /* .Q@...Q@...Q@... */
	$"5140 1F37 7FC1 40BF E4D9 F5FF 00FA E8BF"            /* Q@.7.Á@¿äÙõÿ.úè¿ */
	$"FA2E 4AFC 63A0 0FD9 CFF8 27E7 FC9B 3E81"            /* ú.Jüc .ÙÏø'çü›> */
	$"FF00 5D1F FF00 45C7 5F48 D006 178A 34E1"            /* ÿ.].ÿ.EÇ_HĞ..Š4á */
	$"7B63 226D CE41 AF89 7E3A 784A 6B4B B92E"            /* {c"mÎA¯‰~:xJkK¹. */
	$"E052 8E8D BD59 7820 8390 4500 78CD A46B"            /* àR½Yx ƒE.xÍ¤k */
	$"0DD3 7969 B239 3F7B 1A8E 8AA4 FDD1 ECA7"            /* .Óyi²9?{.Š¤ıÑì§ */
	$"72FF 00C0 4D79 7FED 35A0 B49A 7685 E278"            /* rÿ.ÀMy.í5 ´šv…âx */
	$"D722 1692 C663 8ECD F3C7 9FCA 4A00 F9FE"            /* ×".’ÆcÍóÇŸÊJ.ùş */
	$"B46C 7C47 AF69 711B 7D3F 59BD B788 FF00"            /* ´l|G¯iq.}?Y½·ˆÿ. */
	$"CB38 E665 5FCB 38A9 9423 3569 2BA3 B303"            /* Ë8æe_Ë8©”#5i+£³. */
	$"9862 F2CA BEDF 0756 54E7 B5E2 DA76 EDA1"            /* ˜bòÊ¾ß.VTçµâÚví¡ */
	$"56EE FAF7 5097 CFBE BB9A E24F EF4A E5CF"            /* Vîú÷P—Ï¾»šâOïJåÏ */
	$"E66A 0A69 24AC 8C2B D7AB 89A9 2AD5 E4E5"            /* æjÂi$¬Œ+×«‰©*Õäå */
	$"396A DB6D B6FC DBD5 962C F52D 434E 62F6"            /* 9jÛm¶üÛÕ–,õ-CNbö */
	$"17D7 16CC 7A98 A464 CFD7 069D 7BAA EA5A"            /* .×.Ìz˜¤dÏ×.{ªêZ */
	$"9107 50D4 2E6E 48E4 79B2 B3E3 E993 C52E"            /* ‘.PÔ.nHäy²³ãé“Å. */
	$"48DF 9ADA 9D2B 33C6 AC37 D495 79FB 1FE4"            /* HßšÚ+3Æ¬7Ô•yû.ä */
	$"E67C BFF8 0DED F815 6BF4 0FF6 4CF0 149E"            /* æ|¿ø.íø.kô.öLğ. */
	$"0AF8 370E AD7B 098E FBC5 139D 49C3 0C11"            /* Âø7.­{ÆûÅ.IÃ.. */
	$"0636 403E 8543 3FFD B4AA 384F A7BF 67EF"            /* .6@>…C?ı´ª8O§¿gï */
	$"0D3E A7E3 0BBF 134D 1FEE 3498 4C51 1238"            /* .>§ã.¿.M.î4˜LQ.8 */
	$"33CB C707 D903 E7FD F5AF A328 00A2 800A"            /* 3ËÇ.Ù.çıõ¯£(.¢€Â */
	$"2800 A280 0A28 00A2 800A 2800 A280 0A28"            /* (.¢€Â(.¢€Â(.¢€Â( */
	$"00A2 800A 2800 A280 0A28 00A2 800A 2803"            /* .¢€Â(.¢€Â(.¢€Â(. */
	$"E6EF F828 17FC 9B3E BFFF 005D 17FF 0045"            /* æïø(.ü›>¿ÿ.].ÿ.E */
	$"C95F 8C74 01FB 39FF 0004 FCFF 0093 67D0"            /* É_Œt.û9ÿ..üÿ.“gĞ */
	$"3FEB A3FF 00E8 B8EB E91A 008A E221 2C45"            /* ?ë£ÿ.è¸ëé..Šâ!,E */
	$"48EA 2BC2 7E33 7821 751B 39A4 58B2 707B"            /* Hê+Â~3x!u.9¤X²p{ */
	$"5007 C71A CE8B 2E95 7F25 B3A1 0D0B 33A7"            /* P.Ç.Î‹.•.%³¡..3§ */
	$"1F79 0FDF 1FF0 1C06 1D80 321A CDF1 8784"            /* .y.ß.ğ...€2.Íñ‡„ */
	$"878E BC11 A9F8 6D55 7ED1 3C3E 65A9 3DA7"            /* ‡¼.©ømU~Ñ<>e©=§ */
	$"4F9A 3FA6 48DB F463 401F 14CF 04D6 B349"            /* Oš?¦HÛôc@..Ï.Ö³I */
	$"6D71 1347 2C4E 5244 6182 AC0E 0823 B104"            /* mq.G,NRDa‚¬..#±. */
	$"5328 00A2 800A 2803 D3BF 67AF 8397 DF1A"            /* S(.¢€Â(.Ó¿g¯ƒ—ß. */
	$"3E21 5AE8 6639 1746 B1C5 E6B1 7001 023B"            /* >!Zèf9.F±Åæ±p..; */
	$"6523 E407 FBF2 1F91 7EA4 F453 5FA4 D3D8"            /* e#ä.ûò.‘~¤ôS_¤ÓØ */
	$"3486 DB46 D22C B93B 2DAD 6DA1 5E00 1854"            /* 4†ÛFÒ,¹;-­m¡^..T */
	$"451D 8000 0A00 FA57 E1FF 0084 20F0 4F86"            /* E.€.Â.úWáÿ.„ ğO† */
	$"6DB4 542A D3F3 35D4 83FE 5A4C D8DC 7E83"            /* m´T*Óó5ÔƒşZLØÜ~ƒ */
	$"851E CA2B A4A0 028A 0028 A002 8A00 28A0"            /* ….Ê+¤ .Š.( .Š.(  */
	$"028A 0028 A002 8A00 28A0 028A 0028 A002"            /* .Š.( .Š.( .Š.( . */
	$"8A00 28A0 028A 0028 A00F 9BBF E0A0 5FF2"            /* Š.( .Š.( .›¿à _ò */
	$"6CFA FF00 FD74 5FFD 1725 7E31 D007 ECE7"            /* lúÿ.ıt_ı.%~1Ğ.ìç */
	$"FC13 F3FE 4D9F 40FF 00AE 8FFF 00A2 E3AF"            /* ü.óşMŸ@ÿ.®ÿ.¢ã¯ */
	$"A468 00AE 73C5 7636 F736 5209 40E5 4D00"            /* ¤h.®sÅv6÷6RÆ@åM. */
	$"7C57 F1BB 49B6 B1BC 7BAB 52AB 246D B948"            /* |Wñ»I¶±¼{«R«$m¹H */
	$"03AF F5AF 3CF0 CEA1 6F7C BBAD C81E 59DA"            /* .¯õ¯<ğÎ¡o|»­È.YÚ */
	$"E99C 98DB D0FB 75C1 EE3D C360 03CC BE3A"            /* éœ˜ÛĞûuÁî=Ã`.Ì¾: */
	$"FECF 17BE 259A E3C7 DE06 851E F594 CBA9"            /* şÏ.¾%šãÇŞ.….õ”Ë© */
	$"5816 09E6 ED19 3344 4E06 EC0F 994E 338C"            /* X.Ææí.3DN.ì.™N3Œ */
	$"8E72 0FCB 3400 5140 0514 01FA 6DFB 28FC"            /* r.Ë4.Q@...úmû(ü */
	$"38B5 F875 F033 47BA 6B70 9A9F 89D0 6B17"            /* 8µøuğ3GºkpšŸ‰Ğk. */
	$"D211 F311 20FD C21F 658B 69C7 ABB7 AD7D"            /* Ò.ó. ıÂ.e‹iÇ«·­} */
	$"17F0 6744 82EB 5CBB F105 D283 F615 F26D"            /* .ğgD‚ë\»ñ.Òƒö.òm */
	$"F3D0 48E3 E661 EE17 03E8 E680 3DAC 3291"            /* óĞHãæaî..èæ€=¬2‘ */
	$"C1A5 A002 8A00 2BCE BE31 7C54 6F85 72F8"            /* Á¥ .Š.+Î¾1|To…rø */
	$"1EE6 77B3 8F4E F10F 8A13 45D4 65B8 8A49"            /* .æw³Nñ.Š.EÔe¸ŠI */
	$"5E38 1AC6 F6E1 7C85 8CE4 CCD2 DB42 8A30"            /* ^8.Æöá|…ŒäÌÒÛBŠ0 */
	$"FBB7 950A 5997 0019 DE28 FDA5 3E1D F87B"            /* û·•ÂY—..Ş(ı¥>.ø{ */
	$"E1F5 F7C4 5B27 D4F5 1B5D 3B56 8744 B9B3"            /* áõ÷Ä['Ôõ.];V‡D¹³ */
	$"1A5D E5B5 DDBD E3B4 798A 7B79 6113 40CA"            /* .]åµİ½ã´yŠ{ya.@Ê */
	$"9207 FDE2 2E41 5032 5D41 7EB9 FB46 F813"            /* ’.ıâ.AP2]A~¹ûFø. */
	$"42F1 3F85 3C37 341A D48B E2BB 4BAB C82E"            /* Bñ?…<74.Ô‹â»K«È. */
	$"1746 BFFD D244 C106 F8BC 8DF9 66DC 3180"            /* .F¿ıÒDÁ.ø¼ùfÜ1€ */
	$"5428 6601 5D49 00D0 6FDA 0FE1 2AEA AFA3"            /* T(f.]I.ĞoÚ.á*ê¯£ */
	$"8F14 BB49 0EAB 2687 7370 9A6D E3D9 DAEA"            /* .»I.«&‡spšmãÙÚê */
	$"0931 80DB 5C5D 884C 16F2 9946 D559 1D4B"            /* Æ1€Û\]ˆL.ò™FÕY.K */
	$"654A E430 27A9 5F19 F865 BC64 FF00 0F17"            /* eJä0'©_.øe¼dÿ... */
	$"568C F88A 3D2D 7596 B1D8 FBC5 934A D0AC"            /* VŒøŠ=-u–±ØûÅ“JĞ¬ */
	$"DBB1 B706 4465 C673 C74C 5006 5786 7C61"            /* Û±·.DeÆsÇLP.W†|a */
	$"A978 A7C6 BE26 D3EC 60B6 1E1D F0EB 43A6"            /* ©x§Æ¾&Óì`¶..ğëC¦ */
	$"8BAD AE65 B9D4 F064 B954 6DDB 7CA8 91E0"            /* ‹­®e¹Ôğd¹TmÛ|¨‘à */
	$"4240 24CA 6552 4796 41CC B4F8 AD6D 2F8D"            /* B@$ÊeRG–AÌ´ø­m/ */
	$"F54D 32F2 7B4B 5F0F 5A6A 16BE 1AB2 BA68"            /* õM2ò{K_.Zj.¾.²ºh */
	$"A569 6F75 C911 E69A 1475 CA2C 51C7 E526"            /* ¥iouÉ.æš.uÊ,QÇå& */
	$"E239 94C8 8581 4C30 06C5 BFC5 4F00 DDD9"            /* â9”È…L0.Å¿ÅO.İÙ */
	$"EB37 F6BE 25B6 961F 0FEA EBA0 EA45 15D8"            /* ë7ö¾%¶–..êë êE.Ø */
	$"DBEA 0D24 512D B950 B967 324D 128D A082"            /* Ûê.$Q-¹P¹g2M. ‚ */
	$"CD80 720E 23B2 F8B7 F0F3 517F 0C45 69E2"            /* Í€r.#²ø·ğóQ..Eiâ */
	$"8B66 93C6 66E5 7424 6491 1AF4 DBA9 69C2"            /* ‹f“Æfåt$d‘.ôÛ©iÂ */
	$"8650 54A0 0721 B69C F1D7 8A00 13E2 D7C3"            /* †PT .!¶œñ×Š..â×Ã */
	$"D92E B58B 35F1 2C02 4D02 D2EA FB51 DD14"            /* Ù.µ‹5ñ,.M.ÒêûQİ. */
	$"AAB0 C16D 2C90 DCBE F2BB 5847 2C52 236D"            /* ª°Ám,Ü¾ò»XG,R#m */
	$"2482 BF4A BFA4 F8FB C23A F268 0FA3 EB70"            /* $‚¿J¿¤øûÂ:òh.£ëp */
	$"DDAF 8A2C 1F54 D24C 6AE7 ED36 A8B1 33CA"            /* İ¯Š,.TÒLjçí6¨±3Ê */
	$"38F9 5409 E1CE EC60 C8A3 A9C5 0063 49F1"            /* 8ùTÆáÎì`È£©Å.cIñ */
	$"AFE1 CC7E 281E 0FFE D8BC 7BE3 7E34 A33C"            /* ¯áÌ~(..şØ¼{ã~4£< */
	$"7A4D F496 2B7A 7816 CD7E B09B 559B 3C79"            /* zMô–+zx.Í~°›U›<y */
	$"6650 C1BE 5233 C572 3F03 BF69 3F0B FC4F"            /* fPÁ¾R3År?.¿i?.üO */
	$"F0A7 824E B9A9 436D E29F 1569 8970 61B6"            /* ğ§‚N¹©CmâŸ.i‰pa¶ */
	$"D3AF 21B0 96ED 6132 DC5B DB5D 48AD 0C92"            /* Ó¯!°–ía2Ü[Û]H­.’ */
	$"46AA E5A2 133C 8815 B70E 0D00 757E 1DF8"            /* Fªå¢.<ˆ.·...u~.ø */
	$"E3F0 CFC5 7E21 B7F0 BE87 E209 A7BC BEF3"            /* ãğÏÅ~!·ğ¾‡âÆ§¼¾ó */
	$"BEC1 2BE9 B790 5A6A 1E48 265F B1DE 490A"            /* ¾Á+é·Zj.H&_±ŞIÂ */
	$"DBDD ED01 89F2 647E 158F 4048 E4FC 71FB"            /* Ûİí.‰òd~.@Häüqû */
	$"4AF8 5F4D 974A D33C 13A8 47A8 5FDE 78BF"            /* Jø_M—JÓ<.¨G¨_Şx¿ */
	$"49F0 DCB2 DCE9 97AB 6528 9F51 86D2 F12D"            /* IğÜ²Üé—«e(ŸQ†Òñ- */
	$"AF0A 25BC F3C4 9248 76C7 23ED 284B 290A"            /* ¯Â%¼óÄ’HvÇ#í(K)Â */
	$"C280 3ACD 57E3 97C3 0D0F C413 F867 52F1"            /* Â€:ÍWã—Ã..Ä.øgRñ */
	$"2B47 7567 7315 95E4 EB63 752D 9595 C4BB"            /* +Gugs.•äëcu-••Ä» */
	$"7CB8 6E6F 5223 6D6F 236F 4C24 B22B 1DEB"            /* |¸noR#mo#oL$²+.ë */
	$"C722 AEDE 7C5B F879 A75B A5D5 F78A 2DAD"            /* Ç"®Ş|[øy§[¥Õ÷Š-­ */
	$"E293 C403 C2CA D2A4 899D 54B1 416D 82B9"            /* â“Ä.ÂÊÒ¤‰T±Am‚¹ */
	$"DC48 FBDF 771F 3671 CD00 6778 8BE3 0F84"            /* ÜHûßw.6qÍ.gx‹ã.„ */
	$"3C39 E2AB 8D1B 50F1 6697 6B06 8BA5 EA5A"            /* <9â«.Pñf—k.‹¥êZ */
	$"8EB5 04B6 F70F 3C29 6B0D A5C3 3A3A 0F2F"            /* µ.¶÷.<)k.¥Ã::./ */
	$"6C70 5D23 C8A7 2E44 D16D 1D73 3C7F 1B7E"            /* lp]#È§.DÑm.s<..~ */
	$"1B49 E14B 9F1C 9D76 78F4 2B79 A281 2F66"            /* .IáKŸ.vxô+y¢/f */
	$"D36F 224B B793 6888 5A07 8435 E072 CAA8"            /* Óo"K·“hˆZ.„5àrÊ¨ */
	$"6012 066C AA92 4100 035F C1DF 103C 2DE3"            /* `..lª’A.._Áß.<-ã */
	$"E82E E6F0 DDED C3C9 A7CC 2DEF 6D2F 2C6E"            /* è.æğİíÃÉ§Ì-ïm/,n */
	$"74EB BB59 0A87 559A D6E6 38E6 8B72 B065"            /* të»YÂ‡UšÖæ8æ‹r°e */
	$"DC83 7020 8C8A C6D5 3E38 FC32 D1BC 453F"            /* Üƒp ŒŠÆÕ>8ü2Ñ¼E? */
	$"85B5 0F10 CA97 7677 30D9 5E4E 9A75 E4B6"            /* …µ..Ê—vw0Ù^Nšuä¶ */
	$"3677 12ED F2A1 B9BE 488D B5BC 8DBD 3092"            /* 6w.íò¡¹¾Hµ¼½0’ */
	$"48AC 77AF 1F30 C802 69FF 001C BE18 6ABE"            /* H¬w¯.0È.iÿ..¾.j¾ */
	$"218F C316 3E25 692E A7BD 934D B7B8 3637"            /* !Ã.>%i.§½“M·¸67 */
	$"4963 7179 1EE1 25B4 37CD 10B5 9675 28E0"            /* Icqy.á%´7Í.µ–u(à */
	$"C492 33E5 48C6 41AC FBDF DA4F E0BE 9BA9"            /* Ä’3åHÆA¬ûßÚOà¾›© */
	$"4BA5 5EF8 D522 920D 46E3 489E 7361 766D"            /* K¥^øÕ"’.FãHsavm */
	$"60BF 81A4 592D 65BA 1179 31CF 98A4 DB13"            /* `¿¤Y-eº.y1Ï˜¤Û. */
	$"3877 0014 0C19 4900 EABC 17E3 FF00 0B7C"            /* 8w....I.ê¼.ãÿ..| */
	$"43B0 BAD4 BC2B 7F34 E961 76F6 1791 5CD9"            /* C°ºÔ¼+.4éavö.‘\Ù */
	$"DC58 DC5B 5C2A AB18 A6B7 B88D 2589 B6C8"            /* ÜXÜ[\*«.¦·¸%‰¶È */
	$"8D86 5195 7523 2083 5D0D 007C DDFF 0005"            /* †Q•u# ƒ]..|İÿ.. */
	$"02FF 0093 67D7 FF00 EBA2 FF00 E8B9 2BF1"            /* .ÿ.“g×ÿ.ë¢ÿ.è¹+ñ */
	$"8E80 3F67 3FE0 9FA4 0FD9 9F40 CFFC F47F"            /* €?g?àŸ¤.ÙŸ@Ïüô. */
	$"FD17 1D7D 1C65 45EA C280 2A5E 6A96 F6D1"            /* ı..}.eEêÂ€*^j–öÑ */
	$"9669 00C0 AF22 F891 F13E D34D B695 1675"            /* –i.À¯"ø‘ñ>ÓM¶•.u */
	$"CE0F 7A00 F8DB E247 8E25 F115 E4B1 4520"            /* Î.z.øÛâG%ñ.ä±E  */
	$"C725 9893 855C E327 F303 8E49 2000 4902"            /* Ç%˜“…\ã'ó.I .I. */
	$"B2BC 1DA5 0B12 6E24 CF99 2316 008D A40E"            /* ²¼.¥..n$Ï™#..¤. */
	$"DB80 2413 D7B9 0327 1D49 201B BF13 350B"            /* Û€$.×¹.'.I .¿.5. */
	$"8D37 E13F 8B2F 2D58 ACAB A54C 8AC3 A8DE"            /* 7á?‹/-X¬«¥LŠÃ¨Ş */
	$"3612 3E81 89AF CFEA 0028 A002 9557 7305"            /* 6.>‰¯Ïê.( .•Ws. */
	$"F538 A00F D927 B387 49D1 6C34 9B70 0456"            /* õ8 .Ù'³‡IÑl4›p.V */
	$"5690 DB46 0700 2A22 A8FD 16AB E8FF 0013"            /* VÛF..*"¨ı.«èÿ.. */
	$"E3F0 5096 CE58 6E3C B2FE 733C 7049 2649"            /* ãğP–ÎXn<²şs<pI&I */
	$"C2E0 6D07 270A 3814 01E9 FE15 F8BD A26B"            /* Âàm.'Â8..éş.ø½¢k */
	$"7F24 1A8C 12B2 9218 2C81 8823 1907 07A8"            /* .$.Œ.²’.,ˆ#...¨ */
	$"C8CF A66B D0AC 758B 6BB4 0C92 039F 7A00"            /* ÈÏ¦kĞ¬u‹k´.’.Ÿz. */
	$"BEB2 2B74 34EA 002B CEFE 2C78 435B F14E"            /* ¾²+t4ê.+Îş,xC[ñN */
	$"BBF0 CAEF 49D3 56EE DFC3 DE33 4D5F 5163"            /* »ğÊïIÓVîßÃŞ3M_Qc */
	$"2469 E45B 2E9B A842 24C3 B02C 44B3 C230"            /* $iä[.›¨B$Ã°,D³Â0 */
	$"B96F 9B38 C024 0070 5E35 F859 E3CD 4F4E"            /* ¹o›8À$.p^5øYãÍON */
	$"F8DE DA6E 8427 9BC4 BAF6 8DAB E851 1BA8"            /* øŞÚn„'›Äºö«èQ.¨ */
	$"50EA 11D9 D969 6244 04B6 2266 96CE 68C7"            /* Pê.ÙÙibD.¶"f–ÎhÇ */
	$"99B7 900F DD21 AB7B C637 1E30 D43C 4BF0"            /* ™·.İ!«{Æ7.0Ô<Kğ */
	$"EBE2 C69B F0CB C477 10E9 2357 B4D4 3441"            /* ëâÆ›ğËÄw.é#W´Ô4A */
	$"2582 6A56 EB72 8890 CA55 AE84 0CBB ADD7"            /* %‚jVërˆÊU®„.»­× */
	$"204D 90B2 A920 6D60 003C 9B47 D3BC 6DE3"            /*  M²© m`.<›GÓ¼mã */
	$"7F85 BF14 3E11 787F C017 927F C25D E3EF"            /* .…¿.>.x.À.’.Â]ãï */
	$"1559 43E2 08E7 B55B 2B48 A4D6 2759 A7B9"            /* .YCâ.çµ[+H¤Ö'Y§¹ */
	$"DD2A CE25 882B 9554 8DF7 E210 AD9D FB3E"            /* İ*Î%ˆ+•T÷â.­û> */
	$"86F1 A789 FE21 E973 EAFA 7F84 BE1E BEA4"            /* †ñ§‰ş!ésêú.„¾.¾¤ */
	$"B1E8 82E7 4CD4 DAFE 0586 5D51 E631 25AC"            /* ±è‚çLÔÚş.†]Qæ1%¬ */
	$"9067 CD54 4529 33CB 8DBB 3701 9618 A008"            /* gÍTE)3Ë»7.–. . */
	$"6DFC 3FAB FC2D F84A DA17 81EC 5B5D D76C"            /* mü?«ü-øJÚ.ì[]×l */
	$"6CDC 5B99 CAA1 BFD4 A662 D25C DC37 0007"            /* lÜ[™Ê¡¿Ô¦bÒ\Ü7.. */
	$"B895 E694 8E70 5C80 4E05 739E 36F8 3979"            /* ¸•æ”p\€N.s6ø9y */
	$"0FC0 46F0 1784 19EF FC45 A208 35AD 32EA"            /* .ÀFğ.„.ïüE¢.5­2ê */
	$"4748 66BD D6AD AE56 F84C F21C 2A49 7172"            /* GHf½Ö­®VøLò.*Iqr */
	$"8C5D 8903 F7CF 9C0C D007 9CF8 1FF6 7BF8"            /* Œ]‰.÷Ïœ.Ğ.œø.ö{ø */
	$"83E1 9F1D F83E 6B8B 184F 87F5 9B7B 2F11"            /* ƒáŸ.ø>k‹.O‡õ›{/. */
	$"78E1 85CC 47CB F115 A35D 5C2E C4DC 4BF9"            /* xá…ÌGËñ.£]\.ÄÜKù */
	$"B757 B139 75DC 3FD0 4648 2EA6 B375 9FD9"            /* ·W±9uÜ?ĞFH.¦³uŸÙ */
	$"D3E2 5DA6 B7E3 3D7F 41D2 62B8 7F07 EAD1"            /* Óâ]¦·ã=.AÒb¸..êÑ */
	$"EBBF 0CE1 FB6C 3109 E4B8 D417 54D4 ADB2"            /* ë¿.áûl1Æä¸Ô.TÔ­² */
	$"5B16 E8F2 0FB3 0DE1 4609 6E54 0340 1B5E"            /* [.èò.³.áFÆnT.@.^ */
	$"33FD 9DFC 697B E13F 869A 6E8B 0453 EA26"            /* 3ıüi{á?†šn‹.Sê& */
	$"1974 0F1D BF9D 146B 2E9B A94B 15DE AF29"            /* .t..¿.k.›©K.Ş¯) */
	$"048F 37FD 2206 DA80 B126 727E 61B8 D687"            /* .7ı".Ú€±&r~a¸Ö‡ */
	$"C39F 879F 143E 136A 7E2F D562 F049 F115"            /* ÃŸ‡Ÿ.>.j~/ÕbğIñ. */
	$"AF87 A46D 23C0 B611 6A96 D049 73A6 5E6A"            /* ¯‡¤m#À¶.j–ĞIs¦^j */
	$"2D79 76E6 595B 1179 4B2C 11ED 7C16 161C"            /* -yvæY[.yK,.í|... */
	$"677A 6002 69FC 19E3 9D17 E2A4 575F 0BFC"            /* gz`.iü.ã.â¤W_.ü */
	$"31E3 1F0C 477B E28F B7EB E66D 52CE EBC3"            /* 1ã..G{â·ëæmRÎëÃ */
	$"5A8D 8492 EFBC B910 3CAF 3437 532E 76AC"            /* Z„’ï¼¹.<¯47S.v¬ */
	$"31C6 DE71 DCF9 5DCE 57C1 BF09 7C73 A37C"            /* 1ÆŞqÜù]ÎWÁ¿Æ|s£| */
	$"2CFD 9EBC 3377 E1F5 8350 F036 AD67 77AF"            /* ,ı¼3wáõƒPğ6­gw¯ */
	$"C02E 6DCF D8D1 34AD 42DE 46DC AFB6 53E7"            /* À.mÏØÑ4­BŞFÜ¯¶Sç */
	$"5C44 BFBB 2C4E FCF2 0120 0303 C1DE 08F8"            /* \D¿»,Nüò. ..ÁŞ.ø */
	$"DFAD FC47 F865 E2DF 1D78 7FC4 227F 0D6A"            /* ß­üGøeâß.x.Ä"..j */
	$"FA94 BAF7 9FA8 69B0 E936 425D 36F2 DA15"            /* ú”º÷Ÿ¨i°é6B]6òÚ. */
	$"D2ED 2DDF 74B0 0696 21E6 CA0C C03A E41C"            /* Òí-ßt°.–!æÊ.À:ä. */
	$"CA23 4B5F 01FC 50B5 F875 F0EB E09D BFC2"            /* Ê#K_.üPµøuğëà¿Â */
	$"FBD9 64F0 278B 741B ABED 765B CB15 B2B8"            /* ûÙdğ'‹t.«ív[Ë.²¸ */
	$"B0B2 D462 95EE ED8F 9DE7 3CCF 12EF 6468"            /* °²Ôb•îíç<Ï.ïdh */
	$"D187 EF86 4B6C 1200 68F8 87C0 9F12 2DFC"            /* Ñ‡ï†Kl..hø‡ÀŸ.-ü */
	$"09F1 3BE0 8D8F C3B9 7553 E3ED 575B B8D3"            /* Æñ;àÃ¹uSãíW[¸Ó */
	$"BC43 F69B 41A7 C36F AA48 D234 B761 E513"            /* ¼Cö›A§ÃoªHÒ4·aå. */
	$"ACB6 BE73 00AB 13EF 10C5 B0F2 7666 F8FF"            /* ¬¶¾s.«.ï.Å°òvføÿ */
	$"00F6 61F1 5F8C 3C7B E2BD 3229 3678 4B53"            /* .öañ_Œ<{â½2)6xKS */
	$"D165 D774 DBD6 B840 F0F8 B0D9 C7A7 4728"            /* Ñe×tÛÖ¸@ğø°ÙÇ§G( */
	$"8F3B D4AC 5009 9A4C 60B4 8467 2585 0066"            /* ;Ô¬PÆšL`´„g%….f */
	$"6BBF 04BE 31F8 9FC2 4FAC EADE 138C 78AF"            /* k¿.¾1øŸÂO¬êŞ.Œx¯ */
	$"C4BE 0BF1 F26B 76D1 DFDA 9583 55D5 22B1"            /* Ä¾.ñòkvÑßÚ•ƒUÕ"± */
	$"8B4E B412 7981 5F10 DA24 5E60 2507 9396"            /* ‹N´.y_.Ú$^`%.“– */
	$"6505 457B 17C4 CF06 7896 E7C2 BE00 D4BC"            /* e.E{.ÄÏ.x–çÂ¾.Ô¼ */
	$"3DA1 7F69 DE78 0F5B B0D6 66D1 A296 18E4"            /* =¡.iŞx.[°ÖfÑ¢–.ä */
	$"BC8A 2B59 ED65 8A16 9196 2F35 05C9 9103"            /* ¼Š+YíeŠ.‘–/5.É‘. */
	$"3AA9 6854 6E5C 8200 376C FC75 E3ED 462B"            /* :©hTn\‚.7lüuãíF+ */
	$"2BCB 7F83 DAAD B5BD DEB5 0D84 91DF EA96"            /* +Ë.ƒÚ­µ½Şµ.„‘ßê– */
	$"505C DB58 346C D2DF C912 C8EB B524 0104"            /* P\ÛX4lÒßÉ.Èëµ$.. */
	$"2AED 2B86 DDB5 4715 C8F8 1E3F 1F7C 2FD5"            /* *í+†İµG.Èø.?.|/Õ */
	$"3C49 E0E8 BE19 EA5A E26B 7E2E BFD6 ECB5"            /* <Iàè¾.êZâk~.¿Öìµ */
	$"BB6B CB38 6C4D ADFD C9B8 7FB4 B492 F9F1"            /* »kË8lM­ıÉ¸.´´’ùñ */
	$"C900 91E3 DA21 7DE2 38B6 13B8 EC00 E374"            /* É.‘ãÚ!}â8¶.¸ì.ãt */
	$"6F87 7F12 4FC2 FF00 06FE CF37 7E01 B8B6"            /* o‡..OÂÿ..şÏ7~.¸¶ */
	$"7F0B EB5A 54D7 5E2A 3756 86C5 ACF4 FBF8"            /* ..ëZT×^*7V†Å¬ôûø */
	$"EED6 EADF 6CA6 E0DC CEB0 0428 D129 5796"            /* îÖêßl¦àÜÎ°.(Ñ)W– */
	$"42CD B406 6D7B 4F84 7E33 4F0A 5BE9 73F8"            /* BÍ´.m{O„~3OÂ[ésø */
	$"723F B40F 8C77 5E2C 9633 3DB9 CE9A DACC"            /* r?´.Œw^,–3=¹ÎšÚÌ */
	$"D751 DC93 BF04 F92D 1BEC FF00 5838 1B43"            /* ×QÜ“¿.ù-.ìÿ.X8.C */
	$"0C00 0F42 F87B E14D 7B42 F88B F14F 5ED5"            /* ...Bø{áM{Bø‹ñO^Õ */
	$"2C3C 8B1F 126B B617 9A64 BE6C 6FE7 C31E"            /* ,<‹..k¶.šd¾loçÃ. */
	$"9163 6D23 6D56 2C98 9A09 570C 149D B900"            /* ‘cm#mV,˜šÆW..¹. */
	$"8209 F42A 00F9 BBFE 0A05 FF00 26CF AFFF"            /* ‚Æô*.ù»şÂ.ÿ.&Ï¯ÿ */
	$"00D7 45FF 00D1 7257 E31D 007E B6FE C4DE"            /* .×Eÿ.ÑrWã..~¶şÄŞ */
	$"37B2 D03F 66FD 0ADE 6995 595D 8E09 FF00"            /* 7²Ğ?fıÂŞi•Y]Æÿ. */
	$"623A F49D 63E3 9585 B165 5B95 FCE8 03CF"            /* b:ôcã•…±e[•üè.Ï */
	$"3C55 F1FE 236F 298E E785 52CC 73C0 1EA7"            /* <Uñş#o)ç…RÌsÀ.§ */
	$"D057 CF1F 103E 2A5C 6A44 3CB7 4512 E14B"            /* ĞWÏ..>*\jD<·E.áK */
	$"C39C 9330 C647 963F 8B39 1C8E 0641 240E"            /* Ãœ“0ÆG–?‹9..A$. */
	$"6803 8BD3 266B BB81 7120 3B43 EF8D 1886"            /* h.‹Ó&k»q ;Cï.† */
	$"20E3 1B89 0396 EB8E CA09 0339 2CDD E68C"            /*  ã.‰.–ëÊÆ.9,İæŒ */
	$"DC8A 00EA 6F74 3B6F 14F8 7353 F0D5 DB6D"            /* ÜŠ.êot;o.øsSğÕÛm */
	$"8754 B396 D1DB FBBB D4A8 6FC0 907F 0AFC"            /* ‡T³–ÑÛû»Ô¨oÀ.Âü */
	$"F5F1 3786 F56F 086B D7DE 1AD7 2D5A DEFB"            /* õñ7†õo.k×Ş.×-ZŞû */
	$"4F99 A195 0FA8 E8C3 D548 C107 B820 D006"            /* O™¡•.¨èÃÕHÁ.¸ Ğ. */
	$"6514 0052 AB6D 60DE 8734 01FB 0EBA AC3A"            /* e..R«m`Ş‡4.û.º¬: */
	$"CE89 A6EA F6EC 1A2B FB28 2E90 8E85 5E35"            /* Î‰¦êöì.+û(.…^5 */
	$"71FF 00A1 571B AECB C37E 3401 E73A D5CC"            /* qÿ.¡W.®ËÃ~4.ç:ÕÌ */
	$"90CB F688 6578 E68C E524 8D8A 3A9E C430"            /* ËöˆexæŒå$Š:Ä0 */
	$"E47E 15AB E13F DA6B C5FE 079E 3B6D 69DB"            /* ä~.«á?ÚkÅş.;miÛ */
	$"57D3 4300 C5B0 2E63 5EFB 5B20 3FAE 1B93"            /* WÓC.Å°.c^û[ ?®.“ */
	$"FDE1 401F 4978 07F6 85F0 BF8B ED2D EEF4"            /* ıá@.Ix.ö…ğ¿‹í-îô */
	$"FD5A 1759 DBCB 08CD B1D5 C754 653C 8619"            /* ıZ.YÛË.Í±ÕÇTe<†. */
	$"1C7B 8232 0835 EC1A 4EB9 6FA8 C41E 3707"            /* .{‚2.5ì.N¹o¨Ä.7. */
	$"3EF4 01AC 0E46 4514 0051 4015 34DD 234A"            /* >ô.¬.FE..Q@.4İ#J */
	$"D1A2 9A0D 234C B4B1 8AE2 E26B B992 DA14"            /* Ñ¢š.#L´±Šââk¹’Ú. */
	$"8564 9E57 324D 2B05 0017 7766 6663 CB33"            /* …dW2M+...wffcË3 */
	$"1249 26AD D001 4500 1450 0145 0014 5001"            /* .I&­Ğ.E..P.E..P. */
	$"4500 1450 0145 0014 5001 4500 1450 07CD"            /* E..P.E..P.E..P.Í */
	$"DFF0 502F F936 7D7F FEBA 2FFE 8B92 BF18"            /* ßğP/ù6}.şº/ş‹’¿. */
	$"E803 EACF 86FF 0017 A2F0 57C1 FB14 BFD4"            /* è.êÏ†ÿ..¢ğWÁû.¿Ô */
	$"0DB5 ADBE 4719 2CEE 5570 88A3 EF31 C703"            /* .µ­¾G.,îUpˆ£ï1Ç. */
	$"A772 4004 8F31 F15F ED2D E33D 65E4 8742"            /* §r@.1ñ_í-ã=eä‡B */
	$"D9A7 4049 0267 0269 C83C 7F16 5172 3D14"            /* Ù§@I.g.iÈ<..Qr=. */
	$"907A 3500 7236 7E23 D73C 4330 B8D7 358B"            /* z5.r6~#×<C0¸×5‹ */
	$"DBF7 0CD8 FB44 ED22 AEE6 DCC1 549C 2827"            /* Û÷.ØûDí"®æÜÁTœ(' */
	$"9C00 0577 9E1E 8A08 4030 C31A 1639 6DAA"            /* œ..w.Š.@0Ã..9mª */
	$"173F 5C75 A00F 46D1 1BA5 77DA 3B72 2803"            /* .?\u .FÑ.¥wÚ;r(. */
	$"BCD1 9BA7 359F F12B E01F 82FE 3458 A7F6"            /* ¼Ñ›§5Ÿñ+à.‚ş4X§ö */
	$"C2BE 9FAC 411E CB6D 52D9 4195 4764 914F"            /* Â¾Ÿ¬A.ËmRÙA•Gd‘O */
	$"1226 7B1C 11CE 08C9 A00F 983C 61FB 11FC"            /* .&{..Î.É .˜<aû.ü */
	$"6BF0 ECAF 2787 F4BB 6F14 D982 7649 A6CA"            /* kğì¯'‡ô»o.Ù‚vI¦Ê */
	$"04C4 7FB5 0484 3E7D 9777 D6BC 2B51 D3AF"            /* .Ä.µ.„>}—wÖ¼+QÓ¯ */
	$"B48B FB8D 2F53 B592 DAEE D246 8678 6418"            /* ´‹û/Sµ’ÚîÒF†xd. */
	$"68DD 4E19 48EC 41A0 0AF4 5007 BF78 47F6"            /* hİN.HìA ÂôP.¿xGö */
	$"D0F8 93E0 DF00 E9DE 05B3 D274 7BB6 D2A3"            /* Ğø“àß.éŞ.³Òt{¶Ò£ */
	$"FB3D ADFD DA4A F22C 03EE 2322 B856 2A3E"            /* û=­ıÚJò,.î#"¸V*> */
	$"507D 0004 1C66 BD53 E127 ED45 0FC4 FBC1"            /* P}...f½Sá'íE.ÄûÁ */
	$"E17F 15E9 D6FA 66B9 2AB1 B692 DCB0 B7BA"            /* á..éÖúf¹*±¶’Ü°·º */
	$"2064 A856 24A3 E012 0648 38EC 7008 0763"            /*  d¨V$£à..H8ìp..c */
	$"AFC9 F7BF 1AF3 6F10 4BC3 5007 96F8 8A72"            /* ¯É÷¿.óo.KÃP.–øŠr */
	$"9B99 18AB 7A83 835A FF00 08BF 6C1F 8B5F"            /* ›™.«zƒƒZÿ..¿l.‹_ */
	$"027C 416D FD9F ADDC 6B7E 1E47 1F68 D135"            /* .|AmıŸ­Ük~.G.hÑ5 */
	$"09DA 489A 3E98 8646 CB5B 9E49 1B3E 5CF2"            /* ÆÚHš>˜†FË[I.>\ò */
	$"55BA 5007 EB87 C11F 8C9E 11F8 EDF0 FAC3"            /* UºP.ë‡Á.Œ.øíğúÃ */
	$"E20F 836E 9A4B 4BAD D14F 0498 135A 5C2E"            /* â.ƒnšKK­ÑO.˜.Z\. */
	$"3CC8 6500 F0EB 907D 0AB2 B0C8 606B BEA0"            /* <Èe.ğë}Â²°È`k¾  */
	$"028A 002B E61B 5FDA 07C7 76F1 39BA BFB6"            /* .Š.+æ._Ú.Çvñ9º¿¶ */
	$"BA93 46D2 B51D 02F5 3ECA 886E 7C51 149A"            /* º“FÒµ..õ>Êˆn|Q.š */
	$"8087 8032 80AE 92C7 6001 185E C67A 6DC0"            /* €‡€2€®’Ç`..^ÆzmÀ */
	$"07A0 69FF 0018 357D 51B4 18AD F43B F5FB"            /* . iÿ..5}Q´.­ô;õû */
	$"578B 356F 0A5B CB24 D6C8 BAB5 D69F 6DAB"            /* W‹5oÂ[Ë$ÖÈºµÖŸm« */
	$"8999 C056 3042 D2E9 7B81 187D C40C 6C0D"            /* ‰™ÀV0BÒé{.}Ä.l. */
	$"BE8E 81F1 B7E2 06B3 E0FF 0083 3E23 3E07"            /* ¾ñ·â.³àÿ.ƒ>#>. */
	$"D2FE D1F1 2E58 56F6 35D4 1952 0493 48B8"            /* ÒşÑñ.XVö5Ô.R.“H¸ */
	$"D415 A2CA 9209 FB3B 1C36 EDAA A549 2CC1"            /* Ô.¢Ê’Æû;.6íª¥I,Á */
	$"9402 2D3B F6A9 D1F5 8D21 B5ED 2BC0 9E20"            /* ”.-;ö©Ñõ!µí+À  */
	$"BAB2 B96D 2CE9 52AA 2C6B 7F1D F6A1 6F63"            /* º²¹m,éRª,k..ö¡oc */
	$"0E1E 4DA9 1B93 750C A10B 1CC6 D9DC 0860"            /* ..M©.“u.¡..ÆÙÜ.` */
	$"BB7E 2CF8 ADAC 9F81 BF12 3C73 A4E9 AFA2"            /* »~,ø­¬Ÿ¿.<s¤é¯¢ */
	$"7883 C1FA 5EB8 AB14 CD1D D245 7B67 6F23"            /* xƒÁú^¸«.Í.ÒE{go# */
	$"C6EA 47CB 2212 2361 903A E081 CD00 733E"            /* ÆêGË".#a:àÍ.s> */
	$"1CF8 EB3F 84BC 5BAD F87F E22E A1AD 2693"            /* .øë?„¼[­ø.â.¡­&“ */
	$"6BB1 A1BC D7ED 2DAC EF2D 3FD1 27BA DF30"            /* k±¡¼×í-¬ï-?Ñ'ºß0 */
	$"B78E 385E DE65 B4BC 1148 A33B ED25 46F9"            /* ·8^Şe´¼.H£;í%Fù */
	$"8AD7 B578 6B56 B9D7 BC3B A5EB 979A 54FA"            /* Š×µxkV¹×¼;¥ë—šTú */
	$"64FA 8D94 3752 D8CE 4196 D5A4 40C6 2723"            /* dú”7RØÎA–Õ¤@Æ'# */
	$"8DCA 4ED3 8E32 0D00 7947 C59F 881A BF80"            /* ÊNÓ2..yGÅŸˆ.¿€ */
	$"7E24 58DF E9B0 B5F3 CDE1 A7B4 B4D3 E5B9"            /* ~$Xßé°µóÍá§´´Óå¹ */
	$"682D A5BE BAD5 F4BB 2B67 9480 42AA BDD0"            /* h-¥¾ºÕô»+g”€Bª½Ğ */
	$"DCFB 4954 2F8A E5FC 6BE3 DF8B 517C 49F0"            /* ÜûIT/Šåükãß‹Q|Iğ */
	$"2FC3 9BEB 7F0F 3789 E0F1 0C37 F6F7 3652"            /* /Ã›ë..7‰àñ.7ö÷6R */
	$"DCC3 A7DC D9DC E8DA FC78 B885 98C9 9864"            /* ÜÃ§ÜÙÜèÚüx¸…˜É˜d */
	$"B369 366E 225D B1ED 28C0 ED00 DCD3 3E2E"            /* ³i6n"]±í(Àí.ÜÓ>. */
	$"FC59 F126 B1A1 782B 45D2 3C29 0EB9 751F"            /* üYñ&±¡x+EÒ<).¹u. */
	$"8A23 D4EF 2E3E D2F6 70C9 A4DF D95A A4B1"            /* Š#Ôï.>ÒöpÉ¤ßÙZ¤± */
	$"4618 48C9 28BB E632 D946 61F3 3043 BF43"            /* F.HÉ(»æ2ÙFaó0C¿C */
	$"C6DE 3CD4 FC59 FB22 78A3 E22C 08FA 46A7"            /* ÆŞ<ÔüYû"x£â,.úF§ */
	$"7DF0 FB52 D480 B49D B36B 73F6 0958 98A5"            /* }ğûRÔ€´³ksöÆX˜¥ */
	$"1B5B E570 76B7 0780 700F 400E 5BC1 D77A"            /* .[åpv·.€p.@.[Á×z */
	$"A781 3E25 6A3B FC29 AF78 32CB FE11 0D42"            /* §>%j;ü)¯x2Ëş..B */
	$"F6DB 4DF1 178A DB51 B1D5 AE20 96DD CCE6"            /* öÛMñ.ŠÛQ±Õ® –İÌæ */
	$"733D C476 5F67 562A CD90 CE97 258A B084"            /* s=Äv_gV*ÍÎ—%Š°„ */
	$"639F F187 C5FF 001E 7C47 F0AE BDE0 CD2B"            /* cŸñ‡Åÿ..|Gğ®½àÍ+ */
	$"54D0 84B6 FABF 8352 5D52 D74F D52C ADAF"            /* TĞ„¶ú¿ƒR]R×OÕ,­¯ */
	$"2C35 6D61 ACA4 8231 2BC7 23AE 60DA F3C4"            /* ,5ma¬¤‚1+Ç#®`ÚóÄ */
	$"E524 8E49 514A 4809 400D BD42 E7C5 3E03"            /* å$IQJHÆ@.½BçÅ>. */
	$"F8A7 E21F 1578 8A7D 22F7 C3FF 000B 3E1B"            /* ø§â..xŠ}"÷Ãÿ..>. */
	$"E9BA BC7A 7DA4 57A8 E81A 1D5D 2E3E CC1A"            /* éº¼z}¤W¨è..].>Ì. */
	$"E590 4923 D880 5A55 90F9 4B0A 821D 5DDF"            /* åI#Ø€ZUùKÂ‚.]ß */
	$"57C5 FF00 1A3E 30FC 3AFE C9D3 3C4B E1BF"            /* WÅÿ..>0ü:şÉÓ<Ká¿ */
	$"08DF 6ADE 287B 0FEC 88EC 2EAE 1208 5E4D"            /* .ßjŞ({.ìˆì.®..^M */
	$"574D B19E DEE1 DC31 6C2E A68C 9708 02E5"            /* WM±ŞáÜ1l.¦Œ—..å */
	$"0EE8 F950 C01D 45F7 8CFE 2D9F 123F 8234"            /* .èùPÀ.E÷Œş-Ÿ.?‚4 */
	$"6B3F 094F AB68 7A25 BEB9 ACDC 5C7D A61B"            /* k?ÆO«hz%¾¹¬Ü\}¦. */
	$"7B85 B9B8 BB8A DADE D46E 2622 4594 DBE5"            /* {…¹¸»ŠÚŞÔn&"E”Ûå */
	$"72E1 494F 9086 E357 F67A BAB9 BEF8 05F0"            /* ráIO†ãWözº¹¾ø.ğ */
	$"CEF6 F6E2 5B8B 8B8F 0768 D2CB 2CAE 5DE4"            /* Îööâ[‹‹.hÒË,®]ä */
	$"76B2 84B3 331E 4924 9249 E49A 00F4 1A28"            /* v²„³3.I$’Iäš.ô.( */
	$"00A2 803E 6EFF 0082 817F C9B3 EBFF 00F5"            /* .¢€>nÿ.‚.É³ëÿ.õ */
	$"D17F F45C 95F8 C740 1BDE 25B8 9CE9 BA05"            /* Ñ.ô\•øÇ@.Ş%¸œéº. */
	$"8B4A C608 B4FF 0035 63C9 DA1D E470 CD8E"            /* ‹JÆ.´ÿ.5cÉÚ.äpÍ */
	$"9B88 48C1 3DC2 2FA5 60D0 06E7 875B 0D8F"            /* ›ˆHÁ=Â/¥`Ğ.ç‡[. */
	$"7AF4 ED09 FE55 A00F 43D1 1BA5 77FA 3B72"            /* zôíÆşU .CÑ.¥wú;r */
	$"B401 DE68 C7EE D6FE B1E3 8F0B FC3F D15F"            /* ´.ŞhÇîÖş±ã.ü?Ñ_ */
	$"C45E 2FD6 60D3 AC63 0406 90E5 A56E BB23"            /* Ä^/Ö`Ó¬c..å¥n»# */
	$"41F3 3B7B 0068 03E4 5F8D 5FB6 6F8C 3C79"            /* Aó;{.h.ä__¶oŒ<y */
	$"1DC7 867C 04D7 1E1B F0F3 E639 2547 C5ED"            /* .Ç†|.×..ğóæ9%GÅí */
	$"E2F7 DF22 FF00 AA53 FDC4 3F56 20E2 BE72"            /* â÷ß"ÿ.ªSıÄ?V â¾r */
	$"2493 9340 0514 0057 4DF0 D749 D6B5 9F1D"            /* $““@...WMğ×IÖµŸ. */
	$"E876 9A04 7235 E25E C530 64E3 CB54 60CC"            /* èvš.r5â^Å0dãËT`Ì */
	$"E4F6 5500 9268 03ED 3F10 CA0B 3E3A 1271"            /* äöU.’h.í?.Ê.>:.q */
	$"5E69 E229 786F C680 3CB3 C4B2 F0FF 008D"            /* ^iâ)xoÆ€<³Ä²ğÿ. */
	$"796E A2FB EEDC FA50 07E8 8FFC 123E F2E1"            /* yn¢ûîÜúP.èü.>òá */
	$"E0F8 9DA7 967F 2217 D266 03CC 3B43 B8BB"            /* àø§–.".Òf.Ì;C¸» */
	$"0485 E80E 2319 3DF0 3D05 7E89 D001 4500"            /* .…è.#.=ğ=.~‰Ğ.E. */
	$"15C7 CBF0 83E1 DCC6 E4C9 E1B8 C9BC F114"            /* .ÇËğƒáÜÆäÉá¸É¼ñ. */
	$"3E2B 98F9 F30D FAAC 5E5F 9771 F7F8 C793"            /* >+˜ùó.ú¬^_—q÷øÇ“ */
	$"1FC8 3E43 8395 3B9B 2017 6D7E 1DF8 3ECB"            /* .È>Cƒ•;› .m~.ø>Ë */
	$"FB33 ECDA 46CF EC6D 6AFB C436 5FE9 131F"            /* û3ìÚFÏìmjûÄ6_é.. */
	$"2B50 BCFB 5FDA A6E5 FE6D FF00 DA17 9F23"            /* +P¼û_Ú¦åşmÿ.Ú.Ÿ# */
	$"6517 CDF9 5576 A6DC F83E 0F78 06D7 C3DA"            /* e.ÍùUv¦Üø>.x.×ÃÚ */
	$"5785 6D74 CBE8 74DD 0AF1 6FB4 A48F 57BE"            /* W…mtËètİÂño´¤W¾ */
	$"4934 F956 3689 45B4 C26F 3208 C452 4910"            /* I4ùV6‰E´Âo2.ÄRI. */
	$"8919 6311 BB20 5DAC 4100 8ECB E0AF C38D"            /* ‰.c.» ]¬A.Ëà¯Ã */
	$"3E0F B25A E877 02D5 6EAD 2F21 B67D 4EF2"            /* >.²Zèw.Õn­/!¶}Nò */
	$"482D A4B6 BB8E F60F B3C4 D314 B745 B986"            /* H-¤¶»ö.³ÄÓ.·E¹† */
	$"290A 4615 1B62 AB2B 28DB 5AD7 BF0F BC23"            /* )ÂF..b«+(ÛZ×¿.¼# */
	$"A878 77C4 5E13 BCD2 7CCD 2BC5 62EC 6AF6"            /* ¨xwÄ^.¼Ò|Í+Åbìjö */
	$"FE7C ABF6 9174 863B 8F9C 3EE4 DC84 8F90"            /* ş|«ö‘t†;œ>äÜ„ */
	$"AE3A AE0D 0071 5E2F FD9E 3C2D E2AB 11A1"            /* ®:®..q^/ı<-â«.¡ */
	$"799F F12A D42F ACAF 35D3 A9B5 D6B1 7F7E"            /* yŸñ*Ô/¬¯5Ó©µÖ±.~ */
	$"9692 ACB6 D047 7975 72CD 0441 84AA CBB5"            /* –’¬¶ĞGyurÍ.A„ªËµ */
	$"C159 E509 E5B3 B31F 56A0 0F3F D2FE 037C"            /* ÁYåÆå³³.V .?Òş.| */
	$"33D2 EDB5 1B13 A2DD 5FDA EA56 234B 6B7D"            /* 3Òíµ..¢İ_ÚêV#Kk} */
	$"4B51 BABF 8EDE C836 F16F 6CB3 48C2 DE30"            /* KQº¿ŞÈ6ñol³HÂŞ0 */
	$"D860 23DB 8289 CFEE D36D AD07 E0BF C3BF"            /* Ø`#Û‚‰ÏîÓm­.à¿Ã¿ */
	$"0D5E 59EA 7A76 8D75 26A1 637E 3538 6FAF"            /* .^Yêzvu&¡c~58o¯ */
	$"B54B DD42 ECDC 0B6B 8B45 325C 5C4C F2CA"            /* µKİBìÜ.k‹E2\\LòÊ */
	$"8B05 DDCA 2C6E CC89 E6B1 5507 0400 69E9"            /* ‹.İÊ,nÌ‰æ±U...ié */
	$"7F0E FC21 A26B 71F8 934C D23C 9D46 2FED"            /* ..ü!¢kqø“LÒ<F/í */
	$"2D93 7DA2 67C7 F684 F05C DEFC ACE5 7F79"            /* -“}¢gÇö„ğ\Şü¬å.y */
	$"35B4 0DD3 E5D9 85DA 0B02 37C3 BF08 3780"            /* 5´.ÓåÙ…Ú..7Ã¿.7€ */
	$"67F8 5C74 8FF8 A62E 74C9 7479 6C7E D137"            /* gø\tø¦.tÉtyl~Ñ7 */
	$"CD67 246D 1C91 F9BB FCC1 9466 1B83 6E19"            /* Íg$m.‘ù»üÁ”f.ƒn. */
	$"C820 D006 3E9B F043 E1CE 9CBA 909B 4BD4"            /* È Ğ.>›ğCáÎœº›KÔ */
	$"B576 D5B4 E9B4 8BA9 35ED 6F51 D724 6B29"            /* µvÕ´é´‹©5íoQ×$k) */
	$"B1E7 5BA3 DF4F 3347 1498 5DE8 8543 6D52"            /* ±ç[£ßO3G.˜]è…CmR */
	$"7254 622D 1FE0 27C3 0D08 DDBD 9E8B A84D"            /* rTb-.à'Ã..İ½‹¨M */
	$"2DFC DA5C F753 DF6B 7A8E A134 CFA6 DD1B"            /* -üÚ\÷Sßkz¡4Ï¦İ. */
	$"BB02 F2DC 4EEE DE54 E770 04E0 8C21 CA00"            /* ».òÜNîŞTçp.àŒ!Ê. */
	$"B401 D537 84FC 3D26 B3A9 EBF3 6971 4B7B"            /* ´.Õ7„ü=&³©ëóiqK{ */
	$"ACE9 D6FA 4DFB 4A5A 449E D207 B878 A268"            /* ¬éÖúMûJZDÒ.¸x¢h */
	$"D894 C06B CB9C E172 C24C 3120 281C A697"            /* Ø”ÀkËœárÂL1 (.¦— */
	$"F00B E15E 8FFF 001E 9E1D B890 ACB6 52C2"            /* ğ.á^ÿ...¸¬¶RÂ */
	$"D77A ADF5 E35B 0B4B A8AE EDA2 B769 A673"            /* ×z­õã[.K¨®í¢·i¦s */
	$"6F02 5C5B C127 911E D898 C6A1 9081 8A00"            /* o.\[Á'‘.Ø˜Æ¡Š. */
	$"D6F1 7FC2 CF04 F8EE FADF 53F1 2699 7325"            /* Öñ.ÂÏ.øîúßSñ&™s% */
	$"D5BC 2D6D E6DA EA37 7606 6B76 3968 27FB"            /* Õ¼-mæÚê7v.kv9h'û */
	$"3CA9 F688 09C9 30CB BE33 9395 E4D6 D786"            /* <©öˆÆÉ0Ë¾3“•äÖ×† */
	$"FC3B A3F8 43C3 BA5F 84FC 3D67 F64D 2B45"            /* ü;£øCÃº_„ü=göM+E */
	$"B283 4EB1 B7F3 1E4F 2ADE 1458 E24D EE4B"            /* ²ƒN±·ó.O*Ş.XâMîK */
	$"3611 5465 8927 1924 9A00 D1A2 800A 2803"            /* 6.Te‰'.$š.Ñ¢€Â(. */
	$"E6EF F828 17FC 9B3E BFFF 005D 17FF 0045"            /* æïø(.ü›>¿ÿ.].ÿ.E */
	$"C95F 8C74 01BD E29F 2BC9 D0BC A915 C7F6"            /* É_Œt.½âŸ+ÉĞ¼©.Çö */
	$"4459 2A73 86F3 24C8 FA83 5834 01B1 E1F6"            /* DY*s†ó$ÈúƒX4.±áö */
	$"C484 7BD7 A768 2DF2 AFE1 401E 87A1 B7DD"            /* Ä„{×§h-ò¯á@.‡¡·İ */
	$"FA57 7FA3 1C95 C77A 00E6 BE20 7ED2 3E1D"            /* úW.£.•Çz.æ¾ ~Ò>. */
	$"F004 5269 7E1D 5875 AD71 4152 AAD9 B6B7"            /* ğ.Ri~.Xu­qARªÙ¶· */
	$"6FFA 68E3 EF11 FDC5 3F52 2BE5 9F18 F8EB"            /* oúhãï.ıÅ?R+åŸ.øë */
	$"C53E 3FD5 9B5B F156 AF35 F5C1 C840 C711"            /* Å>?Õ›[ñV¯5õÁÈ@Ç. */
	$"C2BF DC8D 07CA 8BEC 07D7 2680 30A8 A002"            /* Â¿Ü.Ê‹ì.×&€0¨ . */
	$"8A00 96D2 D2E6 FEEA 1B1B 2B79 27B8 B891"            /* Š.–ÒÒæşê..+y'¸¸‘ */
	$"628A 28D4 B33B B1C2 AA81 C924 9031 5F6E"            /* bŠ(Ô³;±ÂªÉ$1_n */
	$"7C1E F837 6FF0 9BC3 266D 4912 4F11 EA51"            /* |.ø7oğ›Ã&mI.O.êQ */
	$"86BE 9460 8817 A881 0FA0 FE23 FC4D EC05"            /* †¾”`ˆ.¨. ş#üMì. */
	$"005D D7A5 FBDF 8D79 9F88 A5E1 B9F5 A00F"            /* .]×¥ûßyŸˆ¥á¹õ . */
	$"2BF1 34B8 57FA 579A DC36 E9DD BFDA 3401"            /* +ñ4¸WúWšÜ6éİ¿Ú4. */
	$"F627 FC13 BBE3 FF00 C3FF 0081 377E 3C9F"            /* ö'ü.»ãÿ.Ãÿ.7~<Ÿ */
	$"E216 A8F6 36BA C47A 6ADB B2A2 B6E9 2237"            /* â.¨ö6ºÄzjÛ²¢¶é"7 */
	$"2482 4B0C 7127 E3F8 57DA 9FF0 F0AF D9A3"            /* $‚K.q'ãøWÚŸğğ¯Ù£ */
	$"FE86 E9BF EFDC 7FFC 7280 0FF8 7857 ECD1"            /* ş†é¿ïÜ.ür€.øxWìÑ */
	$"FF00 4374 DFF7 EE3F FE39 47FC 3C2B F668"            /* ÿ.Ctß÷î?ş9Gü<+öh */
	$"FF00 A1BA 6FFB F71F FF00 1CA0 03FE 1E15"            /* ÿ.¡ºoû÷.ÿ.. .ş.. */
	$"FB34 7FD0 DD37 FDFB 8FFF 008E 51FF 000F"            /* û4.Ğİ7ıûÿ.Qÿ.. */
	$"0AFD 9A3F E86E 9BFE FDC7 FF00 C728 00FF"            /* Âıš?èn›şıÇÿ.Ç(.ÿ */
	$"0087 857E CD1F F437 4DFF 007E E3FF 00E3"            /* .‡…~Í.ô7Mÿ.~ãÿ.ã */
	$"947F C3C2 BF66 8FFA 1BA6 FF00 BF71 FF00"            /* ”.ÃÂ¿fú.¦ÿ.¿qÿ. */
	$"F1CA 003F E1E1 5FB3 47FD 0DD3 7FDF B8FF"            /* ñÊ.?áá_³Gı.Ó.ß¸ÿ */
	$"00F8 E51F F0F0 AFD9 A3FE 86E9 BFEF DC7F"            /* .øå.ğğ¯Ù£ş†é¿ïÜ. */
	$"FC72 800F F878 57EC D1FF 0043 74DF F7EE"            /* ür€.øxWìÑÿ.Ctß÷î */
	$"3FFE 3947 FC3C 2BF6 68FF 00A1 BA6F FBF7"            /* ?ş9Gü<+öhÿ.¡ºoû÷ */
	$"1FFF 001C A003 FE1E 15FB 347F D0DD 37FD"            /* .ÿ.. .ş..û4.Ğİ7ı */
	$"FB8F FF00 8E51 FF00 0F0A FD9A 3FE8 6E9B"            /* ûÿ.Qÿ..Âıš?èn› */
	$"FEFD C7FF 00C7 2800 FF00 8785 7ECD 1FF4"            /* şıÇÿ.Ç(.ÿ.‡…~Í.ô */
	$"374D FF00 7EE3 FF00 E394 7FC3 C2BF 668F"            /* 7Mÿ.~ãÿ.ã”.ÃÂ¿f */
	$"FA1B A6FF 00BF 71FF 00F1 CA00 3FE1 E15F"            /* ú.¦ÿ.¿qÿ.ñÊ.?áá_ */
	$"B347 FD0D D37F DFB8 FF00 F8E5 1FF0 F0AF"            /* ³Gı.Ó.ß¸ÿ.øå.ğğ¯ */
	$"D9A3 FE86 E9BF EFDC 7FFC 7280 0FF8 7857"            /* Ù£ş†é¿ïÜ.ür€.øxW */
	$"ECD1 FF00 4374 DFF7 EE3F FE39 47FC 3C2B"            /* ìÑÿ.Ctß÷î?ş9Gü<+ */
	$"F668 FF00 A1BA 6FFB F71F FF00 1CA0 03FE"            /* öhÿ.¡ºoû÷.ÿ.. .ş */
	$"1E15 FB34 7FD0 DD37 FDFB 8FFF 008E 51FF"            /* ..û4.Ğİ7ıûÿ.Qÿ */
	$"000F 0AFD 9A3F E86E 9BFE FDC7 FF00 C728"            /* ..Âıš?èn›şıÇÿ.Ç( */
	$"00FF 0087 857E CD1F F437 4DFF 007E E3FF"            /* .ÿ.‡…~Í.ô7Mÿ.~ãÿ */
	$"00E3 947F C3C2 BF66 8FFA 1BA6 FF00 BF71"            /* .ã”.ÃÂ¿fú.¦ÿ.¿q */
	$"FF00 F1CA 003F E1E1 5FB3 47FD 0DD3 7FDF"            /* ÿ.ñÊ.?áá_³Gı.Ó.ß */
	$"B8FF 00F8 E51F F0F0 AFD9 A3FE 86E9 BFEF"            /* ¸ÿ.øå.ğğ¯Ù£ş†é¿ï */
	$"DC7F FC72 803C 6BF6 B7FD AF3E 07FC 5CF8"            /* Ü.ür€<kö·ı¯>.ü\ø */
	$"1FAE 783B C1FE 2959 7549 479D 1453 6C4F"            /* .®x;Áş)YuIG.SlO */
	$"336A 382A A431 CB7C DC0E F8F5 C0AF CC8A"            /* 3j8*¤1Ë|Ü.øõÀ¯ÌŠ */
	$"00FF D900 0098 000A 0000 0000 002C 0045"            /* .ÿÙ..˜.Â.....,.E */
	$"0000 0000 002C 0045 0000 0000 0090 0168"            /* .....,.E......h */
	$"0000 08FD FF02 0000 FEFE 000B 0780 7FFF"            /* ...ıÿ...şş...€.ÿ */
	$"FF00 07FF C0FF 000B 0780 7FFF FF00 1FFF"            /* ÿ..ÿÀÿ...€.ÿÿ..ÿ */
	$"F0FF 000B 0780 7FFF FF00 7FFF FCFF 000B"            /* ğÿ...€.ÿÿ..ÿüÿ.. */
	$"0780 7FFF FF00 FFFF FEFF 000A 0480 7FC0"            /* .€.ÿÿ.ÿÿşÿ.Â.€.À */
	$"FF01 FEFF FF00 0B09 887F 003F 03FF 01FF"            /* ÿ.şÿÿ..Æˆ..?.ÿ.ÿ */
	$"8000 0B09 887E 001F 03FC 007F 8000 0B09"            /* €..Æˆ~...ü..€..Æ */
	$"887C 000F 07F8 003F C000 0B09 8078 1C07"            /* ˆ|...ø.?À..Æ€x.. */
	$"07F0 001F C000 0B09 8078 1C07 0FE0 000F"            /* .ğ..À..Æ€x...à.. */
	$"E000 0B09 8070 1C03 0FC0 0007 E000 0B09"            /* à..Æ€p...À..à..Æ */
	$"8070 1C03 1FC0 0007 F000 0B09 8070 1C03"            /* €p...À..ğ..Æ€p.. */
	$"1F80 0003 F000 0B09 8070 1C03 1F80 0003"            /* .€..ğ..Æ€p...€.. */
	$"F000 0B09 8070 1C03 1F80 3FFC F000 0B09"            /* ğ..Æ€p...€?üğ..Æ */
	$"8070 1C03 1F80 27FC F000 0B09 8070 0803"            /* €p...€'üğ..Æ€p.. */
	$"1F80 3FFC F000 0B09 8070 0003 1F80 0003"            /* .€?üğ..Æ€p...€.. */
	$"F000 0B09 8070 0003 1F80 0003 F000 0B09"            /* ğ..Æ€p...€..ğ..Æ */
	$"87F0 1C03 1FC0 0007 F000 0B09 81F0 1C03"            /* ‡ğ...À..ğ..Æğ.. */
	$"0FC0 0007 E000 0B09 81F0 1C07 0FE0 000F"            /* .À..à..Æğ...à.. */
	$"E000 0B09 81F0 0007 07F0 001F C000 0B09"            /* à..Æğ...ğ..À..Æ */
	$"81F0 000F 07F8 003F C000 0B09 81E0 001F"            /* ğ...ø.?À..Æà.. */
	$"03FC 007F 8000 0B09 8F80 007F 03FF 01FF"            /* .ü..€..Æ€...ÿ.ÿ */
	$"C000 0B00 81FE FF00 01FE FF01 E000 0B00"            /* À...şÿ..şÿ.à... */
	$"81FE FF00 00FE FF01 F000 0B00 81FE FF05"            /* şÿ..şÿ.ğ...şÿ. */
	$"007F FFFF F000 0B00 81FE FF05 001F FFFF"            /* ..ÿÿğ...şÿ...ÿÿ */
	$"F000 09FD FF05 0007 FFCF F000 08FC 0004"            /* ğ.Æıÿ...ÿÏğ..ü.. */
	$"01FF 03F0 0002 F700 0B09 3800 0C7C 0000"            /* .ÿ.ğ..÷..Æ8..|.. */
	$"3DCF F800 0B09 4402 0454 8000 1291 A800"            /* =Ïø..ÆD..T€..‘¨. */
	$"0B09 8200 0410 0000 12A0 2000 0B09 83B6"            /* .Æ‚......  ..Æƒ¶ */
	$"7591 B7D8 1CA0 2000 0B09 8292 9510 9A64"            /* u‘·Ø.  ..Æ‚’•.šd */
	$"10A0 2000 0B09 8292 8710 927C 10A0 2000"            /* .  ..Æ‚’‡.’|.  . */
	$"0B09 4492 8510 9260 1091 2000 0B09 38FF"            /* .ÆD’….’`.‘ ..Æ8ÿ */
	$"7FB9 FB7C 39CF 7000 0400 18F8 0004 000E"            /* .¹û|9Ïp....ø.... */
	$"F800 00FF"                                          /* ø..ÿ */
};

