#pragma once
#include <vector>
#include <queue>
#include "memory.h"
#include "Graphics.h"

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

	Graphics* graphics;

	Registers registers;
	bool halted = false;
	bool stop = false;

	bool interupt = false;
	bool interuptEnable = false;
	byte interuptEnableInstructionCount = 0;
	bool ColorGameBoyMode = false;
	byte speedMode = 1;

	bool logState = false;
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
	void AdvanceClocks(int clocks);

	int performInterupts();
	void LCDStatusRegister(word& cyclesThisLine);

	word LD(byte& reg);
	word LDreg(byte& reg1, byte& reg2);
	word LDRegFromMemory(byte& reg, word address);
	word LDregHL(byte& reg);
	word LDHL();
	word WriteSP();

	word LD16(byte& reg1, byte& reg2);
	word LD16(word& reg1);

	word Push(byte& reg1, byte& reg2);
	word Pop(byte& reg1, byte& reg2);

	void halfCarryFlag(byte value1, byte value2, bool carry);
	void halfCarryFlag16(word value1, word value2);
	void halfCarryFlag16Hi(word value1, word value2);
	void carryFlag(byte value1, byte value2, bool carry);
	void carryFlag16(word value1, word value2);
	void carryFlag16Hi(word value1, word value2);
	void halfNoBorrow(byte value1, byte value2, bool carry);
	void noBorrow(byte value1, byte value2, bool carry);
	void zeroFlag(byte val);

	// ALU
	word ADD(byte value, int clocks = 4);
	word ADDC(byte value, int clocks = 4);
	word SUB(byte value, int clocks = 4);
	word SUBC(byte value, int clocks = 4);
	word AND(byte value, int clocks = 4);
	word OR(byte value, int clocks = 4);
	word XOR(byte value, int clocks = 4);
	word CP(byte value, int clocks = 4);
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
	word RLC(byte& reg, bool isRegisterA = false, int clocks = 8);
	word RLC(word address);
	word RL(byte& reg, bool isRegisterA = false, int clocks = 8);
	word RL(word address);
	word RRC(byte& reg, bool isRegisterA = false, int clocks = 8);
	word RRC(word address);
	word RR(byte& reg, bool isRegisterA = false, int clocks = 8);
	word RR(word address);
	word SLA(byte& reg);
	word SLA(word address);
	word SRA(byte& reg);
	word SRA(word address);
	word SRL(byte& reg);
	word SRL(word address);

	// bit opcodes
	word BIT(byte reg, byte bit);
	word BITHL(byte bit);
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


	void decrement16reg(byte& reg1, byte& reg2);
	void increment16reg(byte& reg1, byte& reg2);

	word Combinebytes(byte value1, byte value2);
	std::pair<byte, byte> splitBytes(word value);

	word CombinebytesR(byte value1, byte value2);
	std::pair<byte, byte> splitBytesR(word value);

	void cleanOutputState();
	void outputState();
	std::string outputStateBuffer;
	std::string outputStateBufferOpcodesOnly;
};
