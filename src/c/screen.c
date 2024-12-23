#include "screen.h"
#include "textutils.h"
#include "core.h"
#include <Quickdraw.h>
#include <stdlib.h>
#include <stdint.h>
#include <MacTypes.h>
#include <stdbool.h>

bool wideScreen = false;
bool screenUpdate = false;
uint8_t scale = 4;
uint8_t multiplier = 4;
uint8_t* screen;

Pattern black;
Pattern white;

void screenInit() {
	int countX, countY;
	screen=malloc(sizeof(uint8_t)*128*64);
	GetIndPattern(&black, sysPatListID, 1);
	GetIndPattern(&white, sysPatListID,20);

	if (screen==NULL) {
		shouldClose=true;
		return;
	}

	for (countX=0; countX<128; countX++) {
		for (countY=0; countY<64; countY++) {
			screen[countX*64+countY]=0;
		}
	}
}

void raster() {
	int x, y, s;
	Rect pixel;

	s = multiplier;
	if (wideScreen) {
		s/=2;
		for (x=0; x<128; x++) {
			for (y=0; y<64; y++) {
				if (screen[x*64+y]) {
					SetRect(&pixel, x*s, y*s, x*s+s, y*s+s);
					FillRect(&pixel, &black);
				}
			}
		}
		return;
	}
	for (x=0; x<64; x++) {
		for (y=0; y<32; y++) {
			if (screen[x*64+y]) {
				SetRect(&pixel, x*s, y*s, x*s+s, y*s+s);
				FillRect(&pixel, &black);
			}
		}
	}
}

void drawPixel(short x, short y) {
	Rect pixel;
	PatPtr color = &white;
	int s = (wideScreen) ? multiplier/2 : multiplier;
	if (screen[x*64+y]) {
		color = &black;
	}
	SetRect(&pixel, x*s, y*s, x*s+s, y*s+s);
	FillRect(&pixel, color);
}

void wipe() {
	Rect screen;
	int s = multiplier;
	if (wideScreen) {
		s/=2;
		SetRect(&screen, 0, 0, 128*s, 64*s);
		FillRect(&screen, &white);
		return;
	}
	SetRect(&screen, 0, 0, 64*s, 32*s);
	FillRect(&screen, &white);
}

void normalScale() {
	wideScreen=false;
	scale = SCALE_NORMAL;
}
void highScale() {
	wideScreen=true;
	scale = SCALE_SUPER;
}

void setMultiplier(short m) {
	if (m<0) {
		return;
	}
	multiplier = m+scale;
	wipe();
	resize(64*multiplier, 32*multiplier);
	raster();
}

void screenCleanup() {
	free(screen);
}
