//
// NES-V2: PPU.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_PPU_H
#define NESV2_PPU_H

#include "OutputData.h"
#include "Part.h"

class PPU : public Part {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };
};

#endif //NESV2_PPU_H
