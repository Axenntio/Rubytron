#include <fstream>
#include <sstream>
#include <Window.hpp>
#include <Desktop.hpp>

Window::Window(unsigned int width, unsigned int height, const std::vector<sf::Color>& palette, const std::string& programPath) : _width(width), _height(height)
{
	this->_palette = palette;
	this->_texture.create(this->_width, this->_height);
	this->_texture.clear(this->_palette[0]);
	this->_mrb = mrb_open();
	mrb_define_method(this->_mrb, this->_mrb->object_class, "clear", &Window::mrubyClear, MRB_ARGS_REQ(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "pxl", &Window::mrubyPixel, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &Window::mrubyLine, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	std::ifstream file(programPath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	this->execute(buffer.str());
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "init"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "init", 0);
	}
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
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "update"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "update", 0);
	}

	states.transform *= getTransform();

	states.texture = NULL;

	sf::Sprite sprite(this->_texture.getTexture());
	sprite.setPosition(10, 10); // Update with position of window
	sprite.setTextureRect(sf::IntRect(0, this->_height, this->_width, -this->_height));

	target.draw(sprite, states);

	if (this->_mrb->exc) {
		mrb_print_error(this->_mrb);
	}
}

bool Window::isContext(mrb_state* mrb) const
{
	return this->_mrb == mrb;
}

extern Desktop desktop;

mrb_value Window::mrubyClear(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();


	mrb_int paletteIndex;
	mrb_get_args(mrb, "i", &paletteIndex);

	window->_texture.clear(window->_palette[paletteIndex]);

	return mrb_nil_value();
}

mrb_value Window::mrubyPixel(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int x, y;
	mrb_int colorPalette = 1;
	mrb_get_args(mrb, "ii|i", &x, &y, &colorPalette);

	sf::Vertex point[] = {
		sf::Vertex(sf::Vector2f(x + 1, y), window->_palette[colorPalette]) // Not sure about the +1 trick, but otherwise the pixel is shifted
	};

	window->_texture.draw(point, 1, sf::Points);

	return mrb_nil_value();
}

mrb_value Window::mrubyLine(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int x1, x2, y1, y2;
	mrb_int colorPalette = 1;
	mrb_get_args(mrb, "iiii|i", &x1, &y1, &x2, &y2, &colorPalette);

	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(x1, y1), window->_palette[colorPalette]),
		sf::Vertex(sf::Vector2f(x2, y2), window->_palette[colorPalette])
	};

	window->_texture.draw(line, 2, sf::Lines);

	return mrb_nil_value();
}
