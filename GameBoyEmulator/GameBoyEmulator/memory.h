#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include "CommonDefinitions.h"

class Memory {

private:

	const int memorySize = 0x10000;

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
	byte* BGColorPalette;
	byte* SpriteColorPalette;

	bool stopHblankDMA = false;
	bool TileChanged = false;
	bool ColorGameBoyMode = false;
	byte currentRamBank = 0;
	byte currentWramBank = 1;
	byte currentVramBank = 0;
	byte currentRomBank = 0;
	byte numberOfRamBanks = 0;
	bool ramBankEnabled;
	std::vector<byte*> ramBank;
	std::vector<byte*> wramBank;
	std::vector<byte*> vramBank;
	byte mbc = 0;
	MemoryModel memoryModel;
	word dividerRegister = 0;
	bool timerOverflow = false;
	bool writingToTIMA = false;
	bool oldBit = false;

	Memory(); 
	void PowerUpSequence();
	bool LoadRom(const std::string fileName);
	void CreateRamBanks();
	byte Read(word address, int vRamBank = -1);
	void Write(word address, byte data);
	byte GetJoypadState();
	void IncrementDivAndTimerRegisters(byte clocks);
	void DumpMemory(std::string fileName = "MemoryDump");
	void DumpStack(word spAddress, std::string fileName = "StackDump");

	word vramDMATransferSource;
	word vramDMATransferDestination;
	word vramDMATransferLength;
	word vramDMATransferProgress;
	void HBlankDMA();
};