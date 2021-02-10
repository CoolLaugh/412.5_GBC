#pragma once
#include "memory.h"
#include "CommonDefinitions.h"

struct Color {
	byte red;
	byte green;
	byte blue;
};

class PPU {

public:

	word cyclesThisLine = 0;
	byte lastLY = 0;
	bool ColorGameBoyMode = false;

	byte* backgroundPixels;
	byte* tileMemoryPixels;
	byte* BGMapBackgroundPixels;
	byte* ColorPalettePixels;

private:



	const short cyclesPerLine = 456;

	byte* backgroundPixelsColorIndex;


	Color BWPalette[4] = { {0xFF,0xFF,0xFF},
							{0xAA,0xAA,0xAA},
							{0x55,0x55,0x55},
							{0x00,0x00,0x00} };

	Color CGBPalette[4];
	Color CGBSpritePalette[4];

public:

	Memory* memory;

	PPU();

	void updateTileWindow();
	void updateBGMapWindow();
	void updateColorPaletteWindow();

	void update(word cyclesThisUpdate, int speedMode);
	void drawScanLine();
	void drawBackground();
	byte bitData(byte val, byte bit);
	void drawSprites();

	void DrawBackgroundLine(int startX, int row, int screenY, int screenWidth, byte* screen, bool mainScreen = false);
	void DrawWindowLine();

	void SetPixel(byte* screen, int x, int y, int screenWidth, Color color);
	byte GetPixelIndex(byte dataLow, byte dataHigh, byte pixelX);
	word GetTileDataAddress(byte LCDC, word address);

	Color* GetBGPalette(byte CGBMapAttributes);
	Color* GetColorSpritePalette(int paletteNumber);
};

