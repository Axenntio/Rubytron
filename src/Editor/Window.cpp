#include <Editor/Desktop.hpp>
#include <Shared/helper.hh>
#include <Shared/sprites.hh>
#include <mruby/internal.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/error.h>

extern Desktop desktop;

Window::Window(unsigned int pid, sf::Vector2i position, sf::Vector2u size, const std::vector<sf::Color>& palette, TitleBarMode titleBarMode, const std::string& programPath, const std::vector<std::string>& parameters) : AbstractWindow(position, size, palette, programPath, parameters), _pid(pid), _titleBarMode(titleBarMode)
{
	this->titleBarRefresh();

	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "kill_process", &Window::mrubyKillProcess, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "spawn", &Window::mrubySpawn, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1) | MRB_ARGS_KEY(2, 0));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "folder", &Window::mrubyFolder, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "export", &Window::mrubyExport, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "close", &Window::mrubyDesktopClose, MRB_ARGS_NONE());
}

void Window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
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
		titleBar.setPosition(sf::Vector2f(0, -(height - 1)));
		target.draw(titleBar, states);
	}

	sf::Sprite canvas(this->_textureBuffer.getTexture());
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

void Window::maximize()
{
	unsigned char height = 8;
	if (this->_titleBarMode == TitleBarMode::Minimal) {
		height = 5;
	}
	if (this->_titleBarMode == TitleBarMode::None) {
		height = 1;
	}

	this->setPosition(sf::Vector2f(1, height));
	this->resize(static_cast<sf::Vector2i>(desktop.getSize() - sf::Vector2u(2, height + 1)));
}

void Window::toggleFullscreen()
{
	if (this->_fullscreened) {
		this->_fullscreened = false;
		this->setPosition(static_cast<sf::Vector2f>(this->_prevPosition));
		this->resize(static_cast<sf::Vector2i>(this->_prevSize));
	}
	else {
		this->_fullscreened = true;
		this->_prevSize = static_cast<sf::Vector2u>(this->_size);
		this->_prevPosition = static_cast<sf::Vector2i>(this->getPosition());
		this->setPosition(sf::Vector2f(0, 0));
		this->resize(static_cast<sf::Vector2i>(desktop.getSize()));
	}
}

unsigned int Window::getPid() const
{
	return this->_pid;
}

void Window::addKeyPressed(sf::Keyboard::Key key)
{
	if (this->_mrb->exc) {
		this->_mrb->exc = NULL;
		this->reloadFile();
		return;
	}
	AbstractWindow::addKeyPressed(key);
}

void Window::focusEvent(bool focused)
{
	AbstractWindow::focusEvent(focused);
	this->titleBarRefresh();
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
	std::lock_guard<std::mutex> drawLock(this->_drawMutex);
	if (this->_titleBarMode == TitleBarMode::None) {
		return;
	}
	unsigned char height = (this->_titleBarMode == TitleBarMode::Minimal) ? 3 : 6;
	unsigned char palette = 6;
	if (this->_focused) {
		palette = 0;
	}
	if (this->_titleBarMode == TitleBarMode::Full) {
		if (!this->_barTexture.resize(sf::Vector2u(std::max(1, this->_size.x - 12), height))) {
			throw std::runtime_error("Can't resize texture");
		}
		this->_barTexture.clear(sf::Color::Transparent);
		if (this->_size.x - 12 > 0) {
			drawText(this->_barTexture, 0, 0, this->_title, this->_palette[palette], false);
			this->_barTexture.display();
		}
		sf::Texture tmp(this->_barTexture.getTexture());
		if (!this->_barTexture.resize(sf::Vector2u(this->_size.x, height))) {
			throw std::runtime_error("Can't resize texture");
		}
		this->_barTexture.clear(sf::Color::Transparent);
		sf::Sprite tmpSprite(tmp);
		this->_barTexture.draw(tmpSprite);
		drawOnTexture(this->_barTexture, this->_size.x - 11, 0, spr_maximise_full, SPR_MAXIMISE_FULL_HEIGHT, this->_palette[palette]);
		drawOnTexture(this->_barTexture, this->_size.x - 5, 0, spr_close_full, SPR_CLOSE_FULL_HEIGHT, this->_palette[palette]);
	}
	if (this->_titleBarMode == TitleBarMode::Minimal) {
		if (!this->_barTexture.resize(sf::Vector2u(this->_size.x, height))) {
			throw std::runtime_error("Can't resize texture");
		}
		this->_barTexture.clear(sf::Color::Transparent);
		drawOnTexture(this->_barTexture, this->_size.x - 3, 0, spr_close_minimal, SPR_CLOSE_MINIMAL_HEIGHT, this->_palette[palette]);
		drawOnTexture(this->_barTexture, this->_size.x - 7, 0, spr_maximise_minimal, SPR_MAXIMISE_MINIMAL_HEIGHT, this->_palette[palette]);
	}
	this->_barTexture.display();
}

mrb_value Window::mrubyDesktopClose([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	desktop.close();

	return mrb_true_value();
}

mrb_value Window::mrubySpawn(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	const char* path;
	mrb_value mrbParameters = mrb_nil_value();
	mrb_value kw_rest = mrb_nil_value();
	mrb_sym kw_names[] = { mrb_intern_lit(mrb, "position"), mrb_intern_lit(mrb, "size") };
	mrb_value kw_values[2];
	mrb_kwargs kwargs = { 2, 0, kw_names, kw_values, &kw_rest };
	mrb_get_args(mrb, "z|A:", &path, &mrbParameters, &kwargs);
	std::vector<std::string> parameters;

	if (mrb_array_p(mrbParameters)) {
		mrb_int parameterLength = RARRAY_LEN(mrbParameters);
		mrb_value *parameter = RARRAY_PTR(mrbParameters);
		for (unsigned int i = 0; i < parameterLength; ++i) {
			parameters.push_back(mrb_str_to_cstr(mrb, parameter[i]));
		}
	}

	sf::Vector2i position(0, 0);
	sf::Vector2u size(0, 0);

	if (mrb_array_p(kw_values[0]) && mrb_array_p(kw_values[1])) {
		mrb_value *mrbPosition = RARRAY_PTR(kw_values[0]);
		position = sf::Vector2i(mrb_int(mrb, mrbPosition[0]), mrb_int(mrb, mrbPosition[1]));
		mrb_value *mrbSize = RARRAY_PTR(kw_values[1]);
		size = sf::Vector2u(mrb_int(mrb, mrbSize[0]), mrb_int(mrb, mrbSize[1]));
	}

	if (position == sf::Vector2i(0, 0) || size == sf::Vector2u(0, 0)) {
		if (desktop.spawn(std::string(path), parameters)) {
			return mrb_true_value();
		}
	}
	else {
		if (desktop.spawn(position, size, std::string(path), parameters)) {
			return mrb_true_value();
		}
	}

	return mrb_false_value();
}

mrb_value Window::mrubyFolder([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	#ifdef _WIN32
		const std::wstring path = std::filesystem::current_path();
	#else
		const std::string path = std::filesystem::current_path();
	#endif
	#ifdef _WIN32
		int result = _wsystem((L"explorer \"" + path + L"\"").c_str());
	#elif __APPLE__
		int result = std::system(("open \"" + path + "\"").c_str());
	#elif __linux__
		int result = std::system(("xdg-open \"" + path + "\"").c_str());
	#else
		throw std::runtime_error("Unsupported OS");
		return;
	#endif


	if (result != 0) {
		throw std::runtime_error("Failed to open folder. Exit code: " + std::to_string(result));
	}

	return mrb_true_value();
}

mrb_value Window::mrubyExport(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	const char* path;
	mrb_get_args(mrb, "z", &path);

	return mrb_bool_value(desktop.programExport(std::string(path)));
}

mrb_value Window::mrubyProcesses(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	mrb_value hash = mrb_hash_new_capa(mrb, desktop.getWindows().size());
	for (const std::shared_ptr<Window>& window : desktop.getWindows()) {
		mrb_hash_set(mrb, hash, mrb_int_value(mrb, window->getPid()), mrb_str_new(mrb, window->_programFile.c_str(), window->_programFile.length()));
	}

	return hash;
}

mrb_value Window::mrubyKillProcess(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	mrb_int processId;
	mrb_get_args(mrb, "i", &processId);
	return mrb_bool_value(desktop.killWindow(processId));
}
