#include "Graphics.h"

Graphics::Graphics() {

	window = new sf::RenderWindow(sf::VideoMode(ScreenWidth * DisplayScale, ScreenHeight * DisplayScale), "Gameboy Emulator");
	window->setPosition(sf::Vector2i(150, 150));
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	view = new sf::View(sf::FloatRect(0, 0, ScreenWidth, ScreenHeight));
	window->setView(*view);

	background = new sf::Image();
	background->create(ScreenWidth, ScreenHeight, sf::Color::White);

	setupTileWindow(); 
	setupBGMapWindow();
}

void Graphics::setupTileWindow() {

	tileMemoryWindow = new sf::RenderWindow(sf::VideoMode((128/* + 16*/) * DisplayScale, (192/* + 24*/) * DisplayScale), "VRAM Tiles");
	tileMemoryWindow->setPosition(sf::Vector2i(650, 150));
	tileMemoryWindow->setVerticalSyncEnabled(false);
	tileMemoryWindow->setFramerateLimit(60);

	tileMemoryView = new sf::View(sf::FloatRect(0, 0, 128/* + 16*/, 192/* + 24*/));
	tileMemoryWindow->setView(*tileMemoryView);

	tileMemoryBackground = new sf::Image();
	tileMemoryBackground->create(128/* + 16*/, 192/* + 24*/, sf::Color::Magenta);
}

void Graphics::updateTileWindow() {

	tileMemoryWindow->clear();
	tileMemoryWindow->setView(*tileMemoryView);
	byte BGP = memory->Read(Address::BGWPalette);

	for (size_t y = 0; y < 24; y++) {

		for (size_t x = 0; x < 16; x++) {

			short memoryTile = Address::TilePattern0 + (y * 0x100) + (x * 0x10);

			int tileX = x * 8;
			int tileY = y * 8;

			for (size_t i = 0; i < 8; i++) {

				short address = memoryTile + i * 2;

				byte pixelDataLow = memory->Read(address);
				byte pixelDataHigh = memory->Read(address + 1);

				for (size_t j = 0; j < 8; j++) {

					byte pixel = 0;
					if ((pixelDataLow & (Bits::b7 >> j)) != 0) {
						pixel |= Bits::b0;
					}
					if ((pixelDataHigh & (Bits::b7 >> j)) != 0) {
						pixel |= Bits::b1;
					}

					byte offset = pixel * 2;
					byte color = (BGP & (0x3 << offset)) >> offset;

					tileMemoryBackground->setPixel(/*x + */tileX + j, /*y + */tileY + i, BWPalette[color]);
				}

			}
		}
	}

	//for (size_t y = 0; y < 192 + 24; y++) {
	//	for (size_t x = 0; x < 128 + 16; x++) {

	//		if ((y + 1) % 9 == 0 || (x + 1) % 9 == 0) {
	//			tileMemoryBackground->setPixel(x, y, sf::Color::Red);
	//		}

	//	}
	//}

	sf::Texture tilesTexture;
	tilesTexture.loadFromImage(*tileMemoryBackground);
	sf::Sprite tilesSprite;
	tilesSprite.setTexture(tilesTexture, true);
	tilesSprite.setPosition(0, 0);
	
	tileMemoryWindow->draw(tilesSprite);
	tileMemoryWindow->display();
}

void Graphics::setupBGMapWindow() {

	BGMapWindow = new sf::RenderWindow(sf::VideoMode(256 * DisplayScale, 256 * DisplayScale), "VRAM BGMap");
	BGMapWindow->setPosition(sf::Vector2i(1150, 150));
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
	byte BGP = memory->Read(Address::BGWPalette);
	byte LCDC = memory->Read(Address::LCDC);

	for (size_t y = 0; y < 32; y++) {

		for (size_t x = 0; x < 32; x++) {

			short address = 0x9800 + y * 32 + x;

			short memoryTile = 0;
			if ((LCDC & Bits::b4) == Bits::b4) {

				byte tileNumber = memory->Read(address);
				memoryTile = Address::TilePattern0 + tileNumber * 16;
			}
			else {

				signed char tileNumber = memory->Read(address);
				memoryTile = Address::TilePattern1 + (tileNumber + 0x80) * 16;
			}

			int tileX = x * 8;
			int tileY = y * 8;

			for (size_t i = 0; i < 8; i++) {

				short address = memoryTile + i * 2;

				byte pixelDataLow = memory->Read(address);
				byte pixelDataHigh = memory->Read(address + 1);

				for (size_t j = 0; j < 8; j++) {

					byte pixel = 0;
					if ((pixelDataLow & (Bits::b7 >> j)) != 0) {
						pixel |= Bits::b0;
					}
					if ((pixelDataHigh & (Bits::b7 >> j)) != 0) {
						pixel |= Bits::b1;
					}

					byte offset = pixel * 2;
					byte color = (BGP & (0x3 << offset)) >> offset;

					BGMapBackground->setPixel(tileX + j, tileY + i, BWPalette[color]);
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

void Graphics::updateWindow() {

	window->clear();
	window->setView(*view);
	sf::Texture backgroundTexture;
	backgroundTexture.loadFromImage(*background);
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(backgroundTexture, true);
	backgroundSprite.setPosition(0, 0);
	window->draw(backgroundSprite);
	window->display();
}

void Graphics::update(short cyclesThisUpdate) { 

	byte lcd = memory->Read(Address::LCDC);
	if ((lcd & Bits::b7) == Bits::b7) { // lcd is running

		cyclesThisLine += cyclesThisUpdate;
	}

	if (memory->Read(Address::LY) == ScreenHeight) {
		byte interuptFlags = memory->Read(Address::InteruptFlag);
		interuptFlags |= Bits::b0;
		memory->Write(Address::InteruptFlag, interuptFlags);
	}

	if (memory->Read(Address::LY) > 153) {
		memory->Write(Address::LY, 0);
	}

	if (cyclesThisLine >= cyclesPerLine) {
		drawScanLine();
		cyclesThisLine = 0;
	}
}

void Graphics::drawScanLine() {

	memory->memorySpace[Address::LY]++;

	drawBackground2();
	drawSprites();
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
			backgroundMemory = Address::BGWTileInfo1;
		}
		else {
			backgroundMemory = Address::BGWTileInfo0;
		}

		startY = memory->Read(Address::LY) - startY;
	}
	else {

		if (LCDC & 0x20 == 0x20) {
			backgroundMemory = Address::BGWTileInfo1;
		}
		else {
			backgroundMemory = Address::BGWTileInfo0;
		}

		startY += memory->Read(Address::LY);
	}

	word tileData = startY * 4;

	for (size_t i = 0; i < ScreenWidth; i++) {

		byte pixelX = i + startX;

		if (windowEnabled == true) {

			if (i >= windowStartX) {
				pixelX = i - windowStartX;
			}
		}
	}
}

void Graphics::drawBackground2() {

	byte LY = memory->Read(Address::LY);

	if (LY >= ScreenHeight) {
		return;
	}

	byte scrollY = memory->Read(Address::ScrollX);
	byte scrollX = memory->Read(Address::ScrollY);
	byte LCDC = memory->Read(Address::LCDC);

	word backgroundMemory = 0;

	bool windowEnabled = (LCDC & Bits::b4) == Bits::b4;

	if ((LCDC & Bits::b5) == Bits::b5) {
		backgroundMemory = Address::BGWTileInfo1;
	}
	else { 
		backgroundMemory = Address::BGWTileInfo0;
	}

	//scrollY += memory->Read(Address::LY);
	byte BGP = memory->Read(Address::BGWPalette);

	for (size_t i = 0; i < ScreenWidth; i++) {

		byte startX = (scrollX + i) % ScreenWidth;

		byte TileY = (scrollY + LY) / 8;
		byte TileX = (scrollX + i) / 8;

		word tileLocation = 0;

		if ((LCDC & Bits::b4) == Bits::b4) {

			byte tileIndex = memory->Read(backgroundMemory + (TileY * 32) + (TileX));
			tileLocation = Address::TilePattern0 + (tileIndex * 16);
		}
		else {

			signed char tileIndex = memory->Read(backgroundMemory + (TileY * 32) + (TileX));
			tileLocation = Address::TilePattern1 + ((tileIndex + 0x80) * 16);
		}

		byte tileLine = (scrollY + LY) % 8;
		tileLine *= 2;

		byte pixelDataLow = memory->Read(tileLocation + tileLine);
		byte pixelDataHigh = memory->Read(tileLocation + tileLine + 1);

		byte pixel = 0;
		if ((pixelDataLow & (Bits::b7 >> (i % 8))) != 0) {
			pixel |= Bits::b0;
		}
		if ((pixelDataHigh & (Bits::b7 >> (i % 8))) != 0) {
			pixel |= Bits::b1;
		}

		byte offset = pixel * 2;
		byte color = (BGP & (0x3 << offset)) >> offset;

		background->setPixel(startX, LY, BWPalette[color]);
	}
}

byte Graphics::bitData(byte val, byte bit) {
	
	byte testBit = Bits::b7 >> bit;
	
	if (val & testBit == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void Graphics::drawSprites() {

	byte LCDC = memory->Read(Address::LCDC);
	word spriteAttributeTable = Address::SpriteAttributes;
	byte OGP = memory->Read(Address::OBJPalette0);

	for (size_t i = 0; i < 40; i++) {

		byte positionY = memory->Read(spriteAttributeTable + (i * 4)) - 16;
		byte positionX = memory->Read(spriteAttributeTable + (i * 4) + 1) - 8;
		byte patternNumber = memory->Read(spriteAttributeTable + (i * 4) + 2);
		byte flags = memory->Read(spriteAttributeTable + (i * 4) + 3);

		bool Yflip = (flags & Bits::b6) != 0;
		bool Xflip = (flags & Bits::b5) != 0;

		byte spriteHeight = 8;

		if ((LCDC & Bits::b2) != 0) {
			spriteHeight = 16;
			patternNumber &= ~Bits::b0;
		}

		if ((flags & Bits::b4) != 0) {
			OGP = memory->Read(Address::OBJPalette1);
		}

		byte LY = memory->Read(Address::LY);

		if (LY < positionY + spriteHeight && LY >= positionY) {

			int drawLine = LY - positionY;
			if (Yflip == true) {
				drawLine -= spriteHeight / 2;
				drawLine *= -1;
				drawLine += spriteHeight / 2;
			}

			word tileLocation = Address::TilePattern0 + (patternNumber * 16);

			byte pixelDataLow = memory->Read(tileLocation + drawLine * 2);
			byte pixelDataHigh = memory->Read(tileLocation + drawLine * 2 + 1);

			for (size_t j = 0; j < 8; j++) {

				byte pixel = 0;
				if ((pixelDataLow & (Bits::b7 >> j)) != 0) {
					pixel |= Bits::b0;
				}
				if ((pixelDataHigh & (Bits::b7 >> j)) != 0) {
					pixel |= Bits::b1;
				}
				int pixelOffsetX = j;
				if (Xflip == true) {
					pixelOffsetX -= 8;
					pixelOffsetX *= -1;
					pixelOffsetX += 8;
				}

				int pixelX = positionX + pixelOffsetX;

				byte offset = pixel * 2;
				byte color = (OGP & (0x3 << offset)) >> offset;

				if (LY < 144 && pixelX < 160 && pixel != 0) {
					background->setPixel(pixelX, LY, BWPalette[color]);
				}
			}
		}
	}

}
