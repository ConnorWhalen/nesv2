//
// NES-V2: RomParser.h
//
// Created by Connor Whalen on 2018-12-08.
//

#ifndef NESV2_ROMPARSER_H
#define NESV2_ROMPARSER_H

#include <string>
#include <vector>

namespace RomParser {

struct RomData {
    unsigned short programRomPageCount;
    unsigned short characterRomPageCount;
    unsigned short mapper;
    bool fourScreenMode;
    bool trainer;
    bool batteryBackedRam;
    bool mirroring;
    bool nes2Mode;
    bool playchoice10;
    bool vsUnisystem;
    unsigned char *cartRAM;
    std::vector<unsigned char> *romBytes;
};

RomData* parse(const std::string &fileName);
std::string serialize(RomData *romData);

}

#endif //NESV2_ROMPARSER_H
