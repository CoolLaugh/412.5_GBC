#pragma once
#include <string>
#include <fstream>
#include "cpu.h"

class Emulator {


	Cpu cpu;

public:
	unsigned char* m_GameBank;
	unsigned char* m_Rom;

	Emulator();
	~Emulator();
	bool LoadRom(const std::string fileName);
	void Update();

};

