//
// NES-V2: mapper_test_utilities.cpp
//
// Created by Connor Whalen on 2019-12-08.
//

#include "mapper_test_utilities.h"

#include <iomanip>
#include <sstream>

std::string getM0String(std::vector<nes_byte>* romBytes) {
    int cols = 32;
    int rows = MAPPER0_SIZE/cols/32;
    std::stringstream stream;
    stream << std::hex;
    for (int i = 0; i < rows; i++) {
        stream << std::setw(4) << std::setfill('0') << i*cols << ": ";
        for (int j = 0; j < cols; j++) {
            stream << std::setw(2) << std::setfill('0') << (int) romBytes->at(i*cols+j) << " ";
        }
        stream << "\n";
    }
    return stream.str();
}
