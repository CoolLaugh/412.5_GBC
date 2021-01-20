#include "cpu.h"

bool Cpu::flagTest(flagType flag) {

	byte result = registers.f & flag;
	if (result == 0) {
		return false;
	}
	else {
		return true;
	}
}

void Cpu::flagSet(flagType flag) {
	
	registers.f |= flag;
}

void Cpu::flagSet(flagType flag, bool value) {

	if (value == true) {
		flagSet(flag);
	}
	else {
		flagReset(flag);
	}

}

void Cpu::flagReset(flagType flag) {

	registers.f &= ~flag;
}

bool Cpu::bitTest(byte value, Bits bit) {

	return (value & bit) != 0;
}

void Cpu::bitSet(byte& value, Bits bit) {

	value |= bit;
}

void Cpu::bitReset(byte& value, Bits bit) {

	value &= ~bit;
}

void Cpu::setInterrupt(interruptFlags flag) {
	
	byte interuptFlags = memory.Read(Address::InteruptFlag);
	interuptFlags |= (1 << flag);
	memory.Write(Address::InteruptFlag, interuptFlags);
}

void Cpu::PowerUpSequence() {

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

	memory.PowerUpSequence();

	outputStateBuffer.reserve(11000000);
}



// load immediate value into register
word Cpu::LD(byte & reg) {

	reg = memory.Read(registers.pc);
	registers.pc++;
	return 8; // cycles
}

// load value from one register into another
word Cpu::LDreg(byte & reg1, byte & reg2) {

	reg1 = reg2;

	return 4; // cycles
}

// load value from memory address
word Cpu::LDRegFromMemory(byte & reg, word address) {

	reg = memory.Read(address);

	return 8; // cycles
}

word Cpu::LDregHL(byte & reg) {

	word address = Combinebytes(registers.l, registers.h);
	reg = memory.Read(address);

	return 8; // cycles
}

// put stackpointer plus immediate value into hl
word Cpu::LDHL() {

	signed char n = memory.Read(registers.pc);
	word hl = registers.sp + n;
	registers.pc++;
	auto pair = splitBytes(hl);
	registers.l = pair.first;
	registers.h = pair.second;

	flagReset(flagType::zero);
	flagReset(flagType::negative);
	halfCarryFlag16(registers.sp, (word)n);
	carryFlag16(registers.sp, (word)n);

	return 12; // cycles
}

word Cpu::WriteSP() {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));
	registers.pc += 2;

	auto pair = splitBytes(registers.sp);
	memory.Write(address, pair.first);
	memory.Write(address + 1, pair.second);

	return 20;
}

// load immediate 16-bit value into register pair
word Cpu::LD16(byte & reg1, byte & reg2) {

	reg1 = memory.Read(registers.pc + 1);
	reg2 = memory.Read(registers.pc);
	registers.pc += 2;
	return 12;
}

word Cpu::LD16(word & reg1) {

	reg1 = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));
	registers.pc += 2;

	return 12;
}

// push two bytes onto the stack from a register pair
word Cpu::Push(byte & reg1, byte & reg2) {

	registers.sp--;
	memory.Write(registers.sp, reg1);
	registers.sp--;
	memory.Write(registers.sp, reg2);

	return 16;
}

// pop two bytes off the stack into a register pair
word Cpu::Pop(byte & reg1, byte & reg2) {

	reg2 = memory.Read(registers.sp);
	registers.sp++;
	reg1 = memory.Read(registers.sp);
	registers.sp++;

	return 12;
}

// indicates if the carry flag is set after addition from bit 3
void Cpu::halfCarryFlag(byte value1, byte value2, bool carry) {

	value1 &= 0xF;
	value2 &= 0xF;

	if (carry == true) value2++;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xF);
}

// indicates if the carry flag is set after addition from bit 11
void Cpu::halfCarryFlag16(word value1, word value2) {

	value1 &= 0xF;
	value2 &= 0xF;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xF);
}

void Cpu::halfCarryFlag16Hi(word value1, word value2) {

	value1 &= 0xFFF;
	value2 &= 0xFFF;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xFFF);
}

// indicates if the carry flag is set after addition from bit 7
void Cpu::carryFlag(byte value1, byte value2, bool carry) {

	word Wvalue1 = value1;
	word Wvalue2 = value2;

	if (carry == true) Wvalue2++;

	flagSet(flagType::carry, (Wvalue1 + Wvalue2) > 0xFF);
}

// indicates if the carry flag is set after 16-bit addition from bit 15
void Cpu::carryFlag16(word value1, word value2) {

	value1 &= 0xFF;
	value2 &= 0xFF;

	flagSet(flagType::carry, (value1 + value2) > 0xFF);
}

void Cpu::carryFlag16Hi(word value1, word value2) {

	int result = value1;
	result += value2;

	flagSet(flagType::carry, result > 0xFFFF);
}

// indicates if a bit is borrowed from bit 4 after a subtraction
void Cpu::halfNoBorrow(byte value1, byte value2, bool carry) {

	value1 &= 0xF;
	value2 &= 0xF;
	if (carry == true) value2++;

	flagSet(flagType::halfCarry, value1 < value2);
}

// indicates if a bit is borrowed after a subtraction
void Cpu::noBorrow(byte value1, byte value2, bool carry) {

	word Wvalue1 = value1;
	word Wvalue2 = value2;

	if (carry == true) Wvalue2++;
	flagSet(flagType::carry, Wvalue1 < Wvalue2);
}

void Cpu::zeroFlag(byte val) {

	flagSet(flagType::zero, val == 0);
}

// add value to a, ignore carry
word Cpu::ADD(byte value) {

	byte result = registers.a + value;
	
	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(registers.a, value, false);
	carryFlag(registers.a, value, false);

	registers.a = result;

	return 4; // sometimes add is 8 cycles, check the manual
}

// add value to a, include carry
word Cpu::ADDC(byte value) {

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

	return 4; // sometimes add is 8 cycles, check the manual
}

// subtract value from a, ignore carry
word Cpu::SUB(byte value) {

	byte result = registers.a - value;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value, false);
	noBorrow(registers.a, value, false);

	registers.a = result;

	return 4; // sometimes sub is 8 cycles, check the manual
}

// subtract value from a, include carry
word Cpu::SUBC(byte value) {

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

	return 4; // sometimes sub is 8 cycles, check the manual
}

// and value with register a
word Cpu::AND(byte value) {

	byte result = registers.a & value;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagSet(flagType::halfCarry);
	flagReset(flagType::carry);

	registers.a = result;

	return 4; // sometimes is 8 cycles, check the manual
}

// or value with register a
word Cpu::OR(byte value) {

	byte result = registers.a | value;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	registers.a = result;

	return 4; // sometimes is 8 cycles, check the manual
}

// xor value with register a
word Cpu::XOR(byte value) {

	byte result = registers.a ^ value;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	registers.a = result;

	return 4; // sometimes is 8 cycles, check the manual
}

// compare value to register a and set flags accordingly
word Cpu::CP(byte value) {

	byte result = registers.a - value;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value, false);
	noBorrow(registers.a, value, false);

	return 4; // sometimes is 8 cycles, check the manual
}

// increment register
word Cpu::INC(byte& reg) {

	byte result = reg + 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(reg, 1, false);

	reg = result;

	return 4; // sometimes is 12 cycles, check the manual
}

// increment memory location pointed to by register hl
word Cpu::INCMemory(word address) {

	byte value = memory.Read(address);
	byte result = value + 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(value, 1, false);

	memory.Write(address, result);

	return 12; // sometimes is 12 cycles, check the manual
}

// decrement register
word Cpu::DEC(byte& reg) {

	byte result = reg - 1;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(reg, 1, false);

	reg = result;

	return 4; // sometimes is 12 cycles, check the manual
}

// decrement memory location pointed to by register hl
word Cpu::DECMemory(word address) {

	byte value = memory.Read(address);
	byte result = value - 1;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(value, 1, false);

	memory.Write(address, result);

	return 12; 
}

// add value to register combination hl
word Cpu::ADDHL(word value) {

	word hl = Combinebytes(registers.l, registers.h);
	word result = hl + value;

	flagReset(flagType::negative);
	halfCarryFlag16Hi(hl, value);
	carryFlag16Hi(hl, value);

	auto pair = splitBytes(result);

	registers.l = pair.first;
	registers.h = pair.second;

	return 8;
}

// add immediate signed value to stack pointer
word Cpu::ADDSP() {

	signed char n = memory.Read(registers.pc);
	registers.pc++;

	word result = registers.sp + n;

	flagReset(flagType::zero);
	flagReset(flagType::negative);
	halfCarryFlag16(registers.sp, n);
	carryFlag16(registers.sp, n);;

	registers.sp = result;
	
	return 16;
}

// increment two registers as if they were one 16 bit register
word Cpu::INC16(byte & reg1, byte & reg2) {

	word combinedRegister = Combinebytes(reg1, reg2);

	combinedRegister++;

	auto pair = splitBytes(combinedRegister);

	reg1 = pair.first;
	reg2 = pair.second;

	return 8;
}

// increment 16 bit register
word Cpu::INC16(word & reg) {

	reg++;

	return 8;
}

// decrement two registers as if they were one 16 bit register
word Cpu::DEC16(byte & reg1, byte & reg2) {

	word combinedRegister = Combinebytes(reg1, reg2);

	combinedRegister--;

	auto pair = splitBytes(combinedRegister);

	reg1 = pair.first;
	reg2 = pair.second;

	return 8;
}


// decrement 16 bit register
word Cpu::DEC16(word & reg) {

	reg--;

	return 8;
}

// swap the lower and upper nibbles
word Cpu::SWAP(byte & reg) {

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

	return 8;
}

// swap the lower and upper nibbles (for memory)
word Cpu::SWAP(word address) {

	byte data = memory.Read(address);

	byte upper = data & 0xF0;
	byte lower = data & 0x0F;

	upper >>= 4;
	lower <<= 4;

	byte result = upper | lower;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagReset(flagType::carry);

	memory.Write(address, result);

	return 16;
}

// swap the lower and upper nibble of a byte in memory
word Cpu::SWAPMemory() {

	word address = Combinebytes(registers.l, registers.h);

	byte data = memory.Read(address);

	SWAP(data);

	memory.Write(address, data);

	return 16;
}

// decimal adjust register A
// copied https://www.reddit.com/r/EmuDev/comments/cdtuyw/gameboy_emulator_fails_blargg_daa_test/etwcyvy/
word Cpu::DAA() {

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

	return 4; // cycles
}

// complement A register
word Cpu::CPL() {

	registers.a = ~registers.a;

	flagSet(flagType::negative);
	flagSet(flagType::halfCarry);

	return 4;
}

// complement carry flag
word Cpu::CCF() {

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);

	flagSet(flagType::carry, !flagTest(flagType::carry));
	
	return 4;
}

// set carry flag
word Cpu::SCF() {

	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry);

	return 4;
}

// no operation
word Cpu::NOP() {
	// does nothing
	return 4;
}

// power down cpu until an interrupt occurs
word Cpu::HALT() {

	halted = true;
	return 4;
}

// halt cpu and display until button push
word Cpu::STOP() {

	registers.pc++;

	if (ColorGameBoyMode == true) {

		byte speedSwitch = memory.Read(Address::PrepareSpeedSwitch);
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
			memory.Write(Address::PrepareSpeedSwitch, speedSwitch);
		}
	}
	else {

		stop = true;
	}

	return 4;
}

// disable interupts
word Cpu::DI() {
	interuptDisable = true;
	return 4;
}

// enable interupts
word Cpu::EI() {
	interuptEnable = true;
	return 4;
}

// rotate left bit 7 to carry
word Cpu::RLC(byte& reg, bool isRegisterA) {

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

	return 8;
}

// rotate left bit 7 to carry (for memory)
word Cpu::RLC(word address) {

	byte result = memory.Read(address);

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	if (bit7 == true) {
		bitSet(result, Bits::b0);
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory.Write(address, result);

	return 16;
}

// rotate left through carry
word Cpu::RL(byte & reg, bool isRegisterA) {

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

	return 8;
}

// rotate left through carry (for memory)
word Cpu::RL(word address) {

	byte result = memory.Read(address);

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	if (flagTest(flagType::carry) == true) {
		bitSet(result, Bits::b0);
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory.Write(address, result);

	return 16;
}

// rotate right bit 7 to carry
word Cpu::RRC(byte & reg, bool isRegisterA) {

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

	return 8;
}

// rotate right bit 7 to carry (for memory)
word Cpu::RRC(word address) {

	byte result = memory.Read(address);

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (bit0 == true) {
		bitSet(result, Bits::b7);
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory.Write(address, result);

	return 16;
}

// rotate right through carry
word Cpu::RR(byte & reg, bool isRegisterA) {

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

	return 8;
}

// rotate right through carry (for memory)
word Cpu::RR(word address) {

	byte result = memory.Read(address);

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	if (flagTest(flagType::carry) == true) {
		result |= 0x80;
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory.Write(address, result);

	return 16;
}

// shift left into carry lsb set to 0
word Cpu::SLA(byte & reg) {

	byte result = reg;

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	bitReset(result, Bits::b0);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	reg = result;

	return 8;
}

// shift left into carry lsb set to 0 (for memory)
word Cpu::SLA(word address) {

	byte result = memory.Read(address);

	bool bit7 = bitTest(result, Bits::b7);

	result <<= 1;
	bitReset(result, Bits::b0);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory.Write(address, result);

	return 16;
}

// shift right into carry, msb stays the same
word Cpu::SRA(byte & reg) {

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

	return 8;
}

// shift right into carry, msb stays the same (for memory)
word Cpu::SRA(word address) {

	byte result = memory.Read(address);

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

	memory.Write(address, result);

	return 16;
}

// shift right into carry msb set to 0
word Cpu::SRL(byte & reg) {

	byte result = reg;

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	bitReset(result, Bits::b7);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	return 8;
}

// shift right into carry msb set to 0 (for memory)
word Cpu::SRL(word address) {

	byte result = memory.Read(address);

	bool bit0 = bitTest(result, Bits::b0);

	result >>= 1;
	bitReset(result, Bits::b7);

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory.Write(address, result);

	return 16;
}

// check bit and set flags
word Cpu::BIT(byte reg, byte bit) {

	byte bitmask = 1 << bit;

	flagSet(flagType::zero, (reg & bitmask) == 0);

	flagReset(flagType::negative);
	flagSet(flagType::halfCarry);

	return 8;
}

// set bit
word Cpu::SET(byte& reg, byte bit) {

	byte bitmask = 1 << bit;

	reg |= bitmask;

	return 8;
}

// set bit
word Cpu::SET(word address, byte bit) {

	byte bitmask = 1 << bit;

	byte data = memory.Read(address);

	data |= bitmask;

	memory.Write(address, data);

	return 16;
}

// reset bit
word Cpu::RES(byte& reg, byte bit) {

	byte bitmask = 1 << bit;
	bitmask = ~bitmask;

	reg &= bitmask;

	return 8;
}

// reset bit
word Cpu::RES(word address, byte bit) {

	byte bitmask = 1 << bit;
	bitmask = ~bitmask;

	byte data = memory.Read(address);

	data &= bitmask;

	memory.Write(address, data);

	return 16;
}

// jump
word Cpu::JP() {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));
	registers.pc = address;

	return 16;
}

// conditional jump
word Cpu::JPcc(condition cdn) {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));
	registers.pc += 2;

	word cycles = 12;

	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	registers.pc = address; cycles = 16; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)	registers.pc = address; cycles = 16; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	registers.pc = address; cycles = 16; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	registers.pc = address; cycles = 16; break;
	}

	return cycles;
}

// jump to hl
word Cpu::JPHL() {

	word address = Combinebytes(registers.l, registers.h);
	registers.pc = address;

	return 4;
}

// relative jump
word Cpu::JR() {

	signed char offset = (signed char)memory.Read(registers.pc);
	registers.pc++;
	registers.pc += offset;

	return 12;
}

// conditional relative jump
word Cpu::JRcc(condition cdn) {

	signed char offset = (signed char)memory.Read(registers.pc);
	registers.pc++;

	word cycles = 8;

	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	registers.pc += offset; cycles = 12; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)	registers.pc += offset; cycles = 12; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	registers.pc += offset; cycles = 12; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	registers.pc += offset; cycles = 12; break;
	}

	return cycles;
}

// call
word Cpu::CALL() {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));

	auto pair = splitBytes(registers.pc + 2);
	registers.sp--;
	memory.Write(registers.sp, pair.second);
	registers.sp--;
	memory.Write(registers.sp, pair.first);

	registers.pc = address;

	return 24;
}

// conditional call
word Cpu::CALLcc(condition cdn) {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));

	word cycles = 12;

	bool call = false;
	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	call = true; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)	call = true; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	call = true; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	call = true; break;
	}

	if (call == true) {

		auto pair = splitBytes(registers.pc + 2);
		registers.sp--;
		memory.Write(registers.sp, pair.second);
		registers.sp--;
		memory.Write(registers.sp, pair.first);

		registers.pc = address;
		cycles = 24;
	}
	else {
		registers.pc += 2;
	}

	return 12;
}

// restart
word Cpu::RST(short offset) {

	auto pair = splitBytes(registers.pc);
	registers.sp--;
	memory.Write(registers.sp, pair.second);
	registers.sp--;
	memory.Write(registers.sp, pair.first);

	registers.pc = offset;

	return 16;
}

// return
word Cpu::RET() {

	byte first = memory.Read(registers.sp);
	registers.sp++;
	byte second = memory.Read(registers.sp);
	registers.sp++;

	registers.pc = Combinebytes(first, second);

	return 16;
}

// conditional return
word Cpu::RETcc(condition cdn) {

	word cycles = 8;

	bool ret = false;
	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	ret = true; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)	ret = true; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	ret = true; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	ret = true; break;
	}

	if (ret == true) {

		byte first = memory.Read(registers.sp);
		registers.sp++;
		byte second = memory.Read(registers.sp);
		registers.sp++;

		registers.pc = Combinebytes(first, second);
		cycles = 20;
	}

	return cycles;
}

// return and enable interrupts
word Cpu::RETI() {

	byte first = memory.Read(registers.sp);
	registers.sp++;
	byte second = memory.Read(registers.sp);
	registers.sp++;

	registers.pc = Combinebytes(first, second);

	interuptEnable = true;

	return 16;
}

// decrement two registers as one 16 bit registers
word Cpu::decrement16reg(byte & reg1, byte & reg2) {

	word combined = Combinebytes(reg2, reg1);
	combined--;
	reg2 = (combined & 0xFF);
	reg1 = (combined >> 8);

	return 0;
}

// increment two registers as one 16 bit registers
word Cpu::increment16reg(byte & reg1, byte & reg2) {

	word combined = Combinebytes(reg2, reg1);
	combined++;
	reg2 = (combined & 0xFF);
	reg1 = (combined >> 8);

	return 0;
}

// combine multiple registers into word
word Cpu::Combinebytes(byte value1, byte value2) {

	word combine = value2;
	combine <<= 8;
	combine |= value1;

	return combine;
}

// seperate word into seperate bytes for multiple registers
std::pair<byte, byte> Cpu::splitBytes(word value) {

	byte first = value & 0xFF;
	byte second = (value >> 8) & 0xFF;

	return std::make_pair(first, second);
}

word Cpu::CombinebytesR(byte value1, byte value2) {

	word combine = value1;
	combine <<= 8;
	combine |= value2;

	return combine;
}

std::pair<byte, byte> Cpu::splitBytesR(word value) {

	byte first = (value >> 8) & 0xFF;
	byte second = value & 0xFF;

	return std::make_pair(first, second);
}

void Cpu::outputState() {

	if (logState == false) {
		return;
	}

	std::string hex[16] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };

	outputStateBuffer += "A: " + hex[(registers.a & 0xF0) >> 4] + hex[(registers.a & 0x0F)] + " ";
	outputStateBuffer += "F: " + hex[(registers.f & 0xF0) >> 4] + hex[(registers.f & 0x0F)] + " ";
	outputStateBuffer += "B: " + hex[(registers.b & 0xF0) >> 4] + hex[(registers.b & 0x0F)] + " ";
	outputStateBuffer += "C: " + hex[(registers.c & 0xF0) >> 4] + hex[(registers.c & 0x0F)] + " ";
	outputStateBuffer += "D: " + hex[(registers.d & 0xF0) >> 4] + hex[(registers.d & 0x0F)] + " ";
	outputStateBuffer += "E: " + hex[(registers.e & 0xF0) >> 4] + hex[(registers.e & 0x0F)] + " ";
	outputStateBuffer += "H: " + hex[(registers.h & 0xF0) >> 4] + hex[(registers.h & 0x0F)] + " ";
	outputStateBuffer += "L: " + hex[(registers.l & 0xF0) >> 4] + hex[(registers.l & 0x0F)] + " ";

	outputStateBuffer += "SP: " + hex[(registers.sp & 0xF000) >> 12] + hex[(registers.sp & 0x0F00) >> 8];
	outputStateBuffer += hex[(registers.sp & 0x00F0) >> 4] + hex[(registers.sp & 0x000F)] + " ";
	
	outputStateBuffer += "PC: 00:" + hex[(registers.pc & 0xF000) >> 12] + hex[(registers.pc & 0x0F00) >> 8];
	outputStateBuffer += hex[(registers.pc & 0x00F0) >> 4] + hex[(registers.pc & 0x000F)] + " ";

	outputStateBuffer += "(";
	outputStateBuffer += hex[(memory.Read(registers.pc) & 0xF0) >> 4] + hex[(memory.Read(registers.pc) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory.Read(registers.pc + 1) & 0xF0) >> 4] + hex[(memory.Read(registers.pc + 1) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory.Read(registers.pc + 2) & 0xF0) >> 4] + hex[(memory.Read(registers.pc + 2) & 0x0F)];
	outputStateBuffer += " ";
	outputStateBuffer += hex[(memory.Read(registers.pc + 3) & 0xF0) >> 4] + hex[(memory.Read(registers.pc + 3) & 0x0F)];
	outputStateBuffer += ")\n";

	if (outputStateBuffer.size() > 10000000) {

		std::ofstream out;
		out.open("CPUStateLog.txt", std::ios::out | std::ios::app);
		out << outputStateBuffer.c_str();
		outputStateBuffer.clear();
		out.close();
	}

}

void Cpu::performInterupts() {

	if (interuptEnable == true && interuptEnableInstructionCount == 0) {
		// interupts enable is delayed by one instruction
		interuptEnableInstructionCount++;
	}
	else if (interuptEnable == true && interuptEnableInstructionCount >= 1) {
		interuptEnableInstructionCount = 0;
		interuptEnable = false;
		interupt = true;
	}

	if (interuptDisable == true && interuptDisableInstructionCount == 0) {
		// interupts disable is delayed by one instruction
		interuptDisableInstructionCount++;
	}
	else if (interuptDisable == true && interuptDisableInstructionCount >= 1) {
		interuptDisableInstructionCount = 0;
		interuptDisable = false;
		interupt = false;
	}

	byte interuptFlags = memory.Read(Address::InteruptFlag);
	byte interuptsEnabled = memory.Read(Address::InteruptEnable);

	if (interuptFlags != 0 && interuptsEnabled != 0) {
		halted = false;
	}

	if (interupt == true) {

		for (size_t i = 0; i < interruptFlags::END; i++) {

			bool interuptFlag = (interuptFlags & (1 << i)) != 0;
			bool interuptEnabled = (interuptsEnabled & (1 << i)) != 0;

			if (interuptFlag == true && interuptEnabled == true) {

				auto pair = splitBytes(registers.pc);
				Push(pair.second, pair.first);

				switch (i) {
				case 0: registers.pc = 0x40; break;
				case 1: registers.pc = 0x48; break;
				case 2: registers.pc = 0x50; break;
				case 3: registers.pc = 0x58; break;
				case 4: registers.pc = 0x60; break;
				}

				interuptFlags &= ~(1 << i); // reset bit of interupt
				memory.Write(Address::InteruptFlag, interuptFlags);
				interupt = false;
				halted = false;

				break; // don't check interupts of lower prioriry
			}
		}
	}
}

void Cpu::dividerRegisterINC(short cycles) {

	dividerCycles += cycles * speedMode;

	if (dividerCycles > 256) { // incremented at rate of 16384Hz (4194304 Hz cpu / 16384Hz = 256)
		dividerCycles -= 256;
		memory.memorySpace[Address::DIVRegister]++;
	}

}

void Cpu::TimerCounterINC(short cycles) {


	byte TimerControl = memory.Read(Address::TimerControl);
	
	if (bitTest(TimerControl, Bits::b2) == false) {
		return;
	}

	switch (TimerControl & 0x3) {
	case 0: clockFrequency = 1024; break;
	case 1: clockFrequency = 16; break;
	case 2: clockFrequency = 64; break;
	case 3: clockFrequency = 256; break;
	}

	//if (memory.timerFrequencyChange > 0) {
	//	if (clockFrequency != memory.timerFrequencyChange) {

	//		clockFrequency = memory.timerFrequencyChange;
	//		timerCycles = 0;
	//	}
	//	memory.timerFrequencyChange = 0;
	//}

	timerCycles += cycles * speedMode;

	while(timerCycles >= clockFrequency) {

		timerCycles -= clockFrequency;
		byte TimerCounter = memory.Read(Address::Timer);


		if (TimerCounter == 0xFF) {
			timerOverflow = true; // timer isn't reset until one cycle after overflow
		}

		if (TimerCounter == 0x00 && timerOverflow == true) {

			TimerCounter = memory.Read(Address::TimerModulo);
			timerOverflow = false;
			
			byte interuptFlags = memory.Read(Address::InteruptFlag);
			interuptFlags |= (1 << interruptFlags::Timer);
			memory.Write(Address::InteruptFlag, interuptFlags);
		}
		else {

			TimerCounter++;
		}

		memory.Write(Address::Timer, TimerCounter);
	}
}

void Cpu::LCDStatusRegister(word& cyclesThisLine) {

	byte LCDC = memory.Read(Address::LCDC);
	byte LCDCStatus = memory.Read(Address::LCDCStatus);

	if (bitTest(LCDC, Bits::b7) == false) {

		memory.memorySpace[Address::LY] = 0;
		cyclesThisLine = 0;
		LCDCStatus &= ~0x03;
		bitSet(LCDCStatus, Bits::b0);
	}
	else {

		byte LY = memory.Read(Address::LY);
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

		byte LYCompare = memory.Read(Address::LYCompare);
		if (LY == LYCompare) {
			bitSet(LCDCStatus, Bits::b2);
			if (bitTest(LCDCStatus, Bits::b6) == true) {
				setInterrupt(interruptFlags::LCDSTAT);
			}
		}
		else {
			bitReset(LCDCStatus, Bits::b2);
		}
	}

	memory.Write(Address::LCDCStatus, LCDCStatus);
}
