#pragma once
#include <string>
#include "cpu.h"
#include "ppu.h"
#include "SFMLGraphics.h"
#include "Gameboy.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

class Emulator {

	Gameboy* gameboy;
	SFMLGraphics graphics;

	std::string Filename = "";
	int elapsedFrames = 0;

public:

	Emulator();
	~Emulator();
	void Loop();
	void FileSelectWindow();
};

