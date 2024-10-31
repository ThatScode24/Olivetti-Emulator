#include "screen.h"
#include "CPU.h"
#include <vector>
#include <string>
#include <sstream>



void screen::renderText(SDL_Renderer* renderer, std::string& text, TTF_Font* font, SDL_Color color, int x, int y) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (!surface) {
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
		return;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		std::cerr << "Unable to create texture from surface! SDL Error: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(surface);
		return;
	}
	int width = surface->w;
	int height = surface->h;
	SDL_FreeSurface(surface);

	SDL_Rect renderQuad = { x, y, width, height };
	SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);
	SDL_DestroyTexture(texture);
}



void screen::renderRedLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text, int font_size, bool var, int ledX, int ledY) {
    // LED color in white when active, with a black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black border color
    SDL_Rect borderRect = { ledX - 2, ledY - 2, 54, 54 }; // Slightly larger rectangle for border
    SDL_RenderFillRect(renderer, &borderRect);

    if (var) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    }
    else {
        SDL_SetRenderDrawColor(renderer, 64, 0, 0, 255); // Dimmed LED color
    }

    // LED as filled rectangle inside the border
    SDL_Rect ledRect = { ledX, ledY, 50, 50 };
    SDL_RenderFillRect(renderer, &ledRect);

    // Render text in black on the LED
    SDL_Color textColor = { 0, 0, 0, 255 };
    TTF_Font* ledFont = TTF_OpenFont("./olivettiScreen/led.ttf", font_size);
    if (ledFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(ledFont, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = { static_cast<int>(ledX + offsetX), static_cast<int>(ledY + offsetY), textSurface->w, textSurface->h };

            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        else {
            std::cerr << "Failed to create text surface! TTF Error: " << TTF_GetError() << std::endl;
        }
        TTF_CloseFont(ledFont);
    }
    else {
        std::cerr << "Failed to load LED font! TTF Error: " << TTF_GetError() << std::endl;
    }
}

void screen::renderYellowTwoLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text1, const std::string& text2, int font_size, bool var, int ledX, int ledY) {
    // Draw black border for the LED
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black border color
    SDL_Rect borderRect = { ledX - 2, ledY - 2, 54, 54 }; // Slightly larger rectangle for border
    SDL_RenderFillRect(renderer, &borderRect);

    // Set LED color based on the "var" state
    if (var) {
        SDL_SetRenderDrawColor(renderer, 252, 220, 39, 255); // Bright yellow for active
    }
    else {
        SDL_SetRenderDrawColor(renderer, 64, 64, 0, 255); // Dimmed yellow for inactive
    }

    // LED as a filled rectangle inside the border
    SDL_Rect ledRect = { ledX, ledY, 50, 50 };
    SDL_RenderFillRect(renderer, &ledRect);

    // Set text color
    SDL_Color textColor = { 0, 0, 0, 255 }; // Black text color
    TTF_Font* ledFont = TTF_OpenFont("./olivettiScreen/led.ttf", font_size);

    if (ledFont) {
        // Render `text1` on the top half of the LED
        SDL_Surface* textSurface1 = TTF_RenderText_Solid(ledFont, text1.c_str(), textColor);
        if (textSurface1) {
            SDL_Texture* textTexture1 = SDL_CreateTextureFromSurface(renderer, textSurface1);
            int textHeight1 = textSurface1->h;
            SDL_Rect textRect1 = { ledX + static_cast<int>(offsetX), ledY + static_cast<int>(offsetY), textSurface1->w, textHeight1 };

            // Center `text1` vertically in the top half
            textRect1.y = ledY + static_cast<int>(offsetY);

            SDL_RenderCopy(renderer, textTexture1, nullptr, &textRect1);

            SDL_FreeSurface(textSurface1);
            SDL_DestroyTexture(textTexture1);
        }
        else {
            std::cerr << "Failed to create text surface for text1! TTF Error: " << TTF_GetError() << std::endl;
        }

        // Render `text2` on the bottom half of the LED
        SDL_Surface* textSurface2 = TTF_RenderText_Solid(ledFont, text2.c_str(), textColor);
        if (textSurface2) {
            SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
            int textHeight2 = textSurface2->h;
            SDL_Rect textRect2 = { ledX + static_cast<int>(offsetX), ledY + 25 + static_cast<int>(offsetY), textSurface2->w, textHeight2 };

            // Center `text2` vertically in the bottom half
            textRect2.y = ledY + 25 + static_cast<int>(offsetY);

            SDL_RenderCopy(renderer, textTexture2, nullptr, &textRect2);

            SDL_FreeSurface(textSurface2);
            SDL_DestroyTexture(textTexture2);
        }
        else {
            std::cerr << "Failed to create text surface for text2! TTF Error: " << TTF_GetError() << std::endl;
        }

        TTF_CloseFont(ledFont);
    }
    else {
        std::cerr << "Failed to load LED font! TTF Error: " << TTF_GetError() << std::endl;
    }
}


void screen::renderYellowLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text, int font_size, bool var, int ledX, int ledY) {
    // LED color in white when active, with a black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black border color
    SDL_Rect borderRect = { ledX - 2, ledY - 2, 54, 54 }; // Slightly larger rectangle for border
    SDL_RenderFillRect(renderer, &borderRect);
    
    if (var) {
        SDL_SetRenderDrawColor(renderer, 252, 220, 39, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 64, 64, 0, 255); // Dimmed LED color
    }

    // LED as filled rectangle inside the border
    SDL_Rect ledRect = { ledX, ledY, 50, 50 };
    SDL_RenderFillRect(renderer, &ledRect);

    // Render text in black on the LED
    SDL_Color textColor = { 0, 0, 0, 255 };
    TTF_Font* ledFont = TTF_OpenFont("./olivettiScreen/led.ttf", font_size);
    if (ledFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(ledFont, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = { static_cast<int>(ledX + offsetX), static_cast<int>(ledY + offsetY), textSurface->w, textSurface->h };

            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        else {
            std::cerr << "Failed to create text surface! TTF Error: " << TTF_GetError() << std::endl;
        }
        TTF_CloseFont(ledFont);
    }
    else {
        std::cerr << "Failed to load LED font! TTF Error: " << TTF_GetError() << std::endl;
    }
}

void screen::renderWhiteLED(SDL_Renderer* renderer, float offsetX, float offsetY, const std::string& text, int font_size, bool var, int ledX, int ledY) {
    // LED color in white when active, with a black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black border color
    SDL_Rect borderRect = { ledX - 2, ledY - 2, 54, 54 }; // Slightly larger rectangle for border
    SDL_RenderFillRect(renderer, &borderRect);
    if (var) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White LED color
    }
    else {
        SDL_SetRenderDrawColor(renderer, 140, 140, 140, 255);// Dimmed white for inactive LED
    }

    // LED as filled rectangle inside the border
    SDL_Rect ledRect = { ledX, ledY, 50, 50 };
    SDL_RenderFillRect(renderer, &ledRect);

    // Render text in black on the LED
    SDL_Color textColor = { 0, 0, 0, 255 };
    TTF_Font* ledFont = TTF_OpenFont("./olivettiScreen/led.ttf", font_size);
    if (ledFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(ledFont, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = { static_cast<int>(ledX + offsetX), static_cast<int>(ledY + offsetY), textSurface->w, textSurface->h };

            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        else {
            std::cerr << "Failed to create text surface! TTF Error: " << TTF_GetError() << std::endl;
        }
        TTF_CloseFont(ledFont);
    }
    else {
        std::cerr << "Failed to load LED font! TTF Error: " << TTF_GetError() << std::endl;
    }
}

void screen::renderGreenLED(SDL_Renderer* renderer, int ledX, int ledY, bool var) {
    // Draw the black border for the LED
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black border color
    SDL_Rect borderRect = { ledX - 2, ledY - 2, 54, 54 }; // Slightly larger rectangle for border
    SDL_RenderFillRect(renderer, &borderRect);

    // Set LED color to a softer green when active, dimmed green when inactive
    if (var) {
        SDL_SetRenderDrawColor(renderer, 30, 180, 0, 255); // Warmer green color
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 50, 0, 255); // Dimmed green for inactive LED
    }

    // LED as filled rectangle inside the border
    SDL_Rect ledRect = { ledX, ledY, 50, 50 };
    SDL_RenderFillRect(renderer, &ledRect);
}

