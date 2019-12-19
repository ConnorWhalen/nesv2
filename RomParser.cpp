//
// NES-V2: RomParser.cpp
//
// Created by Connor Whalen on 2018-12-08.
//

#include "RomParser.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace RomParser {

RomData *parse(const std::string &filename) {
    char inChar;
    std::ifstream inFile;
    inFile.open("roms/" + filename);
    if (!inFile.is_open()) {
        std::cout << "could not open rom file " << filename << ". Exiting\n";
        return nullptr;
    }
    std::vector<unsigned char> headerBytes;
    int i = 0;
    while (i < 16 && inFile.get(inChar)) {
        headerBytes.push_back((unsigned char) inChar);
        i++;
    }
    if (headerBytes.size() < 16) {
        std::cout << "ERROR: Could not read rom file header. Exiting";
        return nullptr;
    }

    unsigned short programRomPageCount = headerBytes[4];
    unsigned short characterRomPageCount = headerBytes[5];
    unsigned char flags1 = headerBytes[6];
    unsigned char flags2 = headerBytes[7];

    unsigned short mapper = (flags1 >> 4) + (flags2 & 0xf0);
    unsigned char submapperNumber = 0;
    bool fourScreenMode = flags1 & 0x08;
    bool trainer = flags1 & 0x04;
    bool batteryBackedRam = flags1 & 0x02;
    bool mirroring = flags1 & 0x01;
    bool nes2Mode = ((flags2 & 0x0c) == 0x08);
    bool playchoice10 = flags2 & 0x02;
    bool vsUnisystem = flags2 & 0x01;

    if (nes2Mode) {
        mapper += (headerBytes[8] & 0x0f) << 8;
        submapperNumber = headerBytes[8] >> 4;
        programRomPageCount += (headerBytes[9] & 0x0f) << 8;
        characterRomPageCount += (headerBytes[9] >> 4) << 8;
        // TODO: battery backed / not battery backed RAM
        //       NTSC/PAL
        //       vs. Hardware
    } else {
        // TODO: NES1 spec header
    }

    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    auto romBytes = new std::vector<unsigned char>();
    auto chrBytes = new std::vector<unsigned char>();
    for (int i = 0; i < ROM_PAGE_SIZE*programRomPageCount; i++) {
        inFile.get(inChar);
        romBytes->push_back(inChar);
    }
    for (int i = 0; i < CHR_PAGE_SIZE*characterRomPageCount; i++) {
        inFile.get(inChar);
        chrBytes->push_back(inChar);
    }
    inFile.close();
    return new RomData{
            programRomPageCount,
            characterRomPageCount,
            mapper,
            fourScreenMode,
            trainer,
            batteryBackedRam,
            mirroring,
            nes2Mode,
            playchoice10,
            vsUnisystem,
            cartRAM,
            romBytes,
            chrBytes
    };
}

std::string serialize(RomData *romData) {
    std::stringstream stream;
    stream << "Program ROM page count: " << romData->programRomPageCount << "\n";
    stream << "Character ROM page count: " << romData->characterRomPageCount << "\n";
    stream << "Mapper id: " << romData->mapper << "\n";
    stream << "Four screen mode: " << romData->fourScreenMode << "\n";
    stream << "Trainer: " << romData->trainer << "\n";
    stream << "Battery-backed RAM: " << romData->batteryBackedRam << "\n";
    stream << "Mirroring: " << romData->mirroring << "\n";
    stream << "NES2 mode: " << romData->nes2Mode << "\n";
    stream << "Playchoice10: " << romData->playchoice10 << "\n";
    stream << "vsUnisystem: " << romData->vsUnisystem;
    return stream.str();
}

}
