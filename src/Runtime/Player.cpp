#include <Runtime/Player.hpp>
#include <Shared/helper.hh>
#include <Shared/sprites.hh>
#include <mruby/internal.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/error.h>

extern std::shared_ptr<Player> player_ptr;

Player::Player(sf::Vector2u size, unsigned char scale, const std::vector<std::string>& parameters) : AbstractWindow(sf::Vector2i(0, 0), size, std::vector<sf::Color> {sf::Color(0, 0, 0)}, "program.rb", parameters)
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

	this->_window.create(sf::VideoMode(this->_size.x * scale, this->_size.y * scale), "Rubytron");
	this->_window.setMouseCursorVisible(false);
	this->_cursor_texture.create(4, 6);
	this->_cursor_texture.clear(sf::Color::Transparent);

	drawOnTexture(this->_cursor_texture, 0, 0, spr_cursor, SPR_CURSOR_HEIGHT, this->_palette[7]);

	this->_canvas_view.setCenter(this->_size.x / 2, this->_size.y / 2);
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	this->_canvas_view.setSize(sf::Vector2f(this->_size.x, this->_size.y));
	this->_window.setView(this->_canvas_view);

	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "width", &Player::mrubyGetWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "width=", &Player::mrubySetWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "height", &Player::mrubyGetHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "height=", &Player::mrubySetHeight, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_width", &Player::mrubyGetMinWidth, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_width=", &Player::mrubySetMinWidth, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_height", &Player::mrubyGetMinHeight, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "min_height=", &Player::mrubySetMinHeight, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_x", &Player::mrubyGetMouseX, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_x=", &Player::mrubySetMouseX, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_y", &Player::mrubyGetMouseY, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "mouse_y=", &Player::mrubySetMouseY, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "title", &Player::mrubyGetTitle, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "title=", &Player::mrubySetTitle, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "resizable", &Player::mrubyIsResizable, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "resizable=", &Player::mrubySetResizable, MRB_ARGS_REQ(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "reload", &Player::mrubyReload, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "close", &Player::mrubyClose, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "parameters", &Player::mrubyParameters, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "key", &Player::mrubyKey, MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbWindowClass, "focused", &Player::mrubyFocused, MRB_ARGS_NONE());


	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "processes", &Player::mrubyProcesses, MRB_ARGS_NONE());
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "kill_process", &Player::mrubyKillProcess, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "spawn", &Player::mrubySpawn, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
	mrb_define_class_method(this->_mrb, this->_mrbDesktopClass, "export", &Player::mrubyExport, MRB_ARGS_REQ(1));

	mrb_define_method(this->_mrb, this->_mrb->object_class, "clear", &Player::mrubyClear, MRB_ARGS_REQ(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "pixel", &Player::mrubyPixel, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "line", &Player::mrubyLine, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "rect", &Player::mrubyRectangle, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "circle", &Player::mrubyCircle, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));
	mrb_define_method(this->_mrb, this->_mrb->object_class, "text", &Player::mrubyText, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(2));
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
				this->_isFocused = true;
				this->focusEvent(true);
			}
			if (event.type == sf::Event::LostFocus) {
				this->_isFocused = false;
				this->focusEvent(false);
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

mrb_value Player::mrubyGetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetWidth(player_ptr, mrb, self);
}

mrb_value Player::mrubySetWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetWidth(player_ptr, mrb, self);
}

mrb_value Player::mrubyGetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetHeight(player_ptr, mrb, self);
}

mrb_value Player::mrubySetHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetHeight(player_ptr, mrb, self);
}

mrb_value Player::mrubyGetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetMinWidth(player_ptr, mrb, self);
}

mrb_value Player::mrubySetMinWidth(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetMinWidth(player_ptr, mrb, self);
}

mrb_value Player::mrubyGetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetMinHeight(player_ptr, mrb, self);
}

mrb_value Player::mrubySetMinHeight(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetMinHeight(player_ptr, mrb, self);
}

mrb_value Player::mrubyGetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetMouseX(player_ptr, mrb, self);
}

mrb_value Player::mrubySetMouseX(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetMouseX(player_ptr, mrb, self);
}

mrb_value Player::mrubyGetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetMouseY(player_ptr, mrb, self);
}

mrb_value Player::mrubySetMouseY(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetMouseY(player_ptr, mrb, self);
}

mrb_value Player::mrubyGetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyGetTitle(player_ptr, mrb, self);
}

mrb_value Player::mrubySetTitle(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetTitle(player_ptr, mrb, self);
}

mrb_value Player::mrubyIsResizable(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyIsResizable(player_ptr, mrb, self);
}

mrb_value Player::mrubySetResizable(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubySetResizable(player_ptr, mrb, self);
}

mrb_value Player::mrubyReload(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyReload(player_ptr, mrb, self);
}

mrb_value Player::mrubyClose(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyClose(player_ptr, mrb, self);
}

mrb_value Player::mrubyParameters(mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyParameters(player_ptr, mrb, self);
}

mrb_value Player::mrubyFocused([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_bool_value(player_ptr->_isFocused);
}

mrb_value Player::mrubySpawn([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_false_value();
}

mrb_value Player::mrubyExport([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_false_value();
}

mrb_value Player::mrubyProcesses([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_nil_value();
}

mrb_value Player::mrubyKillProcess([[maybe_unused]] mrb_state *mrb, [[maybe_unused]] mrb_value self)
{
	return mrb_false_value();
}

// Drawing

mrb_value Player::mrubyClear(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyClear(player_ptr, mrb, self);
}

mrb_value Player::mrubyPixel(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyPixel(player_ptr, mrb, self);
}

mrb_value Player::mrubyLine(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyLine(player_ptr, mrb, self);
}

mrb_value Player::mrubyRectangle(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyRectangle(player_ptr, mrb, self);
}

mrb_value Player::mrubyCircle(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyCircle(player_ptr, mrb, self);
}

mrb_value Player::mrubyText(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyText(player_ptr, mrb, self);
}

mrb_value Player::mrubyKey(mrb_state* mrb, [[maybe_unused]] mrb_value self)
{
	return AbstractWindow::mrubyKey(player_ptr, mrb, self);
}