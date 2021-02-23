#include "ppu.h"

PPU::PPU() {

	backgroundPixels = new byte[ScreenWidth * ScreenHeight * 4];
	tileMemoryPixels = new byte[256 * 192 * 4];
	memset(tileMemoryPixels, 0x88, 256 * 192 * 4);
	BGMapBackgroundPixels = new byte[256 * 256 * 4];
	memset(BGMapBackgroundPixels, 0x88, 256 * 256 * 4);
	ColorPalettePixels = new byte[4 * 16 * 4];
	memset(ColorPalettePixels, 0x88, 4 * 16 * 4);

	backgroundPixelsColorIndex = new byte[ScreenWidth * ScreenHeight];
}

PPU::~PPU() {
	delete[] backgroundPixels;
	delete[] tileMemoryPixels;
	delete[] BGMapBackgroundPixels;
	delete[] ColorPalettePixels;
	delete[] backgroundPixelsColorIndex;
}

void PPU::updateTileImage() {

	byte BGP = memory->Read(Address::BGWPalette);

	byte vramBankCount = 1;
	if (ColorGameBoyMode == true) {
		vramBankCount = 2;
	}
	byte preserveBankNumber = memory->currentVramBank;
	for (byte bank = 0; bank < vramBankCount; bank++) {

		memory->currentVramBank = bank;
		for (word y = 0; y < 24; y++) {

			for (word x = 0; x < 16; x++) {

				short memoryTile = Address::TilePattern0 + (y * 0x100) + (x * 0x10);

				word tileX = x * 8;
				word tileY = y * 8;
				Color* palette = GetBGPalette(memory->vramBank[1][(0x9800 + y * 16 + x) - 0x8000]);

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

							//tileMemoryBackground->setPixel((bank * 128) + tileX + j, tileY + i, palette[pixel]);
							SetPixel(tileMemoryPixels, 256 * 192 * 4, (bank * 128) + tileX + j, tileY + i, 256, palette[pixel]);
						}
						else {

							byte offset = pixel * 2;
							byte color = (BGP & (0x3 << offset)) >> offset;

							//tileMemoryBackground->setPixel((bank * 128) + tileX + j, tileY + i, BWPalette[color]);
							SetPixel(tileMemoryPixels, 256 * 192 * 4, (bank * 128) + tileX + j, tileY + i, 256, palette[pixel]);
						}
					}

				}
			}
		}
	}
	memory->currentVramBank = preserveBankNumber;
}

void PPU::updateBGMapImage() {

	for (int i = 0; i < 256; i++) {
		DrawBackgroundLine(0, i, i, 256, BGMapBackgroundPixels, 256 * 256 * 4);
	}
	
	//draw gameboy screen outline
	byte scrollX = memory->Read(Address::ScrollX);
	byte scrollY = memory->Read(Address::ScrollY);

	// horizontal lines
	for (size_t i = 0; i < 2; i++) {

		int y = (scrollY + (i * ScreenHeight)) % 256;

		for (size_t xPos = 0; xPos < ScreenWidth; xPos++) {

			int x = (scrollX + xPos) % 256;

			SetPixel(BGMapBackgroundPixels, 256 * 256 * 4, x, y, 256, { 0xFF, 0x00, 0x00 });
		}
	}
	// vertical lines
	for (size_t i = 0; i < 2; i++) {

		int x = (scrollX + (ScreenWidth * i)) % 256;

		for (size_t yPos = 0; yPos < ScreenHeight; yPos++) {

			int y = (scrollY + yPos) % 256;

			SetPixel(BGMapBackgroundPixels, 256 * 256 * 4, x, y, 256, { 0xFF, 0x00, 0x00 });
		}
	}
}

void PPU::updateColorPaletteImage() {

	if (ColorGameBoyMode == false) {
		return;
	}

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 4; x++) {

			int index = (y * 8) + (x * 2);
			byte low = memory->BGColorPalette[index];
			byte high = memory->BGColorPalette[index + 1];

			byte red = low & 0x1F;
			byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
			byte blue = (high & 0x7C) >> 2;;

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			//ColorPaletteBackground->setPixel(x, y, sf::Color(red, green, blue));
			SetPixel(ColorPalettePixels, 4 * 16 * 4, x, y, 4, { red, green, blue });
		}
	}

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 4; x++) {

			int index = (y * 8) + (x * 2);
			byte low = memory->SpriteColorPalette[index];
			byte high = memory->SpriteColorPalette[index + 1];

			byte red = low & 0x1F;
			byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
			byte blue = high & 0x7C;

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			SetPixel(ColorPalettePixels, 4 * 16 * 4, x, y + 8, 4, { red, green, blue });
		}
	}
}

void PPU::update(word cyclesThisUpdate, int speedMode) {

	cyclesThisLine += cyclesThisUpdate;

	byte LY = memory->Read(Address::LY);
	if (lastLY == (ScreenHeight - 1) && LY == ScreenHeight) {
		byte interuptFlags = memory->Read(Address::InteruptFlag);
		interuptFlags |= Bits::b0;
		memory->Write(Address::InteruptFlag, interuptFlags);
	}

	lastLY = memory->Read(Address::LY);

	if (cyclesThisLine >= (cyclesPerLine * speedMode)) {

		memory->memorySpace[Address::LY]++;
		if (memory->Read(Address::LY) > 153) {
			memory->Write(Address::LY, 0);
		}

		lineDrawn = false;
		cyclesThisLine -= (cyclesPerLine * speedMode);
	}
	// the screen is drawn 150-225? clocks into a given LY, 
	// scrollX may change immediatly after LY increment from a LCDStat interrupt
	if (cyclesThisLine > 200 && lineDrawn == false) {
		drawScanLine();
		lineDrawn = true;
	}
}

void PPU::drawScanLine() {

	if (memory->Read(Address::LY) < ScreenHeight) {

		byte lcd = memory->Read(Address::LCDC);
		if (BitTest(lcd, 7) == false){
			byte LY = memory->Read(Address::LY);
			for (int i = 0; i < ScreenWidth; i++) {
				SetPixel(backgroundPixels, ScreenWidth * ScreenHeight * 4, i, LY, ScreenWidth, { 0xFF, 0xFF, 0xFF });
			}
		}
		else {
			drawBackground();
			DrawWindowLine();
			drawSprites();
		}
		memory->HBlankDMA();

	}
}

void PPU::drawBackground() {

	byte scrollY = memory->Read(Address::ScrollY);
	byte scrollX = memory->Read(Address::ScrollX);
	byte LY = memory->Read(Address::LY); 

	DrawBackgroundLine(scrollX, (scrollY + LY) % 256, LY, ScreenWidth, backgroundPixels, ScreenWidth * ScreenHeight * 4, true);
}

byte PPU::bitData(byte val, byte bit) {
	
	byte testBit = Bits::b7 >> bit;
	
	if ((val & testBit) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void PPU::drawSprites() {

	byte LCDC = memory->Read(Address::LCDC);
	if (BitTest(LCDC, 1) == false) {
		return;
	}
	word spriteAttributeTable = Address::SpriteAttributes;

	int spritesThisLine = 0;
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
		if (BitTest(LCDC, 2) == true) {
			spriteHeight = 16;
			patternNumber &= ~Bits::b0;
		}

		byte OGP = memory->Read(Address::OBJPalette0);
		if (BitTest(flags, 4) == true) {
			OGP = memory->Read(Address::OBJPalette1);
		}

		byte LY = memory->Read(Address::LY);

		if (LY < positionY + spriteHeight && LY >= positionY && spritesThisLine < 10) {

			spritesThisLine++;
			int drawLine = LY - positionY;
			if (Yflip == true) {
				drawLine -= (spriteHeight - 1);
				drawLine *= -1;
			}

			int vramBank = 0;
			if (ColorGameBoyMode == true) {
				if (BitTest(flags, 3) == true) {
					vramBank = 1;
				}
			}

			word tileLocation = Address::TilePattern0 + (patternNumber * 16);

			byte pixelDataLow = memory->Read(tileLocation + drawLine * 2, vramBank);
			byte pixelDataHigh = memory->Read(tileLocation + drawLine * 2 + 1, vramBank);

			for (int j = 0; j < 8; j++) {

				byte pixel = GetPixelIndex(pixelDataLow, pixelDataHigh, j);

				int pixelOffsetX = j;
				if (Xflip == true) {
					pixelOffsetX -= 7;
					pixelOffsetX *= -1;
				}

				int pixelX = positionX + pixelOffsetX;

				if (BitTest(flags, 7) == true && backgroundPixelsColorIndex[(LY * ScreenWidth) + pixelX] > 0 ||
					backgroundPixelsColorIndex[(LY * ScreenWidth) + pixelX] == 0x10 && BitTest(LCDC, 0) == true) {
					continue;
				}

				if (LY < 144 && pixelX < 160 && pixelX >= 0 && pixel != 0) {

					if (ColorGameBoyMode == true) {

						byte CGBPaletteNumber = flags & 0x7;
						byte vRamBank = flags & Bits::b3;

						Color* palette = GetColorSpritePalette(CGBPaletteNumber);
						SetPixel(backgroundPixels, ScreenWidth * ScreenHeight * 4, pixelX, LY, ScreenWidth, palette[pixel]);
					}
					else {

						byte offset = pixel * 2;
						byte color = (OGP & (0x3 << offset)) >> offset;

						SetPixel(backgroundPixels, ScreenWidth * ScreenHeight * 4, pixelX, LY, ScreenWidth, BWPalette[color]);
					}
				}
			}
		}
	}

}

void PPU::DrawBackgroundLine(int startX, int row, int screenY, int screenWidth, byte* screen, int screenSize, bool mainScreen) {

	byte LCDC = memory->Read(Address::LCDC);

	word backgroundMapAddressRowStart = Address::BGWTileInfo0;
	if (BitTest(LCDC, 3) == true) {
		backgroundMapAddressRowStart = Address::BGWTileInfo1;
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
		bool BGtoOAMPriority = false;
		byte CGBAttributes = 0;
		byte VramBank = 0;

		Color* palette = nullptr;
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

			if (BitTest(CGBAttributes, 7) == true) {
				BGtoOAMPriority = true;
			}

			palette = GetBGPalette(CGBAttributes);
		}


		int TilePixelY = row % 8;
		if (yFlip == true) {
			TilePixelY -= 7;
			TilePixelY *= -1;
		}

		word address = memoryTileAddress + TilePixelY * 2;

		byte pixelDataLow = memory->vramBank[VramBank][address - 0x8000];
		byte pixelDataHigh = memory->vramBank[VramBank][(address + 1) - 0x8000];

		do {

			int pixelX = (startX + screenX) % 8;
			if (xFlip == true) { // x flip
				pixelX -= 7;
				pixelX *= -1;
			}

			byte pixel = GetPixelIndex(pixelDataLow, pixelDataHigh, pixelX);

			int screenIndex = ((screenY * screenWidth) + screenX) * 4;
			if (ColorGameBoyMode) {

				SetPixel(screen, screenSize, screenX, screenY, screenWidth, palette[pixel]);
				if (mainScreen == true && screenIndex / 4 < ScreenWidth * ScreenHeight) {
					if (BGtoOAMPriority == true) {
						backgroundPixelsColorIndex[screenIndex / 4] = 0x10; // special BG has priority value
					}
					else {
						backgroundPixelsColorIndex[screenIndex / 4] = pixel;
					}
				}
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				SetPixel(screen, screenSize, screenX, screenY, screenWidth, BWPalette[color]);
				if (mainScreen == true && screenIndex / 4 < ScreenWidth * ScreenHeight) {
					backgroundPixelsColorIndex[screenIndex / 4] = color;
				}
			}

			screenX++;
		} while (((startX + screenX) % 8) != 0 && ((screenY * screenWidth) + screenX) * 4 < screenSize);
	}
}

void PPU::DrawWindowLine() {

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
		bool BGtoOAMPriority = false;
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

			if (BitTest(CGBAttributes, 7) == true) {
				BGtoOAMPriority = true;
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

			int screenIndex = ((LY * ScreenWidth) + screenX);
			if (ColorGameBoyMode) {

				Color* palette = GetBGPalette(CGBAttributes);
				SetPixel(backgroundPixels, ScreenWidth * ScreenHeight * 4, screenX, LY, ScreenWidth, palette[pixel]);
				if (BGtoOAMPriority == true) {
					backgroundPixelsColorIndex[screenIndex] = 0x10; // special BG has priority value
				}
				else {
					backgroundPixelsColorIndex[screenIndex] = pixel;
				}
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				SetPixel(backgroundPixels, ScreenWidth * ScreenHeight * 4, screenX, LY, ScreenWidth, BWPalette[pixel]);
				backgroundPixelsColorIndex[screenIndex] = color;
			}

			screenX++;
			xPosInWindow++;
		} while ((xPosInWindow % 8) != 0 && screenX < ScreenWidth);
	}
}

void PPU::SetPixel(byte* screen, int screenSize, int x, int y, int screenWidth, Color color) {

	int screenIndex = ((y * screenWidth) + x) * 4;

	if (screenIndex + 3 < screenSize) {

		screen[screenIndex + 0] = color.red;
		screen[screenIndex + 1] = color.green;
		screen[screenIndex + 2] = color.blue;
		screen[screenIndex + 3] = 0xFF;
	}
}

byte PPU::GetPixelIndex(byte dataLow, byte dataHigh, byte pixelX) {

	byte pixelIndex = 0;

	if (BitTestReverse(dataLow, pixelX) == true) {
		pixelIndex |= Bits::b0;
	}
	if (BitTestReverse(dataHigh, pixelX) == true) {
		pixelIndex |= Bits::b1;
	}

	return pixelIndex;
}

word PPU::GetTileDataAddress(byte LCDC, word address) {

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

Color* PPU::GetBGPalette(byte CGBMapAttributes) {

	if (ColorGameBoyMode == false) {

		CGBPalette[0] = { 0xFF, 0xFF, 0xFF };
		CGBPalette[1] = { 0xAA, 0xAA, 0xAA };
		CGBPalette[2] = { 0x55, 0x55, 0x55 };
		CGBPalette[3] = { 0x00, 0x00, 0x00 };
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

			CGBPalette[i / 2] = { red, green, blue };
		}
	}

	return CGBPalette;
}

Color* PPU::GetColorSpritePalette(int paletteNumber) {

	for (size_t i = 0; i < 8; i += 2) {

		byte low = memory->SpriteColorPalette[(paletteNumber * 8) + i];
		byte high = memory->SpriteColorPalette[(paletteNumber * 8) + i + 1];

		byte red = low & 0x1F;
		byte green = ((low & 0xE0) >> 5) | ((high & 0x3) << 3);
		byte blue = (high & 0x7C) >> 2;

		red <<= 3;
		green <<= 3;
		blue <<= 3;

		CGBSpritePalette[i / 2] = { red, green, blue };
	}

	return CGBSpritePalette;
}
