//
// NES-V2: TouchInput.h
//
// Created by Connor Whalen on 2019-05-28.
//

#ifndef NESV2_TOUCHINPUT_H
#define NESV2_TOUCHINPUT_H

#include "Input.h"

constexpr int FINGER_LIMIT = 10;

struct TouchRect {
    float top;
    float left;
    float height;
    float width;
};

class TouchInput : public Input {
public:
    TouchInput(TouchRect rect_for_a, TouchRect rect_for_b, TouchRect rect_for_up, TouchRect rect_for_down,
               TouchRect rect_for_left, TouchRect rect_for_right, TouchRect rect_for_start,
               TouchRect rect_for_select);
    InputData Read() override;
    void Apply(SDL_Event e) override;

    long finger_id_map[FINGER_LIMIT];
    float fingers[FINGER_LIMIT*2];
private:
    static bool Intersecting(TouchRect, float x, float y);

    InputData current = 0x00;

    TouchRect rect_for_a;
    TouchRect rect_for_b;
    TouchRect rect_for_up;
    TouchRect rect_for_down;
    TouchRect rect_for_left;
    TouchRect rect_for_right;
    TouchRect rect_for_start;
    TouchRect rect_for_select;

    int last_zero;
};

#endif //NESV2_TOUCHINPUT_H
