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

	// memory values not set by skipping the nintendo logo
	//rom[0xFF44] = 0x3E;

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

	CreateRamBanks();

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
	case 0x12: mbc = 3; break;
	case 0x13: mbc = 3; break;
	case 0x19: mbc = 5; break;
	case 0x1A: mbc = 5; break;
	case 0x1B: mbc = 5; break;
	case 0x1C: mbc = 5; break;
	case 0x1D: mbc = 5; break;
	case 0x1E: mbc = 5; break;
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

	for (size_t i = 0; i < numberOfRamBanks; i++) {
		byte* bank = new byte[0x2000];
		memset(bank, 0, sizeof(bank));
		ramBank.push_back(bank);
	}
}

byte Memory::Read(word address) {

	// rom bank 0x4000 - 0x7FFF
	if (address >= 0x4000 && address <= 0x7FFF) {

		word offset = address - 0x4000;
		word memoryBankOffset = 0x4000 * currentRomBank;
		return cartridge[memoryBankOffset + offset];
	}
	// Ram bank 0xA000 - 0xBFFF
	else if (address >= 0xA000 && address <= 0xBFFF) {
		return ramBank[currentRamBank][address - 0xA000];
	}
	// controls 0xFF00
	else if (address == Address::Joypad) {
		return GetJoypadState();
	}

	return memorySpace[address];
}

void Memory::WriteOld(word address, byte data) {

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
				else if (data == 1) {
					memoryModel = mm4_32;
				}

			}
		}

	}
	else if (address >= 0x8000 && address <= 0x9FFF) { // 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)
		memorySpace[address] = data;
	}
	else if (address >= 0xA000 && address <= 0xBFFF) { // 8kb external ram
		if (ramBankEnabled == true && mbc == 1 || mbc == 3 || mbc == 5) {
			word newAddress = address - 0xA000;
			ramBank.at(currentRamBank)[newAddress] = data;
		}
		if (mbc == 2 && address < 0xA200) {
			word newAddress = address - 0xA000;
			ramBank.at(currentRamBank)[newAddress] = data;
		}
	}
	else if (address >= 0xC000 && address <= 0xCFFF) { // 4KB Work RAM (WRAM) bank 0
		memorySpace[address] = data;
	}
	else if (address >= 0xD000 && address <= 0xDFFF) { // 4KB Work RAM (WRAM) bank 1~N (1 for gameboy, 1-7 for gameboy color)
		memorySpace[address] = data;
	}
	else if (address >= 0xE000 && address <= 0xFDFF) { // typically not used
		memorySpace[address] = data;
	}
	else if (address >= 0xFE00 && address <= 0xFE9F) { // Sprite attribute table
	memorySpace[address] = data;
	}
	else if (address >= 0xFEA0 && address <= 0xFEFF) { // Not Usable
	memorySpace[address] = data;
	}
	else if (address >= 0xFF00 && address <= 0xFF7F) { // I/O Registers 

		if (address == 0xFF00) { // joypad
			memorySpace[address] = data;
		}
		else if (address == 0xFF01) { // serial data
			memorySpace[address] = data;
		}
		else if (address == 0xFF02) { // serial control
			memorySpace[address] = data;
			if (data == 0x81) { // output for test rom
				std::cout << memorySpace[0xFF01];
				std::ofstream resultsFile;
				resultsFile.open("results.txt", std::ios::app);
				byte out = memorySpace[0xFF01];
				char toHex[16] = { '0', '1', '2' , '3' , '4' , '5' , '6' , '7' , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
				resultsFile << "0x" << toHex[(out >> 4)] << toHex[(out & 0xF)] << "\n";
				resultsFile.close();
			}
		}
		else if (address == 0xFF04) { // DIV register
			memorySpace[address] = 0;
		}
		else if(address == 0xFF07) { // timer control
			memorySpace[address] = data;
		}
		else if (address == 0xFF0F) { // interupt flag
			memorySpace[address] = data;
		}
		else if (address >= 0xFF10 && address <= 0xFF3F) { // sound
			memorySpace[address] = data;
		}
		else if (address == 0xFF40) {
			memorySpace[address] = data;
		}
		else if (address == 0xFF41) {
			memorySpace[address] = data;
		}
		else if (address == 0xFF42) {
			memorySpace[address] = data;
		}
		else if (address == 0xFF43) {
			memorySpace[address] = data;
		}
		else if (address == Address::LY) {
			memorySpace[address] = 0;
		}
		else if (address == 0xFF45) {
			memorySpace[address] = data;
		}
		else if (address == 0xFF46) { // DMA

			word DMAAddress = data;
			DMAAddress <<= 8;

			for (word i = 0; i < 0xA0; i++) {
				memorySpace[0xFE00 + i] = Read(DMAAddress + i);
			}
		}
		else if (address == 0xFF47) {
			memorySpace[address] = data;
		}
		else if (address == 0xFF48) {
			memorySpace[address] = data;
		}
		else if (address == 0xFF49) {
			memorySpace[address] = data;
		}
	}
	else if (address >= 0xFF80 && address <= 0xFFFE) { // High RAM (HRAM) 
		if (address == 0xFF85) {
			int a = 1;
		}
		memorySpace[address] = data;
	}
	else if (address == 0xFFFF) { // Interrupts Enable Register (IE)
		memorySpace[address] = data;
	}
}


void Memory::Write(word address, byte data) {

	// 0 - 7FFF cartrigde
	// 8000 - 7FFF

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
	else if (address >= 0x2000 && address <= 0x3FFF) {

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
	else if (address >= 0x4000 && address <= 0x5FFF && mbc == 1) {
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
	else if (address >= 0x6000 && address <= 0x7FFF) {
		if (mbc = 1 || mbc == 3 || mbc == 5) {
			data &= 1;

			if (data == 0) {
				memoryModel = mm16_8;
			}
			else if (data == 1) {
				memoryModel = mm4_32;
			}

		}
	}
	// 0x8000 - 0x9FFF 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)
	else if (address >= 0xA000 && address <= 0xBFFF) { // 8kb external ram
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
	// 0xD000 - 0xDFFF 4KB Work RAM (WRAM) bank 1~N (1 for gameboy, 1-7 for gameboy color)
	// 0xE000 - 0xFDFF Mirror of C000~DDFF (ECHO RAM) typically not used
	// 0xFE00 - 0xFE9F Sprite attribute table
	// 0xFEA0 - 0xFEFF Not Usable
	else if (address == 0xFF02) { // serial control
		memorySpace[address] = data;
		if (data == 0x81) { // output for test rom
			std::cout << memorySpace[0xFF01];
			std::ofstream resultsFile;
			resultsFile.open("results.txt", std::ios::app);
			byte out = memorySpace[0xFF01];
			char toHex[16] = { '0', '1', '2' , '3' , '4' , '5' , '6' , '7' , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
			resultsFile << "0x" << toHex[(out >> 4)] << toHex[(out & 0xF)] << "\n";
			resultsFile.close();
		}
	}
	else if (address == 0xFF04) { // DIV register
		memorySpace[address] = 0;
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
	else {
		memorySpace[address] = data; // no special behaviour
	}

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


void Memory::DumpMemory(std::string fileName) {

	std::ofstream out;
	out.open(fileName + ".dmp", std::ios::binary | std::ios::out | std::ios::trunc);

	for (size_t i = 0; i <= 0xFFFF; i++) {

		byte value = memorySpace[i];
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
