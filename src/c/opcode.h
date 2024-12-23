#ifndef opcode_h
#define opcode_h

#include <MacTypes.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum _vsel {
	V0,
	V1,
	V2,
	V3,
	V4,
	V5,
	V6,
	V7,
	V8,
	V9,
	VA,
	VB,
	VC,
	VD,
	VE,
	VF
} vsel;

typedef struct _registers {
	uint8_t reg[16];

	uint8_t delay;
	uint8_t sound;
	uint16_t i;

	uint16_t pc;
	uint8_t sp;
} registers;

extern uint8_t* memory;
extern registers* cpu;
static uint16_t stack[0x16];

bool opcodeInit();
void opcodeCleanup();

void cls();
void ret();
void jp(uint16_t addr);
void call(uint16_t addr);
void sevb(vsel V, uint8_t byte);
void snevb(vsel V, uint8_t byte);
void sevv(vsel Vx, vsel Vy);
void ldvb(vsel V, uint8_t byte);
void addvb(vsel Vx, uint8_t byte);
void ldvv(vsel Vx, vsel Vy);
void orORIG(vsel Vx, vsel Vy);
void orSUPR(vsel Vx, vsel Vy);
void andORIG(vsel Vx, vsel Vy);
void andSUPR(vsel Vx, vsel Vy);
void eorORIG(vsel Vx, vsel Vy);
void eorSUPR(vsel Vx, vsel Vy);
void addvv(vsel Vx, vsel Vy);
void sub(vsel Vx, vsel Vy);
void shrSUPR(vsel Vx, vsel Vy);
void shrORIG(vsel Vx, vsel Vy);
void subn(vsel Vx, vsel Vy);
void shlSUPR(vsel Vx, vsel Vy);
void shlORIG(vsel Vx, vsel Vy);
void snevv(vsel Vx, vsel Vy);
void ldi(uint16_t addr);
void jpvORIG(uint16_t addr);
void jpvSUPR(vsel V, uint16_t addr);
void rnd(vsel V, uint8_t byte);
void drw(vsel Vx, vsel Vy, uint8_t nibble);
void skp(vsel V);
void sknp(vsel V);
void ldfd(vsel V);
void ldfk(vsel V);
void ldtd(vsel V);
void ldts(vsel V);
void addi(vsel V);
void ldsa(vsel V);
void ldbcd(vsel V);
void ldtmiSUPR(vsel V);
void ldtmiORIG(vsel V);
void ldfmiSUPR(vsel V);
void ldfmiORIG(vsel V);

// Super Chip Specific

void scd(uint8_t nibble);
void scr();
void scl();
void exiti();
void low();
void high();
void drwSUPR(vsel Vx, vsel Vy);
void ldv(vsel V);
void ldtrpl(uint8_t nibble);
void ldfrpl(uint8_t nibble);

// XO chip

void drwLXO(vsel Vx, vsel Vy, uint8_t nibble);
void drwHXO(vsel Vx, vsel Vy);
void ldiL();

#endif
