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
	setupBGMapWindow();
}

void Graphics::setupTileWindow() {

	tileMemoryWindow = new sf::RenderWindow(sf::VideoMode((128 + 16) * scale, (192 + 24) * scale), "VRAM Tiles");
	tileMemoryWindow->setPosition(sf::Vector2i(350, 150));
	tileMemoryWindow->setVerticalSyncEnabled(false);
	tileMemoryWindow->setFramerateLimit(60);

	tileMemoryView = new sf::View(sf::FloatRect(0, 0, 128 + 16, 192 + 24));
	tileMemoryWindow->setView(*tileMemoryView);

	tileMemoryBackground = new sf::Image();
	tileMemoryBackground->create(128 + 16, 192 + 24, sf::Color::Magenta);
}

void Graphics::updateTileWindow() {

	tileMemoryWindow->clear();
	tileMemoryWindow->setView(*tileMemoryView);

	for (size_t y = 0; y < 24; y++) {

		for (size_t x = 0; x < 16; x++) {

			short memoryTile = 0x8000 + (y * 0x100) + (x * 0x10);

			int tileX = x * 8;
			int tileY = y * 8;

			for (size_t i = 0; i < 8; i++) {

				short address = memoryTile + i * 2;

				byte pixelDataLow = memory->Read(address);
				byte pixelDataHigh = memory->Read(address + 1);

				for (size_t j = 0; j < 8; j++) {

					byte pixel = 0;
					if ((pixelDataLow & (0x80 >> j)) != 0) {
						pixel |= 0x1;
					}
					if ((pixelDataHigh & (0x80 >> j)) != 0) {
						pixel |= 0x2;
					}

					tileMemoryBackground->setPixel(x + tileX + j, y + tileY + i, BWPalette[pixel]);
				}

			}
		}
	}

	for (size_t y = 0; y < 192 + 24; y++) {
		for (size_t x = 0; x < 128 + 16; x++) {

			if ((y + 1) % 9 == 0 || (x + 1) % 9 == 0) {
				tileMemoryBackground->setPixel(x, y, sf::Color::Red);
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

void Graphics::setupBGMapWindow() {

	BGMapWindow = new sf::RenderWindow(sf::VideoMode(256 * scale, 256 * scale), "VRAM BGMap");
	BGMapWindow->setPosition(sf::Vector2i(150, 350));
	BGMapWindow->setVerticalSyncEnabled(false);
	BGMapWindow->setFramerateLimit(60);

	BGMapView = new sf::View(sf::FloatRect(0, 0, 256, 256));
	BGMapWindow->setView(*BGMapView);

	BGMapBackground = new sf::Image();
	BGMapBackground->create(256, 256, sf::Color::Magenta);
}

void Graphics::updateBGMapWindow() {

	BGMapWindow->clear();
	BGMapWindow->setView(*BGMapView);

	for (size_t y = 0; y < 32; y++) {

		for (size_t x = 0; x < 32; x++) {

			short address = 0x9800 + y * 32 + x;

			byte tileNumber = memory->Read(address);

			short memoryTile = 0x8000 + tileNumber * 16;

			int tileX = x * 8;
			int tileY = y * 8;

			for (size_t i = 0; i < 8; i++) {

				short address = memoryTile + i * 2;

				byte pixelDataLow = memory->Read(address);
				byte pixelDataHigh = memory->Read(address + 1);

				for (size_t j = 0; j < 8; j++) {

					byte pixel = 0;
					if ((pixelDataLow & (0x80 >> j)) != 0) {
						pixel |= 0x1;
					}
					if ((pixelDataHigh & (0x80 >> j)) != 0) {
						pixel |= 0x2;
					}

					BGMapBackground->setPixel(tileX + j, tileY + i, BWPalette[pixel]);
				}

			}
		}
	}

	sf::Texture BGMapTexture;
	BGMapTexture.loadFromImage(*BGMapBackground);
	sf::Sprite BGMapSprite;
	BGMapSprite.setTexture(BGMapTexture, true);
	BGMapSprite.setPosition(0, 0);

	BGMapWindow->draw(BGMapSprite);
	BGMapWindow->display();
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

	memory->rom[0xFF44]++;

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
