#ifndef interpreter_h
#define interpreter_h

#include <stdint.h>
#include <MacTypes.h> //mactypes must come before stdbool
#include <stdbool.h>

#define CLOCK_SLOW 2 		//  120hz
#define CLOCK_MEDIUM 4 		//  240hz
#define CLOCK_NORMAL 8 		//  520hz
#define CLOCK_FAST 16 		//  960hz
#define CLOCK_LUDICROUS 24 	// 1440hz

extern bool super;
extern bool step;
extern short clock;

static short mode; // 1 chip, 2 super, 3 super v1.1, 4 xo

bool interpreterInit();
void memoryInit();
void reset();
void chipMode();
void superMode();
void superModernMode();
void xoMode();
void interpretForTick();
void decTimers(int delta);
void loadTest();
void renderCPU();
void interpreterCleanup();
void fault();

void ops0CHIP(uint16_t op);
void ops0SUPR(uint16_t op);
void ops0XO(uint16_t op);

void ops8CHIP(uint16_t op);
void ops8SUPR(uint16_t op);
void ops8XO(uint16_t op);

void opsB(uint16_t op);
void opsE(uint16_t op);
//void opESUPER(uint16_t op);

void opsDCHIP(uint16_t op);
void opsDSUPR(uint16_t op);
void opsDXO(uint16_t op);

void opsFCHIP(uint16_t op);
void opsFSUPR(uint16_t op);
void opsFXO(uint16_t op);


uint8_t keyToKeypad(char key);

const static uint8_t romSprite[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xF0, 0x10, 0xF0, 0x80, 0xF0,
	0xF0, 0x10, 0xF0, 0x10, 0xF0,
	0x90, 0x90, 0xF0, 0x10, 0x10,
	0xF0, 0x80, 0xF0, 0x10, 0xF0,
	0xF0, 0x80, 0xF0, 0x90, 0xF0,
	0xF0, 0x10, 0x20, 0x40, 0x40,
	0xF0, 0x90, 0xF0, 0x90, 0xF0,
	0xF0, 0x90, 0xF0, 0x10, 0xF0,
	0xF0, 0x90, 0xF0, 0x90, 0x90,
	0xE0, 0x90, 0xE0, 0x90, 0xE0,
	0xF0, 0x80, 0x80, 0x80, 0xF0,
	0xE0, 0x90, 0x90, 0x90, 0xE0,
	0xF0, 0x80, 0xF0, 0x80, 0xF0,
	0xF0, 0x80, 0xF0, 0x80, 0x80
};

const static uint16_t splash[] = {
0xA248, 0x6100, 0x6200, 0xD128,
0xA250, 0x6108, 0x6200, 0xD128,
0xA258, 0x6110, 0x6200, 0xD128,
0xA260, 0x6118, 0x6200, 0xD128,
0xA268, 0x6120, 0x6200, 0xD128,
0xA270, 0x6128, 0x6200, 0xD128,
0xA278, 0x6130, 0x6200, 0xD128,
0xA280, 0x6138, 0x6200, 0xD128,
0x1240, 0x1240, 0x1240, 0x1240,

0x0000, 0x0608, 0x0808, 0x0600,
0x0000, 0x9792, 0xF292, 0x9700,
0x0000, 0x7048, 0x7340, 0x4000,
0x0000, 0x3142, 0x714A, 0x4A31,
0x0000, 0x8744, 0x8644, 0x4487,
0x0000, 0x4266, 0x5A42, 0x4242,
0x0000, 0x8888, 0x8888, 0x8870,
0x0000, 0x0000, 0x0000, 0x0000
};

#endif