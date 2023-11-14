//
// NES-V2: M1.h
//
// Created by Connor Whalen on 2019-12-15.
//

#ifndef NESV2_M1_H
#define NESV2_M1_H

#include "Mapper.h"

#include "../../RomParser.h"

constexpr int MAPPER1_MAX_SIZE = 0x80000;
constexpr int MAPPER1_MAX_CHR_SIZE = 0x20000;
constexpr nes_address MAPPER1_BANK_SIZE = 0x4000;
constexpr nes_address MAPPER1_CHR_BANK_SIZE = 0x2000;
constexpr nes_address MAPPER1_CONTROL_REGION_START = 0x8000;
constexpr nes_address MAPPER1_CHR0_REGION_START = 0xA000;
constexpr nes_address MAPPER1_CHR1_REGION_START = 0xC000;
constexpr nes_address MAPPER1_PRG_REGION_START = 0xE000;

class M1 : public Mapper {
public:
    M1(std::vector<unsigned char> *romBytes, const unsigned char*cartRAM,
       std::vector<unsigned char> *chrBytes, bool debugOutput);
    std::vector<OutputData>* Serialize() override;
    nes_byte DoRead(nes_address address) override;
    void DoWrite(nes_address address, nes_byte value) override;
    nes_byte DoChrRead(nes_address address) override;
    void DoChrWrite(nes_address address, nes_byte value) override;
private:
    void DoShift(nes_address address, nes_byte value);
    void ShiftWrite(nes_address address, nes_byte value);

    nes_byte prgRom[MAPPER1_MAX_SIZE];
    nes_byte prgRam[CART_RAM_SIZE];
    nes_byte chr[MAPPER1_MAX_CHR_SIZE];
    int prgRomBank1;
    int prgRomBank2;
    int prgRomBankCount;
    nes_byte shiftReg;
    nes_byte controlReg;
    bool ramEnable;
    int chrBank1;
    int chrBank2;
    int chrBankCount;

    bool debugOutput;
};

#endif //NESV2_M1_H
