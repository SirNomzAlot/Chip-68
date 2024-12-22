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
#define windowId 0

#define max_sleep 1

typedef struct _tickData {
	long int ticksStart;
	long int ticksTotal;
	int deltaTick;
} tickData;

extern WindowPtr window;
extern bool shouldClose;
extern char currKey;
extern char keyQueued;
extern bool fileOpen;
extern tickData timer;

void coreInit();
void toolboxInit();
void windowInit();
void menuBarInit();

void doLoop();
void waitTillClose();

void resize(short sx, short sy);

void eventHandler(EventRecord* event);
void keyDownHandler(EventRecord* event);
void keyUpHandler(EventRecord* event);
void autoKeyHandler(EventRecord* event);
void mouseDownHandler(EventRecord* event);
void idle();

void updateTimer();
void restoreEventMask();
void coreCleanUp();

#endif
