#include "Gameboy.h"



// setup the cpu, memory, ppu, and apu
Gameboy::Gameboy(std::string Filename) {

	cpu.graphics = &ppu;
	cpu.memory = &memory;
	RomFilename = Filename;
	memory.LoadRom("./gb/" + RomFilename);
	cpu.ColorGameBoyMode = memory.ColorGameBoyMode;
	ppu.ColorGameBoyMode = memory.ColorGameBoyMode;
	cpu.PowerUpSequence();
	ppu.memory = &memory;
	apu.memory = &memory;
	cpu.apu = &apu;
}



Gameboy::~Gameboy() {

}



// advance the emulation by the number of instructions
void Gameboy::Advance(int opcodeCount) {

	for (size_t i = 0; i < opcodeCount; i++) {

		byte LY = memory.Read(Address::LY);

		if (cpu.halted == false) {
			totalCycles += cpu.ExecuteOpcode();
		}
		else {
			totalCycles += 4;
			cpu.AdvanceClocks(4);
		}

		cpu.LCDStatusRegister(ppu.clocksThisLine);
		cpu.performInterupts();

		if (memory.Read(Address::LY) == 144 && LY == 143) {
			redrawScreen = true;
			elapsedFrames++;
		}
	}

	//totalCycles -= 70224 * cpu.speedMode;
	if (memory.externalRamChanged == true) {
		memory.SaveExternalRam("./gb/" + RomFilename);
	}
}



// change the state of a button from not pressed to pressed and vice versa
void Gameboy::SetButtonState(Buttons button, bool pressed) {

	switch (button) {
	case Gameboy::down:
		memory.buttons.down = pressed;
		break;
	case Gameboy::up:
		memory.buttons.up = pressed;
		break;
	case Gameboy::left:
		memory.buttons.left = pressed;
		break;
	case Gameboy::right:
		memory.buttons.right = pressed;
		break;
	case Gameboy::start:
		memory.buttons.start = pressed;
		break;
	case Gameboy::select:
		memory.buttons.select = pressed;
		break;
	case Gameboy::B:
		memory.buttons.buttonB = pressed;
		break;
	case Gameboy::A:
		memory.buttons.buttonA = pressed;
		break;
	}

}



// get pixels for the main screen
byte * Gameboy::GetScreenPixels() {
	return ppu.backgroundPixels;
}


// get pixels for the background map in memory
byte * Gameboy::GetBackgroundPixels() {
	return ppu.BGMapBackgroundPixels;
}


// get pixels for tile data in memory
byte * Gameboy::GetTilePixels() {
	return ppu.tileMemoryPixels;
}


// get pixels for showing palette color
byte * Gameboy::GetColorPalettePixels() {
	return ppu.ColorPalettePixels;
}



// save the current state of the gameboy to file
void Gameboy::SaveState(int stateNumber) {

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
	variablesState[variablesIndex++] = SplitBytes(cpu.registers.sp).lsb;
	variablesState[variablesIndex++] = SplitBytes(cpu.registers.sp).msb;
	variablesState[variablesIndex++] = SplitBytes(cpu.registers.pc).lsb;
	variablesState[variablesIndex++] = SplitBytes(cpu.registers.pc).msb;


	variablesState[variablesIndex++] = cpu.halted;
	variablesState[variablesIndex++] = cpu.stop;
	variablesState[variablesIndex++] = cpu.interupt;
	variablesState[variablesIndex++] = cpu.interuptEnable;
	variablesState[variablesIndex++] = cpu.interuptEnableInstructionCount;
	variablesState[variablesIndex++] = cpu.ColorGameBoyMode;
	variablesState[variablesIndex++] = cpu.speedMode;
	variablesState[variablesIndex++] = cpu.lastLY;


	variablesState[variablesIndex++] = SplitBytes(ppu.clocksThisLine).lsb;
	variablesState[variablesIndex++] = SplitBytes(ppu.clocksThisLine).msb;


	variablesState[variablesIndex++] = memory.stopHblankDMA;
	variablesState[variablesIndex++] = memory.currentRamBank;
	variablesState[variablesIndex++] = memory.currentWramBank;
	variablesState[variablesIndex++] = memory.currentVramBank;
	variablesState[variablesIndex++] = SplitBytes(memory.currentRomBank).lsb;
	variablesState[variablesIndex++] = SplitBytes(memory.currentRomBank).msb;
	variablesState[variablesIndex++] = memory.numberOfRamBanks;
	variablesState[variablesIndex++] = memory.ramBankEnabled;
	variablesState[variablesIndex++] = memory.mbc;
	variablesState[variablesIndex++] = memory.memoryModel;
	variablesState[variablesIndex++] = SplitBytes(memory.dividerRegister).lsb;
	variablesState[variablesIndex++] = SplitBytes(memory.dividerRegister).msb;
	variablesState[variablesIndex++] = memory.timerOverflow;
	variablesState[variablesIndex++] = memory.oldBit;

	int sizeOfVariables = variablesIndex;
	int sizeOfVram = (int)memory.vramBank.size() * 0x2000;
	int sizeOfMemorySpaceBank = (int)memory.ramBank.size() * 0x2000;
	int sizeOfWram = (int)memory.wramBank.size() * 0x1000;
	int sizeOfSpriteData = 0xA0;
	int sizeOfMemorySpace = 0x100;
	int sizeOfColorPalette = 0x40 + 0x40;

	int fileSize = sizeOfVariables +
		sizeOfVram +
		sizeOfMemorySpaceBank +
		sizeOfWram +
		sizeOfSpriteData +
		sizeOfMemorySpace +
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

	for (size_t i = 0; i < 0xA0; i++) {
		state[index++] = memory.OAM[i];
	}

	for (size_t i = 0; i < 0x100; i++) {
		state[index++] = memory.memorySpace[i];
	}

	for (size_t i = 0x00; i < 0x40; i++) {
		state[index++] = memory.BGColorPalette[i];
	}

	for (size_t i = 0x00; i < 0x40; i++) {
		state[index++] = memory.SpriteColorPalette[i];
	}

	std::string stateFilename = RomFilename.substr(0, RomFilename.find_last_of('.')) + "SaveState" + std::to_string(stateNumber) + ".ss";
	std::ofstream stateFile(stateFilename, std::ofstream::binary);
	stateFile.write((char *)state, fileSize);
	stateFile.close();


	cpu.outputStateBuffer += "SAVE STATE CREATED\n";
}



// load the state of the gameboy from a file
void Gameboy::LoadState(int stateNumber) {

	std::string stateFilename = RomFilename.substr(0, RomFilename.find_last_of('.')) + "SaveState" + std::to_string(stateNumber) + ".ss";

	if(fs::exists(stateFilename) == false) {
		return; // do nothing
	}

	std::ifstream stateFile(stateFilename, std::ifstream::binary);

	stateFile.seekg(0, std::ios::end);
	int length = (int)stateFile.tellg();
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

	cpu.registers.sp = Combinebytes(state[index], state[index + 1]);
	index += 2;
	cpu.registers.pc = Combinebytes(state[index], state[index + 1]);
	index += 2;


	cpu.halted = state[index++];
	cpu.stop = state[index++];
	cpu.interupt = state[index++];
	cpu.interuptEnable = state[index++];
	cpu.interuptEnableInstructionCount = state[index++];
	cpu.ColorGameBoyMode = state[index++];
	cpu.speedMode = state[index++];
	cpu.lastLY = state[index++];

	ppu.clocksThisLine = Combinebytes(state[index], state[index + 1]);
	index += 2;

	memory.stopHblankDMA = state[index++];
	memory.currentRamBank = state[index++];
	memory.currentWramBank = state[index++];
	memory.currentVramBank = state[index++];
	memory.currentRomBank = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;
	memory.numberOfRamBanks = state[index++];
	memory.ramBankEnabled = state[index++];
	memory.mbc = state[index++];
	memory.memoryModel = (Memory::MemoryModel)state[index++];
	memory.dividerRegister = cpu.Combinebytes(state[index], state[index + 1]);
	index += 2;
	memory.timerOverflow = state[index++];
	memory.oldBit = state[index++];

	memory.ColorGameBoyMode = cpu.ColorGameBoyMode;
	ppu.ColorGameBoyMode = cpu.ColorGameBoyMode;

	memory.CreateRamBanks(); // clear and recreate bank memory

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

	for (size_t i = 0; i < 0xA0; i++) {
		memory.OAM[i] = state[index++];
	}

	for (size_t i = 0; i < 0x100; i++) {
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
