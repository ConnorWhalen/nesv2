//
// NES-V2: test_CPU_Loading.cpp
//
// Created by Connor Whalen on 2019-10-13.
//

#include "../catch.hpp"

#include "cpu_test_utilities.h"

#include "../../parts/CPU.h"
#include "../../parts/NullDisplay.h"
#include "../../parts/PPU.h"
#include "../../parts/mappers/M0.h"

TEST_CASE("CPU loading") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);
    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    for (int i = 0; i < CART_RAM_SIZE; i++) cartRAM[i] = 0;
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);

    NullDisplay display;
    PPU ppu(display, false, false);

    SECTION("LDA IMM AND FLAGS") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);

        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x00);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x80);

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        cpu.Step();

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x00, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | ZERO_FLAG, program_start + 2));
        delete output;

        cpu.Step();
        cpu.Step();

        output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                0x80, INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P | NEGATIVE_FLAG, program_start + 4));
        delete output;
    }

    SECTION("LDA/STA ZP") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        ram[0x24] = (nes_byte) (rand() % 0x7D) + 1;
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x24]);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0xA5); // LDA zp
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 9; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x24], INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDA/STA ZPX") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        nes_byte x = 0x33;

        ram[0x57] = (nes_byte) (rand() % 0x7D) + 1;
        relativeRomWrite(romBytes, program_index++, 0xA2); // LDX imm
        relativeRomWrite(romBytes, program_index++, 0x33);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x57]);
        relativeRomWrite(romBytes, program_index++, 0x95); // STA zpx
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0xB5); // LDA zpx
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 13; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x57], x, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDA/STA ABS") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;

        ram[0x0334] = (nes_byte) (rand() % 0x7D) + 1;
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x0334]);
        relativeRomWrite(romBytes, program_index++, 0x8D); // STA abs
        relativeRomWrite(romBytes, program_index++, 0x34);
        relativeRomWrite(romBytes, program_index++, 0x03);
        relativeRomWrite(romBytes, program_index++, 0xAD); // LDA abs
        relativeRomWrite(romBytes, program_index++, 0x34);
        relativeRomWrite(romBytes, program_index++, 0x03);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 11; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x0334], INITIAL_X, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDA/STA ABX") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        nes_byte x = 0x10;

        ram[0x0501] = (nes_byte) (rand() % 0x7D) + 1;
        ram[0x0234] = ram[0x0501];
        relativeRomWrite(romBytes, program_index++, 0xA2); // LDX imm
        relativeRomWrite(romBytes, program_index++, x);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x0501]);
        relativeRomWrite(romBytes, program_index++, 0x9D); // STA abx
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0x02);
        relativeRomWrite(romBytes, program_index++, 0xBD); // LDA abx
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0x02);
        relativeRomWrite(romBytes, program_index++, 0x9D); // STA abx
        relativeRomWrite(romBytes, program_index++, 0xF1);
        relativeRomWrite(romBytes, program_index++, 0x04);
        relativeRomWrite(romBytes, program_index++, 0xBD); // LDA abx page cross
        relativeRomWrite(romBytes, program_index++, 0xF1);
        relativeRomWrite(romBytes, program_index++, 0x04);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 24; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x0501], x, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDA/STA ABY") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        nes_byte y = 0x10;

        ram[0x0501] = (nes_byte) (rand() % 0x7D) + 1;
        ram[0x0234] = ram[0x0501];
        relativeRomWrite(romBytes, program_index++, 0xA0); // LDY imm
        relativeRomWrite(romBytes, program_index++, y);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x0501]);
        relativeRomWrite(romBytes, program_index++, 0x99); // STA aby
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0x02);
        relativeRomWrite(romBytes, program_index++, 0xB9); // LDA aby
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0x02);
        relativeRomWrite(romBytes, program_index++, 0x99); // STA aby
        relativeRomWrite(romBytes, program_index++, 0xF1);
        relativeRomWrite(romBytes, program_index++, 0x04);
        relativeRomWrite(romBytes, program_index++, 0xB9); // LDA aby page cross
        relativeRomWrite(romBytes, program_index++, 0xF1);
        relativeRomWrite(romBytes, program_index++, 0x04);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 24; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x0501], INITIAL_X, y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDA/STA INX") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        nes_byte x = 0x10;

        ram[0x0356] = (nes_byte) (rand() % 0x7D) + 1;
        ram[0x05] = 0x56;
        ram[0x06] = 0x03;
        relativeRomWrite(romBytes, program_index++, 0xA2); // LDX imm
        relativeRomWrite(romBytes, program_index++, x);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x56);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x05);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x03);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x0356]);
        relativeRomWrite(romBytes, program_index++, 0x81); // STA inx
        relativeRomWrite(romBytes, program_index++, 0xF5);
        relativeRomWrite(romBytes, program_index++, 0xA1); // LDA inx
        relativeRomWrite(romBytes, program_index++, 0xF5);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 27; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x356], x, INITIAL_Y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDA/STA INY") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        nes_byte y = 0x10;

        ram[0x0406] = (nes_byte) (rand() % 0x7D) + 1;
        ram[0x0446] = ram[0x0406];
        ram[0x05] = 0xF6;
        ram[0x06] = 0x03;
        ram[0x07] = 0x36;
        ram[0x08] = 0x04;
        relativeRomWrite(romBytes, program_index++, 0xA0); // LDY imm
        relativeRomWrite(romBytes, program_index++, y);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0xF6);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x05);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x03);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x06);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x36);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, 0x04);
        relativeRomWrite(romBytes, program_index++, 0x85); // STA zp
        relativeRomWrite(romBytes, program_index++, 0x08);
        relativeRomWrite(romBytes, program_index++, 0xA9); // LDA imm
        relativeRomWrite(romBytes, program_index++, ram[0x0406]);
        relativeRomWrite(romBytes, program_index++, 0x91); // STA iny
        relativeRomWrite(romBytes, program_index++, 0x05);
        relativeRomWrite(romBytes, program_index++, 0xB1); // LDA iny
        relativeRomWrite(romBytes, program_index++, 0x05);
        relativeRomWrite(romBytes, program_index++, 0x91); // STA iny
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0xB1); // LDA iny
        relativeRomWrite(romBytes, program_index++, 0x07);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 48; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                ram[0x406], INITIAL_X, y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    SECTION("LDX/STX ZPY") {
        nes_address program_start = 0x8000;
        nes_address program_index = program_start;
        setResetVector(romBytes, program_start);
        nes_byte ram[RAM_SIZE];
        for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
        nes_byte y = 0x33;

        ram[0x57] = (nes_byte) (rand() % 0x7D) + 1;
        relativeRomWrite(romBytes, program_index++, 0xA0); // LDY imm
        relativeRomWrite(romBytes, program_index++, 0x33);
        relativeRomWrite(romBytes, program_index++, 0xA2); // LDX imm
        relativeRomWrite(romBytes, program_index++, ram[0x57]);
        relativeRomWrite(romBytes, program_index++, 0x96); // STX zpy
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0xB6); // LDX zpy
        relativeRomWrite(romBytes, program_index++, 0x24);
        relativeRomWrite(romBytes, program_index++, 0xEA); // NOP

        M0 mapper(*romBytes, cartRAM, *chrBytes);
        CPU cpu(mapper, ppu);

        for (int i = 0; i < 13; i++) {
            cpu.Step();
        }

        auto output = cpu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(
                INITIAL_A, ram[0x57], y, INITIAL_S, INITIAL_P, program_index));
        REQUIRE(output->at(1).body == getRamString(ram));
        delete output;
    }

    // TODO: Addressing mode test coverage
//        INDIRECT, JMP
//        ACCUMULATOR, ROL/ROR
//        IMPLICIT,
//        RELATIVE, branch
    // TODO: test Instruction results
    // TODO: test Instruction status register results
    // TODO: test Instruction Duration
}
