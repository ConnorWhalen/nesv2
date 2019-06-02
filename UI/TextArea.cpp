//
// NES-V2: TextArea.cpp
//
// Created by Connor Whalen on 2019-04-20.
//

#include "TextArea.h"

#include <sstream>

TextArea::TextArea() {
    this->messages = new std::vector<SDL_Texture*>();
    this->rects = new std::vector<SDL_Rect>();
    this->lineTop = 0;
    this->lineLeft = 0;
    this->lineBottom = 0;
    this->offsetX = 0;
    this->offsetY = 0;

    if (TTF_Init() == -1) {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }

    this->font = font = TTF_OpenFont("/Library/Fonts/Courier New.ttf", FONT_SIZE);
    if (font == NULL) {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    }
}

void TextArea::AddText(const std::string &text, const SDL_Color &color, SDL_Renderer* renderer) {
    std::stringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, line.c_str(), color);

        messages->push_back(SDL_CreateTextureFromSurface(renderer, surfaceMessage));

        SDL_Rect rect;
        rect.w = surfaceMessage->w;
        rect.h = surfaceMessage->h;
        rect.x = lineLeft;
        rect.y = lineTop;
        if (lineTop + surfaceMessage->h > lineBottom) {
            lineBottom = lineTop + surfaceMessage->h;
        }
        lineLeft = lineLeft + surfaceMessage->w;

        rects->push_back(rect);

        SDL_FreeSurface(surfaceMessage);
        this->NewLine();
    }
}

void TextArea::NewLine() {
    lineLeft = 0;
    lineTop = lineBottom;
}

void TextArea::Offset(const int &offsetX, const int &offsetY) {
    this->offsetX += offsetX *2;
    this->offsetY += offsetY *2;
    if (this->offsetX > 0) {
        this->offsetX = 0;
    }
    if (this->offsetY > 0) {
        this->offsetY = 0;
    }
}

void TextArea::Render(SDL_Renderer* renderer) {
    for (int i = 0; i < messages->size(); i++) {
        SDL_Rect offsetRect = rects->at(i);
        offsetRect.x += offsetX;
        offsetRect.y += offsetY;
        SDL_RenderCopy(renderer, messages->at(i), NULL, &offsetRect);
    }
}

void TextArea::Clear() {
    delete this->messages;
    delete this->rects;
    this->messages = new std::vector<SDL_Texture*>();
    this->rects = new std::vector<SDL_Rect>();
    this->lineTop = 0;
    this->lineLeft = 0;
    this->lineBottom = 0;
}

