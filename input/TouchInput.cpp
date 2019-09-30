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

int TouchInput::GetFingerIndex(const long finger_id, float x, float y) {
    int index_to_use = -1;
    for (int i = 0; i < 10; i++) {
        if (finger_id_map[i] == finger_id ||
                (x-fingers[i*2] < 0.05 &&
                 x-fingers[i*2] > -0.05 &&
                 y-fingers[i*2+1] < 0.05 &&
                 y-fingers[i*2+1] > -0.05)) {
            index_to_use = i;
        }
    }
    if (index_to_use == -1) {
        finger_id_map[last_zero] = finger_id;
        index_to_use = last_zero++;
        if (last_zero == FINGER_LIMIT) last_zero = 0;
    }
    return index_to_use;
}

void TouchInput::UpdateCurrent() {
    this->current = 0;
    for (int i = 0; i < FINGER_LIMIT; i++) {
        if (Intersecting(this->rect_for_a, fingers[i*2], fingers[i*2+1])) {
            this->current |= A_BUTTON;
        } else if (Intersecting(this->rect_for_b, fingers[i*2], fingers[i*2+1])) {
            this->current |= B_BUTTON;
        } else if (Intersecting(this->rect_for_up, fingers[i*2], fingers[i*2+1])) {
            this->current |= UP_BUTTON;
        } else if (Intersecting(this->rect_for_down, fingers[i*2], fingers[i*2+1])) {
            this->current |= DOWN_BUTTON;
        } else if (Intersecting(this->rect_for_left, fingers[i*2], fingers[i*2+1])) {
            this->current |= LEFT_BUTTON;
        } else if (Intersecting(this->rect_for_right, fingers[i*2], fingers[i*2+1])) {
            this->current |= RIGHT_BUTTON;
        } else if (Intersecting(this->rect_for_start, fingers[i*2], fingers[i*2+1])) {
            this->current |= START_BUTTON;
        } else if (Intersecting(this->rect_for_select, fingers[i*2], fingers[i*2+1])) {
            this->current |= SELECT_BUTTON;
        }
    }
}

void TouchInput::Apply(const SDL_Event e) {
    int index_to_use = this->GetFingerIndex(e.tfinger.fingerId, e.tfinger.x, e.tfinger.y);
    if (e.type == SDL_FINGERDOWN) {
        fingers[index_to_use*2] = e.tfinger.x;
        fingers[index_to_use*2+1] = e.tfinger.y;
    } else if (e.type == SDL_FINGERUP) {
        fingers[index_to_use*2] = 0;
        fingers[index_to_use*2+1] = 0;
    } else if (e.type == SDL_FINGERMOTION) {
        fingers[index_to_use*2] = e.tfinger.x;
        fingers[index_to_use*2+1] = e.tfinger.y;
    }
    this->UpdateCurrent();
}
