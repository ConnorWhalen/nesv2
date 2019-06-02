//
// NES-V2: PartAssembler.h
//
// Created by Connor Whalen on 2019-05-12.
//

#ifndef NESV2_PARTASSEMBLER_H
#define NESV2_PARTASSEMBLER_H

#include <string>
#include <vector>

#include "parts/CPU.h"
#include "parts/PPU.h"
#include "parts/APU.h"
#include "parts/Controllers.h"
#include "parts/Mapper.h"
#include "parts/Speakers.h"
#include "UI/Input.h"

struct Parts {
    CPU* cpu;
    PPU* ppu;
    APU* apu;
    Controllers* controllers;
    Mapper* mapper;
    Speakers* speakers;
    std::vector<Part*>* asVector;
};

class PartAssembler {
public:
    Parts* Assemble(
            std::string cpuType,
            std::string ppuType,
            std::string apuType,
            std::string controllersType,
            std::string mapperType,
            std::string speakersType,
            Input *inputDevice
    );
};

#endif //NESV2_PARTASSEMBLER_H
