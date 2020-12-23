#include "cpu.h"

void Cpu::PowerUpSequence() {

	// 2.7.1
	registers.a = 0x01;
	registers.f = 0xB0;
	registers.b = 0x00;
	registers.c = 0x13;
	registers.d = 0x00;
	registers.e = 0xD8;
	registers.h = 0x01;
	registers.l = 0x4D;
	registers.sp = 0xFFFE;
	registers.pc = 0x100; // skips the nintendo logo check

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

void Cpu::CreateRamBanks() {

	// note: use after rom is loaded
	// 2.5.4
	unsigned char cartridgeType = MemoryRead(0x147);
	
	switch (cartridgeType) {
		case 0: mbc = 0; break;
		case 1: mbc = 1; break;
		case 2: mbc = 1; break;
		case 3: mbc = 1; break;
		case 5: mbc = 2; break;
		case 6: mbc = 2; break;
		case 8: mbc = 0; break;
		case 9: mbc = 0; break;
	}

	unsigned char ramSize = MemoryRead(0x149);

	switch (ramSize) {
	case 0: numberOfRamBanks = 0; break;
	case 1: numberOfRamBanks = 1; break;
	case 2: numberOfRamBanks = 1; break;
	case 3: numberOfRamBanks = 4; break;
	case 4: numberOfRamBanks = 16; break;
	}

	for (size_t i = 0; i < numberOfRamBanks; i++) {
		unsigned char* bank = new unsigned char[0x2000];
		memset(bank, 0, sizeof(bank));
		ramBank.push_back(bank);
	}

	// copy 0xA000-0xC000?
}

short Cpu::ExecuteOpcode() {

	short cycles = 0;

	unsigned char opcode = m_rom[registers.pc];
	registers.pc++;

	// read memory instead of rom if pc is 0x4000-0x7FFF or 0xA000 - 0xBFFF

	switch (opcode) {

		case 0x00: cycles = 4;  break;

		// 8 bit loads (3.3.1)
		// load value into register
		case 0x06: cycles = LD(registers.b); break;
		case 0x0E: cycles = LD(registers.c); break;
		case 0x16: cycles = LD(registers.b); break;
		case 0x1E: cycles = LD(registers.c); break;
		case 0x26: cycles = LD(registers.b); break;
		case 0x2E: cycles = LD(registers.c); break;

		// load register into another register
		case 0x7F: cycles = LDreg(registers.a, registers.a); break;
		case 0x78: cycles = LDreg(registers.a, registers.b); break;
		case 0x79: cycles = LDreg(registers.a, registers.c); break;
		case 0x7A: cycles = LDreg(registers.a, registers.d); break;
		case 0x7B: cycles = LDreg(registers.a, registers.e); break;
		case 0x7C: cycles = LDreg(registers.a, registers.h); break;
		case 0x7D: cycles = LDreg(registers.a, registers.l); break;
		case 0x7E: cycles = LDregHL(registers.a); break;

		case 0x40: cycles = LDreg(registers.b, registers.b); break;
		case 0x41: cycles = LDreg(registers.b, registers.c); break;
		case 0x42: cycles = LDreg(registers.b, registers.d); break;
		case 0x43: cycles = LDreg(registers.b, registers.e); break;
		case 0x44: cycles = LDreg(registers.b, registers.h); break;
		case 0x45: cycles = LDreg(registers.b, registers.l); break;
		case 0x46: cycles = LDregHL(registers.b); break;

		case 0x48: cycles = LDreg(registers.c, registers.b); break;
		case 0x49: cycles = LDreg(registers.c, registers.c); break;
		case 0x4A: cycles = LDreg(registers.c, registers.d); break;
		case 0x4B: cycles = LDreg(registers.c, registers.e); break;
		case 0x4C: cycles = LDreg(registers.c, registers.h); break;
		case 0x4D: cycles = LDreg(registers.c, registers.l); break;
		case 0x4E: cycles = LDregHL(registers.c); break;

		case 0x50: cycles = LDreg(registers.d, registers.b); break;
		case 0x51: cycles = LDreg(registers.d, registers.c); break;
		case 0x52: cycles = LDreg(registers.d, registers.d); break;
		case 0x53: cycles = LDreg(registers.d, registers.e); break;
		case 0x54: cycles = LDreg(registers.d, registers.h); break;
		case 0x55: cycles = LDreg(registers.d, registers.l); break;
		case 0x56: cycles = LDregHL(registers.d); break;

		case 0x58: cycles = LDreg(registers.e, registers.b); break;
		case 0x59: cycles = LDreg(registers.e, registers.c); break;
		case 0x5A: cycles = LDreg(registers.e, registers.d); break;
		case 0x5B: cycles = LDreg(registers.e, registers.e); break;
		case 0x5C: cycles = LDreg(registers.e, registers.h); break;
		case 0x5D: cycles = LDreg(registers.e, registers.l); break;
		case 0x5E: cycles = LDregHL(registers.e); break;

		case 0x60: cycles = LDreg(registers.h, registers.b); break;
		case 0x61: cycles = LDreg(registers.h, registers.c); break;
		case 0x62: cycles = LDreg(registers.h, registers.d); break;
		case 0x63: cycles = LDreg(registers.h, registers.e); break;
		case 0x64: cycles = LDreg(registers.h, registers.h); break;
		case 0x65: cycles = LDreg(registers.h, registers.l); break;
		case 0x66: cycles = LDregHL(registers.h); break;

		case 0x68: cycles = LDreg(registers.l, registers.b); break;
		case 0x69: cycles = LDreg(registers.l, registers.c); break;
		case 0x6A: cycles = LDreg(registers.l, registers.d); break;
		case 0x6B: cycles = LDreg(registers.l, registers.e); break;
		case 0x6C: cycles = LDreg(registers.l, registers.h); break;
		case 0x6D: cycles = LDreg(registers.l, registers.l); break;
		case 0x6E: cycles = LDregHL(registers.l); break;


		//case 0x70: break;
		//case 0x71: break;
		//case 0x72: break;
		//case 0x73: break;
		//case 0x74: break;
		//case 0x75: break;
		//case 0x36: break;






	default:
		break;
	}

	return cycles;
}

short Cpu::LD(unsigned char & reg) {

	reg = MemoryRead(registers.pc);
	registers.pc++;
	return 8; // cycles
}

short Cpu::LDreg(unsigned char & reg1, unsigned char & reg2) {

	reg1 = reg2;

	return 4; // cycles
}

short Cpu::LDregHL(unsigned char & reg) {

	short address = CombineReg(registers.h, registers.l);
	reg = MemoryRead(address);

	return 8;
}

short Cpu::LDmemfromreg(unsigned char & reg) {



	return 0;
}

short Cpu::LDmem() {
	return 0;
}

unsigned char Cpu::MemoryRead(short address) {

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

void Cpu::MemoryWrite(short address, unsigned char data) {

	// 0 - 7FFF cartrigde
	// 8000 - 7FFF

	if (address <= 0x3FFF) { // 16kb cartridge fixed bank

		if (address >= 0x0000 && address <= 0x1FFF) {
			if (mbc == 1) {

				data &= 0xF;

				if (data == 0xA) {
					ramBankEnabled = true;
				}
				else {
					ramBankEnabled = false;
				}
			}
			else if (mbc == 2) {
				short addressCheck = address & 0x100;
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
			if (mbc == 1) {
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
			if (mbc = 1) {
				data &= 1;

				if (data == 0) {
					memoryModel = mm16_8;
				}
				else if(data = 1) {
					memoryModel = mm4_32;
				}

			}
		}

	}
	else if (address >= 0x8000 && address <= 0x9FFF) { // 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)

	}
	else if (address >= 0xA000 && address <= 0xBFFF) { // 8kb external ram
		if (ramBankEnabled == true && mbc == 1) {
			//unsigned short newAddress = address - 0xA000;
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

unsigned short Cpu::CombineReg(unsigned char & reg1, unsigned char & reg2) {

	short combinedReg = reg1;
	combinedReg <<= 8;
	combinedReg |= reg2;
	return combinedReg;
}
