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
constexpr nes_address OAM_SIZE = 0x0100;

constexpr nes_address NAME_TABLES_START = 0x2000;
constexpr nes_address PALETTE_INDICES_START = 0x3F00;


class PPU : public Part {
public:
    virtual std::vector<OutputData>* Serialize() override;
    PPU(Display &display, bool mirroring, bool fourScreenMode, bool debugOutput);
    virtual nes_byte Read(nes_address address) override;
    virtual void Write(nes_address address, nes_byte value) override;
    virtual void Step() override;
    virtual bool DoNMI();
    virtual void OAMDMA(nes_byte value, Part* cpu);
    void dumpPatternTable();

private:
    Display &display;

    nes_byte ppuStatus;
    nes_byte ppuCtrl;
    nes_byte ppuMask;
    nes_byte oamAddr;
    nes_byte ppuScroll;
    nes_byte ppuAddr;
    nes_byte ppuData;
    nes_byte patternTable[PATTERN_TABLE_SIZE*2];
    nes_byte nameTable[NAME_TABLE_SIZE*2];
    nes_byte paletteIndices[PALETTE_INDICES_SIZE];
    nes_byte oam[OAM_SIZE];
    nes_byte secondaryOam[0x20];


    bool ppuCtrl_nmiOnVblankStart;
    bool ppuCtrl_sixteenMode;
    bool ppuCtrl_backgroundPatternTableOffset;
    bool ppuCtrl_spritePatternTableOffset;
    bool ppuCtrl_vramIncrement32;
    bool ppuMask_showSprites;
    bool ppuMask_showBackground;
    bool ppuMask_showSpritesLeft8;
    bool ppuMask_showBackgroundLeft8;
    bool ppuMask_greyscale;
    bool singleSceenMirroring;
    bool verticalMirroring;

    bool oddFrame;
    int scanline;
    int clockTick;
    nes_byte fineXScroll;
    unsigned char spriteCount;
    bool hasSpriteZero;
    bool nmiFlag;
    bool addressLatch;
    nes_address vramAddr;
    nes_address tempVramAddr;

    bool debugOutput;

    nes_byte currentTiles[24];
    nes_byte currentSprites[0x20];
    nes_byte spriteTiles[0x10];

    nes_byte ppuValueAt(nes_address address);
    void setPPUValueAt(nes_address address, nes_byte value);
    void fetchNextTile();
    void fetchNextSprites();
    void evaluateSprites();
    void DrawPixel(nes_byte x);
    void SetDisplayPixel(int x, int y, nes_byte colourAddress=0);
    void incXScroll();
    void incYScroll();
    nes_byte nameTableValueAt(nes_address address);
    void setNameTableValueAt(nes_address address, nes_byte value);
};

#endif //NESV2_PPU_H
