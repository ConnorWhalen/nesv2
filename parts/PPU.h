//
// NES-V2: PPU.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_PPU_H
#define NESV2_PPU_H

#include "Display.h"
#include "OutputData.h"
#include "Part.h"

constexpr nes_address PATTERN_TABLE_SIZE = 0x1000;
constexpr nes_address NAME_TABLE_SIZE = 0x400;
constexpr nes_address PALETTE_INDICES_SIZE = 0x20;

constexpr nes_address NAME_TABLES_START = 0x2000;
constexpr nes_address PALETTE_INDICES_START = 0x3F00;


class PPU : public Part {
public:
    std::vector<OutputData>* Serialize() override;
    PPU(Display* display);
    nes_byte Read(nes_address address) override;
    void Write(nes_address address, nes_byte value) override;
    void Step() override;
    bool DoNMI();
private:
    Display* display;

    nes_byte ppuStatus;
    nes_byte ppuCtrl;
    nes_byte ppuMask;
    nes_byte oamAddr;
    nes_byte ppuScroll;
    nes_byte ppuAddr;
    nes_byte ppuData;
    nes_byte patternTable[PATTERN_TABLE_SIZE*2];
    nes_byte nameTable[NAME_TABLE_SIZE*4];
    nes_byte paletteIndices[PALETTE_INDICES_SIZE];
    nes_byte oam[0x100];

    bool ppuMask_showSprites;
    bool ppuMask_showBackground;
    bool ppuMask_showSpritesLeft8;
    bool ppuMask_showBackgroundLeft8;
    bool ppuMask_greyscale;

    bool oddFrame;
    int scanline;
    nes_byte fineXScroll;
    unsigned char spriteCount;
    bool hasSpriteZero;

    nes_byte currentTiles[24];
    nes_byte currentSprites[0x20];
    nes_byte spriteTiles[0x10];

    void DrawPixel(nes_byte x);
    void SetDisplayPixel(int x, int y, nes_byte colourAddress=0);
};

#endif //NESV2_PPU_H
