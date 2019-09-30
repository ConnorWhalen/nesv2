//
// NES-V2: TextArea.cpp
//
// Created by Connor Whalen on 2019-04-20.
//

#include "TextArea.h"

#include <sstream>

TextArea::TextArea(int width, int height) {
    this->windowWidth = width;
    this->windowHeight = height;
    this->oldMessages = new std::vector<SDL_Texture*>();
    this->oldRects = new std::vector<SDL_Rect>();
    this->lines = new std::vector<std::string>();
    this->lineTop = 0;
    this->lineLeft = 0;
    this->lineBottom = 0;
    this->offsetX = 0;
    this->offsetY = 0;
    this->messageIndex = 0;

    if (TTF_Init() == -1) {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }

    this->font = font = TTF_OpenFont("/Library/Fonts/Courier New.ttf", FONT_SIZE);
    if (font == nullptr) {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    }
}

void TextArea::AddText(const std::string &text, const SDL_Color &color, SDL_Renderer* renderer) {
    std::stringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (messageIndex == lines->size()) {
            SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, line.c_str(), color);

            SDL_Rect rect;
            rect.w = surfaceMessage->w;
            rect.h = surfaceMessage->h;
            rect.x = lineLeft;
            rect.y = lineTop;
            if (lineTop + surfaceMessage->h > lineBottom) {
                lineBottom = lineTop + surfaceMessage->h;
            }
            lineLeft = lineLeft + surfaceMessage->w;

            oldRects->push_back(rect);
            auto message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            oldMessages->push_back(message);
            lines->push_back(line);

            SDL_FreeSurface(surfaceMessage);
        } else if (line != (*lines)[messageIndex]) {
            SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, line.c_str(), color);

            (*oldRects)[messageIndex].w = surfaceMessage->w;

            auto message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            (*oldMessages)[messageIndex] = message;
            (*lines)[messageIndex] = line;

            SDL_FreeSurface(surfaceMessage);
        }
        this->NewLine();
        messageIndex++;
    }
}

void TextArea::NewLine() {
    lineLeft = 0;
    lineTop = lineBottom;
}

void TextArea::Offset(const int &offsetX, const int &offsetY) {
//    this->offsetX += offsetX *2;
    this->offsetY += offsetY *20;
    if (this->offsetX > 0) {
        this->offsetX = 0;
    }
    if (this->offsetY > 0) {
        this->offsetY = 0;
    }
}

void TextArea::Render(SDL_Renderer* renderer) {
    SDL_RenderClear(renderer);
    for (int i = 0; i < oldMessages->size(); i++) {
        if ((*oldRects)[i].y+offsetY >= 0 &&
                (*oldRects)[i].y+(*oldRects)[i].h+offsetY < windowHeight*2) {
            SDL_Rect offsetRect = oldRects->at(i);
            offsetRect.x += offsetX;
            offsetRect.y += offsetY;
            SDL_RenderCopy(renderer, oldMessages->at(i), nullptr, &offsetRect);
            SDL_RenderCopy(renderer, oldMessages->at(i), nullptr, &offsetRect);
        }
    }
}

void TextArea::Clear() {
    this->lineTop = 0;
    this->lineLeft = 0;
    this->lineBottom = 0;
    this->messageIndex = 0;
}

