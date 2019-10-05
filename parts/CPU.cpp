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

void CPU::Instruction(nes_byte opcode) {
    switch (opcode) {
        case 0x69:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0x65:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x75:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x6D:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x7D:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x79:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x61:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0x71:
            Execute(OPCODE::ADC, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x29:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0x25:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x35:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x2D:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x3D:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x39:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x21:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0x31:
            Execute(OPCODE::AND, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x0A:
            Execute(OPCODE::ASL, CPU_ADDRESSING_MODE::ACCUMULATOR, OOPS::NONE);
            break;
        case 0x06:
            Execute(OPCODE::ASL, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x16:
            Execute(OPCODE::ASL, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x0E:
            Execute(OPCODE::ASL, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x1E:
            Execute(OPCODE::ASL, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0x90:
            Execute(OPCODE::BCC, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0xB0:
            Execute(OPCODE::BCS, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0xF0:
            Execute(OPCODE::BEQ, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0x30:
            Execute(OPCODE::BMI, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0x24:
            Execute(OPCODE::BIT, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x2C:
            Execute(OPCODE::BIT, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xD0:
            Execute(OPCODE::BNE, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0x10:
            Execute(OPCODE::BPL, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0x00:
            Execute(OPCODE::BRK, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x50:
            Execute(OPCODE::BVC, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0x70:
            Execute(OPCODE::BVS, CPU_ADDRESSING_MODE::RELATIVE, OOPS::BRANCH_INSTRUCTION);
            break;
        case 0x18:
            Execute(OPCODE::CLC, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xD8:
            Execute(OPCODE::CLD, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x58:
            Execute(OPCODE::CLI, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xB8:
            Execute(OPCODE::CLV, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xC9:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xC5:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xD5:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0xCD:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xDD:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xD9:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xC1:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0xD1:
            Execute(OPCODE::CMP, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xE0:
            Execute(OPCODE::CPX, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xE4:
            Execute(OPCODE::CPX, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xEC:
            Execute(OPCODE::CPX, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xC0:
            Execute(OPCODE::CPY, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xC4:
            Execute(OPCODE::CPY, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xCC:
            Execute(OPCODE::CPY, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xC6:
            Execute(OPCODE::DEC, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xD6:
            Execute(OPCODE::DEC, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0xCE:
            Execute(OPCODE::DEC, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xDE:
            Execute(OPCODE::DEC, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0xCA:
            Execute(OPCODE::DEX, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x88:
            Execute(OPCODE::DEY, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x49:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0x45:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x55:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x4D:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x5D:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x59:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x41:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0x51:
            Execute(OPCODE::EOR, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xE6:
            Execute(OPCODE::INC, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xF6:
            Execute(OPCODE::INC, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0xEE:
            Execute(OPCODE::INC, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xFE:
            Execute(OPCODE::INC, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0xE8:
            Execute(OPCODE::INX, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xC8:
            Execute(OPCODE::INY, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x4C:
            Execute(OPCODE::JMP, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x6C:
            Execute(OPCODE::JMP, CPU_ADDRESSING_MODE::INDIRECT, OOPS::NONE);
            break;
        case 0x20:
            Execute(OPCODE::JSR, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xA9:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xA5:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xB5:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0xAD:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xBD:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xB9:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xA1:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0xB1:
            Execute(OPCODE::LDA, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xA2:
            Execute(OPCODE::LDX, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xA6:
            Execute(OPCODE::LDX, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xB6:
            Execute(OPCODE::LDX, CPU_ADDRESSING_MODE::ZERO_PAGE_Y, OOPS::NONE);
            break;
        case 0xAE:
            Execute(OPCODE::LDX, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xBE:
            Execute(OPCODE::LDX, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xA0:
            Execute(OPCODE::LDY, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xA4:
            Execute(OPCODE::LDY, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xB4:
            Execute(OPCODE::LDY, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0xAC:
            Execute(OPCODE::LDY, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xBC:
            Execute(OPCODE::LDY, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x4A:
            Execute(OPCODE::LSR, CPU_ADDRESSING_MODE::ACCUMULATOR, OOPS::NONE);
            break;
        case 0x46:
            Execute(OPCODE::LSR, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x56:
            Execute(OPCODE::LSR, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x4E:
            Execute(OPCODE::LSR, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x5E:
            Execute(OPCODE::LSR, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0xEA:
            Execute(OPCODE::NOP, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x09:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0x05:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x15:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x0D:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x1D:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x19:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x01:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0x11:
            Execute(OPCODE::ORA, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x48:
            Execute(OPCODE::PHA, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x08:
            Execute(OPCODE::PHP, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x68:
            Execute(OPCODE::PLA, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x28:
            Execute(OPCODE::PLP, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x2A:
            Execute(OPCODE::ROL, CPU_ADDRESSING_MODE::ACCUMULATOR, OOPS::NONE);
            break;
        case 0x26:
            Execute(OPCODE::ROL, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x36:
            Execute(OPCODE::ROL, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x2E:
            Execute(OPCODE::ROL, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x3E:
            Execute(OPCODE::ROL, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0x6A:
            Execute(OPCODE::ROR, CPU_ADDRESSING_MODE::ACCUMULATOR, OOPS::NONE);
            break;
        case 0x66:
            Execute(OPCODE::ROR, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x76:
            Execute(OPCODE::ROR, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x6E:
            Execute(OPCODE::ROR, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x7E:
            Execute(OPCODE::ROR, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0x40:
            Execute(OPCODE::RTI, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x60:
            Execute(OPCODE::RTS, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xE9:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::IMMEDIATE, OOPS::NONE);
            break;
        case 0xE5:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0xF5:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0xED:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xFD:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xF9:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0xE1:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0xF1:
            Execute(OPCODE::SBC, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::ADD_IF_PAGE_CROSSED);
            break;
        case 0x38:
            Execute(OPCODE::SEC, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xF8:
            Execute(OPCODE::SED, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x78:
            Execute(OPCODE::SEI, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x85:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x95:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x8D:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x9D:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::ABSOLUTE_X, OOPS::NONE);
            break;
        case 0x99:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::ABSOLUTE_Y, OOPS::NONE);
            break;
        case 0x81:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::INDIRECT_X, OOPS::NONE);
            break;
        case 0x91:
            Execute(OPCODE::STA, CPU_ADDRESSING_MODE::INDIRECT_Y, OOPS::NONE);
            break;
        case 0x86:
            Execute(OPCODE::STX, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x96:
            Execute(OPCODE::STX, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x8E:
            Execute(OPCODE::STX, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0x84:
            Execute(OPCODE::STY, CPU_ADDRESSING_MODE::ZERO_PAGE, OOPS::NONE);
            break;
        case 0x94:
            Execute(OPCODE::STY, CPU_ADDRESSING_MODE::ZERO_PAGE_X, OOPS::NONE);
            break;
        case 0x8C:
            Execute(OPCODE::STY, CPU_ADDRESSING_MODE::ABSOLUTE, OOPS::NONE);
            break;
        case 0xAA:
            Execute(OPCODE::TAX, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xA8:
            Execute(OPCODE::TAY, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0xBA:
            Execute(OPCODE::TSX, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x8A:
            Execute(OPCODE::TXA, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x9A:
            Execute(OPCODE::TXS, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        case 0x98:
            Execute(OPCODE::TYA, CPU_ADDRESSING_MODE::IMPLICIT, OOPS::NONE);
            break;
        default:
            printf("ERROR: ILLEGAL OPCODE %d\n", opcode);
            break;
    }
}
