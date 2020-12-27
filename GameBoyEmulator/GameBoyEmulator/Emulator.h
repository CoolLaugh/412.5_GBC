#pragma once
#include <string>
#include "cpu.h"

class Emulator {

	Cpu cpu;

public:

	Emulator();
	~Emulator();
	void Update();

};

