//
// NES-V2: Part.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_PART_H
#define NESV2_PART_H

#include <ostream>
#include <string>

#include "OutputData.h"

typedef unsigned char nes_byte;
typedef unsigned short nes_address;

class Part {
public:
    virtual std::vector<OutputData>* Serialize() {
        return new std::vector<OutputData>();
    };

    virtual void Step() {};
    void Tick() {
        while (step++ < STEPS_PER_TICK) Step();
    };
protected:
    int step = 0;
    int STEPS_PER_TICK = 0;
};

#endif //NESV2_PART_H
