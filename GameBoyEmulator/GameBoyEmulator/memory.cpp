#include "memory.h"
#pragma warning(disable : 4996)



void Memory::WriteMemorySpace(word address, byte value) {
	memorySpace[address - Address::NonBankMemoryStart] = value;
}

byte Memory::ReadMemorySpace(word address) {
	return memorySpace[address - Address::NonBankMemoryStart];
}

Memory::Memory() {

	memorySpace = new byte[memorySize];
	memset(memorySpace, 0, memorySize);

	OAM = new byte[0xA0];
	memset(OAM, 0, 0xA0);
}

Memory::~Memory() {
	delete[] memorySpace;
	delete[] OAM;
	delete[] cartridge;
	delete[] BGColorPalette;
	delete[] SpriteColorPalette;

	for (size_t i = 0; i < ramBank.size(); i++) {
		byte* ptr = ramBank[i];
		delete[] ptr;
	}
	for (size_t i = 0; i < wramBank.size(); i++) {
		byte* ptr = wramBank[i];
		delete[] ptr;
	}
	for (size_t i = 0; i < vramBank.size(); i++) {
		byte* ptr = vramBank[i];
		delete[] ptr;
	}
}

void Memory::PowerUpSequence() {

	WriteMemorySpace(0xFF05, 0x00);
	WriteMemorySpace(0xFF05, 0x00);
	WriteMemorySpace(0xFF06, 0x00);
	WriteMemorySpace(0xFF07, 0x00);
	WriteMemorySpace(0xFF10, 0x80);
	WriteMemorySpace(0xFF11, 0xBF);
	WriteMemorySpace(0xFF12, 0xF3);
	WriteMemorySpace(0xFF14, 0xBF);
	WriteMemorySpace(0xFF16, 0x3F);
	WriteMemorySpace(0xFF17, 0x00);
	WriteMemorySpace(0xFF19, 0xBF);
	WriteMemorySpace(0xFF1A, 0x7F);
	WriteMemorySpace(0xFF1B, 0xFF);
	WriteMemorySpace(0xFF1C, 0x9F);
	WriteMemorySpace(0xFF1E, 0xBF);
	WriteMemorySpace(0xFF20, 0xFF);
	WriteMemorySpace(0xFF21, 0x00);
	WriteMemorySpace(0xFF22, 0x00);
	WriteMemorySpace(0xFF23, 0xBF);
	WriteMemorySpace(0xFF24, 0x77);
	WriteMemorySpace(0xFF25, 0xF3);
	WriteMemorySpace(0xFF26, 0xF1);
	WriteMemorySpace(0xFF40, 0x91);
	WriteMemorySpace(0xFF42, 0x00);
	WriteMemorySpace(0xFF43, 0x00);
	WriteMemorySpace(0xFF45, 0x00);
	WriteMemorySpace(0xFF47, 0xFC);
	WriteMemorySpace(0xFF48, 0xFF);
	WriteMemorySpace(0xFF49, 0xFF);
	WriteMemorySpace(0xFF4A, 0x00);
	WriteMemorySpace(0xFF4B, 0x00);
	WriteMemorySpace(0xFFFF, 0x00);

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
		memcpy(&memorySpace[0], ColorGameboyFFXXValues, 256);
	}
}

bool Memory::LoadRom(const std::string fileName) {

	std::ifstream romFile(fileName, std::ios::in, std::ios::binary);

	romFile.seekg(0, std::ios::end);
	int cartridgeLength = (int)romFile.tellg();
	romFile.seekg(0, std::ios::beg);

	cartridge = new byte[cartridgeLength];
	memset(cartridge, 0, cartridgeLength);

	romFile.read((char*)cartridge, cartridgeLength);
	romFile.close();

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


	int wramBankSize = 2;
	if (ColorGameBoyMode == true) {
		wramBankSize = 8;
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

	if (address >= Address::CartridgeStart && address <= Address::CartridgeBank0End) {
		return cartridge[address];
	}
	if (address >= Address::CartridgeBankNStart && address <= Address::CartridgeEnd) {

		word offset = address - Address::CartridgeBankNStart;
		int memoryBankOffset = kRomBankSize * currentRomBank;
		return cartridge[memoryBankOffset + offset];
	}
	else if (address >= Address::VramStart && address <= Address::VramEnd) {
		if (vRamBank == -1) {
			return vramBank[currentVramBank][address - Address::VramStart];
		}
		else {
			return vramBank[vRamBank][address - Address::VramStart];
		}
	}
	// Ram bank 0xA000 - 0xBFFF
	else if (address >= Address::ExternalRamStart && address <= Address::ExternalRamEnd) {
		return ramBank[currentRamBank][address - Address::ExternalRamStart];
	}
	else if (address >= Address::WorkRamBank0Start && address <= Address::WorkRamBank0End) {
		return wramBank[0][address - Address::WorkRamBank0Start];
	}
	else if (address >= Address::WorkRamBankNStart && address <= Address::WorkRamBankNEnd) {
		return wramBank[currentWramBank][address - Address::WorkRamBankNStart];
	}
	else if (address >= Address::OAMStart && address <= Address::OAMEnd) {
		return OAM[address - Address::OAMStart];
	}
	else if (address >= Address::NonBankMemoryStart) {
		if (address == Address::DIVRegister) {
			return (byte)((dividerRegister >> 8) & 0xFF);
		}
		else if (address == Address::VRAMDMA && ColorGameBoyMode == true) {
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
		else {
			return ReadMemorySpace(address);
		}
	}
}



void Memory::Write(word address, byte data) {

	// 0 - 7FFF cartrigde
	if (address >= Address::CartridgeStart && address <= Address::CartridgeEnd) {
		WriteMBCSwitch(address, data);
	}
	// 0x8000 - 0x9FFF 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)
	else if (address >= Address::VramStart && address <= Address::VramEnd) {
		vramBank.at(currentVramBank)[address - Address::VramStart] = data;
		TileChanged = true;
	}
	else if (address >= Address::ExternalRamStart && address <= Address::ExternalRamEnd) { // 8kb external ram
		externalRamChanged = true;
		if (ramBankEnabled == true && mbc == 1 || mbc == 3 || mbc == 5) {
			word newAddress = address - Address::ExternalRamStart;
			ramBank.at(currentRamBank)[newAddress] = data;
		}
		if (mbc == 2 && address < 0xA200) {
			word newAddress = address - Address::ExternalRamStart;
			ramBank.at(currentRamBank)[newAddress] = data;
		}
	}
	// 0xC000 - 0xCFFF 4KB Work RAM (WRAM) bank 0
	else if (address >= Address::WorkRamBank0Start && address <= Address::WorkRamBank0End) {
		wramBank.at(0)[address - Address::WorkRamBank0Start] = data;
	}
	// 0xD000 - 0xDFFF 4KB Work RAM (WRAM) bank 1~N (1 for gameboy, 1-7 for gameboy color)
	else if (address >= Address::WorkRamBankNStart && address <= Address::WorkRamBankNEnd) {
		wramBank.at(currentWramBank)[address - Address::WorkRamBankNStart] = data;
	}
	// 0xE000 - 0xFDFF Mirror of C000~DDFF (ECHO RAM) typically not used
	// 0xFE00 - 0xFE9F Sprite attribute table
	else if (address >= Address::OAMStart && address <= Address::OAMEnd) {
		OAM[address - Address::OAMStart] = data;
	}
	else if (address >= Address::NonBankMemoryStart) {

		if (address == Address::SIOControl) { // serial control
			WriteMemorySpace(address, data);
		}
		else if (address == Address::DIVRegister) { // DIV register
			dividerRegister = 0;
		}
		else if (address == Address::Timer) { // TIMA 
			if (writingToTIMA == false) { // writing will fail when TMA is being written to TIMA
				WriteMemorySpace(address, data);
			}
		}
		else if (address == Address::TimerModulo) { // TMA / Timer Module
			WriteMemorySpace(address, data);
			if (writingToTIMA == true) { // writing to TMA in the same cycle as TMA is being loaded into TIMA causes the same value to be written to TIMA
				WriteMemorySpace(Address::Timer, data);
			}
		}
		else if (address == Address::Channel1FrequencyHigh) {
			WriteMemorySpace(address, data);
			resetSC1Length = true;
		}
		else if (address == Address::Channel2FrequencyHigh) {
			WriteMemorySpace(address, data);
			resetSC2Length = true;
		}
		else if (address == Address::Channel3FrequencyHigh) {
			WriteMemorySpace(address, data);
			resetSC3Length = true;
		}
		else if (address == Address::Channel4Counter) {
			WriteMemorySpace(address, data);
			resetSC4Length = true;
		}
		else if (address == Address::SoundOnOFF) {
			if (BitTest(data, 7) == true) {
				byte val = ReadMemorySpace(address);
				BitSet(val, 7);
				WriteMemorySpace(address, val);
			}
			else {
				byte val = ReadMemorySpace(address);
				BitReset(val, 7);
				WriteMemorySpace(address, val);
			}
		}
		else if (address == Address::LY) {
			WriteMemorySpace(address, 0);
		}
		else if (address == Address::DMA) { // DMA

			word DMAAddress = data;
			DMAAddress <<= 8;

			for (word i = 0; i < 0xA0; i++) {
				OAM[i] = Read(DMAAddress + i);
			}
		}
		else if (address == Address::PrepareSpeedSwitch) {
			WriteMemorySpace(address, data | 0x7E);
		}
		else if (address == Address::VRAMBank) {
			if (ColorGameBoyMode == true) {
				currentVramBank = data & Bits::b0;
				WriteMemorySpace(address, (~Bits::b0) | (data & Bits::b0));
			}
			else {
				WriteMemorySpace(address, data);
			}
		}
		else if (address == Address::VRAMDMA) {
			WriteMemorySpace(address, data);
			if (ColorGameBoyMode == true) {

				if (vramDMATransferLength != 0 && (data & Bits::b7) == 0) {
					vramDMATransferLength = 0;
					vramDMATransferProgress = 0;
					return;
				}

				vramDMATransferSource = (ReadMemorySpace(0xFF51) << 8) | (ReadMemorySpace(0xFF52) & 0xF0);
				vramDMATransferDestination = (ReadMemorySpace(0xFF53) << 8) | (ReadMemorySpace(0xFF54) & 0xF0);
				vramDMATransferLength = data & 0x7F;
				vramDMATransferLength++;
				vramDMATransferLength *= 0x10;

				if ((data & Bits::b7) == 0) {
					for (word i = 0; i < vramDMATransferLength; i++) {
						Write(vramDMATransferDestination + i, Read(vramDMATransferSource + i));
					}
					vramDMATransferLength = 0;
				}
				else {
					vramDMATransferProgress = 0;
				}
			}
		}
		else if (address == Address::GBColorBackgroundPaletteData) {
			if (ColorGameBoyMode == true) {

				byte index = ReadMemorySpace(Address::GBColorBackgroundPaletteIndex) & 0x3F;
				BGColorPalette[index] = data;

				if ((ReadMemorySpace(Address::GBColorBackgroundPaletteIndex) & Bits::b7) != 0) {
					WriteMemorySpace(Address::GBColorBackgroundPaletteIndex, (ReadMemorySpace(Address::GBColorBackgroundPaletteIndex) + 1) & 0xBF);
				}
			}
			else {
				WriteMemorySpace(address, data);
			}
		}
		else if (address == Address::GBColorSpritePaletteData) {
			if (ColorGameBoyMode == true) {

				byte index = ReadMemorySpace(Address::GBColorSpritePaletteIndex) & 0x3F;
				SpriteColorPalette[index] = data;

				if ((ReadMemorySpace(Address::GBColorSpritePaletteIndex) & Bits::b7) != 0) {
					WriteMemorySpace(Address::GBColorSpritePaletteIndex, (ReadMemorySpace(Address::GBColorSpritePaletteIndex) + 1) & 0xBF);
				}
			}
			else {
				WriteMemorySpace(address, data);
			}
		}
		else if (address == Address::WramBank) {
			if (ColorGameBoyMode == true) {
				currentWramBank = (data & 0x7);
				if (currentWramBank == 0) { 
					currentWramBank = 1;
				}
				WriteMemorySpace(address, data);
			}
			else {
				WriteMemorySpace(address, data);
			}
		}
		else {
			WriteMemorySpace(address, data); // no special behaviour
		}
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
	
	byte selectedKeys = ReadMemorySpace(Address::Joypad);

	selectedKeys = ~selectedKeys;
	selectedKeys &= 0x30;

	byte buttonsPressed = 0xFF;

	if ((selectedKeys & 0x20) != 0) { // button keys

		if (buttons.start == true) buttonsPressed &= ~(1 << 3);
		if (buttons.select == true) buttonsPressed &= ~(1 << 2);
		if (buttons.buttonB == true) buttonsPressed &= ~(1 << 1);
		if (buttons.buttonA == true) buttonsPressed &= ~(1 << 0);

		buttonsPressed &= ~(1 << 5);
	}
	else if ((selectedKeys & 0x10) != 0) { // direction keys

		if (buttons.down == true) buttonsPressed &= ~(1 << 3);
		if (buttons.up == true) buttonsPressed &= ~(1 << 2);
		if (buttons.left == true) buttonsPressed &= ~(1 << 1);
		if (buttons.right == true) buttonsPressed &= ~(1 << 0);

		buttonsPressed &= ~(1 << 4);
	}

	return buttonsPressed;
}



// below replicates the exact hardware of the divider register and timer counter 
// as described in section 5 of The Cycle-Accurate Game Boy Docs by Antonio Nino Diaz
void Memory::IncrementDivAndTimerRegisters(byte clocks) {

	for (size_t i = 0; i < clocks; i++) {

		if (writingToTIMA == true) {
			writingToTIMACounter++;
			if (writingToTIMACounter > kClocksPerCycle) { // it takes one cycle (4 clocks) to write TMA to TIMA
				writingToTIMA = false;
			}
		}

		dividerRegister++;

		bool newBit = false;
		switch (ReadMemorySpace(Address::TimerControl) & 0x3) {
		case 0: newBit = BitTest(dividerRegister, 9); break;
		case 1: newBit = BitTest(dividerRegister, 3); break;
		case 2: newBit = BitTest(dividerRegister, 5); break;
		case 3: newBit = BitTest(dividerRegister, 7); break;
		}

		newBit &= BitTest(ReadMemorySpace(Address::TimerControl), 2);

		if ((!newBit) & oldBit) {
			if (ReadMemorySpace(Address::Timer) == 0xFF) { // about to overflow
				timerOverflow = true;
			}
			WriteMemorySpace(Address::Timer, ReadMemorySpace(Address::Timer) + 1);
		}

		if (timerOverflow == true) {
			timerOverflowCounter++;
			if (timerOverflowCounter > kClocksPerCycle) { // there is a one cycle (4 clocks) delay to detect if the timer overflows

				timerOverflow = false;

				// if a value is written right after the timer overflows then timer isn't reset and the interupt flag register is unchanged
				if (ReadMemorySpace(Address::Timer) == 0) {

					WriteMemorySpace(Address::Timer, ReadMemorySpace(Address::TimerModulo));
					byte flags = ReadMemorySpace(Address::InteruptFlag);
					BitSet(flags, 2);
					WriteMemorySpace(Address::InteruptFlag, flags);
				}
				writingToTIMA = true; // writing will fail when TMA is being written to TIMA
			}
		}

		oldBit = newBit;
	}

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

		int saveFileSize = (int)fs::file_size(saveFileName);

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