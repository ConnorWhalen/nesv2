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
    this->wait_steps = 0;
}

std::vector<OutputData>* CPU::Serialize() {
    return new std::vector<OutputData>();
};

void CPU::Step() {
    if (--this->wait_steps < 1) {
        Instruction(MemoryRead(++PC));
    }
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

void CPU::Instruction(nes_byte opcode) {
    // TODO: Route the rest of the opcodes
    switch (opcode) {
        case 0x69:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0x7D:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x90:
            Execute(OPCODE::BCC, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::BRANCH_INSTRUCTION);
            break;
        default:
            break;
    }
}

nes_address CPU::GetAddress(CPU_ADDRESSING_MODE mode, nes_address argument) {
    switch (mode) {
        case CPU_ADDRESSING_MODE::ZERO_PAGE:
            return argument;
        case CPU_ADDRESSING_MODE::ZERO_PAGE_X:
            return (argument + X) % 0x100;
        case CPU_ADDRESSING_MODE::ZERO_PAGE_Y:
            return (argument + Y) % 0x100;
        case CPU_ADDRESSING_MODE::ABSOLUTE:
            return argument;
        case CPU_ADDRESSING_MODE::ABSOLUTE_X:
            return argument + X;
        case CPU_ADDRESSING_MODE::ABSOLUTE_Y:
            return argument + Y;
        case CPU_ADDRESSING_MODE::INDIRECT:
            return MemoryRead(argument);
        case CPU_ADDRESSING_MODE::INDIRECT_X:
            return MemoryRead((argument + X) % 256) + (MemoryRead((argument + X + 1) % 256) << 8);
        case CPU_ADDRESSING_MODE::INDIRECT_Y:
            return MemoryRead(argument) + (MemoryRead((argument + 1) % 256) << 8) + Y;
        default:
            break;
    }
    printf("ERROR: ATTEMPTED TO GET ADDRESS FOR INVALID CPU ADDRESSING MODE %d", mode);
    return 0;
}

void CPU::Execute(OPCODE opcode, CPU_ADDRESSING_MODE addressing_mode, OOPS oops) {
    // TODO: Actually execute instruction

    // TODO: replace dummy values with actual values
    bool branch_succeeded = false;
    bool page_crossed = false;
    int wait_cycles = 0;
    switch (opcode) {
        case OPCODE::JMP:
            wait_cycles = -1;
            break;
        case OPCODE::PHA:
        case OPCODE::PHP:
            wait_cycles = 1;
            break;
        case OPCODE::BCC:
        case OPCODE::BCS:
        case OPCODE::BEQ:
        case OPCODE::BMI:
        case OPCODE::BNE:
        case OPCODE::BPL:
        case OPCODE::BVC:
        case OPCODE::BVS:
        case OPCODE::DEC:
        case OPCODE::INC:
        case OPCODE::JSR:
        case OPCODE::LSR:
        case OPCODE::PLA:
        case OPCODE::PLP:
        case OPCODE::ROL:
        case OPCODE::ROR:
            wait_cycles = 2;
            break;
        case OPCODE::RTI:
        case OPCODE::RTS:
            wait_cycles = 4;
            break;
        case OPCODE::BRK:
            wait_cycles = 5;
            break;
        default:
            break;
    }

    switch (addressing_mode) {
        case CPU_ADDRESSING_MODE::ACCUMULATOR:
            break;
        case CPU_ADDRESSING_MODE::IMPLICIT:
        case CPU_ADDRESSING_MODE::IMMEDIATE:
            wait_cycles += 2;
            break;
        case CPU_ADDRESSING_MODE::ZERO_PAGE:
            wait_cycles += 3;
            break;
        case CPU_ADDRESSING_MODE::ZERO_PAGE_X:
        case CPU_ADDRESSING_MODE::ZERO_PAGE_Y:
        case CPU_ADDRESSING_MODE::ABSOLUTE:
            wait_cycles += 4;
            break;
        case CPU_ADDRESSING_MODE::ABSOLUTE_X:
        case CPU_ADDRESSING_MODE::ABSOLUTE_Y:
            wait_cycles += 5;
            switch (oops) {
                case OOPS::NONE:
                    wait_cycles += 1;
                    break;
                case OOPS::ADD_IF_PAGE_CROSSED:
                    if (page_crossed) {
                        wait_cycles += 1;
                    }
                    break;
                default:
                    printf("INVALID OOPS SETTING %d FOR ADDRESSING_MODE %d", oops, addressing_mode);
                    break;
            }
            break;
        case CPU_ADDRESSING_MODE ::INDIRECT:
        case CPU_ADDRESSING_MODE::INDIRECT_X:
            wait_cycles += 6;
            break;
        case CPU_ADDRESSING_MODE::INDIRECT_Y:
            wait_cycles += 5;
            switch (oops) {
                case OOPS::NONE:
                    wait_cycles += 1;
                    break;
                case OOPS::ADD_IF_PAGE_CROSSED:
                    if (page_crossed) {
                        wait_cycles += 1;
                    }
                    break;
                default:
                    printf("INVALID OOPS SETTING %d FOR ADDRESSING_MODE %d", oops, addressing_mode);
                    break;
            }
            break;
        case CPU_ADDRESSING_MODE ::RELATIVE:
            if (branch_succeeded) {
                wait_cycles += 1;
                if (page_crossed) {
                    wait_cycles += 1;
                }
            }
            break;
    }
    this->wait_steps += wait_cycles;
}
