#include "SFMLGraphics.h"

SFMLGraphics::SFMLGraphics() {
	setupWindow();
}

SFMLGraphics::~SFMLGraphics() {
	delete window;
	delete view;
}

void SFMLGraphics::setPixelPointers(byte * mainWindow, byte * tileMemory, byte * BGMap, byte * ColorPalette) {

	backgroundPixels = mainWindow;
}

void SFMLGraphics::clearPixelPointers() {

	backgroundPixels = nullptr;
}

void SFMLGraphics::setupWindow() {

	window = new sf::RenderWindow(sf::VideoMode(kScreenWidth * kDefaultDisplayScale, kScreenHeight * kDefaultDisplayScale), kTitleName);
	window->setPosition(sf::Vector2i(950, 150));
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	view = new sf::View(sf::FloatRect(0, 0, kScreenWidth, kScreenHeight));
	window->setView(*view);
}

void SFMLGraphics::updateWindow(int scale) {
	float offset = 20.f / (kScreenHeight * scale + 20 / scale);
	view->setViewport(sf::FloatRect(0.f, offset, 1.f, 1.f - offset));
	updateWindowGeneric(window, view, backgroundPixels, kScreenWidth, kScreenHeight,false);
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
