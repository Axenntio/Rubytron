#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette);
	~Window();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	mrb_value mrubyLine(mrb_state *mrb, mrb_value self);

	sf::RenderTexture _texture;
	std::vector<sf::Color> _palette;
	unsigned int _width;
	unsigned int _height;
	sf::Vector2u _position;

	mrb_state* _mrb;
};