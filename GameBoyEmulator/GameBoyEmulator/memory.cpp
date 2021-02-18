#include "memory.h"
#pragma warning(disable : 4996)



Memory::Memory() {

	memorySpace = new byte[memorySize];
	memset(memorySpace, 0, memorySize);
}

void Memory::PowerUpSequence() {

	memorySpace[0xFF05] = 0x00;
	memorySpace[0xFF06] = 0x00;
	memorySpace[0xFF07] = 0x00;
	memorySpace[0xFF10] = 0x80;
	memorySpace[0xFF11] = 0xBF;
	memorySpace[0xFF12] = 0xF3;
	memorySpace[0xFF14] = 0xBF;
	memorySpace[0xFF16] = 0x3F;
	memorySpace[0xFF17] = 0x00;
	memorySpace[0xFF19] = 0xBF;
	memorySpace[0xFF1A] = 0x7F;
	memorySpace[0xFF1B] = 0xFF;
	memorySpace[0xFF1C] = 0x9F;
	memorySpace[0xFF1E] = 0xBF;
	memorySpace[0xFF20] = 0xFF;
	memorySpace[0xFF21] = 0x00;
	memorySpace[0xFF22] = 0x00;
	memorySpace[0xFF23] = 0xBF;
	memorySpace[0xFF24] = 0x77;
	memorySpace[0xFF25] = 0xF3;
	memorySpace[0xFF26] = 0xF1;
	memorySpace[0xFF40] = 0x91;
	memorySpace[0xFF42] = 0x00;
	memorySpace[0xFF43] = 0x00;
	memorySpace[0xFF45] = 0x00;
	memorySpace[0xFF47] = 0xFC;
	memorySpace[0xFF48] = 0xFF;
	memorySpace[0xFF49] = 0xFF;
	memorySpace[0xFF4A] = 0x00;
	memorySpace[0xFF4B] = 0x00;
	memorySpace[0xFFFF] = 0x00;

	// cycle accurate docs initial value
	if (ColorGameBoyMode == false) {
		dividerRegister = 0xABCC;
	}
	else {
		dividerRegister = 0x1EA0;
	}
	
	const byte ColorGameboyFFXXValues[256] = {
		0xCF, 0x00, 0x7C, 0xFF, 0x44, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
		0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
		0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00, 0xFE, 0xFF, 0xFF, 0xFF,
		0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
		0x45, 0xEC, 0x42, 0xFA, 0x08, 0xB7, 0x07, 0x5D, 0x01, 0xF5, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
		0x0B, 0xF8, 0xC2, 0xCA, 0xF4, 0xF9, 0x0D, 0x7F, 0x44, 0x6D, 0x19, 0xFE, 0x46, 0x97, 0x33, 0x5E,
		0x08, 0xFF, 0xD1, 0xFF, 0xC6, 0x8B, 0x24, 0x74, 0x12, 0xFC, 0x00, 0x9F, 0x94, 0xB7, 0x06, 0xD5,
		0x40, 0x7A, 0x20, 0x9E, 0x04, 0x5F, 0x41, 0x2F, 0x3D, 0x77, 0x36, 0x75, 0x81, 0x8A, 0x70, 0x3A,
		0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05, 0xF9, 0x00, 0x0B, 0x00
	};

	if (ColorGameBoyMode == true) {
		memcpy(&memorySpace[0xFF00], ColorGameboyFFXXValues, 256);
	}
}

bool Memory::LoadRom(const std::string fileName) {

	std::ifstream romFile(fileName, std::ios::in, std::ios::binary);

	romFile.seekg(0, std::ios::end);
	int cartridgeLength = romFile.tellg();
	romFile.seekg(0, std::ios::beg);

	cartridge = new byte[cartridgeLength];
	memset(cartridge, 0, cartridgeLength);

	romFile.read((char*)cartridge, cartridgeLength);
	romFile.close();

	memcpy(memorySpace, cartridge, 0x8000);
	currentRomBank = 1;

	if (cartridge[Address::CGBFlag] == 0x80 || cartridge[Address::CGBFlag] == 0xC0) {
		ColorGameBoyMode = true;
	}

	BGColorPalette = new byte[0x40]; // these exist in non color mode to make save states easier
	memset(BGColorPalette, 0xFF, 0x40);

	SpriteColorPalette = new byte[0x40];
	memset(SpriteColorPalette, 0, 0x40);

	CreateRamBanks();

	LoadExternalRam(fileName);

	return true;
}

void Memory::CreateRamBanks() {

	// note: use after rom is loaded
	// 2.5.4
	byte cartridgeType = Read(Address::CartridgeType);

	switch (cartridgeType) {
	case 0x0: mbc = 0; break;
	case 0x1: mbc = 1; break;
	case 0x2: mbc = 1; break;
	case 0x3: mbc = 1; break;
	case 0x5: mbc = 2; break;
	case 0x6: mbc = 2; break;
	case 0x8: mbc = 0; break;
	case 0x9: mbc = 0; break;
	case 0xB: mbc = 0; break;
	case 0xC: mbc = 0; break;
	case 0xD: mbc = 0; break;
	case 0xF: mbc = 3; break;
	case 0x10: mbc = 3; break;
	case 0x11: mbc = 3; break;
	case 0x12: mbc = 3; break;
	case 0x13: mbc = 3; break;
	case 0x19: mbc = 5; break;
	case 0x1A: mbc = 5; break;
	case 0x1B: mbc = 5; break;
	case 0x1C: mbc = 5; break;
	case 0x1D: mbc = 5; break;
	case 0x1E: mbc = 5; break;
	case 0x20: mbc = 6; break;
	case 0x22: mbc = 7; break;
	}

	byte romSize = Read(Address::RomSize);
	byte ramSize = Read(Address::RamSize);

	switch (ramSize) {
	case 0: numberOfRamBanks = 1; break;
	case 1: numberOfRamBanks = 1; break;
	case 2: numberOfRamBanks = 1; break;
	case 3: numberOfRamBanks = 4; break;
	case 4: numberOfRamBanks = 16; break;
	case 5: numberOfRamBanks = 8; break;
	}


	for (size_t i = 0; i < ramBank.size(); i++) {
		byte* ptr = ramBank[i];
		delete[] ptr;
	}
	ramBank.clear();
	for (size_t i = 0; i < numberOfRamBanks; i++) {
		byte* bank = new byte[RamBankSize];
		memset(bank, 0, RamBankSize);
		ramBank.push_back(bank);
	}


	int wramBankSize = 0;
	if (ColorGameBoyMode == true) {
		wramBankSize = 8;
	}
	else {
		wramBankSize = 2;
	}

	for (size_t i = 0; i < wramBank.size(); i++) {
		byte* ptr = wramBank[i];
		delete[] ptr;
	}
	wramBank.clear();
	for (size_t i = 0; i < wramBankSize; i++) {
		byte* bank = new byte[0x1000];
		memset(bank, 0, 0x1000);
		wramBank.push_back(bank);
	}


	int vramBankSize = 1;
	if (ColorGameBoyMode == true) {
		vramBankSize = 2;
	}

	for (size_t i = 0; i < vramBank.size(); i++) {
		byte* ptr = vramBank[i];
		delete[] ptr;
	}
	vramBank.clear();
	for (size_t i = 0; i < 2; i++) {
		byte* bank = new byte[0x2000];
		memset(bank, 0, 0x2000);
		vramBank.push_back(bank);
	}
}

byte Memory::Read(word address, int vRamBank) {

	//if (address == 0xFF26) { // hack for no audio
	//	return 0x00;
	//}

	// rom bank 0x4000 - 0x7FFF
	if (address >= 0x4000 && address <= 0x7FFF) {

		word offset = address - 0x4000;
		int memoryBankOffset = 0x4000 * currentRomBank;
		return cartridge[memoryBankOffset + offset];
	}
	else if (address >= 0x8000 && address <= 0x9FFF) {
		if (vRamBank == -1) {
			return vramBank[currentVramBank][address - 0x8000];
		}
		else {
			return vramBank[vRamBank][address - 0x8000];
		}
	}
	// Ram bank 0xA000 - 0xBFFF
	else if (address >= 0xA000 && address <= 0xBFFF) {
		return ramBank[currentRamBank][address - 0xA000];
	}
	else if (address >= 0xC000 && address <= 0xCFFF) {
		return wramBank[0][address - 0xC000];
	}
	else if (address >= 0xD000 && address <= 0xDFFF) {
		return wramBank[currentWramBank][address - 0xD000];
	}
	else if (address == 0xFF04) {
		return (byte)((dividerRegister >> 8) & 0xFF);
	}
	else if (address == 0xFF55 && ColorGameBoyMode == true) {
		if (vramDMATransferProgress == 0 && vramDMATransferLength == 0) {
			return 0xFF;
		}
		else {
			byte lengthRemaining = ((vramDMATransferLength - vramDMATransferProgress) / 10) - 1;
			if (stopHblankDMA == true) {
				lengthRemaining |= Bits::b7;
			}
			return lengthRemaining;
		}
	}
	// controls 0xFF00
	else if (address == Address::Joypad) {
		return GetJoypadState();
	}
	return memorySpace[address];
}



void Memory::Write(word address, byte data) {

	// 0 - 7FFF cartrigde
	if (address >= 0x0000 && address <= 0x7FFF) {
		WriteMBCSwitch(address, data);
	}
	// 0x8000 - 0x9FFF 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)
	else if (address >= 0x8000 && address <= 0x9FFF) {
		vramBank.at(currentVramBank)[address - 0x8000] = data;
		TileChanged = true;
	}
	else if (address >= 0xA000 && address <= 0xBFFF) { // 8kb external ram
		externalRamChanged = true;
		if (ramBankEnabled == true && mbc == 1 || mbc == 3 || mbc == 5) {
			word newAddress = address - 0xA000;
			ramBank.at(currentRamBank)[newAddress] = data;
		}
		if (mbc == 2 && address < 0xA200) {
			word newAddress = address - 0xA000;
			ramBank.at(currentRamBank)[newAddress] = data;
		}
	}
	// 0xC000 - 0xCFFF 4KB Work RAM (WRAM) bank 0
	else if (address >= 0xC000 && address <= 0xCFFF) {
		wramBank.at(0)[address - 0xC000] = data;
	}
	// 0xD000 - 0xDFFF 4KB Work RAM (WRAM) bank 1~N (1 for gameboy, 1-7 for gameboy color)
	else if (address >= 0xD000 && address <= 0xDFFF) {
		wramBank.at(currentWramBank)[address - 0xD000] = data;
	}
	// 0xE000 - 0xFDFF Mirror of C000~DDFF (ECHO RAM) typically not used
	// 0xFE00 - 0xFE9F Sprite attribute table
	// 0xFEA0 - 0xFEFF Not Usable
	else if (address == 0xFF02) { // serial control
		memorySpace[address] = data;
		if (data == 0x81) { // output for test rom
			std::cout << memorySpace[0xFF01];
		}
	}
	else if (address == 0xFF04) { // DIV register
		dividerRegister = 0;
	}
	else if (address == 0xFF05) { // TIMA 
		if (writingToTIMA == false) { // writing will fail when TMA is being written to TIMA
			memorySpace[address] = data;
		}
	}
	else if (address == 0xFF06) { // TMA / Timer Module
		memorySpace[address] = data;
		if (writingToTIMA == true) { // writing to TMA in the same cycle as TMA is being loaded into TIMA causes the same value to be written to TIMA
			memorySpace[Address::Timer] = data;
		}
	}
	else if (address == 0xFF14) {
		memorySpace[address] = data;
		resetSC1Length = true;
	}
	else if (address == 0xFF44) {
		memorySpace[address] = 0;
	}
	else if (address == 0xFF46) { // DMA

		word DMAAddress = data;
		DMAAddress <<= 8;

		for (word i = 0; i < 0xA0; i++) {
			memorySpace[0xFE00 + i] = Read(DMAAddress + i);
		}
	}
	else if (address == 0xFF4D) {
		memorySpace[address] = data | 0x7E;
	}
	else if (address == 0xFF4F) {
		if (ColorGameBoyMode == true) {
			currentVramBank = data & Bits::b0;
			memorySpace[address] = (~Bits::b0) | (data & Bits::b0);
		}
		else {
			memorySpace[address] = data;
		}
	}
	else if (address == 0xFF55) {
		memorySpace[address] = data;
		if (ColorGameBoyMode == true) {

			if (vramDMATransferLength != 0 && (data & Bits::b7) == 0) {
				vramDMATransferLength = 0;
				vramDMATransferProgress = 0;
				return;
			}

			vramDMATransferSource = (memorySpace[0xFF51] << 8) | (memorySpace[0xFF52] & 0xF0);
			vramDMATransferDestination = (memorySpace[0xFF53] << 8) | (memorySpace[0xFF54] & 0xF0);
			vramDMATransferLength = data & 0x7F;
			vramDMATransferLength++;
			vramDMATransferLength *= 0x10;

			if ((data & Bits::b7) == 0) {
				for (size_t i = 0; i < vramDMATransferLength; i++) {
					Write(vramDMATransferDestination + i, Read(vramDMATransferSource + i));
				}
				vramDMATransferLength = 0;
			}
			else {
				vramDMATransferProgress = 0;
			}
		}
	}
	else if (address == 0xFF69) {
		if (ColorGameBoyMode == true) {
			
			byte index = memorySpace[0xFF68] & 0x3F;
			BGColorPalette[index] = data;
			
			if ((memorySpace[0xFF68] & Bits::b7) != 0) {
				memorySpace[0xFF68] = (memorySpace[0xFF68] + 1) & 0xBF;
			}
		}
		else {
			memorySpace[address] = data;
		}
	}
	else if (address == 0xFF6B) {
		if (ColorGameBoyMode == true) {

			byte index = memorySpace[0xFF6A] & 0x3F;
			SpriteColorPalette[index] = data;

			if ((memorySpace[0xFF6A] & Bits::b7) != 0) {
				memorySpace[0xFF6A] = (memorySpace[0xFF6A] + 1) & 0xBF;
			}
		}
		else {
			memorySpace[address] = data;
		}
	}
	else if (address == 0xFF70) {
		if (ColorGameBoyMode == true) {
			currentWramBank = (data & 0x7);
			if(currentWramBank == 0) currentWramBank = 1;
			memorySpace[address] = data;
		}
		else {
			memorySpace[address] = data;
		}
	}
	else {
		memorySpace[address] = data; // no special behaviour
	}
}

void Memory::WriteMBCSwitch(word address, byte data) {

	switch (mbc) {
	case 1:
		if (address < 0x2000) {
			mbc1_00001FFF(data);
		}
		else if (address < 0x4000) {
			mbc1_20003FFF(address, data);
		}
		else if (address < 0x6000) {
			mbc1_40005FFF(data);
		}
		else {
			mbc1_60007FFF(data);
		}
		break;
	case 2:
		if (address < 0x4000) {
			mbc2_00003FFF(address, data);
		}
		break;
	case 3:
		if (address < 0x2000) {
			mbc3_00001FFF(data);
		}
		else if (address < 0x4000) {
			mbc3_20003FFF(data);
		}
		else if (address < 0x6000) {
			mbc3_40005FFF(data);
		}
		else {
			mbc3_60007FFF(data);
		}
		break;
	case 5:
		if (address < 0x2000) {
			mbc5_00001FFF(data);
		}
		else if (address < 0x3000) {
			mbc5_20002FFF(data);
		}
		else if (address < 0x4000) {
			mbc5_30003FFF(data);
		}
		else if (address < 0x6000) {
			mbc5_40005FFF(data);
		}
		break;
	}
}

void Memory::mbc1_00001FFF(byte data) {
	data &= 0x0F;
	ramBankEnabled = (data == 0xA);
}

void Memory::mbc1_20003FFF(word address, byte data) {

	data &= 0x1F;
	if (data == 0) { // values of 0 and 1 do the same thing
		data = 1;
	}
	currentRomBank &= 0xE0; // top 3 bits must be preserved
	currentRomBank |= data;
}

void Memory::mbc1_40005FFF(byte data) {

	if (memoryModel == mm4_32) {

		data &= 3;
		currentRamBank = data;
	}
	else if (memoryModel == mm16_8) {

		data &= 3;
		data <<= 5;
		currentRamBank |= data;
	}
}

void Memory::mbc1_60007FFF(byte data) {

	data &= 1;

	if (data == 0) {
		memoryModel = mm16_8;
	}
	else if (data == 1) {
		memoryModel = mm4_32;
	}
}

void Memory::mbc2_00003FFF(word address, byte data) {

	if (BitTest(address, 8) == false) {
		data &= 0x0F;
		ramBankEnabled = (data == 0x0A);
	}
	else {
		if (data == 0) {
			data = 1;
		}
		currentRomBank = data;
	}

}

void Memory::mbc3_00001FFF(byte data) {
	mbc1_00001FFF(data);
}

void Memory::mbc3_20003FFF(byte data) {

	data &= 0x7F;
	if (data == 0) { // values of 0 and 1 do the same thing
		data = 1;
	}
	currentRomBank = data;
}

void Memory::mbc3_40005FFF(byte data) {
	if (data <= 3) {
		data &= 3;
		currentRamBank = data;
	}
	else if (data >= 0x8 && data <= 0xC) {
		// real time clock register
	}

}

void Memory::mbc3_60007FFF(byte data) {

	// latch the current time by writing 0x00 then 0x01
}

void Memory::mbc5_00001FFF(byte data) {
	mbc1_00001FFF(data);
}

void Memory::mbc5_20002FFF(byte data) {
	currentRomBank &= 0xFF00;
	currentRomBank |= data;
}

void Memory::mbc5_30003FFF(byte data) {
	word bit8 = data & 0x1;
	bit8 <<= 8;
	currentRomBank |= bit8;
}

void Memory::mbc5_40005FFF(byte data) {
	data &= 0x0F;
	currentRamBank = data;
}


byte Memory::GetJoypadState() {

	byte selectedKeys = memorySpace[Address::Joypad];

	selectedKeys = ~selectedKeys;
	selectedKeys &= 0x30;

	byte buttonsPressed = 0xFF;

	if ((selectedKeys & 0x20) != 0) { // button keys

		if (buttons.start == true) buttonsPressed &= ~(1 << 3);
		if (buttons.select == true) buttonsPressed &= ~(1 << 2);
		if (buttons.buttonB == true) buttonsPressed &= ~(1 << 1);
		if (buttons.buttonA == true) buttonsPressed &= ~(1 << 0);

		buttonsPressed &= ~Bits::b5;
	}
	else if ((selectedKeys & 0x10) != 0) { // direction keys

		if (buttons.down == true) buttonsPressed &= ~(1 << 3);
		if (buttons.up == true) buttonsPressed &= ~(1 << 2);
		if (buttons.left == true) buttonsPressed &= ~(1 << 1);
		if (buttons.right == true) buttonsPressed &= ~(1 << 0);

		buttonsPressed &= ~Bits::b4;
	}

	return buttonsPressed;
}



// below replicates the exact hardware of the divider register and timer counter 
// as described in section 5 of The Cycle-Accurate Game Boy Docs by Antonio Nino Diaz
void Memory::IncrementDivAndTimerRegisters(byte clocks) {

	for (size_t i = 0; i < clocks; i++) {

		if (writingToTIMA == true) {
			writingToTIMACounter++;
			if (writingToTIMACounter > 4) { // it takes one cycle (4 clocks) to write TMA to TIMA
				writingToTIMA = false;
			}
		}

		dividerRegister++;

		bool newBit = false;
		switch (memorySpace[Address::TimerControl] & 0x3) {
		case 0: newBit = BitTest(dividerRegister, 9); break;
		case 1: newBit = BitTest(dividerRegister, 3); break;
		case 2: newBit = BitTest(dividerRegister, 5); break;
		case 3: newBit = BitTest(dividerRegister, 7); break;
		}

		newBit &= BitTest(memorySpace[Address::TimerControl], 2);

		if ((!newBit) & oldBit) {
			if (memorySpace[Address::Timer] == 0xFF) { // about to overflow
				timerOverflow = true;
			}
			memorySpace[Address::Timer]++;
		}

		if (timerOverflow == true) {
			timerOverflowCounter++;
			if (timerOverflowCounter > 4) { // there is a one cycle (4 clocks) delay to detect if the timer overflows

				timerOverflow = false;

				// if a value is written right after the timer overflows then timer isn't reset and the interupt flag register is unchanged
				if (memorySpace[Address::Timer] == 0) {

					memorySpace[Address::Timer] = memorySpace[Address::TimerModulo];
					BitSet(memorySpace[Address::InteruptFlag], 2);
				}
				writingToTIMA = true; // writing will fail when TMA is being written to TIMA
			}
		}

		oldBit = newBit;
	}

}

void Memory::DumpMemory(std::string fileName) {

	std::ofstream out;
	out.open(fileName + ".dmp", std::ios::binary | std::ios::out | std::ios::trunc);

	for (size_t i = 0; i <= 0xFFFF; i++) {

		byte value = Read(i);
		out << value;
	}
	out.close();
}

void Memory::DumpStack(word spAddress, std::string fileName) {

	char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	std::ofstream out;
	out.open(fileName + ".dmp", std::ios::out | std::ios::trunc);

	byte SPaddress1 = (spAddress & 0xF000) >> 4 * 3;
	byte SPaddress2 = (spAddress & 0x0F00) >> 4 * 2;
	byte SPaddress3 = (spAddress & 0x00F0) >> 4 * 1;
	byte SPaddress4 = (spAddress & 0x000F);

	out << "current: " << hex[SPaddress1] << hex[SPaddress2] << hex[SPaddress3] << hex[SPaddress4] << "\n";

	for (size_t i = (spAddress + 1) - 0xFF; i <= (spAddress + 1) + 0xFF; i += 2) {

		byte address1 = (i & 0xF000) >> 4 * 3;
		byte address2 = (i & 0x0F00) >> 4 * 2;
		byte address3 = (i & 0x00F0) >> 4 * 1;
		byte address4 = (i & 0x000F);

		byte value1 = (memorySpace[i] & 0xF0) >> 4;
		byte value2 = memorySpace[i] & 0x0F;
		byte value3 = (memorySpace[i + 1] & 0xF0) >> 4;
		byte value4 = memorySpace[i + 1] & 0x0F;

		out << hex[address1] << hex[address2] << hex[address3] << hex[address4] << ": ";
		out << hex[value1] << hex[value2] << hex[value3] << hex[value4] << "\n";
	}
	out.close();


}

void Memory::SaveExternalRam(std::string fileName) {

	std::string saveFile = fileName.substr(0, fileName.find_last_of('.')) + ".sav";

	std::ofstream saveFileStream(saveFile, std::ios::out | std::ios::binary | std::ios::trunc);

	for (size_t i = 0; i < ramBank.size(); i++) {
		for (size_t j = 0; j < RamBankSize; j++) {
			saveFileStream << ramBank[i][j];
		}
	}
	saveFileStream.close();

	externalRamChanged = false;
}

void Memory::LoadExternalRam(std::string fileName) {

	std::string saveFileName = fileName.substr(0, fileName.find_last_of('.')) + ".sav";

	if (fs::exists(saveFileName)) {

		int saveFileSize = fs::file_size(saveFileName);

		std::ifstream saveFileStream(saveFileName, std::ios::in | std::ios::binary);

		byte* SaveFileData = new byte[saveFileSize];
		memset(SaveFileData, 0, saveFileSize);

		saveFileStream.read((char*)SaveFileData, saveFileSize);
		saveFileStream.close();

		for (size_t i = 0; i < ramBank.size(); i++) {
			for (size_t j = 0; j < RamBankSize; j++) {
				ramBank[i][j] = SaveFileData[i * RamBankSize + j];
			}
		}

		delete SaveFileData;
	}
}



void Memory::HBlankDMA() {

	if (vramDMATransferProgress < vramDMATransferLength) {
		for (size_t i = 0; i < 0x10; i++) {
			Write(vramDMATransferDestination + vramDMATransferProgress, Read(vramDMATransferSource + vramDMATransferProgress));
			vramDMATransferProgress++;
		}
	}

	if (vramDMATransferProgress >= vramDMATransferLength) {
		vramDMATransferProgress = 0;
		vramDMATransferLength = 0;
	}
}