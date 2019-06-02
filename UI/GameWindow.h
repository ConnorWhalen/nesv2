//
// NES-V2: GameWindow.h
//
// Created by Connor Whalen on 2019-05-20.
//

#ifndef NESV2_GAMEWINDOW_H
#define NESV2_GAMEWINDOW_H

#include "Window.h"

class GameWindow : public Window {
public:
    GameWindow(int width, int height);
    void Flip(const Uint32 gamePixels[], int gameWidth, int gameHeight);

    SDL_Texture *GetTexture();
protected:
    void RenderChild() override;
private:
    SDL_Texture* gameTexture;
    Uint32 *pixels;
};

#endif //NESV2_GAMEWINDOW_H
