#pragma once
#include "SFML/Graphics.hpp"
#include "memory.h"

class Graphics {

	sf::RenderWindow* window;
	sf::View* view;
	sf::Image* background;


	const short cyclesPerLine = 456;
	const word screenWidth = 160;
	const word screenHeight = 144;


	short cyclesThisLine = 0;

public:

	Memory* memory;

	Graphics();

	void update(short cyclesThisUpdate);
	void drawScanLine();
	void drawBackground();
	void drawBackground2();
	byte bitData(byte val, byte bit);
	void drawSprites();

};

