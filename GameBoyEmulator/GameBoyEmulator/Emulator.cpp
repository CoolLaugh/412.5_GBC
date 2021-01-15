#include "Emulator.h"

Emulator::Emulator() {

	cpu.memory.LoadRom("./gb/LOZLA.gb");
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
		byte nextData1 = cpu.memory.Read(cpu.registers.pc + 1);
		byte nextData2 = cpu.memory.Read(cpu.registers.pc + 2);
		byte nextData3 = cpu.memory.Read(cpu.registers.pc + 3);
		//if (std::binary_search(uncheckedOpcodes.begin(), uncheckedOpcodes.end(), nextOpcode)) {
		//	int a = 1;
		//}

		short cycles = 0;
		if (cpu.halted == false) {
			cycles = cpu.ExecuteOpcode();
		}
		else {
			cycles = 4;
		}

		totalCycles += cycles;
		cpu.LCDStatusRegister(graphics.cyclesThisLine);
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
				else if (event.key.code == sf::Keyboard::F1) {
					saveState();
				}
				else if (event.key.code == sf::Keyboard::F2) {
					loadState();
				}
				else if (event.key.code == sf::Keyboard::F5) {
					cpu.logState = !cpu.logState;
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

void Emulator::saveState() {

	auto memory = cpu.memory;

	int sizeOfVariables = 34;
	int sizeOfMemorySpaceBeforeBank = 0xA000 - 0x8000;
	int sizeOfMemorySpaceBank = memory.ramBank.size() * 0x2000;
	int sizeOfMemorySpaceAfterBank = 0x10000 - 0xC000;

	int fileSize = sizeOfVariables +
					sizeOfMemorySpaceBeforeBank +
					sizeOfMemorySpaceBank +
					sizeOfMemorySpaceAfterBank;

	byte* state = new byte[fileSize];
	int index = 0;

	state[index++] = cpu.registers.a;
	state[index++] = cpu.registers.f;
	state[index++] = cpu.registers.b;
	state[index++] = cpu.registers.c;
	state[index++] = cpu.registers.d;
	state[index++] = cpu.registers.e;
	state[index++] = cpu.registers.h;
	state[index++] = cpu.registers.l;
	state[index++] = cpu.splitBytes(cpu.registers.sp).first;
	state[index++] = cpu.splitBytes(cpu.registers.sp).second;
	state[index++] = cpu.splitBytes(cpu.registers.pc).first;
	state[index++] = cpu.splitBytes(cpu.registers.pc).second;

	state[index++] = cpu.halted;
	state[index++] = cpu.stop;
	state[index++] = cpu.interupt;
	state[index++] = cpu.interuptEnable;
	state[index++] = cpu.interuptEnableInstructionCount;
	state[index++] = cpu.interuptDisable;
	state[index++] = cpu.interuptDisableInstructionCount;
	state[index++] = cpu.splitBytes(cpu.dividerCycles).first;
	state[index++] = cpu.splitBytes(cpu.dividerCycles).second;
	state[index++] = cpu.splitBytes(cpu.clockFrequency).first;
	state[index++] = cpu.splitBytes(cpu.clockFrequency).second;
	state[index++] = cpu.splitBytes(cpu.timerCycles).first;
	state[index++] = cpu.splitBytes(cpu.timerCycles).second;
	state[index++] = cpu.timerOverflow;

	state[index++] = cpu.splitBytes(graphics.cyclesThisLine).first;
	state[index++] = cpu.splitBytes(graphics.cyclesThisLine).second;

	state[index++] = memory.currentRamBank;
	state[index++] = memory.currentRomBank;
	state[index++] = memory.numberOfRamBanks;
	state[index++] = memory.ramBankEnabled;
	state[index++] = memory.mbc;
	state[index++] = memory.memoryModel;


	for (size_t i = 0x8000; i <= 0x9FFF; i++) {
		state[index++] = memory.memorySpace[i];
	}

	for (size_t i = 0; i < memory.ramBank.size(); i++) {
		for (size_t j = 0; j < 0x2000; j++) {
			state[index++] = memory.ramBank[i][j];
		}
	}

	for (size_t i = 0xC000; i <= 0xFFFF; i++) {
		state[index++] = memory.memorySpace[i];
	}

	std::ofstream stateFile("saveState", std::ofstream::binary);
	stateFile.write((char *)state, fileSize);
	stateFile.close();


	cpu.outputStateBuffer += "SAVE STATE CREATED\n";
}

void Emulator::loadState() {

	std::ifstream stateFile("saveState", std::ifstream::binary);

	stateFile.seekg(0, std::ios::end);
	int length = stateFile.tellg();
	stateFile.seekg(0, std::ios::beg);

	byte* state = new byte[length];
	stateFile.read((char*)state, length);
	stateFile.close();

	int index = 0;

	cpu.registers.a = state[index++];
	cpu.registers.f = state[index++];
	cpu.registers.b = state[index++];
	cpu.registers.c = state[index++];
	cpu.registers.d = state[index++];
	cpu.registers.e = state[index++];
	cpu.registers.h = state[index++];
	cpu.registers.l = state[index++];

	cpu.registers.sp = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;
	cpu.registers.pc = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;

	cpu.halted = state[index++];
	cpu.stop = state[index++];
	cpu.interupt = state[index++];
	cpu.interuptEnable = state[index++];
	cpu.interuptEnableInstructionCount = state[index++];
	cpu.interuptDisable = state[index++];
	cpu.interuptDisableInstructionCount = state[index++];


	cpu.dividerCycles = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;
	cpu.clockFrequency = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;
	cpu.timerCycles = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;

	cpu.timerOverflow = state[index++];

	graphics.cyclesThisLine = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;

	auto memory = cpu.memory;

	memory.currentRamBank = state[index++];
	memory.currentRomBank = state[index++];
	memory.numberOfRamBanks = state[index++];
	memory.ramBankEnabled = state[index++];
	memory.mbc = state[index++];
	memory.memoryModel = (Memory::MemoryModel)state[index++];


	for (size_t i = 0x8000; i <= 0x9FFF; i++) {
		memory.memorySpace[i] = state[index++];
	}

	for (size_t i = 0; i < memory.ramBank.size(); i++) {
		for (size_t j = 0; j < 0x2000; j++) {
			memory.ramBank[i][j] = state[index++];
		}
	}

	for (size_t i = 0xC000; i <= 0xFFFF; i++) {
		memory.memorySpace[i] = state[index++];
	}

	cpu.outputStateBuffer += "SAVE STATE lOADED\n";
}
