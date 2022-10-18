#include <Runtime/Player.hpp>
#include <Shared/helper.hh>
#include <Shared/sprites.hh>
#include <mruby/internal.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/error.h>

Player::Player(sf::Vector2u size, unsigned char scale, const std::vector<std::string>& parameters) : AbstractWindow(sf::Vector2i(0, 0), size, std::vector<sf::Color> {sf::Color(0, 0, 0)}, "program.rb", parameters), _scale(scale)
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

	this->_window.create(sf::VideoMode(this->_size.x * this->_scale, this->_size.y * this->_scale), this->_title);
	this->_window.setMouseCursorVisible(false);
	this->_cursor_texture.create(4, 6);
	this->_cursor_texture.clear(sf::Color::Transparent);

	drawOnTexture(this->_cursor_texture, 0, 0, spr_cursor, SPR_CURSOR_HEIGHT, this->_palette[7]);

	this->_canvas_view.setCenter(this->_size.x / 2, this->_size.y / 2);
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	this->_canvas_view.setSize(sf::Vector2f(this->_size.x, this->_size.y));
	this->_window.setView(this->_canvas_view);
}

void Player::run()
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
				this->addKeyPressed(event.key.code);
			}
			if (event.type == sf::Event::TextEntered) {
				this->textEvent(event);
			}
			if (event.type == sf::Event::GainedFocus) {
				this->focusEvent(true);
			}
			if (event.type == sf::Event::LostFocus) {
				this->focusEvent(false);
			}
			if (event.type == sf::Event::MouseWheelScrolled) {
				this->mouseWheelEvent(event);
			}
		}

		this->_window.setVerticalSyncEnabled(true);
		this->_window.clear();
		sf::Sprite cursor(this->_cursor_texture.getTexture());
		cursor.setTextureRect(sf::IntRect(0, 6, 4, -6));
		cursor.setPosition(this->_mouse_coordinated.x, this->_mouse_coordinated.y);
		this->exceptionHandler();
		this->_window.draw(*this);
		this->_window.draw(cursor);
		this->_window.display();
	}
}

void Player::closeEvent([[maybe_unused]] sf::Event event)
{
	this->_window.close();
}

void Player::resizeEvent(sf::Event event)
{
	sf::Vector2f factors = sf::Vector2f(
		static_cast<float>(this->_size.x) / event.size.width,
		static_cast<float>(this->_size.y) / event.size.height
	);
	float maxFactor = std::max(factors.x, factors.y);
	factors /= maxFactor;
	this->_canvas_view.setViewport(sf::FloatRect((1.f - factors.x) * 0.5 , (1.f - factors.y) * 0.5, factors.x, factors.y));
	this->_window.setView(this->_canvas_view);
}

void Player::mouseButtonPressEvent([[maybe_unused]] sf::Event event)
{
}

void Player::mouseButtonReleaseEvent([[maybe_unused]] sf::Event event)
{
}

void Player::mouseMoveEvent(sf::Event event)
{
	sf::Vector2f mappedPoint = this->_window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
	this->_mouse_coordinated = static_cast<sf::Vector2i>(mappedPoint);
	this->_mouse_coordinated.x = std::max(0, std::min(this->_mouse_coordinated.x, static_cast<int>(this->_size.x - 1)));
	this->_mouse_coordinated.y = std::max(0, std::min(this->_mouse_coordinated.y, static_cast<int>(this->_size.y - 1)));
	this->setMousePosition(this->_mouse_coordinated);
}

void Player::keyPressEvent(sf::Event event)
{
	this->addKeyPressed(event.key.code);
}

void Player::keyReleaseEvent(sf::Event event)
{
	this->removeKeyPressed(event.key.code);
}

void Player::textEvent(sf::Event event)
{
	this->textEnteredEvent(event.text.unicode);
}

void Player::mouseWheelEvent(sf::Event event)
{
	AbstractWindow::mouseWheelEvent(event.mouseWheelScroll);
}

void Player::resizeTrigger()
{
	// This can pin the window on the top left corner, but is buggy
	// sf::Vector2i size = static_cast<sf::Vector2i>(this->_window.getSize());
	// this->_window.setPosition(this->_window.getPosition() - sf::Vector2i(0, size.y - this->_size.y));
	this->_window.setSize(sf::Vector2u(this->_size.x * this->_scale, this->_size.y * this->_scale));

	// This properly resize window but issue on fullscreen
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	this->_canvas_view.setSize(sf::Vector2f(this->_size.x, this->_size.y));
	this->_canvas_view.setCenter(static_cast<float>(this->_size.x) / 2, static_cast<float>(this->_size.y) / 2);
	this->_window.setView(this->_canvas_view);

	AbstractWindow::resizeTrigger();
}

void Player::changeTitleTrigger()
{
	this->_window.setTitle(this->_title);
	AbstractWindow::changeTitleTrigger();
}