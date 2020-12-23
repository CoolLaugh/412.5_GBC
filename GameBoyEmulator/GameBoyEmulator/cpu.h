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

	void PowerUpSeq();
	void CreateRamBanks();
	short ExecuteOpcode();
	short LD(unsigned char& reg);
	short LDreg(unsigned char& reg1, unsigned char& reg2);
	short LDregHL(unsigned char& reg);
	short LDmemfromreg(unsigned char& reg);
	short LDmem();

	unsigned char MemoryRead(short address);
	void MemoryWrite(short address, unsigned char data);
	unsigned short CombineReg(unsigned char& reg1, unsigned char& reg2);
};

