#include "ppu.h"



PPU::PPU() {

	backgroundPixels = new byte[kScreenWidth * kScreenHeight * kRGBASize];
	tileMemoryPixels = new byte[256 * 192 * kRGBASize];
	memset(tileMemoryPixels, 0x88, 256 * 192 * kRGBASize);
	BGMapBackgroundPixels = new byte[kBackgroundLayerSize * kBackgroundLayerSize * kRGBASize];
	memset(BGMapBackgroundPixels, 0x88, kBackgroundLayerSize * kBackgroundLayerSize * kRGBASize);
	ColorPalettePixels = new byte[kNumberOfColorPalettes * 2 * kColorsPerPalette * kRGBASize];
	memset(ColorPalettePixels, 0x88, kNumberOfColorPalettes * 2 * kColorsPerPalette * kRGBASize);
	backgroundPixelsColorIndex = new byte[kScreenWidth * kScreenHeight];
	memset(backgroundPixelsColorIndex, 0, kScreenWidth * kScreenHeight);

	for (size_t i = 0; i < kNumberOfSprites; i++) {
		byte* pixels = new byte[kSpriteSize * kSpriteSize * 2 * kRGBASize];
		memset(pixels, 0x88, kSpriteSize * kSpriteSize * 2 * kRGBASize);
		spritePixels.push_back(pixels);
	}
}



PPU::~PPU() {
	delete[] backgroundPixels;
	delete[] tileMemoryPixels;
	delete[] BGMapBackgroundPixels;
	delete[] ColorPalettePixels;
	delete[] backgroundPixelsColorIndex;
	for (size_t i = 0; i < spritePixels.size(); i++) {
		delete[] spritePixels[i];
	}
}



// update a pixel array/image that shows all tiles in vram, shows all tiles from 0x8000 to 0x97FF in both banks
// left side is bank 0 and right side is bank 1, right side is blank for original game boy
void PPU::updateTileImage() {

	byte BGP = memory->Read(Address::BGWPalette);

	byte vramBankCount = 1;
	if (ColorGameBoyMode == true) {
		vramBankCount = 2;
	}

	for (byte bank = 0; bank < vramBankCount; bank++) {

		for (word y = 0; y < 24; y++) { // 24 groups of 0x100 bytes between 8000-97FF

			for (word x = 0; x < 16; x++) { // 16 tiles every 0x100 bytes

				short memoryTile = Address::TilePattern0 + (y * 0x100) + (x * 0x10);

				word tileX = x * 8; // top left x position of tile in image
				word tileY = y * 8; // top left y position of tile in image
				Color* palette = GetBGPalette(memory->vramBank[1][(Address::BGWTileInfo0 + y * 16 + x) - 0x8000]);

				for (word i = 0; i < kSpriteSize; i++) { // 8 pixel height per tile

					word address = memoryTile + i * 2;

					byte pixelDataLow = memory->Read(address, bank);
					byte pixelDataHigh = memory->Read(address + 1, bank);

					for (word j = 0; j < 8; j++) {

						byte pixel = 0;
						if (BitTestReverse(pixelDataLow, j) == true) {
							pixel |= Bits::b0;
						}
						if (BitTestReverse(pixelDataHigh, j) == true) {
							pixel |= Bits::b1;
						}
						if (ColorGameBoyMode) {

							SetPixel(tileMemoryPixels, 256 * 192 * kRGBASize, (bank * 128) + tileX + j, tileY + i, 256, palette[pixel]);
						}
						else {

							byte offset = pixel * 2; // offset of color data in color palette
							byte color = (BGP & (0x3 << offset)) >> offset;

							SetPixel(tileMemoryPixels, 256 * 192 * kRGBASize, (bank * 128) + tileX + j, tileY + i, 256, palette[color]);
						}
					}

				}
			}
		}
	}
}



// update a pixel array/image that shows the background map at 0x9800-0x9BFF or 0x9C00-0x9FFF
void PPU::updateBGMapImage() {

	for (int i = 0; i < kBackgroundLayerSize; i++) {
		DrawBackgroundLine(0, i, i, kBackgroundLayerSize, BGMapBackgroundPixels, kBackgroundLayerSize * kBackgroundLayerSize * kRGBASize);
	}
	
	// draw region that is visable on the gameboy screen
	// this may not match the background that is drawn because some games change scrollX line by line
	byte scrollX = memory->Read(Address::ScrollX);
	byte scrollY = memory->Read(Address::ScrollY);

	Color red = { 0xFF, 0x00, 0x00 };

	// horizontal lines
	for (size_t i = 0; i < 2; i++) {

		int y = (scrollY + (i * kScreenHeight)) % kBackgroundLayerSize;

		for (size_t xPos = 0; xPos < kScreenWidth; xPos++) {

			int x = (scrollX + xPos) % kBackgroundLayerSize;

			SetPixel(BGMapBackgroundPixels, kBackgroundLayerSize * kBackgroundLayerSize * kRGBASize, x, y, kBackgroundLayerSize, red);
		}
	}
	// vertical lines
	for (size_t i = 0; i < 2; i++) {

		int x = (scrollX + (kScreenWidth * i)) % kBackgroundLayerSize;

		for (size_t yPos = 0; yPos < kScreenHeight; yPos++) {

			int y = (scrollY + yPos) % kBackgroundLayerSize;

			SetPixel(BGMapBackgroundPixels, kBackgroundLayerSize * kBackgroundLayerSize * kRGBASize, x, y, kBackgroundLayerSize, red);
		}
	}
}



// update a pixel array/image that shows the colors used by each color palette for tiles and sprites
// only works for gameboy color games
void PPU::updateColorPaletteImage() {

	if (ColorGameBoyMode == false) {
		return;
	}

	// tile color palettes
	for (int y = 0; y < kNumberOfColorPalettes; y++) {
		for (int x = 0; x < kColorsPerPalette; x++) { // 4 colors per palette

			int index = (y * 8) + (x * 2);
			word colorData = memory->BGColorPalette[index]; 
			colorData |= memory->BGColorPalette[index + 1] << 8;

			byte red =	  colorData & 0b0000000000011111;		// 0x001F
			byte green = (colorData & 0b0000001111100000) >> 5; // 0x03E0
			byte blue =  (colorData & 0b0111110000000000) >> 10;// 0x7C00

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			SetPixel(ColorPalettePixels, kNumberOfColorPalettes * 2 * kColorsPerPalette * kRGBASize, x, y, kColorsPerPalette * 2, { red, green, blue });
		}
	}

	// sprite color palettes
	for (int y = 0; y < kNumberOfColorPalettes; y++) {
		for (int x = 0; x < kColorsPerPalette; x++) { // color 0 is not visable / is transparent

			int index = (y * 8) + (x * 2);
			word colorData = memory->SpriteColorPalette[index];
			colorData |= memory->SpriteColorPalette[index + 1] << 8;

			byte red =    colorData & 0b0000000000011111;		// 0x001F
			byte green = (colorData & 0b0000001111100000) >> 5; // 0x03E0
			byte blue =  (colorData & 0b0111110000000000) >> 10;// 0x7C00

			red <<= 3;
			green <<= 3;
			blue <<= 3;

			SetPixel(ColorPalettePixels, kNumberOfColorPalettes * 2 * kColorsPerPalette * kRGBASize, x + 4, y, kColorsPerPalette * 2, { red, green, blue });
		}
	}
}


// update an array of pixel arrays/images of sprites
void PPU::updateSpriteImages() {

	word spriteAttributeTable = Address::SpriteAttributes;
	byte LCDC = memory->Read(Address::LCDC);
	for (word i = 0; i < kNumberOfSprites; i++) {

		byte patternNumber = memory->Read(spriteAttributeTable + (i * 4) + 2);
		byte flags = memory->Read(spriteAttributeTable + (i * 4) + 3);

		bool Yflip = BitTest(flags, Bit::Sprite::YFlip);
		bool Xflip = BitTest(flags, Bit::Sprite::XFlip);

		byte spriteHeight = kSpriteSize;
		if (BitTest(LCDC, 2) == true) {
			spriteHeight = kSpriteSize * 2;
			patternNumber &= 0b11111110;
		}
		
		byte OGP = memory->Read(Address::OBJPalette0);
		if (BitTest(flags, Bit::Sprite::PaletteNumber) == true) {
			OGP = memory->Read(Address::OBJPalette1);
		}

		int vramBank = 0;
		if (ColorGameBoyMode == true) {
			if (BitTest(flags, Bit::Sprite::VRamBank) == true) {
				vramBank = 1;
			}
		}

		Color* palette = GetColorSpritePalette(flags & 0x7);

		word pixelDataLocation = Address::TilePattern0 + (patternNumber * 16);

		for (int y = 0; y < spriteHeight; y++) {

			int yPos = y;
			if (Yflip == true) {
				yPos -= (spriteHeight - 1);
				yPos *= -1;
			}

			byte pixelDataLow = memory->Read(pixelDataLocation + y * 2, vramBank);
			byte pixelDataHigh = memory->Read(pixelDataLocation + y * 2 + 1, vramBank);

			for (int x = 0; x < 8; x++) {

				byte pixel = GetPixelColorIndex(pixelDataLow, pixelDataHigh, x);

				int xPos = x;
				if (Xflip == true) {
					xPos -= 7;
					xPos *= -1;
				}

				if (ColorGameBoyMode == true) {

					SetPixel(spritePixels[i], 8 * 16 * 4, xPos, yPos, 8, palette[pixel]);
				}
				else {

					byte offset = pixel * 2; // offset of color data in color palette
					byte color = (OGP & (0x3 << offset)) >> offset;

					SetPixel(spritePixels[i], 8 * 16 * 4, xPos, yPos, 8, BWPalette[color]);
				}
			}
		}
	}

}



// advance the ppu further in time by the number of clock cycles
// scanlines are drawn and vblank interrupts are triggered over time
void PPU::update(word clocksThisUpdate, int speedMode) {

	clocksThisLine += clocksThisUpdate;

	byte LY = memory->Read(Address::LY);
	if (lastLY == (kScreenHeight - 1) && LY == kScreenHeight) { // vblank interrupt occurs when LY goes beyond the gameboy screen
		byte interuptFlags = memory->Read(Address::InteruptFlag);
		interuptFlags |= Bits::b0;
		memory->Write(Address::InteruptFlag, interuptFlags);
	}

	lastLY = memory->Read(Address::LY);

	if (clocksThisLine >= (clocksPerLine * speedMode)) {

		memory->WriteMemorySpace(Address::LY, memory->ReadMemorySpace(Address::LY) + 1);
		if (memory->ReadMemorySpace(Address::LY) > kLastScanLine) {
			memory->Write(Address::LY, 0);
			memory->WriteMemorySpace(Address::LY, 0);
		}

		lineDrawn = false;
		clocksThisLine -= (clocksPerLine * speedMode);
	}

	// the screen is drawn 150-225? clocks into a given line, 
	// scrollX may change immediatly after LY increment from a LCDStat interrupt
	if (clocksThisLine > 200 && lineDrawn == false) {
		drawScanLine();
		lineDrawn = true;
	}
}


// draw one row of pixels to the screen including background, window, and sprite layers
void PPU::drawScanLine() {

	if (memory->Read(Address::LY) < kScreenHeight) {

		byte lcd = memory->Read(Address::LCDC);
		if (BitTest(lcd, Bit::LCDC::LCDDisplayEnable) == false){ // screen is turned off, behaviour changes on SGB, CGB?
			byte LY = memory->Read(Address::LY);
			for (int i = 0; i < kScreenWidth; i++) {
				SetPixel(backgroundPixels, kScreenWidth * kScreenHeight * 4, i, LY, kScreenWidth, { 0xFF, 0xFF, 0xFF });
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



// draw the background layer to the screen for the current row
void PPU::drawBackground() {

	byte scrollY = memory->Read(Address::ScrollY);
	byte scrollX = memory->Read(Address::ScrollX);
	byte LY = memory->Read(Address::LY); 

	DrawBackgroundLine(scrollX, (scrollY + LY) % 256, LY, kScreenWidth, backgroundPixels, kScreenWidth * kScreenHeight * 4, true);
}


// draw sprites for the current row of pixels to the screen
void PPU::drawSprites() {

	byte LCDC = memory->Read(Address::LCDC);
	if (BitTest(LCDC, Bit::LCDC::SpriteDisplayEnable) == false) { // sprites are disabled
		return;
	}

	int spritesThisLine = 0;
	for (word i = 0; i < kNumberOfSprites; i++) {

		word spriteData = Address::SpriteAttributes + ((39 - i) * 4);
		int positionY = memory->Read(spriteData);
		if (positionY <= 0 || positionY >= 160) { // off-screen
			continue;
		}
		int positionX = memory->Read(spriteData + 1);
		if (positionX <= 0 || positionX >= 168) { // off-screen
			continue;
		}
		positionY -= 16;
		positionX -= 8;

		byte patternNumber = memory->Read(spriteData + 2);

		byte flags = memory->Read(spriteData + 3);

		bool Yflip = BitTest(flags, Bit::Sprite::YFlip);
		bool Xflip = BitTest(flags, Bit::Sprite::XFlip);

		byte spriteHeight = kSpriteSize;
		if (BitTest(LCDC, Bit::LCDC::SpriteSize) == true) {
			spriteHeight = kSpriteSize * 2;
			patternNumber &= 0b11111110;
		}

		byte grayPalette = memory->Read(Address::OBJPalette0);
		if (BitTest(flags, Bit::Sprite::PaletteNumber) == true) {
			grayPalette = memory->Read(Address::OBJPalette1);
		}

		byte LY = memory->Read(Address::LY);

		// sprite has pixels on the current line
		if (LY < positionY + spriteHeight && LY >= positionY && spritesThisLine < 10) { 

			//spritesThisLine++;
			int drawLine = LY - positionY; // row index of pixels for current ly
			if (Yflip == true) {
				drawLine -= (spriteHeight - 1);
				drawLine *= -1;
			}

			int vramBank = 0;
			if (ColorGameBoyMode == true) {
				if (BitTest(flags, Bit::Sprite::VRamBank) == true) {
					vramBank = 1;
				}
			}

			word tileLocation = Address::TilePattern0 + (patternNumber * 16);

			byte pixelDataLow = memory->Read(tileLocation + drawLine * 2, vramBank);
			byte pixelDataHigh = memory->Read(tileLocation + drawLine * 2 + 1, vramBank);

			for (int j = 0; j < 8; j++) {

				byte pixelColorIndex = GetPixelColorIndex(pixelDataLow, pixelDataHigh, j);

				int pixelOffsetX = j;
				if (Xflip == true) {
					pixelOffsetX -= 7;
					pixelOffsetX *= -1;
				}

				int pixelX = positionX + pixelOffsetX;

				if (BitTest(flags, Bit::Sprite::SpritetoBGPriority) == true && backgroundPixelsColorIndex[(LY * kScreenWidth) + pixelX] > 0 || // some sprites are not drawn if background color index is 1-3
					backgroundPixelsColorIndex[(LY * kScreenWidth) + pixelX] == 0x10 && BitTest(LCDC, Bit::LCDC::BackgroundWindowDisplayPriority) == true) { // some background tiles do not allow sprites to be drawn on top of them
					continue;
				}

				if (LY < 144 && pixelX < 160 && pixelX >= 0 && pixelColorIndex != 0) { // color 0 is transparent

					if (ColorGameBoyMode == true) {

						byte CGBPaletteNumber = flags & 0x7;
						byte vRamBank = flags & Bits::b3;

						Color* palette = GetColorSpritePalette(CGBPaletteNumber);
						SetPixel(backgroundPixels, kScreenWidth * kScreenHeight * 4, pixelX, LY, kScreenWidth, palette[pixelColorIndex]);
					}
					else {

						byte offset = pixelColorIndex * 2;
						byte color = (grayPalette & (0x3 << offset)) >> offset;

						SetPixel(backgroundPixels, kScreenWidth * kScreenHeight * 4, pixelX, LY, kScreenWidth, BWPalette[color]);
					}
				}
			}
		}
	}

}



// draws the background to a pixel array
// used for both the main screen and debug background image
void PPU::DrawBackgroundLine(int startX, int row, int screenY, int screenWidth, byte* screen, int screenSize, bool mainScreen) {

	byte LCDC = memory->Read(Address::LCDC);

	word backgroundMapAddressRowStart = Address::BGWTileInfo0;
	if (BitTest(LCDC, Bit::LCDC::BGTileMapDisplaySelect) == true) {
		backgroundMapAddressRowStart = Address::BGWTileInfo1;
	}
	backgroundMapAddressRowStart += (row / 8) * 32;

	byte BGP = memory->Read(Address::BGWPalette);

	int screenX = 0; // controlls where the background pixel is drawn to
	while (screenX < screenWidth) {

		int backgroundX = (screenX + startX) % kBackgroundLayerSize; // controlls where the background pixel is drawn from
		word backgroundMapAddress = backgroundMapAddressRowStart + (backgroundX / 8);
		word memoryTileAddress = GetTileDataAddress(LCDC, backgroundMapAddress); // address of tile data

		bool yFlip = false;
		bool xFlip = false;
		bool BGtoOAMPriority = false;
		byte VramBank = 0;

		Color* palette = nullptr;
		if (ColorGameBoyMode == true) {
			
			byte CGBAttributes = memory->Read(backgroundMapAddress, 1);

			if (BitTest(CGBAttributes, Bit::Tile::YFlip) == true) {
				yFlip = true;
			}

			if (BitTest(CGBAttributes, Bit::Tile::XFlip) == true) {
				xFlip = true;
			}

			if (BitTest(CGBAttributes, Bit::Tile::VRamBank) == true) {
				VramBank = 1;
			}

			if (BitTest(CGBAttributes, Bit::Tile::BGtoOAMPriority) == true) {
				BGtoOAMPriority = true;
			}

			palette = GetBGPalette(CGBAttributes);
		}


		int TilePixelY = row % kTileSize;
		if (yFlip == true) { // vertical flip
			TilePixelY -= (kTileSize - 1);
			TilePixelY *= -1;
		}

		word address = memoryTileAddress + TilePixelY * 2; // offset by 2 bytes per line

		byte pixelDataLow = memory->vramBank[VramBank][address - Address::TilePattern0];
		byte pixelDataHigh = memory->vramBank[VramBank][(address + 1) - Address::TilePattern0];

		do {

			int pixelX = (startX + screenX) % kTileSize;
			if (xFlip == true) { // horizontal flip
				pixelX -= (kTileSize - 1);
				pixelX *= -1;
			}

			byte pixel = GetPixelColorIndex(pixelDataLow, pixelDataHigh, pixelX);

			int screenIndex = ((screenY * screenWidth) + screenX) * kRGBASize;
			if (ColorGameBoyMode) {

				SetPixel(screen, screenSize, screenX, screenY, screenWidth, palette[pixel]);
				if (mainScreen == true && screenIndex / kRGBASize < kScreenWidth * kScreenHeight) {
					if (BGtoOAMPriority == true) {
						backgroundPixelsColorIndex[screenIndex / kRGBASize] = kBGPriority; // special BG has priority value
					}
					else {
						backgroundPixelsColorIndex[screenIndex / kRGBASize] = pixel;
					}
				}
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				SetPixel(screen, screenSize, screenX, screenY, screenWidth, BWPalette[color]);
				if (mainScreen == true && screenIndex / kRGBASize < kScreenWidth * kScreenHeight) {
					backgroundPixelsColorIndex[screenIndex / kRGBASize] = color;
				}
			}

			screenX++;
		} while (((startX + screenX) % kTileSize) != 0 && ((screenY * screenWidth) + screenX) * kRGBASize < screenSize);
	}
}

void PPU::DrawWindowLine() {

	byte LCDC = memory->Read(Address::LCDC);

	if (BitTest(LCDC, Bit::LCDC::WindowDisplayEnable) == false) {
		return;
	}

	byte windowY = memory->Read(Address::WindowY);
	byte LY = memory->Read(Address::LY);

	if (windowY > LY) {
		return;
	}

	word windowTileMap = Address::BGWTileInfo0;
	if (BitTest(LCDC, Bit::LCDC::WindowTileMapDisplaySelect) == true) {
		windowTileMap = Address::BGWTileInfo1;
	}

	byte BGP = memory->Read(Address::BGWPalette);

	byte windowX = memory->Read(Address::WindowX);

	int screenX = windowX - 7;
	byte xPosInWindow = 0;

	while (screenX < kScreenWidth) {

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

			if (BitTest(CGBAttributes, Bit::Tile::YFlip) == true) {
				yFlip = true;
			}

			if (BitTest(CGBAttributes, Bit::Tile::XFlip) == true) {
				xFlip = true;
			}

			if (BitTest(CGBAttributes, Bit::Tile::VRamBank) == true) {
				VramBank = 1;
			}

			if (BitTest(CGBAttributes, Bit::Tile::BGtoOAMPriority) == true) {
				BGtoOAMPriority = true;
			}
		}

		int TilePixelY = yPosInWindow % kTileSize;
		if (yFlip == true) {
			TilePixelY -= (kTileSize - 1);
			TilePixelY *= -1;
		}
		word address = memoryTileAddress + TilePixelY * 2;

		byte pixelDataLow = memory->vramBank[VramBank][address - Address::TilePattern0];
		byte pixelDataHigh = memory->vramBank[VramBank][(address + 1) - Address::TilePattern0];

		do {

			int pixelX = (xPosInWindow) % kTileSize;
			if (xFlip == true) { // x flip
				pixelX -= (kTileSize - 1);
				pixelX *= -1;
			}

			byte pixel = GetPixelColorIndex(pixelDataLow, pixelDataHigh, pixelX);

			int screenIndex = ((LY * kScreenWidth) + screenX);
			if (ColorGameBoyMode) {

				Color* palette = GetBGPalette(CGBAttributes);
				SetPixel(backgroundPixels, kScreenWidth * kScreenHeight * kRGBASize, screenX, LY, kScreenWidth, palette[pixel]);
				if (BGtoOAMPriority == true) {
					backgroundPixelsColorIndex[screenIndex] = kBGPriority; // special BG has priority value
				}
				else {
					backgroundPixelsColorIndex[screenIndex] = pixel;
				}
			}
			else {

				byte offset = pixel * 2;
				byte color = (BGP & (0x3 << offset)) >> offset;

				SetPixel(backgroundPixels, kScreenWidth * kScreenHeight * kRGBASize, screenX, LY, kScreenWidth, BWPalette[pixel]);
				backgroundPixelsColorIndex[screenIndex] = color;
			}

			screenX++;
			xPosInWindow++;
		} while ((xPosInWindow % 8) != 0 && screenX < kScreenWidth);
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

byte PPU::GetPixelColorIndex(byte dataLow, byte dataHigh, byte pixelX) {

	byte pixelIndex = 0;

	if (BitTestReverse(dataLow, pixelX) == true) {
		pixelIndex |= 0b01;
	}
	if (BitTestReverse(dataHigh, pixelX) == true) {
		pixelIndex |= 0b10;
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

			word colorData = memory->BGColorPalette[(paletteNumber * 8) + i];
			colorData |= memory->BGColorPalette[(paletteNumber * 8) + i + 1] << 8;

			byte red =    colorData & 0b0000000000011111;		// 0x001F
			byte green = (colorData & 0b0000001111100000) >> 5; // 0x03E0
			byte blue =  (colorData & 0b0111110000000000) >> 10;// 0x7C00

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

		word colorData = memory->SpriteColorPalette[(paletteNumber * 8) + i];
		colorData |= memory->SpriteColorPalette[(paletteNumber * 8) + i + 1] << 8;

		byte red =    colorData & 0b0000000000011111;		// 0x001F
		byte green = (colorData & 0b0000001111100000) >> 5; // 0x03E0
		byte blue =  (colorData & 0b0111110000000000) >> 10;// 0x7C00

		red <<= 3;
		green <<= 3;
		blue <<= 3;

		CGBSpritePalette[i / 2] = { red, green, blue };
	}

	return CGBSpritePalette;
}
