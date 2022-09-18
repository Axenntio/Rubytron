#include <Desktop.hpp>

Desktop::Desktop(unsigned int width, unsigned int height, unsigned char scale) : _width(width), _height(height)
{
	this->_palette = std::vector<sf::Color> {
		sf::Color( 0,    0,   0), // #000000
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
	this->_background_texture.create(this->_width, this->_height);
	this->_background_texture.clear(this->_palette[0]);
	this->_foreground_texture.create(this->_width, this->_height);
	this->_foreground_texture.clear(sf::Color::Transparent);
	this->_canvas_view.setCenter(this->_width / 2, this->_height / 2);
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	this->_canvas_view.setSize(sf::Vector2f(this->_width, this->_height));
	this->_window.setView(this->_canvas_view);

	this->_focusedWindow = nullptr;
	this->_windows.push_back(new Window(sf::Vector2i(10, 10), sf::Vector2u(60, 30), this->_palette, "test.rb"));
	this->_windows.push_back(new Window(sf::Vector2i(60, 60), sf::Vector2u(10, 10), this->_palette, "resize.rb"));
	this->_windows.push_back(new Window(sf::Vector2i(30, 5), sf::Vector2u(20, 20), this->_palette, "palette.rb"));
	this->_windows.push_back(new Window(sf::Vector2i(100, 15), sf::Vector2u(40, 20), this->_palette, "xeyes.rb"));
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
        }

		this->_window.setVerticalSyncEnabled(true);
		this->_window.clear();
		sf::Sprite background(this->_background_texture.getTexture());
		sf::Sprite foreground(this->_foreground_texture.getTexture());
		this->_window.draw(background);
		for (const Window* window : this->_windows) {
			if (window != this->_focusedWindow) {
				this->_window.draw(*window);
			}
		}
		if (this->_focusedWindow != nullptr) {
			this->_window.draw(*this->_focusedWindow);
		}
		this->_window.draw(foreground);
		this->_window.display();
    }
}

Window* Desktop::getWindow(mrb_state* mrb)
{
	for (Window* window : this->_windows) {
		if (window->isContext(mrb)) {
			return window;
		}
	}
	return nullptr;
}

void Desktop::closeEvent([[maybe_unused]] sf::Event event)
{
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
	sf::Vector2f mappedPoint = this->_window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
	this->_focusedWindow = nullptr;
	for (std::vector<Window*>::reverse_iterator it = this->_windows.rbegin(); it != this->_windows.rend(); ++it) {
		if ((*it)->isIn(static_cast<sf::Vector2i>(mappedPoint))) {
			this->_focusedWindow = *it;
			break;
		}
	}
	if (this->_focusedWindow == nullptr) {
		return;
	}
	this->_windows.erase(
		std::remove_if(this->_windows.begin(), this->_windows.end(), [this](Window* window) { return window == this->_focusedWindow; } )
	);
	this->_windows.push_back(this->_focusedWindow);
	if (event.mouseButton.button == sf::Mouse::Right) {
		this->_focusResize = true;
		this->_focusInitialDelta = static_cast<sf::Vector2f>(this->_focusedWindow->getSize()) - mappedPoint;
	}
	else {
		this->_focusMove = true;
		this->_focusInitialDelta = this->_focusedWindow->getPosition() - mappedPoint;
	}
}

void Desktop::mouseButtonReleaseEvent([[maybe_unused]] sf::Event event)
{
	this->_focusMove = false;
	this->_focusResize = false;
}

void Desktop::mouseMoveEvent([[maybe_unused]] sf::Event event)
{
	sf::Vector2f mappedPoint = this->_window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	for (Window* window : this->_windows) {
		window->setMousePosition(mappedPoint);
	}
	if (this->_focusMove && this->_focusedWindow != nullptr) {
		this->_focusedWindow->setPosition(sf::Vector2f(sf::Vector2i(mappedPoint + this->_focusInitialDelta)));
	}
	if (this->_focusResize && this->_focusedWindow != nullptr) {
		this->_focusedWindow->setSize(sf::Vector2u(mappedPoint + this->_focusInitialDelta));
	}
}