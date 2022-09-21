#pragma once

#include <SFML/Graphics.hpp>

void drawOnTexture(sf::RenderTexture& texture, int x, int y, const unsigned char* sprBytes, unsigned char height, sf::Color color);
void drawOnTexture(sf::RenderTarget& target, sf::RenderStates states, int x, int y, const unsigned char* sprBytes, unsigned char height, sf::Color color);
void drawText(sf::RenderTexture& texture, int x, int y, const std::string& text, sf::Color color);
void drawText(sf::RenderTarget& target, sf::RenderStates states, int x, int y, const std::string& text, sf::Color color);