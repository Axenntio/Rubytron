#pragma once

#include <mruby/compile.h>
#include <Shared/AbstractWindow.hpp>

class Player : public AbstractWindow {
public:
	Player(sf::Vector2u size, unsigned char scale, const std::vector<std::string>& parameters);

	void run();
private:
	void closeEvent(sf::Event event);
	void resizeEvent(sf::Event event);
	void mouseButtonPressEvent(sf::Event event);
	void mouseButtonReleaseEvent(sf::Event event);
	void mouseMoveEvent(sf::Event event);
	void keyPressEvent(sf::Event event);
	void keyReleaseEvent(sf::Event event);
	void textEvent(sf::Event event);

	void resizeTrigger();
	void changeTitleTrigger();

	sf::RenderWindow _window;
	sf::RenderTexture _cursor_texture;
	sf::Vector2i _mouse_coordinated;
	sf::View _canvas_view;
	unsigned char _scale;
};