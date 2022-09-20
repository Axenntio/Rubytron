#pragma once

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Window.hpp>

enum FocusAction {
	None,
	Move,
	Resize
};

class Desktop {
public:
	Desktop(unsigned int width, unsigned int height, unsigned char scale = 1);

	void run();
	Window* getWindow(mrb_state *mrb) const;
	bool isFocused(const Window* window) const;

private:
	void closeEvent(sf::Event event);
	void resizeEvent(sf::Event event);
	void mouseButtonPressEvent(sf::Event event);
	void mouseButtonReleaseEvent(sf::Event event);
	void mouseMoveEvent(sf::Event event);
	void keyPressEvent(sf::Event event);
	void keyReleaseEvent(sf::Event event);

	sf::RenderWindow _window;
	sf::RenderTexture _background_texture;
	sf::RenderTexture _foreground_texture;
	std::vector<sf::Color> _palette;
	sf::View _canvas_view;
	unsigned int _width;
	unsigned int _height;

	Window* _focusedWindow;
	FocusAction _focusAction;
	sf::Vector2f _focusInitialDelta;
	std::vector<Window*> _windows;
};