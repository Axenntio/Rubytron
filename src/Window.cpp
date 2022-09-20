#include <fstream>
#include <sstream>
#include <Window.hpp>
#include <Desktop.hpp>

extern Desktop desktop;

Window::Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath) : _size(size), _title(programPath)
{
	this->setPosition(sf::Vector2f(position));
	this->_minSize = sf::Vector2u(8, 4);
	this->_palette = palette;
	this->_texture.create(this->_size.x, this->_size.y);
	this->_texture.clear(this->_palette[0]);
	this->_lastKey = sf::Keyboard::Unknown;
	this->_mrb = mrb_open();

	if (!this->_mrb) {
		throw new std::runtime_error("Unable to start mruby for window");
	}

	this->_mrbWindowClass = mrb_define_class(this->_mrb, "Window", this->_mrb->object_class);
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "width", &Window::mrubyGetWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "width=", &Window::mrubySetWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "height", &Window::mrubyGetHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "height=", &Window::mrubySetHeight, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_width", &Window::mrubyGetMinWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_width=", &Window::mrubySetMinWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_height", &Window::mrubyGetMinHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_height=", &Window::mrubySetMinHeight, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_x", &Window::mrubyGetMouseX, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_x=", &Window::mrubySetMouseX, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_y", &Window::mrubyGetMouseY, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_y=", &Window::mrubySetMouseY, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "title", &Window::mrubyGetTitle, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "title=", &Window::mrubySetTitle, MRB_ARGS_REQ(1));


	mrb_define_method(this->_mrb, this->_mrb->object_class, "clear", &Window::mrubyClear, MRB_ARGS_REQ(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "pxl", &Window::mrubyPixel, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &Window::mrubyLine, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "rect", &Window::mrubyRectangle, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "circle", &Window::mrubyCircle, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "key", &Window::mrubyKey, MRB_ARGS_OPT(1));

	std::ifstream file(programPath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	this->execute(buffer.str());
}

Window::~Window()
{
	// if (mrb_obj_respond_to(this->_mrb, this->_mrbWindowClass, mrb_intern_cstr(this->_mrb, "close_event"))) {
	// 	mrb_funcall(this->_mrb, mrb_nil_value(), "close_event", 0);
	// }
	mrb_close(this->_mrb);
}

void Window::execute(const std::string& string)
{
	mrb_load_string(this->_mrb, string.c_str());
}

void Window::init()
{
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "init"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "init", 0);
	}
	this->_mrbWindowClass = mrb_class_get(this->_mrb, "Window");
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "update"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "update", 0);
	}

	states.transform *= getTransform();

	states.texture = NULL;

	sf::RectangleShape decorator(static_cast<sf::Vector2f>(this->_size + sf::Vector2u(2, 8)));
	decorator.setPosition(sf::Vector2f(-1, -7));
	unsigned char palette = 6;
	if (desktop.isFocused(this)) {
		palette = 7;
	}
	decorator.setOutlineColor(this->_palette[palette]);
	decorator.setFillColor(this->_palette[palette]);
	sf::Sprite sprite(this->_texture.getTexture());
	sprite.setTextureRect(sf::IntRect(0, this->_size.y, this->_size.x, -this->_size.y));

	target.draw(decorator, states);
	target.draw(sprite, states);

	if (this->_mrb->exc) {
		mrb_print_error(this->_mrb);
	}
}

sf::Vector2u Window::getSize() const
{
	return this->_size;
}

void Window::resize(sf::Vector2u size)
{
	this->_size.x = std::max(this->_minSize.x, size.x);
	this->_size.y = std::max(this->_minSize.y, size.y);

	this->_texture.create(this->_size.x, this->_size.y);
	// if (mrb_obj_respond_to(this->_mrb, this->_mrbWindowClass, mrb_intern_cstr(this->_mrb, "close_event"))) {
	// 	mrb_funcall(this->_mrb, mrb_nil_value(), "close_event", 0);
	// }
}

bool Window::isContext(mrb_state* mrb) const
{
	return this->_mrb == mrb;
}

bool Window::isIn(sf::Vector2i point) const
{
	return
		point.x >= this->getPosition().x &&
		point.x <= this->getPosition().x + static_cast<int>(this->_size.x) &&
		point.y >= this->getPosition().y &&
		point.y <= this->getPosition().y + static_cast<int>(this->_size.y);
}

void Window::setMousePosition(sf::Vector2f position)
{
	this->_mousePosition = sf::Vector2i(position - this->getPosition());
}

void Window::setLastKeypress(sf::Keyboard::Key key)
{
	this->_lastKey = key;
}

mrb_value Window::mrubyGetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_size.x);
}

mrb_value Window::mrubySetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_size.x = std::max(window->_minSize.x, static_cast<unsigned int>(newWidth));
	window->_texture.create(window->_size.x, window->_size.y);

	return mrb_nil_value();
}

mrb_value Window::mrubyGetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_size.y);
}

mrb_value Window::mrubySetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_size.y = std::max(window->_minSize.y, static_cast<unsigned int>(newHeight));
	window->_texture.create(window->_size.x, window->_size.y);

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_minSize.x);
}

mrb_value Window::mrubySetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_minSize.x = std::max(1u, static_cast<unsigned int>(newWidth));

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_minSize.y);
}

mrb_value Window::mrubySetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_minSize.y = std::max(1u, static_cast<unsigned int>(newHeight));

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_mousePosition.x);
}

mrb_value Window::mrubySetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newX;
	mrb_get_args(mrb, "i", &newX);
	window->_mousePosition.x = newX;

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_mousePosition.y);
}

mrb_value Window::mrubySetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newY;
	mrb_get_args(mrb, "i", &newY);
	window->_mousePosition.y = newY;

	return mrb_nil_value();
}

mrb_value Window::mrubyGetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_str_new(mrb, window->_title.c_str(), window->_title.length());
}

mrb_value Window::mrubySetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	const char* title;
	mrb_get_args(mrb, "z", &title);
	window->_title = std::string(title);

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

mrb_value Window::mrubyRectangle(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int x, y, w, h;
	mrb_int colorPalette = 1;
	mrb_get_args(mrb, "iiii|i", &x, &y, &w, &h, &colorPalette);

	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(sf::Vector2f(x, y));
	rectangle.setOutlineColor(window->_palette[colorPalette]);
	rectangle.setFillColor(window->_palette[colorPalette]);
	window->_texture.draw(rectangle);

	return mrb_nil_value();
}

mrb_value Window::mrubyCircle(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int x, y, radius;
	mrb_int colorPalette = 1;
	mrb_get_args(mrb, "iii|i", &x, &y, &radius, &colorPalette);

	sf::CircleShape circle(radius);
	circle.setPosition(sf::Vector2f(x, y));
	circle.setOutlineColor(window->_palette[colorPalette]);
	circle.setFillColor(window->_palette[colorPalette]);
	window->_texture.draw(circle);

	return mrb_nil_value();
}

mrb_value Window::mrubyKey(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	Window* window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();
	if (!desktop.isFocused(window)) return mrb_nil_value();

	mrb_int key = -1;
	mrb_get_args(mrb, "|i", &key);

	if (key != -1) {
		return mrb_bool_value(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key)));
	}

	return mrb_int_value(mrb, window->_lastKey);
}