#include "files.h"
#include "interpreter.h"
#include "opcode.h"
#include "textutils.h"
#include <StandardFile.h>
#include <MacTypes.h>
#include <stdbool.h>

bool fileOpen = false;
SFReply romInfo, test;

short fRefNum;
long fileLen;

SFTypeList list = {	(OSType)"BINA",
					(OSType)"CHP8",
					(OSType)"SCP8",
					(OSType)"XOCP"
};

bool tryOpenRead() {
	SFGetFile((Point){100,100},NULL,NULL,0,list,NULL,&test);
	if (!test.good) {
		return false;
	}
	closeFile();
	if (!openFile(&test)) {
		return false;
	}
	if (!readFile()) {
		return false;
	}
	reset();
	return true;
}

bool openFile(SFReply *file) {
	OSErr e;
	if (file->good == false) {
		fileError(1);
		return false;
	}
	if (file->vRefNum==romInfo.vRefNum&&pstrcmp(file->fName,romInfo.fName)) {
		fileError(1);
		return false;
	}
	e = FSOpen(file->fName, file->vRefNum, &fRefNum);
	if (e) {
		fileError(e);
		return false;
	}
	e = GetEOF(fRefNum, &fileLen);
	if (e) {
		fileError(e);
		return false;
	}
	if (fileLen>0xFFFF) {
		fileError(1);
		return false;
	}
	if (fileOpen) {
		closeFile();
	}
	fileOpen=true;
	pstrcpy(file->fName, romInfo.fName);
	romInfo.vRefNum=file->vRefNum;
	return true;
}

void fileError(OSErr e) {
	moveCursor(100,100);
	mprintf("File Error: %d", e);
}

bool readFile() {
	OSErr e;
	e = FSRead(fRefNum, &fileLen, memory+0x200);
	if (e&&e!=-39) {
		fileError(e);
		return false;
	}
	return true;
}
void closeFile() {
	if (!fileOpen) {
		return;
	}
	FSClose(fRefNum);
	fileOpen=false;
}