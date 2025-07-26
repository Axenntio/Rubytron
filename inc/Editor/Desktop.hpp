#pragma once

#include <vector>
#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Editor/Window.hpp>
#include <Editor/TitleBarMode.hh>
#include <Editor/FocusAction.hh>

class Desktop {
public:
	Desktop(unsigned int width, unsigned int height, unsigned char scale = 1, TitleBarMode titleBarMode = TitleBarMode::Full);

	void run();
	std::shared_ptr<Window> getWindow(mrb_state *mrb) const;
	sf::Vector2u getSize() const;
	bool isFocused(const Window* window) const;

	bool programExport(const std::string& path) const;
	bool spawn(const std::string& path, const std::vector<std::string>& parameters = {});
	bool spawn(sf::Vector2i position, sf::Vector2u size, const std::string& path, const std::vector<std::string>& parameters = {});
	const std::vector<std::shared_ptr<Window>>& getWindows() const;
	bool killWindow(unsigned int processId);

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

	sf::RenderWindow _window;
	sf::RenderTexture _background_texture;
	sf::RenderTexture _foreground_texture;
	sf::RenderTexture _cursor_texture;
	sf::Vector2i _mouse_coordinated;
	std::vector<sf::Color> _palette;
	sf::View _canvas_view;
	sf::Vector2u _size;
	TitleBarMode _titleBarMode;

	std::shared_ptr<Window> _focusedWindow;
	FocusAction _focusAction;
	sf::Vector2f _focusInitialDelta;
	std::vector<std::shared_ptr<Window>> _windows;
	unsigned int _windowsPid;
};
