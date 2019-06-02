//
// NES-V2: TextWindow.cpp
//
// Created by Connor Whalen on 2019-04-21.
//

#include "TextWindow.h"

TextWindow::TextWindow(int width, int height)
    : Window(width, height) {
    this->textArea = new TextArea();
}

void TextWindow::RenderChild() {
    textArea->Render(renderer);
}

void TextWindow::AddText(const std::string &text, const SDL_Color &color) {
    textArea->AddText(text, color, renderer);
}

void TextWindow::NewLine() {
    textArea->NewLine();
}

void TextWindow::Offset(const int &x, const int &y) {
    textArea->Offset(x, y);
}

void TextWindow::Clear() {
    textArea->Clear();
}

