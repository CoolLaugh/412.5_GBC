#include "Emulator.h"

Emulator::Emulator() {

	gameboy = new Gameboy(Filename);

	graphics.setPixelPointers(gameboy->GetScreenPixels(), gameboy->GetTilePixels(), gameboy->GetBackgroundPixels(), gameboy->GetColorPalettePixels());

	ImGui::SFML::Init(*graphics.window);
}

Emulator::~Emulator() {
	ImGui::SFML::Shutdown();
	delete gameboy;
}

void Emulator::Loop() {

	sf::Clock deltaClock;

	sf::Event event;
	while (graphics.window->isOpen()) {

		while (graphics.window->pollEvent(event)) {

			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				graphics.window->close();
			}
			else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {

				bool pressed = false;
				if (event.type == sf::Event::KeyPressed) {
					pressed = true;
				}

				if (event.key.code == sf::Keyboard::Up) {
					gameboy->SetButtonState(Gameboy::Buttons::up, pressed);
				}
				else if (event.key.code == sf::Keyboard::Down) {
					gameboy->SetButtonState(Gameboy::Buttons::down, pressed);
				}
				else if (event.key.code == sf::Keyboard::Left) {
					gameboy->SetButtonState(Gameboy::Buttons::left, pressed);
				}
				else if (event.key.code == sf::Keyboard::Right) {
					gameboy->SetButtonState(Gameboy::Buttons::right, pressed);
				}
				else if (event.key.code == sf::Keyboard::Z) {
					gameboy->SetButtonState(Gameboy::Buttons::B, pressed);
				}
				else if (event.key.code == sf::Keyboard::X) {
					gameboy->SetButtonState(Gameboy::Buttons::A, pressed);
				}
				else if (event.key.code == sf::Keyboard::C) {
					gameboy->SetButtonState(Gameboy::Buttons::select, pressed);
				}
				else if (event.key.code == sf::Keyboard::V) {
					gameboy->SetButtonState(Gameboy::Buttons::start, pressed);
				}
				else if (event.key.code == sf::Keyboard::F1 && pressed == true) {
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

				graphics.updateWindow();
				if (elapsedFrames % 15 == 0) {
					graphics.updateBGMapWindow();
					graphics.updateTileWindow();
					graphics.updateColorPaletteWindow();
				}
				gameboy->redrawScreen = false;
				elapsedFrames++;
			}
		}

	}
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
					 static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int heightInItems) {
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
					   static_cast<void*>(&values), values.size(), heightInItems);
	}

}


void Emulator::FileSelectWindow() {

	sf::Clock deltaClock;
	bool fileSelected = false;

	sf::Event event;
	while (graphics.window->isOpen() && fileSelected == false) {

		while (graphics.window->pollEvent(event)) {

			graphics.window->clear();

			ImGui::SFML::ProcessEvent(event);
			ImGui::SFML::Update(*graphics.window, deltaClock.restart());

			ImGui::Begin("Rom Select", NULL, ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoResize);

			static int item = 0;
			fs::path gamesFolder = "./gb";
			std::vector<std::string> files;
			for (const auto & entry : fs::directory_iterator(gamesFolder)) {
				std::string file = entry.path().string();
				int pos = file.find_last_of("\\") + 1;
				file = file.substr(pos, file.length() - pos);
				files.push_back(file);
			}
			
			ImGui::ListBox("", &item, files, 22);

			if (ImGui::Button("Open")) {

				fileSelected = true;
				Filename = files[item];
				delete gameboy;
				gameboy = new Gameboy(Filename);
				graphics.setPixelPointers(gameboy->GetScreenPixels(), gameboy->GetTilePixels(), gameboy->GetBackgroundPixels(), gameboy->GetColorPalettePixels());
			}

			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::SetWindowSize(ImVec2(ScreenWidth * 3, ScreenHeight * 3));
			ImGui::End();

			ImGui::SFML::Render(*graphics.window);
			graphics.window->display();
		}
	}
}