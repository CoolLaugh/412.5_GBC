#include <stdio.h>
#include "Emulator.h"



int main() {

	Emulator emulator;
	emulator.FileSelectWindow();
	emulator.Loop();

	return 0;
}