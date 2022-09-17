#include <fstream>
#include <sstream>
#include <Window.hpp>
#include <Desktop.hpp>

Window::Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath) : _position(position), _size(size)
{
	this->_palette = palette;
	this->_texture.create(this->_size.x, this->_size.y);
	this->_texture.clear(this->_palette[0]);
	this->_mrb = mrb_open();

	RClass *windowClass = mrb_define_class(this->_mrb, "Window", this->_mrb->object_class);
	mrb_define_class_method(this->_mrb, windowClass, "width", &Window::mrubyGetWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, windowClass, "height", &Window::mrubyGetHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, windowClass, "width=", &Window::mrubySetWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, windowClass, "height=", &Window::mrubySetHeight, MRB_ARGS_REQ(1));

	mrb_define_method(this->_mrb, this->_mrb->object_class, "clear", &Window::mrubyClear, MRB_ARGS_REQ(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "pxl", &Window::mrubyPixel, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &Window::mrubyLine, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	std::ifstream file(programPath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	this->execute(buffer.str());
}

Window::~Window()
{
	mrb_close(this->_mrb);
}

void Window::execute(const std::string& string)
{
	mrb_load_string(this->_mrb, string.c_str());
}

void Window::init() const
{
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "init"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "init", 0);
	}
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "update"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "update", 0);
	}

	states.transform *= getTransform();

	states.texture = NULL;

	sf::Sprite sprite(this->_texture.getTexture());
	sprite.setPosition(sf::Vector2f(this->_position));
	sprite.setTextureRect(sf::IntRect(0, this->_size.y, this->_size.x, -this->_size.y));

	target.draw(sprite, states);

	if (this->_mrb->exc) {
		mrb_print_error(this->_mrb);
	}
}

bool Window::isContext(mrb_state* mrb) const
{
	return this->_mrb == mrb;
}

bool Window::isIn(sf::Vector2i point) const
{
	return
		point.x >= this->_position.x &&
		point.x <= this->_position.x + static_cast<int>(this->_size.x) &&
		point.y >= this->_position.y &&
		point.y <= this->_position.y + static_cast<int>(this->_size.y);
}

extern Desktop desktop;

mrb_value Window::mrubyGetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_size.x);
}

mrb_value Window::mrubyGetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_size.y);
}

mrb_value Window::mrubySetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_size.x = newWidth;

	return mrb_nil_value();
}

mrb_value Window::mrubySetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_size.y = newHeight;

	return mrb_nil_value();
}

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
