//
// NES-V2: KeyboardInput.cpp
//
// Created by Connor Whalen on 2019-05-18.
//

#include "KeyboardInput.h"

KeyboardInput::KeyboardInput(SDL_Keycode key_for_a, SDL_Keycode key_for_b, SDL_Keycode key_for_up,
                             SDL_Keycode key_for_down, SDL_Keycode key_for_left, SDL_Keycode key_for_right,
                             SDL_Keycode key_for_start, SDL_Keycode key_for_select) {
    this->key_for_a = key_for_a;
    this->key_for_b = key_for_b;
    this->key_for_up = key_for_up;
    this->key_for_down = key_for_down;
    this->key_for_left = key_for_left;
    this->key_for_right = key_for_right;
    this->key_for_start = key_for_start;
    this->key_for_select = key_for_select;
}

InputData KeyboardInput::Read() {
    return this->current;
}

void KeyboardInput::Apply(const SDL_Event e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == this->key_for_a) {
            this->current |= A_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_b) {
            this->current |= B_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_up) {
            this->current |= UP_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_down) {
            this->current |= DOWN_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_left) {
            this->current |= LEFT_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_right) {
            this->current |= RIGHT_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_start) {
            this->current |= START_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_select) {
            this->current |= SELECT_BUTTON;
        }
    } else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == this->key_for_a) {
            this->current &= ~A_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_b) {
            this->current &= ~B_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_up) {
            this->current &= ~UP_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_down) {
            this->current &= ~DOWN_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_left) {
            this->current &= ~LEFT_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_right) {
            this->current &= ~RIGHT_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_start) {
            this->current &= ~START_BUTTON;
        } else if (e.key.keysym.sym == this->key_for_select) {
            this->current &= ~SELECT_BUTTON;
        }
    }
}
