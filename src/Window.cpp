#include <Window.hpp>

Window::Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette) : _width(width), _height(height)
{
	this->_palette = palette;
	this->_texture.create(this->_width, this->_height);
	this->_texture.clear(this->_palette[3]);
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = NULL;

	sf::Sprite sprite(this->_texture.getTexture());

	target.draw(sprite, states);
}