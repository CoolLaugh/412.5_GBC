#include "memory.h"



Memory::Memory() {

	m_GameBank = new byte[0x200000];
	m_rom = new byte[0x10000];

}

void Memory::PowerUpSequence() {

	m_rom[0xFF05] = 0x00;
	m_rom[0xFF06] = 0x00;
	m_rom[0xFF07] = 0x00;
	m_rom[0xFF10] = 0x80;
	m_rom[0xFF11] = 0xBF;
	m_rom[0xFF12] = 0xF3;
	m_rom[0xFF14] = 0xBF;
	m_rom[0xFF16] = 0x3F;
	m_rom[0xFF17] = 0x00;
	m_rom[0xFF19] = 0xBF;
	m_rom[0xFF1A] = 0x7F;
	m_rom[0xFF1B] = 0xFF;
	m_rom[0xFF1C] = 0x9F;
	m_rom[0xFF1E] = 0xBF;
	m_rom[0xFF20] = 0xFF;
	m_rom[0xFF21] = 0x00;
	m_rom[0xFF22] = 0x00;
	m_rom[0xFF23] = 0xBF;
	m_rom[0xFF24] = 0x77;
	m_rom[0xFF25] = 0xF3;
	m_rom[0xFF26] = 0xF1;
	m_rom[0xFF40] = 0x91;
	m_rom[0xFF42] = 0x00;
	m_rom[0xFF43] = 0x00;
	m_rom[0xFF45] = 0x00;
	m_rom[0xFF47] = 0xFC;
	m_rom[0xFF48] = 0xFF;
	m_rom[0xFF49] = 0xFF;
	m_rom[0xFF4A] = 0x00;
	m_rom[0xFF4B] = 0x00;
	m_rom[0xFFFF] = 0x00;
}

bool Memory::LoadRom(const std::string fileName) {

	memset(m_rom, 0, sizeof(m_rom));
	memset(m_GameBank, 0, sizeof(m_GameBank));

	std::ifstream romFile(fileName, std::ios::in, std::ios::binary);
	romFile.read((char*)m_GameBank, 0x200000);
	romFile.close();

	memcpy(m_rom, m_GameBank, 0x8000);

	CreateRamBanks();

	return true;
}

void Memory::CreateRamBanks() {

	// note: use after rom is loaded
	// 2.5.4
	byte cartridgeType = Read(0x147);

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
		case 0x12: mbc = 3; break;
		case 0x13: mbc = 3; break;
		case 0x19: mbc = 5; break;
		case 0x1A: mbc = 5; break;
		case 0x1B: mbc = 5; break;
		case 0x1C: mbc = 5; break;
		case 0x1D: mbc = 5; break;
		case 0x1E: mbc = 5; break;
	}

	byte ramSize = Read(0x149);

	switch (ramSize) {
	case 0: numberOfRamBanks = 0; break;
	case 1: numberOfRamBanks = 1; break;
	case 2: numberOfRamBanks = 1; break;
	case 3: numberOfRamBanks = 4; break;
	case 4: numberOfRamBanks = 16; break;
	}

	for (size_t i = 0; i < numberOfRamBanks; i++) {
		byte* bank = new byte[0x2000];
		memset(bank, 0, sizeof(bank));
		ramBank.push_back(bank);
	}

	// copy 0xA000-0xC000?
}

byte Memory::Read(word address) {

	// rom bank 0x4000 - 0x7FFF
	if (address >= 0x4000 && address <= 0x7FFF) {

	}
	// Ram bank 0xA000 - 0xBFFF
	else if (address >= 0xA000 && address <= 0xBFFF) {

	}
	// controls 0xFF00
	else if (address == 0xFF00) {

	}

	return m_rom[address];
}

void Memory::Write(word address, byte data) {

	// 0 - 7FFF cartrigde
	// 8000 - 7FFF

	if (address <= 0x3FFF) { // 16kb cartridge fixed bank

		if (address >= 0x0000 && address <= 0x1FFF) {
			if (mbc == 1 || mbc == 3 || mbc == 5) {

				data &= 0xF;

				if (data == 0xA) {
					ramBankEnabled = true;
				}
				else {
					ramBankEnabled = false;
				}
			}
			else if (mbc == 2) {
				word addressCheck = address & 0x100;
				if (addressCheck == 0) {

					data &= 0xF;
					if (data == 0xA) {
						ramBankEnabled = true;
					}
					else {
						ramBankEnabled = false;
					}
				}
			}
		}

		// ref 2.6
		if (address >= 0x2000 && address <= 0x3FFF) {

			if (mbc == 1) {

				data &= 31;

				if (data == 0) { // values of 0 and 1 do the same thing
					data = 1;
				}

				// top 3 bits must be preserved
				currentRomBank &= 224;
				currentRomBank |= data;

			}
			else if (mbc == 2) {

				data &= 15;
				if (data == 0) { // values of 0 and 1 do the same thing
					data = 1;
				}

				currentRomBank = data;
			}
			else if (mbc == 3) {

				data &= 127;
				currentRomBank = data;
			}
			else if (mbc == 5) {

				if (address >= 0x2000 && address <= 0x2FFF) {

					currentRomBank &= 0x10000;
					currentRomBank |= data;
				}
				else if (address >= 0x3000 && address <= 0x3FFF) {

					data &= 1;
					data <<= 8;
					currentRomBank |= data;
				}

			}
		}
	}
	else if (address >= 0x4000 && address <= 0x7FFF) { // 16kb switchable bank via MBC

		// ref 2.6
		if (address >= 0x4000 && address <= 0x5FFF && mbc == 1) {
			if (mbc == 1 || mbc == 3) {
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
			if (mbc == 5) {

				data &= 15;
				currentRamBank = data;
			}
		}

		// ref 2.6
		if (address >= 0x6000 && address <= 0x7FFF) {
			if (mbc = 1 || mbc == 3 || mbc == 5) {
				data &= 1;

				if (data == 0) {
					memoryModel = mm16_8;
				}
				else if (data = 1) {
					memoryModel = mm4_32;
				}

			}
		}

	}
	else if (address >= 0x8000 && address <= 0x9FFF) { // 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)

	}
	else if (address >= 0xA000 && address <= 0xBFFF) { // 8kb external ram
		if (ramBankEnabled == true && mbc == 1 || mbc == 3 || mbc == 5) {
			//word newAddress = address - 0xA000;
			//ramBank.at(currentRamBank)[newAddress] = data;
		}
		if (mbc == 2 && address < 0xA200) {

		}
	}
	else if (address >= 0xC000 && address <= 0xCFFF) { // 4KB Work RAM (WRAM) bank 0

	}
	else if (address >= 0xD000 && address <= 0xDFFF) { // 4KB Work RAM (WRAM) bank 1~N (1 for gameboy, 1-7 for gameboy color)

	}
	else if (address >= 0xE000 && address <= 0xFDFF) { // typically not used

	}
	else if (address >= 0xFE00 && address <= 0xFE9F) { // Sprite attribute table

	}
	else if (address >= 0xFEA0 && address <= 0xFEFF) { // Not Usable

	}
	else if (address >= 0xFF00 && address <= 0xFF7F) { // I/O Registers 

	}
	else if (address >= 0xFF80 && address <= 0xFFFE) { // High RAM (HRAM) 

	}
	else if (address == 0xFFFF) { // Interrupts Enable Register (IE)

	}
}
