#pragma once
#include <vector>
#include "memory.h"

typedef unsigned char byte;
typedef unsigned short word;


class Cpu {

public:

	enum interruptFlags {
		VBlank,
		LCDSTAT,
		Timer,
		Serial,
		Joypad,
		END
	};

	enum MemoryModel {
		mm16_8,
		mm4_32
	};

	enum condition {
		NotZero,
		Zero,
		NotCarry,
		Carry
	};

	enum flagType {
		zero = 0x80,
		negative = 0x40,
		halfCarry = 0x20,
		carry = 0x10
	};

	struct Registers {
		byte a;
		byte f;
		byte b;
		byte c;
		byte d;
		byte e;
		byte h;
		byte l;
		word sp = 0xFFFE;
		word pc = 0x100;
	};

	Registers registers;
	bool halted = false;
	bool stop = false;

	bool interupt = false;
	bool interuptEnable = false;
	int interuptEnableInstructionCount = 0;
	bool interuptDisable = false;
	int interuptDisableInstructionCount = 0;
	int dividerCycles = 0;
	int clockFrequency = 1024;
	int timerCycles = 0;

	Memory memory;

	bool flagTest(flagType flag);
	void flagSet(flagType flag);
	void flagSet(flagType flag, bool value);
	void flagReset(flagType flag);

	bool bitTest(byte value, Bits bit);
	void bitSet(byte& value, Bits bit);
	void bitReset(byte& value, Bits bit);
	void setInterrupt(interruptFlags flag);

	void PowerUpSequence();
	word ExecuteOpcode();
	word ExecuteExtendedOpcode();

	void performInterupts();
	void dividerRegisterINC(short cycles);
	void TimerCounterINC(short cycles);
	void LCDStatusRegister(short cyclesThisFrame);

	word LD(byte& reg);
	word LDreg(byte& reg1, byte& reg2);
	word LDRegFromMemory(byte& reg, word address);
	word LDRegFromMemory();
	word LDregHL(byte& reg);
	word LDmemfromreg(byte& reg);
	word LDmem();
	word LDHL();
	word WriteSP();

	word LD16(byte& reg1, byte& reg2);
	word LD16(word& reg1);

	word Push(byte& reg1, byte& reg2);
	word Pop(byte& reg1, byte& reg2);

	void halfCarryFlag(byte value1, byte value2);
	void halfCarryFlag16(word value1, word value2);
	void carryFlag(byte value1, byte value2);
	void carryFlag16(word value1, word value2);
	void halfNoBorrow(byte value1, byte value2);
	void noBorrow(byte value1, byte value2);
	void zeroFlag(byte val);

	// ALU
	word ADD(byte value);
	word ADDC(byte value);
	word SUB(byte value);
	word SUBC(byte value);
	word AND(byte value);
	word OR(byte value);
	word XOR(byte value);
	word CP(byte value);
	word INC(byte& reg);
	word INCMemory(word address);
	word DEC(byte& reg);
	word DECMemory(word address);

	// 16 bit ALU
	word ADDHL(word value);
	word ADDSP();
	word INC16(byte& reg1, byte& reg2);
	word INC16(word& reg);
	word DEC16(byte& reg1, byte& reg2);
	word DEC16(word& reg);

	// Miscellaneous
	word SWAP(byte& reg);
	word SWAP(word address);
	word SWAPMemory();
	word DAA();
	word CPL();
	word CCF();
	word SCF();
	word NOP();
	word HALT();
	word STOP();
	word DI();
	word EI();

	// rotates and shifts
	word RLC(byte& reg);
	word RLC(word address);
	word RL(byte& reg);
	word RL(word address);
	word RRC(byte& reg);
	word RRC(word address);
	word RR(byte& reg);
	word RR(word address);
	word SLA(byte& reg);
	word SLA(word address);
	word SRA(byte& reg);
	word SRA(word address);
	word SRL(byte& reg);
	word SRL(word address);

	// bit opcodes
	word BIT(byte reg, byte bit);
	word SET(byte& reg, byte bit);
	word SET(word address, byte bit);
	word RES(byte& reg, byte bit);
	word RES(word address, byte bit);

	// jumps
	word JP();
	word JPcc(condition cdn);
	word JPHL();
	word JR();
	word JRcc(condition cdn);

	// calls
	word CALL();
	word CALLcc(condition cdn);

	// restarts
	word RST(short offset);

	// returns
	word RET();
	word RETcc(condition cdn);
	word RETI();


	word decrement16reg(byte& reg1, byte& reg2);
	word increment16reg(byte& reg1, byte& reg2);

	word Combinebytes(byte value1, byte value2);
	std::pair<byte, byte> splitBytes(word value);

	word CombinebytesR(byte value1, byte value2);
	std::pair<byte, byte> splitBytesR(word value);

};
