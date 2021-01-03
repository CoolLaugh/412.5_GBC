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

	memory.PowerUpSequence();

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

// load 
word Cpu::LDRegFromMemory() {

	registers.a = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));
	registers.pc += 2;
	return 16; // cycles
}

word Cpu::LDregHL(byte & reg) {

	word address = Combinebytes(registers.h, registers.l);
	reg = memory.Read(address);

	return 8; // cycles
}

word Cpu::LDmemfromreg(byte & reg) {
	return 0;
}

word Cpu::LDmem() {
	return 0;
}

// put stackpointer plus immediate value into hl
word Cpu::LDHL() {

	signed char n = memory.Read(registers.pc);
	word hl = registers.sp + n;
	registers.pc++;
	auto pair = splitBytes(hl);
	registers.h = pair.first;
	registers.l = pair.second;

	flagReset(flagType::zero);
	flagReset(flagType::negative);
	halfCarryFlag(registers.sp, n);
	carryFlag(registers.sp, n);

	return 12; // cycles
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
void Cpu::halfCarryFlag(byte value1, byte value2) {

	value1 &= 0xF;
	value2 &= 0xF;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xF);
}

// indicates if the carry flag is set after addition from bit 11
void Cpu::halfCarryFlag16(word value1, word value2) {

	value1 &= 0xFFF;
	value2 &= 0xFFF;

	flagSet(flagType::halfCarry, (value1 + value2) > 0xFFF);
}

// indicates if the carry flag is set after addition from bit 7
void Cpu::carryFlag(byte value1, byte value2) {

	word add = value1;
	add += value2;

	flagSet(flagType::carry, add > 0xFF);
}

// indicates if the carry flag is set after 16-bit addition from bit 15
void Cpu::carryFlag16(word value1, word value2) {

	unsigned int add = value1;
	add += value2;

	flagSet(flagType::carry, add > 0xFFFF);
}

// indicates if a bit is borrowed from bit 4 after a subtraction
void Cpu::halfNoBorrow(byte value1, byte value2) {

	value1 &= 0xF;
	value2 &= 0xF;

	flagSet(flagType::halfCarry, (value1 & 0xF) < (value2 & 0xF));
}

// indicates if a bit is borrowed after a subtraction
void Cpu::noBorrow(byte value1, byte value2) {

	flagSet(flagType::carry, value1 < value2);
}

void Cpu::zeroFlag(byte val) {

	flagSet(flagType::zero, val == 0);
}

// add value to a, ignore carry
word Cpu::ADD(byte value) {

	byte result = registers.a + value;
	
	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(registers.a, value);
	carryFlag(registers.a, value);

	registers.a = result;

	return 4; // sometimes add is 8 cycles, check the manual
}

// add value to a, include carry
word Cpu::ADDC(byte value) {

	if (flagTest(flagType::carry)) value++;
	byte result = registers.a + value;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(registers.a, value);
	carryFlag(registers.a, value);

	registers.a = result;

	return 4; // sometimes add is 8 cycles, check the manual
}

// subtract value from a, ignore carry
word Cpu::SUB(byte value) {

	byte result = registers.a - value;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value);
	noBorrow(registers.a, value);

	registers.a = result;

	return 4; // sometimes sub is 8 cycles, check the manual
}

// subtract value from a, include carry
word Cpu::SUBC(byte value) {

	if (flagTest(flagType::carry)) value++;
	byte result = registers.a - value;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(registers.a, value);
	noBorrow(registers.a, value);

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
	halfNoBorrow(registers.a, value);
	noBorrow(registers.a, value);

	return 4; // sometimes is 8 cycles, check the manual
}

// increment register
word Cpu::INC(byte& reg) {

	byte result = reg + 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(reg, 1);

	reg = result;

	return 4; // sometimes is 12 cycles, check the manual
}

// increment memory location pointed to by register hl
word Cpu::INCMemory(word address) {

	byte result = memory.Read(address);
	result++;

	zeroFlag(result);
	flagReset(flagType::negative);
	halfCarryFlag(result, 1);

	memory.Write(address, result);

	return 12; // sometimes is 12 cycles, check the manual
}

// decrement register
word Cpu::DEC(byte& reg) {

	byte result = reg - 1;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(reg, 1);

	reg = result;

	return 4; // sometimes is 12 cycles, check the manual
}

// decrement memory location pointed to by register hl
word Cpu::DECMemory(word address) {

	byte result = memory.Read(address);
	result--;

	zeroFlag(result);
	flagSet(flagType::negative);
	halfNoBorrow(result, 1);

	memory.Write(address, result);


	return 12; 
}

// add value to register combination hl
word Cpu::ADDHL(word value) {

	word hl = Combinebytes(registers.h, registers.l);
	word result = hl + value;

	flagReset(flagType::negative);
	halfCarryFlag16(hl, value);
	carryFlag16(hl, value);

	auto pair = splitBytes(result);

	registers.h = pair.first;
	registers.l = pair.second;

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

	upper >> 4;
	lower << 4;

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

	upper >> 4;
	lower << 4;

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

	word address = Combinebytes(registers.h, registers.l);

	byte data = memory.Read(address);

	SWAP(data);

	memory.Write(address, data);

	return 16;
}

// decimal adjust register A
// copied
word Cpu::DAA() {

	if (flagTest(flagType::negative)) {
		if ((registers.a & 0x0F) > 0x09 || flagTest(flagType::halfCarry)) {
			registers.a -= 0x06; //Half borrow: (0-1) = (0xF-0x6) = 9
			if ((registers.a & 0xF0) == 0xF0) flagSet(flagType::carry); else flagReset(flagType::carry);
		}

		if ((registers.a & 0xF0) > 0x90 || flagTest(flagType::carry) == true) registers.a -= 0x60;
	}
	else {
		if ((registers.a & 0x0F) > 9 || flagTest(flagType::halfCarry)) {
			registers.a += 0x06; //Half carry: (9+1) = (0xA+0x6) = 10
			if ((registers.a & 0xF0) == 0) flagSet(flagType::carry); else flagReset(flagType::carry);
		}

		if ((registers.a & 0xF0) > 0x90 || flagTest(flagType::carry) == true) registers.a += 0x60;
	}

	if (registers.a == 0) flagSet(flagType::zero); else flagReset(flagType::zero);

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

	return 0;
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
	// todo
	registers.pc++;
	return 4;
}

// disable interupts
word Cpu::DI() {
	interupt = false;
	return 4;
}

// enable interupts
word Cpu::EI() {
	interupt = true;
	return 4;
}

// rotate left bit 7 to carry
word Cpu::RLC(byte& reg) {

	byte result = reg;

	bool bit7 = (result & 0x80) == 0x80;

	result <<= 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	reg = result;

	return 8;
}

// rotate left bit 7 to carry (for memory)
word Cpu::RLC(word address) {

	byte result = memory.Read(address);

	bool bit7 = (result & 0x80) == 0x80;

	result <<= 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory.Write(address, result);

	return 8;
}

// rotate left through carry
word Cpu::RL(byte & reg) {

	byte result = reg;

	bool bit7 = (result & 0x80) == 0x80;

	result <<= 1;
	if (flagTest(flagType::carry) == true) {
		result |= 0x1;
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	reg = result;

	return 8;
}

// rotate left through carry (for memory)
word Cpu::RL(word address) {

	byte result = memory.Read(address);

	bool bit7 = (result & 0x80) == 0x80;

	result <<= 1;
	if (flagTest(flagType::carry) == true) {
		result |= 0x1;
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit7);

	memory.Write(address, result);

	return 8;
}

// rotate right bit 7 to carry
word Cpu::RRC(byte & reg) {

	byte result = reg;

	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	return 8;
}

// rotate right bit 7 to carry (for memory)
word Cpu::RRC(word address) {

	byte result = memory.Read(address);

	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory.Write(address, result);

	return 8;
}

// rotate right through carry
word Cpu::RR(byte & reg) {

	byte result = reg;

	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;
	if (flagTest(flagType::carry) == true) {
		result |= 0x80;
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	reg = result;

	return 8;
}

// rotate right through carry (for memory)
word Cpu::RR(word address) {

	byte result = memory.Read(address);

	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;
	if (flagTest(flagType::carry) == true) {
		result |= 0x80;
	}

	zeroFlag(result);
	flagReset(flagType::negative);
	flagReset(flagType::halfCarry);
	flagSet(flagType::carry, bit0);

	memory.Write(address, result);

	return 8;
}

// shift left into carry lsb set to 0
word Cpu::SLA(byte & reg) {

	byte result = reg;

	bool bit7 = (result & 0x80) == 0x80;

	result <<= 1;
	result &= 0xFE; //LSB set to 0

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

	bool bit7 = (result & 0x80) == 0x80;

	result <<= 1;
	result &= 0xFE; //LSB set to 0

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

	bool bit7 = (result & 0x80) == 0x80;
	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;
	if (bit7 == true) {
		result |= 0x80; // keep MSB the same
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

	bool bit7 = (result & 0x80) == 0x80;
	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;
	if (bit7 == true) {
		result |= 0x80; // keep MSB the same
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

	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;
	result &= 0x7F;

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

	bool bit0 = (result & 0x1) == 0x1;

	result >>= 1;
	result &= 0x7F;

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
word Cpu::RES(byte reg, byte bit) {

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

	return 12;
}

// conditional jump
word Cpu::JPcc(condition cdn) {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));
	registers.pc += 2;

	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	registers.pc = address; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)	registers.pc = address; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	registers.pc = address; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	registers.pc = address; break;
	}

	return 12;
}

// jump to hl
word Cpu::JPHL() {

	word address = Combinebytes(registers.h, registers.l);
	registers.pc = address;

	return 4;
}

// relative jump
word Cpu::JR() {

	signed char offset = memory.Read(registers.pc);
	registers.pc += offset;

	return 8;
}

// conditional relative jump
word Cpu::JRcc(condition cdn) {

	signed char offset = memory.Read(registers.pc);
	registers.pc++;

	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	registers.pc += offset; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)	registers.pc += offset; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	registers.pc += offset; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	registers.pc += offset; break;
	}

	return 8;
}

// call
word Cpu::CALL() {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));

	auto pair = splitBytes(registers.pc + 2);
	registers.sp--;
	memory.Write(registers.sp, pair.first);
	registers.sp--;
	memory.Write(registers.sp, pair.second);

	registers.pc = address;

	return 12;
}

// conditional call
word Cpu::CALLcc(condition cdn) {

	word address = Combinebytes(memory.Read(registers.pc), memory.Read(registers.pc + 1));

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
		memory.Write(registers.sp, pair.first);
		registers.sp--;
		memory.Write(registers.sp, pair.second);

		registers.pc = address;
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
	memory.Write(registers.sp, pair.first);
	registers.sp--;
	memory.Write(registers.sp, pair.second);

	registers.pc = offset;

	return 32;
}

// return
word Cpu::RET() {

	byte second = memory.Read(registers.sp);
	registers.sp++;
	byte first = memory.Read(registers.sp);
	registers.sp++;

	registers.pc = Combinebytes(first, second);

	return 8;
}

// conditional return
word Cpu::RETcc(condition cdn) {


	bool ret = false;
	switch (cdn) {
	case Cpu::NotZero:	if (flagTest(flagType::zero) == false)	ret = true; break;
	case Cpu::Zero:		if (flagTest(flagType::zero) == true)		ret = true; break;
	case Cpu::NotCarry: if (flagTest(flagType::carry) == false)	ret = true; break;
	case Cpu::Carry:	if (flagTest(flagType::carry) == true)	ret = true; break;
	}

	if (ret == true) {

		byte second = memory.Read(registers.sp);
		registers.sp++;
		byte first = memory.Read(registers.sp);
		registers.sp++;

		registers.pc = Combinebytes(first, second);
	}
	else {
		registers.pc++;
	}

	return 8;
}

// return and enable interups
word Cpu::RETI() {

	byte second = memory.Read(registers.sp);
	registers.sp++;
	byte first = memory.Read(registers.sp);
	registers.sp++;

	registers.pc = Combinebytes(first, second);

	interupt = true;

	return 8;
}

// decrement two registers as one 16 bit registers
word Cpu::decrement16reg(byte & reg1, byte & reg2) {

	word combined = Combinebytes(reg1, reg2);
	combined--;
	reg2 = (combined >> 8);
	reg1 = (combined & 0xFF);

	return 0;
}

// increment two registers as one 16 bit registers
word Cpu::increment16reg(byte & reg1, byte & reg2) {

	word combined = Combinebytes(reg1, reg2);
	combined++;
	reg2 = (combined >> 8);
	reg1 = (combined & 0xFF);

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
