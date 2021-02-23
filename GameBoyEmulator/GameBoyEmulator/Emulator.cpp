#include "Emulator.h"

Emulator::Emulator() {

	//gameboy = new Gameboy(Filename);

	//graphics.setPixelPointers(gameboy->GetScreenPixels(), gameboy->GetTilePixels(), gameboy->GetBackgroundPixels(), gameboy->GetColorPalettePixels());

	ImGui::SFML::Init(*graphics.window);

	keybindings[Gameboy::Buttons::up] = sf::Keyboard::Key::Up;
	keybindings[Gameboy::Buttons::down] = sf::Keyboard::Key::Down;
	keybindings[Gameboy::Buttons::left] = sf::Keyboard::Key::Left;
	keybindings[Gameboy::Buttons::right] = sf::Keyboard::Key::Right;
	keybindings[Gameboy::Buttons::B] = sf::Keyboard::Key::Z;
	keybindings[Gameboy::Buttons::A] = sf::Keyboard::Key::X;
	keybindings[Gameboy::Buttons::select] = sf::Keyboard::Key::C;
	keybindings[Gameboy::Buttons::start] = sf::Keyboard::Key::V;
}

Emulator::~Emulator() {
	ImGui::SFML::Shutdown();
	if (gameboy != nullptr) {
		delete gameboy;
	}
}

void Emulator::Loop() {

	sf::Clock deltaClock;

	sf::Event event;
	while (graphics.window->isOpen()) {

		sf::Keyboard::Key keyPressed = sf::Keyboard::Key::Unknown;
		while (graphics.window->pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				graphics.window->close();
			}

			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {

				bool pressed = false;
				if (event.type == sf::Event::KeyPressed) {
					pressed = true;
					keyPressed = event.key.code;
				}

				for (auto iter = keybindings.begin(); iter != keybindings.end(); iter++) {
					if (event.key.code == iter->second) {
						gameboy->SetButtonState(iter->first, pressed);
					}
				}

				if (event.key.code == sf::Keyboard::F1 && pressed == true) {
					gameboy->SaveState(1);
				}
				else if (event.key.code == sf::Keyboard::F2 && pressed == true) {
					gameboy->LoadState(1);
				}
				else if (event.key.code == sf::Keyboard::F3 && pressed == true) {
					gameboy->memory.DumpMemory();
				}
				else if (event.key.code == sf::Keyboard::F5 && pressed == true) {
					gameboy->cpu.logState = !gameboy->cpu.logState;
					if (gameboy->cpu.logState == true) {
						gameboy->cpu.cleanOutputState();
					}
				}
				else if (event.key.code == sf::Keyboard::F8 && pressed == true) {
					if (framerateLimit == 60) {
						framerateLimit = 600;
					}
					else {
						framerateLimit = 60;
					}
					graphics.window->setFramerateLimit(framerateLimit);
				}
			}
			else if (event.type == sf::Event::GainedFocus || event.type == sf::Event::LostFocus) {
				gameboy->SetButtonState(Gameboy::Buttons::up, false);
				gameboy->SetButtonState(Gameboy::Buttons::down, false);
				gameboy->SetButtonState(Gameboy::Buttons::left, false);
				gameboy->SetButtonState(Gameboy::Buttons::right, false);
				gameboy->SetButtonState(Gameboy::Buttons::B, false);
				gameboy->SetButtonState(Gameboy::Buttons::A, false);
				gameboy->SetButtonState(Gameboy::Buttons::select, false);
				gameboy->SetButtonState(Gameboy::Buttons::start, false);
			}
		}
		for (size_t i = 0; i < 54; i++) { // run the gameboy for 1 frame before checking the inputs again

			gameboy->Advance(456);
			if (gameboy->redrawScreen == true) {

				graphics.updateWindow(scale);
				ImGui::SFML::Update(*graphics.window, deltaClock.restart());
				MainMenuBar(keyPressed);
				ImGui::SFML::Render(*graphics.window);
				graphics.window->display();
				gameboy->redrawScreen = false;
				elapsedFrames++;
			}
		}

	}
}

void Emulator::MainMenuBar(sf::Keyboard::Key keyPresse) {

	ToolBar();
	FileOpen();
	Keybindings(keyPresse);
	TileWindow();
	BackgroundWindow();
	ColorPaletteWindow();
	about();
	Channel();
	SettingsMenu();
}

// https://eliasdaler.github.io/using-imgui-with-sfml-pt2/ 
namespace ImGui {
	static auto vector_getter = [](void* vec, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
					 static_cast<void*>(&values), (int)values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int heightInItems) {
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
					   static_cast<void*>(&values), (int)values.size(), heightInItems);
	}

}


void Emulator::FileSelectWindow() {

	sf::Clock deltaClock;

	sf::Event event;
	while (Filename == "") {

		sf::Keyboard::Key keyPressed = sf::Keyboard::Key::Unknown;
		while (graphics.window->pollEvent(event)) {

			ImGui::SFML::ProcessEvent(event); 

			if (event.type == sf::Event::KeyPressed) {
				keyPressed = event.key.code;
			}
		}
		graphics.window->clear();
		ImGui::SFML::Update(*graphics.window, deltaClock.restart());
		MainMenuBar(keyPressed);
		ImGui::SFML::Render(*graphics.window);
		graphics.window->display();
	}

}

void Emulator::SettingsMenu() {

	if (showSettings == false) {
		return;
	}

	ImGui::Begin("Settings", &showSettings, 0);

	ImGui::Text("Settings");
	if (ImGui::SliderInt("Scale", &scale, 2, 9)) {
		graphics.window->setSize(sf::Vector2u(ScreenWidth * scale, ScreenHeight * scale + 20));
	}
	ImGui::End();

}

void Emulator::ToolBar() {

	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("Open", NULL, &showFileOpen);
			if (ImGui::MenuItem("Reset", NULL, false)) {
				if (gameboy != nullptr) {
					graphics.clearPixelPointers();
					delete gameboy;
					gameboy = new Gameboy(Filename);
					graphics.setPixelPointers(gameboy->GetScreenPixels(), gameboy->GetTilePixels(), gameboy->GetBackgroundPixels(), gameboy->GetColorPalettePixels());
				}
			}
			if (ImGui::MenuItem("Exit", NULL, false)) {
				graphics.window->close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("State")) {
			if (ImGui::BeginMenu("Save State")) {
				for (int i = 0; i < 10; i++) {
					SaveStateMenuItem(i);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Load State")) {
				for (int i = 0; i < 10; i++) {
					LoadStateMenuItem(i);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings")) {
			ImGui::MenuItem("Graphics", NULL, &showSettings);
			ImGui::MenuItem("Keys", NULL, &showKeyBinds);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			ImGui::MenuItem("Tiles", NULL, &showTileWindow);
			ImGui::MenuItem("Background Map", NULL, &showBackgroundWindow);
			ImGui::MenuItem("ColorPalette(CGB)", NULL, &showColorPalettteWindow);
			ImGui::MenuItem("Audio", NULL, &showAudioWindow);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("?")) {
			ImGui::MenuItem("About", NULL, &showAbout);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void Emulator::FileOpen() {

	if (showFileOpen == false) {
		return;
	}

	ImGui::Begin("Open File", &showFileOpen, 0);

	static int item = 0;
	fs::path gamesFolder = "./gb";
	std::vector<std::string> files;
	for (const auto & entry : fs::directory_iterator(gamesFolder)) {
		std::string file = entry.path().string();
		if (file.find(".gb") != std::string::npos) {

			int pos = (int)file.find_last_of("\\") + 1;
			file = file.substr(pos, file.length() - pos);
			files.push_back(file);
		}
	}

	ImGui::ListBox("", &item, files, 20);
	
	if (ImGui::Button("Open")) {
		showFileOpen = false;
		Filename = files[item];
		delete gameboy;
		gameboy = new Gameboy(Filename);
		graphics.setPixelPointers(gameboy->GetScreenPixels(), gameboy->GetTilePixels(), gameboy->GetBackgroundPixels(), gameboy->GetColorPalettePixels());
		graphics.window->setTitle(TitleName + " - " + Filename);
	}


	ImGui::End();

}

void Emulator::Keybindings(sf::Keyboard::Key keyPressed) {

	if (showKeyBinds == false) {
		return;
	}

	ImGui::Begin("Keybinds", &showKeyBinds, 0);
	SetKey("Up", "c", Gameboy::Buttons::up, keyPressed);
	SetKey("Down", "c", Gameboy::Buttons::down, keyPressed);
	SetKey("Left", "c", Gameboy::Buttons::left, keyPressed);
	SetKey("Right", "c", Gameboy::Buttons::right, keyPressed);
	SetKey("A", "c", Gameboy::Buttons::A, keyPressed);
	SetKey("B", "c", Gameboy::Buttons::B, keyPressed);
	SetKey("Start", "c", Gameboy::Buttons::start, keyPressed);
	SetKey("Select", "c", Gameboy::Buttons::select, keyPressed);
	ImGui::End();
}

void Emulator::about() {

	if (showAbout == false) {
		return;
	}

	ImGui::Begin("About", &showAbout, 0);
	ImGui::Text("Gameboy Emulator");
	ImGui::Text("Programmed by Zach Walters");
	ImGui::Text("Emulator for the Gameboy and Gameboy Color systems");
	ImGui::Text("Licensed under GNU GENERAL PUBLIC LICENSE Version 3");
	ImGui::Text("\"Gameboy\" is a trademark of Nintendo");
	ImGui::Text("I'm not affiliated with Nintendo in any way");
	ImGui::End();
}

void Emulator::SetKey(std::string buttonName, std::string currentKey, Gameboy::Buttons button, sf::Keyboard::Key keyPressed) {

	ImGui::Text((buttonName + ": ").c_str());
	ImGui::SameLine();
	if (keyListening[button] == true) {

		ImGui::Button("Listening");
		if (keyPressed != sf::Keyboard::Key::Unknown) {
			keybindings[button] = keyPressed;
			keyListening[button] = false;
		}
	}
	else {

		if (ImGui::Button(SFMLKeyNames[keybindings[button]].c_str())) {
			keyListening[button] = true;
		}
	}
}

void Emulator::SaveStateMenuItem(int number) {

	if (gameboy == nullptr) {
		return;
	}

	if (ImGui::MenuItem(("Save State #" + std::to_string(number)).c_str(), NULL, false)) {
		gameboy->SaveState(number);
	}
}

void Emulator::LoadStateMenuItem(int number) {

	if (gameboy == nullptr) {
		return;
	}

	if (ImGui::MenuItem(("Load State #" + std::to_string(number)).c_str(), NULL, false)) {
		gameboy->LoadState(number);
	}
}

void Emulator::TileWindow() {

	if (showTileWindow == false || gameboy == nullptr) {
		return;
	}

	ImGui::Begin("Tile Memory", &showTileWindow, 0);

	if (gameboy != nullptr) {
		gameboy->ppu.updateTileImage();
		byte* tiles = gameboy->GetTilePixels();
		sf::Image image;
		image.create(256, 192, tiles);
		TileTexture.loadFromImage(image);
		ImGui::Image(TileTexture, ImVec2(256 * 2, 192 * 2));
	}

	ImGui::End();
}

void Emulator::BackgroundWindow() {

	if (showBackgroundWindow == false || gameboy == nullptr) {
		return;
	}

	ImGui::Begin("Background Map Memory", &showBackgroundWindow, 0);

	if (gameboy != nullptr) {
		gameboy->ppu.updateBGMapImage();
		sf::Image image;
		image.create(256, 256, gameboy->GetBackgroundPixels());
		BackgroundTexture.loadFromImage(image);
		ImGui::Image(BackgroundTexture, ImVec2(256 * 2, 256 * 2));
	}

	ImGui::End();
}

void Emulator::ColorPaletteWindow() {

	if (showColorPalettteWindow == false) {
		return;
	}

	ImGui::Begin("Color Palette Memory", &showColorPalettteWindow, 0);

	if (gameboy != nullptr) {

		gameboy->ppu.updateColorPaletteImage();
		byte* colors = gameboy->GetColorPalettePixels();
		sf::Image image;
		image.create(4, 16, colors);
		ColorTexture.loadFromImage(image);
		ImGui::Image(ColorTexture, ImVec2(4 * 12, 16 * 12));
	}

	ImGui::End();
}

void Emulator::Channel() {

	if (showAudioWindow == false) {
		return;
	}

	ImGui::Begin("Audio Channels", &showAudioWindow, 0);
	if (gameboy != nullptr && 
		gameboy->apu.channel1.displayBuffer.size() > 0) {
		ImGui::Checkbox("Mute All", &gameboy->apu.muteAll);
		ImGui::PlotLines("Channel 1", &gameboy->apu.channel1.displayBuffer[0], (int)gameboy->apu.channel1.displayBuffer.size());
		ImGui::SameLine();
		ImGui::Checkbox("Mute##1", &gameboy->apu.channel1.mute);
		ImGui::PlotLines("Channel 2", &gameboy->apu.channel2.displayBuffer[0], (int)gameboy->apu.channel2.displayBuffer.size());
		ImGui::SameLine();
		ImGui::Checkbox("Mute##2", &gameboy->apu.channel2.mute);
		ImGui::PlotLines("Channel 3", &gameboy->apu.channel3.displayBuffer[0], (int)gameboy->apu.channel3.displayBuffer.size());
		ImGui::SameLine();
		ImGui::Checkbox("Mute##3", &gameboy->apu.channel3.mute);
		ImGui::PlotLines("Channel 4", &gameboy->apu.channel4.displayBuffer[0], (int)gameboy->apu.channel4.displayBuffer.size());
		ImGui::SameLine();
		ImGui::Checkbox("Mute##4", &gameboy->apu.channel4.mute);
		ImGui::SliderInt("Volume", &gameboy->apu.scale, 0, 1000);
		ImGui::SliderInt("Sound Buffer", &gameboy->apu.BufferAmount, 1000, 44100);
	}

	ImGui::End();
}
