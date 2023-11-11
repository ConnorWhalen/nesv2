//
// NES-V2: PartUtilities.cpp
//
// Created by Connor Whalen on 2021-07-12.
//

#include "PartUtilities.h"


namespace PartUtilities {

void serializeBytes(std::stringstream &stream, const nes_byte bytes[], int size){
    int lines = size/LINE_WIDTH;
    for (int i = 0; i < lines; i++) {
        stream << std::setfill('0') << std::setw(4) << std::hex << i*LINE_WIDTH << ": ";
        for (int j = 0; j < LINE_WIDTH; j++) {
            stream << std::setfill('0') << std::setw(2) << std::hex << int(bytes[i*LINE_WIDTH + j]) << " ";
        }
        stream << "\n";
    }
}

}
