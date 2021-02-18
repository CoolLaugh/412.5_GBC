#include "cpu.h"

bool CPU::flagTest(flagType flag) {

	byte result = registers.f & flag;
	if (result == 0) {
		return false;
	}
	else {
		return true;
	}
}

void CPU::flagSet(flagType flag) {
	
	registers.f |= flag;
}

void CPU::flagSet(flagType flag, bool value) {

	if (value == true) {
		flagSet(flag);
	}
	else {
		flagReset(flag);
	}

}

void CPU::flagReset(flagType flag) {

	registers.f &= ~flag;
}

bool CPU::bitTest(byte value, Bits bit) {

	return (value & bit) != 0;
}

void CPU::bitSet(byte& value, Bits bit) {

	value |= bit;
}

void CPU::bitReset(byte& value, Bits bit) {

	value &= ~bit;
}

void CPU::setInterrupt(interruptFlags flag) {
	
	byte interuptFlags = memory->Read(Address::InteruptFlag);
	interuptFlags |= (1 << flag);
	memory->Write(Address::InteruptFlag, interuptFlags);
}

void CPU::PowerUpSequence() {

	// 2.7.1
	if (ColorGameBoyMode == false) {
		registers.a = 0x01;
	}
	else {
		registers.a = 0x11;
	}
	registers.f = 0xB0;
	registers.b = 0x00;
	registers.c = 0x13;
	registers.d = 0x00;
	registers.e = 0xD8;
	registers.h = 0x01;
	registers.l = 0x4D;
	registers.sp = 0xFFFE;
	registers.pc = 0x100; // skips the nintendo logo check

	memory->PowerUpSequence();

	outputStateBuffer.reserve(11000000);
}



// load immediate value into register
word CPU::LD(byte & reg) {

	reg = memory->Read(registers.pc);
	registers.pc++;
	AdvanceClocks(8);
	return 8; // cycles
}

// load value from one register into another
word CPU::LDreg(byte & reg1, byte & reg2) {

	reg1 = reg2;

	AdvanceClocks(4);
	return 4; // cycles
}

// load value from memory address
word CPU::LDRegFromMemory(byte & reg, word address) {

	reg = memory->Read(address);

	AdvanceClocks(8);
	return 8; // cycles
}

word CPU::LDregHL(byte & reg) {

	word address = Combinebytes(registers.l, registers.h);
	reg = memory->Read(address);

	AdvanceClocks(8);
	return 8; // cycles
}

// put stackpointer plus immediate value into hl
word CPU::LDHL() {

	signed char n = memory->Read(registers.pc);
	word hl = registers.sp + n;
	registers.pc++;
	std::pair<byte, byte> pair = splitBytes(hl);
	registers.l = pair.first;
	registers.h = pair.second;

	flagReset(flagType::zero);
	flagReset(flagType::negative);
	halfCarryFlag16(registers.sp, (word)n);
	carryFlag16(registers.sp, (word)n);

	AdvanceClocks(12);
	return 12; // cycles
}

word CPU::WriteSP() {

	word address = Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1));
	registers.pc += 2;

	std::pair<byte, byte> pair = splitBytes(registers.sp);
	memory->Write(address, pair.first);
	memory->Write(address + 1, pair.second);

	AdvanceClocks(20);
	return 20;
}

// load immediate 16-bit value into register pair
word CPU::LD16(byte & reg1, byte & reg2) {

	reg1 = memory->Read(registers.pc + 1);
	reg2 = memory->Read(registers.pc);
	registers.pc += 2;
	AdvanceClocks(12);
	return 12;
}

word CPU::LD16(word & reg1) {

	reg1 = Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1));
	registers.pc += 2;

	AdvanceClocks(12);
	return 12;
}

// push two bytes onto the stack from a register pair
word CPU::Push(byte & reg1, byte & reg2) {

	registers.sp--;
	memory->Write(registers.sp, reg1);
	registers.sp--;
	memory->Write(registers.sp, reg2);

	AdvanceClocks(16);
	return 16;
}

// pop two bytes off the stack into a register pair
word CPU::Pop(byte & reg1, byte & reg2) {

	reg2 = memory->Read(registers.sp);
	registers.sp++;
	reg1 = memory->Read(registers.sp);
	registers.sp++;

	AdvanceClocks(12);
	return 12;
}

// indicates if the carry flag is set after addition from bit 3
void CPU::halfCarryFlag(byte value1, byte value2, bool carry) {

	value1 &= 0xF;
	value2 &= 0xF;

	if (carry == true) value2++;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xF);
}

// indicates if the carry flag is set after addition from bit 11
void CPU::halfCarryFlag16(word value1, word value2) {

	value1 &= 0xF;
	value2 &= 0xF;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xF);
}

void CPU::halfCarryFlag16Hi(word value1, word value2) {

	value1 &= 0xFFF;
	value2 &= 0xFFF;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xFFF);
}

// indicates if the carry flag is set after addition from bit 7
void CPU::carryFlag(byte value1, byte value2, bool carry) {

	word Wvalue1 = value1;
	word Wvalue2 = value2;

	if (carry == true) Wvalue2++;

	flagSet(flagType::carry, (Wvalue1 + Wvalue2) > 0xFF);
}

// indicates if the carry flag is set after 16-bit addition from bit 15
void CPU::carryFlag16(word value1, word value2) {

	value1 &= 0xFF;
	value2 &= 0xFF;

	flagSet(flagType::carry, (value1 + value2) > 0xFF);
}

void CPU::carryFlag16Hi(word value1, word value2) {

	int result = value1;
	result += value2;

	flagSet(flagType::carry, result > 0xFFFF);
}

// indicates if a bit is borrowed from bit 4 after a subtraction
void CPU::halfNoBorrow(byte value1, byte value2, bool carry) {

	value1 &= 0xF;
	value2 &= 0xF;
	if (carry == true) value2++;

	flagSet(flagType::halfCarry, value1 < value2);
}

// indicates if a bit is borrowed after a subtraction
void CPU::noBorrow(byte value1, byte value2, bool carry) {

	word Wvalue1 = value1;
	word Wvalue2 = value2;

	if (carry == true) Wvalue2++;
	flagSet(flagType::carry, Wvalue1 < Wvalue2);
}

void CPU::zeroFlag(byte val) {

	flagSet(flagType::zero, val == 0);
}

// add value to a, ignore carry
word CPU::ADD(byte value, int clocks) {

	byte result = registers.a + value;
	
	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(registers.a, value, false);
	carryFlag(registers.a, value, false);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks; // sometimes add is 8 cycles, check the manual
}

// add value to a, include carry
word CPU::ADDC(byte value, int clocks) {

	bool carry = flagTest(flagType::carry);

	byte valueWithCarry = value;

	if (carry == true) {
		valueWithCarry = value + 1;
	}

	byte result = registers.a + valueWithCarry;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(registers.a, value, carry);
	carryFlag(registers.a, value, carry);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks;
}

// subtract value from a, ignore carry
word CPU::SUB(byte value, int clocks) {

	byte result = registers.a - value;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value, false);
	noBorrow(registers.a, value, false);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks;
}

// subtract value from a, include carry
word CPU::SUBC(byte value, int clocks) {

	bool carry = flagTest(flagType::carry);

	byte valueWithCarry = value;

	if (carry == true) {
		valueWithCarry = value + 1;
	}

	byte result = registers.a - valueWithCarry;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value, carry);
	noBorrow(registers.a, value, carry);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks;
}

// and value with register a
word CPU::AND(byte value, int clocks) {

	byte result = registers.a & value;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagSet(flagType::halfCarry);
	flagReset(flagType::carry);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks;
}

// or value with register a
word CPU::OR(byte value, int clocks) {

	byte result = registers.a | value;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks;
}

// xor value with register a
word CPU::XOR(byte value, int clocks) {

	byte result = registers.a ^ value;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	registers.a = result;

	AdvanceClocks(clocks);
	return clocks;
}

// compare value to register a and set flags accordingly
word CPU::CP(byte value, int clocks) {

	byte result = registers.a - value;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value, false);
	noBorrow(registers.a, value, false);

	AdvanceClocks(clocks);
	return clocks;
}

// increment register
word CPU::INC(byte& reg) {

	byte result = reg + 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(reg, 1, false);

	reg = result;

	AdvanceClocks(4);
	return 4;
}

// increment memory location pointed to by register hl
word CPU::INCMemory(word address) {

	byte value = memory->Read(address);
	AdvanceClocks(4);
	byte result = value + 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(value, 1, false);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 12;
}

// decrement register
word CPU::DEC(byte& reg) {

	byte result = reg - 1;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(reg, 1, false);

	reg = result;

	AdvanceClocks(4);
	return 4;
}

// decrement memory location pointed to by register hl
word CPU::DECMemory(word address) {

	byte value = memory->Read(address);
	AdvanceClocks(4);
	byte result = value - 1;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(value, 1, false);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 12; 
}

// add value to register combination hl
word CPU::ADDHL(word value) {

	word hl = Combinebytes(registers.l, registers.h);
	word result = hl + value;

	flagReset(flagType::negative);
	halfCarryFlag16Hi(hl, value);
	carryFlag16Hi(hl, value);

	std::pair<byte, byte> pair = splitBytes(result);

	registers.l = pair.first;
	registers.h = pair.second;

	AdvanceClocks(8);
	return 8;
}

// add immediate signed value to stack pointer
word CPU::ADDSP() {

	signed char n = memory->Read(registers.pc);
	registers.pc++;

	word result = registers.sp + n;

	flagReset(flagType::zero);
	flagReset(flagType::negative);
	halfCarryFlag16(registers.sp, n);
	carryFlag16(registers.sp, n);;

	registers.sp = result;

	AdvanceClocks(16);
	return 16;
}

// increment two registers as if they were one 16 bit register
word CPU::INC16(byte & reg1, byte & reg2) {

	word combinedRegister = Combinebytes(reg1, reg2);

	combinedRegister++;

	std::pair<byte, byte> pair = splitBytes(combinedRegister);

	reg1 = pair.first;
	reg2 = pair.second;

	AdvanceClocks(8);
	return 8;
}

// increment 16 bit register
word CPU::INC16(word & reg) {

	reg++;

	AdvanceClocks(8);
	return 8;
}

// decrement two registers as if they were one 16 bit register
word CPU::DEC16(byte & reg1, byte & reg2) {

	word combinedRegister = Combinebytes(reg1, reg2);

	combinedRegister--;

	std::pair<byte, byte> pair = splitBytes(combinedRegister);

	reg1 = pair.first;
	reg2 = pair.second;

	AdvanceClocks(8);
	return 8;
}


// decrement 16 bit register
word CPU::DEC16(word & reg) {

	reg--;

	AdvanceClocks(8);
	return 8;
}

// swap the lower and upper nibbles
word CPU::SWAP(byte & reg) {

	byte upper = reg & 0xF0;
	byte lower = reg & 0x0F;

	upper >>= 4;
	lower <<= 4;

	byte result = upper | lower;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	reg = result;

	AdvanceClocks(8);
	return 8;
}

// swap the lower and upper nibbles (for memory)
word CPU::SWAP(word address) {

	AdvanceClocks(4);
	byte data = memory->Read(address);
	AdvanceClocks(4);

	byte upper = data & 0xF0;
	byte lower = data & 0x0F;

	upper >>= 4;
	lower <<= 4;

	byte result = upper | lower;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// swap the lower and upper nibble of a byte in memory
word CPU::SWAPMemory() {

	word address = Combinebytes(registers.l, registers.h);

	byte data = memory->Read(address);

	SWAP(data);

	memory->Write(address, data);

	AdvanceClocks(16);
	return 16;
}

// decimal adjust register A
// copied https://www.reddit.com/r/EmuDev/comments/cdtuyw/gameboy_emulator_fails_blargg_daa_test/etwcyvy/
word CPU::DAA() {

	if (flagTest(flagType::negative) == false) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
		if (flagTest(flagType::carry) || registers.a > 0x99) {
			registers.a += 0x60;
			flagSet(flagType::carry);
		}
		if (flagTest(flagType::halfCarry) || (registers.a & 0x0f) > 0x09) {
			registers.a += 0x6;
		}
	}
	else {  // after a subtraction, only adjust if (half-)carry occurred
		if (flagTest(flagType::carry)) {
			registers.a -= 0x60;
			flagSet(flagType::carry);
		}
		if (flagTest(flagType::halfCarry)) {
			registers.a -= 0x6;
		}
	}
	// these flags are always updated
	zeroFlag(registers.a);
	flagReset(flagType::halfCarry);

	AdvanceClocks(4);
	return 4; // cycles
}

// complement A register
word CPU::CPL() {

	registers.a = ~registers.a;

	flagSet(flagType::negative);
	flagSet(flagType::halfCarry);

	AdvanceClocks(4);
	return 4;
}

// complement carry flag
word CPU::CCF() {

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);

	flagSet(flagType::carry, !flagTest(flagType::carry));

	AdvanceClocks(4);
	return 4;
}

// set carry flag
word CPU::SCF() {

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry);

	AdvanceClocks(4);
	return 4;
}

// no operation
word CPU::NOP() {
	// does nothing
	AdvanceClocks(4);
	return 4;
}

// power down cpu until an interrupt occurs
word CPU::HALT() {

	halted = true;
	AdvanceClocks(4);
	return 4;
}

// halt cpu and display until button push
word CPU::STOP() {

	memory->dividerRegister = 0;

	registers.pc++;

	if (ColorGameBoyMode == true) {

		byte speedSwitch = memory->Read(Address::PrepareSpeedSwitch);
		if ((speedSwitch & Bits::b0) != 0) {
			if (speedMode == 1) {
				speedMode = 2;
				bitSet(speedSwitch, Bits::b7);
			}
			else {
				speedMode = 1;
				bitReset(speedSwitch, Bits::b7);
			}
			speedSwitch &= ~Bits::b0;
			memory->Write(Address::PrepareSpeedSwitch, speedSwitch);
		}
	}
	else {

		stop = true;
	}

	AdvanceClocks(4);
	return 4;
}

// disable interupts
word CPU::DI() {
	interupt = false;
	interuptEnable = false;
	AdvanceClocks(4);
	return 4;
}

// enable interupts
// EI has a one cycle delay but DI and RETI do not
word CPU::EI() {
	interuptEnable = true;
	AdvanceClocks(4);
	return 4;
}

// rotate left bit 7 to carry
word CPU::RLC(byte& reg, bool isRegisterA, int clocks) {

	byte result = reg;

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	if (bit7 == true) {
		bitSet(result, Bits::b0);
	}

	if (isRegisterA == true) {
		flagReset(flagType::zero);
	}
	else {
		zeroFlag(result);
	}

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	reg = result;

	AdvanceClocks(clocks);
	return clocks;
}

// rotate left bit 7 to carry (for memory)
word CPU::RLC(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	if (bit7 == true) {
		bitSet(result, Bits::b0);
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// rotate left through carry
word CPU::RL(byte & reg, bool isRegisterA, int clocks) {

	byte result = reg;

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	if (flagTest(flagType::carry) == true) {
		bitSet(result, Bits::b0);
	}

	if (isRegisterA == true) {
		flagReset(flagType::zero);
	}
	else {
		zeroFlag(result);
	}

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	reg = result;

	AdvanceClocks(clocks);
	return clocks;
}

// rotate left through carry (for memory)
word CPU::RL(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	if (flagTest(flagType::carry) == true) {
		bitSet(result, Bits::b0);
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// rotate right bit 7 to carry
word CPU::RRC(byte & reg, bool isRegisterA, int clocks) {

	byte result = reg;

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (bit0 == true) {
		bitSet(result, Bits::b7);
	}

	if (isRegisterA == true) {
		flagReset(flagType::zero);
	}
	else {
		zeroFlag(result);
	}

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	AdvanceClocks(clocks);
	return clocks;
}

// rotate right bit 7 to carry (for memory)
word CPU::RRC(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (bit0 == true) {
		bitSet(result, Bits::b7);
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// rotate right through carry
word CPU::RR(byte & reg, bool isRegisterA, int clocks) {

	byte result = reg;

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (flagTest(flagType::carry) == true) {
		bitSet(result, Bits::b7);
	}

	if (isRegisterA == true) {
		flagReset(flagType::zero);
	}
	else {
		zeroFlag(result);
	}

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	AdvanceClocks(clocks);
	return clocks;
}

// rotate right through carry (for memory)
word CPU::RR(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (flagTest(flagType::carry) == true) {
		result |= 0x80;
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// shift left into carry lsb set to 0
word CPU::SLA(byte & reg) {

	byte result = reg;

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	bitReset(result, Bits::b0);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	reg = result;

	AdvanceClocks(8);
	return 8;
}

// shift left into carry lsb set to 0 (for memory)
word CPU::SLA(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	bitReset(result, Bits::b0);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// shift right into carry, msb stays the same
word CPU::SRA(byte & reg) {

	byte result = reg;

	bool bit7 = bitTest(result, Bits::b7);
	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (bit7 == true) {
		bitSet(result, Bits::b7); // keep MSB the same
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	AdvanceClocks(8);
	return 8;
}

// shift right into carry, msb stays the same (for memory)
word CPU::SRA(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit7 = bitTest(result, Bits::b7);
	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (bit7 == true) {
		bitSet(result, Bits::b7); // keep MSB the same
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// shift right into carry msb set to 0
word CPU::SRL(byte & reg) {

	byte result = reg;

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	bitReset(result, Bits::b7);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	AdvanceClocks(8);
	return 8;
}

// shift right into carry msb set to 0 (for memory)
word CPU::SRL(word address) {

	AdvanceClocks(4);
	byte result = memory->Read(address);
	AdvanceClocks(4);

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	bitReset(result, Bits::b7);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory->Write(address, result);

	AdvanceClocks(8);
	return 16;
}

// check bit and set flags
word CPU::BIT(byte reg, byte bit) {

	byte bitmask = 1 << bit;

	flagSet(flagType::zero, (reg & bitmask) == 0);

	flagReset(flagType::negative);
	flagSet(flagType::halfCarry);

	AdvanceClocks(8);
	return 8;
}

word CPU::BITHL(byte bit) {

	AdvanceClocks(4);
	byte memVal = memory->Read(Combinebytes(registers.l, registers.h));

	return BIT(memVal, bit);
}

// set bit
word CPU::SET(byte& reg, byte bit) {

	byte bitmask = 1 << bit;

	reg |= bitmask;

	AdvanceClocks(8);
	return 8;
}

// set bit
word CPU::SET(word address, byte bit) {

	byte bitmask = 1 << bit;

	AdvanceClocks(4);
	byte data = memory->Read(address);
	AdvanceClocks(4);

	data |= bitmask;

	memory->Write(address, data);

	AdvanceClocks(8);
	return 16;
}

// reset bit
word CPU::RES(byte& reg, byte bit) {

	byte bitmask = 1 << bit;
	bitmask = ~bitmask;

	reg &= bitmask;

	AdvanceClocks(8);
	return 8;
}

// reset bit
word CPU::RES(word address, byte bit) {

	byte bitmask = 1 << bit;
	bitmask = ~bitmask;

	AdvanceClocks(4);
	byte data = memory->Read(address);
	AdvanceClocks(4);

	data &= bitmask;

	memory->Write(address, data);

	AdvanceClocks(8);
	return 16;
}

// jump
word CPU::JP() {

	word address = Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1));
	registers.pc = address;

	AdvanceClocks(16);
	return 16;
}

// conditional jump
word CPU::JPcc(condition cdn) {

	word address = Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1));
	registers.pc += 2;

	word cycles = 12;

	switch (cdn) {
	case CPU::NotZero:	if (flagTest(flagType::zero) == false) { registers.pc = address; cycles = 16; } break;
	case CPU::Zero:		if (flagTest(flagType::zero) == true) { registers.pc = address; cycles = 16; } break;
	case CPU::NotCarry: if (flagTest(flagType::carry) == false) { registers.pc = address; cycles = 16; } break;
	case CPU::Carry:	if (flagTest(flagType::carry) == true) { registers.pc = address; cycles = 16; } break;
	}

	AdvanceClocks(cycles);
	return cycles;
}

// jump to hl
word CPU::JPHL() {

	word address = Combinebytes(registers.l, registers.h);
	registers.pc = address;

	AdvanceClocks(4);
	return 4;
}

// relative jump
word CPU::JR() {

	signed char offset = (signed char)memory->Read(registers.pc);
	registers.pc++;
	registers.pc += offset;

	AdvanceClocks(12);
	return 12;
}

// conditional relative jump
word CPU::JRcc(condition cdn) {

	signed char offset = (signed char)memory->Read(registers.pc);
	registers.pc++;

	word cycles = 8;

	switch (cdn) {
	case CPU::NotZero:	if (flagTest(flagType::zero) == false) { registers.pc += offset; cycles = 12; } break;
	case CPU::Zero:		if (flagTest(flagType::zero) == true) { registers.pc += offset; cycles = 12; } break;
	case CPU::NotCarry: if (flagTest(flagType::carry) == false) { registers.pc += offset; cycles = 12; } break;
	case CPU::Carry:	if (flagTest(flagType::carry) == true) { registers.pc += offset; cycles = 12; } break;
	}

	AdvanceClocks(cycles);
	return cycles;
}

// call
word CPU::CALL() {

	word address = Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1));

	std::pair<byte, byte> pair = splitBytes(registers.pc + 2);
	registers.sp--;
	memory->Write(registers.sp, pair.second);
	registers.sp--;
	memory->Write(registers.sp, pair.first);

	registers.pc = address;

	AdvanceClocks(24);
	return 24;
}

// conditional call
word CPU::CALLcc(condition cdn) {

	word address = Combinebytes(memory->Read(registers.pc), memory->Read(registers.pc + 1));

	word cycles = 12;

	bool call = false;
	switch (cdn) {
	case CPU::NotZero:	if (flagTest(flagType::zero) == false)	call = true; break;
	case CPU::Zero:		if (flagTest(flagType::zero) == true)	call = true; break;
	case CPU::NotCarry: if (flagTest(flagType::carry) == false)	call = true; break;
	case CPU::Carry:	if (flagTest(flagType::carry) == true)	call = true; break;
	}

	if (call == true) {

		std::pair<byte, byte> pair = splitBytes(registers.pc + 2);
		registers.sp--;
		memory->Write(registers.sp, pair.second);
		registers.sp--;
		memory->Write(registers.sp, pair.first);

		registers.pc = address;
		cycles = 24;
	}
	else {
		registers.pc += 2;
	}

	AdvanceClocks(cycles);
	return cycles;
}

// restart
word CPU::RST(short offset) {

	std::pair<byte, byte> pair = splitBytes(registers.pc);
	registers.sp--;
	memory->Write(registers.sp, pair.second);
	registers.sp--;
	memory->Write(registers.sp, pair.first);

	registers.pc = offset;

	AdvanceClocks(16);
	return 16;
}

// return
word CPU::RET() {

	byte first = memory->Read(registers.sp);
	registers.sp++;
	byte second = memory->Read(registers.sp);
	registers.sp++;

	registers.pc = Combinebytes(first, second);

	AdvanceClocks(16);
	return 16;
}

// conditional return
word CPU::RETcc(condition cdn) {

	word cycles = 8;

	bool ret = false;
	switch (cdn) {
	case CPU::NotZero:	if (flagTest(flagType::zero) == false)	ret = true; break;
	case CPU::Zero:		if (flagTest(flagType::zero) == true)	ret = true; break;
	case CPU::NotCarry: if (flagTest(flagType::carry) == false)	ret = true; break;
	case CPU::Carry:	if (flagTest(flagType::carry) == true)	ret = true; break;
	}

	if (ret == true) {

		byte first = memory->Read(registers.sp);
		registers.sp++;
		byte second = memory->Read(registers.sp);
		registers.sp++;

		registers.pc = Combinebytes(first, second);
		cycles = 20;
	}

	AdvanceClocks(cycles);
	return cycles;
}

// return and enable interrupts
word CPU::RETI() {

	byte first = memory->Read(registers.sp);
	registers.sp++;
	byte second = memory->Read(registers.sp);
	registers.sp++;

	registers.pc = Combinebytes(first, second);

	interupt = true;

	AdvanceClocks(16);
	return 16;
}

// decrement two registers as one 16 bit registers
void CPU::decrement16reg(byte & reg1, byte & reg2) {

	word combined = Combinebytes(reg2, reg1);
	combined--;
	reg2 = (combined & 0xFF);
	reg1 = (combined >> 8);
}

// increment two registers as one 16 bit registers
void CPU::increment16reg(byte & reg1, byte & reg2) {

	word combined = Combinebytes(reg2, reg1);
	combined++;
	reg2 = (combined & 0xFF);
	reg1 = (combined >> 8);
}

// combine multiple registers into word
word CPU::Combinebytes(byte value1, byte value2) {

	word combine = value2;
	combine <<= 8;
	combine |= value1;

	return combine;
}

// seperate word into seperate bytes for multiple registers
std::pair<byte, byte> CPU::splitBytes(word value) {

	byte first = value & 0xFF;
	byte second = (value >> 8) & 0xFF;

	return std::make_pair(first, second);
}

word CPU::CombinebytesR(byte value1, byte value2) {

	word combine = value1;
	combine <<= 8;
	combine |= value2;

	return combine;
}

std::pair<byte, byte> CPU::splitBytesR(word value) {

	byte first = (value >> 8) & 0xFF;
	byte second = value & 0xFF;

	return std::make_pair(first, second);
}

void CPU::cleanOutputState() {

	std::ofstream out;
	out.open("CPUStateLog.txt", std::ios::out | std::ios::trunc);
	out.close();
}

void CPU::outputState() {

	if (logState == false) {
		return;
	}

	std::string hex[16] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };

	outputStateBuffer += "AF: " + hex[(registers.a & 0xF0) >> 4] + hex[(registers.a & 0x0F)];
	outputStateBuffer += hex[(registers.f & 0xF0) >> 4] + hex[(registers.f & 0x0F)] + " ";
	outputStateBuffer += "BC: " + hex[(registers.b & 0xF0) >> 4] + hex[(registers.b & 0x0F)];
	outputStateBuffer += hex[(registers.c & 0xF0) >> 4] + hex[(registers.c & 0x0F)] + " ";
	outputStateBuffer += "DE: " + hex[(registers.d & 0xF0) >> 4] + hex[(registers.d & 0x0F)];
	outputStateBuffer += hex[(registers.e & 0xF0) >> 4] + hex[(registers.e & 0x0F)] + " ";
	outputStateBuffer += "HL: " + hex[(registers.h & 0xF0) >> 4] + hex[(registers.h & 0x0F)];
	outputStateBuffer += hex[(registers.l & 0xF0) >> 4] + hex[(registers.l & 0x0F)] + " ";

	word HLAddress = Combinebytes(registers.l, registers.h);
	outputStateBuffer += "(HL+-1): (";
	outputStateBuffer += hex[(memory->Read(HLAddress - 1) & 0xF0) >> 4] + hex[(memory->Read(HLAddress - 1) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory->Read(HLAddress) & 0xF0) >> 4] + hex[(memory->Read(HLAddress) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory->Read(HLAddress + 1) & 0xF0) >> 4] + hex[(memory->Read(HLAddress + 1) & 0x0F)];
	outputStateBuffer += ") ";

	outputStateBuffer += "SP: " + hex[(registers.sp & 0xF000) >> 12] + hex[(registers.sp & 0x0F00) >> 8];
	outputStateBuffer += hex[(registers.sp & 0x00F0) >> 4] + hex[(registers.sp & 0x000F)] + " ";

	outputStateBuffer += "DIV: " + hex[(memory->dividerRegister & 0xF000) >> 12] + hex[(memory->dividerRegister & 0x0F00) >> 8];
	outputStateBuffer += hex[(memory->dividerRegister & 0x00F0) >> 4] + hex[(memory->dividerRegister & 0x000F)] + " ";

	outputStateBuffer += "TIMA: " + hex[(memory->Read(Address::Timer) & 0xF0) >> 4] + hex[(memory->Read(Address::Timer) & 0x0F)] + " ";

	outputStateBuffer += "PC: " + hex[(registers.pc & 0xF000) >> 12] + hex[(registers.pc & 0x0F00) >> 8];
	outputStateBuffer += hex[(registers.pc & 0x00F0) >> 4] + hex[(registers.pc & 0x000F)] + " ";

	outputStateBuffer += "(PC): (";
	outputStateBuffer += hex[(memory->Read(registers.pc) & 0xF0) >> 4] + hex[(memory->Read(registers.pc) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory->Read(registers.pc + 1) & 0xF0) >> 4] + hex[(memory->Read(registers.pc + 1) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory->Read(registers.pc + 2) & 0xF0) >> 4] + hex[(memory->Read(registers.pc + 2) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory->Read(registers.pc + 3) & 0xF0) >> 4] + hex[(memory->Read(registers.pc + 3) & 0x0F)];
	outputStateBuffer += ") ";

	outputStateBuffer += OpcodeNames[memory->Read(registers.pc)];
	outputStateBufferOpcodesOnly += OpcodeNames[memory->Read(registers.pc)];

	if (memory->Read(registers.pc) == 0xCB) {
		outputStateBuffer += ExtendedOpcodeNames[memory->Read(registers.pc + 1)];
		outputStateBufferOpcodesOnly += ExtendedOpcodeNames[memory->Read(registers.pc + 1)];
	}
	outputStateBuffer += "\n";
	outputStateBufferOpcodesOnly += "\n";

	if (outputStateBuffer.size() > 10000000) {

		std::ofstream out;
		out.open("CPUStateLog.txt", std::ios::out | std::ios::app);
		out << outputStateBuffer.c_str();
		outputStateBuffer.clear();
		out.close();

		std::ofstream out2;
		out2.open("CPUStateLogOpcodesOnly.txt", std::ios::out | std::ios::app);
		out2 << outputStateBufferOpcodesOnly.c_str();
		outputStateBufferOpcodesOnly.clear();
		out2.close();
	}

}

int CPU::performInterupts() {

	int clocks = 0;

	if (interuptEnable == true && interuptEnableInstructionCount == 0) {
		// interupts enable is delayed by one instruction, DI and RETI are not
		interuptEnableInstructionCount++;
	}
	else if (interuptEnable == true && interuptEnableInstructionCount >= 1) {
		interuptEnableInstructionCount = 0;
		interuptEnable = false;
		interupt = true;
	}


	byte interuptFlags = memory->Read(Address::InteruptFlag);
	byte interuptsEnabled = memory->Read(Address::InteruptEnable);

	if (interuptFlags != 0 && interuptsEnabled != 0) {
		halted = false;
	}

	if (interupt == true) {

		for (size_t i = 0; i < interruptFlags::END; i++) {

			bool interuptFlag = (interuptFlags & (1 << i)) != 0;
			bool interuptEnabled = (interuptsEnabled & (1 << i)) != 0;

			if (interuptFlag == true && interuptEnabled == true) {

				std::pair<byte, byte> pair = splitBytes(registers.pc);
				Push(pair.second, pair.first);

				switch (i) {
				case 0: registers.pc = 0x40; break;
				case 1: registers.pc = 0x48; break;
				case 2: registers.pc = 0x50; break;
				case 3: registers.pc = 0x58; break;
				case 4: registers.pc = 0x60; break;
				}

				clocks = 20;
				if (halted == true) {
					clocks += 4;
				}

				interuptFlags &= ~(1 << i); // reset bit of interupt
				memory->Write(Address::InteruptFlag, interuptFlags);
				interupt = false;
				halted = false;

				break; // don't check interupts of lower prioriry
			}
		}
	}
	AdvanceClocks(clocks);
	return clocks;
}



void CPU::LCDStatusRegister(word& cyclesThisLine) {

	byte LCDC = memory->Read(Address::LCDC);
	byte LCDCStatus = memory->Read(Address::LCDCStatus);

	if (bitTest(LCDC, Bits::b7) == false) {

		memory->memorySpace[Address::LY] = 0;
		cyclesThisLine = 0;
		LCDCStatus &= ~0x03;
	}
	else {

		byte LY = memory->Read(Address::LY);
		byte mode = LCDCStatus & 0x3;

		if (LY >= 144 && mode != 0x1) { // mode 1

			bitSet(LCDCStatus, Bits::b0);
			bitReset(LCDCStatus, Bits::b1);
			if (bitTest(LCDCStatus, Bits::b4) == true) {
				setInterrupt(interruptFlags::LCDSTAT);
			}
		}
		else if ((cyclesThisLine < 80) && mode != 0x2) { // mode 2

			bitReset(LCDCStatus, Bits::b0);
			bitSet(LCDCStatus, Bits::b1);
			if (bitTest(LCDCStatus, Bits::b5) == true) {
				setInterrupt(interruptFlags::LCDSTAT);
			}
		}
		else if ((cyclesThisLine < (80 + 168)) && mode != 0x3) { // mode 3

			bitSet(LCDCStatus, Bits::b0);
			bitSet(LCDCStatus, Bits::b1);
		}
		else if (mode != 0x0) { // mode 0

			bitReset(LCDCStatus, Bits::b0);
			bitReset(LCDCStatus, Bits::b1);
			if (bitTest(LCDCStatus, Bits::b3) == true) {
				setInterrupt(interruptFlags::LCDSTAT);
			}
		}
	}

	byte LYCompare = memory->Read(Address::LYCompare);
	byte LY = memory->Read(Address::LY);
	int lastLine = LY - 1;
	if (lastLine < 0) {
		lastLine = 153;
	}

	if (LY == LYCompare && lastLY == lastLine) {
		BitSet(LCDCStatus, 2);
		if (BitTest(LCDCStatus, 6) == true) {
			byte interuptFlags = memory->Read(Address::InteruptFlag);
			interuptFlags |= (1 << 1);
			memory->Write(Address::InteruptFlag, interuptFlags);
		}
	}
	else {
		BitReset(LCDCStatus, 2);
	}
	lastLY = LY;

	memory->Write(Address::LCDCStatus, LCDCStatus);
}

void CPU::AdvanceClocks(int clocks) {

	memory->IncrementDivAndTimerRegisters(clocks);
	graphics->update(clocks, speedMode);
	apu->step(clocks);
}
