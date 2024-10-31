#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>

namespace screen {
	void renderText(SDL_Renderer* renderer, std::string& text, TTF_Font* font, SDL_Color color, int x, int y);
	void renderRedLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text, int font_size, bool var, int ledX, int ledY);
	void renderYellowLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text, int font_size, bool var, int ledX, int ledY);

	void renderYellowTwoLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text1, const std::string& text2, int font_size, bool var, int ledX, int ledY);
	void renderWhiteLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text, int font_size, bool var, int ledX, int ledY);
	void renderGreenLED(SDL_Renderer* renderer, int ledX, int ledY, bool var);
}
