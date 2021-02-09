#include "Graphics.h"

Graphics::Graphics() {

	window = new sf::RenderWindow(sf::VideoMode(ScreenWidth * DisplayScale, ScreenHeight * DisplayScale), "Gameboy Emulator");
	window->setPosition(sf::Vector2i(950, 150));
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	view = new sf::View(sf::FloatRect(0, 0, ScreenWidth, ScreenHeight));
	window->setView(*view);

	background = new sf::Image();
	background->create(ScreenWidth, ScreenHeight, sf::Color::White);

	backgroundPixels = new sf::Uint8[ScreenWidth * ScreenHeight * 4];
	backgroundPixelsColorIndex = new byte[ScreenWidth * ScreenHeight];

	setupTileWindow(); 
	setupBGMapWindow();
	setupColorPaletteWindow();
}

void Graphics::setupTileWindow() {

	tileMemoryWindow = new sf::RenderWindow(sf::VideoMode((256) * DisplayScale, (192) * DisplayScale), "VRAM Tiles");
	tileMemoryWindow->setPosition(sf::Vector2i(150, 150));
	tileMemoryWindow->setVerticalSyncEnabled(false);
	tileMemoryWindow->setFramerateLimit(60);

	tileMemoryView = new sf::View(sf::FloatRect(0, 0, 256, 192));
	tileMemoryWindow->setView(*tileMemoryView);

	tileMemoryBackground = new sf::Image();
	tileMemoryBackground->create(256, 192, sf::Color::Magenta);
}

void Graphics::updateTileWindow() {

	tileMemoryWindow->clear();
	tileMemoryWindow->setView(*tileMemoryView);
	byte BGP = memory->Read(Address::BGWPalette);

	int vramBankCount = 1;
	if (ColorGameBoyMode == true) {
		vramBankCount = 2;
	}
	byte preserveBankNumber = memory->currentVramBank;
	for (size_t bank = 0; bank < vramBankCount; bank++) {

		memory->currentVramBank = bank;
		for (word y = 0; y < 24; y++) {

			for (word x = 0; x < 16; x++) {

				short memoryTile = Address::TilePattern0 + (y * 0x100) + (x * 0x10);

				word tileX = x * 8;
				word tileY = y * 8;
				sf::Color* palette = GetBGPalette(memory->vramBank[1][(0x9800 + y * 16 + x) - 0x8000]);

				for (word i = 0; i < 8; i++) {

					word address = memoryTile + i * 2;

					byte pixelDataLow = memory->Read(address);
					byte pixelDataHigh = memory->Read(address + 1);

					for (word j = 0; j < 8; j++) {

						byte pixel = 0;
						if ((pixelDataLow & (Bits::b7 >> j)) != 0) {
							pixel |= Bits::b0;
						}
						if ((pixelDataHigh & (Bits::b7 >> j)) != 0) {
							pixel |= Bits::b1;
						}
						if (ColorGameBoyMode) {

							tileMemoryBackground->setPixel((bank * 128) + tileX + j, tileY + i, palette[pixel]);
						}
						else {

							byte offset = pixel * 2;
							byte color = (BGP & (0x3 << offset)) >> offset;

							tileMemoryBackground->setPixel((bank * 128) + tileX + j, tileY + i, BWPalette[color]);
						}
					}

				}
			}
		}
	}
	memory->currentVramBank = preserveBankNumber;


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
	BGMapWindow->setPosition(sf::Vector2i(1450, 150));
	BGMapWindow->setVerticalSyncEnabled(false);
	BGMapWindow->setFramerateLimit(60);

	BGMapView = new sf::View(sf::FloatRect(0, 0, 256, 256));
	BGMapWindow->setView(*BGMapView);

	BGMapBackground = new sf::Image();
	BGMapBackground->create(256, 256, sf::Color::Magenta);

	BGMapBackgroundPixels = new	sf::Uint8[256 * 256 * 4];
}

void Graphics::updateBGMapWindow() {

	BGMapWindow->clear();
	BGMapWindow->setView(*BGMapView);

	for (size_t i = 0; i < 256; i++) {
		DrawBackgroundLine(0, i, i, 256, BGMapBackgroundPixels);
	}	
	
	//draw gameboy screen outline
	byte scrollX = memory->Read(Address::ScrollX);
	byte scrollY = memory->Read(Address::ScrollY);

	for (size_t i = 0; i < 2; i++) {

		int y = (scrollY + (i * ScreenHeight)) % 256;

		for (size_t xPos = 0; xPos < ScreenWidth; xPos++) {

			int x = (scrollX + xPos) % 256;

			SetPixel(BGMapBackgroundPixels, x, y, 256, { 0xFF, 0x00, 0x00 });
		}
	}

	for (size_t i = 0; i < 2; i++) {

		int x = (scrollX + (ScreenWidth * i)) % 256;

		for (size_t yPos = 0; yPos < ScreenHeight; yPos++) {

			int y = (scrollY + yPos) % 256;

			SetPixel(BGMapBackgroundPixels, x, y, 256, { 0xFF, 0x00, 0x00 });
		}
	}

	BGMapBackground->create(256, 256, BGMapBackgroundPixels);
	sf::Texture BGMapTexture;
	BGMapTexture.loadFromImage(*BGMapBackground);
	sf::Sprite BGMapSprite;
	BGMapSprite.setTexture(BGMapTexture, true);
	BGMapSprite.setPosition(0, 0);
	BGMapWindow->draw(BGMapSprite);
	BGMapWindow->display();
}

void Graphics::setupColorPaletteWindow() {

	ColorPaletteWindow = new sf::RenderWindow(sf::VideoMode(4 * DisplayScale * 12, 16 * DisplayScale * 12), "Color Palettes");
	ColorPaletteWindow->setPosition(sf::Vector2i(950, 650));
	ColorPaletteWindow->setVerticalSyncEnabled(false);
	ColorPaletteWindow->setFramerateLimit(60);

	ColorPaletteView = new sf::View(sf::FloatRect(0, 0, 4, 16));
	ColorPaletteWindow->setView(*ColorPaletteView);

	ColorPaletteBackground = new sf::Image();
	ColorPaletteBackground->create(4, 16, sf::Color::Magenta);
}

void Graphics::updateColorPaletteWindow() {

	if (ColorGameBoyMode == false) {
		return;
	}

	ColorPaletteWindow->clear();
	ColorPaletteWindow->setView(*ColorPaletteView);

	for (size_t y = 0; y < 8; y++) {
		for (size_t x = 0; x < 4; x++) {

			int index = (y * 8) + (x * 2);
			byte low = memory->BGColorPalette[index];
			byte high = memory->BGColorPalette[index + 1];

			byte red = low & 0x1F;
			byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
			byte blue = (high & 0x7C) >> 2;;

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			ColorPaletteBackground->setPixel(x, y, sf::Color(red, green, blue));
		}
	}

	for (size_t y = 0; y < 8; y++) {
		for (size_t x = 0; x < 4; x++) {

			int index = (y * 8) + (x * 2);
			byte low = memory->SpriteColorPalette[index];
			byte high = memory->SpriteColorPalette[index + 1];

			byte red = low & 0x1F;
			byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
			byte blue = high & 0x7C;

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			ColorPaletteBackground->setPixel(x, y + 8, sf::Color(red, green, blue));
		}
	}


	sf::Texture ColorPaletteTexture;
	ColorPaletteTexture.loadFromImage(*ColorPaletteBackground);
	sf::Sprite ColorPaletteSprite;
	ColorPaletteSprite.setTexture(ColorPaletteTexture, true);
	ColorPaletteSprite.setPosition(0, 0);
	ColorPaletteWindow->draw(ColorPaletteSprite);
	ColorPaletteWindow->display();
}

void Graphics::updateWindow() {

	window->clear();
	window->setView(*view);
	background->create(ScreenWidth, ScreenHeight, backgroundPixels);
	sf::Texture backgroundTexture;
	backgroundTexture.loadFromImage(*background);
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(backgroundTexture, true);
	backgroundSprite.setPosition(0, 0);
	window->draw(backgroundSprite);
	window->display();
}

void Graphics::update(word cyclesThisUpdate, int speedMode) {

	cyclesThisLine += cyclesThisUpdate;

	if (lastLY == (ScreenHeight - 1) && memory->Read(Address::LY) == ScreenHeight) {
		byte interuptFlags = memory->Read(Address::InteruptFlag);
		interuptFlags |= Bits::b0;
		memory->Write(Address::InteruptFlag, interuptFlags);
	}
	lastLY = memory->Read(Address::LY);

	if (memory->Read(Address::LY) > 153) {
		memory->Write(Address::LY, 0);
	}

	if (cyclesThisLine >= (cyclesPerLine * speedMode)) {
		drawScanLine();
		cyclesThisLine -= (cyclesPerLine * speedMode);
	}

}

void Graphics::drawScanLine() {

	if (memory->Read(Address::LY) < ScreenHeight) {

		byte lcd = memory->Read(Address::LCDC);
		if (BitTest(lcd, 7) == false){
			byte LY = memory->Read(Address::LY);
			for (size_t i = 0; i < ScreenWidth; i++) {
				SetPixel(backgroundPixels, i, LY, ScreenWidth, { 0xFF, 0xFF, 0xFF });
			}
		}
		else {
			drawBackground();
			DrawWindowLine();
			drawSprites();
		}
		memory->HBlankDMA();

	}

	memory->memorySpace[Address::LY]++;
}

void Graphics::drawBackground() {

	byte scrollY = memory->Read(Address::ScrollY);
	byte scrollX = memory->Read(Address::ScrollX);
	byte LY = memory->Read(Address::LY); 

	DrawBackgroundLine(scrollX, (scrollY + LY) % 256, LY, ScreenWidth, backgroundPixels, true);
}

byte Graphics::bitData(byte val, byte bit) {
	
	byte testBit = Bits::b7 >> bit;
	
	if ((val & testBit) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void Graphics::drawSprites() {

	byte LCDC = memory->Read(Address::LCDC);
	if (BitTest(LCDC, 1) == false) {
		return;
	}
	word spriteAttributeTable = Address::SpriteAttributes;

	for (word i = 0; i < 40; i++) {

		int positionY = memory->Read(spriteAttributeTable + (i * 4));
		if (positionY <= 0 || positionY >= 160) { // off-screen
			continue;
		}
		int positionX = memory->Read(spriteAttributeTable + (i * 4) + 1);
		if (positionX <= 0 || positionX >= 168) { // off-screen
			continue;
		}
		positionY -= 16;
		positionX -= 8;
		byte patternNumber = memory->Read(spriteAttributeTable + (i * 4) + 2);
		byte flags = memory->Read(spriteAttributeTable + (i * 4) + 3);

		bool Yflip = BitTest(flags, 6);
		bool Xflip = BitTest(flags, 5);

		byte spriteHeight = 8;
		if (BitTest(LCDC, 2)) {
			spriteHeight = 16;
			patternNumber &= ~Bits::b0;
		}

		byte OGP = memory->Read(Address::OBJPalette0);
		if (BitTest(flags, 4)) {
			OGP = memory->Read(Address::OBJPalette1);
		}

		byte LY = memory->Read(Address::LY);

		if (LY < positionY + spriteHeight && LY >= positionY) {

			int drawLine = LY - positionY;
			if (Yflip == true) {
				drawLine -= (spriteHeight - 1);
				drawLine *= -1;
			}

			word tileLocation = Address::TilePattern0 + (patternNumber * 16);

			byte pixelDataLow = memory->Read(tileLocation + drawLine * 2);
			byte pixelDataHigh = memory->Read(tileLocation + drawLine * 2 + 1);

			for (int j = 0; j < 8; j++) {

				byte pixel = GetPixelIndex(pixelDataLow, pixelDataHigh, j);

				int pixelOffsetX = j;
				if (Xflip == true) {
					pixelOffsetX -= 7;
					pixelOffsetX *= -1;
				}

				int pixelX = positionX + pixelOffsetX;

				if (BitTest(flags, 7) == true && backgroundPixelsColorIndex[(LY * ScreenWidth) + pixelX] > 0) {
					continue;
				}

				if (LY < 144 && pixelX < 160 && pixelX >= 0 && pixel != 0) {

					if (ColorGameBoyMode == true) {

						byte CGBPaletteNumber = flags & 0x7;
						byte vRamBank = flags & Bits::b3;

						sf::Color* palette = GetColorSpritePalette(CGBPaletteNumber);
						SetPixel(backgroundPixels, pixelX, LY, ScreenWidth, palette[pixel]);
					}
					else {

						byte offset = pixel * 2;
						byte color = (OGP & (0x3 << offset)) >> offset;

						SetPixel(backgroundPixels, pixelX, LY, ScreenWidth, BWPalette[color]);
					}
				}
			}
		}
	}

}

void Graphics::DrawBackgroundLine(int startX, int row, int screenY, int screenWidth, sf::Uint8* screen, bool mainScreen) {

	byte LCDC = memory->Read(Address::LCDC);

	word backgroundMapAddressRowStart = BGWTileInfo0;
	if (BitTest(LCDC, 3) == true) {
		backgroundMapAddressRowStart = BGWTileInfo1;
	}
	backgroundMapAddressRowStart += (row / 8) * 32;

	byte BGP = memory->Read(Address::BGWPalette);

	int screenX = 0;
	while (screenX < screenWidth) {

		int backgroundX = (screenX + startX) % 256;
		word backgroundMapAddress = backgroundMapAddressRowStart + (backgroundX / 8);
		word memoryTileAddress = GetTileDataAddress(LCDC, backgroundMapAddress);

		bool yFlip = false;
		bool xFlip = false;
		byte CGBAttributes = 0;
		byte VramBank = 0;

		if (ColorGameBoyMode == true) {
			
			CGBAttributes = memory->Read(backgroundMapAddress, 1);

			if (BitTest(CGBAttributes, 6) == true) {
				yFlip = true;
			}
			if (BitTest(CGBAttributes, 5) == true) {
				xFlip = true;
			}
			if (BitTest(CGBAttributes, 3) == true) {
				VramBank = 1;
			}
		}


		int TilePixelY = row % 8;
		if (yFlip == true) {
			TilePixelY -= 7;
			TilePixelY *= -1;
		}

		word address = memoryTileAddress + TilePixelY * 2;

		byte pixelDataLow = memory->vramBank[VramBank][address - 0x8000];
		byte pixelDataHigh = memory->vramBank[VramBank][(address + 1) - 0x8000];

		sf::Color* palette = nullptr;

		if (ColorGameBoyMode) {
			palette = GetBGPalette(CGBAttributes);
		}

		do {

			int pixelX = (startX + screenX) % 8;
			if (xFlip == true) { // x flip
				pixelX -= 7;
				pixelX *= -1;
			}

			byte pixel = GetPixelIndex(pixelDataLow, pixelDataHigh, pixelX);

			int screenIndex = ((screenY * screenWidth) + screenX) * 4;
			if (ColorGameBoyMode) {

				SetPixel(screen, screenX, screenY, screenWidth, palette[pixel]);
				if (mainScreen == true) {
					backgroundPixelsColorIndex[screenIndex / 4] = pixel;
				}
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				SetPixel(screen, screenX, screenY, screenWidth, BWPalette[color]);
				if (mainScreen == true) {
					backgroundPixelsColorIndex[screenIndex / 4] = color;
				}
			}

			screenX++;
		} while (((startX + screenX) % 8) != 0);
	}
}

void Graphics::DrawWindowLine() {

	byte LCDC = memory->Read(Address::LCDC);

	if (BitTest(LCDC, 5) == false) {
		return;
	}

	byte windowY = memory->Read(Address::WindowY);
	byte LY = memory->Read(Address::LY);

	if (windowY > LY) {
		return;
	}

	word windowTileMap = Address::BGWTileInfo0;
	if (BitTest(LCDC, 6) == true) {
		windowTileMap = Address::BGWTileInfo1;
	}

	byte BGP = memory->Read(Address::BGWPalette);

	byte windowX = memory->Read(Address::WindowX);

	int screenX = windowX - 7;
	byte xPosInWindow = 0;

	while (screenX < ScreenWidth) {

		if (screenX < 0) {
			screenX++;
			xPosInWindow++;
			continue;
		}
		byte yPosInWindow = LY - windowY;

		word windowTileMapAddress = windowTileMap + ((yPosInWindow / 8) * 32) + ((xPosInWindow) / 8);
		word memoryTileAddress = GetTileDataAddress(LCDC, windowTileMapAddress);

		bool yFlip = false;
		bool xFlip = false;
		byte CGBAttributes = 0;
		byte VramBank = 0;

		if (ColorGameBoyMode == true) {

			CGBAttributes = memory->Read(windowTileMapAddress, 1);

			if (BitTest(CGBAttributes, 6) == true) {
				yFlip = true;
			}
			if (BitTest(CGBAttributes, 5) == true) {
				xFlip = true;
			}
			if (BitTest(CGBAttributes, 3) == true) {
				VramBank = 1;
			}
		}

		int TilePixelY = yPosInWindow % 8;
		if (yFlip == true) {
			TilePixelY -= 7;
			TilePixelY *= -1;
		}
		word address = memoryTileAddress + TilePixelY * 2;

		byte pixelDataLow = memory->vramBank[VramBank][address - 0x8000];
		byte pixelDataHigh = memory->vramBank[VramBank][(address + 1) - 0x8000];

		do {

			int pixelX = (xPosInWindow) % 8;
			if (xFlip == true) { // x flip
				pixelX -= 7;
				pixelX *= -1;
			}

			byte pixel = GetPixelIndex(pixelDataLow, pixelDataHigh, pixelX);

			if (ColorGameBoyMode) {

				sf::Color* palette = GetBGPalette(CGBAttributes);
				SetPixel(backgroundPixels, screenX, LY, ScreenWidth, palette[pixel]);
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				SetPixel(backgroundPixels, screenX, LY, ScreenWidth, BWPalette[pixel]);
			}

			screenX++;
			xPosInWindow++;
		} while ((xPosInWindow % 8) != 0);
	}
}

void Graphics::SetPixel(sf::Uint8 * screen, int x, int y, int screenWidth, sf::Color color) {

	int screenIndex = ((y * screenWidth) + x) * 4;
	screen[screenIndex + 0] = color.r;
	screen[screenIndex + 1] = color.g;
	screen[screenIndex + 2] = color.b;
	screen[screenIndex + 3] = 0xFF;
}

byte Graphics::GetPixelIndex(byte dataLow, byte dataHigh, byte pixelX) {

	byte pixelIndex = 0;

	if (BitTestReverse(dataLow, pixelX) == true) {
		pixelIndex |= Bits::b0;
	}
	if (BitTestReverse(dataHigh, pixelX) == true) {
		pixelIndex |= Bits::b1;
	}

	return pixelIndex;
}

word Graphics::GetTileDataAddress(byte LCDC, word address) {

	word tileDataAddress = 0;

	if (BitTest(LCDC, 4) == true) {

		byte tileNumber = memory->Read(address, 0);
		tileDataAddress = Address::TilePattern0 + tileNumber * 16;
	}
	else {

		signed char tileNumber = memory->Read(address, 0);
		tileDataAddress = Address::TilePattern1 + (tileNumber + 0x80) * 16;
	}

	return tileDataAddress;
}

sf::Color* Graphics::GetBGPalette(byte CGBMapAttributes) {

	if (ColorGameBoyMode == false) {
		CGBPalette[0] = sf::Color(0xFF, 0xFF, 0xFF);
		CGBPalette[1] = sf::Color(0xAA, 0xAA, 0xAA);
		CGBPalette[2] = sf::Color(0x55, 0x55, 0x55);
		CGBPalette[3] = sf::Color(0x00, 0x00, 0x00);
	}
	else {
		byte paletteNumber = CGBMapAttributes & 0x7;

		for (size_t i = 0; i < 8; i+= 2) {

			byte low = memory->BGColorPalette[(paletteNumber * 8) + i];
			byte high = memory->BGColorPalette[(paletteNumber * 8) + i + 1];

			byte red = low & 0x1F;
			byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
			byte blue = (high & 0x7C) >> 2;

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			CGBPalette[i / 2] = sf::Color(red, green, blue);
		}

	}

	return CGBPalette;
}

sf::Color * Graphics::GetColorSpritePalette(int paletteNumber) {

	for (size_t i = 0; i < 8; i += 2) {

		byte low = memory->SpriteColorPalette[(paletteNumber * 8) + i];
		byte high = memory->SpriteColorPalette[(paletteNumber * 8) + i + 1];

		byte red = low & 0x1F;
		byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
		byte blue = (high & 0x7C) >> 2;

		red <<= 3;
		green <<= 3;
		blue <<= 3;

		CGBSpritePalette[i / 2] = sf::Color(red, green, blue);
	}

	return CGBSpritePalette;
}
