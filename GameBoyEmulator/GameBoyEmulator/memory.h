#pragma once
#include <vector>
#include <fstream>

typedef unsigned char byte;
typedef unsigned short word;

class Memory {

private:

	const int romSize = 0x10000;
	const int gameBankSize = 0x200000;


public:

	enum MemoryModel {
		mm16_8,
		mm4_32
	};

	byte* rom;
	byte* gameBank;
	int currentRamBank = 0;
	int currentRomBank = 0;
	int numberOfRamBanks = 0;
	bool ramBankEnabled;
	std::vector<byte*> ramBank;
	int mbc = 0;
	MemoryModel memoryModel;

	Memory(); 
	void PowerUpSequence();
	bool LoadRom(const std::string fileName);
	void CreateRamBanks();
	byte Read(word address);
	void Write(word address, byte data);

};