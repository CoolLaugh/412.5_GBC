#include "Graphics.h"

Graphics::Graphics() {

	window = new sf::RenderWindow(sf::VideoMode(screenWidth, screenHeight), "Gameboy Emulator");
	window->setPosition(sf::Vector2i(150, 150));
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	view = new sf::View(sf::FloatRect(0, 0, screenWidth, screenHeight));
	window->setView(*view);

	background = new sf::Image();
	background->create(screenWidth, screenHeight, sf::Color::White);

	setupTileWindow();
}

void Graphics::setupTileWindow() {

	tileMemoryWindow = new sf::RenderWindow(sf::VideoMode(128, 192), "VRAM Tiles");
	tileMemoryWindow->setPosition(sf::Vector2i(250, 150));
	tileMemoryWindow->setVerticalSyncEnabled(false);
	tileMemoryWindow->setFramerateLimit(60);

	tileMemoryView = new sf::View(sf::FloatRect(0, 0, 128, 192));
	tileMemoryWindow->setView(*tileMemoryView);

	tileMemoryBackground = new sf::Image();
	tileMemoryBackground->create(128, 192, sf::Color::White);
}

void Graphics::updateTileWindow() {

	tileMemoryWindow->clear();
	tileMemoryWindow->setView(*tileMemoryView);

	for (size_t y = 0; y < 24; y++) {

		for (size_t x = 0; x < 16; x++) {

			short memoryTile = 0x8000 + (y * 0x100) + (x * 0x10);

			for (size_t i = 0; i < 16; i++) {

				int row = i / 2;
				int column = i % 2;

				short address = memoryTile + i;

				byte pixelData = memory->Read(address);

				byte pixel0 = pixelData & 0x3;
				byte pixel1 = (pixelData >> 2) & 0x3;
				byte pixel2 = (pixelData >> 4) & 0x3;
				byte pixel3 = (pixelData >> 6) & 0x3;

				int tileX = x * 8 + (column * 4);
				int tileY = y * 8 + row;

				tileMemoryBackground->setPixel(tileX, tileY, BWPalette[pixel0]);
				tileMemoryBackground->setPixel(tileX + 1, tileY, BWPalette[pixel1]);
				tileMemoryBackground->setPixel(tileX + 2, tileY, BWPalette[pixel2]);
				tileMemoryBackground->setPixel(tileX + 3, tileY, BWPalette[pixel3]);
			}
		}
	}


	sf::Texture tilesTexture;
	tilesTexture.loadFromImage(*tileMemoryBackground);
	sf::Sprite tilesSprite;
	tilesSprite.setTexture(tilesTexture, true);
	tilesSprite.setPosition(0, 0);
	
	tileMemoryWindow->draw(tilesSprite);
	tileMemoryWindow->display();
}

void Graphics::update(short cyclesThisUpdate) { 

	byte lcd = memory->Read(0xFF40);
	if ((lcd & 0x80) == 0x80) { // lcd is running

		cyclesThisLine += cyclesThisUpdate;
	}

	if (memory->Read(0xFF44) == 144) {
		byte interuptFlags = memory->Read(0xFF0F);
		interuptFlags |= 0x1;
		memory->Write(0xFF0F, interuptFlags);
	}

	if (memory->Read(0xFF44) > 153) {
		memory->Write(0xFF44, 0);
	}

	if (cyclesThisLine >= cyclesPerLine) {
		drawScanLine();
		cyclesThisLine = 0;
	}
}

void Graphics::drawScanLine() {

	byte mem = memory->Read(0xFF44);
	mem++;
	memory->Write(0xFF44, mem);

	//drawBackground2();
}

void Graphics::drawBackground() {

	byte startY = memory->Read(0xFF42);
	byte startX = memory->Read(0xFF43);
	byte windowStartY = memory->Read(0xFF4A);
	byte windowStartX = memory->Read(0xFF4B);

	byte LCDC = memory->Read(0xFF40);

	word backgroundMemory = 0;

	bool windowEnabled = (LCDC & 0x10) == 0x10;

	if (windowEnabled) { // window is enabled

		if (LCDC & 0x4 == 0x4) {
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}

		startY = memory->Read(0xFF44) - startY;
	}
	else {

		if (LCDC & 0x20 == 0x20) {
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}

		startY += memory->Read(0xFF44);
	}

	word tileData = startY * 4;

	for (size_t i = 0; i < screenWidth; i++) {

		byte pixelX = i + startX;

		if (windowEnabled == true) {

			if (i >= windowStartX) {
				pixelX = i - windowStartX;
			}
		}
	}
}

void Graphics::drawBackground2() {

	byte scrollY = memory->Read(0xFF42);
	byte scrollX = memory->Read(0xFF43);
	byte LCDC = memory->Read(0xFF40);

	word backgroundMemory = 0;

	bool windowEnabled = (LCDC & 0x10) == 0x10;

	if ((LCDC & 0x20) == 0x20) {
		backgroundMemory = 0x9C00;
	}
	else {
		backgroundMemory = 0x9800;
	}

	//scrollY += memory->Read(0xFF44);

	for (size_t i = 0; i < screenWidth; i++) {

		byte startX = scrollX + i;

		word tileIndex = memory->Read(backgroundMemory + (scrollY * 4) + (startX / 8));

		word tileLocation = (tileIndex * 16);

		byte tileLine = scrollY % 8;
		tileLine *= 2;
		byte data1 = memory->Read(tileLocation + tileLine);
		byte data2 = memory->Read(tileLocation + tileLine + 1);

		byte color = bitData(data1, startX % 8);
		color <<= 1;
		color |= bitData(data2, startX % 8);

		switch (color) {
		case 0:
			background->setPixel(scrollX, scrollY, sf::Color(0xFF, 0xFF, 0xFF));
			break;
		case 1:
			background->setPixel(scrollX, scrollY, sf::Color((0xFF / 4) * 3, (0xFF / 4) * 3, (0xFF / 4) * 3));
			break;
		case 2:
			background->setPixel(scrollX, scrollY, sf::Color((0xFF / 4) * 2, (0xFF / 4) * 2, (0xFF / 4) * 2));
			break;
		case 3:
			background->setPixel(scrollX, scrollY, sf::Color((0xFF / 4) , (0xFF / 4), (0xFF / 4)));
			break;
		default:
			break;
		}
	}

}

byte Graphics::bitData(byte val, byte bit) {
	
	byte testBit = 0x80 >> bit;
	
	if (val & testBit == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void Graphics::drawSprites() {


}
