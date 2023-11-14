//
// NES-V2: test_CPU_Transfers.cpp
//
// Created by Connor Whalen on 2019-10-13.
//

#include "../catch.hpp"

#include "cpu_test_utilities.h"

#include "../../parts/CPU.h"
#include "../../parts/NullDisplay.h"
#include "../../parts/PPU.h"
#include "../../parts/mappers/M0.h"

TEST_CASE("CPU transfers") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);
    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    for (int i = 0; i < CART_RAM_SIZE; i++) cartRAM[i] = 0;
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);

    NullDisplay display;
    PPU ppu(display, false, false, true);

    SECTION("Stack Operations") {
        
    }

    // TODO: Addressing mode test coverage
//        ACCUMULATOR, ROL/ROR
//        IMPLICIT,
    // TODO: test Instruction results
    // TODO: test Instruction status register results
    // TODO: test Instruction Duration
}
