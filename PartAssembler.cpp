//
// NES-V2: PartAssembler.cpp
//
// Created by Connor Whalen on 2019-05-12.
//

#include "PartAssembler.h"

Parts* PartAssembler::Assemble(
        std::string cpuType,
        std::string ppuType,
        std::string apuType,
        std::string controllersType,
        std::string mapperType,
        std::string speakersType,
        Input *inputDevice
) {
    auto parts = new Parts {};
    if (cpuType == "CPU") {
        parts->cpu = new CPU();
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
    if (mapperType == "Mapper") {
        parts->mapper = new Mapper();
    } else {
        printf("unknown mapper type %s\n", mapperType.c_str());
    }
    if (speakersType == "Speakers") {
        parts->speakers = new Speakers();
    } else {
        printf("unknown speakers type %s\n", speakersType.c_str());
    }
    parts->ppu = new PPU();
    parts->apu = new APU();
    parts->controllers = new Controllers(inputDevice);
    parts->mapper = new Mapper();
    parts->speakers = new Speakers();
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
