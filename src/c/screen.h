#ifndef screen_h
#define screen_h

#include <MacTypes.h>
#include <stdbool.h>
#include <stdint.h>

extern bool wideScreen;
extern bool screenUpdate; // must set true if screen modified
extern uint8_t scale;
extern uint8_t* screen;

void screenInit();
void raster();
void drawPixel(short x, short y);
void wipe();
void screenCleanup();

#endif