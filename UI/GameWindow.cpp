//
// NES-V2: GameWindow.cpp
//
// Created by Connor Whalen on 2019-05-20.
//

#include "GameWindow.h"

GameWindow::GameWindow(int width, int height)
    : Window(width, height) {
    gameTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
    );
    this->pixels = new Uint32[width*height];
}

void GameWindow::RenderChild() {
    SDL_UpdateTexture(gameTexture, NULL, pixels, width * sizeof (Uint32));
    SDL_RenderCopy(renderer, gameTexture, NULL, NULL);
}

void GameWindow::Flip(const Uint32 gamePixels[], int gameWidth, int gameHeight) {
    double widthRatio = (double) gameWidth/width;
    double heightRatio = (double) gameHeight/height;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            this->pixels[i*width+j] = gamePixels[((int)(i*heightRatio)*gameWidth+(int)(j*widthRatio))];
        }
    }
}

SDL_Texture* GameWindow::GetTexture() {
    return this->gameTexture;
}
