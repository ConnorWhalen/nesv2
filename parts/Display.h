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
    virtual std::vector<OutputData>* Serialize() override;

    Display(bool debugOutput);

    virtual void SetPixelFormat(SDL_Texture *gameTexture);
    virtual void SetColourEmphasis(bool r, bool g, bool b);
    virtual void SetPixel(int x, int y, unsigned char colourValue);
    virtual Uint32* GetPixels();

private:
    Uint32* pixels;
    SDL_PixelFormat *pixelFormat;
    bool blueEmphasis;
    bool greenEmphasis;
    bool redEmphasis;

    bool debugOutput;
};

#endif //NESV2_DISPLAY_H
