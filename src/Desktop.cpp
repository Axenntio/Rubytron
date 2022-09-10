#include <Desktop.hpp>

Desktop::Desktop(unsigned int w, unsigned int h, unsigned char scale = 1) : _width(w), _height(h)
{
	this->_window.create(sf::VideoMode(this->_width * scale, this->_height * scale), "Rubytron");
	this->_background_texture.create(this->_width, this->_height);
	this->_background_texture.clear(sf::Color::White);
	this->_foreground_texture.create(this->_width, this->_height);
	this->_foreground_texture.clear(sf::Color::Transparent);
	this->_canvas_view.setCenter(this->_width / 2, this->_height / 2);
	this->_canvas_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	this->_canvas_view.setSize(sf::Vector2f(this->_width, this->_height));
	this->_window.setView(this->_canvas_view);

	sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color::Red);
	shape.setPosition(0, 0);
	this->_foreground_texture.draw(shape);

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
				sf::Vector2f factors = sf::Vector2f(
					static_cast<float>(this->_width) / event.size.width,
					static_cast<float>(this->_height) / event.size.height
				);
				float maxFactor = std::max(factors.x, factors.y);
				factors /= maxFactor;
				this->_canvas_view.setViewport(sf::FloatRect((1.f - factors.x) * 0.5 , (1.f - factors.y) * 0.5, factors.x, factors.y));
				this->_window.setView(this->_canvas_view);
			}
        }

		this->_window.setVerticalSyncEnabled(true);
		this->_window.clear();
		sf::Sprite background(this->_background_texture.getTexture());
		sf::Sprite foreground(this->_foreground_texture.getTexture());
		this->_window.draw(background);
		this->_window.draw(foreground);
		this->_window.display();
    }
}