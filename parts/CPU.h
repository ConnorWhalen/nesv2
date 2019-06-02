//
// NES-V2: CPU.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_CPU_H
#define NESV2_CPU_H

#include "OutputData.h"
#include "Part.h"

class CPU : public Part {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };
};

#endif //NESV2_CPU_H
