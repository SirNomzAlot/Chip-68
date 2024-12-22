#include "menu.h"
#include "core.h"
#include "textutils.h"
#include "screen.h"
#include "files.h"
#include "interpreter.h"

#include <Events.h>

Handle menubar;

void menuInit() {
	menubar = GetNewMBar(128);
    SetMenuBar(menubar);
    DrawMenuBar();
}

void menuBarHandler(EventRecord* event) {
	short menuID, itemNum;
	long var = MenuSelect(event->where);
	menuID = HiWord(var);
	itemNum = LoWord(var);
	switch (menuID) {
	case 128:
		inAppleMenu(itemNum);
		return;
	case 129:
		inFileMenu(itemNum);
		return;
	case 130:
		inEditMenu(itemNum);
		return;
	case 131:
		inWindowMenu(itemNum);
		return;
	case 132:
		inInterpreterMenu(itemNum);
		return;
	default:
		mprintf("%d is menu id", menuID);
	}
}

void inAppleMenu(short itemNum) {
	mprintf("%d is 128 item num", itemNum);
}

void inFileMenu(short itemNum) {
	switch (itemNum) {
	case 1: // reset
		reset();
		currKey = '\0';
		return;
	case 2: // load PRGM
		tryOpenRead();
		return;
	case 3: // new PRGM
		return;
	case 5: // quit
		shouldClose=true;
		closeFile();
		interpreterCleanup();
		screenCleanup();
		restoreEventMask();
		ExitToShell();
		return;
	default:
		return;
	}
}

void inEditMenu(short itemNum) {
	return; // theres no editing rn
}

void inWindowMenu(short itemNum) {
	switch (itemNum) {
	case 1:
		setMultiplier(0);
		return;
	case 2:
		setMultiplier(2);
		return;
	case 3:
		setMultiplier(4);
		return;
	default:
		return;
	}
}

void inInterpreterMenu(short itemNum) {
	switch (itemNum) {
	case 1:	// toggle compat
		chipMode();
		return;
	case 2:
		superMode();
		return;
	case 3:
		superModernMode();
		return;
	case 4:
		xoMode();
		return;
	case 5:	// divider thing
		return;
	case 6:	// step
		clock=1;
		step = true;
		renderCPU();
		interpretForTick();
		return;
	case 7:	// divider thing
		return;
	case 8:	// clock slow
		clock=CLOCK_SLOW;
		step = false;
		return;
	case 9:	// clock medium
		clock=CLOCK_MEDIUM;
		step = false;
		return;
	case 10:	// clock normal
		clock=CLOCK_NORMAL;
		step = false;
		return;
	case 11:	// clock ludicrous
		clock=CLOCK_LUDICROUS; // we brake for nobody, exept the cpu limits
		step = false;
		return;
	default:
		return;
	}
}
