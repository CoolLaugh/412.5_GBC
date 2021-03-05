#include "cpu.h"


// read and interprit the next opcode in memory then perform the instruction
word CPU::ExecuteOpcode() {

	word clocks = 0;

	//programCounterTrace.push(registers.pc);
	//while (programCounterTrace.size() > 100) {
	//	programCounterTrace.pop();
	//}
	outputState();
	byte opcode = memory->Read(registers.pc);
	registers.pc++;

	// read memory instead of rom if pc is 0x4000-0x7FFF or 0xA000 - 0xBFFF

	switch (opcode) {

	// 8 bit loads (3.3.1)
	// load value into register
	case 0x06: clocks = LD(registers.b); break;
	case 0x0E: clocks = LD(registers.c); break;
	case 0x16: clocks = LD(registers.d); break;
	case 0x1E: clocks = LD(registers.e); break;
	case 0x26: clocks = LD(registers.h); break;
	case 0x2E: clocks = LD(registers.l); break;
	case 0x3E: clocks = LD(registers.a); break;

	// load register into another register
	case 0x7F: clocks = LDreg(registers.a, registers.a); break;
	case 0x78: clocks = LDreg(registers.a, registers.b); break;
	case 0x79: clocks = LDreg(registers.a, registers.c); break;
	case 0x7A: clocks = LDreg(registers.a, registers.d); break;
	case 0x7B: clocks = LDreg(registers.a, registers.e); break;
	case 0x7C: clocks = LDreg(registers.a, registers.h); break;
	case 0x7D: clocks = LDreg(registers.a, registers.l); break;
	case 0x7E: clocks = LDregHL(registers.a); break;

	case 0x40: clocks = LDreg(registers.b, registers.b); break;
	case 0x41: clocks = LDreg(registers.b, registers.c); break;
	case 0x42: clocks = LDreg(registers.b, registers.d); break;
	case 0x43: clocks = LDreg(registers.b, registers.e); break;
	case 0x44: clocks = LDreg(registers.b, registers.h); break;
	case 0x45: clocks = LDreg(registers.b, registers.l); break;
	case 0x46: clocks = LDregHL(registers.b); break;

	case 0x48: clocks = LDreg(registers.c, registers.b); break;
	case 0x49: clocks = LDreg(registers.c, registers.c); break;
	case 0x4A: clocks = LDreg(registers.c, registers.d); break;
	case 0x4B: clocks = LDreg(registers.c, registers.e); break;
	case 0x4C: clocks = LDreg(registers.c, registers.h); break;
	case 0x4D: clocks = LDreg(registers.c, registers.l); break;
	case 0x4E: clocks = LDregHL(registers.c); break;

	case 0x50: clocks = LDreg(registers.d, registers.b); break;
	case 0x51: clocks = LDreg(registers.d, registers.c); break;
	case 0x52: clocks = LDreg(registers.d, registers.d); break;
	case 0x53: clocks = LDreg(registers.d, registers.e); break;
	case 0x54: clocks = LDreg(registers.d, registers.h); break;
	case 0x55: clocks = LDreg(registers.d, registers.l); break;
	case 0x56: clocks = LDregHL(registers.d); break;

	case 0x58: clocks = LDreg(registers.e, registers.b); break;
	case 0x59: clocks = LDreg(registers.e, registers.c); break;
	case 0x5A: clocks = LDreg(registers.e, registers.d); break;
	case 0x5B: clocks = LDreg(registers.e, registers.e); break;
	case 0x5C: clocks = LDreg(registers.e, registers.h); break;
	case 0x5D: clocks = LDreg(registers.e, registers.l); break;
	case 0x5E: clocks = LDregHL(registers.e); break;

	case 0x60: clocks = LDreg(registers.h, registers.b); break;
	case 0x61: clocks = LDreg(registers.h, registers.c); break;
	case 0x62: clocks = LDreg(registers.h, registers.d); break;
	case 0x63: clocks = LDreg(registers.h, registers.e); break;
	case 0x64: clocks = LDreg(registers.h, registers.h); break;
	case 0x65: clocks = LDreg(registers.h, registers.l); break;
	case 0x66: clocks = LDregHL(registers.h); break;

	case 0x68: clocks = LDreg(registers.l, registers.b); break;
	case 0x69: clocks = LDreg(registers.l, registers.c); break;
	case 0x6A: clocks = LDreg(registers.l, registers.d); break;
	case 0x6B: clocks = LDreg(registers.l, registers.e); break;
	case 0x6C: clocks = LDreg(registers.l, registers.h); break;
	case 0x6D: clocks = LDreg(registers.l, registers.l); break;
	case 0x6E: clocks = LDregHL(registers.l); break;

	case 0x70: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.b); AdvanceClocks(8); break;
	case 0x71: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.c); AdvanceClocks(8); break;
	case 0x72: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.d); AdvanceClocks(8); break;
	case 0x73: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.e); AdvanceClocks(8); break;
	case 0x74: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.h); AdvanceClocks(8); break;
	case 0x75: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.l); AdvanceClocks(8); break;
	case 0x36: {

		clocks = 12; 
		byte value = memory->Read(registers.pc);
		AdvanceClocks(4);
		memory->Write(Combinebytes(registers.l, registers.h), value);
		AdvanceClocks(8); 
		registers.pc++; 
		break;
	}
	case 0x0A: clocks = LDRegFromMemory(registers.a, Combinebytes(registers.c, registers.b)); break;
	case 0x1A: clocks = LDRegFromMemory(registers.a, Combinebytes(registers.e, registers.d)); break;
	case 0xFA: {
		
		byte first = memory->Read(registers.pc);
		AdvanceClocks(4);
		byte second = memory->Read(registers.pc + 1);
		AdvanceClocks(4);
		clocks = LDRegFromMemory(registers.a, Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1))); 
		registers.pc += 2; 
		break;
	} 

	case 0x47: clocks = LDreg(registers.b, registers.a); break;
	case 0x4F: clocks = LDreg(registers.c, registers.a); break;
	case 0x57: clocks = LDreg(registers.d, registers.a); break;
	case 0x5F: clocks = LDreg(registers.e, registers.a); break;
	case 0x67: clocks = LDreg(registers.h, registers.a); break;
	case 0x6F: clocks = LDreg(registers.l, registers.a); break;

	case 0x02: clocks = 8; memory->Write(Combinebytes(registers.c, registers.b), registers.a); AdvanceClocks(8); break;
	case 0x12: clocks = 8; memory->Write(Combinebytes(registers.e, registers.d), registers.a); AdvanceClocks(8); break;
	case 0x77: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.a); AdvanceClocks(8); break;
	case 0xEA: {
		clocks = 16; 
		byte low = memory->Read(registers.pc);
		AdvanceClocks(4);
		byte high = memory->Read(registers.pc + 1);
		AdvanceClocks(4);
		memory->Write(Combinebytes(low, high), registers.a);
		AdvanceClocks(8); 
		registers.pc += 2; 
		break;
	}

	case 0xF2: clocks = 8; registers.a = memory->Read(0xFF00 + registers.c); AdvanceClocks(8); break;

	case 0xE2: clocks = 8; memory->Write(0xFF00 + registers.c, registers.a); AdvanceClocks(8); break;

	case 0x3A: clocks = 8; registers.a = memory->Read(Combinebytes(registers.l, registers.h)); decrement16reg(registers.h, registers.l); AdvanceClocks(8); break;

	case 0x32: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.a); decrement16reg(registers.h, registers.l); AdvanceClocks(8); break;

	case 0x2A: clocks = 8; registers.a = memory->Read(Combinebytes(registers.l, registers.h)); increment16reg(registers.h, registers.l); AdvanceClocks(8); break;

	case 0x22: clocks = 8; memory->Write(Combinebytes(registers.l, registers.h), registers.a); increment16reg(registers.h, registers.l); AdvanceClocks(8); break;

	case 0xE0: {
		clocks = 12; 
		byte addressOffset = memory->Read(registers.pc);
		AdvanceClocks(4);
		memory->Write(0xFF00 + addressOffset, registers.a);
		registers.pc++; 
		AdvanceClocks(8); 
		break;
	}

	case 0xF0: {
		clocks = 12;
		
		byte value = memory->Read(registers.pc);
		AdvanceClocks(4);
		registers.a = memory->Read(0xFF00 + value);
		AdvanceClocks(8);
		registers.pc++;  
		break;
	} 

	case 0x01: clocks = LD16(registers.b, registers.c); break;
	case 0x11: clocks = LD16(registers.d, registers.e); break;
	case 0x21: clocks = LD16(registers.h, registers.l); break;
	case 0x31: clocks = LD16(registers.sp); break;

	case 0xF9: clocks = 8; registers.sp = Combinebytes(registers.l, registers.h); AdvanceClocks(8); break;

	case 0xF8: clocks = LDHL(); break;

	case 0x08: clocks = WriteSP(); break;

	// push register pairs onto the stack
	case 0xF5: clocks = Push(registers.a, registers.f); break; // may need to reverse the order of registers
	case 0xC5: clocks = Push(registers.b, registers.c); break;
	case 0xD5: clocks = Push(registers.d, registers.e); break;
	case 0xE5: clocks = Push(registers.h, registers.l); break;

	// pop register pairs off of the stack
	case 0xF1: clocks = Pop(registers.a, registers.f); registers.f &= 0xF0; break;
	case 0xC1: clocks = Pop(registers.b, registers.c); break;
	case 0xD1: clocks = Pop(registers.d, registers.e); break;
	case 0xE1: clocks = Pop(registers.h, registers.l); break;

	// add
	case 0x87: clocks = ADD(registers.a); break;
	case 0x80: clocks = ADD(registers.b); break;
	case 0x81: clocks = ADD(registers.c); break;
	case 0x82: clocks = ADD(registers.d); break;
	case 0x83: clocks = ADD(registers.e); break;
	case 0x84: clocks = ADD(registers.h); break;
	case 0x85: clocks = ADD(registers.l); break;
	case 0x86: clocks = ADD(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xC6: clocks = ADD(memory->Read(registers.pc), 8); registers.pc++; break;

	// add with carry
	case 0x8F: clocks = ADDC(registers.a); break;
	case 0x88: clocks = ADDC(registers.b); break;
	case 0x89: clocks = ADDC(registers.c); break;
	case 0x8A: clocks = ADDC(registers.d); break;
	case 0x8B: clocks = ADDC(registers.e); break;
	case 0x8C: clocks = ADDC(registers.h); break;
	case 0x8D: clocks = ADDC(registers.l); break;
	case 0x8E: clocks = ADDC(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xCE: clocks = ADDC(memory->Read(registers.pc), 8); registers.pc++; break;

	// sub
	case 0x97: clocks = SUB(registers.a); break;
	case 0x90: clocks = SUB(registers.b); break;
	case 0x91: clocks = SUB(registers.c); break;
	case 0x92: clocks = SUB(registers.d); break;
	case 0x93: clocks = SUB(registers.e); break;
	case 0x94: clocks = SUB(registers.h); break;
	case 0x95: clocks = SUB(registers.l); break;
	case 0x96: clocks = SUB(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xD6: clocks = SUB(memory->Read(registers.pc), 8); registers.pc++; break;

	// sub with carry
	case 0x9F: clocks = SUBC(registers.a); break;
	case 0x98: clocks = SUBC(registers.b); break;
	case 0x99: clocks = SUBC(registers.c); break;
	case 0x9A: clocks = SUBC(registers.d); break;
	case 0x9B: clocks = SUBC(registers.e); break;
	case 0x9C: clocks = SUBC(registers.h); break;
	case 0x9D: clocks = SUBC(registers.l); break;
	case 0x9E: clocks = SUBC(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xDE: clocks = SUBC(memory->Read(registers.pc), 8); registers.pc++; break;

	// AND
	case 0xA7: clocks = AND(registers.a); break;
	case 0xA0: clocks = AND(registers.b); break;
	case 0xA1: clocks = AND(registers.c); break;
	case 0xA2: clocks = AND(registers.d); break;
	case 0xA3: clocks = AND(registers.e); break;
	case 0xA4: clocks = AND(registers.h); break;
	case 0xA5: clocks = AND(registers.l); break;
	case 0xA6: clocks = AND(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xE6: clocks = AND(memory->Read(registers.pc), 8); registers.pc++; break;

	// OR
	case 0xB7: clocks = OR(registers.a); break;
	case 0xB0: clocks = OR(registers.b); break;
	case 0xB1: clocks = OR(registers.c); break;
	case 0xB2: clocks = OR(registers.d); break;
	case 0xB3: clocks = OR(registers.e); break;
	case 0xB4: clocks = OR(registers.h); break;
	case 0xB5: clocks = OR(registers.l); break;
	case 0xB6: clocks = OR(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xF6: clocks = OR(memory->Read(registers.pc), 8); registers.pc++; break;

	// XOR
	case 0xAF: clocks = XOR(registers.a); break;
	case 0xA8: clocks = XOR(registers.b); break;
	case 0xA9: clocks = XOR(registers.c); break;
	case 0xAA: clocks = XOR(registers.d); break;
	case 0xAB: clocks = XOR(registers.e); break;
	case 0xAC: clocks = XOR(registers.h); break;
	case 0xAD: clocks = XOR(registers.l); break;
	case 0xAE: clocks = XOR(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xEE: clocks = XOR(memory->Read(registers.pc), 8); registers.pc++; break;

	// CP
	case 0xBF: clocks = CP(registers.a); break;
	case 0xB8: clocks = CP(registers.b); break;
	case 0xB9: clocks = CP(registers.c); break;
	case 0xBA: clocks = CP(registers.d); break;
	case 0xBB: clocks = CP(registers.e); break;
	case 0xBC: clocks = CP(registers.h); break;
	case 0xBD: clocks = CP(registers.l); break;
	case 0xBE: clocks = CP(memory->Read(Combinebytes(registers.l, registers.h)), 8); break;
	case 0xFE: clocks = CP(memory->Read(registers.pc), 8); registers.pc++; break;

	// INC
	case 0x3C: clocks = INC(registers.a); break;
	case 0x04: clocks = INC(registers.b); break;
	case 0x0C: clocks = INC(registers.c); break;
	case 0x14: clocks = INC(registers.d); break;
	case 0x1C: clocks = INC(registers.e); break;
	case 0x24: clocks = INC(registers.h); break;
	case 0x2C: clocks = INC(registers.l); break;
	case 0x34: clocks = INCMemory(Combinebytes(registers.l, registers.h)); break;

	// DEC
	case 0x3D: clocks = DEC(registers.a); break;
	case 0x05: clocks = DEC(registers.b); break;
	case 0x0D: clocks = DEC(registers.c); break;
	case 0x15: clocks = DEC(registers.d); break;
	case 0x1D: clocks = DEC(registers.e); break;
	case 0x25: clocks = DEC(registers.h); break;
	case 0x2D: clocks = DEC(registers.l); break;
	case 0x35: clocks = DECMemory(Combinebytes(registers.l, registers.h)); break;

	// 16 bit ALU
	// ADD HL
	case 0x09: clocks = ADDHL(Combinebytes(registers.c, registers.b)); break;
	case 0x19: clocks = ADDHL(Combinebytes(registers.e, registers.d)); break;
	case 0x29: clocks = ADDHL(Combinebytes(registers.l, registers.h)); break;
	case 0x39: clocks = ADDHL(registers.sp); break;

	// ADD SP
	case 0xE8: clocks = ADDSP(); break;

	// INC 16 bit
	case 0x03: clocks = INC16(registers.c, registers.b); break;
	case 0x13: clocks = INC16(registers.e, registers.d); break;
	case 0x23: clocks = INC16(registers.l, registers.h); break;
	case 0x33: clocks = INC16(registers.sp); break;

	// DEC 16 bit
	case 0x0B: clocks = DEC16(registers.c, registers.b); break;
	case 0x1B: clocks = DEC16(registers.e, registers.d); break;
	case 0x2B: clocks = DEC16(registers.l, registers.h); break;
	case 0x3B: clocks = DEC16(registers.sp); break;

	// Extended opcodes
	case 0xCB: clocks = ExecuteExtendedOpcode(); break;

	// Miscellaneous
	case 0x27: clocks = DAA(); break;

	case 0x2F: clocks = CPL(); break;

	case 0x3F: clocks = CCF(); break;

	case 0x37: clocks = SCF(); break;

	case 0x00: clocks = NOP(); break;

	case 0x76: clocks = HALT(); break;

	case 0x10: clocks = STOP(); break;

	case 0xF3: clocks = DI(); break;

	case 0xFB: clocks = EI(); break;

	// rotates and shifts
	case 0x07: clocks = RLC(registers.a, true, 4); break;
	case 0x17: clocks = RL(registers.a, true, 4); break;
	case 0x0F: clocks = RRC(registers.a, true, 4); break;
	case 0x1F: clocks = RR(registers.a, true, 4); break;

	// jump
	case 0xC3: clocks = JP(); break;

	// conditional jump
	case 0xC2: clocks = JPcc(condition::NotZero); break;
	case 0xCA: clocks = JPcc(condition::Zero); break;
	case 0xD2: clocks = JPcc(condition::NotCarry); break;
	case 0xDA: clocks = JPcc(condition::Carry); break;

	// jump to hl
	case 0xE9: clocks = JPHL(); break;

	// relative jump
	case 0x18: clocks = JR(); break;

	// conditional relative jump
	case 0x20: clocks = JRcc(condition::NotZero); break;
	case 0x28: clocks = JRcc(condition::Zero); break;
	case 0x30: clocks = JRcc(condition::NotCarry); break;
	case 0x38: clocks = JRcc(condition::Carry); break;

	// call
	case 0xCD: clocks = CALL(); break;

	// conditional call
	case 0xC4: clocks = CALLcc(condition::NotZero); break;
	case 0xCC: clocks = CALLcc(condition::Zero); break;
	case 0xD4: clocks = CALLcc(condition::NotCarry); break;
	case 0xDC: clocks = CALLcc(condition::Carry); break;

	// restart
	case 0xC7: clocks = RST(0x00);  break;
	case 0xCF: clocks = RST(0x08);  break;
	case 0xD7: clocks = RST(0x10);  break;
	case 0xDF: clocks = RST(0x18);  break;
	case 0xE7: clocks = RST(0x20);  break;
	case 0xEF: clocks = RST(0x28);  break;
	case 0xF7: clocks = RST(0x30);  break;
	case 0xFF: clocks = RST(0x38);  break;

	// return
	case 0xC9: clocks = RET(); break;

	// conditional return
	case 0xC0: clocks = RETcc(condition::NotZero); break;
	case 0xC8: clocks = RETcc(condition::Zero); break;
	case 0xD0: clocks = RETcc(condition::NotCarry); break;
	case 0xD8: clocks = RETcc(condition::Carry); break;

	// return and enable interups
	case 0xD9: clocks = RETI(); break;


	default: std::cout << "Unknown Opcode " << std::hex << opcode << "\n" ; break;
	}

	return clocks;
}


// same as ExecuteOpcode() but for extended opcodes (0xCB 0xXX)
word CPU::ExecuteExtendedOpcode() {

	word clocks = 0;

	byte opcode = memory->Read(registers.pc);
	registers.pc++;

	switch (opcode) {

	// Miscellaneous
	case 0x37: clocks = SWAP(registers.a); break;
	case 0x30: clocks = SWAP(registers.b); break;
	case 0x31: clocks = SWAP(registers.c); break;
	case 0x32: clocks = SWAP(registers.d); break;
	case 0x33: clocks = SWAP(registers.e); break;
	case 0x34: clocks = SWAP(registers.h); break;
	case 0x35: clocks = SWAP(registers.l); break;
	case 0x36: clocks = SWAP(Combinebytes(registers.l, registers.h)); break;

	// rotates and shifts
	// rotate left bit 7 to carry
	case 0x07: clocks = RLC(registers.a); break;
	case 0x00: clocks = RLC(registers.b); break;
	case 0x01: clocks = RLC(registers.c); break;
	case 0x02: clocks = RLC(registers.d); break;
	case 0x03: clocks = RLC(registers.e); break;
	case 0x04: clocks = RLC(registers.h); break;
	case 0x05: clocks = RLC(registers.l); break;
	case 0x06: clocks = RLC(Combinebytes(registers.l, registers.h)); break;

	// rotate left through carry
	case 0x17: clocks = RL(registers.a); break;
	case 0x10: clocks = RL(registers.b); break;
	case 0x11: clocks = RL(registers.c); break;
	case 0x12: clocks = RL(registers.d); break;
	case 0x13: clocks = RL(registers.e); break;
	case 0x14: clocks = RL(registers.h); break;
	case 0x15: clocks = RL(registers.l); break;
	case 0x16: clocks = RL(Combinebytes(registers.l, registers.h)); break;

	// rotate right bit 7 to carry
	case 0x0F: clocks = RRC(registers.a); break;
	case 0x08: clocks = RRC(registers.b); break;
	case 0x09: clocks = RRC(registers.c); break;
	case 0x0A: clocks = RRC(registers.d); break;
	case 0x0B: clocks = RRC(registers.e); break;
	case 0x0C: clocks = RRC(registers.h); break;
	case 0x0D: clocks = RRC(registers.l); break;
	case 0x0E: clocks = RRC(Combinebytes(registers.l, registers.h)); break;

	// rotate right through carry
	case 0x1F: clocks = RR(registers.a); break;
	case 0x18: clocks = RR(registers.b); break;
	case 0x19: clocks = RR(registers.c); break;
	case 0x1A: clocks = RR(registers.d); break;
	case 0x1B: clocks = RR(registers.e); break;
	case 0x1C: clocks = RR(registers.h); break;
	case 0x1D: clocks = RR(registers.l); break;
	case 0x1E: clocks = RR(Combinebytes(registers.l, registers.h)); break;

	// shift left into carry lsb set to 0
	case 0x27: clocks = SLA(registers.a); break;
	case 0x20: clocks = SLA(registers.b); break;
	case 0x21: clocks = SLA(registers.c); break;
	case 0x22: clocks = SLA(registers.d); break;
	case 0x23: clocks = SLA(registers.e); break;
	case 0x24: clocks = SLA(registers.h); break;
	case 0x25: clocks = SLA(registers.l); break;
	case 0x26: clocks = SLA(Combinebytes(registers.l, registers.h)); break;

	// shift right into carry, msb stays the same
	case 0x2F: clocks = SRA(registers.a); break;
	case 0x28: clocks = SRA(registers.b); break;
	case 0x29: clocks = SRA(registers.c); break;
	case 0x2A: clocks = SRA(registers.d); break;
	case 0x2B: clocks = SRA(registers.e); break;
	case 0x2C: clocks = SRA(registers.h); break;
	case 0x2D: clocks = SRA(registers.l); break;
	case 0x2E: clocks = SRA(Combinebytes(registers.l, registers.h)); break;

	// shift right into carry msb set to 0
	case 0x3F: clocks = SRL(registers.a); break;
	case 0x38: clocks = SRL(registers.b); break;
	case 0x39: clocks = SRL(registers.c); break;
	case 0x3A: clocks = SRL(registers.d); break;
	case 0x3B: clocks = SRL(registers.e); break;
	case 0x3C: clocks = SRL(registers.h); break;
	case 0x3D: clocks = SRL(registers.l); break;
	case 0x3E: clocks = SRL(Combinebytes(registers.l, registers.h)); break;

	// check bit and set flags
	case 0x40: clocks = BIT(registers.b, 0); break;
	case 0x41: clocks = BIT(registers.c, 0); break;
	case 0x42: clocks = BIT(registers.d, 0); break;
	case 0x43: clocks = BIT(registers.e, 0); break;
	case 0x44: clocks = BIT(registers.h, 0); break;
	case 0x45: clocks = BIT(registers.l, 0); break;
	case 0x46: clocks = BITHL(0); break;
	case 0x47: clocks = BIT(registers.a, 0); break;
	case 0x48: clocks = BIT(registers.b, 1); break;
	case 0x49: clocks = BIT(registers.c, 1); break;
	case 0x4A: clocks = BIT(registers.d, 1); break;
	case 0x4B: clocks = BIT(registers.e, 1); break;
	case 0x4C: clocks = BIT(registers.h, 1); break;
	case 0x4D: clocks = BIT(registers.l, 1); break;
	case 0x4E: clocks = BITHL(1); break;
	case 0x4F: clocks = BIT(registers.a, 1); break;
	case 0x50: clocks = BIT(registers.b, 2); break;
	case 0x51: clocks = BIT(registers.c, 2); break;
	case 0x52: clocks = BIT(registers.d, 2); break;
	case 0x53: clocks = BIT(registers.e, 2); break;
	case 0x54: clocks = BIT(registers.h, 2); break;
	case 0x55: clocks = BIT(registers.l, 2); break;
	case 0x56: clocks = BITHL(2); break;
	case 0x57: clocks = BIT(registers.a, 2); break;
	case 0x58: clocks = BIT(registers.b, 3); break;
	case 0x59: clocks = BIT(registers.c, 3); break;
	case 0x5A: clocks = BIT(registers.d, 3); break;
	case 0x5B: clocks = BIT(registers.e, 3); break;
	case 0x5C: clocks = BIT(registers.h, 3); break;
	case 0x5D: clocks = BIT(registers.l, 3); break;
	case 0x5E: clocks = BITHL(3); break;
	case 0x5F: clocks = BIT(registers.a, 3); break;
	case 0x60: clocks = BIT(registers.b, 4); break;
	case 0x61: clocks = BIT(registers.c, 4); break;
	case 0x62: clocks = BIT(registers.d, 4); break;
	case 0x63: clocks = BIT(registers.e, 4); break;
	case 0x64: clocks = BIT(registers.h, 4); break;
	case 0x65: clocks = BIT(registers.l, 4); break;
	case 0x66: clocks = BITHL(4); break;
	case 0x67: clocks = BIT(registers.a, 4); break;
	case 0x68: clocks = BIT(registers.b, 5); break;
	case 0x69: clocks = BIT(registers.c, 5); break;
	case 0x6A: clocks = BIT(registers.d, 5); break;
	case 0x6B: clocks = BIT(registers.e, 5); break;
	case 0x6C: clocks = BIT(registers.h, 5); break;
	case 0x6D: clocks = BIT(registers.l, 5); break;
	case 0x6E: clocks = BITHL(5); break;
	case 0x6F: clocks = BIT(registers.a, 5); break;
	case 0x70: clocks = BIT(registers.b, 6); break;
	case 0x71: clocks = BIT(registers.c, 6); break;
	case 0x72: clocks = BIT(registers.d, 6); break;
	case 0x73: clocks = BIT(registers.e, 6); break;
	case 0x74: clocks = BIT(registers.h, 6); break;
	case 0x75: clocks = BIT(registers.l, 6); break;
	case 0x76: clocks = BITHL(6); break;
	case 0x77: clocks = BIT(registers.a, 6); break;
	case 0x78: clocks = BIT(registers.b, 7); break;
	case 0x79: clocks = BIT(registers.c, 7); break;
	case 0x7A: clocks = BIT(registers.d, 7); break;
	case 0x7B: clocks = BIT(registers.e, 7); break;
	case 0x7C: clocks = BIT(registers.h, 7); break;
	case 0x7D: clocks = BIT(registers.l, 7); break;
	case 0x7E: clocks = BITHL(7); break;
	case 0x7F: clocks = BIT(registers.a, 7); break;

	// set bit
	case 0xC0: clocks = SET(registers.b, 0); break;
	case 0xC1: clocks = SET(registers.c, 0); break;
	case 0xC2: clocks = SET(registers.d, 0); break;
	case 0xC3: clocks = SET(registers.e, 0); break;
	case 0xC4: clocks = SET(registers.h, 0); break;
	case 0xC5: clocks = SET(registers.l, 0); break;
	case 0xC6: clocks = SET(Combinebytes(registers.l, registers.h), 0); break;
	case 0xC7: clocks = SET(registers.a, 0); break;
	case 0xC8: clocks = SET(registers.b, 1); break;
	case 0xC9: clocks = SET(registers.c, 1); break;
	case 0xCA: clocks = SET(registers.d, 1); break;
	case 0xCB: clocks = SET(registers.e, 1); break;
	case 0xCC: clocks = SET(registers.h, 1); break;
	case 0xCD: clocks = SET(registers.l, 1); break;
	case 0xCE: clocks = SET(Combinebytes(registers.l, registers.h), 1); break;
	case 0xCF: clocks = SET(registers.a, 1); break;
	case 0xD0: clocks = SET(registers.b, 2); break;
	case 0xD1: clocks = SET(registers.c, 2); break;
	case 0xD2: clocks = SET(registers.d, 2); break;
	case 0xD3: clocks = SET(registers.e, 2); break;
	case 0xD4: clocks = SET(registers.h, 2); break;
	case 0xD5: clocks = SET(registers.l, 2); break;
	case 0xD6: clocks = SET(Combinebytes(registers.l, registers.h), 2); break;
	case 0xD7: clocks = SET(registers.a, 2); break;
	case 0xD8: clocks = SET(registers.b, 3); break;
	case 0xD9: clocks = SET(registers.c, 3); break;
	case 0xDA: clocks = SET(registers.d, 3); break;
	case 0xDB: clocks = SET(registers.e, 3); break;
	case 0xDC: clocks = SET(registers.h, 3); break;
	case 0xDD: clocks = SET(registers.l, 3); break;
	case 0xDE: clocks = SET(Combinebytes(registers.l, registers.h), 3); break;
	case 0xDF: clocks = SET(registers.a, 3); break;
	case 0xE0: clocks = SET(registers.b, 4); break;
	case 0xE1: clocks = SET(registers.c, 4); break;
	case 0xE2: clocks = SET(registers.d, 4); break;
	case 0xE3: clocks = SET(registers.e, 4); break;
	case 0xE4: clocks = SET(registers.h, 4); break;
	case 0xE5: clocks = SET(registers.l, 4); break;
	case 0xE6: clocks = SET(Combinebytes(registers.l, registers.h), 4); break;
	case 0xE7: clocks = SET(registers.a, 4); break;
	case 0xE8: clocks = SET(registers.b, 5); break;
	case 0xE9: clocks = SET(registers.c, 5); break;
	case 0xEA: clocks = SET(registers.d, 5); break;
	case 0xEB: clocks = SET(registers.e, 5); break;
	case 0xEC: clocks = SET(registers.h, 5); break;
	case 0xED: clocks = SET(registers.l, 5); break;
	case 0xEE: clocks = SET(Combinebytes(registers.l, registers.h), 5); break;
	case 0xEF: clocks = SET(registers.a, 5); break;
	case 0xF0: clocks = SET(registers.b, 6); break;
	case 0xF1: clocks = SET(registers.c, 6); break;
	case 0xF2: clocks = SET(registers.d, 6); break;
	case 0xF3: clocks = SET(registers.e, 6); break;
	case 0xF4: clocks = SET(registers.h, 6); break;
	case 0xF5: clocks = SET(registers.l, 6); break;
	case 0xF6: clocks = SET(Combinebytes(registers.l, registers.h), 6); break;
	case 0xF7: clocks = SET(registers.a, 6); break;
	case 0xF8: clocks = SET(registers.b, 7); break;
	case 0xF9: clocks = SET(registers.c, 7); break;
	case 0xFA: clocks = SET(registers.d, 7); break;
	case 0xFB: clocks = SET(registers.e, 7); break;
	case 0xFC: clocks = SET(registers.h, 7); break;
	case 0xFD: clocks = SET(registers.l, 7); break;
	case 0xFE: clocks = SET(Combinebytes(registers.l, registers.h), 7); break;
	case 0xFF: clocks = SET(registers.a, 7); break;

	// reset bit
	case 0x80: clocks = RES(registers.b, 0); break;
	case 0x81: clocks = RES(registers.c, 0); break;
	case 0x82: clocks = RES(registers.d, 0); break;
	case 0x83: clocks = RES(registers.e, 0); break;
	case 0x84: clocks = RES(registers.h, 0); break;
	case 0x85: clocks = RES(registers.l, 0); break;
	case 0x86: clocks = RES(Combinebytes(registers.l, registers.h), 0); break;
	case 0x87: clocks = RES(registers.a, 0); break;
	case 0x88: clocks = RES(registers.b, 1); break;
	case 0x89: clocks = RES(registers.c, 1); break;
	case 0x8A: clocks = RES(registers.d, 1); break;
	case 0x8B: clocks = RES(registers.e, 1); break;
	case 0x8C: clocks = RES(registers.h, 1); break;
	case 0x8D: clocks = RES(registers.l, 1); break;
	case 0x8E: clocks = RES(Combinebytes(registers.l, registers.h), 1); break;
	case 0x8F: clocks = RES(registers.a, 1); break;
	case 0x90: clocks = RES(registers.b, 2); break;
	case 0x91: clocks = RES(registers.c, 2); break;
	case 0x92: clocks = RES(registers.d, 2); break;
	case 0x93: clocks = RES(registers.e, 2); break;
	case 0x94: clocks = RES(registers.h, 2); break;
	case 0x95: clocks = RES(registers.l, 2); break;
	case 0x96: clocks = RES(Combinebytes(registers.l, registers.h), 2); break;
	case 0x97: clocks = RES(registers.a, 2); break;
	case 0x98: clocks = RES(registers.b, 3); break;
	case 0x99: clocks = RES(registers.c, 3); break;
	case 0x9A: clocks = RES(registers.d, 3); break;
	case 0x9B: clocks = RES(registers.e, 3); break;
	case 0x9C: clocks = RES(registers.h, 3); break;
	case 0x9D: clocks = RES(registers.l, 3); break;
	case 0x9E: clocks = RES(Combinebytes(registers.l, registers.h), 3); break;
	case 0x9F: clocks = RES(registers.a, 3); break;
	case 0xA0: clocks = RES(registers.b, 4); break;
	case 0xA1: clocks = RES(registers.c, 4); break;
	case 0xA2: clocks = RES(registers.d, 4); break;
	case 0xA3: clocks = RES(registers.e, 4); break;
	case 0xA4: clocks = RES(registers.h, 4); break;
	case 0xA5: clocks = RES(registers.l, 4); break;
	case 0xA6: clocks = RES(Combinebytes(registers.l, registers.h), 4); break;
	case 0xA7: clocks = RES(registers.a, 4); break;
	case 0xA8: clocks = RES(registers.b, 5); break;
	case 0xA9: clocks = RES(registers.c, 5); break;
	case 0xAA: clocks = RES(registers.d, 5); break;
	case 0xAB: clocks = RES(registers.e, 5); break;
	case 0xAC: clocks = RES(registers.h, 5); break;
	case 0xAD: clocks = RES(registers.l, 5); break;
	case 0xAE: clocks = RES(Combinebytes(registers.l, registers.h), 5); break;
	case 0xAF: clocks = RES(registers.a, 5); break;
	case 0xB0: clocks = RES(registers.b, 6); break;
	case 0xB1: clocks = RES(registers.c, 6); break;
	case 0xB2: clocks = RES(registers.d, 6); break;
	case 0xB3: clocks = RES(registers.e, 6); break;
	case 0xB4: clocks = RES(registers.h, 6); break;
	case 0xB5: clocks = RES(registers.l, 6); break;
	case 0xB6: clocks = RES(Combinebytes(registers.l, registers.h), 6); break;
	case 0xB7: clocks = RES(registers.a, 6); break;
	case 0xB8: clocks = RES(registers.b, 7); break;
	case 0xB9: clocks = RES(registers.c, 7); break;
	case 0xBA: clocks = RES(registers.d, 7); break;
	case 0xBB: clocks = RES(registers.e, 7); break;
	case 0xBC: clocks = RES(registers.h, 7); break;
	case 0xBD: clocks = RES(registers.l, 7); break;
	case 0xBE: clocks = RES(Combinebytes(registers.l, registers.h), 7); break;
	case 0xBF: clocks = RES(registers.a, 7); break;

	default: throw "Unknown extended Opcode"; break;
	}
	return clocks;
}