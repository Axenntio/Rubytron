#include <Editor/Desktop.hpp>
#include <Shared/helper.hh>
#include <Shared/sprites.hh>
#include <mruby/internal.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/error.h>

extern Desktop desktop;

Window::Window(sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, TitleBarMode titleBarMode, const std::string& programPath, const std::vector<std::string>& parameters) : AbstractWindow(position, size, palette, programPath, parameters), _titleBarMode(titleBarMode)
{
	this->titleBarRefresh();

	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "processes", &Window::mrubyProcesses, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "kill_process", &Window::mrubyKillProcess, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "spawn", &Window::mrubySpawn, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "export", &Window::mrubyExport, MRB_ARGS_REQ(1));
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
	if (this->_focused) {
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
		case WindowZone::Right:
			return
				(
					point.x == this->getPosition().x + static_cast<int>(this->_size.x) &&
					point.y >= this->getPosition().y &&
					point.y < this->getPosition().y + static_cast<int>(this->_size.y) - 2
				);
		case WindowZone::Bottom:
			return
				(
					point.y == this->getPosition().y + static_cast<int>(this->_size.y) &&
					point.x >= this->getPosition().x &&
					point.x < this->getPosition().x + static_cast<int>(this->_size.x) - 2
				);
	}
	return false;
}

void Window::toggleFullscreen()
{
	if (this->_fullscreened) {
		this->_fullscreened = false;
		this->setPosition(this->_prevPosition);
		this->resize(this->_prevSize);
	}
	else {
		this->_fullscreened = true;
		this->_prevSize = this->_size;
		this->_prevPosition = this->getPosition();
		this->setPosition(sf::Vector2f(0, 0));
		this->resize(static_cast<sf::Vector2i>(desktop.getSize()));
	}
}

void Window::addKeyPressed(sf::Keyboard::Key key)
{
	if (this->_mrb->exc) {
		this->_closed = true;
		desktop.spawn(static_cast<sf::Vector2i>(this->getPosition()), static_cast<sf::Vector2u>(this->_size), this->_programFile, this->_programParameters);
		return;
	}
	AbstractWindow::addKeyPressed(key);
}

void Window::focusEvent(bool focused)
{
	this->_focused = focused;
	this->titleBarRefresh();
	AbstractWindow::focusEvent(focused);
}

void Window::resizeTrigger()
{
	AbstractWindow::resizeTrigger();
	this->titleBarRefresh();
}

void Window::changeTitleTrigger()
{
	this->titleBarRefresh();
	AbstractWindow::changeTitleTrigger();
}

void Window::titleBarRefresh()
{
	if (this->_titleBarMode == TitleBarMode::None) {
		return;
	}
	unsigned char height = (this->_titleBarMode == TitleBarMode::Minimal) ? 3 : 6;
	unsigned char palette = 6;
	if (this->_focused) {
		palette = 0;
	}
	this->_barTexture.create(this->_size.x, height);
	this->_barTexture.clear(sf::Color::Transparent);
	if (this->_titleBarMode == TitleBarMode::Full) {
		drawText(this->_barTexture, 0, 0, this->_title, this->_palette[palette], false);
		drawOnTexture(this->_barTexture, this->_size.x - 11, 0, spr_maximise_full, SPR_MAXIMISE_FULL_HEIGHT, this->_palette[palette]);
		drawOnTexture(this->_barTexture, this->_size.x - 5, 0, spr_close_full, SPR_CLOSE_FULL_HEIGHT, this->_palette[palette]);
	}
	if (this->_titleBarMode == TitleBarMode::Minimal) {
		drawOnTexture(this->_barTexture, this->_size.x - 3, 0, spr_close_minimal, SPR_CLOSE_MINIMAL_HEIGHT, this->_palette[palette]);
		drawOnTexture(this->_barTexture, this->_size.x - 7, 0, spr_maximise_minimal, SPR_MAXIMISE_MINIMAL_HEIGHT, this->_palette[palette]);
	}
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

mrb_value Window::mrubyExport(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	const char* path;
	mrb_get_args(mrb, "z", &path);

	return mrb_bool_value(desktop.programExport(std::string(path)));
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