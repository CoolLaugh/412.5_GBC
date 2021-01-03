#pragma once
#include <string>
#include "cpu.h"
#include "Graphics.h"

class Emulator {

	Cpu cpu;
	Graphics graphics;

public:

	Emulator();
	~Emulator();
	void Update();

};

