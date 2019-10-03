//
// NES-V2: M0.cpp
//
// Created by Connor Whalen on 2019-09-29.
//

#include "M2.h"

#include <iomanip>
#include <sstream>

M2::M2(std::vector<unsigned char> *romBytes) {
    for (int i = 0; i < MAPPER2_SIZE; i++) {
        bytes[i] = (*romBytes)[i];
    }
    this->bank = 0;
}

std::vector<OutputData>* M2::Serialize() {
    OutputData outputData;
    outputData.title = "MAPPER 2 BANK NUMBER";
    std::stringstream ss;
    ss << std::hex << this->bank;
    outputData.body = ss.str();
    auto outputDatas = new std::vector<OutputData>();
    outputDatas->push_back(outputData);
    return outputDatas;
}

nes_byte M2::DoRead(nes_address address) {
    if (address < MAPPER_REGION_MIDDLE) {
        return bytes[address+MAPPER2_BANK_SIZE*(this->bank-2)];
    } else {
        return this->bytes[address+0x10000];
    }
}

void M2::DoWrite(nes_address address, nes_byte value) {
    if (address > 7) {
        printf("WARNING: WROTE TO MAPPER 2: Addr %d Value %d\n", address, value);
        printf("TAKING MODULUS %d\n", value%8);
    }
    this->bank = value%8;
}