//
// NES-V2: Mapper.cpp
//
// Created by Connor Whalen on 2019-10-02.
//

#include "Mapper.h"

nes_byte Mapper::Read(nes_address address) {
    if (address < MAPPER_REGION_START) {
        printf("ERROR: INVALID MAPPER READ ADDRESS %x\n", address);
    }
    return DoRead(address);
}

void Mapper::Write(nes_address address, nes_byte value) {
    if (address < MAPPER_REGION_START) {
        printf("ERROR: INVALID MAPPER WRITE ADDRESS %x\n", address);
    }
    DoWrite(address, value);
}

nes_byte Mapper::ChrRead(nes_address address) {
    if (address >= MAPPER_CHR_REGION_SIZE) {
        printf("ERROR: INVALID MAPPER CHR READ ADDRESS %x\n", address);
    }
    return DoChrRead(address);
}

void Mapper::ChrWrite(nes_address address, nes_byte value) {
    if (address >= MAPPER_CHR_REGION_SIZE) {
        printf("ERROR: INVALID MAPPER CHR WRITE ADDRESS %x\n", address);
    }
    DoChrWrite(address, value);
}
