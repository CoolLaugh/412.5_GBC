#pragma once
#include <vector>
#include <fstream>

typedef unsigned char byte;
typedef unsigned short word;

class Memory {

private:



public:

	enum MemoryModel {
		mm16_8,
		mm4_32
	};

	byte* m_rom;
	byte* m_GameBank;
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