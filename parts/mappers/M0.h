//
// NES-V2: M0.h
//
// Created by Connor Whalen on 2019-09-29.
//

#ifndef NESV2_M0_H
#define NESV2_M0_H

#include "Mapper.h"

#include "../../RomParser.h"

constexpr nes_address MAPPER0_SIZE = 0x8000;

class M0 : public Mapper {
public:
    explicit M0(std::vector<unsigned char> *romBytes, const unsigned char *cartRAM);
    std::vector<OutputData>* Serialize() override;
    nes_byte DoRead(nes_address address) override;
    void DoWrite(nes_address address, nes_byte value) override;
private:

    nes_byte bytes[MAPPER0_SIZE];
    nes_byte ram[CART_RAM_SIZE];
    std::string serialized;
};

#endif //NESV2_M0_H
