#include <Carbon/Carbon.r>

resource 'DITL' (1541, purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{35, 15, 50, 215},
		CheckBox {
			enabled,
			"Display userlist in channel windows"
		},
		/* [2] */
		{50, 25, 65, 225},
		CheckBox {
			enabled,
			"Display on right side of window"
		},
	}
};

resource 'dftb' (1541, purgeable) {
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
		}
	}
};
