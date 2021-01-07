#pragma once
#include "SFML/Graphics.hpp"
#include "memory.h"

class Graphics {

public:
	sf::RenderWindow* window;

private:
	sf::View* view;
	sf::Image* background;

	sf::RenderWindow* tileMemoryWindow;
	sf::View* tileMemoryView;
	sf::Image* tileMemoryBackground;

	const short cyclesPerLine = 456;
	const word screenWidth = 160;
	const word screenHeight = 144;


	short cyclesThisLine = 0;

	sf::Color BWPalette[4] = { sf::Color(0xFF,0xFF,0xFF),
							sf::Color(0xAA,0xAA,0xAA), 
							sf::Color(0x55,0x55,0x55), 
							sf::Color(0x00,0x00,0x00)};

public:

	Memory* memory;

	Graphics();

	void setupTileWindow();
	void updateTileWindow();

	void update(short cyclesThisUpdate);
	void drawScanLine();
	void drawBackground();
	void drawBackground2();
	byte bitData(byte val, byte bit);
	void drawSprites();

};

