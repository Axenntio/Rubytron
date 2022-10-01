#include <Desktop.hpp>
#include <helper.hh>
#include <sprites.hh>

Desktop::Desktop(unsigned int width, unsigned int height, unsigned char scale, TitleBarMode titleBarMode) : _width(width), _height(height), _titleBarMode(titleBarMode)
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

	this->_window.create(sf::VideoMode(this->_width * scale, this->_height * scale), "Rubytron");
	this->_window.setMouseCursorVisible(false);
	this->_background_texture.create(this->_width, this->_height);
	this->_background_texture.clear(this->_palette[1]);
	this->_foreground_texture.create(this->_width, this->_height);
	this->_foreground_texture.clear(sf::Color::Transparent);
	this->_cursor_texture.create(4, 6);
	this->_cursor_texture.clear(sf::Color::Transparent);

	drawOnTexture(this->_cursor_texture, 0, 0, spr_cursor, SPR_CURSOR_HEIGHT, this->_palette[7]);


	this->_canvas_view.setCenter(this->_width / 2, this->_height / 2);
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	this->_canvas_view.setSize(sf::Vector2f(this->_width, this->_height));
	this->_window.setView(this->_canvas_view);

	this->_focusedWindow = nullptr;
	this->_focusAction = FocusAction::None;
	this->_windows.push_back(new Window(sf::Vector2i(10, 10), sf::Vector2u(60, 30), this->_palette, this->_titleBarMode, "programs/test.rb", {}));
	this->_windows.push_back(new Window(sf::Vector2i(60, 60), sf::Vector2u(10, 10), this->_palette, this->_titleBarMode, "programs/resize.rb", {}));
	this->_windows.push_back(new Window(sf::Vector2i(30, 8), sf::Vector2u(20, 20), this->_palette, this->_titleBarMode, "programs/palette.rb", {}));
	this->_windows.push_back(new Window(sf::Vector2i(85, 30), sf::Vector2u(40, 40), this->_palette, this->_titleBarMode, "programs/key.rb", {}));
	this->_windows.push_back(new Window(sf::Vector2i(100, 15), sf::Vector2u(40, 20), this->_palette, this->_titleBarMode, "programs/xeyes.rb", {}));
	this->_windows.push_back(new Window(sf::Vector2i(120, 50), sf::Vector2u(40, 40), this->_palette, this->_titleBarMode, "programs/snake.rb", {}));
	this->_windows.push_back(new Window(sf::Vector2i(5, 54), sf::Vector2u(48, 68), this->_palette, this->_titleBarMode, "programs/editor.rb", {"programs/editor.rb"}));
	this->_windows.push_back(new Window(sf::Vector2i(57, 80), sf::Vector2u(60, 42), this->_palette, this->_titleBarMode, "programs/terminal.rb", {}));
	for (Window* window : this->_windows) {
		window->init();
	}
}

void Desktop::run()
{
	while (this->_window.isOpen())
	{
		sf::Event event;
		while (this->_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				this->closeEvent(event);
			}
			if (event.type == sf::Event::Resized) {
				this->resizeEvent(event);
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				this->mouseButtonPressEvent(event);
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				this->mouseButtonReleaseEvent(event);
			}
			if (event.type == sf::Event::MouseMoved) {
				this->mouseMoveEvent(event);
			}
			if (event.type == sf::Event::KeyPressed) {
				this->keyPressEvent(event);
			}
			if (event.type == sf::Event::KeyReleased) {
				this->keyReleaseEvent(event);
			}
			if (event.type == sf::Event::TextEntered) {
				this->textEvent(event);
			}
		}

		this->_windows.erase(
			std::remove_if(this->_windows.begin(), this->_windows.end(), [](const Window* window) { return window->isClosed(); }),
			this->_windows.end()
		);
		this->_window.setVerticalSyncEnabled(true);
		this->_window.clear();
		sf::Sprite background(this->_background_texture.getTexture());
		sf::Sprite foreground(this->_foreground_texture.getTexture());
		sf::Sprite cursor(this->_cursor_texture.getTexture());
		cursor.setTextureRect(sf::IntRect(0, 6, 4, -6));
		cursor.setPosition(this->_mouse_coordinated.x, this->_mouse_coordinated.y);
		this->_window.draw(background);
		for (Window* window : this->_windows) {
			window->exceptionHandler();
			this->_window.draw(*window);
		}
		this->_window.draw(foreground);
		this->_window.draw(cursor);
		this->_window.display();
	}
}

Window* Desktop::getWindow(mrb_state* mrb) const
{
	for (Window* window : this->_windows) {
		if (window->isContext(mrb)) {
			return window;
		}
	}
	return nullptr;
}

bool Desktop::isFocused(const Window* window) const
{
	return this->_focusedWindow == window;
}

bool Desktop::spawn(const std::string& path, const std::vector<std::string>& parameters)
{
	try {
		Window* window = new Window(sf::Vector2i(10, 10), sf::Vector2u(60, 30), this->_palette, this->_titleBarMode, path, parameters);
		this->_windows.push_back(window);
		window->init();
		Window* previous = this->_focusedWindow;
		this->_focusedWindow = window;
		if (previous != nullptr) {
			previous->focusEvent(false);
		}
		this->_focusedWindow->focusEvent(true);

		return true;
	}
	catch (const std::runtime_error& error) {
		return false;
	}
}

void Desktop::closeEvent([[maybe_unused]] sf::Event event)
{
	this->_focusedWindow = nullptr;
	for (Window* window : this->_windows) {
		delete window;
	}
	this->_windows.clear();
	this->_window.close();
}

void Desktop::resizeEvent(sf::Event event)
{
	sf::Vector2f factors = sf::Vector2f(
		static_cast<float>(this->_width) / event.size.width,
		static_cast<float>(this->_height) / event.size.height
	);
	float maxFactor = std::max(factors.x, factors.y);
	factors /= maxFactor;
	this->_canvas_view.setViewport(sf::FloatRect((1.f - factors.x) * 0.5 , (1.f - factors.y) * 0.5, factors.x, factors.y));
	this->_window.setView(this->_canvas_view);
}

void Desktop::mouseButtonPressEvent([[maybe_unused]] sf::Event event)
{
	Window* previous = this->_focusedWindow;
	this->_focusedWindow = nullptr;
	for (std::vector<Window*>::reverse_iterator it = this->_windows.rbegin(); it != this->_windows.rend(); ++it) {
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
	this->_windows.erase(
		std::remove_if(this->_windows.begin(), this->_windows.end(), [this](Window* window) { return window == this->_focusedWindow; } ),
		this->_windows.end()
	);
	this->_windows.push_back(this->_focusedWindow);

	if (this->_focusedWindow->isIn(WindowZone::BottomRight, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusAction = FocusAction::Resize;
		this->_focusInitialDelta = static_cast<sf::Vector2f>(this->_focusedWindow->getSize() - this->_mouse_coordinated);
	}
	else if (this->_focusedWindow->isIn(WindowZone::TitleBar, static_cast<sf::Vector2i>(this->_mouse_coordinated))) {
		this->_focusAction = FocusAction::Move;
		this->_focusInitialDelta = this->_focusedWindow->getPosition() - static_cast<sf::Vector2f>(this->_mouse_coordinated);
	}
}

void Desktop::mouseButtonReleaseEvent([[maybe_unused]] sf::Event event)
{
	this->_focusAction = FocusAction::None;
}

void Desktop::mouseMoveEvent(sf::Event event)
{
	sf::Vector2f mappedPoint = this->_window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	this->_mouse_coordinated = static_cast<sf::Vector2i>(mappedPoint);
	this->_mouse_coordinated.x = std::max(0, std::min(this->_mouse_coordinated.x, static_cast<int>(this->_width - 1)));
	this->_mouse_coordinated.y = std::max(0, std::min(this->_mouse_coordinated.y, static_cast<int>(this->_height - 1)));
	for (Window* window : this->_windows) {
		window->setMousePosition(this->_mouse_coordinated);
	}
	switch (this->_focusAction) {
		case FocusAction::None:
			return;
		case FocusAction::Move:
			this->_focusedWindow->setPosition(sf::Vector2f(this->_mouse_coordinated + sf::Vector2i(this->_focusInitialDelta)));
			return;
		case FocusAction::Resize:
			this->_focusedWindow->resize(this->_mouse_coordinated + sf::Vector2i(this->_focusInitialDelta));
			return;
	}
}

void Desktop::keyPressEvent(sf::Event event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_focusedWindow->addKeyPressed(event.key.code);
}

void Desktop::keyReleaseEvent(sf::Event event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_focusedWindow->removeKeyPressed(event.key.code);
}

void Desktop::textEvent(sf::Event event)
{
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_focusedWindow->textEnteredEvent(event.text.unicode);
}