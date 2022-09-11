#pragma once

#include <SFML/Graphics.hpp>

class Window : public sf::Drawable, public sf::Transformable {
public:
	Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::RenderTexture _texture;
	std::vector<sf::Color> _palette;
	unsigned int _width;
	unsigned int _height;
	sf::Vector2u _position;
};