//
// NES-V2: Display.cpp
//
// Created by Connor Whalen on 2021-05-29.
//

#include "Colour.h"
#include "Display.h"

Display::Display() {
    pixels = new Uint32[NES_WIDTH*NES_HEIGHT];
    for (int i = 0; i < NES_WIDTH*NES_HEIGHT; i++) pixels[i] = 0;
	blueEmphasis = false;
	greenEmphasis = false;
	redEmphasis = false;
}

void Display::SetPixelFormat(SDL_Texture *gameTexture) {
	Uint32 pixelFormatCode;
	SDL_QueryTexture(gameTexture, &pixelFormatCode, nullptr, nullptr, nullptr);
	this->pixelFormat = SDL_AllocFormat(pixelFormatCode);
}

void Display::SetColourEmphasis(bool r, bool g, bool b) {
	blueEmphasis = b;
	greenEmphasis = g;
	redEmphasis = r;
}

void Display::SetPixel(int x, int y, unsigned char colourValue){
	if (x >= NES_WIDTH || y >= NES_HEIGHT){
		std::cout<< "Pixel index " << x << ", " << y << " (hex) out of range." << "\n";
	}
	unsigned char r = Colour::red(colourValue);
	unsigned char g = Colour::green(colourValue);
	unsigned char b = Colour::blue(colourValue);
	Colour::tint(r, g, b, redEmphasis, greenEmphasis, blueEmphasis);
	pixels[y*NES_WIDTH+x] = SDL_MapRGB(pixelFormat, r, g, b);
}

Uint32* Display::GetPixels() {
	return pixels;
}
