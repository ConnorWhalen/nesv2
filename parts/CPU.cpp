//
// NES-V2: CPU.cpp
//
// Created by Connor Whalen on 2019-10-02.
//

#include "CPU.h"

CPU::CPU(Mapper* mapper) {
    // http://wiki.nesdev.com/w/index.php/CPU
    this->STEPS_PER_TICK = (CLOCK_FREQUENCY/12)/FPS;
    // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    this->A = 0;
    this->X = 0;
    this->Y = 0;
    this->S = 0xFD;
    this->P = 0x34;
    this->PC = (this->MemoryRead(RESET_VECTOR_HIGH) << 8) + this->MemoryRead(RESET_VECTOR_LOW);
    this->mapper = mapper;
}

std::vector<OutputData>* CPU::Serialize() {
    return new std::vector<OutputData>();
};

void CPU::Step() {
    // weeeeeeee
}

nes_byte CPU::MemoryRead(nes_address address) {
    if (address < RAM_SEGMENT_END) {
        return RAM[address % RAM_SIZE];
    } else if (address < PPU_REGISTERS_END) {
        return 0; // TODO: PPU read
    } else if (address < APU_REGISTERS_END) {
        return 0; // TODO: APU read
    } else if (address < DISABLED_REGION_END) {
        printf("WARNING: READ FROM DISABLED REGION ADDRESS %d\n", address);
        return 0;
    } else if (address < UNUSED_CARTRIDGE_REGION_END) {
        // TODO: Unused cartridge region
        printf("WARNING: READ FROM UNUSED CARTRIDGE REGION ADDRESS %d\n", address);
        return 0;
    } else {
        return mapper->Read(address);
    }
}

void CPU::MemoryWrite(nes_address address, nes_byte value) {
    if (address < RAM_SEGMENT_END) {
        RAM[address % RAM_SIZE] = value;
    } else if (address < PPU_REGISTERS_END) {
        // TODO: PPU read
    } else if (address < APU_REGISTERS_END) {
        // TODO: APU read
    } else if (address < DISABLED_REGION_END) {
        printf("WARNING: WRITE TO DISABLED REGION ADDRESS %d VALUE %d\n", address, value);
    } else if (address < UNUSED_CARTRIDGE_REGION_END) {
        // TODO: Unused cartridge region
        printf("WARNING: WRITE TO UNUSED CARTRIDGE REGION ADDRESS %d VALUE %d\n", address, value);
    } else {
        return mapper->Write(address, value);
    }
}

nes_address CPU::GetAddress(CPU_ADDRESSING_MODE mode, nes_address argument) {
    switch (mode) {
        case CPU_ADDRESSING_MODE::ZERO_PAGE_X:
            return (argument + X) % 0x100;
        case CPU_ADDRESSING_MODE::ZERO_PAGE_Y:
            return (argument + Y) % 0x100;
        case CPU_ADDRESSING_MODE::ABSOLUTE_X:
            return argument + X;
        case CPU_ADDRESSING_MODE::ABSOLUTE_Y:
            return argument + Y;
        case CPU_ADDRESSING_MODE::INDIRECT_X:
            return MemoryRead((argument + X) % 256) + MemoryRead((argument + X + 1) % 256) << 8;
        case CPU_ADDRESSING_MODE::INDIRECT_Y:
            return MemoryRead(argument) + (MemoryRead((argument + 1) % 256) << 8) + Y;
    }
    printf("ERROR: UNKNOWN CPU ADDRESSING MODE %d", mode);
}
