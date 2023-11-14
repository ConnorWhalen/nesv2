//
// NES-V2: NullDisplay.h
//
// Created by Connor Whalen on 2022-07-05.
//

#ifndef NESV2_NULLDISPLAY_H
#define NESV2_NULLDISPLAY_H

#include "Display.h"
#include "OutputData.h"


class NullDisplay : public Display {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };

    NullDisplay();

    void SetPixelFormat(SDL_Texture *gameTexture) override;
    void SetColourEmphasis(bool r, bool g, bool b) override;
    void SetPixel(int x, int y, unsigned char colourValue) override;
    Uint32* GetPixels() override;
private:
    Uint32* pixels;
    SDL_PixelFormat *pixelFormat;
    bool blueEmphasis;
    bool greenEmphasis;
    bool redEmphasis;

    
};

#endif //NESV2_NULLDISPLAY_H
