//
// NES-V2: TextWindow.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_TEXTWINDOW_H
#define NESV2_TEXTWINDOW_H

#include "TextArea.h"
#include "Window.h"

class TextWindow : public Window {
public:
    TextWindow(int width, int height);

    void AddText(const std::string &text, const SDL_Color &color);
    void NewLine();
    void Offset(const int &x, const int &y);
    void Clear();
private:
    Window InitializeWithTextArea(int width, int height);

    TextArea* textArea;
};

#endif //NESV2_TEXTWINDOW_H
