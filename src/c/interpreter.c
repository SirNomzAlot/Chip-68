#include "interpreter.h"
#include "opcode.h"
#include "screen.h"
#include "textutils.h"
#include "core.h"
#include "files.h"

#include <stdint.h>
#include <string.h>

#include <MacTypes.h>
#include <stdbool.h> //stdbool must come after mactypes

#include <Sound.h>

bool super = false;
bool step = false;
short clock = CLOCK_NORMAL;

uint8_t key;

static void(*ops0)(uint16_t);
static void(*ops8)(uint16_t);
static void(*opsD)(uint16_t);
static void(*opsF)(uint16_t);

bool interpreterInit() {
	if (!opcodeInit()) {
		return false;
	}
	ops0=&ops0CHIP;
	ops8=&ops8CHIP;
	opsD=&opsDCHIP;
	opsF=&opsFCHIP;
	mode = 1;
	key=0xFF;
	memcpy(memory, romSprite, 16*5);
	loadTest();
	return true;
}

void reset() {
	int c;
	normalScale();
	cls();
	clock = (clock) ? clock : CLOCK_NORMAL;
	cpu->pc=0x200;
	cpu->sp=0;
	cpu->delay=0;
	cpu->sound=0;
	cpu->i=0;
	for (c=0;c<16;c++) {
		cpu->reg[c]=0;
	}
	key=0xFF;
	wideScreen = false;
	memcpy(memory, romSprite, 16*5);
	if (!fileOpen) {
		loadTest();
		return;
	}
	readFile();
}

void chipMode() {
	if (mode==1) {
		return;
	}
	clock = (clock) ? clock : CLOCK_NORMAL;
	reset();
	mode=1;
	ops0=&ops0CHIP;
	ops8=&ops8CHIP;
	opsD=&opsDCHIP;
	opsF=&opsFCHIP;
	wideScreen = false;
	super = false;
}

void superMode() {
	if (mode==2) {
		return;
	}
	mode=2;
	clock = (clock) ? clock : CLOCK_NORMAL;
	reset();
	ops0=&ops0SUPR;
	ops8=&ops8SUPR;
	opsD=&opsDSUPR;
	opsF=&opsFSUPR;
	super=true;
}
void superModernMode() {
	if (mode==3) {
		return;
	}
	mode=3;
	clock = (clock) ? clock : CLOCK_NORMAL;
	reset();
	ops0=&ops0SUPR;
	ops8=&ops8SUPR;
	opsD=&opsDSUPR;
	opsF=&opsFSUPR;
	super=true;
}

void xoMode() {
	if (mode==4) {
		return;
	}
	mode=4;
	clock = (clock) ? clock : CLOCK_NORMAL;
	reset();
	ops0=&ops0XO;
	ops8=&ops8XO;
	opsD=&opsDXO;
	opsF=&opsFXO;
	super=false;
}

void interpretForTick() {
	unsigned char var;
	uint16_t op;
	if (cpu->sound) {
		SysBeep(1);
	}
	for (var=0; var<clock; var++) {
		op = (((uint16_t)memory[cpu->pc])<<8)+memory[cpu->pc+1];
		switch (op>>12) {
		case 0:
			ops0(op);
			break;
		case 0x1:
			jp(op&0xFFF);
			break;
		case 0x2:
			call(op&0xFFF);
			break;
		case 0x3:
			sevb((op>>8)&0xF, op&0xFF);
			break;
		case 0x4:
			snevb((op>>8)&0xF, op&0xFF);
			break;
		case 0x5:
			if (op&0xF) {
				fault();
			}
			sevv((op>>8)&0xF, (op>>4)&0xF);
			break;
		case 0x6:
			ldvb((op>>8)&0xF, op&0xFF);
			break;
		case 0x7:
			addvb((op>>8)&0xF, op&0xFF);
			break;
		case 0x8:
			ops8(op);
			break;
		case 0x9:
			if ((op&0xF)!=0) {
				fault();
			}
			snevv((op>>8)&0xF, (op>>4)&0xF);
			break;
		case 0xA:
			ldi(op&0xFFF);
			break;
		case 0xB:
			opsB(op);
			break;
		case 0xC:
			rnd((op>>8)&0xF, op&0xFF);
			break;
		case 0xD:
			opsD(op);
			break;
		case 0xE:
			opsE(op);
			break;
		case 0xF:
			opsF(op);
			break;
		}
	}
}

void decTimers() {
	if (cpu->delay) {
		cpu->delay--;
	}
	if (cpu->sound) {
		cpu->sound--;
	}
}

void loadTest() {
	int foo;
	for (foo=0; foo<17*4; foo++) {
		memory[0x200+(foo<<1)]=splash[foo]>>8;
		memory[0x201+(foo<<1)]=splash[foo]&0xFF;
	}
}

// who needs good code?
void renderCPU() {
	uint8_t high, low;
	high = memory[cpu->pc];
	low = memory[cpu->pc+1];
	moveCursor(400,0);
	newLine();
	DrawCString("CPU STATE:\n");
	if (super) {
		DrawCString("SCHIP");
	} else {
		DrawCString("CHIP");
	}
	newLine();
	DrawCString("OP: 0x");
	drawChar(hexLookup[high>>4]);
	drawChar(hexLookup[high&0xF]);
	drawChar(hexLookup[low>>4]);
	drawChar(hexLookup[low&0xF]);
	newLine();
	high=(cpu->pc)>>8;
	low=(cpu->pc)&0xFF;
	DrawCString("PC: 0x");
	drawChar(hexLookup[high>>4]);
	drawChar(hexLookup[high&0xF]);
	drawChar(hexLookup[low>>4]);
	drawChar(hexLookup[low&0xF]);
	newLine();
	high=(cpu->i)>>8;
	low=(cpu->i)&0xFF;
	DrawCString("I:  0x");
	drawChar(hexLookup[high>>4]);
	drawChar(hexLookup[high&0xF]);
	drawChar(hexLookup[low>>4]);
	drawChar(hexLookup[low&0xF]);
	newLine();
	DrawCString("MI: 0x");
	drawChar(hexLookup[memory[cpu->i]>>4]);
	drawChar(hexLookup[memory[cpu->i]&0xF]);
	newLine();
	DrawCString("SD: 0x");
	drawChar(hexLookup[cpu->sound>>4]);
	drawChar(hexLookup[cpu->sound&0xF]);
	newLine();
	DrawCString("DY: 0x");
	drawChar(hexLookup[cpu->delay>>4]);
	drawChar(hexLookup[cpu->delay&0xF]);
	newLine();
	DrawCString("V0: 0x");
	drawChar(hexLookup[cpu->reg[V0]>>4]);
	drawChar(hexLookup[cpu->reg[V0]&0xF]);
	newLine();
	DrawCString("V1: 0x");
	drawChar(hexLookup[cpu->reg[V1]>>4]);
	drawChar(hexLookup[cpu->reg[V1]&0xF]);
	newLine();
	DrawCString("V2: 0x");
	drawChar(hexLookup[cpu->reg[V2]>>4]);
	drawChar(hexLookup[cpu->reg[V2]&0xF]);
	newLine();
	DrawCString("V3: 0x");
	drawChar(hexLookup[cpu->reg[V3]>>4]);
	drawChar(hexLookup[cpu->reg[V3]&0xF]);
	newLine();
	DrawCString("V4: 0x");
	drawChar(hexLookup[cpu->reg[V4]>>4]);
	drawChar(hexLookup[cpu->reg[V4]&0xF]);
	newLine();
	DrawCString("V5: 0x");
	drawChar(hexLookup[cpu->reg[V5]>>4]);
	drawChar(hexLookup[cpu->reg[V5]&0xF]);
	newLine();
	DrawCString("V6: 0x");
	drawChar(hexLookup[cpu->reg[V6]>>4]);
	drawChar(hexLookup[cpu->reg[V6]&0xF]);
	newLine();
	DrawCString("V7: 0x");
	drawChar(hexLookup[cpu->reg[V7]>>4]);
	drawChar(hexLookup[cpu->reg[V7]&0xF]);
	newLine();
	DrawCString("V8: 0x");
	drawChar(hexLookup[cpu->reg[V8]>>4]);
	drawChar(hexLookup[cpu->reg[V8]&0xF]);
	newLine();
	DrawCString("V9: 0x");
	drawChar(hexLookup[cpu->reg[V9]>>4]);
	drawChar(hexLookup[cpu->reg[V9]&0xF]);
	newLine();
	DrawCString("VA: 0x");
	drawChar(hexLookup[cpu->reg[VA]>>4]);
	drawChar(hexLookup[cpu->reg[VA]&0xF]);
	newLine();
	DrawCString("VB: 0x");
	drawChar(hexLookup[cpu->reg[VB]>>4]);
	drawChar(hexLookup[cpu->reg[VB]&0xF]);
	newLine();
	DrawCString("VC: 0x");
	drawChar(hexLookup[cpu->reg[VC]>>4]);
	drawChar(hexLookup[cpu->reg[VC]&0xF]);
	newLine();
	DrawCString("VD: 0x");
	drawChar(hexLookup[cpu->reg[VD]>>4]);
	drawChar(hexLookup[cpu->reg[VD]&0xF]);
	newLine();
	DrawCString("VE: 0x");
	drawChar(hexLookup[cpu->reg[VE]>>4]);
	drawChar(hexLookup[cpu->reg[VE]&0xF]);
	newLine();
	DrawCString("VF: 0x");
	drawChar(hexLookup[cpu->reg[VF]>>4]);
	drawChar(hexLookup[cpu->reg[VF]&0xF]);
}

void fault() {
	renderCPU();
	clock=0;
	// somethin i dunno
}

void interpreterCleanup() {
	opcodeCleanup();
}

uint8_t keyToKeypad(char key) {
	switch (key) {
	case '1':
		return 0x1;
	case '2':
		return 0x2;
	case '3':
		return 0x3;
	case '4':
		return 0xC;

	case 'q':
		return 0x4;
	case 'w':
		return 0x5;
	case 'e':
		return 0x6;
	case 'r':
		return 0xD;

	case 'a':
		return 0x7;
	case 's':
		return 0x8;
	case 'd':
		return 0x9;
	case 'f':
		return 0xE;

	case 'z':
		return 0xA;
	case 'x':
		return 0x0;
	case 'c':
		return 0xB;
	case 'v':
		return 0xF;
	default:
		return 0xFF;
	}
}

void ops0CHIP(uint16_t op) {
	if (op==0x00E0) {
		cls();
		return;
	}
	if (op==0x00EE) {
		ret();
		return;
	} 
	fault();
}

void ops0SUPR(uint16_t op) {
	if (((op>>4)&0xF)==0xC) {
		scd(op&0xF);
		return;
	}
	switch (op&0xFF) {
	case 0xE0:
		cls();
		return;
	case 0xEE:
		ret();
		return;
	case 0xFB:
		scr();
		return;
	case 0xFC:
		scl();
		return;
	case 0xFD:
		exiti();
		return;
	case 0xFE:
		low();
		return;
	case 0xFF:
		high();
		return;
	default:
		fault();
	}
}

void ops0XO(uint16_t op) {
	if (((op>>4)&0xF)==0xC) {
		scd(op&0xF);
		return;
	}
	switch (op&0xFF) {
	case 0xE0:
		cls();
		return;
	case 0xEE:
		ret();
		return;
	case 0xFB:
		scr();
		return;
	case 0xFC:
		scl();
		return;
	case 0xFD:
		exiti();
		return;
	case 0xFE:
		low();
		return;
	case 0xFF:
		high();
		return;
	default:
		fault();
	}
}

void ops8CHIP(uint16_t op) {
	switch (op&0xF) {
	case 0x0:
		ldvv((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x1:
		orORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x2:
		andORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x3:
		eorORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x4:
		addvv((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x5:
		sub((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x6:
		shrORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x7:
		subn((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0xE:
		shlORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	default:
		fault();
	}
}

void ops8SUPR(uint16_t op) {
	switch (op&0xF) {
	case 0x0:
		ldvv((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x1:
		orSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x2:
		andSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x3:
		eorSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x4:
		addvv((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x5:
		sub((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x6:
		shrSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x7:
		subn((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0xE:
		shlSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	default:
		fault();
	}
}

void ops8XO(uint16_t op) {
	switch (op&0xF) {
	case 0x0:
		ldvv((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x1:
		orSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x2:
		andSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x3:
		eorSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x4:
		addvv((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x5:
		sub((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x6:
		shrORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0x7:
		subn((op>>8)&0xF, (op>>4)&0xF);
		return;
	case 0xE:
		shlORIG((op>>8)&0xF, (op>>4)&0xF);
		return;
	default:
		fault();
	}
}

void opsB(uint16_t op) {
	if ( super) {
		jpvSUPR((op>>8)&0xF, op&0xFFF);
		return;
	}
	jpvORIG(op&0xFFF);
}

void opsE(uint16_t op) {
	switch (op&0xFF) {
	case 0x9E:
		skp((op>>8)&0xF);
		return;
	case 0xA1:
		sknp((op>>8)&0xF);
		return;
	default:
		fault();
	}
}

void opsDCHIP(uint16_t op) {
	drw((op>>8)&0xF, (op>>4)&0xF, op&0xF);
}

void opsDSUPR(uint16_t op) {
	if (wideScreen&&(op&0xF)==0) {
		drwSUPR((op>>8)&0xF, (op>>4)&0xF);
		return;
	}
	drw((op>>8)&0xF, (op>>4)&0xF, op&0xF);
}

void opsDXO(uint16_t op) {
	if (wideScreen&&(op&0xF)==0) {
		drwHXO((op>>8)&0xF, (op>>4)&0xF);
		return;
	}
	drwLXO((op>>8)&0xF, (op>>4)&0xF, op&0xF);
}

void opsFCHIP(uint16_t op) {
	switch (op&0xFF) {
	case 0x07:
		ldfd((op>>8)&0xF);
		return;
	case 0x0A:
		ldfk((op>>8)&0xF);
		return;
	case 0x15:
		ldtd((op>>8)&0xF);
		return;
	case 0x18:
		ldts((op>>8)&0xF);
		return;
	case 0x1E:
		addi((op>>8)&0xF);
		return;
	case 0x29:
		ldi((op>>8)&0xF);
		return;
	case 0x33:
		ldbcd((op>>8)&0xF);
		return;
	case 0x55:
		ldtmiORIG((op>>8)&0xF);
		return;
	case 0x65:
		ldfmiORIG((op>>8)&0xF);
		return;
	default:
		fault();
	}
}

void opsFSUPR(uint16_t op) {
	switch (op&0xFF) {
	case 0x07:
		ldfd((op>>8)&0xF);
		return;
	case 0x0A:
		ldfk((op>>8)&0xF);
		return;
	case 0x15:
		ldtd((op>>8)&0xF);
		return;
	case 0x18:
		ldts((op>>8)&0xF);
		return;
	case 0x1E:
		addi((op>>8)&0xF);
		return;
	case 0x29:
		ldi((op>>8)&0xF);
		return;
	case 0x30:
		ldv((op>>8)&0xF);
		return;
	case 0x33:
		ldbcd((op>>8)&0xF);
		return;
	case 0x55:
		ldtmiSUPR((op>>8)&0xF);
		return;
	case 0x65:
		ldfmiSUPR((op>>8)&0xF);
		return;		
	case 0x75:
		ldtrpl((op>>8)&0xF);
		return;
	case 0x85:
		ldfrpl((op>>8)&0xF);
		return;
	default:
		fault();
	}
}

void opsFXO(uint16_t op) {
	switch (op&0xFF) {
	case 0x00:
		if (op&0xF00) {
			fault();
			return;
		}
		ldiL();
	case 0x07:
		ldfd((op>>8)&0xF);
		return;
	case 0x0A:
		ldfk((op>>8)&0xF);
		return;
	case 0x15:
		ldtd((op>>8)&0xF);
		return;
	case 0x18:
		ldts((op>>8)&0xF);
		return;
	case 0x1E:
		addi((op>>8)&0xF);
		return;
	case 0x29:
		ldi((op>>8)&0xF);
		return;
	case 0x30:
		ldv((op>>8)&0xF);
		return;
	case 0x33:
		ldbcd((op>>8)&0xF);
		return;
	case 0x55:
		ldtmiORIG((op>>8)&0xF);
		return;
	case 0x65:
		ldfmiORIG((op>>8)&0xF);
		return;		
	case 0x75:
		ldtrpl((op>>8)&0xF);
		return;
	case 0x85:
		ldfrpl((op>>8)&0xF);
		return;
	default:
		fault();
	}
}
