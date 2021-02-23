#pragma once
#include "SFML/Graphics.hpp"
#include "CommonDefinitions.h"

class SFMLGraphics {

public:
	sf::RenderWindow* window;

private:

	sf::View* view;
	sf::Uint8* backgroundPixels = nullptr;

public:
	SFMLGraphics();
	~SFMLGraphics();

	void setPixelPointers(byte* mainWindow, byte* tileMemory, byte* BGMap, byte* ColorPalette);
	void clearPixelPointers();

	void setupWindow();

	void updateWindow(int scale);

	void updateWindowGeneric(sf::RenderWindow* screen, sf::View* view, sf::Uint8* pixels, int x, int y, bool display = true);
};

