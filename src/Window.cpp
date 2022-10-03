#include <Desktop.hpp>
#include <helper.hh>
#include <sprites.hh>
#include <mruby/internal.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/error.h>

extern Desktop desktop;

Window::Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, TitleBarMode titleBarMode, const std::string& programPath, const std::vector<std::string>& parameters) : _size(size), _title(programPath), _titleBarMode(titleBarMode), _resizable(true), _closed(false), _programFile(programPath), _programParameters(parameters)
{
	this->setPosition(sf::Vector2f(position));
	this->_minSize = sf::Vector2i(8, 4);
	this->_palette = palette;
	this->_texture.create(this->_size.x, this->_size.y);
	this->_texture.clear(this->_palette[0]);
	this->titleBarRefresh();
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
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "resizable", &Window::mrubyIsResizable, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "resizable=", &Window::mrubySetResizable, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "reload", &Window::mrubyReload, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "close", &Window::mrubyClose, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "parameters", &Window::mrubyParameters, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "key", &Window::mrubyKey, MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "focused", &Window::mrubyFocused, MRB_ARGS_NONE());


	this->_mrbDesktopClass = mrb_define_class(this->_mrb, "Desktop", this->_mrb->object_class);
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "processes", &Window::mrubyProcesses, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "kill_process", &Window::mrubyKillProcess, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "spawn", &Window::mrubySpawn, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));


	mrb_define_method(this->_mrb, this->_mrb->object_class, "clear", &Window::mrubyClear, MRB_ARGS_REQ(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "pixel", &Window::mrubyPixel, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &Window::mrubyLine, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "rect", &Window::mrubyRectangle, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "circle", &Window::mrubyCircle, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "text", &Window::mrubyText, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(2));

	this->loadFile();
}

Window::~Window()
{
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "close_event"))) {
		mrb_funcall(this->_mrb, obj, "close_event", 0);
	}
	mrbc_context_free(this->_mrb, this->_mrbContext);
	mrb_close(this->_mrb);
}

void Window::loadFile()
{
	FILE *file = fopen(this->_programFile.c_str(), "r");
	if (file == nullptr) {
		throw std::runtime_error("The file '" + this->_programFile + "' can't be opened");
	}
	this->_mrbContext = mrbc_context_new(this->_mrb);
	this->_mrbContext->capture_errors = true;
	mrbc_filename(this->_mrb, this->_mrbContext, this->_programFile.c_str());
	mrb_load_file_cxt(this->_mrb, file, this->_mrbContext);
	fclose(file);
}

void Window::reloadFile()
{
	mrbc_context_free(this->_mrb, this->_mrbContext);
	this->loadFile();
	this->init();
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
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (this->_mrb->exc == nullptr && mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "update"))) {
		mrb_funcall(this->_mrb, mrb_nil_value(), "update", 0);
	}

	states.transform *= getTransform();

	states.texture = NULL;

	unsigned char height = 8;
	if (this->_titleBarMode == TitleBarMode::Minimal) {
		height = 5;
	}
	if (this->_titleBarMode == TitleBarMode::None) {
		height = 1;
	}

	sf::RectangleShape decorator(static_cast<sf::Vector2f>(this->_size + sf::Vector2i(2, height + 1)));
	decorator.setPosition(sf::Vector2f(-1, -height));
	unsigned char palette = 5;
	if (desktop.isFocused(this)) {
		palette = 6;
	}
	decorator.setOutlineColor(this->_palette[palette]);
	decorator.setFillColor(this->_palette[palette]);
	target.draw(decorator, states);

	if (this->_titleBarMode != TitleBarMode::None) {
		sf::Sprite titleBar(this->_barTexture.getTexture());
		titleBar.setTextureRect(sf::IntRect(0, (height - 2), this->_size.x, -(height - 2)));
		titleBar.setPosition(sf::Vector2f(0, -(height - 1)));
		target.draw(titleBar, states);
	}

	sf::Sprite canvas(this->_texture.getTexture());
	canvas.setTextureRect(sf::IntRect(0, this->_size.y, this->_size.x, -this->_size.y));
	target.draw(canvas, states);
}

sf::Vector2i Window::getSize() const
{
	return this->_size;
}

void Window::exceptionHandler()
{
	if (this->_mrb->exc) {
		this->_texture.clear(this->_palette[0]);
		this->_resizable = true;
		this->_minSize = sf::Vector2i(8, 4);
		mrb_value exception = mrb_obj_as_string(this->_mrb, mrb_obj_value(this->_mrb->exc));
		drawText(this->_texture, 1, 1, mrb_str_to_cstr(this->_mrb, exception), this->_palette[8], false);

		mrb_value backtraceObj = mrb_funcall(this->_mrb, mrb_obj_value(this->_mrb->exc), "backtrace", 0);
		if (mrb_array_p(backtraceObj)) {
			mrb_int backtraceLength = RARRAY_LEN(backtraceObj);
			mrb_value *backtrace = RARRAY_PTR(backtraceObj);
			for (unsigned int i = 0; i < backtraceLength; ++i) {
				drawText(this->_texture, 1, (i + 1) * (FONT_HEIGHT + 1) + 1, mrb_str_to_cstr(this->_mrb, backtrace[i]) , this->_palette[8], false);
			}
		}
	}
}

bool Window::isClosed() const
{
	return this->_closed;
}

void Window::close()
{
	this->_closed = true;
}

void Window::resize(sf::Vector2i size)
{
	if (!this->_resizable) {
		return;
	}
	this->_size.x = std::max(this->_minSize.x, size.x);
	this->_size.y = std::max(this->_minSize.y, size.y);

	this->resizeTrigger();

	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "resize_event"))) {
		mrb_funcall(this->_mrb, obj, "resize_event", 2, mrb_int_value(this->_mrb, this->_size.x), mrb_int_value(this->_mrb, this->_size.y));
	}
}

void Window::resizeTrigger()
{
	sf::Texture tmp(this->_texture.getTexture());
	this->_texture.create(this->_size.x, this->_size.y);
	this->_texture.clear(this->_palette[0]);
	sf::Sprite tmpSprite(tmp);
	tmpSprite.setTextureRect(sf::IntRect(0, tmp.getSize().y, tmp.getSize().x, -tmp.getSize().y));
	this->_texture.draw(tmpSprite);

	this->titleBarRefresh();
}

void Window::titleBarRefresh()
{
	if (this->_titleBarMode == TitleBarMode::None) {
		return;
	}
	unsigned char height = (this->_titleBarMode == TitleBarMode::Minimal) ? 3 : 6;
	unsigned char palette = 6;
	if (desktop.isFocused(this)) {
		palette = 0;
	}
	this->_barTexture.create(this->_size.x, height);
	this->_barTexture.clear(sf::Color::Transparent);
	if (this->_titleBarMode == TitleBarMode::Full) {
		drawText(this->_barTexture, 0, 0, this->_title, this->_palette[palette], false);
		drawOnTexture(this->_barTexture, this->_size.x - 5, 0, spr_close_full, SPR_CLOSE_FULL_HEIGHT, this->_palette[palette]);
	}
	if (this->_titleBarMode == TitleBarMode::Minimal) {
		drawOnTexture(this->_barTexture, this->_size.x - 3, 0, spr_close_minimal, SPR_CLOSE_MINIMAL_HEIGHT, this->_palette[palette]);
	}
}

bool Window::isContext(mrb_state* mrb) const
{
	return this->_mrb == mrb;
}

bool Window::isIn(WindowZone zone, sf::Vector2i point) const
{
	unsigned char height = 8;
	if (this->_titleBarMode == TitleBarMode::Minimal) {
		height = 5;
	}
	if (this->_titleBarMode == TitleBarMode::None) {
		height = 1;
	}
	switch (zone) {
		case WindowZone::All:
			return
				point.x >= this->getPosition().x - 1 &&
				point.x < this->getPosition().x + static_cast<int>(this->_size.x) + 1 &&
				point.y >= this->getPosition().y - 8 &&
				point.y < this->getPosition().y + static_cast<int>(this->_size.y) + 1;
		case WindowZone::TitleBar:
			return
				point.x >= this->getPosition().x - 1 &&
				point.x < this->getPosition().x + static_cast<int>(this->_size.x) + 1 &&
				point.y >= this->getPosition().y - height &&
				point.y < this->getPosition().y;
		case WindowZone::Canvas:
			return
				point.x >= this->getPosition().x &&
				point.x < this->getPosition().x + static_cast<int>(this->_size.x) &&
				point.y >= this->getPosition().y &&
				point.y < this->getPosition().y + static_cast<int>(this->_size.y);
		case WindowZone::BottomRight:
			return
				(
					point.x == this->getPosition().x + static_cast<int>(this->_size.x) &&
					point.y >= this->getPosition().y + static_cast<int>(this->_size.y) - 2 &&
					point.y < this->getPosition().y + static_cast<int>(this->_size.y) + 1
				) || (
					point.y == this->getPosition().y + static_cast<int>(this->_size.y) &&
					point.x >= this->getPosition().x + static_cast<int>(this->_size.x) - 2 &&
					point.x < this->getPosition().x + static_cast<int>(this->_size.x) + 1
				);
	}
	return false;
}

void Window::setMousePosition(sf::Vector2i position)
{
	this->_mousePosition = position - sf::Vector2i(this->getPosition());
}

void Window::addKeyPressed(sf::Keyboard::Key key)
{
	if (this->_mrb->exc) {
		this->_closed = true;
		desktop.spawn(static_cast<sf::Vector2i>(this->getPosition()), static_cast<sf::Vector2u>(this->_size), this->_programFile, this->_programParameters);
		return;
	}
	if (std::find(this->_keyPressed.begin(), this->_keyPressed.end(), static_cast<sf::Keyboard::Key>(key)) == this->_keyPressed.end()) {
		this->_keyPressed.push_back(key);
	}
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "key_press_event"))) {
		mrb_funcall(this->_mrb, obj, "key_press_event", 1, mrb_int_value(this->_mrb, key));
	}
}

void Window::removeKeyPressed(sf::Keyboard::Key key)
{
	if (std::find(this->_keyPressed.begin(), this->_keyPressed.end(), static_cast<sf::Keyboard::Key>(key)) != this->_keyPressed.end()) {
		this->_keyPressed.erase(
			std::remove_if(this->_keyPressed.begin(), this->_keyPressed.end(), [key](sf::Keyboard::Key testKey) { return testKey == key; } )
		);
	}
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "key_release_event"))) {
		mrb_funcall(this->_mrb, obj, "key_release_event", 1, mrb_int_value(this->_mrb, key));
	}
}

void Window::textEnteredEvent(sf::Uint32 unicode)
{
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "text_event"))) {
		mrb_funcall(this->_mrb, obj, "text_event", 1, mrb_int_value(this->_mrb, unicode));
	}
}

void Window::focusEvent(bool isFocused)
{
	this->titleBarRefresh();
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "focus_event"))) {
		mrb_funcall(this->_mrb, obj, "focus_event", 1, mrb_bool_value(isFocused));
	}
}

mrb_value Window::mrubyGetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_size.x);
}

mrb_value Window::mrubySetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_size.x = std::max(window->_minSize.x, static_cast<int>(newWidth));
	window->resizeTrigger();

	return mrb_nil_value();
}

mrb_value Window::mrubyGetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_size.y);
}

mrb_value Window::mrubySetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_size.y = std::max(window->_minSize.y, static_cast<int>(newHeight));
	window->resizeTrigger();

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_minSize.x);
}

mrb_value Window::mrubySetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_minSize.x = std::max(1u, static_cast<unsigned int>(newWidth));

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_minSize.y);
}

mrb_value Window::mrubySetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_minSize.y = std::max(1u, static_cast<unsigned int>(newHeight));

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_mousePosition.x);
}

mrb_value Window::mrubySetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newX;
	mrb_get_args(mrb, "i", &newX);
	window->_mousePosition.x = newX;

	return mrb_nil_value();
}

mrb_value Window::mrubyGetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_int_value(mrb, window->_mousePosition.y);
}

mrb_value Window::mrubySetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int newY;
	mrb_get_args(mrb, "i", &newY);
	window->_mousePosition.y = newY;

	return mrb_nil_value();
}

mrb_value Window::mrubyGetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_str_new(mrb, window->_title.c_str(), window->_title.length());
}

mrb_value Window::mrubySetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	const char* title;
	mrb_get_args(mrb, "z", &title);
	window->_title = std::string(title);
	window->titleBarRefresh();

	return mrb_nil_value();
}

mrb_value Window::mrubyIsResizable(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_bool_value(window->_resizable);
}

mrb_value Window::mrubySetResizable(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_bool resizable;
	mrb_get_args(mrb, "b", &resizable);
	window->_resizable = resizable;

	return mrb_nil_value();
}

mrb_value Window::mrubyReload(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	window->reloadFile();
	window->_keyPressed = {};

	return mrb_nil_value();
}

mrb_value Window::mrubyClose(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	window->_closed = true;

	return mrb_nil_value();
}

mrb_value Window::mrubyParameters(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_value parameters = mrb_ary_new_capa(mrb, window->_programParameters.size());
	for (const std::string& parameter : window->_programParameters) {
		mrb_ary_push(mrb, parameters, mrb_str_new(mrb, parameter.c_str(), parameter.length()));
	}

	return parameters;
}

mrb_value Window::mrubyFocused(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	return mrb_bool_value(desktop.isFocused(window.get()));
}


mrb_value Window::mrubySpawn(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	const char* path;
	mrb_value mrbParameters = mrb_nil_value();
	mrb_get_args(mrb, "z|A", &path, &mrbParameters);
	std::vector<std::string> parameters;

	if (mrb_array_p(mrbParameters)) {
		mrb_int parameterLength = RARRAY_LEN(mrbParameters);
		mrb_value *parameter = RARRAY_PTR(mrbParameters);
		for (unsigned int i = 0; i < parameterLength; ++i) {
			parameters.push_back(mrb_str_to_cstr(mrb, parameter[i]));
		}
	}

	if (desktop.spawn(std::string(path), parameters)) {
		return mrb_true_value();
	}

	return mrb_false_value();
}

mrb_value Window::mrubyProcesses(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	mrb_value array = mrb_ary_new_capa(mrb, desktop.getWindows().size());
	for (const std::shared_ptr<Window>& window : desktop.getWindows()) { // TODO: Do index for kill command
		mrb_ary_push(mrb, array, mrb_str_new(mrb, window->_programFile.c_str(), window->_programFile.length()));
	}

	return array;
}

mrb_value Window::mrubyKillProcess(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	mrb_int processId;
	mrb_get_args(mrb, "i", &processId);
	return mrb_bool_value(desktop.killWindow(processId));
}

// Drawing

mrb_value Window::mrubyClear(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();


	mrb_int paletteIndex;
	mrb_get_args(mrb, "i", &paletteIndex);

	window->_texture.clear(window->_palette[paletteIndex]);

	return mrb_nil_value();
}

mrb_value Window::mrubyPixel(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int x, y;
	mrb_int colorPalette = 1;
	mrb_get_args(mrb, "ii|i", &x, &y, &colorPalette);

	sf::RectangleShape pixel(sf::Vector2f(1, 1));
	pixel.setPosition(sf::Vector2f(x, y));
	pixel.setOutlineColor(window->_palette[colorPalette]);
	pixel.setFillColor(window->_palette[colorPalette]);
	window->_texture.draw(pixel);

	return mrb_nil_value();
}

mrb_value Window::mrubyLine(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
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
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
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
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
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

mrb_value Window::mrubyText(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	const char* text;
	mrb_int x, y;
	mrb_int colorPalette = 1;
	mrb_bool monospaces = false;
	mrb_get_args(mrb, "iiz|ib", &x, &y, &text, &colorPalette, &monospaces);
	drawText(window->_texture, x, y, std::string(text), window->_palette[colorPalette], monospaces);

	return mrb_nil_value();
}

mrb_value Window::mrubyKey(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	std::shared_ptr<Window> window = desktop.getWindow(mrb);
	if (window == nullptr) return mrb_nil_value();

	mrb_int key = -1;
	mrb_get_args(mrb, "|i", &key);

	if (key != -1) {
		return mrb_bool_value(std::find(window->_keyPressed.begin(), window->_keyPressed.end(), static_cast<sf::Keyboard::Key>(key)) != window->_keyPressed.end());
	}

	mrb_value array = mrb_ary_new_capa(mrb, window->_keyPressed.size());
	for (const sf::Keyboard::Key& key : window->_keyPressed) {
		mrb_ary_push(mrb, array, mrb_int_value(mrb, key));
	}

	return array;
}