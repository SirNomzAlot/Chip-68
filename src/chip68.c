#include "chip68.h"
#include "c/core.h"
#include "c/textutils.h"
#include "c/interpreter.h"
#include "c/screen.h"

int main(int argc, char** argv) {
	coreInit();
	if (!interpreterInit()) {
		coreCleanUp();
		ExitToShell();
		return -1;
	}
	screenInit();
	loadTest();
	while (!shouldClose) {
		doLoop();
	}
	interpreterCleanup();
	screenCleanup();
	coreCleanUp();
	ExitToShell();
}
