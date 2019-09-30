//
// NES-V2: WindowArea.h
//
// Created by Connor Whalen on 2019-09-29.
//

#ifndef NESV2_WINDOWAREA_H
#define NESV2_WINDOWAREA_H

#include <SDL2/SDL.h>

//SDL_Rect* ConstructSDLRect(int w, int h, int x, int y) {
//    auto rect = new SDL_Rect;
//    rect->w = w;
//    rect->h = h;
//    rect->x = x;
//    rect->y = y;
//    return rect;
//}

class WindowArea {
public:
    static SDL_Rect* ConstructSDLRect(int w, int h, int x, int y) {
        auto rect = new SDL_Rect;
        rect->w = w;
        rect->h = h;
        rect->x = x;
        rect->y = y;
        return rect;
    }

    virtual void Render(SDL_Renderer* renderer) = 0;
};

#endif //NESV2_WINDOWAREA_H
