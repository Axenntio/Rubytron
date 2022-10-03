#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>
#include <WindowZone.hh>
#include <TitleBarMode.hh>

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, TitleBarMode _titleBarMode, const std::string& programPath, const std::vector<std::string>& parameters);
	~Window();

	void loadFile();
	void reloadFile();
	void execute(const std::string& string);

	void init();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2i getSize() const;

	void exceptionHandler();

	void resize(sf::Vector2i size);
	void resizeH(int width);
	void resizeV(int height);
	bool isContext(mrb_state* mrb) const;
	bool isIn(WindowZone zone, sf::Vector2i point) const;
	bool isClosed() const;
	void close();

	void setMousePosition(sf::Vector2i position);
	void addKeyPressed(sf::Keyboard::Key key);
	void removeKeyPressed(sf::Keyboard::Key key);
	void textEnteredEvent(sf::Uint32 unicode);
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

	sf::RenderTexture _texture;
	sf::RenderTexture _barTexture;
	std::vector<sf::Color> _palette;
	sf::Vector2i _size;
	sf::Vector2i _minSize;
	sf::Vector2i _mousePosition;
	std::vector<sf::Keyboard::Key> _keyPressed;
	std::string _title;
	TitleBarMode _titleBarMode;
	bool _resizable;
	bool _closed;

	std::string _programFile;
	std::vector<std::string> _programParameters;
	mrb_state* _mrb;
	mrbc_context* _mrbContext;
	RClass *_mrbWindowClass;
	RClass *_mrbDesktopClass;
};