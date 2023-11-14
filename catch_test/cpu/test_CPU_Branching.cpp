//
// NES-V2: test_CPU_Branching.cpp
//
// Created by Connor Whalen on 2019-10-13.
//

#include "../catch.hpp"

#include "cpu_test_utilities.h"

#include "../../parts/CPU.h"
#include "../../parts/NullDisplay.h"
#include "../../parts/PPU.h"
#include "../../parts/mappers/M0.h"

TEST_CASE("CPU branching") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);
    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    for (int i = 0; i < CART_RAM_SIZE; i++) cartRAM[i] = 0;
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);

    NullDisplay display;
    PPU ppu(display, false, false, true);

    SECTION("JMP ABS") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        ram[0x0628] = 0xEA;
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0xEA); // Store a NOP
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0x4C); // JMP abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x06);

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 10; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0xEA, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | NEGATIVE_FLAG, 0x0629));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("JMP IND") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        ram[0x0628] = 0x28;
        ram[0x0629] = 0x07;
        ram[0x0728] = 0xEA;
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x29);
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0xEA); // Store a NOP
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0x6C); // JMP ind
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x06);

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 24; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0xEA, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | NEGATIVE_FLAG, 0x0729));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("JMP IND PAGE BOUNDARY") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        ram[0x06FF] = 0x28;
        ram[0x0600] = 0x07;
        ram[0x0728] = 0xEA;
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0xFF);
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x00);
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0xEA); // Store a NOP
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0x6C); // JMP ind
        relativeRomWrite(romBytes, program_index++, 0xFF);
        relativeRomWrite(romBytes, program_index++, 0x06);

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 24; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0xEA, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | NEGATIVE_FLAG, 0x0729));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("BCC/BCS TIMING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);

        relativeRomWrite(romBytes, program_index++, 0x90); // BCC
        relativeRomWrite(romBytes, program_index++, 0x08);
        program_index += 8;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP
        relativeRomWrite(romBytes, program_index++, 0x38); // SEC
        relativeRomWrite(romBytes, program_index++, 0xB0); // BCS
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0xB0); // BCS page cross
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 15; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | CARRY_FLAG, program_index));
        delete output;
    }

    SECTION("BEQ/BNE TIMING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);

        relativeRomWrite(romBytes, program_index++, 0xD0); // BNE
        relativeRomWrite(romBytes, program_index++, 0x08);
        program_index += 8;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP
        relativeRomWrite(romBytes, program_index++, 0xAA); // TAX
        relativeRomWrite(romBytes, program_index++, 0xF0); // BEQ
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0xF0); // BEQ page cross
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 15; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | ZERO_FLAG, program_index));
        delete output;
    }

    SECTION("BMI/BPL TIMING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);

        relativeRomWrite(romBytes, program_index++, 0x10); // BPL
        relativeRomWrite(romBytes, program_index++, 0x08);
        program_index += 8;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA
        relativeRomWrite(romBytes, program_index++, 0x80);
        relativeRomWrite(romBytes, program_index++, 0x30); // BMI
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0x30); // BMI page cross
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 15; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x80, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | NEGATIVE_FLAG, program_index));
        delete output;
    }

    SECTION("BVC/BVS TIMING") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);

        relativeRomWrite(romBytes, program_index++, 0x50); // BVC
        relativeRomWrite(romBytes, program_index++, 0x08);
        program_index += 8;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA
        relativeRomWrite(romBytes, program_index++, 0x40);
        relativeRomWrite(romBytes, program_index++, 0x69); // ADC imm
        relativeRomWrite(romBytes, program_index++, 0x40);
        relativeRomWrite(romBytes, program_index++, 0x70); // BVS
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0x70); // BVS page cross
        relativeRomWrite(romBytes, program_index++, 0x7F);
        program_index += 0x7F;
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 17; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x80, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | OVERFLOW_FLAG | NEGATIVE_FLAG, program_index));
        delete output;
    }

    SECTION("BRK/RTI") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        setIRQVector(romBytes, 0x0728);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x40); // store an RTI
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0x00); // BRK, suppressed
        relativeRomWrite(romBytes, program_index++, 0x58); // CLI
        relativeRomWrite(romBytes, program_index++, 0x00); // BRK
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP, skipped
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        ram[0x0728] = 0x40; // RTI

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 13; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x40, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start+6));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;

        for (int i = 0; i < 9; i++) {
            cpu.Step();
        }

        ram[0x01FD] = 0x80;
        ram[0x01FC] = 0x09;
        ram[0x01FB] = 0x30;

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x40, INITIAL_X, INITIAL_Y, INITIAL_S - 3, INITIAL_P, 0x0728));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;

        for (int i = 0; i < 7; i++) {
            cpu.Step();
        }

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x40, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P - INTERRUPT_DISABLE_FLAG, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("NMI/RTI") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        setNMIVector(romBytes, 0x0728);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x40); // store an RTI
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        ram[0x0728] = 0x40; // RTI

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 6; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x40, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_start+5));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;

        cpu.NMI();
        ram[0x01FD] = 0x80;
        ram[0x01FC] = 0x05;
        ram[0x01FB] = 0x34;

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x40, INITIAL_X, INITIAL_Y, INITIAL_S - 3, INITIAL_P, 0x0728));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;

        for (int i = 0; i < 7; i++) {
            cpu.Step();
        }

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x40, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("JSR/RTS") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        ram[0x0728] = 0x60; // RTS
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x60); // store an RTS
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0x20); // JSR abs
        relativeRomWrite(romBytes, program_index++, 0x28);
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        ram[0x01FD] = 0x80;
        ram[0x01FC] = 0x07;

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        for (int i = 0; i < 7; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x60, INITIAL_X, INITIAL_Y, INITIAL_S - 2, INITIAL_P, 0x0728));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;

        for (int i = 0; i < 12; i++) {
            cpu.Step();
        }

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x60, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    // TODO: Addressing mode test coverage
//        ACCUMULATOR, ROL/ROR
//        IMPLICIT,
    // TODO: test Instruction results
    // TODO: test Instruction status register results
    // TODO: test Instruction Duration
}
