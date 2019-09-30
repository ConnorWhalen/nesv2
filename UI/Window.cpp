//
// NES-V2: Window.cpp
//
// Created by Connor Whalen on 2019-05-20.
//

#include "Window.h"

Window::Window(int width, int height, std::vector<WindowArea*>* windowAreas) {
    this->window = SDL_CreateWindow("SDL WINDOW", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(this->window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    }
    this->width = width;
    this->height = height;
    this->windowAreas = windowAreas;
}

Window::Window(int width, int height) {
    this->window = SDL_CreateWindow("SDL WINDOW", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(this->window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    }
    this->width = width;
    this->height = height;
    this->windowAreas = new std::vector<WindowArea*>();
}

void Window::Render() {
    for (WindowArea* w : *windowAreas) {
        w->Render(renderer);
    }
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}

void Window::Destroy() {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;
}

void Window::AddWindowArea(WindowArea* windowArea) {
    this->windowAreas->push_back(windowArea);
}

