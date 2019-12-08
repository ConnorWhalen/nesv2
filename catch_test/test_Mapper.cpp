//
// NES-V2: test_mapper.cpp
//
// Created by Connor Whalen on 2019-12-08.
//

#include "catch.hpp"

#include "../parts/mappers/M0.h"
#include "../parts/mappers/M2.h"

TEST_CASE("MAPPER 0") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);

    SECTION("INITIALIZE") {
        M0 mapper(romBytes);
    }
}

TEST_CASE("MAPPER 2") {
    auto romBytes = new std::vector<nes_byte>(MAPPER2_SIZE);

    SECTION("INITIALIZE") {
        M2 mapper(romBytes);
    }
}
