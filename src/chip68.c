#include "chip68.h"
#include "c/core.h"
#include "c/textutils.h"
#include "c/interpreter.h"
#include "c/screen.h"

int main(int argc, char** argv) {
	coreInit();
	interpreterInit();
	screenInit();
	loadTest();
	while (!shouldClose) {
		doLoop();
		if (!step) {
			interpretForTick();
		}
		decTimers();
	}
	interpreterCleanup();
	screenCleanup();
	coreCleanUp();
	ExitToShell();
}
