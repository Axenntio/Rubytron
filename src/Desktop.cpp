#include <Desktop.hpp>

Desktop::Desktop(unsigned int w, unsigned int h, unsigned char scale) : _width(w), _height(h), _scale(scale)
{
	this->_window.create(sf::VideoMode(this->_width * this->_scale, this->_height * this->_scale), "Rubytron");
	this->_window_texture.create(this->_width, this->_height);
	this->_window_texture.clear(sf::Color::Red);
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, this->_width, this->_height));
	this->_canvas_view.setSize(sf::Vector2f(this->_width * this->_scale, this->_height * this->_scale));
	sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color::Blue);
	shape.setPosition(10, 10);
	this->_window_texture.draw(shape);
}

void Desktop::run()
{
	while (this->_window.isOpen())
    {
        sf::Event event;
        while (this->_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                this->_window.close();
			if (event.type == sf::Event::Resized) {
				// sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				// this->_window.setView(sf::View(visibleArea));
			}
        }

		this->_window.setVerticalSyncEnabled(true);
		this->_window.clear();
		// this->_window.setView(this->_canvas_view);
		sf::Sprite sprite(this->_window_texture.getTexture());
		sprite.setScale(this->_scale, this->_scale);
		this->_window.draw(sprite);
		this->_window.setView(this->_window.getDefaultView());
		this->_window.display();
    }
}