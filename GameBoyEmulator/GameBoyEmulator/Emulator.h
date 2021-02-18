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

	Gameboy* gameboy = nullptr;
	SFMLGraphics graphics;

	std::string Filename = "";
	std::map<Gameboy::Buttons, sf::Keyboard::Key> keybindings;
	std::map<Gameboy::Buttons, bool> keyListening;
	sf::Texture BackgroundTexture;
	sf::Texture TileTexture;
	sf::Texture ColorTexture;
	int elapsedFrames = 0;
	int scale = 4;
	int framerateLimit = 60;

	bool showSettings = false;
	bool showFileOpen = false;
	bool showKeyBinds = false;
	bool showTileWindow = false;
	bool showBackgroundWindow = false;
	bool showColorPalettteWindow = false;
	bool showAbout = false;
	bool showAudioWindow = true;

	bool menuTest = false;

public:

	Emulator();
	~Emulator();
	void Loop();
	void MainMenuBar(sf::Keyboard::Key keyPressed);
	void FileSelectWindow();
	void SettingsMenu();

	void ToolBar();
	void FileOpen();
	void Keybindings(sf::Keyboard::Key keyPressed);
	void about();

	void SetKey(std::string buttonName, std::string currentKey, Gameboy::Buttons button, sf::Keyboard::Key keyPressed);
	void SaveStateMenuItem(int number);
	void LoadStateMenuItem(int number);

	void TileWindow();
	void BackgroundWindow();
	void ColorPaletteWindow();
	void Channel();

};

