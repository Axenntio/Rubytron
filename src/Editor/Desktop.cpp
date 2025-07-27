#include <algorithm>
#include <filesystem>
#include <fstream>
#include <Editor/Desktop.hpp>
#include <Shared/helper.hh>
#include <Shared/sprites.hh>

Desktop::Desktop(unsigned int width, unsigned int height, unsigned char scale, TitleBarMode titleBarMode) : _size(sf::Vector2u(width, height)), _titleBarMode(titleBarMode), _windowsPid(0)
{
	this->_palette = std::vector<sf::Color> {
		sf::Color(  0,   0,   0), // #000000
		sf::Color( 29,  43,  83), // #1D2B53
		sf::Color(126,  37,  83), // #7e2553
		sf::Color(  0, 135,  81), // #008751
		sf::Color(171,  82,  54), // #AB5236
		sf::Color( 95,  87,  79), // #5F574F
		sf::Color(194, 195, 199), // #C2C3C7
		sf::Color(255, 241, 232), // #FFF1E8
		sf::Color(255,   0,  77), // #FF004D
		sf::Color(255, 163,   0), // #FFA300
		sf::Color(255, 236,  39), // #FFEC27
		sf::Color(  0, 228,  54), // #00E436
		sf::Color( 41, 173, 255), // #29ADFF
		sf::Color(131, 118, 156), // #83769C
		sf::Color(255, 119, 168), // #FF77A8
		sf::Color(255, 204, 170)  // #FFCCAA
	};

	this->_window.create(sf::VideoMode(sf::Vector2u(this->_size.x * scale, this->_size.y * scale)), "Rubytron");
	this->_window.setMinimumSize(sf::Vector2u(this->_size.x, this->_size.y));
	this->_window.setMouseCursorVisible(false);
	if (!this->_background_texture.resize(this->_size)) {
		throw std::runtime_error("Can't resize texture");
	}
	this->_background_texture.clear(this->_palette[1]);
	if (!this->_foreground_texture.resize(this->_size)) {
		throw std::runtime_error("Can't resize texture");
	}
	this->_foreground_texture.clear(sf::Color::Transparent);
	if (!this->_cursor_texture.resize({4, 6})) {
		throw std::runtime_error("Can't resize texture");
	}
	this->_cursor_texture.clear(sf::Color::Transparent);

	drawOnTexture(this->_cursor_texture, 0, 0, spr_cursor, SPR_CURSOR_HEIGHT, this->_palette[7]);

	this->_canvas_view.setCenter(sf::Vector2f(this->_size.x / 2, this->_size.y / 2));
	this->_canvas_view.setViewport(sf::FloatRect({0, 0}, {1, 1}));
	this->_canvas_view.setSize(sf::Vector2f(this->_size.x, this->_size.y));
	this->_window.setView(this->_canvas_view);
}

void Desktop::run()
{
	this->spawn("autorun.rb");
	if (this->_focusedWindow != nullptr) {
		this->_focusedWindow->focusEvent(false);
	}
	this->_focusedWindow = nullptr;
	this->_focusAction = FocusAction::None;

	while (this->_window.isOpen())
	{
		while (const std::optional event = this->_window.pollEvent())
		{
			if (const auto* eventData = event->getIf<sf::Event::Closed>()) {
				this->closeEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::Resized>()) {
				this->resizeEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::MouseButtonPressed>()) {
				this->mouseButtonPressEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::MouseButtonReleased>()) {
				this->mouseButtonReleaseEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::MouseMoved>()) {
				this->mouseMoveEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::KeyPressed>()) {
				this->keyPressEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::KeyReleased>()) {
				this->keyReleaseEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::TextEntered>()) {
				this->textEvent(eventData);
			}
			if (const auto* eventData = event->getIf<sf::Event::MouseWheelScrolled>()) {
				this->mouseWheelEvent(eventData);
			}
		}

		for (const std::shared_ptr<Window>& window : this->_windows) {
			if (window->isClosed() && this->_focusedWindow == window) {
				this->_focusedWindow = nullptr;
			}
		}
		this->_windows.erase(
			std::remove_if(this->_windows.begin(), this->_windows.end(), [](const std::shared_ptr<Window> window) { return window->isClosed(); }),
			this->_windows.end()
		);
		this->_window.setVerticalSyncEnabled(true);
		this->_window.clear();
		sf::Sprite background(this->_background_texture.getTexture());
		sf::Sprite foreground(this->_foreground_texture.getTexture());
		sf::Sprite cursor(this->_cursor_texture.getTexture());
		cursor.setTextureRect(sf::IntRect({0, 6}, {4, -6}));
		cursor.setPosition(sf::Vector2f(this->_mouse_coordinated));
		this->_window.draw(background);
		for (const std::shared_ptr<Window>& window : this->_windows) {
			window->exceptionHandler();
			this->_window.draw(*window);
			window->resetClock();
		}
		this->_window.draw(foreground);
		this->_window.draw(cursor);
		this->_window.display();
	}
}

std::shared_ptr<Window> Desktop::getWindow(mrb_state* mrb) const
{
	for (const std::shared_ptr<Window>& window : this->_windows) {
		if (window->isContext(mrb)) {
			return window;
		}
	}
	return nullptr;
}

sf::Vector2u Desktop::getSize() const
{
	return this->_size;
}

bool Desktop::isFocused(const Window* window) const
{
	return this->_focusedWindow.get() == window;
}

#if defined(__APPLE__) || defined(MULTI_EXPORT)
	#include <runtime-darwin.h>
#endif
#if defined(__linux__) || defined(MULTI_EXPORT)
	#include <runtime-linux.h>
#endif
struct arch_t {
	std::string name;
	unsigned char* file;
	unsigned int length;
};

bool Desktop::programExport(const std::string& path) const
{
	std::vector<arch_t> archs = {
#if defined(__APPLE__) || defined(MULTI_EXPORT)
		{"darwin", src_Runtime_runtime_darwin, src_Runtime_runtime_darwin_len},
#endif
#if defined(__linux__) || defined(MULTI_EXPORT)
		{"linux", src_Runtime_runtime_linux, src_Runtime_runtime_linux_len},
#endif
	};
	std::filesystem::create_directory(path + ".bin");
	for (const arch_t& arch : archs) {
		// Expose runtime
		std::filesystem::create_directory(path + ".bin/" + arch.name);
		std::ofstream runtimeFile(path + ".bin/" + arch.name + "/runtime", std::ios::binary);
		if (!runtimeFile.is_open()) {
			return false;
		}
		runtimeFile.write(reinterpret_cast<char*>(arch.file), arch.length);
		runtimeFile.close();
		std::filesystem::permissions(path + ".bin/" + arch.name + "/runtime",
			std::filesystem::perms::owner_exec,
			std::filesystem::perm_options::add
		);

		// Output file
		std::ifstream programFileContent(path, std::ios::binary);
		std::ofstream programFile(path + ".bin/" + arch.name + "/program.rb");
		if (!programFile.is_open()) {
			return false;
		}
		programFile << programFileContent.rdbuf();
		programFile.close();
		programFileContent.close();
	}
	return true;
}

bool Desktop::spawn(const std::string& path, const std::vector<std::string>& parameters)
{
	return this->spawn(sf::Vector2i(10, 10), sf::Vector2u(60, 30), path, parameters);
}

bool Desktop::spawn(sf::Vector2i position, sf::Vector2u size, const std::string& path, const std::vector<std::string>& parameters)
{
	try {
		std::shared_ptr<Window> window = std::make_unique<Window>(this->_windowsPid, position, size, this->_palette, this->_titleBarMode, path, parameters);
		this->_windows.push_back(window);
		window->init();
		if (this->_focusedWindow != nullptr) {
			this->_focusedWindow->focusEvent(false);
		}
		this->_focusedWindow = window;
		this->_focusedWindow->focusEvent(true);
		this->_windowsPid++;

		return true;
	}
	catch (const std::runtime_error& error) {
		return false;
	}
}

const std::vector<std::shared_ptr<Window>>& Desktop::getWindows() const
{
	return this->_windows;
}

bool Desktop::killWindow(unsigned int processId)
{
	std::shared_ptr<Window> matchingWindow = nullptr;
	for (const std::shared_ptr<Window>& window: this->_windows) {
		if (window->getPid() == processId) {
			matchingWindow = window;
		}
	}
	if (matchingWindow == nullptr) {
		return false;
	}
	matchingWindow->close();
	return true;
}

void Desktop::closeEvent([[maybe_unused]] const sf::Event::Closed *event)
{
	this->_focusedWindow = nullptr;
	this->_windows.clear();
	this->_window.close();
}

void Desktop::resizeEvent(const sf::Event::Resized *event)
{
	sf::Vector2f factors = sf::Vector2f(
		static_cast<float>(this->_size.x) / event->size.x,
		static_cast<float>(this->_size.y) / event->size.y
	);
	float maxFactor = std::max(factors.x, factors.y);
	factors /= maxFactor;
	this->_canvas_view.setViewport(sf::FloatRect(sf::Vector2f((1.f - factors.x) * 0.5, (1.f - factors.y) * 0.5), factors));
	this->_window.setView(this->_canvas_view);
}

void Desktop::mouseButtonPressEvent([[maybe_unused]] const sf::Event::MouseButtonPressed *event)
{
	std::shared_ptr<Window> previous = this->_focusedWindow;
	this->_focusedWindow = nullptr;
	for (std::vector<std::shared_ptr<Window>>::reverse_iterator it = this->_windows.rbegin(); it != this->_windows.rend(); ++it) {
		if ((*it)->isIn(WindowZone::All, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
			this->_focusedWindow = *it;
			break;
		}
	}
	if (previous != nullptr && this->_focusedWindow != previous) {
		previous->focusEvent(false);
	}
	if (this->_focusedWindow == nullptr) {
		return;
	}
	if (this->_focusedWindow != previous) {
		this->_focusedWindow->focusEvent(true);
	}
	if (this->_focusedWindow == previous && this->_focusedWindow->isIn(WindowZone::Canvas, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusedWindow->addButtonPressed(event->button);
	}
	this->_windows.erase(
		std::remove_if(this->_windows.begin(), this->_windows.end(), [this](std::shared_ptr<Window> window) { return window == this->_focusedWindow; } ),
		this->_windows.end()
	);
	this->_windows.push_back(this->_focusedWindow);

	if (this->_focusedWindow->isIn(WindowZone::BottomRight, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusAction = FocusAction::Resize;
		this->_focusInitialDelta = static_cast<sf::Vector2f>(this->_focusedWindow->getSize() - this->_mouse_coordinated);
	}
	else if (this->_focusedWindow->isIn(WindowZone::Right, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusAction = FocusAction::ResizeH;
		this->_focusInitialDelta = static_cast<sf::Vector2f>(this->_focusedWindow->getSize() - this->_mouse_coordinated);
	}
	else if (this->_focusedWindow->isIn(WindowZone::Bottom, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusAction = FocusAction::ResizeV;
		this->_focusInitialDelta = static_cast<sf::Vector2f>(this->_focusedWindow->getSize() - this->_mouse_coordinated);
	}
	else if (this->_focusedWindow->isIn(WindowZone::TitleBar, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusAction = FocusAction::Move;
		this->_focusInitialDelta = this->_focusedWindow->getPosition() - static_cast<sf::Vector2f>(this->_mouse_coordinated);
	}
}

void Desktop::mouseButtonReleaseEvent([[maybe_unused]] const sf::Event::MouseButtonReleased *event)
{
	this->_focusAction = FocusAction::None;
	if (this->_focusedWindow != nullptr && this->_focusedWindow->isIn(WindowZone::Canvas, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusedWindow->removeButtonPressed(event->button);
	}
}

void Desktop::mouseMoveEvent(const sf::Event::MouseMoved *event)
{
	sf::Vector2f mappedPoint = this->_window.mapPixelToCoords(event->position);
	this->_mouse_coordinated = static_cast<sf::Vector2i>(mappedPoint);
	this->_mouse_coordinated.x = std::max(0, std::min(this->_mouse_coordinated.x, static_cast<int>(this->_size.x - 1)));
	this->_mouse_coordinated.y = std::max(0, std::min(this->_mouse_coordinated.y, static_cast<int>(this->_size.y - 1)));
	for (const std::shared_ptr<Window>& window : this->_windows) {
		window->setMousePosition(this->_mouse_coordinated);
	}
	switch (this->_focusAction) {
		case FocusAction::None:
			return;
		case FocusAction::Move:
			this->_focusedWindow->setPosition(sf::Vector2f(this->_mouse_coordinated + static_cast<sf::Vector2i>(this->_focusInitialDelta)));
			return;
		case FocusAction::Resize:
			this->_focusedWindow->resize(this->_mouse_coordinated + static_cast<sf::Vector2i>(this->_focusInitialDelta));
			return;
		case FocusAction::ResizeH:
			this->_focusedWindow->resizeH(this->_mouse_coordinated.x + static_cast<int>(this->_focusInitialDelta.x));
			return;
		case FocusAction::ResizeV:
			this->_focusedWindow->resizeV(this->_mouse_coordinated.y + static_cast<int>(this->_focusInitialDelta.y));
			return;
	}
}

void Desktop::keyPressEvent(const sf::Event::KeyPressed *event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	if (event->control) {
		switch (event->code) {
			case sf::Keyboard::Key::F:
				this->_focusedWindow->toggleFullscreen();
				return;
			case sf::Keyboard::Key::Q:
				this->_focusedWindow->close();
				return;
			default:
				break;
		}
	}
	this->_focusedWindow->addKeyPressed(event->code);
}

void Desktop::keyReleaseEvent(const sf::Event::KeyReleased *event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_focusedWindow->removeKeyPressed(event->code);
}

void Desktop::textEvent(const sf::Event::TextEntered *event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_focusedWindow->textEnteredEvent(event->unicode);
}

void Desktop::mouseWheelEvent(const sf::Event::MouseWheelScrolled *event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_focusedWindow->mouseWheelEvent(event);
}
