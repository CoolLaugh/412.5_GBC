#pragma once
#include "SFML/Graphics.hpp"
#include "memory.h"
#include "CommonDefinitions.h"

class Graphics {

public:
	sf::RenderWindow* window;
	sf::RenderWindow* tileMemoryWindow;
	sf::RenderWindow* BGMapWindow;

	short cyclesThisLine = 0;

private:
	sf::View* view;
	sf::Image* background;

	sf::View* tileMemoryView;
	sf::Image* tileMemoryBackground;

	sf::View* BGMapView;
	sf::Image* BGMapBackground;

	const short cyclesPerLine = 456;



	sf::Color BWPalette[4] = { sf::Color(0xFF,0xFF,0xFF),
							sf::Color(0xAA,0xAA,0xAA), 
							sf::Color(0x55,0x55,0x55), 
							sf::Color(0x00,0x00,0x00)};

public:

	Memory* memory;

	Graphics();

	void setupTileWindow();
	void updateTileWindow();

	void setupBGMapWindow();
	void updateBGMapWindow();

	void updateWindow();

	void update(short cyclesThisUpdate);
	void drawScanLine();
	void drawBackground();
	void drawBackground2();
	byte bitData(byte val, byte bit);
	void drawSprites();

};

