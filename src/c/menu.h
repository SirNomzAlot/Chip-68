#ifndef menu_h
#define menu_h

#include <Events.h>

void menuInit();

void menuBarHandler(EventRecord* event);

void inAppleMenu(short itemNum);
void inFileMenu(short itemNum);
void inEditMenu(short itemNum);
void inWindowMenu(short itemNum);
void inInterpreterMenu(short itemNum);

#endif