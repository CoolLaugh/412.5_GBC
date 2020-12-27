#include "cpu.h"



word Cpu::ExecuteOpcode() {

	word cycles = 0;

	byte opcode = memory.Read(registers.pc);
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

	case 0x70: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.b); break;
	case 0x71: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.b); break;
	case 0x72: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.b); break;
	case 0x73: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.b); break;
	case 0x74: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.b); break;
	case 0x75: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.b); break;
	case 0x36: cycles = 12; memory.Write(Combinebytes(registers.h, registers.l), memory.Read(registers.pc));  registers.pc++; break;

	case 0x0A: cycles = LDRegFromMemory(registers.a, Combinebytes(registers.b, registers.c)); break;
	case 0x1A: cycles = LDRegFromMemory(registers.a, Combinebytes(registers.d, registers.e)); break;
	case 0xFA: cycles = 16; LDRegFromMemory(registers.a, Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1))); registers.pc += 2; break;
	case 0x3E: cycles = 8; registers.a = memory.Read(registers.pc); registers.pc++; break;

	case 0x47: cycles = LDreg(registers.b, registers.a); break;
	case 0x4F: cycles = LDreg(registers.c, registers.a); break;
	case 0x57: cycles = LDreg(registers.d, registers.a); break;
	case 0x5F: cycles = LDreg(registers.e, registers.a); break;
	case 0x67: cycles = LDreg(registers.h, registers.a); break;
	case 0x6F: cycles = LDreg(registers.l, registers.a); break;

	case 0x02: cycles = 8; memory.Write(Combinebytes(registers.b, registers.c), registers.a); break;
	case 0x12: cycles = 8; memory.Write(Combinebytes(registers.b, registers.c), registers.a); break;
	case 0x77: cycles = 8; memory.Write(Combinebytes(registers.b, registers.c), registers.a); break;
	case 0xEA: cycles = 16; memory.Write(Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1)), registers.a); registers.pc += 2; break;

	case 0xF2: cycles = 8; registers.a = memory.Read(0xFF00 + registers.c); break;

	case 0xE2: cycles = 8; memory.Write(0xFF00 + registers.c, registers.a); break;

	case 0x3A: cycles = 8; registers.a = memory.Read(Combinebytes(registers.h, registers.l)); decrement16reg(registers.h, registers.l); break;

	case 0x32: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.a); decrement16reg(registers.h, registers.l); break;

	case 0x2A: cycles = 8; registers.a = memory.Read(Combinebytes(registers.h, registers.l)); increment16reg(registers.h, registers.l); break;

	case 0x22: cycles = 8; memory.Write(Combinebytes(registers.h, registers.l), registers.a); increment16reg(registers.h, registers.l); break;

	case 0xE0: cycles = 12; memory.Write(0xFF00 + memory.Read(registers.pc), registers.a); registers.pc++; break;

	case 0xF0: cycles = 12; registers.a = memory.Read(0xFF00 + memory.Read(registers.pc)); break;

	case 0x01: cycles = LD16(registers.b, registers.c); break;
	case 0x11: cycles = LD16(registers.b, registers.c); break;
	case 0x21: cycles = LD16(registers.b, registers.c); break;
	case 0x31: cycles = LD16(registers.b, registers.c); break;

	case 0xF9: cycles = 8; registers.pc = Combinebytes(registers.h, registers.l); break;

	case 0xF8: cycles = LDHL(); break;

	case 0x08: cycles = 20; memory.Write(memory.Read(registers.pc), registers.sp); registers.pc++; break;

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
	case 0x86: cycles = 8; ADD(memory.Read(Combinebytes(registers.h, registers.l))); break;
	case 0xC6: cycles = 8; ADD(memory.Read(registers.pc)); registers.pc++; break;

	// add with carry
	case 0x8F: cycles = ADDC(registers.a); break;
	case 0x88: cycles = ADDC(registers.b); break;
	case 0x89: cycles = ADDC(registers.c); break;
	case 0x8A: cycles = ADDC(registers.d); break;
	case 0x8B: cycles = ADDC(registers.e); break;
	case 0x8C: cycles = ADDC(registers.h); break;
	case 0x8D: cycles = ADDC(registers.l); break;
	case 0x8E: cycles = 8; ADDC(memory.Read(Combinebytes(registers.h, registers.l))); break;
	case 0xCE: cycles = 8; ADDC(memory.Read(registers.pc)); registers.pc++; break;

	// sub
	case 0x97: cycles = SUB(registers.a); break;
	case 0x90: cycles = SUB(registers.b); break;
	case 0x91: cycles = SUB(registers.c); break;
	case 0x92: cycles = SUB(registers.d); break;
	case 0x93: cycles = SUB(registers.e); break;
	case 0x94: cycles = SUB(registers.h); break;
	case 0x95: cycles = SUB(registers.l); break;
	case 0x96: cycles = 8; SUB(memory.Read(Combinebytes(registers.h, registers.l))); break;
	case 0xD6: cycles = 8; SUB(memory.Read(registers.pc)); registers.pc++; break;

	// sub with carry
	case 0x9F: cycles = SUBC(registers.a); break;
	case 0x98: cycles = SUBC(registers.b); break;
	case 0x99: cycles = SUBC(registers.c); break;
	case 0x9A: cycles = SUBC(registers.d); break;
	case 0x9B: cycles = SUBC(registers.e); break;
	case 0x9C: cycles = SUBC(registers.h); break;
	case 0x9D: cycles = SUBC(registers.l); break;
	case 0x9E: cycles = 8; SUBC(memory.Read(Combinebytes(registers.h, registers.l))); break;
	case 0xDE: cycles = 8; SUBC(memory.Read(registers.pc)); registers.pc++; break;

	// AND
	case 0xA7: cycles = AND(registers.a); break;
	case 0xA0: cycles = AND(registers.b); break;
	case 0xA1: cycles = AND(registers.c); break;
	case 0xA2: cycles = AND(registers.d); break;
	case 0xA3: cycles = AND(registers.e); break;
	case 0xA4: cycles = AND(registers.h); break;
	case 0xA5: cycles = AND(registers.l); break;
	case 0xA6: cycles = 8; AND(Combinebytes(registers.h, registers.l)); break;
	case 0xE6: cycles = 8; AND(memory.Read(registers.pc)); registers.pc++; break;

	// OR
	case 0xB7: cycles = OR(registers.a); break;
	case 0xB0: cycles = OR(registers.b); break;
	case 0xB1: cycles = OR(registers.c); break;
	case 0xB2: cycles = OR(registers.d); break;
	case 0xB3: cycles = OR(registers.e); break;
	case 0xB4: cycles = OR(registers.h); break;
	case 0xB5: cycles = OR(registers.l); break;
	case 0xB6: cycles = 8; OR(Combinebytes(registers.h, registers.l)); break;
	case 0xF6: cycles = 8; OR(memory.Read(registers.pc)); registers.pc++; break;

	// XOR
	case 0xAF: cycles = XOR(registers.a); break;
	case 0xA8: cycles = XOR(registers.b); break;
	case 0xA9: cycles = XOR(registers.c); break;
	case 0xAA: cycles = XOR(registers.d); break;
	case 0xAB: cycles = XOR(registers.e); break;
	case 0xAC: cycles = XOR(registers.h); break;
	case 0xAD: cycles = XOR(registers.l); break;
	case 0xAE: cycles = 8; XOR(Combinebytes(registers.h, registers.l)); break;
	case 0xEE: cycles = 8; XOR(memory.Read(registers.pc)); registers.pc++; break;

	// CP
	case 0xBF: cycles = CP(registers.a); break;
	case 0xB8: cycles = CP(registers.b); break;
	case 0xB9: cycles = CP(registers.c); break;
	case 0xBA: cycles = CP(registers.d); break;
	case 0xBB: cycles = CP(registers.e); break;
	case 0xBC: cycles = CP(registers.h); break;
	case 0xBD: cycles = CP(registers.l); break;
	case 0xBE: cycles = 8; CP(Combinebytes(registers.h, registers.l)); break;
	case 0xFE: cycles = 8; CP(memory.Read(registers.pc)); registers.pc++; break;

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

	// Extended opcodes
	case 0xCB: cycles = ExecuteExtendedOpcode(); break;

	// Miscellaneous
	case 0x27: cycles = DAA(); break;

	case 0x2F: cycles = CPL(); break;

	case 0x3F: cycles = CCF(); break;

	case 0x37: cycles = SCF(); break;

	case 0x76: cycles = HALT(); break;

	case 0x10: cycles = STOP(); break;

	case 0xF3: cycles = DI(); break;

	case 0xFB: cycles = EI(); break;

	// rotates and shifts
	case 0x07: cycles = 4; RLC(registers.a); break;
	case 0x17: cycles = 4; RL(registers.a); break;
	case 0x0F: cycles = 4; RRC(registers.a); break;
	case 0x1F: cycles = 4; RR(registers.a); break;

	// jump
	case 0xC3: cycles = JP(); break;

	// conditional jump
	case 0xC2: cycles = JPcc(condition::NotZero); break;
	case 0xCA: cycles = JPcc(condition::Zero); break;
	case 0xD2: cycles = JPcc(condition::NotCarry); break;
	case 0xDA: cycles = JPcc(condition::Carry); break;

	// jump to hl
	case 0xE9: cycles = JPHL(); break;

	// relative jump
	case 0x18: cycles = JR(); break;

	// conditional relative jump
	case 0x20: cycles = JRcc(condition::NotZero); break;
	case 0x28: cycles = JRcc(condition::Zero); break;
	case 0x30: cycles = JRcc(condition::NotCarry); break;
	case 0x38: cycles = JRcc(condition::Carry); break;

	// call
	case 0xCD: cycles = CALL(); break;

	// conditional call
	case 0xC4: cycles = CALLcc(condition::NotZero); break;
	case 0xCC: cycles = CALLcc(condition::Zero); break;
	case 0xD4: cycles = CALLcc(condition::NotCarry); break;
	case 0xDC: cycles = CALLcc(condition::Carry); break;

	// restart
	case 0xC7: cycles = RST(0x00);  break;
	case 0xCF: cycles = RST(0x08);  break;
	case 0xD7: cycles = RST(0x10);  break;
	case 0xDF: cycles = RST(0x18);  break;
	case 0xE7: cycles = RST(0x20);  break;
	case 0xEF: cycles = RST(0x28);  break;
	case 0xF7: cycles = RST(0x30);  break;
	case 0xFF: cycles = RST(0x38);  break;

	// return
	case 0xC9: cycles = RET(); break;

	// conditional return
	case 0xC0: cycles = RETcc(condition::NotZero); break;
	case 0xC8: cycles = RETcc(condition::Zero); break;
	case 0xD0: cycles = RETcc(condition::NotCarry); break;
	case 0xD8: cycles = RETcc(condition::Carry); break;

	// return and enable interups
	case 0xD9: cycles = RETI(); break;


	default: throw "Unknown Opcode"; break;
	}

	return cycles;
}



word Cpu::ExecuteExtendedOpcode() {

	word cycles = 0;

	byte opcode = memory.Read(registers.pc);
	registers.pc++;

	switch (opcode) {

	// Miscellaneous
	case 0x37: cycles = SWAP(registers.a); break;
	case 0x30: cycles = SWAP(registers.b); break;
	case 0x31: cycles = SWAP(registers.c); break;
	case 0x32: cycles = SWAP(registers.d); break;
	case 0x33: cycles = SWAP(registers.e); break;
	case 0x34: cycles = SWAP(registers.h); break;
	case 0x35: cycles = SWAP(registers.l); break;
	case 0x36: cycles = SWAP(Combinebytes(registers.h, registers.l)); break;

	// rotates and shifts
	// rotate left bit 7 to carry
	case 0x07: cycles = RLC(registers.a); break;
	case 0x00: cycles = RLC(registers.b); break;
	case 0x01: cycles = RLC(registers.c); break;
	case 0x02: cycles = RLC(registers.d); break;
	case 0x03: cycles = RLC(registers.e); break;
	case 0x04: cycles = RLC(registers.h); break;
	case 0x05: cycles = RLC(registers.l); break;
	case 0x06: cycles = RLC(Combinebytes(registers.h, registers.l)); break;

	// rotate left through carry
	case 0x17: cycles = RL(registers.a); break;
	case 0x10: cycles = RL(registers.b); break;
	case 0x11: cycles = RL(registers.c); break;
	case 0x12: cycles = RL(registers.d); break;
	case 0x13: cycles = RL(registers.e); break;
	case 0x14: cycles = RL(registers.h); break;
	case 0x15: cycles = RL(registers.l); break;
	case 0x16: cycles = RL(Combinebytes(registers.h, registers.l)); break;

	// rotate right bit 7 to carry
	case 0x0F: cycles = RRC(registers.a); break;
	case 0x08: cycles = RRC(registers.b); break;
	case 0x09: cycles = RRC(registers.c); break;
	case 0x0A: cycles = RRC(registers.d); break;
	case 0x0B: cycles = RRC(registers.e); break;
	case 0x0C: cycles = RRC(registers.h); break;
	case 0x0D: cycles = RRC(registers.l); break;
	case 0x0E: cycles = RRC(Combinebytes(registers.h, registers.l)); break;

	// rotate right through carry
	case 0x1F: cycles = RR(registers.a); break;
	case 0x18: cycles = RR(registers.b); break;
	case 0x19: cycles = RR(registers.c); break;
	case 0x1A: cycles = RR(registers.d); break;
	case 0x1B: cycles = RR(registers.e); break;
	case 0x1C: cycles = RR(registers.h); break;
	case 0x1D: cycles = RR(registers.l); break;
	case 0x1E: cycles = RR(Combinebytes(registers.h, registers.l)); break;

	// shift left into carry lsb set to 0
	case 0x27: cycles = SLA(registers.a); break;
	case 0x20: cycles = SLA(registers.b); break;
	case 0x21: cycles = SLA(registers.c); break;
	case 0x22: cycles = SLA(registers.d); break;
	case 0x23: cycles = SLA(registers.e); break;
	case 0x24: cycles = SLA(registers.h); break;
	case 0x25: cycles = SLA(registers.l); break;
	case 0x26: cycles = SLA(Combinebytes(registers.h, registers.l)); break;

	// shift right into carry, msb stays the same
	case 0x2F: cycles = SRA(registers.a); break;
	case 0x28: cycles = SRA(registers.b); break;
	case 0x29: cycles = SRA(registers.c); break;
	case 0x2A: cycles = SRA(registers.d); break;
	case 0x2B: cycles = SRA(registers.e); break;
	case 0x2C: cycles = SRA(registers.h); break;
	case 0x2D: cycles = SRA(registers.l); break;
	case 0x2E: cycles = SRA(Combinebytes(registers.h, registers.l)); break;

	// shift right into carry msb set to 0
	case 0x3F: cycles = SRL(registers.a); break;
	case 0x38: cycles = SRL(registers.b); break;
	case 0x39: cycles = SRL(registers.c); break;
	case 0x3A: cycles = SRL(registers.d); break;
	case 0x3B: cycles = SRL(registers.e); break;
	case 0x3C: cycles = SRL(registers.h); break;
	case 0x3D: cycles = SRL(registers.l); break;
	case 0x3E: cycles = SRL(Combinebytes(registers.h, registers.l)); break;

	// check bit and set flags
	case 0x40: cycles = BIT(registers.b, 0); break;
	case 0x41: cycles = BIT(registers.c, 0); break;
	case 0x42: cycles = BIT(registers.d, 0); break;
	case 0x43: cycles = BIT(registers.e, 0); break;
	case 0x44: cycles = BIT(registers.h, 0); break;
	case 0x45: cycles = BIT(registers.l, 0); break;
	case 0x46: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 0); break;
	case 0x47: cycles = BIT(registers.a, 0); break;
	case 0x48: cycles = BIT(registers.b, 1); break;
	case 0x49: cycles = BIT(registers.c, 1); break;
	case 0x4A: cycles = BIT(registers.d, 1); break;
	case 0x4B: cycles = BIT(registers.e, 1); break;
	case 0x4C: cycles = BIT(registers.h, 1); break;
	case 0x4D: cycles = BIT(registers.l, 1); break;
	case 0x4E: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 1); break;
	case 0x4F: cycles = BIT(registers.a, 1); break;
	case 0x50: cycles = BIT(registers.b, 2); break;
	case 0x51: cycles = BIT(registers.c, 2); break;
	case 0x52: cycles = BIT(registers.d, 2); break;
	case 0x53: cycles = BIT(registers.e, 2); break;
	case 0x54: cycles = BIT(registers.h, 2); break;
	case 0x55: cycles = BIT(registers.l, 2); break;
	case 0x56: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 2); break;
	case 0x57: cycles = BIT(registers.a, 2); break;
	case 0x58: cycles = BIT(registers.b, 3); break;
	case 0x59: cycles = BIT(registers.c, 3); break;
	case 0x5A: cycles = BIT(registers.d, 3); break;
	case 0x5B: cycles = BIT(registers.e, 3); break;
	case 0x5C: cycles = BIT(registers.h, 3); break;
	case 0x5D: cycles = BIT(registers.l, 3); break;
	case 0x5E: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 3); break;
	case 0x5F: cycles = BIT(registers.a, 3); break;
	case 0x60: cycles = BIT(registers.b, 4); break;
	case 0x61: cycles = BIT(registers.c, 4); break;
	case 0x62: cycles = BIT(registers.d, 4); break;
	case 0x63: cycles = BIT(registers.e, 4); break;
	case 0x64: cycles = BIT(registers.h, 4); break;
	case 0x65: cycles = BIT(registers.l, 4); break;
	case 0x66: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 4); break;
	case 0x67: cycles = BIT(registers.a, 4); break;
	case 0x68: cycles = BIT(registers.b, 5); break;
	case 0x69: cycles = BIT(registers.c, 5); break;
	case 0x6A: cycles = BIT(registers.d, 5); break;
	case 0x6B: cycles = BIT(registers.e, 5); break;
	case 0x6C: cycles = BIT(registers.h, 5); break;
	case 0x6D: cycles = BIT(registers.l, 5); break;
	case 0x6E: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 5); break;
	case 0x6F: cycles = BIT(registers.a, 5); break;
	case 0x70: cycles = BIT(registers.b, 6); break;
	case 0x71: cycles = BIT(registers.c, 6); break;
	case 0x72: cycles = BIT(registers.d, 6); break;
	case 0x73: cycles = BIT(registers.e, 6); break;
	case 0x74: cycles = BIT(registers.h, 6); break;
	case 0x75: cycles = BIT(registers.l, 6); break;
	case 0x76: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 6); break;
	case 0x77: cycles = BIT(registers.a, 6); break;
	case 0x78: cycles = BIT(registers.b, 7); break;
	case 0x79: cycles = BIT(registers.c, 7); break;
	case 0x7A: cycles = BIT(registers.d, 7); break;
	case 0x7B: cycles = BIT(registers.e, 7); break;
	case 0x7C: cycles = BIT(registers.h, 7); break;
	case 0x7D: cycles = BIT(registers.l, 7); break;
	case 0x7E: cycles = 16; BIT(memory.Read(Combinebytes(registers.h, registers.l)), 7); break;
	case 0x7F: cycles = BIT(registers.a, 7); break;

	// set bit
	case 0xC0: cycles = SET(registers.b, 0); break;
	case 0xC1: cycles = SET(registers.c, 0); break;
	case 0xC2: cycles = SET(registers.d, 0); break;
	case 0xC3: cycles = SET(registers.e, 0); break;
	case 0xC4: cycles = SET(registers.h, 0); break;
	case 0xC5: cycles = SET(registers.b, 0); break;
	case 0xC6: cycles = SET(Combinebytes(registers.h, registers.l), 0); break;
	case 0xC7: cycles = SET(registers.a, 0); break;
	case 0xC8: cycles = SET(registers.b, 1); break;
	case 0xC9: cycles = SET(registers.c, 1); break;
	case 0xCA: cycles = SET(registers.d, 1); break;
	case 0xCB: cycles = SET(registers.e, 1); break;
	case 0xCC: cycles = SET(registers.h, 1); break;
	case 0xCD: cycles = SET(registers.b, 1); break;
	case 0xCE: cycles = SET(Combinebytes(registers.h, registers.l), 1); break;
	case 0xCF: cycles = SET(registers.a, 1); break;
	case 0xD0: cycles = SET(registers.b, 2); break;
	case 0xD1: cycles = SET(registers.c, 2); break;
	case 0xD2: cycles = SET(registers.d, 2); break;
	case 0xD3: cycles = SET(registers.e, 2); break;
	case 0xD4: cycles = SET(registers.h, 2); break;
	case 0xD5: cycles = SET(registers.b, 2); break;
	case 0xD6: cycles = SET(Combinebytes(registers.h, registers.l), 2); break;
	case 0xD7: cycles = SET(registers.a, 2); break;
	case 0xD8: cycles = SET(registers.b, 3); break;
	case 0xD9: cycles = SET(registers.c, 3); break;
	case 0xDA: cycles = SET(registers.d, 3); break;
	case 0xDB: cycles = SET(registers.e, 3); break;
	case 0xDC: cycles = SET(registers.h, 3); break;
	case 0xDD: cycles = SET(registers.b, 3); break;
	case 0xDE: cycles = SET(Combinebytes(registers.h, registers.l), 3); break;
	case 0xDF: cycles = SET(registers.a, 3); break;
	case 0xE0: cycles = SET(registers.b, 4); break;
	case 0xE1: cycles = SET(registers.c, 4); break;
	case 0xE2: cycles = SET(registers.d, 4); break;
	case 0xE3: cycles = SET(registers.e, 4); break;
	case 0xE4: cycles = SET(registers.h, 4); break;
	case 0xE5: cycles = SET(registers.b, 4); break;
	case 0xE6: cycles = SET(Combinebytes(registers.h, registers.l), 4); break;
	case 0xE7: cycles = SET(registers.a, 4); break;
	case 0xE8: cycles = SET(registers.b, 5); break;
	case 0xE9: cycles = SET(registers.c, 5); break;
	case 0xEA: cycles = SET(registers.d, 5); break;
	case 0xEB: cycles = SET(registers.e, 5); break;
	case 0xEC: cycles = SET(registers.h, 5); break;
	case 0xED: cycles = SET(registers.b, 5); break;
	case 0xEE: cycles = SET(Combinebytes(registers.h, registers.l), 5); break;
	case 0xEF: cycles = SET(registers.a, 5); break;
	case 0xF0: cycles = SET(registers.b, 6); break;
	case 0xF1: cycles = SET(registers.c, 6); break;
	case 0xF2: cycles = SET(registers.d, 6); break;
	case 0xF3: cycles = SET(registers.e, 6); break;
	case 0xF4: cycles = SET(registers.h, 6); break;
	case 0xF5: cycles = SET(registers.b, 6); break;
	case 0xF6: cycles = SET(Combinebytes(registers.h, registers.l), 6); break;
	case 0xF7: cycles = SET(registers.a, 6); break;
	case 0xF8: cycles = SET(registers.b, 7); break;
	case 0xF9: cycles = SET(registers.c, 7); break;
	case 0xFA: cycles = SET(registers.d, 7); break;
	case 0xFB: cycles = SET(registers.e, 7); break;
	case 0xFC: cycles = SET(registers.h, 7); break;
	case 0xFD: cycles = SET(registers.b, 7); break;
	case 0xFE: cycles = SET(Combinebytes(registers.h, registers.l), 7); break;
	case 0xFF: cycles = SET(registers.a, 7); break;

	// reset bit
	case 0x80: cycles = RES(registers.b, 0); break;
	case 0x81: cycles = RES(registers.c, 0); break;
	case 0x82: cycles = RES(registers.d, 0); break;
	case 0x83: cycles = RES(registers.e, 0); break;
	case 0x84: cycles = RES(registers.h, 0); break;
	case 0x85: cycles = RES(registers.l, 0); break;
	case 0x86: cycles = RES(Combinebytes(registers.h, registers.l), 0); break;
	case 0x87: cycles = RES(registers.a, 0); break;
	case 0x88: cycles = RES(registers.b, 1); break;
	case 0x89: cycles = RES(registers.c, 1); break;
	case 0x8A: cycles = RES(registers.d, 1); break;
	case 0x8B: cycles = RES(registers.e, 1); break;
	case 0x8C: cycles = RES(registers.h, 1); break;
	case 0x8D: cycles = RES(registers.l, 1); break;
	case 0x8E: cycles = RES(Combinebytes(registers.h, registers.l), 1); break;
	case 0x8F: cycles = RES(registers.a, 1); break;
	case 0x90: cycles = RES(registers.b, 2); break;
	case 0x91: cycles = RES(registers.c, 2); break;
	case 0x92: cycles = RES(registers.d, 2); break;
	case 0x93: cycles = RES(registers.e, 2); break;
	case 0x94: cycles = RES(registers.h, 2); break;
	case 0x95: cycles = RES(registers.l, 2); break;
	case 0x96: cycles = RES(Combinebytes(registers.h, registers.l), 2); break;
	case 0x97: cycles = RES(registers.a, 2); break;
	case 0x98: cycles = RES(registers.b, 3); break;
	case 0x99: cycles = RES(registers.c, 3); break;
	case 0x9A: cycles = RES(registers.d, 3); break;
	case 0x9B: cycles = RES(registers.e, 3); break;
	case 0x9C: cycles = RES(registers.h, 3); break;
	case 0x9D: cycles = RES(registers.l, 3); break;
	case 0x9E: cycles = RES(Combinebytes(registers.h, registers.l), 3); break;
	case 0x9F: cycles = RES(registers.a, 3); break;
	case 0xA0: cycles = RES(registers.b, 4); break;
	case 0xA1: cycles = RES(registers.c, 4); break;
	case 0xA2: cycles = RES(registers.d, 4); break;
	case 0xA3: cycles = RES(registers.e, 4); break;
	case 0xA4: cycles = RES(registers.h, 4); break;
	case 0xA5: cycles = RES(registers.l, 4); break;
	case 0xA6: cycles = RES(Combinebytes(registers.h, registers.l), 4); break;
	case 0xA7: cycles = RES(registers.a, 4); break;
	case 0xA8: cycles = RES(registers.b, 5); break;
	case 0xA9: cycles = RES(registers.c, 5); break;
	case 0xAA: cycles = RES(registers.d, 5); break;
	case 0xAB: cycles = RES(registers.e, 5); break;
	case 0xAC: cycles = RES(registers.h, 5); break;
	case 0xAD: cycles = RES(registers.l, 5); break;
	case 0xAE: cycles = RES(Combinebytes(registers.h, registers.l), 5); break;
	case 0xAF: cycles = RES(registers.a, 5); break;
	case 0xB0: cycles = RES(registers.b, 6); break;
	case 0xB1: cycles = RES(registers.c, 6); break;
	case 0xB2: cycles = RES(registers.d, 6); break;
	case 0xB3: cycles = RES(registers.e, 6); break;
	case 0xB4: cycles = RES(registers.h, 6); break;
	case 0xB5: cycles = RES(registers.l, 6); break;
	case 0xB6: cycles = RES(Combinebytes(registers.h, registers.l), 6); break;
	case 0xB7: cycles = RES(registers.a, 6); break;
	case 0xB8: cycles = RES(registers.b, 7); break;
	case 0xB9: cycles = RES(registers.c, 7); break;
	case 0xBA: cycles = RES(registers.d, 7); break;
	case 0xBB: cycles = RES(registers.e, 7); break;
	case 0xBC: cycles = RES(registers.h, 7); break;
	case 0xBD: cycles = RES(registers.l, 7); break;
	case 0xBE: cycles = RES(Combinebytes(registers.h, registers.l), 7); break;
	case 0xBF: cycles = RES(registers.a, 7); break;

	default: throw "Unknown extended Opcode"; break;
	}
	return cycles;
}
