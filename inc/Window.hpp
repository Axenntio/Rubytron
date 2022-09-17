#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath);
	~Window();

	void execute(const std::string& string);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	bool isContext(mrb_state* mrb) const;
	bool isIn(sf::Vector2i point) const;

	static mrb_value mrubyClear(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(mrb_state *mrb, mrb_value self);
private:

	sf::RenderTexture _texture;
	std::vector<sf::Color> _palette;
	sf::Vector2i _position;
	sf::Vector2u _size;

	mrb_state* _mrb;
};