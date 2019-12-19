//
// NES-V2: test_CPU.cpp
//
// Created by Connor Whalen on 2019-10-13.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "cpu_test_utilities.h"

#include "../parts/CPU.h"
#include "../parts/mappers/M0.h"

TEST_CASE("CPU functionality") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);

    SECTION("CPU INITIALIZATION") {
        M0 mapper(romBytes);
        CPU cpu(&mapper);

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, 0x0000));
        delete output;
    }

    SECTION("PC RESET") {
        nes_address program_start = 0x1234;
        setResetVector(romBytes, program_start);
        M0 mapper(romBytes);
        CPU cpu(&mapper);

        for (int i = 0; i < 17; i++) {
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
        M0 mapper(romBytes);
        CPU cpu(&mapper);

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
        relativeRomWrite(romBytes, program_index++, 0xA9);
        relativeRomWrite(romBytes, program_index++, ram[0]);
        relativeRomWrite(romBytes, program_index++, 0x8D);
        relativeRomWrite(romBytes, program_index++, (nes_byte) 0x00);
        relativeRomWrite(romBytes, program_index++, (nes_byte) 0x00);
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            ram[(i*RAM_SEGMENT_END/RAM_SIZE)+1] = ram[0];
            relativeRomWrite(romBytes, program_index++, 0xAD);
            relativeRomWrite(romBytes, program_index++, (nes_byte) (((i*RAM_SIZE)) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) (((i*RAM_SIZE)) >> 8));
            relativeRomWrite(romBytes, program_index++, 0x8D);
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i+1) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i+1) >> 8));
        }
        M0 mapper(romBytes);
        CPU cpu(&mapper);

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
        delete output;
    }

    SECTION("RAM WRITE MIRRORING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (nes_byte v : ram) v = 0;
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            ram[i] = (nes_byte) (rand() % 0x7D) + 1;
            relativeRomWrite(romBytes, program_index++, 0xA9);
            relativeRomWrite(romBytes, program_index++, ram[i]);
            relativeRomWrite(romBytes, program_index++, 0x8D);
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i*RAM_SIZE + i) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i*RAM_SIZE + i) >> 8));
        }
        M0 mapper(romBytes);
        CPU cpu(&mapper);

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
        delete output;
    }

    SECTION("LDA/STA") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (nes_byte v : ram) v = 0;
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            ram[i] = (nes_byte) (rand() % 0x7D) + 1;
            relativeRomWrite(romBytes, program_index++, 0xA9);
            relativeRomWrite(romBytes, program_index++, ram[i]);
            relativeRomWrite(romBytes, program_index++, 0x8D);
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i*RAM_SIZE + i) & 0xFF));
            relativeRomWrite(romBytes, program_index++, (nes_byte) ((i*RAM_SIZE + i) >> 8));
        }
        M0 mapper(romBytes);
        CPU cpu(&mapper);

        // TODO:
        // IMMEDIATE load
        // ZERO_PAGE store
        // ZERO_PAGE load
        // ZERO_PAGE_X store
        // ZERO_PAGE_X load
        // ABSOLUTE store
        // ABSOLUTE load
        // ABSOLUTE_X store
        // ABSOLUTE_X load
        // ABSOLUTE_Y store
        // ABSOLUTE_Y load
        // INDIRECT_X store
        // INDIRECT_X load
        // INDIRECT_Y store
        // INDIRECT_Y load
        // increment before each store
    }

    // TODO: Addressing mode test coverage
//        ZERO_PAGE_X, LDA STA
//        ZERO_PAGE_Y, LDX STX
//        ABSOLUTE_X, LDA STA
//        ABSOLUTE_Y, LDA STA
//        INDIRECT_X, LDA STA
//        INDIRECT_Y, LDA STA
//        ZERO_PAGE, LDA STA
//        IMMEDIATE, LDA
//        ABSOLUTE, LDA STA
//        INDIRECT, JMP
//        ACCUMULATOR, ROL/ROR
//        IMPLICIT,
//        RELATIVE, branch
    // TODO: test Instruction results
    // TODO: test Instruction status register results
    // TODO: test Instruction Duration
}
