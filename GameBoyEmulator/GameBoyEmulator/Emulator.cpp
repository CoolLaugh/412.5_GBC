#include "Emulator.h"

Emulator::Emulator() {

	cpu.memory.LoadRom("./Tetris.gb");
	cpu.PowerUpSequence();
	graphics.memory = &cpu.memory;
}

Emulator::~Emulator() {

}

void Emulator::Update() {

	int totalCycles = 0;

	while (totalCycles < 70221) {

		short cycles = cpu.ExecuteOpcode();
		totalCycles += cycles;
		graphics.update(cycles);
		cpu.performInterupts();
		cpu.dividerRegisterINC(cycles);
		cpu.TimerCounterINC(cycles);
	}

}
