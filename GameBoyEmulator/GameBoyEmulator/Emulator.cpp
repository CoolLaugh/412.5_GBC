#include "Emulator.h"

Emulator::Emulator() {

	cpu.memory.LoadRom("./Tetris.gb");
}

Emulator::~Emulator() {

}

void Emulator::Update() {

	for (size_t i = 0; i < 70221; i++) {

		cpu.ExecuteOpcode();
	}

}
