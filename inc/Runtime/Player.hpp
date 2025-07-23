#pragma once

#include <mruby.h>
#include <mruby/compile.h>
#include <Shared/AbstractWindow.hpp>

class Player : public AbstractWindow {
public:
	Player(sf::Vector2u size, unsigned char scale, const std::vector<std::string>& parameters);

	void run();
private:
	void closeEvent(const sf::Event::Closed *event);
	void resizeEvent(const sf::Event::Resized *event);
	void mouseButtonPressEvent(const sf::Event::MouseButtonPressed *event);
	void mouseButtonReleaseEvent(const sf::Event::MouseButtonReleased *event);
	void mouseMoveEvent(const sf::Event::MouseMoved *event);
	void keyPressEvent(const sf::Event::KeyPressed *event);
	void keyReleaseEvent(const sf::Event::KeyReleased *event);
	void textEvent(const sf::Event::TextEntered *event);
	void mouseWheelEvent(const sf::Event::MouseWheelScrolled *event);

	void resizeTrigger();
	void changeTitleTrigger();

	sf::RenderWindow _window;
	sf::RenderTexture _cursor_texture;
	sf::Vector2i _mouse_coordinated;
	sf::View _canvas_view;
	unsigned char _scale;
};