#pragma once

#include <SFML/Graphics.hpp>
#include <mruby/compile.h>

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette, const std::string& programPath);
	~Window();

	void execute(const std::string& string);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	bool isContext(mrb_state* mrb) const;

	static mrb_value mrubyClear(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyPixel(mrb_state *mrb, mrb_value self);
	static mrb_value mrubyLine(mrb_state *mrb, mrb_value self);
private:

	sf::RenderTexture _texture;
	std::vector<sf::Color> _palette;
	unsigned int _width;
	unsigned int _height;
	sf::Vector2u _position;

	mrb_state* _mrb;
};