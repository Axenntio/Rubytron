#include <Window.hpp>

Window::Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette) : _width(width), _height(height)
{
	this->_palette = palette;
	this->_texture.create(this->_width, this->_height);
	this->_texture.clear(this->_palette[3]);
	this->_mrb = mrb_open();
	// mrb_define_method(this->_mrb, this->_mrb->object_class, "line", , MRB_ARGS_NONE());
}

Window::~Window()
{
	mrb_close(this->_mrb);
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = NULL;

	sf::Sprite sprite(this->_texture.getTexture());

	target.draw(sprite, states);
}

mrb_value Window::mrubyLine(mrb_state* mrb, mrb_value self)
{
	return mrb_nil_value();
}