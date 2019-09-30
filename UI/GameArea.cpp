//
// NES-V2: GameArea.cpp
//
// Created by Connor Whalen on 2019-05-20.
//

#include "GameArea.h"

GameArea::GameArea(SDL_Rect* gameRect, SDL_Renderer* renderer) {
    gameTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            gameRect->w,
            gameRect->h
    );
    this->pixels = new Uint32[gameRect->w*gameRect->h];
    this->gameRect = gameRect;
    this->renderRect = new SDL_Rect();
    this->renderRect->w = gameRect->w*2;
    this->renderRect->h = gameRect->h*2;
    this->renderRect->x = gameRect->x*2;
    this->renderRect->y = gameRect->y*2;
}

void GameArea::Render(SDL_Renderer* renderer) {
    // SDL_UpdateTexture(gameTexture, this->gameRect, pixels, this->gameRect->w * sizeof(Uint32));
    Uint32 *texturePixels = nullptr;
    int texturePitch;
    SDL_LockTexture(gameTexture, nullptr, (void**)&texturePixels, &texturePitch);
    int v_offset = 0; // gameRect->w*gameRect->y;
    int h_offset = 0; // (windowWidth-gameRect->w)/2;
    for (int i = 0; i < this->gameRect->h; i++) {
        for (int j = 0; j < this->gameRect->w; j++) {
            texturePixels[i*gameRect->w+j+v_offset+h_offset] = pixels[i*gameRect->w+j];
        }
        // h_offset += windowWidth-gameRect->w;
    }
    SDL_UnlockTexture(gameTexture);
    SDL_RenderCopy(renderer, gameTexture, nullptr, this->renderRect);
}

void GameArea::Flip(const Uint32 gamePixels[], int pixelsWidth, int pixelsHeight) {
    double widthRatio = (double) pixelsWidth/gameRect->w;
    double heightRatio = (double) pixelsHeight/gameRect->h;
    for (int i = 0; i < gameRect->h; i++) {
        for (int j = 0; j < gameRect->w; j++) {
            this->pixels[i*gameRect->w+j] = gamePixels[((int)(i*heightRatio)*pixelsWidth+(int)(j*widthRatio))];
        }
    }
}

SDL_Texture* GameArea::GetTexture() {
    return this->gameTexture;
}
