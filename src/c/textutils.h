#ifndef textutils_h
#define textutils_h

#include <stdint.h>
#include <MacTypes.h>
#include <stdbool.h>

const static int tens[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
const extern char hexLookup[16];

//should be DrawString equivelant
void DrawCString(const char* string);
void drawChar(char c);
//printf but as drawCString and NO ERROR CHECKING
void mprintf(const char* string, ...);
void newLine();
void moveCursor(short x, short y);

bool pstrcmp(const char* s1, const char* s2);
bool pstrcpy(const char* src, char* dst);

#endif