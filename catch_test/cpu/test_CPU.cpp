//
// NES-V2: test_CPU.cpp
//
// Created by Connor Whalen on 2019-10-13.
//

#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "cpu_test_utilities.h"

#include "../../parts/CPU.h"
#include "../../parts/mappers/M0.h"

TEST_CASE("CPU functionality") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);
    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    for (int i = 0; i < CART_RAM_SIZE; i++) cartRAM[i] = 0;
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);

    SECTION("CPU INITIALIZATION") {
        M0 mapper(romBytes, cartRAM, chrBytes);
        CPU cpu(&mapper, nullptr);

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, 0x0000));
        delete output;
    }

    SECTION("PC RESET") {
        nes_address program_start = 0x8100;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);

        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x80);
        relativeRomWrite(romBytes, program_index++, 0xA0); // LDY imm
        relativeRomWrite(romBytes, program_index++, 0x33);
        relativeRomWrite(romBytes, program_index++, 0xA2); // LDX imm
        relativeRomWrite(romBytes, program_index++, 0x45);
        relativeRomWrite(romBytes, program_index++, 0x08); // PHP
        relativeRomWrite(romBytes, program_index++, 0x38); // SEC

        M0 mapper(romBytes, cartRAM, chrBytes);
        CPU cpu(&mapper, nullptr);

        for (int i = 0; i < 9; i++) {
            cpu.Step();
        }

        cpu.Reset();

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start));
        delete output;

    }

    SECTION("RAM OUTPUT") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[0x800];
        for (int i = 0; i < RAM_SIZE; i++) {
            ram[i] = (nes_byte) (rand() % 0x7D) + 1;
            relativeRomWrite(romBytes, program_index++, 0xA9);
            relativeRomWrite(romBytes, program_index++, ram[i]);
            relativeRomWrite(romBytes, program_index++, 0x8D);
            relativeRomWrite(romBytes, program_index++, (nes_byte) (i & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) (i >> 8));
        }
        M0 mapper(romBytes, cartRAM, chrBytes);
        CPU cpu(&mapper, nullptr);

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start));
        delete output;

        for (int i = 0; i < RAM_SIZE * 6; i++) {
            cpu.Step();
        }

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[RAM_SIZE-1], INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start + (RAM_SIZE*5)));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("RAM READ MIRRORING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (nes_byte v : ram) v = 0;
        ram[0] = (nes_byte) (rand() % 0x7D) + 1;
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0]);
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, (nes_byte) 0x00);
        relativeRomWrite(romBytes, program_index++, (nes_byte) 0x00);
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            relativeRomWrite(romBytes, program_index++, 0xAD); // LDA abs
            relativeRomWrite(romBytes, program_index++, (nes_byte) (((i*RAM_SIZE)) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) (((i*RAM_SIZE)) >> 8));
            relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i+1) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i+1) >> 8));
            ram[i+1] = ram[0];
        }
        M0 mapper(romBytes, cartRAM, chrBytes);
        CPU cpu(&mapper, nullptr);

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start));
        delete output;

        for (int i = 0; i < 6 + (RAM_SEGMENT_END/RAM_SIZE * 8); i++) {
            cpu.Step();
        }

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0], INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start + 5 + (RAM_SEGMENT_END/RAM_SIZE * 6)));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("RAM WRITE MIRRORING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            ram[i] = (nes_byte) (rand() % 0x7D) + 1;
            relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
            relativeRomWrite(romBytes, program_index++, ram[i]);
            relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i*RAM_SIZE + i) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i*RAM_SIZE + i) >> 8));
        }
        M0 mapper(romBytes, cartRAM, chrBytes);
        CPU cpu(&mapper, nullptr);

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start));
        delete output;

        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE * 6; i++) {
            cpu.Step();
        }

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[RAM_SEGMENT_END/RAM_SIZE-1], INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start + (RAM_SEGMENT_END/RAM_SIZE*5)));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }
}
