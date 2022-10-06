#pragma once

#include <mruby/compile.h>
#include <Shared/AbstractWindow.hpp>
#include <Editor/WindowZone.hh>
#include <Editor/TitleBarMode.hh>

class Window : public AbstractWindow {
public:
	Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, TitleBarMode _titleBarMode, const std::string& programPath, const std::vector<std::string>& parameters);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	bool isContext(mrb_state* mrb) const;
	bool isIn(WindowZone zone, sf::Vector2i point) const;

	void addKeyPressed(sf::Keyboard::Key key);
	void focusEvent(bool isFocused);

private:
	void resizeTrigger();
	void titleBarRefresh();

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

	static mrb_value mrubyClear(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyRectangle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyCircle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyText(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKey(mrb_state *mrb, mrb_value self);

	sf::RenderTexture _barTexture;
	TitleBarMode _titleBarMode;
	bool _isFocused;
};