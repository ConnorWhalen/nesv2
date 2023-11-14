//
// NES-V2: NullDisplay.cpp
//
// Created by Connor Whalen on 2022-07-05.
//
#include "NullDisplay.h"

#include "Colour.h"

NullDisplay::NullDisplay() : Display(false) {
    pixels = new Uint32[NES_WIDTH*NES_HEIGHT];
    for (int i = 0; i < NES_WIDTH*NES_HEIGHT; i++) pixels[i] = 0;
	blueEmphasis = false;
	greenEmphasis = false;
	redEmphasis = false;

    this->pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
}

void NullDisplay::SetPixelFormat(SDL_Texture *gameTexture) {

}

void NullDisplay::SetColourEmphasis(bool r, bool g, bool b) {
	blueEmphasis = b;
	greenEmphasis = g;
	redEmphasis = r;
}

void NullDisplay::SetPixel(int x, int y, unsigned char colourValue) {
	if (x >= NES_WIDTH || y >= NES_HEIGHT){
		std::cout<< "Pixel index " << x << ", " << y << " (hex) out of range." << "\n";
	}
	unsigned char r = Colour::red(colourValue);
	unsigned char g = Colour::green(colourValue);
	unsigned char b = Colour::blue(colourValue);
	Colour::tint(r, g, b, redEmphasis, greenEmphasis, blueEmphasis);
	pixels[y*NES_WIDTH+x] = SDL_MapRGB(pixelFormat, r, g, b);
}

Uint32* NullDisplay::GetPixels() {
	return pixels;
}
