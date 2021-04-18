//
// NES-V2: PPU.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_PPU_H
#define NESV2_PPU_H

#include "OutputData.h"
#include "Part.h"

constexpr nes_address PATTERN_TABLE_SIZE = 0x1000;
constexpr nes_address NAME_TABLE_SIZE = 0x400;
constexpr nes_address PALETTE_INDICES_SIZE = 0x400;

constexpr nes_address NAME_TABLES_START = 0x2000;
constexpr nes_address PALETTE_INDICES_START = 0x2000;


class PPU : public Part {
public:
    std::vector<OutputData>* Serialize() override;
    PPU();
    nes_byte Read(nes_address address) override;
    void Write(nes_address address, nes_byte value) override;
    bool DoNMI();
private:
    nes_byte ppuStatus;
    nes_byte ppuCtrl;
    nes_byte ppuMask;
    nes_byte oamAddr;
    nes_byte ppuScroll;
    nes_byte ppuAddr;
    nes_byte ppuData;
    bool oddFrame;
    nes_byte patternTable0[PATTERN_TABLE_SIZE*2];
    nes_byte nameTable0[NAME_TABLE_SIZE*4];
    nes_byte paletteIndices[PALETTE_INDICES_SIZE];
    nes_byte oam[0x100];
};

#endif //NESV2_PPU_H
