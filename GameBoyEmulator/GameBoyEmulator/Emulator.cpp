#include "Emulator.h"

Emulator::Emulator() {

	cpu.memory.LoadRom("./gb/04-op r,imm.gb");
	cpu.PowerUpSequence();
	graphics.memory = &cpu.memory;
}

Emulator::~Emulator() {

}

void Emulator::Update() {

	int totalCycles = 0;

	while (totalCycles < 70221) {

		//std::vector<byte> uncheckedOpcodes = {              0x02,                         0x07, 0x08, 0x09, 0x0A,                         0x0F,
		//										0x10,                         0x15,       0x17,                   0x1B,
		//										                                          0x27,                   0x2B,             0x2E,
		//										                  0x33,                   0x37,       0x39, 0x3A, 0x3B,                   0x3F,
		//										0x40, 0x41, 0x42, 0x43, 0x44, 0x45,/*0x46,*/    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,       
		//										0x50, 0x51, 0x52, 0x53, 0x54, 0x55,             0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D,
		//										0x60, 0x61,       0x63, 0x64, 0x65, 0x66,       0x68, 0x69, 0x6A,       0x6C, 0x6D,
		//										                  0x73, 0x74, 0x75, 0x76,                                                 0x7F,
		//										0x80,       0x82, 0x83, 0x84, 0x85, 0x86,       0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		//										0x90,       0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		//										0xA0,       0xA2, 0xA3, 0xA4, 0xA5, 0xA6,       0xA8,/*0xA9,*/0xAA, 0xAB, 0xAC, 0xAD,
		//										            0xB2, 0xB3, 0xB4, 0xB5,             0xB8, 0xB9, 0xBA,       0xBC, 0xBD, 0xBE, 0xBF,
		//										                                          0xC7,               /*0xCB,*/ 0xCC,         0xCF,
		//										            0xD2, 0xD3, 0xD4,             0xD7,             0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		//											              0xE3, 0xE4,             0xE7, 0xE8,             0xEB, 0xEC, 0xED,
		//										/*0xF0,*/   0xF2,       0xF4,             0xF7,            /*0xFA,*/    0xFC, 0xFD,/* 0xFE,*/0xFF };

		byte nextOpcode = cpu.memory.Read(cpu.registers.pc);
		byte nextData = cpu.memory.Read(cpu.registers.pc + 1);
		//if (std::binary_search(uncheckedOpcodes.begin(), uncheckedOpcodes.end(), nextOpcode)) {
		//	int a = 1;
		//}

		//if((elapsedOpcodes % 100) == 0){
		//	int a = 1;
		//}
		//if (cpu.registers.pc == 0x289F) {
		//	int a = 1;
		//}

		if (nextOpcode == 0xDE && nextData == 0xFF && cpu.registers.a == 0x00) {
			int a = cpu.memory.Read(cpu.registers.pc - 1);
			int b = 1;
		}

		short cycles = 0;
		if (cpu.halted == false) {
			cycles = cpu.ExecuteOpcode();
		}
		if (nextOpcode == 0xDE) {
			int a = cpu.memory.Read(cpu.registers.pc - 1);
			int b = 1;
		}
		else {
			cycles = 4;
		}

		totalCycles += cycles;
		cpu.LCDStatusRegister(totalCycles);
		graphics.update(cycles);
		cpu.performInterupts();
		cpu.dividerRegisterINC(cycles);
		cpu.TimerCounterINC(cycles);

		elapsedOpcodes++;
	}
	graphics.updateWindow();
	graphics.updateTileWindow();
	graphics.updateBGMapWindow();
	elapsedFrames++;
}

void Emulator::Loop() {

	sf::Event event;
	while (graphics.window->isOpen()) {

		while (graphics.window->pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				graphics.window->close();
			}
			else if (event.type == sf::Event::KeyPressed) {

				if (event.key.code == sf::Keyboard::Up) {
					cpu.memory.buttons.up = true;
				}
				else if (event.key.code == sf::Keyboard::Down) {
					cpu.memory.buttons.down = true;
				}
				else if (event.key.code == sf::Keyboard::Left) {
					cpu.memory.buttons.left = true;
				}
				else if (event.key.code == sf::Keyboard::Right) {
					cpu.memory.buttons.right = true;
				}
				else if (event.key.code == sf::Keyboard::Z) {
					cpu.memory.buttons.buttonB = true;
				}
				else if (event.key.code == sf::Keyboard::X) {
					cpu.memory.buttons.buttonA = true;
				}
				else if (event.key.code == sf::Keyboard::C) {
					cpu.memory.buttons.select = true;
				}
				else if (event.key.code == sf::Keyboard::V) {
					cpu.memory.buttons.start = true;
				}
			}
			else if (event.type == sf::Event::KeyReleased) {

				if (event.key.code == sf::Keyboard::Up) {
					cpu.memory.buttons.up = false;
				}
				else if (event.key.code == sf::Keyboard::Down) {
					cpu.memory.buttons.down = false;
				}
				else if (event.key.code == sf::Keyboard::Left) {
					cpu.memory.buttons.left = false;
				}
				else if (event.key.code == sf::Keyboard::Right) {
					cpu.memory.buttons.right = false;
				}
				else if (event.key.code == sf::Keyboard::Z) {
					cpu.memory.buttons.buttonB = false;
				}
				else if (event.key.code == sf::Keyboard::X) {
					cpu.memory.buttons.buttonA = false;
				}
				else if (event.key.code == sf::Keyboard::C) {
					cpu.memory.buttons.select = false;
				}
				else if (event.key.code == sf::Keyboard::V) {
					cpu.memory.buttons.start = false;
				}
			}
		}

		Update();
	}
}
