#pragma once
#include <string>
#include "cpu.h"
#include "Graphics.h"

class Emulator {

	Cpu cpu;
	Graphics graphics;

	int elapsedFrames = 0;

public:

	Emulator();
	~Emulator();
	void Update();
	void Loop();

	void DumpMemory();
};

