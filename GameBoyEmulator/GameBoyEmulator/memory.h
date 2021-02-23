#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include "CommonDefinitions.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

class Memory {

private:


	const int memorySize = 0x10000;
	const int RamBankSize = 0x2000;

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
	bool externalRamChanged = false;
	bool ColorGameBoyMode = false;
	byte currentRamBank = 0;
	byte currentRamBankB = 0;
	byte currentWramBank = 1;
	byte currentVramBank = 0;
	word currentRomBank = 0;
	byte numberOfRamBanks = 0;
	bool ramBankEnabled;
	std::vector<byte*> ramBank;
	std::vector<byte*> wramBank;
	std::vector<byte*> vramBank;
	byte mbc = 0;
	MemoryModel memoryModel;
	word dividerRegister = 0;
	bool timerOverflow = false;
	int timerOverflowCounter = 0;
	bool writingToTIMA = false;
	int writingToTIMACounter = 0;
	bool oldBit = false;
	bool resetSC1Length = false;
	bool resetSC2Length = false;
	bool resetSC3Length = false;
	bool resetSC4Length = false;

	Memory();
	~Memory();
	void PowerUpSequence();
	bool LoadRom(const std::string fileName);
	void CreateRamBanks();

	byte Read(word address, int vRamBank = -1);

	void Write(word address, byte data);
	void WriteMBCSwitch(word address, byte data);
	void mbc1_00001FFF(byte data);
	void mbc1_20003FFF(word address, byte data);
	void mbc1_40005FFF(byte data);
	void mbc1_60007FFF(byte data);
	void mbc2_00003FFF(word address, byte data);
	void mbc3_00001FFF(byte data);
	void mbc3_20003FFF(byte data);
	void mbc3_40005FFF(byte data);
	void mbc3_60007FFF(byte data);
	void mbc5_00001FFF(byte data);
	void mbc5_20002FFF(byte data);
	void mbc5_30003FFF(byte data);
	void mbc5_40005FFF(byte data);

	byte GetJoypadState();
	void IncrementDivAndTimerRegisters(byte clocks);
	void DumpMemory(std::string fileName = "MemoryDump");
	void DumpStack(word spAddress, std::string fileName = "StackDump");

	void SaveExternalRam(std::string fileName);
	void LoadExternalRam(std::string fileName);

	word vramDMATransferSource;
	word vramDMATransferDestination;
	word vramDMATransferLength;
	word vramDMATransferProgress;
	void HBlankDMA();
};