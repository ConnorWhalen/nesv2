//
// NES-V2: ppu_test_utilities.cpp
//
// Created by Connor Whalen on 2021-07-08.
//

#include "ppu_test_utilities.h"

#include <iomanip>
#include <sstream>

std::string getRegisterString(nes_byte ppuStatus, nes_byte ppuCtrl, nes_byte ppuMask, nes_byte oamAddr, nes_byte ppuScroll, nes_byte ppuData) {
    std::stringstream registerStream;
    registerStream << std::setfill('0') << std::setw(2) << std::hex << int(ppuCtrl) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuMask) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuStatus) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(oamAddr) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuScroll) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuData);
    return registerStream.str();
}

void write_ppu_addr(PPU &ppu, nes_address address, bool reading) {
        ppu.Read(2);

        ppu.Write(6, address / 0x100);
        ppu.Write(6, address & 0x100);

        if (reading) {
            ppu.Read(7);
        }
}
