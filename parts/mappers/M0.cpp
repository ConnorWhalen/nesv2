//
// NES-V2: M0.cpp
//
// Created by Connor Whalen on 2019-09-29.
//

#include "M0.h"

#include <iomanip>
#include <sstream>

M0::M0(std::vector<unsigned char> *romBytes) {
    for (int i = 0; i < MAPPER0_SIZE; i++) {
        bytes[i] = (*romBytes)[i];
    }
    int cols = 32;
    int rows = MAPPER0_SIZE/cols/32;
    std::stringstream stream;
    stream << std::hex;
    for (int i = 0; i < rows; i++) {
        stream << std::setw(4) << std::setfill('0') << i*cols << ": ";
        for (int j = 0; j < cols; j++) {
            stream << std::setw(2) << std::setfill('0') << (int) bytes[i*cols+j] << " ";
        }
        stream << "\n";
    }
    this->serialized = stream.str();
}

std::vector<OutputData>* M0::Serialize() {
    OutputData outputData;
    outputData.title = "MAPPER 0 ROM DUMP";
    outputData.body = serialized;
    auto outputDatas = new std::vector<OutputData>();
    outputDatas->push_back(outputData);
    return outputDatas;
}

nes_byte M0::DoRead(nes_address address) {
    if (address < MAPPER0_SIZE) {
        printf("ERROR: INVALID MAPPER READ ADDRESS %x\n", address);
    }
    return bytes[address-MAPPER0_SIZE];
}

void M0::DoWrite(nes_address address, nes_byte value) {
    printf("WARNING: WROTE TO MAPPER 0: Addr %x Value %x\n", address, value);
}
