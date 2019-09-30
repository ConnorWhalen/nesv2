//
// NES-V2: KeyboardInput.h
//
// Created by Connor Whalen on 2019-05-18.
//

#ifndef NESV2_KEYBOARDINPUT_H
#define NESV2_KEYBOARDINPUT_H

#include "Input.h"

class KeyboardInput : public Input {
public:
    KeyboardInput(SDL_Keycode key_for_a, SDL_Keycode key_for_b, SDL_Keycode key_for_up, SDL_Keycode key_for_down,
                  SDL_Keycode key_for_left, SDL_Keycode key_for_right, SDL_Keycode key_for_start,
                  SDL_Keycode key_for_select);
    InputData Read() override;
    void Apply(SDL_Event e) override;
private:
    InputData current = 0x00;

    SDL_Keycode key_for_a;
    SDL_Keycode key_for_b;
    SDL_Keycode key_for_up;
    SDL_Keycode key_for_down;
    SDL_Keycode key_for_left;
    SDL_Keycode key_for_right;
    SDL_Keycode key_for_start;
    SDL_Keycode key_for_select;
};

#endif //NESV2_KEYBOARDINPUT_H
