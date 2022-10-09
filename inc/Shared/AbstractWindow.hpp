#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>
#include <Editor/WindowZone.hh>
#include <Editor/TitleBarMode.hh>

class AbstractWindow : public sf::Drawable, public sf::Transformable {
public:
	AbstractWindow(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath, const std::vector<std::string>& parameters);
	~AbstractWindow();

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
	bool isClosed() const;
	void close();

	void setMousePosition(sf::Vector2i position);
	virtual void addKeyPressed(sf::Keyboard::Key key);
	void removeKeyPressed(sf::Keyboard::Key key);
	void textEnteredEvent(sf::Uint32 unicode);
	virtual void focusEvent(bool isFocused);

protected:
	virtual void resizeTrigger();

	static mrb_value mrubyGetWidth(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetWidth(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetHeight(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetHeight(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMinWidth(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMinWidth(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMinHeight(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMinHeight(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMouseX(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMouseX(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetMouseY(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetMouseY(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetTitle(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetTitle(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyIsResizable(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetResizable(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyClose(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyReload(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyParameters(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyFocused(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);

	static mrb_value mrubyProcesses(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKillProcess(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubySpawn(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyExport(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);

	static mrb_value mrubyClear(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyRectangle(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyCircle(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyText(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKey(std::shared_ptr<AbstractWindow> window, mrb_state *mrb, mrb_value self);

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