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
	bool spawn(const std::string& path, const std::vector<std::string>& parameters);
	bool spawn(sf::Vector2i position, sf::Vector2u size, const std::string& path, const std::vector<std::string>& parameters);
	const std::vector<std::shared_ptr<Window>>& getWindows() const;
	bool killWindow(unsigned int processId);

private:
	void closeEvent(sf::Event event);
	void resizeEvent(sf::Event event);
	void mouseButtonPressEvent(sf::Event event);
	void mouseButtonReleaseEvent(sf::Event event);
	void mouseMoveEvent(sf::Event event);
	void keyPressEvent(sf::Event event);
	void keyReleaseEvent(sf::Event event);
	void textEvent(sf::Event event);

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
};