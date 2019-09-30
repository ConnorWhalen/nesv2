//
// NES-V2: M0.h
//
// Created by Connor Whalen on 2019-09-29.
//

#ifndef NESV2_M1_H
#define NESV2_M1_H

#include "Mapper.h"

constexpr nes_address MAPPER0_SIZE = 0x8000;

class M0 : public Mapper {
public:
    M0(std::vector<unsigned char> *romBytes);
    std::vector<OutputData>* Serialize() override;
    nes_byte Read(nes_address address) override;
private:
    nes_byte bytes[MAPPER0_SIZE];
    std::string serialized;
};

#endif //NESV2_M1_H
