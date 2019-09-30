//
// NES-V2: GameArea.h
//
// Created by Connor Whalen on 2019-05-20.
//

#ifndef NESV2_GAMEWINDOW_H
#define NESV2_GAMEWINDOW_H

#include "WindowArea.h"

class GameArea : public WindowArea {
public:
    GameArea(SDL_Rect* gamerect, SDL_Renderer* renderer);
    void Flip(const Uint32 gamePixels[], int gameWidth, int gameHeight);

    SDL_Texture *GetTexture();
protected:
    void Render(SDL_Renderer* renderer) override;
private:
    SDL_Texture* gameTexture;
    SDL_Rect* gameRect;
    SDL_Rect* renderRect;
    Uint32 *pixels;
};

#endif //NESV2_GAMEWINDOW_H
