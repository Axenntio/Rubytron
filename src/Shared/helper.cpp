#include <Shared/helper.hh>

void drawOnTexture(sf::RenderTexture& texture, int x, int y, const unsigned char* sprBytes, unsigned char height, sf::Color color)
{
	sf::RectangleShape pixel(sf::Vector2f(1, 1));
	pixel.setOutlineColor(color);
	pixel.setFillColor(color);

	for (unsigned char i = 0; i < height; ++i) {
		unsigned char row = sprBytes[i];
		for (unsigned char b = 0; b < 8; ++b) {
			if ((row << b) & 0x80) {
				pixel.setPosition(sf::Vector2f(x, y) + sf::Vector2f(b, i));
				texture.draw(pixel);
			}
		}
	}
}

void drawOnTexture(sf::RenderTarget& target, sf::RenderStates states, int x, int y, const unsigned char* sprBytes, unsigned char height, sf::Color color)
{
	sf::RenderTexture texture;
	texture.resize({8, height});
	texture.clear(sf::Color::Transparent);
	drawOnTexture(texture, 0, 0, sprBytes, height, color);
	sf::Sprite sprite(texture.getTexture());
	sprite.setTextureRect(sf::IntRect({0, height}, {8, -height}));
	sprite.setPosition(sf::Vector2f(x, y));
	target.draw(sprite, states);
}

// line1, line2, line3, line4, line5, xoffset, width;
const unsigned char font[] = {
	// 0x00 - nul
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x01 - soh
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x02 - stx
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x03 - etx
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x04 - eot
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x05 - enq
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x06 - ack
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x07 - bel
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x08 - bs
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x09 - ht
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 0,
	// 0x0a - nl
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 0,
	// 0x0b - vt
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x0c - np
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x0d - cr
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 0,
	// 0x0e - so
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x0f - si
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x10 - dle
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x11 - dc1
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x12 - dc2
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x13 - dc3
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x14 - dc4
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x15 - nak
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x16 - syn
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x17 - etb
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x18 - can
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x19 - em
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x1a - sub
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x1b - esc
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x1c - fs
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x1d - gs
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x1e - rs
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x1f - us
	0b10101000, 0b01010000, 0b10101000, 0b01010000, 0b10101000, 0b01010000, 0, FONT_WIDTH,
	// 0x20 - sp
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 1,
	// 0x21 - !
	0b01000000, 0b01000000, 0b01000000, 0b00000000, 0b01000000, 0b00000000, 1, 2,
	// 0x22 - "
	0b10100000, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 3,
	// 0x23 - #
	0b01010000, 0b11110000, 0b01010000, 0b11110000, 0b01010000, 0b00000000, 0, 4,
	// 0x24 - $
	0b01100000, 0b11000000, 0b01100000, 0b11000000, 0b01000000, 0b00000000, 0, 3,
	// 0x25 - %
	0b10000000, 0b00100000, 0b01000000, 0b10000000, 0b00100000, 0b00000000, 0, 3,
	// 0x26 - &
	0b11000000, 0b11000000, 0b01010000, 0b10100000, 0b01010000, 0b00000000, 0, 4,
	// 0x27 - '
	0b01000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 1, 2,
	// 0x28 - (
	0b01000000, 0b10000000, 0b10000000, 0b10000000, 0b01000000, 0b00000000, 0, 2,
	// 0x29 - )
	0b10000000, 0b01000000, 0b01000000, 0b01000000, 0b10000000, 0b00000000, 0, 2,
	// 0x2a - *
	0b10100000, 0b01000000, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0, 3,
	// 0x2b - +
	0b00000000, 0b01000000, 0b11100000, 0b01000000, 0b00000000, 0b00000000, 0, 3,
	// 0x2c - ,
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b10000000, 0, 2,
	// 0x2d - -
	0b00000000, 0b00000000, 0b11100000, 0b00000000, 0b00000000, 0b00000000, 0, 3,
	// 0x2e - .
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 1, 2,
	// 0x2f - /
	0b00100000, 0b00100000, 0b01000000, 0b10000000, 0b10000000, 0b00000000, 0, 3,
	// 0x30 - 0
	0b01100000, 0b10100000, 0b10100000, 0b10100000, 0b11000000, 0b00000000, 0, 3,
	// 0x31 - 1
	0b01000000, 0b11000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000, 0, 2,
	// 0x32 - 2
	0b11000000, 0b00100000, 0b01000000, 0b10000000, 0b11100000, 0b00000000, 0, 3,
	// 0x33 - 3
	0b11000000, 0b00100000, 0b01000000, 0b00100000, 0b11000000, 0b00000000, 0, 3,
	// 0x34 - 4
	0b10100000, 0b10100000, 0b11100000, 0b00100000, 0b00100000, 0b00000000, 0, 3,
	// 0x35 - 5
	0b11100000, 0b10000000, 0b11000000, 0b00100000, 0b11000000, 0b00000000, 0, 3,
	// 0x36 - 6
	0b01100000, 0b10000000, 0b11100000, 0b10100000, 0b11100000, 0b00000000, 0, 3,
	// 0x37 - 7
	0b11100000, 0b00100000, 0b01000000, 0b10000000, 0b10000000, 0b00000000, 0, 3,
	// 0x38 - 8
	0b11100000, 0b10100000, 0b11100000, 0b10100000, 0b11100000, 0b00000000, 0, 3,
	// 0x39 - 9
	0b11100000, 0b10100000, 0b11100000, 0b00100000, 0b11000000, 0b00000000, 0, 3,
	// 0x3a - :
	0b00000000, 0b01000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 1, 2,
	// 0x3b - ;
	0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b01000000, 0b10000000, 0, 3,
	// 0x3c - <
	0b00100000, 0b01000000, 0b10000000, 0b01000000, 0b00100000, 0b00000000, 0, 3,
	// 0x3d - =
	0b00000000, 0b11100000, 0b00000000, 0b11100000, 0b00000000, 0b00000000, 0, 3,
	// 0x3e - >
	0b10000000, 0b01000000, 0b00100000, 0b01000000, 0b10000000, 0b00000000, 0, 3,
	// 0x3f - ?
	0b11100000, 0b00100000, 0b01000000, 0b00000000, 0b01000000, 0b00000000, 0, 3,
	// 0x40 - @
	0b01100000, 0b10110000, 0b10110000, 0b10000000, 0b01100000, 0b00000000, 0, 4,
	// 0x41 - A
	0b01000000, 0b10100000, 0b11100000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x42 - B
	0b11000000, 0b10100000, 0b11000000, 0b10100000, 0b11000000, 0b00000000, 0, 3,
	// 0x43 - C
	0b01100000, 0b10000000, 0b10000000, 0b10000000, 0b01100000, 0b00000000, 0, 3,
	// 0x44 - D
	0b11000000, 0b10100000, 0b10100000, 0b10100000, 0b11000000, 0b00000000, 0, 3,
	// 0x45 - E
	0b11100000, 0b10000000, 0b11100000, 0b10000000, 0b11100000, 0b00000000, 0, 3,
	// 0x46 - F
	0b11100000, 0b10000000, 0b11100000, 0b10000000, 0b10000000, 0b00000000, 0, 3,
	// 0x47 - G
	0b01100000, 0b10000000, 0b11100000, 0b10100000, 0b01100000, 0b00000000, 0, 3,
	// 0x48 - H
	0b10100000, 0b10100000, 0b11100000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x49 - I
	0b11100000, 0b01000000, 0b01000000, 0b01000000, 0b11100000, 0b00000000, 0, 3,
	// 0x4a - J
	0b00100000, 0b00100000, 0b00100000, 0b10100000, 0b01000000, 0b00000000, 0, 3,
	// 0x4b - K
	0b10100000, 0b10100000, 0b11000000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x4c - L
	0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b11100000, 0b00000000, 0, 3,
	// 0x4d - M
	0b10100000, 0b11100000, 0b11100000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x4e - N
	0b10100000, 0b11100000, 0b11100000, 0b11100000, 0b10100000, 0b00000000, 0, 3,
	// 0x4f - O
	0b01000000, 0b10100000, 0b10100000, 0b10100000, 0b01000000, 0b00000000, 0, 3,
	// 0x50 - P
	0b11000000, 0b10100000, 0b11000000, 0b10000000, 0b10000000, 0b00000000, 0, 3,
	// 0x51 - Q
	0b01000000, 0b10100000, 0b10100000, 0b10100000, 0b01100000, 0b00000000, 0, 3,
	// 0x52 - R
	0b11000000, 0b10100000, 0b11100000, 0b11000000, 0b10100000, 0b00000000, 0, 3,
	// 0x53 - S
	0b01100000, 0b10000000, 0b01000000, 0b00100000, 0b11000000, 0b00000000, 0, 3,
	// 0x54 - T
	0b11100000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000, 0, 3,
	// 0x55 - U
	0b10100000, 0b10100000, 0b10100000, 0b10100000, 0b01100000, 0b00000000, 0, 3,
	// 0x56 - V
	0b10100000, 0b10100000, 0b10100000, 0b01000000, 0b01000000, 0b00000000, 0, 3,
	// 0x57 - W
	0b10100000, 0b10100000, 0b11100000, 0b11100000, 0b10100000, 0b00000000, 0, 3,
	// 0x58 - X
	0b10100000, 0b10100000, 0b01000000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x59 - Y
	0b10100000, 0b10100000, 0b01000000, 0b01000000, 0b01000000, 0b00000000, 0, 3,
	// 0x5a - Z
	0b11100000, 0b00100000, 0b01000000, 0b10000000, 0b11100000, 0b00000000, 0, 3,
	// 0x5b - [
	0b11000000, 0b10000000, 0b10000000, 0b10000000, 0b11000000, 0b00000000, 0, 2,
	// 0x5c - backslash
	0b10000000, 0b10000000, 0b01000000, 0b00100000, 0b00100000, 0b00000000, 0, 3,
	// 0x5d - ]
	0b11000000, 0b01000000, 0b01000000, 0b01000000, 0b11000000, 0b00000000, 0, 2,
	// 0x5e - ^
	0b01000000, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 3,
	// 0x5f - _
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11100000, 0b00000000, 0, 3,
	// 0x60 - `
	0b10000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, 2,
	// 0x61 - a
	0b00000000, 0b11000000, 0b01100000, 0b10100000, 0b11100000, 0b00000000, 0, 3,
	// 0x62 - b
	0b10000000, 0b11000000, 0b10100000, 0b10100000, 0b11000000, 0b00000000, 0, 3,
	// 0x63 - c
	0b00000000, 0b01100000, 0b10000000, 0b10000000, 0b01100000, 0b00000000, 0, 3,
	// 0x64 - d
	0b00100000, 0b01100000, 0b10100000, 0b10100000, 0b01100000, 0b00000000, 0, 3,
	// 0x65 - e
	0b00000000, 0b01100000, 0b10100000, 0b11000000, 0b01100000, 0b00000000, 0, 3,
	// 0x66 - f
	0b00100000, 0b01000000, 0b11100000, 0b01000000, 0b01000000, 0b00000000, 0, 3,
	// 0x67 - g
	0b00000000, 0b01100000, 0b10100000, 0b11100000, 0b00100000, 0b01000000, 0, 3,
	// 0x68 - h
	0b10000000, 0b11000000, 0b10100000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x69 - i
	0b01000000, 0b00000000, 0b01000000, 0b01000000, 0b01000000, 0b00000000, 1, 2,
	// 0x6a - j
	0b00100000, 0b00000000, 0b00100000, 0b00100000, 0b10100000, 0b01000000, 0, 3,
	// 0x6b - k
	0b10000000, 0b10100000, 0b11000000, 0b11000000, 0b10100000, 0b00000000, 0, 3,
	// 0x6c - l
	0b11000000, 0b01000000, 0b01000000, 0b01000000, 0b11100000, 0b00000000, 0, 3,
	// 0x6d - m
	0b00000000, 0b11100000, 0b11100000, 0b11100000, 0b10100000, 0b00000000, 0, 3,
	// 0x6e - n
	0b00000000, 0b11000000, 0b10100000, 0b10100000, 0b10100000, 0b00000000, 0, 3,
	// 0x6f - o
	0b00000000, 0b01000000, 0b10100000, 0b10100000, 0b01000000, 0b00000000, 0, 3,
	// 0x70 - p
	0b00000000, 0b11000000, 0b10100000, 0b10100000, 0b11000000, 0b10000000, 0, 3,
	// 0x71 - q
	0b00000000, 0b01100000, 0b10100000, 0b10100000, 0b01100000, 0b00100000, 0, 3,
	// 0x72 - r
	0b00000000, 0b01100000, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0, 3,
	// 0x73 - s
	0b00000000, 0b01100000, 0b11000000, 0b01100000, 0b11000000, 0b00000000, 0, 3,
	// 0x74 - t
	0b01000000, 0b11100000, 0b01000000, 0b01000000, 0b01100000, 0b00000000, 0, 3,
	// 0x75 - u
	0b00000000, 0b10100000, 0b10100000, 0b10100000, 0b01100000, 0b00000000, 0, 3,
	// 0x76 - v
	0b00000000, 0b10100000, 0b10100000, 0b11100000, 0b01000000, 0b00000000, 0, 3,
	// 0x77 - w
	0b00000000, 0b10100000, 0b11100000, 0b11100000, 0b11100000, 0b00000000, 0, 3,
	// 0x78 - x
	0b00000000, 0b10100000, 0b01000000, 0b01000000, 0b10100000, 0b00000000, 0, 3,
	// 0x79 - y
	0b00000000, 0b10100000, 0b10100000, 0b01100000, 0b00100000, 0b01000000, 0, 3,
	// 0x7a - z
	0b00000000, 0b11100000, 0b01100000, 0b11000000, 0b11100000, 0b00000000, 0, 3,
	// 0x7b - {
	0b01100000, 0b01000000, 0b10000000, 0b01000000, 0b01100000, 0b00000000, 0, 3,
	// 0x7c - |
	0b01000000, 0b01000000, 0b00000000, 0b01000000, 0b01000000, 0b00000000, 1, 2,
	// 0x7d - }
	0b11000000, 0b01000000, 0b00100000, 0b01000000, 0b11000000, 0b00000000, 0, 3,
	// 0x7e - ~
	0b00000000, 0b01010000, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0, 4,
	// 0x7f - del
	0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0, FONT_WIDTH,
};

void drawText(sf::RenderTexture& texture, int x, int y, const std::string& text, sf::Color color, bool monospace)
{
	unsigned int xoffset = 0;
	unsigned int yoffset = 0;
	for (unsigned int i = 0; i < text.length(); ++i) {
		unsigned char c = text[i];
		if (c > 0x7f) {
			c = 0;
		}
		if (!monospace) {
			xoffset -= (font + FONT_DATA_LEN * c)[FONT_HEIGHT];
		}
		drawOnTexture(texture, x + xoffset, y + yoffset, font + FONT_DATA_LEN * c, FONT_HEIGHT, color);
		if (monospace) {
			xoffset += FONT_WIDTH;
		}
		else {
			xoffset += (font + FONT_DATA_LEN * c)[FONT_HEIGHT + 1];
		}
		if (c == 0x0a) {
			yoffset += FONT_HEIGHT;
			continue;
		}
		if (c == 0x0d) {
			xoffset = 0;
			continue;
		}
		++xoffset;
	}
}
void drawText(sf::RenderTarget& target, sf::RenderStates states, int x, int y, const std::string& text, sf::Color color, bool monospace)
{
	unsigned int xoffset = 0;
	unsigned int yoffset = 0;
	for (unsigned int i = 0; i < text.length(); ++i) {
		unsigned char c = text[i];
		if (c > 0x7f) {
			c = 0;
		}
		if (!monospace) {
			xoffset -= (font + FONT_DATA_LEN * c)[FONT_HEIGHT];
		}
		drawOnTexture(target, states, x + xoffset, y + yoffset, font + FONT_DATA_LEN * c, FONT_HEIGHT, color);
		if (monospace) {
			xoffset += FONT_WIDTH;
		}
		else {
			xoffset += (font + FONT_DATA_LEN * c)[FONT_HEIGHT + 1];
		}
		if (c == 0x0a) {
			yoffset += FONT_HEIGHT;
			continue;
		}
		if (c == 0x0d) {
			xoffset = 0;
			continue;
		}
		++xoffset;
	}
}