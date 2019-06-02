//
// NES-V2: Speakers.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_SPEAKERS_H
#define NESV2_SPEAKERS_H

#include "OutputData.h"
#include "Part.h"

class Speakers : public Part {
public:
    std::vector<OutputData>* Serialize() override {
        return new std::vector<OutputData>();
    };
};

#endif //NESV2_SPEAKERS_H
