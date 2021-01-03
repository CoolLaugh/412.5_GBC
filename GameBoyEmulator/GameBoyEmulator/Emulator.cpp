#include "Emulator.h"

Emulator::Emulator() {

	cpu.memory.LoadRom("./Tetris.gb");
	cpu.PowerUpSequence();
	graphics.memory = &cpu.memory;
}

Emulator::~Emulator() {

}

void Emulator::Update() {

	for (size_t i = 0; i < 70221; i++) {

		short cycles = cpu.ExecuteOpcode();
		int ff44 = cpu.memory.Read(0xFF44);
		graphics.update(cycles);
	}

}
