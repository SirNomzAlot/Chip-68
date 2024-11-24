#ifndef core_h
#define core_h

#include <StandardFile.h>
#include <stdlib.h>
#include <stdbool.h>

#include <Quickdraw.h>
#include <MacMemory.h>
#include <Sound.h>
#include <Events.h>
#include <Fonts.h>

#include <ToolUtils.h>
#include <OSUtils.h>

#define screenX 512
#define screenY 342

#define windowX 6
#define windowY 42
#define windowWidth 500
#define windowHeight 290
#define windowId 4

#define max_sleep 3

typedef struct _tickData {
	long int ticksStart;
	long int ticksTotal;
	int deltaTick;
} tickData;

extern bool shouldClose;
extern char currKey;
extern bool fileOpen;
extern tickData timer;

void coreInit();
void toolboxInit();
void windowInit();
void menuBarInit();

void doLoop();

void eventHandler(EventRecord* event);
void keyDownHandler(EventRecord* event);
void keyUpHandler(EventRecord* event);
void autoKeyHandler(EventRecord* event);
void menuBarHandler(EventRecord* event);
void mouseDownHandler(EventRecord* event);

bool openFile(SFReply* r);
void fileError(OSErr e);
bool readFile();
void closeFile();

void printTimeInfo();
void updateTimer();
void coreCleanUp();

#endif
