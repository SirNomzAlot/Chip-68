#include "textutils.h"

#include <MacTypes.h>
#include <stdbool.h>

#include <Quickdraw.h>
#include <NumberFormatting.h>
#include <stdlib.h>

short cursorPosY = 0;
short cursorPosX = 0;	//Change for different text boxes, used in newline

const char hexLookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void DrawCString(const char* string) {
	int i = 0;
	while (string[i]!='\0' && i<=255) {
		switch (string[i]){
			case '\n':
				newLine();
				break;
			default:
			DrawChar(string[i]);
		}
		i++;
	}
}

void drawChar(char c) {
	FontInfo info;
	if (c=='\n') {
		newLine();
		return;
	} if (c=='\b') {
		GetFontInfo(&info);
		cursorPosX-=info.widMax;
		if (cursorPosX<=0) {
			//shrug
		}
		MoveTo(cursorPosX, cursorPosY);
		return;
	}
	DrawChar(c);
}

void mprintf(const char* string, ...) {
	int i = 0;
	int width = -1;
	int precision = -1;
	char number[255];
	int numberC = 0;
	bool inNumber = false;
	void* val;
	va_list args;
	va_start(args, string);
	while (string[i]!='\0') {
		if (string[i]=='%') {
			inNumber=true;
			do {
				i++;
				switch (string[i]) {
					case '%':
						DrawChar(string[i]);
						inNumber=false;
						break;
					case 'd':
					case 'i':
						number[numberC] = '\0';
						if (atoi(number)) {
							width = atoi(number);
							itoa(va_arg(args, int), number, 10);
							numberC=0;
							while (number[numberC]!='\0') {
								numberC++;
							}
							DrawCString(number+numberC-width);
							inNumber=false;
							break;
						}
						itoa(va_arg(args, int), number, 10);
						DrawCString(number);
						width=-1;
						inNumber=false;
						number[0] = '\0';
						numberC = 0;
						break;
					case 'f':
						number[numberC] = '\0'; 		//find precision if any
						if (numberC>0&&width!=-1) {
							precision = atoi(number);
						} else {
							width = (atoi(number)) ? atoi(number) : 255;
							precision = 6;
						}
						precision = tens[precision];

						val = malloc(sizeof(double));	//prints non decimal
						*(double*)val = va_arg(args, double);
						itoa((int)(*(double*)val), number, 10);
						numberC=0;
						while (number[numberC]!='\0') {
							numberC++;
						}
						DrawCString((numberC>width) ? number+numberC-width : number);
						if (precision>1) {
							DrawChar('.');					//prints decimal
							*(double*)val = (*(double*)val-(int)*(double*)val)*precision;
							itoa((int)(*(double*)val), number, 10);
							DrawCString(number);
						}
						free(val);						//cleanup
						width=-1;
						precision=-1;
						number[0] = '\0';
						numberC = 0;
						inNumber=false;
						break;
					case 's':
						number[numberC] = '\0'; 		//find precision if any
						if (numberC>0) {
							width = atoi(number);
							numberC=0;
							number[numberC]='\0';
						}
						val=va_arg(args, char*);
						DrawCString((char*)val);
						while (((char*)val)[numberC]!='\0'&&width) {
							numberC++;
							width--;
						}
						while (width) {
							DrawChar(' ');
							width--;
						}
						numberC=0;
						width=-1;
						inNumber=false;
						break;
					case '.':
						number[numberC]='\0';
						width = atoi(number) ? atoi(number) : 255;
						number[0] = '\0';
						numberC = 0;
						break;
					default:
						number[numberC] = string[i];
						numberC++;
				}
			} while (inNumber);
		} else {
			DrawChar(string[i]);
		}
		i++;
	}
	va_end(args);
}

void newLine() {
	FontInfo info;
	GetFontInfo(&info);
	cursorPosY+=info.ascent+info.descent+info.leading;
	MoveTo(cursorPosX, cursorPosY);
}

void moveCursor(short x, short y) {
	MoveTo(x,y);
	cursorPosX=x;
	cursorPosY=y;
}

bool pstrcmp(const char* s1, const char* s2) {
	int slen, i;

	slen = s1[0];
	if (slen!=s2[0]) {
		return false;
	}
	for (i = 1; i<slen+1; i++) {
		if (s1[i]!=s2[i])  {
			return false;
		}
	}
	return true;
}

bool pstrcpy(const char* src, char* dst) {
	int i;
	for (i=0;i<=src[0];i++) {
		dst[i]=src[i];
	}
}
