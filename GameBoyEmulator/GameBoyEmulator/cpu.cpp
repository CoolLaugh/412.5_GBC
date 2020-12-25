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

		case 0x70: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.b); break;
		case 0x71: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.b); break;
		case 0x72: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.b); break;
		case 0x73: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.b); break;
		case 0x74: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.b); break;
		case 0x75: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.b); break;
		case 0x36: cycles = 12; MemoryWrite(Combinebytes(registers.h, registers.l), MemoryRead(registers.pc));  registers.pc++; break;

		case 0x0A: cycles = LDRegFromMemory(registers.a, Combinebytes(registers.b, registers.c)); break;
		case 0x1A: cycles = LDRegFromMemory(registers.a, Combinebytes(registers.d, registers.e)); break;
		case 0xFA: cycles = LDRegFromMemory(registers.a); break;
		case 0x3E: cycles = 8; registers.a = MemoryRead(registers.pc); registers.pc++; break;

		case 0x47: cycles = LDreg(registers.b, registers.a); break;
		case 0x4F: cycles = LDreg(registers.c, registers.a); break;
		case 0x57: cycles = LDreg(registers.d, registers.a); break;
		case 0x5F: cycles = LDreg(registers.e, registers.a); break;
		case 0x67: cycles = LDreg(registers.h, registers.a); break;
		case 0x6F: cycles = LDreg(registers.l, registers.a); break;

		case 0x02: cycles = 8; MemoryWrite(Combinebytes(registers.b, registers.c), registers.a); break;
		case 0x12: cycles = 8; MemoryWrite(Combinebytes(registers.b, registers.c), registers.a); break;
		case 0x77: cycles = 8; MemoryWrite(Combinebytes(registers.b, registers.c), registers.a); break;
		case 0xEA: cycles = 16; MemoryWrite(Combinebytes(MemoryRead(registers.pc), MemoryRead(registers.pc + 1)), registers.a); registers.pc += 2; break;

		case 0xF2: cycles = 8; registers.a = MemoryRead(0xFF00 + registers.c); break;

		case 0xE2: cycles = 8; MemoryWrite(0xFF00 + registers.c, registers.a); break;

		case 0x3A: cycles = 8; registers.a = MemoryRead(Combinebytes(registers.h, registers.l)); decrement16reg(registers.h, registers.l); break;

		case 0x32: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.a); decrement16reg(registers.h, registers.l); break;

		case 0x2A: cycles = 8; registers.a = MemoryRead(Combinebytes(registers.h, registers.l)); increment16reg(registers.h, registers.l); break;

		case 0x22: cycles = 8; MemoryWrite(Combinebytes(registers.h, registers.l), registers.a); increment16reg(registers.h, registers.l); break;

		case 0xE0: cycles = 12; MemoryWrite(0xFF00 + MemoryRead(registers.pc), registers.a); registers.pc++; break;

		case 0xF0: cycles = 12; registers.a = MemoryRead(0xFF00 + MemoryRead(registers.pc)); break;

		case 0x01: cycles = LD16(registers.b, registers.c); break;
		case 0x11: cycles = LD16(registers.b, registers.c); break;
		case 0x21: cycles = LD16(registers.b, registers.c); break;
		case 0x31: cycles = LD16(registers.b, registers.c); break;

		case 0xF9: cycles = 8; registers.pc = Combinebytes(registers.h, registers.l); break;

		case 0xF8: cycles = LDHL(); break;

		case 0x08: cycles = 20; MemoryWrite(MemoryRead(registers.pc), registers.sp); registers.pc++; break;

		// push register pairs onto the stack
		case 0xF5: cycles = Push(registers.a, registers.f); break;
		case 0xC5: cycles = Push(registers.b, registers.c); break;
		case 0xD5: cycles = Push(registers.d, registers.e); break;
		case 0xE5: cycles = Push(registers.h, registers.l); break;

		// pop register pairs off of the stack
		case 0xF1: cycles = Pop(registers.a, registers.f); break;
		case 0xC1: cycles = Pop(registers.b, registers.c); break;
		case 0xD1: cycles = Pop(registers.d, registers.e); break;
		case 0xE1: cycles = Pop(registers.h, registers.l); break;

		// add
		case 0x87: cycles = ADD(registers.a); break;
		case 0x80: cycles = ADD(registers.b); break;
		case 0x81: cycles = ADD(registers.c); break;
		case 0x82: cycles = ADD(registers.d); break;
		case 0x83: cycles = ADD(registers.e); break;
		case 0x84: cycles = ADD(registers.h); break;
		case 0x85: cycles = ADD(registers.l); break;
		case 0x86: cycles = 8; ADD(MemoryRead(Combinebytes(registers.h, registers.l))); break;
		case 0xC6: cycles = 8; ADD(MemoryRead(registers.pc)); registers.pc++; break;

		// add with carry
		case 0x8F: cycles = ADDC(registers.a); break;
		case 0x88: cycles = ADDC(registers.b); break;
		case 0x89: cycles = ADDC(registers.c); break;
		case 0x8A: cycles = ADDC(registers.d); break;
		case 0x8B: cycles = ADDC(registers.e); break;
		case 0x8C: cycles = ADDC(registers.h); break;
		case 0x8D: cycles = ADDC(registers.l); break;
		case 0x8E: cycles = 8; ADDC(MemoryRead(Combinebytes(registers.h, registers.l))); break;
		case 0xCE: cycles = 8; ADDC(MemoryRead(registers.pc)); registers.pc++; break;

		// sub
		case 0x97: cycles = SUB(registers.a); break;
		case 0x90: cycles = SUB(registers.b); break;
		case 0x91: cycles = SUB(registers.c); break;
		case 0x92: cycles = SUB(registers.d); break;
		case 0x93: cycles = SUB(registers.e); break;
		case 0x94: cycles = SUB(registers.h); break;
		case 0x95: cycles = SUB(registers.l); break;
		case 0x96: cycles = 8; SUB(MemoryRead(Combinebytes(registers.h, registers.l))); break;
		case 0xD6: cycles = 8; SUB(MemoryRead(registers.pc)); registers.pc++; break;

		// sub with carry
		case 0x9F: cycles = SUBC(registers.a); break;
		case 0x98: cycles = SUBC(registers.b); break;
		case 0x99: cycles = SUBC(registers.c); break;
		case 0x9A: cycles = SUBC(registers.d); break;
		case 0x9B: cycles = SUBC(registers.e); break;
		case 0x9C: cycles = SUBC(registers.h); break;
		case 0x9D: cycles = SUBC(registers.l); break;
		case 0x9E: cycles = 8; SUBC(MemoryRead(Combinebytes(registers.h, registers.l))); break;
		case 0xDE: cycles = 8; SUBC(MemoryRead(registers.pc)); registers.pc++; break;

		// AND
		case 0xA7: cycles = AND(registers.a); break;
		case 0xA0: cycles = AND(registers.b); break;
		case 0xA1: cycles = AND(registers.c); break;
		case 0xA2: cycles = AND(registers.d); break;
		case 0xA3: cycles = AND(registers.e); break;
		case 0xA4: cycles = AND(registers.h); break;
		case 0xA5: cycles = AND(registers.l); break;
		case 0xA6: cycles = 8; AND(Combinebytes(registers.h, registers.l)); break;
		case 0xE6: cycles = 8; AND(MemoryRead(registers.pc)); registers.pc++; break;

		// OR
		case 0xB7: cycles = OR(registers.a); break;
		case 0xB0: cycles = OR(registers.b); break;
		case 0xB1: cycles = OR(registers.c); break;
		case 0xB2: cycles = OR(registers.d); break;
		case 0xB3: cycles = OR(registers.e); break;
		case 0xB4: cycles = OR(registers.h); break;
		case 0xB5: cycles = OR(registers.l); break;
		case 0xB6: cycles = 8; OR(Combinebytes(registers.h, registers.l)); break;
		case 0xF6: cycles = 8; OR(MemoryRead(registers.pc)); registers.pc++; break;

		// XOR
		case 0xAF: cycles = XOR(registers.a); break;
		case 0xA8: cycles = XOR(registers.b); break;
		case 0xA9: cycles = XOR(registers.c); break;
		case 0xAA: cycles = XOR(registers.d); break;
		case 0xAB: cycles = XOR(registers.e); break;
		case 0xAC: cycles = XOR(registers.h); break;
		case 0xAD: cycles = XOR(registers.l); break;
		case 0xAE: cycles = 8; XOR(Combinebytes(registers.h, registers.l)); break;
		case 0xEE: cycles = 8; XOR(MemoryRead(registers.pc)); registers.pc++; break;

		// CP
		case 0xBF: cycles = CP(registers.a); break;
		case 0xB8: cycles = CP(registers.b); break;
		case 0xB9: cycles = CP(registers.c); break;
		case 0xBA: cycles = CP(registers.d); break;
		case 0xBB: cycles = CP(registers.e); break;
		case 0xBC: cycles = CP(registers.h); break;
		case 0xBD: cycles = CP(registers.l); break;
		case 0xBE: cycles = 8; CP(Combinebytes(registers.h, registers.l)); break;
		case 0xFE: cycles = 8; CP(MemoryRead(registers.pc)); registers.pc++; break;

		// INC
		case 0x3C: cycles = INC(registers.a); break;
		case 0x04: cycles = INC(registers.b); break;
		case 0x0C: cycles = INC(registers.c); break;
		case 0x14: cycles = INC(registers.d); break;
		case 0x1C: cycles = INC(registers.e); break;
		case 0x24: cycles = INC(registers.h); break;
		case 0x2C: cycles = INC(registers.l); break;
		case 0x34: cycles = INCMemory(Combinebytes(registers.h, registers.l)); break;

		// DEC
		case 0x3D: cycles = DEC(registers.a); break;
		case 0x05: cycles = DEC(registers.b); break;
		case 0x0D: cycles = DEC(registers.c); break;
		case 0x15: cycles = DEC(registers.d); break;
		case 0x1D: cycles = DEC(registers.e); break;
		case 0x25: cycles = DEC(registers.h); break;
		case 0x2D: cycles = DEC(registers.l); break;
		case 0x35: cycles = DECMemory(Combinebytes(registers.h, registers.l)); break;

		// 16 bit ALU
		// ADD HL
		case 0x09: cycles = ADDHL(Combinebytes(registers.b, registers.c)); break;
		case 0x19: cycles = ADDHL(Combinebytes(registers.d, registers.e)); break;
		case 0x29: cycles = ADDHL(Combinebytes(registers.h, registers.l)); break;
		case 0x39: cycles = ADDHL(registers.sp); break;

		// ADD SP
		case 0xE8: cycles = ADDSP(); break;

		// INC 16 bit
		case 0x03: cycles = INC16(registers.b, registers.c); break;
		case 0x13: cycles = INC16(registers.d, registers.e); break;
		case 0x23: cycles = INC16(registers.h, registers.l); break;
		case 0x33: cycles = INC16(registers.sp); break;

		// DEC 16 bit
		case 0x0B: cycles = DEC16(registers.b, registers.c); break;
		case 0x1B: cycles = DEC16(registers.d, registers.e); break;
		case 0x2B: cycles = DEC16(registers.h, registers.l); break;
		case 0x3B: cycles = DEC16(registers.sp); break;

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

short Cpu::LDRegFromMemory(unsigned char & reg, unsigned short address) {

	reg = MemoryRead(address);

	return 8; // cycles
}

short Cpu::LDRegFromMemory(unsigned char & reg) {

	reg = Combinebytes(MemoryRead(registers.pc), MemoryRead(registers.pc + 1));
	registers.pc += 2;
	return 16; // cycles
}

short Cpu::LDregHL(unsigned char & reg) {

	unsigned short address = Combinebytes(registers.h, registers.l);
	reg = MemoryRead(address);

	return 8; // cycles
}

short Cpu::LDmemfromreg(unsigned char & reg) {
	return 0;
}

short Cpu::LDmem() {
	return 0;
}

// put stackpointer plus immediate value into hl
short Cpu::LDHL() {

	signed char n = MemoryRead(registers.pc);
	short hl = registers.sp + n;
	registers.pc++;
	auto pair = splitBytes(hl);
	registers.h = pair.first;
	registers.l = pair.second;

	flags.zero = false;
	flags.negative = false;
	flags.halfCarry = halfCarry(registers.sp, n);
	flags.carry = carry(registers.sp, n);

	return 12; // cycles
}

// load immediate 16-bit value into register pair
short Cpu::LD16(unsigned char & reg1, unsigned char & reg2) {

	reg1 = MemoryRead(registers.pc + 1);
	reg2 = MemoryRead(registers.pc);
	registers.pc += 2;
	return 12;
}

// push two bytes onto the stack from a register pair
short Cpu::Push(unsigned char & reg1, unsigned char & reg2) {

	registers.sp--;
	MemoryWrite(registers.sp, reg1);
	registers.sp--;
	MemoryWrite(registers.sp, reg2);

	return 16;
}

// pop two bytes off the stack into a register pair
short Cpu::Pop(unsigned char & reg1, unsigned char & reg2) {

	reg2 = MemoryRead(registers.sp);
	registers.sp++;
	reg1 = MemoryRead(registers.sp);
	registers.sp++;

	return 12;
}

// indicates if the carry flag is set after addition from bit 3
bool Cpu::halfCarry(unsigned char value1, unsigned char value2) {

	value1 &= 0xF;
	value2 &= 0xF;

	return (value1 + value2) > 0xF;
}

// indicates if the carry flag is set after addition from bit 11
bool Cpu::halfCarry16(unsigned short value1, unsigned short value2) {

	value1 &= 0xFFF;
	value2 &= 0xFFF;

	return (value1 + value2) > 0xFFF;
}

// indicates if the carry flag is set after addition from bit 7
bool Cpu::carry(unsigned char value1, unsigned char value2) {

	unsigned short add = value1;
	add += value2;

	return add > 0xFF;
}

// indicates if the carry flag is set after 16-bit addition from bit 15
bool Cpu::carry16(unsigned short value1, unsigned short value2) {

	unsigned int add = value1;
	add += value2;

	return add > 0xFFFF;
}

// indicates if a bit is borrowed from bit 4 after a subtraction
bool Cpu::halfNoBorrow(unsigned char value1, unsigned char value2) {

	value1 &= 0xF;
	value2 &= 0xF;

	return ((value1 & 0xF) < (value2 & 0xF));
}

// indicates if a bit is borrowed after a subtraction
bool Cpu::noBorrow(unsigned char value1, unsigned char value2) {

	return (value1 < value2);
}

// add value to a, ignore carry
short Cpu::ADD(unsigned char value) {

	unsigned char result = registers.a + value;
	
	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = halfCarry(registers.a, value);
	flags.carry = carry(registers.a, value);

	registers.a = result;

	return 4; // sometimes add is 8 cycles, check the manual
}

// add value to a, include carry
short Cpu::ADDC(unsigned char value) {

	if (flags.carry) value++;
	unsigned char result = registers.a + value;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = halfCarry(registers.a, value);
	flags.carry = carry(registers.a, value);

	registers.a = result;

	return 4; // sometimes add is 8 cycles, check the manual
}

// subtract value from a, ignore carry
short Cpu::SUB(unsigned char value) {

	unsigned char result = registers.a - value;

	flags.zero = (result == 0);
	flags.negative = true;
	flags.halfCarry = halfNoBorrow(registers.a, value);
	flags.carry = noBorrow(registers.a, value);

	registers.a = result;

	return 4; // sometimes sub is 8 cycles, check the manual
}

// subtract value from a, include carry
short Cpu::SUBC(unsigned char value) {

	if (flags.carry) value++;
	unsigned char result = registers.a - value;

	flags.zero = (result == 0);
	flags.negative = true;
	flags.halfCarry = halfNoBorrow(registers.a, value);
	flags.carry = noBorrow(registers.a, value);

	registers.a = result;

	return 4; // sometimes sub is 8 cycles, check the manual
}

// and value with register a
short Cpu::AND(unsigned char value) {

	unsigned char result = registers.a & value;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = true;
	flags.carry = false;

	registers.a = result;

	return 4; // sometimes is 8 cycles, check the manual
}

// or value with register a
short Cpu::OR(unsigned char value) {

	unsigned char result = registers.a | value;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = false;
	flags.carry = false;

	registers.a = result;

	return 4; // sometimes is 8 cycles, check the manual
}

// xor value with register a
short Cpu::XOR(unsigned char value) {

	unsigned char result = registers.a ^ value;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = false;
	flags.carry = false;

	registers.a = result;

	return 4; // sometimes is 8 cycles, check the manual
}

// compare value to register a and set flags accordingly
short Cpu::CP(unsigned char value) {

	unsigned char result = registers.a - value;

	flags.zero = (result == 0);
	flags.negative = true;
	flags.halfCarry = halfNoBorrow(registers.a, value);
	flags.carry = noBorrow(registers.a, value);

	return 4; // sometimes is 8 cycles, check the manual
}

// increment register
short Cpu::INC(unsigned char& reg) {

	unsigned char result = reg + 1;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = halfCarry(reg, 1);

	reg = result;

	return 4; // sometimes is 12 cycles, check the manual
}

// increment memory location pointed to by register hl
short Cpu::INCMemory(unsigned short address) {

	unsigned char result = MemoryRead(address);
	result++;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = halfCarry(result, 1);

	MemoryWrite(address, result);

	return 12; // sometimes is 12 cycles, check the manual
}

// decrement register
short Cpu::DEC(unsigned char& reg) {

	unsigned char result = reg - 1;

	flags.zero = (result == 0);
	flags.negative = true;
	flags.halfCarry = halfNoBorrow(reg, 1);

	reg = result;

	return 4; // sometimes is 12 cycles, check the manual
}

// decrement memory location pointed to by register hl
short Cpu::DECMemory(unsigned short address) {

	unsigned char result = MemoryRead(address);
	result--;

	flags.zero = (result == 0);
	flags.negative = true;
	flags.halfCarry = halfNoBorrow(result, 1);

	MemoryWrite(address, result);


	return 12; 
}

// add value to register combination hl
short Cpu::ADDHL(unsigned short value) {

	unsigned short hl = Combinebytes(registers.h, registers.l);
	unsigned short result = hl + value;

	flags.negative = false;
	flags.halfCarry = halfCarry16(hl, value);
	flags.carry = halfCarry16(hl, value);

	auto pair = splitBytes(result);

	registers.h = pair.first;
	registers.l = pair.second;

	return 8;
}

// add immediate signed value to stack pointer
short Cpu::ADDSP() {

	signed char n = MemoryRead(registers.pc);
	registers.pc++;

	unsigned short result = registers.sp + n;

	flags.zero = false;
	flags.negative = false;
	flags.halfCarry = halfCarry16(registers.sp, n);
	flags.carry = carry16(registers.sp, n);;

	registers.sp = result;
	
	return 16;
}

// increment two registers as if they were one 16 bit register
short Cpu::INC16(unsigned char & reg1, unsigned char & reg2) {

	unsigned short combinedRegister = Combinebytes(reg1, reg2);

	combinedRegister++;

	auto pair = splitBytes(combinedRegister);

	reg1 = pair.first;
	reg2 = pair.second;

	return 8;
}

// increment 16 bit register
short Cpu::INC16(unsigned short & reg) {

	reg++;

	return 8;
}

// decrement two registers as if they were one 16 bit register
short Cpu::DEC16(unsigned char & reg1, unsigned char & reg2) {

	unsigned short combinedRegister = Combinebytes(reg1, reg2);

	combinedRegister--;

	auto pair = splitBytes(combinedRegister);

	reg1 = pair.first;
	reg2 = pair.second;

	return 8;
}


// decrement 16 bit register
short Cpu::DEC16(unsigned short & reg) {

	reg--;

	return 8;
}

// swap the lower and upper nibble
short Cpu::SWAP(unsigned char & reg) {

	unsigned char upper = reg & 0xF0;
	unsigned char lower = reg & 0x0F;

	upper >> 4;
	lower << 4;

	unsigned char result = upper | lower;

	flags.zero = (result == 0);
	flags.negative = false;
	flags.halfCarry = false;
	flags.carry = false;

	reg = result;

	return 8;
}

// swap the lower and upper nibble of a byte in memory
short Cpu::SWAPMemory() {

	unsigned short address = Combinebytes(registers.h, registers.l);

	unsigned char data = MemoryRead(address);

	SWAP(data);

	MemoryWrite(address, data);

	return 16;
}

// copied
short Cpu::DAA() {

	if (flags.negative) {
		if ((registers.a & 0x0F) > 0x09 || flags.halfCarry == true) {
			registers.a -= 0x06; //Half borrow: (0-1) = (0xF-0x6) = 9
			if ((registers.a & 0xF0) == 0xF0) flags.carry = true; else flags.carry = false;
		}

		if ((registers.a & 0xF0) > 0x90 || flags.carry == true) registers.a -= 0x60;
	}
	else {
		if ((registers.a & 0x0F) > 9 || flags.halfCarry == true) {
			registers.a += 0x06; //Half carry: (9+1) = (0xA+0x6) = 10
			if ((registers.a & 0xF0) == 0) flags.carry = true; else flags.carry = false;
		}

		if ((registers.a & 0xF0) > 0x90 || flags.carry == true) registers.a += 0x60;
	}

	if (registers.a == 0) flags.zero = true; else flags.zero = false;

	return 4; // cycles
}

short Cpu::CPL() {

	registers.a = ~registers.a;

	flags.negative = true;
	flags.halfCarry = true;

	return 4;
}

short Cpu::CCF() {

	flags.negative = false;
	flags.halfCarry = false;
	flags.carry = !flags.carry;

	return 4;
}

short Cpu::SCF() {

	flags.negative = false;
	flags.halfCarry = false;
	flags.carry = true;

	return 0;
}

short Cpu::NOP() {
	// does nothing
	return 4;
}

short Cpu::HALT() {

	halted = true;
	return 4;
}

short Cpu::STOP() {
	// todo
	return 4;
}

short Cpu::DI() {
	interupt = false;
	return 4;
}

short Cpu::EI() {
	interupt = true;
	return 4;
}

short Cpu::decrement16reg(unsigned char & reg1, unsigned char & reg2) {

	unsigned short combined = Combinebytes(reg1, reg2);
	combined--;
	reg1 = (combined >> 8);
	reg2 = (combined & 0xFF);

	return 0;
}

short Cpu::increment16reg(unsigned char & reg1, unsigned char & reg2) {

	unsigned short combined = Combinebytes(reg1, reg2);
	combined++;
	reg1 = (combined >> 8);
	reg2 = (combined & 0xFF);

	return 0;
}

unsigned char Cpu::MemoryRead(unsigned short address) {

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

void Cpu::MemoryWrite(unsigned short address, unsigned char data) {

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
				else if(data = 1) {
					memoryModel = mm4_32;
				}

			}
		}

	}
	else if (address >= 0x8000 && address <= 0x9FFF) { // 8kb video ram (vram) (1 for gameboy, 2 switchable for gameboy color)

	}
	else if (address >= 0xA000 && address <= 0xBFFF) { // 8kb external ram
		if (ramBankEnabled == true && mbc == 1 || mbc == 3 || mbc == 5) {
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

unsigned short Cpu::Combinebytes(unsigned char value1, unsigned char value2) {

	unsigned short combine = value2;
	combine <<= 8;
	combine |= value1;

	return combine;
}

std::pair<unsigned char, unsigned char> Cpu::splitBytes(unsigned short value) {

	unsigned char first = value & 0xFF;
	unsigned char second = (value >> 8) & 0xFF;

	return std::make_pair(first, second);
}
