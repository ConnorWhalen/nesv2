//
// NES-V2: TouchInput.cpp
//
// Created by Connor Whalen on 2019-05-28.
//

#include "TouchInput.h"

bool TouchInput::Intersecting(TouchRect rect, float x, float y) {
    return (y > rect.top  && y < rect.top+rect.height &&
            x > rect.left && x < rect.left+rect.width);
}

TouchInput::TouchInput(TouchRect rect_for_a, TouchRect rect_for_b, TouchRect rect_for_up,
                       TouchRect rect_for_down, TouchRect rect_for_left, TouchRect rect_for_right,
                       TouchRect rect_for_start, TouchRect rect_for_select) {
    this->rect_for_a = rect_for_a;
    this->rect_for_b = rect_for_b;
    this->rect_for_up = rect_for_up;
    this->rect_for_down = rect_for_down;
    this->rect_for_left = rect_for_left;
    this->rect_for_right = rect_for_right;
    this->rect_for_start = rect_for_start;
    this->rect_for_select = rect_for_select;
    this->last_zero = 0;
    this->current = 0;
}

InputData TouchInput::Read() {
    return this->current;
}

void TouchInput::Apply(const SDL_Event e) {
    if (e.type == SDL_FINGERDOWN) {
        if (Intersecting(this->rect_for_a, e.tfinger.x, e.tfinger.y)) {
            this->current |= A_BUTTON;
        } else if (Intersecting(this->rect_for_b, e.tfinger.x, e.tfinger.y)) {
            this->current |= B_BUTTON;
        } else if (Intersecting(this->rect_for_up, e.tfinger.x, e.tfinger.y)) {
            this->current |= UP_BUTTON;
        } else if (Intersecting(this->rect_for_down, e.tfinger.x, e.tfinger.y)) {
            this->current |= DOWN_BUTTON;
        } else if (Intersecting(this->rect_for_left, e.tfinger.x, e.tfinger.y)) {
            this->current |= LEFT_BUTTON;
        } else if (Intersecting(this->rect_for_right, e.tfinger.x, e.tfinger.y)) {
            this->current |= RIGHT_BUTTON;
        } else if (Intersecting(this->rect_for_start, e.tfinger.x, e.tfinger.y)) {
            this->current |= START_BUTTON;
        } else if (Intersecting(this->rect_for_select, e.tfinger.x, e.tfinger.y)) {
            this->current |= SELECT_BUTTON;
        }
        int index_to_use = -1;
        for (int i = 0; i < 10; i++) {
            if (finger_id_map[i] == e.tfinger.fingerId) {
                index_to_use = i;
            }
        }
        if (index_to_use == -1) {
            finger_id_map[last_zero] = e.tfinger.fingerId;
            index_to_use = (last_zero++)%FINGER_LIMIT;
        }
        fingers[index_to_use*2] = e.tfinger.x;
        fingers[index_to_use*2+1] = e.tfinger.y;
    } else if (e.type == SDL_FINGERUP) {
        if (Intersecting(this->rect_for_a, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~A_BUTTON;
        } else if (Intersecting(this->rect_for_b, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~B_BUTTON;
        } else if (Intersecting(this->rect_for_up, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~UP_BUTTON;
        } else if (Intersecting(this->rect_for_down, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~DOWN_BUTTON;
        } else if (Intersecting(this->rect_for_left, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~LEFT_BUTTON;
        } else if (Intersecting(this->rect_for_right, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~RIGHT_BUTTON;
        } else if (Intersecting(this->rect_for_start, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~START_BUTTON;
        } else if (Intersecting(this->rect_for_select, e.tfinger.x, e.tfinger.y)) {
            this->current &= ~SELECT_BUTTON;
        }
        int index_to_use = -1;
        for (int i = 0; i < 10; i++) {
            if (finger_id_map[i] == e.tfinger.fingerId) {
                index_to_use = i;
            }
        }
        if (index_to_use == -1) {
            finger_id_map[last_zero] = e.tfinger.fingerId;
            index_to_use = (last_zero++)%FINGER_LIMIT;
        }
        fingers[index_to_use*2] = 0;
        fingers[index_to_use*2+1] = 0;
    }
}
