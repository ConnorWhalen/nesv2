//
// NES-V2: APU.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_APU_H
#define NESV2_APU_H

#include "OutputData.h"
#include "Part.h"

class APU : public Part {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };
};

#endif //NESV2_APU_H
