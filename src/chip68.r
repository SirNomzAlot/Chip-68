#include "Menus.r"
#include "Windows.r"
#include "Processes.r"

resource 'MENU' (128) {
	128, textMenuProc;
	allEnabled, enabled;
	apple;
	{
		"About Chip-68...", noIcon, noKey, noMark, plain;
		"-", noIcon, noKey, noMark, plain;
		//"Toggle Super Compat", noIcon, noKey, noMark, plain;
	}
};

resource 'MENU' (129) {
    129, textMenuProc;
    allEnabled, enabled;
    "File";
    {
    	"Reset", noIcon, noKey, noMark, plain;
        "Load PRGM", noIcon, "L", noMark, plain;
        "New PRGM", noIcon, "N", noMark, plain;
        "-", noIcon, noKey, noMark, plain;
        "Quit", noIcon, "Q", noMark, plain;
    }
};

resource 'MENU' (130) {
    130, textMenuProc;
    0, enabled;
    "Edit";
    {
        "Undo", noIcon, "Z", noMark, plain;
        "-", noIcon, noKey, noMark, plain;
        "Cut", noIcon, "X", noMark, plain;
        "Copy", noIcon, "C", noMark, plain;
        "Paste", noIcon, "V", noMark, plain;
        "Clear", noIcon, noKey, noMark, plain;
    }
};

resource 'MENU' (131) {
	131, textmenuProc;
	allEnabled, enabled; // nah
	"Window";
	{	"Scale x4", noIcon, noKey, noMark, plain;
		"Scale x6", noIcon, noKey, noMark, plain;
		"Scale x8", noIcon, noKey, noMark, plain;
	}
};

resource 'MENU' (132) {
	132, textmenuProc;
	allEnabled, enabled;
	"Interpreter";
	{
		"Chip-8 Mode", noIcon, noKey, noMark, plain;
		"SChip-8 Mode", noIcon, noKey, noMark, plain;
		"SChip v1.1 Mode", noIcon, noKey, noMark, plain;
		"XO-Chip Mode", noIcon, noKey, noMark, plain;
		"-", noIcon, noKey, noMark, plain;
		"Step", noIcon, "S", noMark, plain;
		"-", noIcon, noKey, noMark, plain;
		"Speed Slow", noIcon, noKey, noMark, plain;
		"Speed Medium", noIcon, noKey, noMark, plain;
		"Speed Normal", noIcon, noKey, noMark, plain;
		"Speed Ludicrous", noIcon, noKey, noMark, plain;
	}
};

resource 'MBAR' (128) {
	{128,129,130,131, 132}
};

data 'TEXT' (128) {
	"About Chip-68\r\r"
	"	By Jonathan Ekman\r\r"
	"Chip 8 interpreter for 68k Macs\r\r"
	"Supports Original-ish Chip 8 and\r\r"
	"Super Chip 8"
};

resource 'WIND' (128) {
	{0,0,220,320}, altDBoxProc;
	invisible;
	noGoAway;
	0,"";
	noAutoCenter;
};

data 'WDEF' (128) {
    $"2F3A 0004 4E75 0000 0000"
};

resource 'SIZE' (-1) {
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	doesActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	is32BitCompatible,
	isHighLevelEventAware,
	onlyLocalHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
	200 * 1024,
	200 * 1024
};
