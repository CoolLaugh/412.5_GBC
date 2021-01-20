#pragma once

typedef unsigned char byte;
typedef unsigned short word;

const int ScreenWidth = 160;
const int ScreenHeight = 144;
const int DisplayScale = 3;

enum Address {

	CGBFlag = 0x0143,
	CartridgeType = 0x0147,
	RomSize = 0x0148,
	RamSize = 0x0149,

	TilePattern0 = 0x8000,
	TilePattern1 = 0x8800,

	BGWTileInfo0 = 0x9800,
	BGWTileInfo1 = 0x9C00,

	SpriteAttributes = 0xFE00,

	Joypad = 0xFF00,
	SerialTransfer = 0xFF01,
	SIOControl = 0xFF02,
	DIVRegister = 0xFF04,
	Timer = 0xFF05,
	TimerModulo = 0xFF06,
	TimerControl = 0xFF07,
	InteruptFlag = 0xFF0F,
	LCDC = 0xFF40,
	LCDCStatus = 0xFF41,
	ScrollY = 0xFF42,
	ScrollX = 0xFF43,
	LY = 0xFF44,
	LYCompare = 0xFF45,
	DMA = 0xFF46,
	BGWPalette = 0xFF47,
	OBJPalette0 = 0xFF48,
	OBJPalette1 = 0xFF49,
	WindowY = 0xFF4A,
	WindowX = 0xFF4B,
	PrepareSpeedSwitch = 0xFF4D,
	VRAMBank = 0xFF4F,
	InfraredPort = 0xFF56,
	GBColorBackgroundPaletteIndex = 0xFF68,
	GBColorBackgroundPaletteData = 0xFF69,
	GBColorSpritePaletteIndex = 0xFF68,
	GBColorSpritePaletteData = 0xFF69,
	WramBank = 0xFF70,
	InteruptEnable = 0xFFFF

};

enum Bits {
	b0 = (1 << 0),
	b1 = (1 << 1),
	b2 = (1 << 2),
	b3 = (1 << 3),
	b4 = (1 << 4),
	b5 = (1 << 5),
	b6 = (1 << 6),
	b7 = (1 << 7)
};

bool BitTest(byte value, int bit);