//
// NES-V2: mapper_test_utilities.h
//
// Created by Connor Whalen on 2019-12-08.
//

#ifndef NESV2_MAPPER_TEST_UTILITIES_H
#define NESV2_MAPPER_TEST_UTILITIES_H

#include <string>
#include <vector>

#include "../parts/mappers/M0.h"

std::string getM0String(std::vector<nes_byte>* romBytes);

#endif //NESV2_MAPPER_TEST_UTILITIES_H
