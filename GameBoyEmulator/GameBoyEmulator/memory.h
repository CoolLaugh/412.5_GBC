#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include "CommonDefinitions.h"

typedef unsigned char byte;
typedef unsigned short word;

class Memory {

private:

	const int memorySize = 0x10000;
	const int romSize = 0x10000;
	const int gameBankSize = 0x200000;

	struct Buttons {
		bool down = false;
		bool up = false;
		bool left = false;
		bool right = false;
		bool start = false;
		bool select = false;
		bool buttonB = false;
		bool buttonA = false;
	};


public:

	Buttons buttons;

	enum MemoryModel {
		mm16_8,
		mm4_32
	};

	byte* cartridge;
	byte* memorySpace;

	int currentRamBank = 0;
	int currentRomBank = 0;
	int numberOfRamBanks = 0;
	int timerFrequencyChange = 0; // if this is not 0 then the clock frequence in the cpu will change to this
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
	byte GetJoypadState();
	void DumpMemory(std::string fileName = "MemoryDump");

};