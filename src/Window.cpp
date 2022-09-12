#include <Window.hpp>
#include <Desktop.hpp>

Window::Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette) : _width(width), _height(height)
{
	this->_palette = palette;
	this->_texture.create(this->_width, this->_height);
	this->_texture.clear(this->_palette[3]);
	this->_mrb = mrb_open();
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &Window::mrubyLine, MRB_ARGS_REQ(4));
}

Window::~Window()
{
	mrb_close(this->_mrb);
}

void Window::execute(const std::string& string)
{
	mrb_load_string(this->_mrb, string.c_str());
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = NULL;

	sf::Sprite sprite(this->_texture.getTexture());

	target.draw(sprite, states);
}

bool Window::isContext(mrb_state* mrb) const
{
	return this->_mrb == mrb;
}


#include <iostream>
#include <mruby/variable.h>

extern Desktop desktop;

mrb_value Window::mrubyLine(mrb_state* mrb, mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();


	mrb_int x1;
	mrb_int x2;
	mrb_int y1;
	mrb_int y2;
	mrb_get_args(mrb, "iiii", &x1, &y1, &x2, &y2);

	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(x1, y1)),
		sf::Vertex(sf::Vector2f(x2, y2))
	};

	window->_texture.draw(line, 2, sf::Lines);

	return mrb_nil_value();
}
