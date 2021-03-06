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
#include "parts/mappers/Mapper.h"
#include "parts/Speakers.h"
#include "input/Input.h"
#include "RomParser.h"

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
            const std::string& cpuType,
            const std::string& ppuType,
            const std::string& apuType,
            const std::string& controllersType,
            const std::string& speakersType,
            Input *inputDevice,
            RomParser::RomData *romData
    );
};

#endif //NESV2_PARTASSEMBLER_H
