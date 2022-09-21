#include <helper.hh>

void drawOnTexture(sf::RenderTexture& texture, const unsigned char* sprite, unsigned char height, sf::Color color)
{
	sf::RectangleShape pixel(sf::Vector2f(1, 1));
	pixel.setOutlineColor(color);
	pixel.setFillColor(color);

	for (unsigned char i = 0; i < height; ++i) {
		unsigned char row = sprite[i];
		for (unsigned char b = 0; b < 8; ++b) {
			if ((row << b) & 0x80) {
				pixel.setPosition(sf::Vector2f(b, i));
				texture.draw(pixel);
			}
		}
	}
}