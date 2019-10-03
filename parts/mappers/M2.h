//
// NES-V2: M0.h
//
// Created by Connor Whalen on 2019-09-29.
//

#ifndef NESV2_M2_H
#define NESV2_M2_H

#include "Mapper.h"

constexpr int MAPPER2_SIZE = 0x20000;
constexpr nes_address MAPPER2_BANK_SIZE = 0x4000;

class M2 : public Mapper {
public:
    M2(std::vector<unsigned char> *romBytes);
    std::vector<OutputData>* Serialize() override;
    nes_byte DoRead(nes_address address) override;
    void DoWrite(nes_address address, nes_byte value) override;
private:
    nes_byte bytes[MAPPER2_SIZE];
    int bank;
};

#endif //NESV2_M2_H