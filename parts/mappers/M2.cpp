//
// NES-V2: M0.cpp
//
// Created by Connor Whalen on 2019-09-29.
//

#include "M2.h"

#include "../PartUtilities.h"

#include <iomanip>
#include <sstream>

M2::M2(std::vector<unsigned char> *romBytes, std::vector<unsigned char> *chrBytes, bool debugOutput) {
    for (int i = 0; i < MAPPER2_SIZE; i++) {
        prgBytes[i] = (*romBytes)[i];
    }
    if (chrBytes->size() > 0) {
        for (int i = 0; i < MAPPER_CHR_REGION_SIZE; i++) {
            chr[i] = (*chrBytes)[i % chrBytes->size()];
        }
    }
    this->bank = 0;

    this->debugOutput = debugOutput;
}

std::vector<OutputData>* M2::Serialize() {
	if (!debugOutput) return new std::vector<OutputData>();

    OutputData outputData;
    outputData.title = "MAPPER 2 BANK NUMBER";
    std::stringstream ss;
    ss << std::hex << this->bank;
    outputData.body = ss.str();

    std::stringstream romDumpStream;
    nes_byte dump[0x1000];
    for (int i = 0; i < 0x100; i++) dump[i] = prgBytes[i + (MAPPER2_BANK_SIZE * 7)];
    PartUtilities::serializeBytes(romDumpStream, dump, 0x100);

    auto outputDatas = new std::vector<OutputData>();
    outputDatas->push_back(outputData);
    outputDatas->push_back({"M2 ROM DUMP", romDumpStream.str()});
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
    // printf("M2 WRITE! %x : %x\n", address, value);
    this->bank = value%8;
}

nes_byte M2::DoChrRead(nes_address address) {
    // printf("M2 CHR READ! %x\n", address);
    return chr[address];
}

void M2::DoChrWrite(nes_address address, nes_byte value) {
    chr[address] = value;
}