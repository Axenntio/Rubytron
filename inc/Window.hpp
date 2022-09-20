#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>

enum WindowZone {
	All,
	TitleBar,
	Canvas,
	BottomRight
};

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath);
	~Window();

	void execute(const std::string& string);

	void init();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2u getSize() const;
	void resize(sf::Vector2u size);

	bool isContext(mrb_state* mrb) const;
	bool isIn(WindowZone zone, sf::Vector2i point) const;

	void setMousePosition(sf::Vector2f position);
	void addKeyPressed(sf::Keyboard::Key key);
	void removeKeyPressed(sf::Keyboard::Key key);

private:
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

	static mrb_value mrubyClear(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyRectangle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyCircle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyKey(mrb_state *mrb, mrb_value self);

	sf::RenderTexture _texture;
	std::vector<sf::Color> _palette;
	sf::Vector2u _size;
	sf::Vector2u _minSize;
	sf::Vector2i _mousePosition;
	std::vector<sf::Keyboard::Key> _keyPressed;
	std::string _title;

	mrb_state* _mrb;
	RClass *_mrbWindowClass;
};