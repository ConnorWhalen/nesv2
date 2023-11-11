//
// NES-V2: NullPPU.h
//
// Created by Connor Whalen on 2021-08-08.
//

#ifndef NESV2_NULLPPU_H
#define NESV2_NULLPPU_H

#include "Display.h"
#include "OutputData.h"
#include "PPU.h"


class NullPPU : public PPU {
public:
    std::vector<OutputData>* Serialize() override;
    NullPPU(Display &display, bool mirroring, bool fourScreenMode);
    nes_byte Read(nes_address address) override;
    void Write(nes_address address, nes_byte value) override;
    void Step() override;
    bool DoNMI() override;
    void OAMDMA(nes_byte value, Part* cpu) override;
private:
};

#endif //NESV2_NULLPPU_H
