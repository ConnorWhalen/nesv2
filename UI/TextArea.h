//
// NES-V2: TextArea.h
//
// Created by Connor Whalen on 2019-04-20.
//

#ifndef NESV2_TEXTAREA_H
#define NESV2_TEXTAREA_H

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

constexpr int FONT_SIZE = 24;

class TextArea {
public:
    TextArea();
    void AddText(const std::string &text, const SDL_Color &color, SDL_Renderer* renderer);
    void NewLine();
    void Offset(const int &offsetX, const int &offsetY);
    void Render(SDL_Renderer* renderer);
    void Clear();
private:
    std::vector<SDL_Texture*>* messages;
    std::vector<SDL_Rect>* rects;
    int lineTop;
    int lineLeft;
    int lineBottom;
    int offsetX;
    int offsetY;
    TTF_Font* font;
};

#endif //NESV2_TEXTAREA_H
