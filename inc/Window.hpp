#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath);
	~Window();

	void execute(const std::string& string);

	void init() const;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	bool isContext(mrb_state* mrb) const;
	bool isIn(sf::Vector2i point) const;

	static mrb_value mrubyGetWidth(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyGetHeight(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetWidth(mrb_state *mrb, mrb_value self);
	static mrb_value mrubySetHeight(mrb_state *mrb, mrb_value self);

	static mrb_value mrubyClear(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyRectangle(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyCircle(mrb_state *mrb, mrb_value self);
private:

	sf::RenderTexture _texture;
	std::vector<sf::Color> _palette;
	sf::Vector2u _size;

	mrb_state* _mrb;
};