#pragma once

#include <mruby.h>
#include <mruby/compile.h>
#include <Shared/AbstractWindow.hpp>
#include <Editor/WindowZone.hh>
#include <Editor/TitleBarMode.hh>

class Window : public AbstractWindow {
public:
	Window(unsigned int pid, sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, TitleBarMode _titleBarMode, const std::string& programPath, const std::vector<std::string>& parameters);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	bool isContext(mrb_state* mrb) const;
	bool isIn(WindowZone zone, sf::Vector2i point) const;
	void toggleFullscreen();
	unsigned int getPid() const;

	void addKeyPressed(sf::Keyboard::Key key);
	void focusEvent(bool isFocused);

private:
	void resizeTrigger();
	void changeTitleTrigger();
	void titleBarRefresh();

	static mrb_value mrubyProcesses(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKillProcess(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySpawn(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyExport(mrb_state *mrb, mrb_value self);

	unsigned int _pid;
	sf::RenderTexture _barTexture;
	TitleBarMode _titleBarMode;
};
