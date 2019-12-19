//
// NES-V2: cpu_test_utilities.cpp
//
// Created by Connor Whalen on 2019-12-08.
//

#include "cpu_test_utilities.h"

#include <iomanip>
#include <sstream>

std::string getRegisterString(nes_byte A, nes_byte X, nes_byte Y, nes_byte S, nes_byte P, nes_address PC) {
    std::stringstream registerStream;
    registerStream << std::setfill('0') << std::setw(2) << std::hex << int(A) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(X) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(Y) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(S) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(P) << " "
                   << std::setfill('0') << std::setw(4) << std::hex << int(PC);
    return registerStream.str();
}

std::string getRamString(nes_byte ram[]) {
    std::stringstream ramStream;
    int ram_line_width = 0x20;
    int ram_lines = RAM_SIZE/ram_line_width;
    for (int i = 0; i < ram_lines; i++) {
        ramStream << std::setfill('0') << std::setw(4) << std::hex << i*ram_line_width << ": ";
        for (int j = 0; j < ram_line_width; j++) {
            ramStream << std::setfill('0') << std::setw(2) << std::hex << int(ram[i*ram_line_width + j]) << " ";
        }
        ramStream << "\n";
    }
    return ramStream.str();
}

void setResetVector(std::vector<nes_byte>* romBytes, nes_address value) {
    romBytes->at(RESET_VECTOR_HIGH-CARTRIDGE_RAM_REGION_END) = (nes_byte) (value >> 8);
    romBytes->at(RESET_VECTOR_LOW-CARTRIDGE_RAM_REGION_END) = (nes_byte) (value & 0xFF);
}

void relativeRomWrite(std::vector<nes_byte>* romBytes, nes_address address, nes_byte value) {
    romBytes->at(address - CARTRIDGE_RAM_REGION_END) = value;
}
