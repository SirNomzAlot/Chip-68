#include "files.h"
#include "interpreter.h"
#include "opcode.h"
#include "textutils.h"
#include <StandardFile.h>
#include <MacTypes.h>
#include <stdbool.h>
#include <stdint.h>

file rom = {0,0};

SFTypeList list = {	(OSType)"BINA",
					(OSType)"CHP8",
					(OSType)"SCP8",
					(OSType)"XOCP"
};

bool tryOpenRead() {
	SFReply test;
	SFGetFile((Point){100,100},NULL,NULL,0,list,NULL,&test);
	if (!test.good) {
		return false;
	}
	closeFile(&rom);
	if (!openFile(&test, &rom)) {
		return false;
	}
	reset();
	//if (!readFile(rom, (uint8_t*)(memory+0x200))) {
	//	return false;
	//}
	return true;
}

bool openFile(SFReply* sffile, file* f) {
	OSErr e;
	if (sffile->good == false) {
		fileError(1);
		return false;
	}
	e = FSOpen(sffile->fName, sffile->vRefNum, &(f->fRefNum));
	if (e) {
		fileError(e);
		return false;
	}
	e = GetEOF(f->fRefNum, &(f->fileLen));
	if (e) {
		fileError(e);
		return false;
	}
	if (f->fileLen>0x10000) {
		fileError(1);
		return false;
	}
	f->isOpen=true;
	return true;
}

void fileError(OSErr e) {
	moveCursor(100,100);
	mprintf("File Error: %d", e);
}

bool readFile(file f, uint8_t* buff) {
	if (!f.isOpen) {
		return false;
	}
	OSErr e;
	long c = f.fileLen;
	SetFPos(f.fRefNum, fsFromStart, 0);
	e = FSRead(f.fRefNum, &c, buff);
	if (e) {
		fileError(e);
		return false;
	}
	return true;
}

void closeFile(file* f) {
	if (!f->isOpen) {
		return;
	}
	FSClose(f->fRefNum);
	f->isOpen=false;
}

void closeRom() {
	closeFile(&rom);
}

bool readRom() {
	return readFile(rom, (uint8_t*)(memory+0x200));
}