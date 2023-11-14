//
// NES-V2: ppu_test_utilities.h
//
// Created by Connor Whalen on 2021-07-08.
//

#ifndef NESV2_PPU_TEST_UTILITIES_H
#define NESV2_PPU_TEST_UTILITIES_H

#include <string>

#include "../../parts/CPU.h"

std::string getRegisterString(nes_byte ppuStatus, nes_byte ppuCtrl, nes_byte ppuMask, nes_byte oamAddr, nes_byte ppuScroll, nes_byte ppuData);

void write_ppu_addr(PPU &ppu, nes_address address, bool reading);

#endif //NESV2_PPU_TEST_UTILITIES_H
