//
// NES-V2: Button.h
//
// Created by Connor Whalen on 2019-09-29.
//

#ifndef NESV2_BUTTON_H
#define NESV2_BUTTON_H

#include <string>

#include "../input/Input.h"
#include "WindowArea.h"

class Button : public WindowArea {
public:
    Button(SDL_Rect* buttonRect, SDL_Renderer* renderer,
           const std::string& onButtonImagePath, const std::string& offButtonImagePath, Input* input,
           InputData buttonMask);
protected:
    void Render(SDL_Renderer* renderer) override;
private:
    SDL_Texture* offTexture;
    SDL_Texture* onTexture;
    SDL_Rect* buttonRect;
    SDL_Rect* renderRect;
    Input* input;
    InputData buttonMask;
};

#endif //NESV2_BUTTON_H
