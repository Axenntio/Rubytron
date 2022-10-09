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

	static mrb_value mrubyGetWidth(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetWidth(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetHeight(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetHeight(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMinWidth(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMinWidth(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMinHeight(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMinHeight(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMouseX(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMouseX(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMouseY(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMouseY(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetTitle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetTitle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyIsResizable(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetResizable(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyClose(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyReload(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyParameters(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyFocused(mrb_state *mrb, mrb_value self);

	static mrb_value mrubyProcesses(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKillProcess(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySpawn(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyExport(mrb_state *mrb, mrb_value self);

	static mrb_value mrubyClear(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyRectangle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyCircle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyText(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKey(mrb_state *mrb, mrb_value self);

	sf::RenderWindow _window;
	sf::RenderTexture _cursor_texture;
	sf::Vector2i _mouse_coordinated;
	sf::View _canvas_view;
	bool _isFocused;
};