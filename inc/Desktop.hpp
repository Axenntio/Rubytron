#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Desktop {
public:
	Desktop(unsigned int w, unsigned int h, unsigned char scale);

	void run();

private:
	sf::RenderWindow _window;
	sf::RenderTexture _window_texture;
	sf::View _canvas_view;
	unsigned int _width;
	unsigned int _height;
	unsigned char _scale;
};