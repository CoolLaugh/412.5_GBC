#pragma once
#include <vector>


class Cpu {

public:

	enum MemoryModel {
		mm16_8,
		mm4_32
	};

	struct Registers {
		unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned char d;
		unsigned char e;
		unsigned char f;
		unsigned char h;
		unsigned char l;
		unsigned short sp = 0xFFFE;
		unsigned short pc = 0x100;
	};

	struct Flags {
		bool zero;
		bool negative;
		bool halfCarry;
		bool carry;
	};

	Registers registers;
	Flags flags;
	unsigned char* m_rom;
	int currentRamBank = 0;
	int currentRomBank = 0;
	int numberOfRamBanks = 0;
	bool ramBankEnabled;
	std::vector<unsigned char*> ramBank;	
	int mbc = 0;
	MemoryModel memoryModel;
	bool halted;
	bool interupt;

	void PowerUpSequence();
	void CreateRamBanks();
	short ExecuteOpcode();
	short LD(unsigned char& reg);
	short LDreg(unsigned char& reg1, unsigned char& reg2);
	short LDRegFromMemory(unsigned char& reg, unsigned short address);
	short LDRegFromMemory(unsigned char& reg);
	short LDregHL(unsigned char& reg);
	short LDmemfromreg(unsigned char& reg);
	short LDmem();
	short LDHL();

	short LD16(unsigned char& reg1, unsigned char& reg2);

	short Push(unsigned char& reg1, unsigned char& reg2);
	short Pop(unsigned char& reg1, unsigned char& reg2);

	bool halfCarry(unsigned char value1, unsigned char value2);
	bool halfCarry16(unsigned short value1, unsigned short value2);
	bool carry(unsigned char value1, unsigned char value2);
	bool carry16(unsigned short value1, unsigned short value2);
	bool halfNoBorrow(unsigned char value1, unsigned char value2);
	bool noBorrow(unsigned char value1, unsigned char value2);

	// ALU
	short ADD(unsigned char value);
	short ADDC(unsigned char value);
	short SUB(unsigned char value);
	short SUBC(unsigned char value);
	short AND(unsigned char value);
	short OR(unsigned char value);
	short XOR(unsigned char value);
	short CP(unsigned char value);
	short INC(unsigned char& reg);
	short INCMemory(unsigned short address);
	short DEC(unsigned char& reg);
	short DECMemory(unsigned short address);

	// 16 bit ALU
	short ADDHL(unsigned short value);
	short ADDSP();
	short INC16(unsigned char& reg1, unsigned char& reg2);
	short INC16(unsigned short& reg);
	short DEC16(unsigned char& reg1, unsigned char& reg2);
	short DEC16(unsigned short& reg);

	// Miscellaneous
	short SWAP(unsigned char& reg);
	short SWAPMemory();
	short DAA();
	short CPL();
	short CCF();
	short SCF();
	short NOP();
	short HALT();
	short STOP();
	short DI();
	short EI();

	short decrement16reg(unsigned char& reg1, unsigned char& reg2);
	short increment16reg(unsigned char& reg1, unsigned char& reg2);

	unsigned char MemoryRead(unsigned short address);
	void MemoryWrite(unsigned short address, unsigned char data);

	unsigned short Combinebytes(unsigned char value1, unsigned char value2);
	std::pair<unsigned char, unsigned char> splitBytes(unsigned short value);


};
