//
// NES-V2: PartAssembler.cpp
//
// Created by Connor Whalen on 2019-05-12.
//

#include "PartAssembler.h"

#include "parts/mappers/M0.h"
#include "parts/mappers/M1.h"
#include "parts/mappers/M2.h"

Parts* PartAssembler::Assemble(
        const std::string& cpuType,
        const std::string& ppuType,
        const std::string& apuType,
        const std::string& controllersType,
        const std::string& speakersType,
        Input *inputDevice,
        RomParser::RomData *romData
) {
    auto parts = new Parts {};
    if (romData->mapper == 0) {
        parts->mapper = new M0(romData->romBytes, romData->cartRAM);
    } else if (romData->mapper == 1) {
        parts->mapper = new M1(romData->romBytes, romData->cartRAM);
    } else if (romData->mapper == 2) {
        parts->mapper = new M2(romData->romBytes);
    } else {
        printf("unknown mapper type %d\n", romData->mapper);
    }
    if (cpuType == "CPU") {
        parts->cpu = new CPU(parts->mapper);
    } else {
        printf("unknown cpu type %s\n", cpuType.c_str());
    }
    if (ppuType == "PPU") {
        parts->ppu = new PPU();
    } else {
        printf("unknown ppu type %s\n", ppuType.c_str());
    }
    if (apuType == "APU") {
        parts->apu = new APU();
    } else {
        printf("unknown apu type %s\n", apuType.c_str());
    }
    if (controllersType == "Controllers") {
        parts->controllers = new Controllers(inputDevice);
    } else {
        printf("unknown controllers type %s\n", controllersType.c_str());
    }
    if (speakersType == "Speakers") {
        parts->speakers = new Speakers();
    } else {
        printf("unknown speakers type %s\n", speakersType.c_str());
    }
    parts->asVector = new std::vector<Part*> {
            parts->cpu,
            parts->ppu,
            parts->apu,
            parts->controllers,
            parts->mapper,
            parts->speakers
    };
    return parts;
}
