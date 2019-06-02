//
// NES-V2: Window.h
//
// Created by Connor Whalen on 2019-05-20.
//

#ifndef NESV2_WINDOW_H
#define NESV2_WINDOW_H

#include <SDL2/SDL.h>

class Window {
public:
    void Render();
    void Destroy();
protected:
    Window(int width, int height);
    virtual void RenderChild() = 0;

    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif //NESV2_WINDOW_H
