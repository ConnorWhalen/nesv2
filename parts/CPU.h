//
// NES-V2: CPU.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_CPU_H
#define NESV2_CPU_H

#include "OutputData.h"
#include "Part.h"
#include "mappers/Mapper.h"

constexpr nes_address NMI_VECTOR_LOW = 0xFFFA;
constexpr nes_address NMI_VECTOR_HIGH = 0xFFFB;
constexpr nes_address RESET_VECTOR_HIGH = 0xFFFC;
constexpr nes_address RESET_VECTOR_LOW = 0xFFFD;
constexpr nes_address IRQ_BRK_VECTOR_HIGH = 0xFFFE;
constexpr nes_address IRQ_BRK_VECTOR_LOW = 0xFFFF;

constexpr nes_address RAM_SIZE = 0x800;

constexpr nes_address RAM_SEGMENT_END = 0x2000;
constexpr nes_address PPU_REGISTERS_END = 0x4000;
constexpr nes_address APU_REGISTERS_END = 0x4018;
constexpr nes_address DISABLED_REGION_END = 0x4020;
constexpr nes_address UNUSED_CARTRIDGE_REGION_END = 0x8000;

enum class CPU_ADDRESSING_MODE {
    ZERO_PAGE_X = 0,
    ZERO_PAGE_Y = 1,
    ABSOLUTE_X = 2,
    ABSOLUTE_Y = 3,
    INDIRECT_X = 4,
    INDIRECT_Y = 5,
};

class CPU : public Part {
public:
    CPU(Mapper* mapper);
    std::vector<OutputData>* Serialize() override;
    void Step() override;
private:
    nes_byte MemoryRead(nes_address address);
    void MemoryWrite(nes_address address, nes_byte value);
    nes_address GetAddress(CPU_ADDRESSING_MODE mode, nes_address argument);

    nes_byte A;
    nes_byte X;
    nes_byte Y;
    nes_byte P;
    nes_byte S;
    nes_address PC;
    Mapper* mapper;
    nes_byte RAM[RAM_SIZE];
};

#endif //NESV2_CPU_H
