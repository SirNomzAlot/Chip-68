#ifndef files_h
#define files_h

#include <StandardFile.h>
#include <MacTypes.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct _file {
	long fileLen;
	bool isOpen;
	short fRefNum;
} file;

bool tryOpenRead();
bool openFile(SFReply* sffile, file* f);
void fileError(OSErr e);
bool readFile(file f, uint8_t* buff);
void closeFile(file* f);
void closeRom();
bool readRom();

#endif