//
// NES-V2: test_PPU.cpp
//
// Created by Connor Whalen on 2022-07-05.
//
#include "../catch.hpp"

#include "ppu_test_utilities.h"

#include "../../parts/CPU.h"
#include "../../parts/NullDisplay.h"
#include "../../parts/PPU.h"
#include "../../parts/mappers/M0.h"

TEST_CASE("PPU functionality") {
    SECTION("PPU INITIALIZATION") {
        NullDisplay display;
        PPU ppu(display, false, false);

        auto output = ppu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(0, 0, 0, 0, 0, 0));
        delete output;
    }
}
