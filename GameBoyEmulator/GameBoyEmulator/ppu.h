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

	word clocksThisLine = 0;
	byte lastLY = 0;
	bool ColorGameBoyMode = false;

	byte* backgroundPixels;
	byte* tileMemoryPixels;
	byte* BGMapBackgroundPixels;
	byte* ColorPalettePixels;
	std::vector<byte*> spritePixels;

	word debugTileLocation = 0;
	word debugMapLocation = 0;

private:

	byte* backgroundPixelsColorIndex;


	Color BWPalette[4] = { {0xFF,0xFF,0xFF},
							{0xAA,0xAA,0xAA},
							{0x55,0x55,0x55},
							{0x00,0x00,0x00} };

	Color CGBPalette[4];
	Color CGBSpritePalette[4];
	bool lineDrawn = false;

public:

	Memory* memory;

	PPU();
	~PPU();

	void updateTileImage();
	void updateBGMapImage();
	void updateColorPaletteImage();
	void updateSpriteImages();

	void update(word clocksThisUpdate, int speedMode);
	void drawScanLine();
	void drawBackground();
	void drawSprites();

	void DrawBackgroundLine(int startX, int row, int screenY, int screenWidth, byte* screen, int screenSize, bool mainScreen = false);
	void DrawWindowLine();

	void SetPixel(byte* screen, int screenSize, int x, int y, int screenWidth, Color color);
	byte GetPixelColorIndex(byte dataLow, byte dataHigh, byte pixelX);
	word GetTileDataAddress(byte LCDC, word address);

	Color* GetBGPalette(byte CGBMapAttributes);
	Color* GetColorSpritePalette(int paletteNumber);
};

