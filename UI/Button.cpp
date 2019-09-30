//
// NES-V2: Button.cpp
//
// Created by Connor Whalen on 2019-09-29.
//

#include "Button.h"

#include <SDL2_image/SDL_image.h>

Button::Button(SDL_Rect* buttonRect, SDL_Renderer* renderer,
               const std::string& onButtonImagePath, const std::string& offButtonImagePath, Input* input,
               InputData buttonMask) {
    SDL_Surface* loadedSurface = IMG_Load(onButtonImagePath.c_str());
    onTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface( loadedSurface );
    loadedSurface = IMG_Load(offButtonImagePath.c_str());
    offTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface( loadedSurface );

    this->buttonRect = buttonRect;
    this->renderRect = new SDL_Rect();
    this->renderRect->w = this->buttonRect->w*2;
    this->renderRect->h = this->buttonRect->h*2;
    this->renderRect->x = this->buttonRect->x*2;
    this->renderRect->y = this->buttonRect->y*2;
    this->input = input;
    this->buttonMask = buttonMask;
}

void Button::Render(SDL_Renderer* renderer) {
    SDL_Texture* buttonTexture;
    if (input->Read() & buttonMask) {
        buttonTexture = onTexture;
    } else {
        buttonTexture = offTexture;
    }
    SDL_RenderCopy(renderer, buttonTexture, nullptr, renderRect);
}

