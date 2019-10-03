//
// NES-V2: Mapper.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_MAPPER_H
#define NESV2_MAPPER_H

#include "../OutputData.h"
#include "../Part.h"

constexpr nes_address MAPPER_REGION_START = 0x8000;
constexpr nes_address MAPPER_REGION_MIDDLE = 0xC000;

class Mapper : public Part {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };
    nes_byte Read(nes_address address);
    void Write(nes_address address, nes_byte value);
protected:
    virtual nes_byte DoRead(nes_address address) = 0;
    virtual void DoWrite(nes_address address, nes_byte value) = 0;
};

#endif //NESV2_MAPPER_H
