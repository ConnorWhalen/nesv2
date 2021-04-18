//
// NES-V2: cpu_test_utilities.h
//
// Created by Connor Whalen on 2019-12-07.
//

#ifndef NESV2_CPU_TEST_UTILITIES_H
#define NESV2_CPU_TEST_UTILITIES_H

#include <string>

#include "../../parts/CPU.h"

std::string getRegisterString(nes_byte A, nes_byte X, nes_byte Y, nes_byte S, nes_byte P, nes_address PC);
std::string getRamString(nes_byte ram[]);
void setResetVector(std::vector<nes_byte>* romBytes, nes_address value);
void setIRQVector(std::vector<nes_byte>* romBytes, nes_address value);
void relativeRomWrite(std::vector<nes_byte>* romBytes, nes_address address, nes_byte value);

#endif //NESV2_CPU_TEST_UTILITIES_H
