//
// NES-V2: PartUtilities.h
//
// Created by Connor Whalen on 2021-07-12.
//

#ifndef NESV2_PARTUTILITIES_H
#define NESV2_PARTUTILITIES_H

#include <iomanip>
#include <sstream>
#include <string>

#include <SDL2/SDL.h>

#include "Part.h"

namespace PartUtilities {

constexpr int LINE_WIDTH = 0x20;

void serializeBytes(std::stringstream &stream, const nes_byte bytes[], int size);
void serializeUint32s(std::stringstream &stream, const Uint32 ints[], int size);

}

#endif //NESV2_PARTUTILITIES_H
