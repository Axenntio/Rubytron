#pragma once

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Window.hpp>

class Desktop {
public:
	Desktop(unsigned int width, unsigned int height, unsigned char scale = 1);

	void run();
	Window* getWindow(mrb_state *mrb);

private:
	void resizeEvent(sf::Event event);
	void closeEvent(sf::Event event);

	sf::RenderWindow _window;
	sf::RenderTexture _background_texture;
	sf::RenderTexture _foreground_texture;
	std::vector<sf::Color> _palette;
	sf::View _canvas_view;
	unsigned int _width;
	unsigned int _height;

	std::vector<Window*> _windows;
};