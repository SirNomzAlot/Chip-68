#include "screen.h"
#include "textutils.h"
#include <Quickdraw.h>
#include <stdlib.h>
#include <stdint.h>
#include <MacTypes.h>
#include <stdbool.h>

bool wideScreen = false;
bool screenUpdate = false;
uint8_t scale = 4;
uint8_t* screen;

Pattern black;
Pattern white;

void screenInit() {
	int countX, countY;
	screen=malloc(sizeof(uint8_t)*128*64);
	GetIndPattern(&black, sysPatListID, 1);
	GetIndPattern(&white, sysPatListID,20);

	if (screen==NULL) {
		return;
		//ahhhhhh
	}

	for (countX=0; countX<128; countX++) {
		for (countY=0; countY<64; countY++) {
			screen[countX*64+countY]=0;
		}
	}
}

void raster() {
	int x, y;
	Rect pixel;

    // if (!screenUpdate) {
    // 	return;
    // }
    // screenUpdate=false;

	if (wideScreen) {
		for (x=0; x<128; x++) {
			for (y=0; y<64; y++) {
				if (screen[x*64+y]) {
					SetRect(&pixel, x*scale, y*scale, x*scale+scale, y*scale+scale);
					FillRect(&pixel, &black);
				}
			}
		}
		return;
	}
	for (x=0; x<64; x++) {
		for (y=0; y<32; y++) {
			if (screen[x*64+y]) {
				SetRect(&pixel, x*scale, y*scale, x*scale+scale, y*scale+scale);
				FillRect(&pixel, &black);
			}
		}
	}
}

void drawPixel(short x, short y) {
	Rect pixel;
    PatPtr color = &white;
	if (screen[x*64+y]) {
		color = &black;
	}
	SetRect(&pixel, x*scale, y*scale, x*scale+scale, y*scale+scale);
	FillRect(&pixel, color);
}

void wipe() {
	Rect screen;
	if (wideScreen) {
		SetRect(&screen, 0, 0, 128*scale, 64*scale);
		FillRect(&screen, &white);
		return;
	}
	SetRect(&screen, 0, 0, 64*scale, 32*scale);
	FillRect(&screen, &white);
}

void screenCleanup() {
	free(screen);
}