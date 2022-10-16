#include <filesystem>
#include <Shared/AbstractWindow.hpp>
#include <Shared/helper.hh>
#include <Shared/sprites.hh>
#include <mruby/internal.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/error.h>

AbstractWindow::AbstractWindow(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, const std::string& programPath, const std::vector<std::string>& parameters) : _size(size), _title(programPath), _isFocused(false), _resizable(true), _closed(false), _programFile(programPath), _programParameters(parameters)
{
	this->setPosition(sf::Vector2f(position));
	this->_minSize = sf::Vector2i(8, 4);
	this->_palette = palette;
	this->_texture.create(this->_size.x, this->_size.y);
	this->_texture.clear(this->_palette[0]);
	this->_mrb = mrb_open();

	if (!this->_mrb) {
		throw new std::runtime_error("Unable to start mruby for window");
	}

	this->_mrbWindowClass = mrb_define_class(this->_mrb, "Window", this->_mrb->object_class);
	this->_mrbDesktopClass = mrb_define_class(this->_mrb, "Desktop", this->_mrb->object_class);

	mrb_define_const(this->_mrb, this->_mrbWindowClass, "POINTER", mrb_int_value(this->_mrb, reinterpret_cast<std::uintptr_t>(this)));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "width", &AbstractWindow::mrubyGetWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "width=", &AbstractWindow::mrubySetWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "height", &AbstractWindow::mrubyGetHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "height=", &AbstractWindow::mrubySetHeight, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_width", &AbstractWindow::mrubyGetMinWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_width=", &AbstractWindow::mrubySetMinWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_height", &AbstractWindow::mrubyGetMinHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_height=", &AbstractWindow::mrubySetMinHeight, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_x", &AbstractWindow::mrubyGetMouseX, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_x=", &AbstractWindow::mrubySetMouseX, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_y", &AbstractWindow::mrubyGetMouseY, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_y=", &AbstractWindow::mrubySetMouseY, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "title", &AbstractWindow::mrubyGetTitle, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "title=", &AbstractWindow::mrubySetTitle, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "resizable", &AbstractWindow::mrubyIsResizable, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "resizable=", &AbstractWindow::mrubySetResizable, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "reload", &AbstractWindow::mrubyReload, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "close", &AbstractWindow::mrubyClose, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "parameters", &AbstractWindow::mrubyParameters, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "key", &AbstractWindow::mrubyKey, MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "button", &AbstractWindow::mrubyButton, MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "focused", &AbstractWindow::mrubyFocused, MRB_ARGS_OPT(1));

	mrb_define_method(this->_mrb, this->_mrb->object_class, "clear", &AbstractWindow::mrubyClear, MRB_ARGS_REQ(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "pixel", &AbstractWindow::mrubyPixel, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &AbstractWindow::mrubyLine, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "rect", &AbstractWindow::mrubyRectangle, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "circle", &AbstractWindow::mrubyCircle, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "text", &AbstractWindow::mrubyText, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(2));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "sound", &AbstractWindow::mrubySound, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(2));

	this->loadFile();
}

AbstractWindow::~AbstractWindow()
{
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "close_event"))) {
		mrb_funcall(this->_mrb, obj, "close_event", 0);
	}
	mrbc_context_free(this->_mrb, this->_mrbContext);
	mrb_close(this->_mrb);
}

void AbstractWindow::loadFile()
{
	FILE *file = fopen((std::filesystem::current_path() / this->_programFile).c_str(), "r");
	if (file == nullptr) {
		throw std::runtime_error("The file '" + std::string(std::filesystem::current_path() / this->_programFile) + "' can't be opened");
	}
	this->_mrbContext = mrbc_context_new(this->_mrb);
	this->_mrbContext->capture_errors = true;
	mrbc_filename(this->_mrb, this->_mrbContext, this->_programFile.c_str());
	mrb_load_file_cxt(this->_mrb, file, this->_mrbContext);
	fclose(file);
}

void AbstractWindow::reloadFile()
{
	mrbc_context_free(this->_mrb, this->_mrbContext);
	this->loadFile();
	this->init();
}

void AbstractWindow::execute(const std::string& string)
{
	mrb_load_string(this->_mrb, string.c_str());
}

void AbstractWindow::init()
{
	if (mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "init"))) {
		mrb_funcall(this->_mrb, mrb_obj_value(this->_mrb->object_class), "init", 0);
	}
}

void AbstractWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (this->_mrb->exc == nullptr && mrb_obj_respond_to(this->_mrb, this->_mrb->object_class, mrb_intern_cstr(this->_mrb, "update"))) {
		mrb_funcall(this->_mrb, mrb_obj_value(this->_mrb->object_class), "update", 0);
	}

	states.transform *= getTransform();

	states.texture = NULL;

	sf::Sprite canvas(this->_texture.getTexture());
	canvas.setTextureRect(sf::IntRect(0, this->_size.y, this->_size.x, -this->_size.y));
	target.draw(canvas, states);
}

sf::Vector2i AbstractWindow::getSize() const
{
	return this->_size;
}

void AbstractWindow::exceptionHandler()
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
			for (unsigned int i = 0; i < backtraceLength - 1; ++i) {
				drawText(this->_texture, 1, (i + 1) * (FONT_HEIGHT + 1) + 1, mrb_str_to_cstr(this->_mrb, backtrace[i]) , this->_palette[5], false); // Palette 9 also render nice
			}
		}
	}
}

bool AbstractWindow::isClosed() const
{
	return this->_closed;
}

void AbstractWindow::close()
{
	this->_closed = true;
}

void AbstractWindow::resize(sf::Vector2i size)
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

void AbstractWindow::resizeH(int width)
{
	this->resize(sf::Vector2i(width, this->_size.y));
}

void AbstractWindow::resizeV(int height)
{
	this->resize(sf::Vector2i(this->_size.x, height));
}

void AbstractWindow::resizeTrigger()
{
	sf::Texture tmp(this->_texture.getTexture());
	this->_texture.create(this->_size.x, this->_size.y);
	this->_texture.clear(this->_palette[0]);
	sf::Sprite tmpSprite(tmp);
	tmpSprite.setTextureRect(sf::IntRect(0, tmp.getSize().y, tmp.getSize().x, -tmp.getSize().y));
	this->_texture.draw(tmpSprite);
}

void AbstractWindow::changeTitleTrigger()
{
}

void AbstractWindow::setMousePosition(sf::Vector2i position)
{
	this->_mousePosition = position - sf::Vector2i(this->getPosition());
}

void AbstractWindow::addKeyPressed(sf::Keyboard::Key key)
{
	if (std::find(this->_keyPressed.begin(), this->_keyPressed.end(), static_cast<sf::Keyboard::Key>(key)) == this->_keyPressed.end()) {
		this->_keyPressed.push_back(key);
	}
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "key_press_event"))) {
		mrb_funcall(this->_mrb, obj, "key_press_event", 1, mrb_int_value(this->_mrb, key));
	}
}

void AbstractWindow::removeKeyPressed(sf::Keyboard::Key key)
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

void AbstractWindow::addButtonPressed(sf::Mouse::Button button)
{
	if (std::find(this->_buttonPressed.begin(), this->_buttonPressed.end(), static_cast<sf::Mouse::Button>(button)) == this->_buttonPressed.end()) {
		this->_buttonPressed.push_back(button);
	}
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "button_press_event"))) {
		mrb_funcall(this->_mrb, obj, "button_press_event", 1, mrb_int_value(this->_mrb, button));
	}
}

void AbstractWindow::removeButtonPressed(sf::Mouse::Button button)
{
	if (std::find(this->_buttonPressed.begin(), this->_buttonPressed.end(), static_cast<sf::Mouse::Button>(button)) != this->_buttonPressed.end()) {
		this->_buttonPressed.erase(
			std::remove_if(this->_buttonPressed.begin(), this->_buttonPressed.end(), [button](sf::Mouse::Button testButton) { return testButton == button; } )
		);
	}
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "button_release_event"))) {
		mrb_funcall(this->_mrb, obj, "button_release_event", 1, mrb_int_value(this->_mrb, button));
	}
}

void AbstractWindow::textEnteredEvent(sf::Uint32 unicode)
{
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "text_event"))) {
		mrb_funcall(this->_mrb, obj, "text_event", 1, mrb_int_value(this->_mrb, unicode));
	}
}

void AbstractWindow::focusEvent(bool isFocused)
{
	this->_isFocused = isFocused;
	mrb_value obj = mrb_const_get(this->_mrb, mrb_obj_value(this->_mrb->object_class), mrb_intern_cstr(this->_mrb, "Window"));
	if (mrb_respond_to(this->_mrb, obj, mrb_intern_cstr(this->_mrb, "focus_event"))) {
		mrb_funcall(this->_mrb, obj, "focus_event", 1, mrb_bool_value(isFocused));
	}
}

// This method is dangerous, is will take the POINTER const in Window class, an get a pointer to the Window.
// If by any manner, the const is modified, then the entire app will crash with BAD_ACCESS
AbstractWindow* AbstractWindow::mrubyGetWindowObject(mrb_state *mrb)
{
	mrb_value windowObj = mrb_const_get(mrb, mrb_obj_value(mrb->object_class), mrb_intern_cstr(mrb, "Window"));
	mrb_int mrbPointer = mrb_as_int(mrb, mrb_const_get(mrb, windowObj, mrb_intern_cstr(mrb, "POINTER")));
	std::uintptr_t pointer = static_cast<std::uintptr_t>(mrbPointer);

	return reinterpret_cast<AbstractWindow*>(pointer);
}

mrb_value AbstractWindow::mrubyGetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_int_value(mrb, window->_size.x);
}

mrb_value AbstractWindow::mrubySetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_size.x = std::max(window->_minSize.x, static_cast<int>(newWidth));
	window->resizeTrigger();

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyGetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_int_value(mrb, window->_size.y);
}

mrb_value AbstractWindow::mrubySetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_size.y = std::max(window->_minSize.y, static_cast<int>(newHeight));
	window->resizeTrigger();

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyGetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_int_value(mrb, window->_minSize.x);
}

mrb_value AbstractWindow::mrubySetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int newWidth;
	mrb_get_args(mrb, "i", &newWidth);
	window->_minSize.x = std::max(1u, static_cast<unsigned int>(newWidth));

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyGetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_int_value(mrb, window->_minSize.y);
}

mrb_value AbstractWindow::mrubySetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int newHeight;
	mrb_get_args(mrb, "i", &newHeight);
	window->_minSize.y = std::max(1u, static_cast<unsigned int>(newHeight));

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyGetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_int_value(mrb, window->_mousePosition.x);
}

mrb_value AbstractWindow::mrubySetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int newX;
	mrb_get_args(mrb, "i", &newX);
	window->_mousePosition.x = newX;

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyGetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_int_value(mrb, window->_mousePosition.y);
}

mrb_value AbstractWindow::mrubySetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int newY;
	mrb_get_args(mrb, "i", &newY);
	window->_mousePosition.y = newY;

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyGetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	return mrb_str_new(mrb, window->_title.c_str(), window->_title.length());
}

mrb_value AbstractWindow::mrubySetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	const char* title;
	mrb_get_args(mrb, "z", &title);
	window->_title = std::string(title);
	window->changeTitleTrigger();

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyIsResizable(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_bool_value(window->_resizable);
}

mrb_value AbstractWindow::mrubySetResizable(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_bool resizable;
	mrb_get_args(mrb, "b", &resizable);
	window->_resizable = resizable;

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyReload(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	window->reloadFile();
	window->_keyPressed = {};

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyClose(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	window->_closed = true;

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyParameters(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_value parameters = mrb_ary_new_capa(mrb, window->_programParameters.size());
	for (const std::string& parameter : window->_programParameters) {
		mrb_ary_push(mrb, parameters, mrb_str_new(mrb, parameter.c_str(), parameter.length()));
	}

	return parameters;
}

mrb_value AbstractWindow::mrubyFocused([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);

	return mrb_bool_value(window->_isFocused);
}

mrb_value AbstractWindow::mrubySpawn([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_false_value();
}

mrb_value AbstractWindow::mrubyExport([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_false_value();
}

mrb_value AbstractWindow::mrubyProcesses([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyKillProcess([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_nil_value();
}

// Drawing

mrb_value AbstractWindow::mrubyClear(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int paletteIndex;
	mrb_get_args(mrb, "i", &paletteIndex);

	window->_texture.clear(window->_palette[paletteIndex]);

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyPixel(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
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

mrb_value AbstractWindow::mrubyLine(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
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

mrb_value AbstractWindow::mrubyRectangle(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
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

mrb_value AbstractWindow::mrubyCircle(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
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

mrb_value AbstractWindow::mrubyText(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	const char* text;
	mrb_int x, y;
	mrb_int colorPalette = 1;
	mrb_bool monospaces = false;
	mrb_get_args(mrb, "iiz|ib", &x, &y, &text, &colorPalette, &monospaces);
	drawText(window->_texture, x, y, std::string(text), window->_palette[colorPalette], monospaces);

	return mrb_nil_value();
}

mrb_value AbstractWindow::mrubyKey(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
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

mrb_value AbstractWindow::mrubyButton(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int button = -1;
	mrb_get_args(mrb, "|i", &button);

	if (button != -1) {
		return mrb_bool_value(std::find(window->_buttonPressed.begin(), window->_buttonPressed.end(), static_cast<sf::Mouse::Button>(button)) != window->_buttonPressed.end());
	}

	mrb_value array = mrb_ary_new_capa(mrb, window->_buttonPressed.size());
	for (const sf::Mouse::Button& button : window->_buttonPressed) {
		mrb_ary_push(mrb, array, mrb_int_value(mrb, button));
	}

	return array;
}

mrb_value AbstractWindow::mrubySound(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	AbstractWindow* window = mrubyGetWindowObject(mrb);
	mrb_int frequency = 440;
	std::vector<sf::Int16> samples;

	mrb_get_args(mrb, "|i", &frequency);

	for (unsigned int i = 0; i < 44100 / 10; i++) {
		samples.push_back(window->generateSineWave(i, frequency, 0.9));
	}
	window->_soundBuffer.loadFromSamples(&samples[0], samples.size(), 1, 44100);
	window->_sound.setBuffer(window->_soundBuffer);
	window->_sound.play();
	return mrb_nil_value();
}

#define TWOPI 6.283185307
short AbstractWindow::generateSineWave(double time, double frequency, double amplitude)
{
	double tickPerCycle = 44100 / frequency;
	double cycles = time / tickPerCycle;
	double rad = TWOPI * cycles;
	short amp = amplitude * 32767;
	return amp * sin(rad);
}

short AbstractWindow::generateSquareWave(double time, double frequency, double amplitude) {
	int tickPerCycle = 44100 / frequency;
	int cyclepart = static_cast<int>(time) % tickPerCycle;
	int halfcycle = tickPerCycle / 2;
	short amp = 32767 * amplitude;
	return (cyclepart < halfcycle) ? amp : 0;
}

short AbstractWindow::generateNoise(double amplitude) {
	short amp = 32767 * amplitude;
	return rand() % amp;
}