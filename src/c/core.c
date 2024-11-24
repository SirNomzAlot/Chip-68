#include "core.h"
#include "textutils.h"
#include "interpreter.h"
#include "opcode.h"
#include "screen.h"

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
Handle menubar;
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

uint16_t oldSysMask;

SFReply romInfo;
short fRefNum;
long fileLen;
SFTypeList list = {	(OSType)"BINA",
					(OSType)"CHP8",
					(OSType)"SCP8",
					(OSType)"XOCP"
};
bool fileOpen = false;
uint8_t oldclock;

void coreInit() {
	SysEnvRec sys;
	toolboxInit();
	windowInit();
	menuBarInit();
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
	SetPort(window);
}

void menuBarInit() {
    menubar = GetNewMBar(128);
    SetMenuBar(menubar);
    //apple_menu = GetMHandle(APPLE_MENU_ID);
   // menu_defaults();
    DrawMenuBar();
}

void doLoop() {
	if (!multifinder) {
		SystemTask();
		GetNextEvent(everyEvent, &event);
		eventHandler(&event);
		return;
	}
	if (WaitNextEvent(everyEvent, &event, max_sleep, NULL)) {
		eventHandler(&event);
		//updateTimer();
		//loopCount++;
	}
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

SFReply r; //because lazy
void menuBarHandler(EventRecord* event) {
	short menuID, itemNum;
	long var = MenuSelect(event->where);
	menuID = HiWord(var);
	itemNum = LoWord(var);
	switch (menuID) {
	case 128:
		mprintf("%d is 128 item num", itemNum);
		return;
	case 129:
		//mprintf("%d is 129 item num", itemNum);
			switch (itemNum) {
			case 1:
				reset();
				currKey = '\0';
				return;
			case 2:
				//SFGetFile((Point){100,100},NULL,NULL,4,list,NULL,&r);
				SFGetFile((Point){100,100},NULL,NULL,0,list,NULL,&r);
				if (!r.good) {
					fileError(0);
					return;
				}
				closeFile();
				openFile(&r);
				reset();
				readFile();
				return;
			case 3:
				return;
			case 5:
				shouldClose=true;
				interpreterCleanup();
				screenCleanup();
				SetEventMask(oldSysMask);
				ExitToShell();
				return;
			default:
				return;
			}
		return;
	case 130:
		//mprintf("%d is 130 item num", itemNum);
		return;
	case 131:
		switch (itemNum) {
		case 1:
			toggleCompat();
			return;
		case 2:
			return;
		case 3:
			clock=1;
			step = true;
			renderCPU();
			interpretForTick();
			return;
		case 4:
			return;
		case 5:
			clock=CLOCK_SLOW;
			step = false;
			return;
		case 6:
			clock=CLOCK_NORM;
			step = false;
			return;
		case 7:
			clock=CLOCK_FAST;
			step = false;
			return;
		default:
			return;
		}
	default:
		mprintf("%d is menu id", menuID);
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

bool openFile(SFReply *r) {
	OSErr e;
	if (r->good == false) {
		fileError(1);
		return false;
	}
	if (r->vRefNum==romInfo.vRefNum&&pstrcmp(r->fName,romInfo.fName)) {
		fileError(1);
		return false;
	}
	e = FSOpen(r->fName, r->vRefNum, &fRefNum);
	if (e) {
		fileError(e);
		return false;
	}
	e = GetEOF(fRefNum, &fileLen);
	if (e) {
		fileError(e);
		return false;
	}
	if (fileLen>0xFFFF) {
		fileError(1);
		return false;
	}
	if (fileOpen) {
		closeFile();
	}
	fileOpen=true;
	pstrcpy(r->fName, romInfo.fName);
	romInfo.vRefNum=r->vRefNum;
	return true;
}

void fileError(OSErr e) {
	moveCursor(100,100);
	mprintf("File Error: %d", e);
}

bool readFile() {
	OSErr e;
	e = FSRead(fRefNum, &fileLen, memory+0x200);
	if (e&&e!=-39) {
		fileError(e);
		return false;
	}
	return true;
}
void closeFile() {
	if (!fileOpen) {
		return;
	}
	FSClose(fRefNum);
	fileOpen=false;
}

void coreCleanUp() {
	if (fileOpen) {
		closeFile();
	}
	SetEventMask(oldSysMask);
	DisposeWindow(window);
}
