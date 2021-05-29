//
// NES-V2: Display.h
//
// Created by Connor Whalen on 2021-05-29.
//

#ifndef NESV2_DISPLAY_H
#define NESV2_DISPLAY_H

#include <SDL2/SDL.h>
#include "OutputData.h"
#include "Part.h"

class Display : public Part {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };

    Display();

    void SetPixelFormat(SDL_Texture *gameTexture);
    void SetPixel(int x, int y, unsigned char colourValue);
    Uint32* GetPixels();

private:
    Uint32* pixels;
    SDL_PixelFormat *pixelFormat;
    bool blueEmphasis;
    bool greenEmphasis;
    bool redEmphasis;
};

#endif //NESV2_DISPLAY_H
