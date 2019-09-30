//
// NES-V2: Mapper.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_MAPPER_H
#define NESV2_MAPPER_H

#include "../OutputData.h"
#include "../Part.h"

class Mapper : public Part {
public:
    virtual std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };
    virtual nes_byte Read(nes_address address) = 0;
};

#endif //NESV2_MAPPER_H
