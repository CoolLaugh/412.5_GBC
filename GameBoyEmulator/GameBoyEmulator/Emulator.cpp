#include "Emulator.h"

Emulator::Emulator() {

	cpu.memory.LoadRom("./gb/LOZLADX.gbc");
	cpu.ColorGameBoyMode = cpu.memory.ColorGameBoyMode;
	graphics.ColorGameBoyMode = cpu.memory.ColorGameBoyMode;
	cpu.PowerUpSequence();
	graphics.memory = &cpu.memory;
}

Emulator::~Emulator() {

}

void Emulator::Update() {

	while (totalCycles <= 70224 * cpu.speedMode) {
		
		short cycles = 0;
		if (cpu.halted == false) {
			cycles = cpu.ExecuteOpcode();
		}
		else {
			cycles = 4;
		}

		totalCycles += cycles;
		cpu.LCDStatusRegister(graphics.cyclesThisLine);
		graphics.update(cycles / cpu.speedMode);
		cpu.performInterupts();
		cpu.dividerRegisterINC(cycles);
		cpu.TimerCounterINC(cycles);

		elapsedOpcodes++;
	}
	totalCycles -= 70224;
	graphics.updateWindow();
	//graphics.updateTileWindow();
	graphics.updateBGMapWindow2();
	//graphics.updateColorPaletteWindow();
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
				else if (event.key.code == sf::Keyboard::F3) {
					cpu.memory.DumpMemory();
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

	byte* variablesState = new byte[0x100];
	int variablesIndex = 0;


	variablesState[variablesIndex++] = totalCycles;


	variablesState[variablesIndex++] = cpu.registers.a;
	variablesState[variablesIndex++] = cpu.registers.f;
	variablesState[variablesIndex++] = cpu.registers.b;
	variablesState[variablesIndex++] = cpu.registers.c;
	variablesState[variablesIndex++] = cpu.registers.d;
	variablesState[variablesIndex++] = cpu.registers.e;
	variablesState[variablesIndex++] = cpu.registers.h;
	variablesState[variablesIndex++] = cpu.registers.l;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.registers.sp).first;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.registers.sp).second;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.registers.pc).first;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.registers.pc).second;


	variablesState[variablesIndex++] = cpu.halted;
	variablesState[variablesIndex++] = cpu.stop;
	variablesState[variablesIndex++] = cpu.interupt;
	variablesState[variablesIndex++] = cpu.interuptEnable;
	variablesState[variablesIndex++] = cpu.interuptEnableInstructionCount;
	variablesState[variablesIndex++] = cpu.interuptDisable;
	variablesState[variablesIndex++] = cpu.interuptDisableInstructionCount;

	variablesState[variablesIndex++] = cpu.splitBytes(cpu.dividerCycles).first;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.dividerCycles).second;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.clockFrequency).first;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.clockFrequency).second;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.timerCycles).first;
	variablesState[variablesIndex++] = cpu.splitBytes(cpu.timerCycles).second;

	variablesState[variablesIndex++] = cpu.timerOverflow;
	variablesState[variablesIndex++] = cpu.ColorGameBoyMode;
	variablesState[variablesIndex++] = cpu.speedMode;


	variablesState[variablesIndex++] = cpu.splitBytes(graphics.cyclesThisLine).first;
	variablesState[variablesIndex++] = cpu.splitBytes(graphics.cyclesThisLine).second;


	variablesState[variablesIndex++] = memory.currentRamBank;
	variablesState[variablesIndex++] = memory.currentWramBank;
	variablesState[variablesIndex++] = memory.currentVramBank;
	variablesState[variablesIndex++] = memory.currentRomBank;
	variablesState[variablesIndex++] = memory.numberOfRamBanks;
	variablesState[variablesIndex++] = memory.ramBankEnabled;
	variablesState[variablesIndex++] = memory.mbc;
	variablesState[variablesIndex++] = memory.memoryModel;

	int sizeOfVariables = variablesIndex;
	int sizeOfVram = memory.vramBank.size() * 0x2000;
	int sizeOfMemorySpaceBank = memory.ramBank.size() * 0x2000;
	int sizeOfWram = memory.wramBank.size() * 0x1000;
	int sizeOfMemorySpaceAfterBank = 0x10000 - 0xE000;
	int sizeOfColorPalette = 0x40 + 0x40;

	int fileSize = sizeOfVariables +
					sizeOfVram +
					sizeOfMemorySpaceBank +
					sizeOfWram +
					sizeOfMemorySpaceAfterBank + 
					sizeOfColorPalette;

	byte* state = new byte[fileSize];
	int index = variablesIndex;	
	
	memcpy(state, variablesState, sizeOfVariables);

	for (size_t bank = 0; bank < memory.vramBank.size(); bank++) {

		for (size_t address = 0x00; address < 0x2000; address++) {

			state[index++] = memory.vramBank[bank][address];
		}
	}

	for (size_t bank = 0; bank < memory.ramBank.size(); bank++) {

		for (size_t address = 0x00; address < 0x2000; address++) {

			state[index++] = memory.ramBank[bank][address];
		}
	}

	for (size_t bank = 0; bank < memory.wramBank.size(); bank++) {

		for (size_t address = 0x00; address < 0x1000; address++) {

			state[index++] = memory.wramBank[bank][address];
		}
	}

	for (size_t i = 0xE000; i <= 0xFFFF; i++) {
		state[index++] = memory.memorySpace[i];
	}

	for (size_t i = 0x00; i < 0x40; i++) {
		state[index++] = memory.BGColorPalette[i];
	}

	for (size_t i = 0x00; i < 0x40; i++) {
		state[index++] = memory.SpriteColorPalette[i];
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


	totalCycles = state[index++];


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
	cpu.ColorGameBoyMode = state[index++];
	cpu.memory.ColorGameBoyMode = cpu.ColorGameBoyMode;
	graphics.ColorGameBoyMode = cpu.ColorGameBoyMode;
	cpu.speedMode = state[index++];


	graphics.cyclesThisLine = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;

	auto memory = cpu.memory;

	memory.currentRamBank = state[index++];
	memory.currentWramBank = state[index++];
	memory.currentVramBank = state[index++];
	memory.currentRomBank = state[index++];
	memory.numberOfRamBanks = state[index++];
	memory.ramBankEnabled = state[index++];
	memory.mbc = state[index++];
	memory.memoryModel = (Memory::MemoryModel)state[index++];

	memory.CreateRamBanks();

	for (size_t bank = 0; bank < memory.vramBank.size(); bank++) {

		for (size_t address = 0x00; address < 0x2000; address++) {

			memory.vramBank[bank][address] = state[index++];
		}
	}

	for (size_t bank = 0; bank < memory.ramBank.size(); bank++) {

		for (size_t address = 0x00; address < 0x2000; address++) {

			memory.ramBank[bank][address] = state[index++];
		}
	}

	for (size_t bank = 0; bank < memory.wramBank.size(); bank++) {

		for (size_t address = 0x00; address < 0x1000; address++) {

			memory.wramBank[bank][address] = state[index++];
		}
	}

	for (size_t i = 0xE000; i <= 0xFFFF; i++) {
		memory.memorySpace[i] = state[index++];
	}

	for (size_t i = 0x00; i < 0x40; i++) {
		memory.BGColorPalette[i] = state[index++];
	}

	for (size_t i = 0x00; i < 0x40; i++) {
		memory.SpriteColorPalette[i] = state[index++];
	}

	cpu.outputStateBuffer += "SAVE STATE lOADED\n";
}
