#ifndef files_h
#define files_h

#include <StandardFile.h>
#include <MacTypes.h>
#include <stdbool.h>

bool tryOpenRead();
bool openFile(SFReply* r);
void fileError(OSErr e);
bool readFile();
void closeFile();

#endif