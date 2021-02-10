#include "SFMLGraphics.h"

SFMLGraphics::SFMLGraphics() {
	setupWindow();
	setupTileWindow();
	setupBGMapWindow();
	setupColorPaletteWindow();
}

void SFMLGraphics::setPixelPointers(byte * mainWindow, byte * tileMemory, byte * BGMap, byte * ColorPalette) {

	backgroundPixels = mainWindow;
	tileMemoryBackgroundPixels = tileMemory;
	BGMapBackgroundPixels = BGMap;
	ColorPaletteBackgroundPixels = ColorPalette;
}

void SFMLGraphics::setupWindow() {

	window = new sf::RenderWindow(sf::VideoMode(ScreenWidth * DisplayScale, ScreenHeight * DisplayScale), "Gameboy Emulator");
	window->setPosition(sf::Vector2i(950, 150));
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	view = new sf::View(sf::FloatRect(0, 0, ScreenWidth, ScreenHeight));
	window->setView(*view);
}

void SFMLGraphics::setupTileWindow() {

	tileMemoryWindow = new sf::RenderWindow(sf::VideoMode((256) * DisplayScale, (192) * DisplayScale), "VRAM Tiles");
	tileMemoryWindow->setPosition(sf::Vector2i(150, 150));
	tileMemoryWindow->setVerticalSyncEnabled(false);
	tileMemoryWindow->setFramerateLimit(60);

	tileMemoryView = new sf::View(sf::FloatRect(0, 0, 256, 192));
	tileMemoryWindow->setView(*tileMemoryView);
}

void SFMLGraphics::setupBGMapWindow() {

	BGMapWindow = new sf::RenderWindow(sf::VideoMode(256 * DisplayScale, 256 * DisplayScale), "VRAM BGMap");
	BGMapWindow->setPosition(sf::Vector2i(1450, 150));
	BGMapWindow->setVerticalSyncEnabled(false);
	BGMapWindow->setFramerateLimit(60);

	BGMapView = new sf::View(sf::FloatRect(0, 0, 256, 256));
	BGMapWindow->setView(*BGMapView);
}

void SFMLGraphics::setupColorPaletteWindow() {

	ColorPaletteWindow = new sf::RenderWindow(sf::VideoMode(4 * DisplayScale * 12, 16 * DisplayScale * 12), "Color Palettes");
	ColorPaletteWindow->setPosition(sf::Vector2i(950, 650));
	ColorPaletteWindow->setVerticalSyncEnabled(false);
	ColorPaletteWindow->setFramerateLimit(60);

	ColorPaletteView = new sf::View(sf::FloatRect(0, 0, 4, 16));
	ColorPaletteWindow->setView(*ColorPaletteView);
}

void SFMLGraphics::updateWindow() {

	updateWindowGeneric(window, view, backgroundPixels, ScreenWidth, ScreenHeight);
}

void SFMLGraphics::updateTileWindow() {

	updateWindowGeneric(tileMemoryWindow, tileMemoryView, tileMemoryBackgroundPixels, 256, 192);
}

void SFMLGraphics::updateBGMapWindow() {

	updateWindowGeneric(BGMapWindow, BGMapView, BGMapBackgroundPixels, 256, 256);
}

void SFMLGraphics::updateColorPaletteWindow() {

	updateWindowGeneric(ColorPaletteWindow, ColorPaletteView, ColorPaletteBackgroundPixels, 4, 16);
}

void SFMLGraphics::updateWindowGeneric(sf::RenderWindow * screen, sf::View * view, sf::Uint8 * pixels, int x, int y, bool display) {

	screen->clear();
	screen->setView(*view);
	sf::Image image;
	image.create(x, y, pixels);
	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite sprite;
	sprite.setTexture(texture, true);
	sprite.setPosition(0, 0);
	screen->draw(sprite);
	if (display == true) {

		screen->display();
	}
}
