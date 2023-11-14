//
// NES-V2: test_mapper.cpp
//
// Created by Connor Whalen on 2019-12-08.
//

#include "catch.hpp"

#include "mapper_test_utilities.h"

#include "../parts/mappers/M0.h"
#include "../parts/mappers/M2.h"

TEST_CASE("MAPPER 0") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);
    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);
    for (nes_byte &b : *romBytes) b = rand();
    M0 mapper(*romBytes, cartRAM, *chrBytes, true);

    SECTION("MAPPER 0 READ") {
        for (int i = 0; i < MAPPER0_SIZE; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER0_SIZE) == romBytes->at(i));
        }
    }

    SECTION("MAPPER 0 SERIALIZE") {
        REQUIRE(mapper.Serialize()->at(0).body == getM0String(romBytes));
    }
}

TEST_CASE("MAPPER 2") {
    auto romBytes = new std::vector<nes_byte>(MAPPER2_SIZE);
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);
    for (nes_byte &b : *romBytes) b = rand();
    M2 mapper(romBytes, chrBytes, true);

    SECTION("BANK 7 READ") {
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_MIDDLE) == romBytes->at(i+MAPPER2_BANK_SIZE*7));
        }
    }

    SECTION("BANK 0 READ") {
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i));
        }
    }

    SECTION("BANK 1 READ") {
        mapper.DoWrite(0x8000, 1);
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i+MAPPER2_BANK_SIZE));
        }
    }

    SECTION("BANK 2 READ") {
        mapper.DoWrite(0x8000, 2);
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i+MAPPER2_BANK_SIZE*2));
        }
    }

    SECTION("BANK 3 READ") {
        mapper.DoWrite(0x8000, 3);
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i+MAPPER2_BANK_SIZE*3));
        }
    }

    SECTION("BANK 4 READ") {
        mapper.DoWrite(0x8000, 4);
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i+MAPPER2_BANK_SIZE*4));
        }
    }

    SECTION("BANK 5 READ") {
        mapper.DoWrite(0x8000, 5);
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i+MAPPER2_BANK_SIZE*5));
        }
    }

    SECTION("BANK 6 READ") {
        mapper.DoWrite(0x8000, 6);
        for (int i = 0; i < MAPPER0_SIZE/2; i++) {
            REQUIRE(mapper.DoRead(i+MAPPER_ROM_REGION_START) == romBytes->at(i+MAPPER2_BANK_SIZE*6));
        }
    }
}
