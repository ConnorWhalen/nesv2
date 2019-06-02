//
// NES-V2: Input.h
//
// Created by Connor Whalen on 2019-05-18.
//

#ifndef NESV2_INPUT_H
#define NESV2_INPUT_H

#include <SDL2/SDL.h>

typedef unsigned char InputData;

constexpr InputData A_BUTTON = 0x01;
constexpr InputData B_BUTTON = 0x02;
constexpr InputData UP_BUTTON = 0x80;
constexpr InputData DOWN_BUTTON = 0x20;
constexpr InputData LEFT_BUTTON = 0x40;
constexpr InputData RIGHT_BUTTON = 0x10;
constexpr InputData START_BUTTON = 0x08;
constexpr InputData SELECT_BUTTON = 0x04;

class Input {
public:
    virtual InputData Read() = 0;
    virtual void Apply(SDL_Event e) = 0;
};

#endif //NESV2_INPUT_H
