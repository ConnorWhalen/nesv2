//
// NES-V2: Window.h
//
// Created by Connor Whalen on 2019-05-20.
//

#ifndef NESV2_WINDOW_H
#define NESV2_WINDOW_H

#include <vector>

#include <SDL2/SDL.h>

#include "WindowArea.h"

class Window {
public:
    Window(int width, int height, std::vector<WindowArea*>* windowAreas);
    Window(int width, int height);
    void Render();
    void Destroy();
    SDL_Renderer* GetRenderer() {return renderer;};
    void AddWindowArea(WindowArea* windowArea);
protected:
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<WindowArea*>* windowAreas;
};

#endif //NESV2_WINDOW_H
