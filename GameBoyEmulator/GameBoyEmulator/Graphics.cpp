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
	byte BGP = memory->Read(Address::BGWPalette);
	byte LCDC = memory->Read(Address::LCDC);

	if (/*memory->TileChanged == */true) {

		for (word y = 0; y < 32; y++) {

			for (word x = 0; x < 32; x++) {

				word address = 0x9800 + y * 32 + x;
				
				if ((LCDC & Bits::b3) != 0) {
					address += 0x0400;
				}

				byte CGBAttributes = memory->vramBank[1][address - 0x8000];
				sf::Color* palette = GetBGPalette(CGBAttributes);

				short memoryTile = 0;
				if ((LCDC & Bits::b4) == Bits::b4) {

					byte tileNumber = memory->Read(address);
					memoryTile = Address::TilePattern0 + tileNumber * 16;
				}
				else {

					signed char tileNumber = memory->Read(address);
					memoryTile = Address::TilePattern1 + (tileNumber + 0x80) * 16;
				}

				word tileX = x * 8;
				word tileY = y * 8;

				for (word i = 0; i < 8; i++) {

					int pixelY = i;
					if (BitTest(CGBAttributes, 6) == true) { // y flip
						pixelY -= 7;
						pixelY *= -1;
					}

					word address = memoryTile + pixelY * 2;

					byte pixelDataLow = memory->Read(address);
					byte pixelDataHigh = memory->Read(address + 1);

					for (word j = 0; j < 8; j++) {

						int pixelX = j;
						if (BitTest(CGBAttributes, 5) == true) { // x flip
							pixelX -= 7;
							pixelX *= -1;
						}

						byte pixel = 0;
						if ((pixelDataLow & (Bits::b7 >> pixelX)) != 0) {
							pixel |= Bits::b0;
						}
						if ((pixelDataHigh & (Bits::b7 >> pixelX)) != 0) {
							pixel |= Bits::b1;
						}
						if (ColorGameBoyMode) {

							BGMapBackground->setPixel(tileX + j, tileY + i, palette[pixel]);
						}
						else {

							byte offset = pixel * 2;
							byte color = (BGP & (0x3 << offset)) >> offset;

							BGMapBackground->setPixel(tileX + j, tileY + i, BWPalette[color]);
						}
					}

				}
			}
		}
		memory->TileChanged = false;
	}


	// draw gameboy screen outline
	//byte scrollX = memory->Read(Address::ScrollX);
	//byte scrollY = memory->Read(Address::ScrollY);

	//for (size_t i = 0; i < 2; i++) {

	//	for (size_t xPos = 0; xPos < ScreenWidth; xPos++) {

	//		int x = (scrollX + xPos) % 256;
	//		int y = (scrollY + (i * ScreenHeight)) % 256;

	//		BGMapBackground->setPixel(x, y, sf::Color::Red);
	//	}
	//}
	//for (size_t i = 0; i < 2; i++) {

	//	for (size_t yPos = 0; yPos < ScreenHeight; yPos++) {

	//		int x = (scrollX + (ScreenWidth * i)) % 256;
	//		int y = (scrollY + yPos) % 256;

	//		BGMapBackground->setPixel(x, y, sf::Color::Red);
	//	}
	//}


	sf::Texture BGMapTexture;
	BGMapTexture.loadFromImage(*BGMapBackground);
	sf::Sprite BGMapSprite;
	BGMapSprite.setTexture(BGMapTexture, true);
	BGMapSprite.setPosition(0, 0);
	BGMapWindow->draw(BGMapSprite);
	BGMapWindow->display();
}

void Graphics::updateBGMapWindow2() {

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

			int index = (y * 256 + x) * 4;

			BGMapBackgroundPixels[index + 0] = 0xFF;
			BGMapBackgroundPixels[index + 1] = 0x00;
			BGMapBackgroundPixels[index + 2] = 0x00;
			BGMapBackgroundPixels[index + 3] = 0xFF;
		}
	}
	for (size_t i = 0; i < 2; i++) {

		int x = (scrollX + (ScreenWidth * i)) % 256;

		for (size_t yPos = 0; yPos < ScreenHeight; yPos++) {

			int y = (scrollY + yPos) % 256;

			int index = (y * 256 + x) * 4;

			BGMapBackgroundPixels[index + 0] = 0xFF;
			BGMapBackgroundPixels[index + 1] = 0x00;
			BGMapBackgroundPixels[index + 2] = 0x00;
			BGMapBackgroundPixels[index + 3] = 0xFF;
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
		cyclesThisLine -= cyclesPerLine;
	}
}

void Graphics::drawScanLine() {

	if (memory->Read(Address::LY) < ScreenHeight) {
		drawBackground2();
		DrawWindowLine();
		drawSprites();
		memory->HBlankDMA();
	}

	memory->memorySpace[Address::LY]++;
}

void Graphics::drawBackground() {

	byte LY = memory->Read(Address::LY);
	byte scrollY = memory->Read(Address::ScrollY);
	byte scrollX = memory->Read(Address::ScrollX);
	byte windowY = memory->Read(Address::WindowY);
	byte windowX = memory->Read(Address::WindowX);
	byte LCDC = memory->Read(Address::LCDC);


	byte YLine = 0;
	word backgroundMemory = 0;
	bool window;
	if ((LCDC & Bits::b5) == Bits::b5 && windowY <= LY) { // window enabled
		YLine = LY - windowY;
		window = true;
		if ((LCDC & Bits::b6) != 0) {
			backgroundMemory = Address::BGWTileInfo1;
		}
		else {
			backgroundMemory = Address::BGWTileInfo0;
		}
	}
	else {
		YLine = LY + scrollY;
		window = false;
		if ((LCDC & Bits::b3) != 0) {
			backgroundMemory = Address::BGWTileInfo1;
		}
		else {
			backgroundMemory = Address::BGWTileInfo0;
		}
	}



	for (byte i = 0; i < ScreenWidth; i++) {

		if (window == true) {

			byte TileY = (LY - windowY) / 8;
			byte TileX = (i) / 8;

			word tileLocation = 0;
			if ((LCDC & Bits::b4) == Bits::b4) {

				byte tileIndex = memory->Read(backgroundMemory + (TileY * 0x20) + (TileX));
				tileLocation = Address::TilePattern0 + tileIndex * 16;
			}
			else {

				signed char tileIndex = memory->Read(backgroundMemory + (TileY * 0x20) + (TileX));
				tileLocation = Address::TilePattern1 + (tileIndex + 0x80) * 16;
			}

			byte tileLine = LY % 8;
			tileLine *= 2;

			byte pixelDataLow = memory->Read(tileLocation + tileLine);
			byte pixelDataHigh = memory->Read(tileLocation + tileLine + 1);

			byte pixel = 0;
			byte bitMask = Bits::b7 >> (i % 8);
			if ((pixelDataLow & bitMask) != 0) {
				pixel |= Bits::b0;
			}
			if ((pixelDataHigh & bitMask) != 0) {
				pixel |= Bits::b1;
			}

			byte offset = pixel * 2;
			byte BGP = memory->Read(Address::BGWPalette);
			byte color = (BGP & (0x3 << offset)) >> offset;

			background->setPixel(i, LY, BWPalette[color]);
		}
		else {

			byte backgroundY = (scrollY + LY) % 256;
			byte backgroundX = (scrollX + i) % 256;

			byte TileY = backgroundY / 8;
			byte TileX = backgroundX / 8;


			word tileLocation = 0;
			if ((LCDC & Bits::b4) == Bits::b4) {

				byte tileIndex = memory->Read(backgroundMemory + (TileY * 0x20) + (TileX));
				tileLocation = Address::TilePattern0 + tileIndex * 16;
			}
			else {

				signed char tileIndex = memory->Read(backgroundMemory + (TileY * 0x20) + (TileX));
				tileLocation = Address::TilePattern1 + (tileIndex + 0x80) * 16;
			}

			byte tileLine = backgroundY % 8;
			tileLine *= 2;

			byte pixelDataLow = memory->Read(tileLocation + tileLine);
			byte pixelDataHigh = memory->Read(tileLocation + tileLine + 1);

			byte pixel = 0;
			byte bitMask = Bits::b7 >> (backgroundX % 8);
			if ((pixelDataLow & bitMask) != 0) {
				pixel |= Bits::b0;
			}
			if ((pixelDataHigh & bitMask) != 0) {
				pixel |= Bits::b1;
			}

			if (ColorGameBoyMode) {

				sf::Color* palette = GetBGPalette(memory->vramBank[1][(backgroundMemory + (TileY * 0x20) + (TileX)) - 0x8000]);
				background->setPixel(i, LY, palette[pixel]);
			}
			else {

				byte offset = pixel * 2;
				byte BGP = memory->Read(Address::BGWPalette);
				byte color = (BGP & (0x3 << offset)) >> offset;

				background->setPixel(i, LY, BWPalette[color]);
			}
		}
	}
}

void Graphics::drawBackground2() {

	byte scrollY = memory->Read(Address::ScrollY);
	byte scrollX = memory->Read(Address::ScrollX);
	byte LY = memory->Read(Address::LY);

	DrawBackgroundLine(scrollX, (scrollY + LY) % 256, LY, ScreenWidth, backgroundPixels);
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

				byte pixel = 0;
				if ((pixelDataLow & (Bits::b7 >> j)) != 0) {
					pixel |= Bits::b0;
				}
				if ((pixelDataHigh & (Bits::b7 >> j)) != 0) {
					pixel |= Bits::b1;
				}
				int pixelOffsetX = j;
				if (Xflip == true) {
					pixelOffsetX -= 7;
					pixelOffsetX *= -1;
				}

				int pixelX = positionX + pixelOffsetX;
				int screenIndex = ((LY * ScreenWidth) + pixelX) * 4;

				bool BGPriority = BitTest(flags, 7);
				if (BGPriority == false && backgroundPixels[screenIndex/4] == 1) {
					continue;
				}

				if (LY < 144 && pixelX < 160 && pixel != 0) {

					if (ColorGameBoyMode == true) {

						byte CGBPaletteNumber = flags & 0x7;
						byte vRamBank = flags & Bits::b3;

						sf::Color* palette = GetColorSpritePalette(CGBPaletteNumber);
						backgroundPixels[screenIndex + 0] = palette[pixel].r;
						backgroundPixels[screenIndex + 1] = palette[pixel].g;
						backgroundPixels[screenIndex + 2] = palette[pixel].b;
						backgroundPixels[screenIndex + 3] = 0xFF;
					}
					else {

						byte offset = pixel * 2;
						byte color = (OGP & (0x3 << offset)) >> offset;

						backgroundPixels[screenIndex + 0] = BWPalette[color].r;
						backgroundPixels[screenIndex + 1] = BWPalette[color].g;
						backgroundPixels[screenIndex + 2] = BWPalette[color].b;
						backgroundPixels[screenIndex + 3] = 0xFF;
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

	bool signedAddressingMode = false;
	if (BitTest(LCDC, 4) == false) {
		signedAddressingMode = true;
	}

	byte BGP = memory->Read(Address::BGWPalette);

	int screenX = 0;
	while (screenX < screenWidth) {

		word backgroundMapAddress = backgroundMapAddressRowStart + (((screenX + startX) % 256) / 8);
		word memoryTileAddress = 0;
		if (signedAddressingMode == false) {

			byte tileNumber = memory->Read(backgroundMapAddress, 0);
			memoryTileAddress = Address::TilePattern0 + tileNumber * 16;
		}
		else {

			signed char tileNumber = memory->Read(backgroundMapAddress, 0);
			memoryTileAddress = Address::TilePattern1 + (tileNumber + 0x80) * 16;
		}

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

			byte pixel = 0;
			if ((pixelDataLow & (Bits::b7 >> pixelX)) != 0) {
				pixel |= Bits::b0;
			}
			if ((pixelDataHigh & (Bits::b7 >> pixelX)) != 0) {
				pixel |= Bits::b1;
			}

			int screenIndex = ((screenY * screenWidth) + screenX) * 4;
			if (ColorGameBoyMode) {

				screen[screenIndex + 0] = palette[pixel].r;
				screen[screenIndex + 1] = palette[pixel].g;
				screen[screenIndex + 2] = palette[pixel].b;
				screen[screenIndex + 3] = 0xFF;
				if (mainScreen == true) {
					backgroundPixelsColorIndex[screenIndex / 4] = 0;
				}
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				screen[screenIndex + 0] = BWPalette[color].r;
				screen[screenIndex + 1] = BWPalette[color].g;
				screen[screenIndex + 2] = BWPalette[color].b;
				screen[screenIndex + 3] = 0xFF;
				if (mainScreen == true) {
					backgroundPixelsColorIndex[screenIndex / 4] = 0;
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

	bool signedAddressingMode = false;
	if (BitTest(LCDC, 4) == false) {
		signedAddressingMode = true;
	}
	byte BGP = memory->Read(Address::BGWPalette);

	byte windowX = memory->Read(Address::WindowX);

	int screenX = windowX;
	while (screenX < ScreenWidth) {

		byte xPosInWindow = screenX - windowX;
		byte yPosInWindow = LY - windowY;

		word windowTileMapAddress = windowTileMap + ((yPosInWindow / 8) * 32) + ((xPosInWindow) / 8);
		word memoryTileAddress = 0;

		if (signedAddressingMode == false) {

			byte tileNumber = memory->Read(windowTileMapAddress, 0);
			memoryTileAddress = Address::TilePattern0 + tileNumber * 16;
		}
		else {

			signed char tileNumber = memory->Read(windowTileMapAddress, 0);
			memoryTileAddress = Address::TilePattern1 + (tileNumber + 0x80) * 16;
		}

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
			xPosInWindow = screenX - windowX;
			int pixelX = (xPosInWindow - 1) % 8;
			if (xFlip == true) { // x flip
				pixelX -= 7;
				pixelX *= -1;
			}

			byte pixel = 0;
			if ((pixelDataLow & (Bits::b7 >> pixelX)) != 0) {
				pixel |= Bits::b0;
			}
			if ((pixelDataHigh & (Bits::b7 >> pixelX)) != 0) {
				pixel |= Bits::b1;
			}

			int screenIndex = ((LY * ScreenWidth) + (screenX - 7)) * 4;
			if (ColorGameBoyMode) {

				sf::Color* palette = GetBGPalette(CGBAttributes);
				backgroundPixels[screenIndex + 0] = palette[pixel].r;
				backgroundPixels[screenIndex + 1] = palette[pixel].g;
				backgroundPixels[screenIndex + 2] = palette[pixel].b;
				backgroundPixels[screenIndex + 3] = 0xFF;
				backgroundPixelsColorIndex[screenIndex / 4] = 1;
				
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				backgroundPixels[screenIndex + 0] = BWPalette[color].r;
				backgroundPixels[screenIndex + 1] = BWPalette[color].g;
				backgroundPixels[screenIndex + 2] = BWPalette[color].b;
				backgroundPixels[screenIndex + 3] = 0xFF;
				backgroundPixelsColorIndex[screenIndex / 4] = 1;
			}

			screenX++;
		} while ((xPosInWindow % 8) != 0);
	}
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
