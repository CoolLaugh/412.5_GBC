#pragma once
#include "SFML/Graphics.hpp"
#include "CommonDefinitions.h"

class SFMLGraphics {

public:
	sf::RenderWindow* window;

private:

	sf::View* view;
	sf::Uint8* backgroundPixels;

	sf::RenderWindow* tileMemoryWindow;
	sf::View* tileMemoryView;
	sf::Uint8* tileMemoryBackgroundPixels;

	sf::RenderWindow* BGMapWindow;
	sf::View* BGMapView;
	sf::Uint8* BGMapBackgroundPixels;

	sf::RenderWindow* ColorPaletteWindow;
	sf::View* ColorPaletteView;
	sf::Uint8* ColorPaletteBackgroundPixels;

public:
	SFMLGraphics();

	void setPixelPointers(byte* mainWindow, byte* tileMemory, byte* BGMap, byte* ColorPalette);

	void setupWindow();
	void setupTileWindow();
	void setupBGMapWindow();
	void setupColorPaletteWindow();

	void updateWindow(int scale);
	void updateTileWindow();
	void updateBGMapWindow();
	void updateColorPaletteWindow();

	void updateWindowGeneric(sf::RenderWindow* screen, sf::View* view, sf::Uint8* pixels, int x, int y, bool display = true);
};

