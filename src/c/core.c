#include "core.h"
#include "textutils.h"
#include "interpreter.h"
#include "opcode.h"
#include "files.h"
#include "screen.h"
#include "menu.h"

#include <Quickdraw.h>
#include <MacMemory.h>
#include <Sound.h>
#include <Events.h>
#include <Fonts.h>
#include <Files.h>
#include <NumberFormatting.h>
#include <ToolUtils.h>
#include <OSUtils.h>

#include <StandardFile.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

WindowPtr window;
MenuHandle apple_menu;
QDGlobals qd;
Rect windowRect;
EventRecord event;

bool multifinder = false;

bool shouldClose = false;
tickData timer;
int loopCount = 0;
char currKey;

bool draw = true;
short sx = 0;
short sy = 0;

uint16_t oldSysMask;
uint8_t oldclock;

void coreInit() {
	SysEnvRec sys;
	toolboxInit();
	windowInit();
	menuInit();
	TextFont(22);
	TextSize(9);
	TextMode(0);
	oldSysMask = *((uint16_t*)0x144);
	SetEventMask(oldSysMask|keyUpMask);
	timer=(tickData){TickCount(),0,0.0f};
	SysEnvirons(1, &sys);
	if (sys.machineType>=1) {
		if (NGetTrapAddress(0xA860, 1)!=(void*)0xA89F) {
			multifinder=true;
		}
		return;
	}
	shouldClose=true;
}

void toolboxInit() {
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
	InitCursor();
	FlushEvents(everyEvent, 0);
}

void windowInit() {
	SetRect(&windowRect, windowX, windowY, windowX+scale*64, windowY+scale*32);
	window = NewWindow(NULL, &windowRect, "\pChip-68", true, windowId, (WindowPtr)-1, true, 0);
	if (window==NULL) {
		shouldClose=true;
		return;
	}
	SetPort(window);
}

void doLoop() {
	if (!multifinder) {
		SystemTask();
		if (!GetNextEvent(everyEvent, &event)) {
			return;
		}
	} else if (!WaitNextEvent(everyEvent, &event, max_sleep, NULL)) {
		return;
	}
	eventHandler(&event);
	if (!step) {
		interpretForTick();
	}
	decTimers();
}

void waitTillClose() {
	if (!multifinder) {
		SystemTask();
		if (!GetNextEvent(everyEvent, &event)) {
			return;
		}
	} else if (!WaitNextEvent(everyEvent, &event, max_sleep, NULL)) {
		return;
	}
	eventHandler(&event);
}

void resize(short x, short y) {
	if (x<0||y<0) {
		return;
	}
	if (x==sx&&y==sy) {
		return;
	}
	sx=x;
	sy=y;
	SizeWindow(FrontWindow(), x, y, true);
}

void eventHandler(EventRecord* event) {
	switch(event->what) {
		case keyDown:
			keyDownHandler(event);
			return;
		case keyUp:
			keyUpHandler(event);
			return;
		case autoKey:
			autoKeyHandler(event);
			return;
		case mouseDown:
			mouseDownHandler(event);
			return;
		case updateEvt:
			//raster();
			return;
		case activateEvt:
			raster();
			return;
		case app4Evt:
			if (event->message&0x01000000) {
				if (event->message&0x1) {
					clock=oldclock;
					raster();
					return;
				}
				oldclock=clock;
				clock=0;
			}
			return;
		default:
			return;
	}
}

void keyDownHandler(EventRecord* event) {
	currKey = event->message;
	if (currKey=='p') {
		renderCPU();
	}
	if (step&&currKey==' ') {
		renderCPU();
		interpretForTick();
	}
}

void keyUpHandler(EventRecord* event) {
	currKey = '\0';
}

void autoKeyHandler(EventRecord* event) {
	if (step&&currKey==' ') {
		renderCPU();
		interpretForTick();
	}
}
void mouseDownHandler(EventRecord* event) {
	WindowPtr eventWindow;
	short int object;
	long menuChoice;

	object = FindWindow(event->where, &window);
	switch (object) {
	case inMenuBar:
		menuBarHandler(event);
		HiliteMenu(0);
		return;
	case inContent:
		//render();
		return;
	case inDrag:
		if (window == FrontWindow()) {
			BeginUpdate(window);
			DragWindow(window, event->where, &qd.screenBits.bounds);
			EndUpdate(window);
		}
		return;
	case inGoAway:
		if (TrackGoAway(window, event->where)) {
			shouldClose=true;
		}
		return;
	}
}

void updateTimer() {
	long int newTime = TickCount();
	timer.deltaTick=(newTime-timer.ticksTotal-timer.ticksStart);
	timer.ticksTotal=(newTime-timer.ticksStart);
}

void restoreEventMask() {
	SetEventMask(oldSysMask);
}

void coreCleanUp() {
	SetEventMask(oldSysMask);
	if (fileOpen) {
		closeFile();
	}
	if (window!=NULL) {
		DisposeWindow(FrontWindow());
		return;
	}
	DisposeWindow(window);
}
