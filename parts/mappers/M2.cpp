//
// NES-V2: M0.cpp
//
// Created by Connor Whalen on 2019-09-29.
//

#include "M2.h"

#include <iomanip>
#include <sstream>

M2::M2(std::vector<unsigned char> *romBytes, std::vector<unsigned char> *chrBytes) {
    for (int i = 0; i < MAPPER2_SIZE; i++) {
        prgBytes[i] = (*romBytes)[i];
    }
    if (chrBytes->size() > 0) {
        for (int i = 0; i < MAPPER_CHR_REGION_SIZE; i++) {
            chr[i] = (*chrBytes)[i % chrBytes->size()];
        }
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
    if (address < MAPPER_ROM_REGION_START) {
        printf("ERROR: INVALID MAPPER 2 READ ADDRESS %x\n", address);
        return 0;
    }
    else if (address < MAPPER_ROM_REGION_MIDDLE) {
        return prgBytes[address+MAPPER2_BANK_SIZE*(this->bank-2)];
    } else {
        return prgBytes[address+0x10000];
    }
}

void M2::DoWrite(nes_address address, nes_byte value) {
    this->bank = value%8;
}

nes_byte M2::DoChrRead(nes_address address) {
    return chr[address];
}

void M2::DoChrWrite(nes_address address, nes_byte value) {
    chr[address] = value;
}