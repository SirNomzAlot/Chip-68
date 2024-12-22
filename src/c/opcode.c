#include "opcode.h"
#include "screen.h"
#include "core.h"
#include "interpreter.h"

#include "textutils.h"

#include <Sound.h>
#include <MacTypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t* memory;
registers* cpu;

bool opcodeInit() {
	cpu = malloc(sizeof(registers));
	//memory = malloc(0x10000);
	memory = malloc(0x10000);
	if (cpu==NULL||memory==NULL) {
		moveCursor(50,100);
		DrawCString("Could not allocate memory or cpu!");
		SysBeep(1);
		clock = 0;
		return false;
	}
	*cpu = (registers){{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0,0,0,0x200,0};
	return true;
}

void opcodeCleanup() {
	free(cpu);
	free(memory);
}

// 00E0
// Clear screen
void cls() {
	int countX, countY;
	wipe();
	if (wideScreen) {
		for (countX=0; countX<128; countX++) {
			for (countY=0; countY<64; countY++) {
				screen[countX*64+countY]=0;
			}
		}
		cpu->pc+=2;
		return;
	}
	for (countX=0; countX<64; countX++) {
		for (countY=0; countY<32; countY++) {
			screen[countX*64+countY]=0;
		}
	}
	cpu->pc+=2;
}


// 00EE
// return from routine
void ret() {
	cpu->pc=stack[cpu->sp];
	cpu->sp--;
}

// 1nnn
// jumpt to nnn
void jp(uint16_t addr) {
	cpu->pc=addr;
}

// 2nnn
// call routine at nnn
void call(uint16_t addr) {
	cpu->sp++;
	stack[cpu->sp]=cpu->pc+2;
	cpu->pc=addr;
}

// 3xkk
// skip next if Vx == kk
void sevb(vsel V, uint8_t byte) {
	if (cpu->reg[V]!=byte) {
		cpu->pc+=2;
		return;
	}
	cpu->pc+=4;
}

// 4xkk
// skip next if Vx != kk
void snevb(vsel V, uint8_t byte) {
	if (cpu->reg[V]==byte) {
		cpu->pc+=2;
		return;
	}
	cpu->pc+=4;
}

// 5xy0
// skip next if Vx == Vy
void sevv(vsel Vx, vsel Vy) {
	if (cpu->reg[Vx]!=cpu->reg[Vy]) {
		cpu->pc+=2;
		return;
	}
	cpu->pc+=4;
}

// 6xkk
// set Vx = kk
void ldvb(vsel V, uint8_t byte) {
	cpu->reg[V]=byte;
	cpu->pc+=2;
}

// 7xkk
// Vx = Vx + kk
void addvb(vsel V, uint8_t byte) {
	cpu->reg[V]+=byte;
	cpu->pc+=2;
}

// 8xy0
// set Vx = Vy
void ldvv(vsel Vx, vsel Vy) {
	cpu->reg[Vx]=cpu->reg[Vy];
	cpu->pc+=2;
}

// 8xy1
// Vx = Vx OR Vy
void orORIG(vsel Vx, vsel Vy) {
	cpu->reg[Vx]|=cpu->reg[Vy];
	cpu->reg[VF]=0;
	cpu->pc+=2;
}
void orSUPR(vsel Vx, vsel Vy) {
	cpu->reg[Vx]|=cpu->reg[Vy];
	cpu->pc+=2;
}

// 8xy2
// Vx = Vx AND Vy
void andORIG(vsel Vx, vsel Vy) {
	cpu->reg[Vx]&=cpu->reg[Vy];
	cpu->reg[VF]=0;
	cpu->pc+=2;
}
void andSUPR(vsel Vx, vsel Vy) {
	cpu->reg[Vx]&=cpu->reg[Vy];
	cpu->pc+=2;
}

// 8xy3
// Vx = Vx XOR Vy
void eorORIG(vsel Vx, vsel Vy) {
	cpu->reg[Vx]^=cpu->reg[Vy];
	cpu->reg[VF]=0;
	cpu->pc+=2;
}
void eorSUPR(vsel Vx, vsel Vy) {
	cpu->reg[Vx]^=cpu->reg[Vy];
	cpu->pc+=2;
}

// 8xy4
// Vx = Vx + Vy, VF is carry
void addvv(vsel Vx, vsel Vy) {
	int hold = (int)(cpu->reg[Vx])+cpu->reg[Vy];
	cpu->reg[Vx] = hold&0xFF;
	cpu->reg[VF] = (hold>0xFF) ? 1 : 0;
	cpu->pc+=2;
}

// 8xy5
// Vx = Vx - Vy, flag Vx > Vy ? 1 : 0
void sub(vsel Vx, vsel Vy) {
	uint8_t flag = (cpu->reg[Vx] >= cpu->reg[Vy]) ? 1 : 0;
	cpu->reg[Vx] -= cpu->reg[Vy];
	cpu->reg[VF] = flag;
	cpu->pc+=2;
}

// 8xy6
// Vx = Vx>>1, flag is shifted out
void shrSUPR(vsel Vx, vsel Vy) {
	uint8_t flag = cpu->reg[Vx]&0x1;
	cpu->reg[Vx]>>=1;
	cpu->reg[VF]=flag;
	cpu->pc+=2;
}
// Vx = Vy = Vy>>1 flag is shifted out
void shrORIG(vsel Vx, vsel Vy) {
	uint8_t flag = cpu->reg[Vy]&0x1;
	cpu->reg[Vy]>>=1;
	cpu->reg[Vx]=cpu->reg[Vy];
	cpu->reg[VF]=flag;
	cpu->pc+=2;
}

// 8xy7
// Vx = Vy - Vx, flag Vy > Vx ? 1 : 0
void subn(vsel Vx, vsel Vy){
	uint8_t flag = (cpu->reg[Vy] >= cpu->reg[Vx]) ? 1 : 0;
	cpu->reg[Vx]=cpu->reg[Vy]-cpu->reg[Vx];
	cpu->reg[VF]=flag;
	cpu->pc+=2;
}

// 8xyE
// Vx = Vx<<1, flag is shifted out
void shlSUPR(vsel Vx, vsel Vy) {
	uint8_t flag = (cpu->reg[Vx]&0x80) ? 1 : 0;
	cpu->reg[Vx] <<= 1;
	cpu->reg[VF] = flag;
	cpu->pc+=2;
}
void shlORIG(vsel Vx, vsel Vy) {
	uint8_t flag = (cpu->reg[Vy]&0x80) ? 1 : 0;
	cpu->reg[Vy] <<= 1;
	cpu->reg[Vx] = cpu->reg[Vy];
	cpu->reg[VF] = flag;
	cpu->pc+=2;
}

// 9xy0
// skip next if Vx!=Vy
void snevv(vsel Vx, vsel Vy) {
	if (cpu->reg[Vx]==cpu->reg[Vy]) {
		cpu->pc+=2;
		return;
	}
	cpu->pc += 4;
}

// Annn
// set i to nnn
void ldi(uint16_t addr) {
	cpu->i=addr;
	cpu->pc+=2;
}

// Bnnn
// jump to nnn+V0
void jpvORIG(uint16_t addr) {
	cpu->pc=addr+cpu->reg[0];
}

void jpvSUPR(vsel V, uint16_t addr) {
	cpu->pc=((uint16_t)cpu->reg[V])+addr;
}

// Cxkk
// Vx = rnd & kk
void rnd(vsel V, uint8_t byte) {
	cpu->reg[V]=((uint8_t)(Random()*255/65536.0f))&byte;
	cpu->pc+=2;
}

// Dxyn
// Display n byte sprite starting at memory I
// at Vx, Vy set VF = collision.
// sprite is XORed onto screen
void drw(vsel Vx, vsel Vy, uint8_t nibble) {
	uint16_t c;
	uint8_t hold, Vxx, Vyy;
	char b, till;

	Vxx=cpu->reg[Vx];
	Vyy=cpu->reg[Vy];
	till=8;
	if ((Vyy%32)>((Vyy+nibble)%32)) {
		nibble-=((Vyy+nibble)%32);
	}
	if ((Vxx%64)>((Vxx+7)%64)) {
		till-=((Vxx+7)%64);
	}
	cpu->reg[VF]=0;
	for (c=0;c<nibble;c++) {
		hold = memory[cpu->i+c];
		for (b=0;b<till;b++) {
			if (screen[((Vxx+b)%64)*64+(Vyy+c)%32]&(hold>>(7-b))&0x1) {
				cpu->reg[VF]=1;
			}
			screen[((Vxx+b)%64)*64+(Vyy+c)%32]^=(hold>>(7-b))&0x1;
			drawPixel((Vxx+b)%64,(Vyy+c)%32);
		}
	}
	cpu->pc+=2;
}

// Ex9E
// skip next if keyboard == Vx
// does not busy loop
void skp(vsel V) {
	uint8_t button = keyToKeypad(currKey);
	if (button!=cpu->reg[V]||button==0xFF) {
		cpu->pc+=2;
		return;
	}
	cpu->pc+=4;
}

// ExA1
// skip next if keyboard != Vx
// does not busy loop
void sknp(vsel V) {
	uint8_t button = keyToKeypad(currKey);
	if (button==cpu->reg[V]&&cpu->reg[V]!=0xFF) {
		cpu->pc+=2;
		return;
	}
	cpu->pc+=4;
}

// Fx0A
// set Vx = delayTimer
void ldfd(vsel V) {
	cpu->reg[V]=cpu->delay;
	cpu->pc+=2;
}

// Fx0A
// set Vx = keyboard
// does busy loop
void ldfk(vsel V) {
	uint8_t var = keyToKeypad(currKey);
	if (var!=0xFF&&key==0xFF) {
		key=var;
		return;
	}
	if (key==var) {
		return;
	}
	cpu->reg[V]=key;
	key=0xFF;
	cpu->pc+=2;
}

// Fx15
// Set delayTimer = Vx
void ldtd(vsel V) {
	cpu->delay=cpu->reg[V];
	cpu->pc+=2;
}

// Fx18
// set soundTimer = Vx
void ldts(vsel V) {
	cpu->sound=cpu->reg[V];
	cpu->pc+=2;
}

// Fx1E
// I = I + Vx
void addi(vsel V) {
	cpu->i+=cpu->reg[V];
	cpu->pc+=2;
}

// Fx29
// set I addr of sprite for digit Vx
void ldsa(vsel V) {
	cpu->i=(cpu->reg[V]&0xF)*5;
	cpu->pc+=2;
}

// Fx33
// store BCD of Vx in
// i, i+1, i+2
void ldbcd(vsel V) {
	memory[cpu->i+2]=cpu->reg[V]%10;
	memory[cpu->i+1]=(cpu->reg[V]/10)%10;
	memory[cpu->i]=cpu->reg[V]/100;
	cpu->pc+=2;
}

// Fx55
// Store regs V0-Vx into memory startig from i
// V0->i, V1->i+1, ...
void ldtmiSUPR(vsel V) {
	int regCount;
	for (regCount = 0; regCount<=V; regCount++) {
		memory[cpu->i+regCount]=cpu->reg[regCount];
	}
	cpu->pc+=2;
}
void ldtmiORIG(vsel V) {
	int regCount;
	for (regCount = 0; regCount<=V; regCount++) {
		memory[cpu->i]=cpu->reg[regCount];
		cpu->i++;
	}
	cpu->i;
	cpu->pc+=2;
}

// Fx65
// Read regs V0-Vx from memory starting from i
// i->V0, i+1->V1, ...
void ldfmiSUPR(vsel V) {
	int regCount;
	for (regCount = 0; regCount<=V; regCount++) {
		cpu->reg[regCount]=memory[cpu->i+regCount];
	}
	cpu->pc+=2;
}
void ldfmiORIG(vsel V) {
	int regCount;
	for (regCount = 0; regCount<=V; regCount++) {
		cpu->reg[regCount]=memory[cpu->i];
		cpu->i++;
	}
	cpu->pc+=2;
}

// Super Chip Specific

// 00CN
// scroll n lines down
void scd(uint8_t nibble) {
	cpu->pc+=2;
}

// 00FB
// scroll display 4 pixels right
void scr() {
	int x, y;
	if (wideScreen) {
		for (y=63;y>=0;y--) {
			for (x=127;x>=0;x--) {
				if (x<4) {
					screen[x*64+y]=0;
				}
				screen[x*64+y]=screen[(x-4)*64+y];
			}
		}
		cpu->pc+=2;
		return;
	}
	for (y=31;y>=0;y--) {
		for (x=63;x>=0;x--) {
			if (x<4) {
				screen[x*64+y]=0;
			}
			screen[x*64+y]=screen[(x-4)*64+y];
		}
	}
	cpu->pc+=2;
}

// 00FC
// scroll display 4 pixels left
void scl() {
	cpu->pc+=2;
}

// 00FD
// exit interpreter
void exiti() {
	return;
}

// 00FE
// disable extended screen
void low() {
	normalScale();
	wideScreen=false;
	cpu->pc+=2;
}

// 00FF
// enable extended screen
void high() {
	highScale();
	wideScreen=true;
	cpu->pc+=2;
}

// Dxy0
// superchip specific draw
// if n==0 and extended 16x16 sprite
void drwSUPR(vsel Vx, vsel Vy) {
	uint16_t y;
	uint8_t hold, Vxx, Vyy;
	char b, till;

	Vxx=cpu->reg[Vx];
	Vyy=cpu->reg[Vy];
	till=8;
	
	cpu->reg[VF]=0;
	for (y=0;y<16;y++) {
		hold = memory[cpu->i+y*2];
		for (b=0;b<till;b++) {
			if (screen[((Vxx+b)%128)*64+(Vyy+y)%64]&(hold>>(7-b))&0x1) {
				cpu->reg[VF]=1;
			}
			screen[((Vxx+b)%128)*64+(Vyy+y)%64]^=(hold>>(7-b))&0x1;
			drawPixel((Vxx+b)%128,(Vyy+y)%64);
		}
		hold = memory[cpu->i+y*2+1];
		for (b=0;b<till;b++) {
			if (screen[((Vxx+b+8)%128)*64+(Vyy+y)%64]&(hold>>(7-b))&0x1) {
				cpu->reg[VF]=1;
			}
			screen[((Vxx+b+8)%128)*64+(Vyy+y)%64]^=(hold>>(7-b))&0x1;
			drawPixel((Vxx+b+8)%128,(Vyy+y)%64);
		}
	}
	cpu->pc+=2;
}

// Fx30
// point I to 10 byte font sprite for Vx digits 0-9
void ldv(vsel V) {
	cpu->pc+=2;
}

// Fx75
// store V0-Vx in RPL user flag (x<=7)
void ldtrpl(uint8_t nibble) {
	cpu->pc+=2;
}

// Fx85
// read V0-Vx from RPL user flag (x<=7)
void ldfrpl(uint8_t nibble) {
	cpu->pc+=2;
}

// XO chip specific

void drwLXO(vsel Vx, vsel Vy, uint8_t nibble) {
	uint16_t c;
	uint8_t hold, Vxx, Vyy;
	char b;

	Vxx=cpu->reg[Vx];
	Vyy=cpu->reg[Vy];
	cpu->reg[VF]=0;
	for (c=0;c<nibble;c++) {
		hold = memory[cpu->i+c];
		for (b=0;b<8;b++) {
			if (screen[((Vxx+b)%64)*64+(Vyy+c)%32]&(hold>>(7-b))&0x1) {
				cpu->reg[VF]=1;
			}
			screen[((Vxx+b)%64)*64+(Vyy+c)%32]^=(hold>>(7-b))&0x1;
			drawPixel((Vxx+b)%64,(Vyy+c)%32);
		}
	}
	cpu->pc+=2;
}

void drwHXO(vsel Vx, vsel Vy) {
	uint16_t y;
	uint8_t hold, Vxx, Vyy;
	char b;

	Vxx=cpu->reg[Vx];
	Vyy=cpu->reg[Vy];
	
	cpu->reg[VF]=0;
	for (y=0;y<16;y++) {
		hold = memory[cpu->i+y*2];
		for (b=0;b<8;b++) {
			if (screen[((Vxx+b)%128)*64+(Vyy+y)%64]&(hold>>(7-b))&0x1) {
				cpu->reg[VF]=1;
			}
			screen[((Vxx+b)%128)*64+(Vyy+y)%64]^=(hold>>(7-b))&0x1;
			drawPixel((Vxx+b)%128,(Vyy+y)%64);
		}
		hold = memory[cpu->i+y*2+1];
		for (b=0;b<8;b++) {
			if (screen[((Vxx+b+8)%128)*64+(Vyy+y)%64]&(hold>>(7-b))&0x1) {
				cpu->reg[VF]=1;
			}
			screen[((Vxx+b+8)%128)*64+(Vyy+y)%64]^=(hold>>(7-b))&0x1;
			drawPixel((Vxx+b+8)%128,(Vyy+y)%64);
		}
	}
	cpu->pc+=2;
}

void ldiL() {
	cpu->pc+=2;
	cpu->i=((uint16_t)memory[cpu->pc])<<8+memory[cpu->pc+1];
	cpu->pc+=2;
}
