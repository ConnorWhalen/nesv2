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
        setResetVector(romBytes, program_start);
        nes_byte ram[0x800];
        for (int i = 0; i < RAM_SIZE; i++) {
            ram[i] = (nes_byte) (rand() % 0x7D) + 1;
            relativeRomWrite(romBytes, program_start + (i*5), 0xA9);
            relativeRomWrite(romBytes, program_start + (i*5) + 1, ram[i]);
            relativeRomWrite(romBytes, program_start + (i*5) + 2, 0x8D);
            relativeRomWrite(romBytes, program_start + (i*5) + 3, (nes_byte) (i & 0xFF));
            relativeRomWrite(romBytes, program_start + (i*5) + 4, (nes_byte) (i >> 8));
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
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (nes_byte v : ram) v = 0;
        ram[0] = (nes_byte) (rand() % 0x7D) + 1;
        relativeRomWrite(romBytes, program_start, 0xA9);
        relativeRomWrite(romBytes, program_start + 1, ram[0]);
        relativeRomWrite(romBytes, program_start + 2, 0x8D);
        relativeRomWrite(romBytes, program_start + 3, (nes_byte) 0x00);
        relativeRomWrite(romBytes, program_start + 4, (nes_byte) 0x00);
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            ram[(i*RAM_SEGMENT_END/RAM_SIZE)+1] = ram[0];
            relativeRomWrite(romBytes, program_start + (i*6) + 5, 0xAD);
            relativeRomWrite(romBytes, program_start + (i*6) + 6, (nes_byte) (((i*RAM_SIZE)) & 0xFF));
            relativeRomWrite(romBytes, program_start + (i*6) + 7, (nes_byte) (((i*RAM_SIZE)) >> 8));
            relativeRomWrite(romBytes, program_start + (i*6) + 8, 0x8D);
            relativeRomWrite(romBytes, program_start + (i*6) + 9, (nes_byte) ((i+1) & 0xFF));
            relativeRomWrite(romBytes, program_start + (i*6) + 10, (nes_byte) ((i+1) >> 8));
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
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (nes_byte v : ram) v = 0;
        for (int i = 0; i < RAM_SEGMENT_END/RAM_SIZE; i++) {
            ram[i] = (nes_byte) (rand() % 0x7D) + 1;
            relativeRomWrite(romBytes, program_start + (i*5), 0xA9);
            relativeRomWrite(romBytes, program_start + (i*5) + 1, ram[i]);
            relativeRomWrite(romBytes, program_start + (i*5) + 2, 0x8D);
            relativeRomWrite(romBytes, program_start + (i*5) + 3, (nes_byte) ((i*RAM_SIZE + i) & 0xFF));
            relativeRomWrite(romBytes, program_start + (i*5) + 4, (nes_byte) ((i*RAM_SIZE + i) >> 8));
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
}
