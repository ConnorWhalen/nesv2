//
// NES-V2: CPU.cpp
//
// Created by Connor Whalen on 2019-10-02.
//

#include "CPU.h"

CPU::CPU(Mapper* mapper) {
    this->mapper = mapper;
    // http://wiki.nesdev.com/w/index.php/CPU
    this->STEPS_PER_TICK = (CLOCK_FREQUENCY/12)/FPS;
    // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    this->A = 0;
    this->X = 0;
    this->Y = 0;
    this->S = 0xFD;
    this->P = 0x34;
    this->PC = (this->MemoryRead(RESET_VECTOR_HIGH) << 8) + this->MemoryRead(RESET_VECTOR_LOW);
    this->wait_steps = 0;
}

std::vector<OutputData>* CPU::Serialize() {
    auto out = new std::vector<OutputData>();
    return out;
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
    bool page_crossed = false;
    nes_byte argument;
    nes_address address_argument;
    nes_address address;
    switch (addressing_mode) {
        case CPU_ADDRESSING_MODE::IMPLICIT:
            argument = 0;
            break;
        case CPU_ADDRESSING_MODE::IMMEDIATE:
        case CPU_ADDRESSING_MODE::RELATIVE:
            argument = MemoryRead(++PC);
            break;
        case CPU_ADDRESSING_MODE::ACCUMULATOR:
            argument = A;
            break;
        case CPU_ADDRESSING_MODE::ZERO_PAGE:
        case CPU_ADDRESSING_MODE::ZERO_PAGE_X:
        case CPU_ADDRESSING_MODE::ZERO_PAGE_Y:
        case CPU_ADDRESSING_MODE::INDIRECT:
        case CPU_ADDRESSING_MODE::INDIRECT_X:
        case CPU_ADDRESSING_MODE::INDIRECT_Y:
            address = GetAddress(addressing_mode, MemoryRead(++PC), page_crossed);
            argument = MemoryRead(address);
            break;
        case CPU_ADDRESSING_MODE::ABSOLUTE:
        case CPU_ADDRESSING_MODE::ABSOLUTE_X:
        case CPU_ADDRESSING_MODE::ABSOLUTE_Y:
            address_argument = MemoryRead(++PC);
            address_argument += (MemoryRead(++PC) << 8);
            address = GetAddress(addressing_mode, address_argument, page_crossed);
            argument = MemoryRead(address);
            break;
    }

    bool branch_succeeded = false;
    switch (opcode) {
        case OPCODE::ADC:
            ADC(argument);
            break;
        case OPCODE::AND:
            AND(argument);
            break;
        case OPCODE::ASL:
            ASL(argument, address, addressing_mode);
            break;
        case OPCODE::BCC:
            Branch(argument, !(P & CARRY_FLAG), branch_succeeded);
            break;
        case OPCODE::BCS:
            Branch(argument, (P & CARRY_FLAG), branch_succeeded);
            break;
        case OPCODE::BEQ:
            Branch(argument, (P & ZERO_FLAG), branch_succeeded);
            break;
        case OPCODE::BIT:
            BIT(argument);
            break;
        case OPCODE::BMI:
            Branch(argument, (P & NEGATIVE_FLAG), branch_succeeded);
            break;
        case OPCODE::BNE:
            Branch(argument, !(P & ZERO_FLAG), branch_succeeded);
            break;
        case OPCODE::BPL:
            Branch(argument, !(P & NEGATIVE_FLAG), branch_succeeded);
            break;
        case OPCODE::BRK:
            BRK(argument);
            break;
        case OPCODE::BVC:
            Branch(argument, !(P & OVERFLOW_FLAG), branch_succeeded);
            break;
        case OPCODE::BVS:
            Branch(argument, (P & OVERFLOW_FLAG), branch_succeeded);
            break;
        case OPCODE::CLC:
            ClearFlag(CARRY_FLAG);
            break;
        case OPCODE::CLD:
            ClearFlag(DECIMAL_FLAG);
            break;
        case OPCODE::CLI:
            ClearFlag(INTERRUPT_DISABLE_FLAG);
            break;
        case OPCODE::CLV:
            ClearFlag(OVERFLOW_FLAG);
            break;
        case OPCODE::CMP:
            Compare(argument, A);
            break;
        case OPCODE::CPX:
            Compare(argument, X);
            break;
        case OPCODE::CPY:
            Compare(argument, Y);
            break;
        case OPCODE::DEC:
            DEC(argument, address);
            break;
        case OPCODE::DEX:
            DEX();
            break;
        case OPCODE::DEY:
            DEY();
            break;
        case OPCODE::EOR:
            EOR(argument);
            break;
        case OPCODE::INC:
            INC(argument, address);
            break;
        case OPCODE::INX:
            INX();
            break;
        case OPCODE::INY:
            INY();
            break;
        case OPCODE::JMP:
            JMP(address);
            break;
        case OPCODE::JSR:
            JSR(address);
            break;
        case OPCODE::LDA:
            Load(A, argument);
            break;
        case OPCODE::LDX:
            Load(X, argument);
            break;
        case OPCODE::LDY:
            Load(Y, argument);
            break;
        case OPCODE::LSR:
            LSR(argument, address, addressing_mode);
            break;
        case OPCODE::NOP:
            break;
        case OPCODE::ORA:
            ORA(argument);
            break;
        case OPCODE::PHA:
            StackPush(A);
            break;
        case OPCODE::PHP:
            StackPush(P);
            break;
        case OPCODE::PLA:
            PullReg(A);
            break;
        case OPCODE::PLP:
            PullReg(P);
            break;
        case OPCODE::ROL:
            ROL(argument, address, addressing_mode);
            break;
        case OPCODE::ROR:
            ROR(argument, address, addressing_mode);
            break;
        case OPCODE::RTI:
            Return(true);
            break;
        case OPCODE::RTS:
            Return(false);
            break;
        case OPCODE::SBC:
            SBC(argument);
            break;
        case OPCODE::SEC:
            SetFlag(CARRY_FLAG);
            break;
        case OPCODE::SED:
            SetFlag(DECIMAL_FLAG);
            break;
        case OPCODE::SEI:
            SetFlag(INTERRUPT_DISABLE_FLAG);
            break;
        case OPCODE::STA:
            MemoryWrite(address, A);
            break;
        case OPCODE::STX:
            MemoryWrite(address, X);
            break;
        case OPCODE::STY:
            MemoryWrite(address, Y);
            break;
        case OPCODE::TAX:
            Transfer(A, X);
            break;
        case OPCODE::TAY:
            Transfer(A, Y);
            break;
        case OPCODE::TSX:
            Transfer(S, X);
            break;
        case OPCODE::TXA:
            Transfer(X, A);
            break;
        case OPCODE::TXS:
            Transfer(X, S, false);
            break;
        case OPCODE::TYA:
            Transfer(Y, A);
            break;
    }


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

nes_address CPU::GetAddress(CPU_ADDRESSING_MODE mode, nes_address argument, bool& page_crossed) {
    switch (mode) {
        case CPU_ADDRESSING_MODE::ZERO_PAGE:
            return argument;
        case CPU_ADDRESSING_MODE::ZERO_PAGE_X:
            return (nes_byte)(argument + X);
        case CPU_ADDRESSING_MODE::ZERO_PAGE_Y:
            return (nes_byte)(argument + Y);
        case CPU_ADDRESSING_MODE::ABSOLUTE:
            return argument;
        case CPU_ADDRESSING_MODE::ABSOLUTE_X:
            if ((argument % 0x100) + X > 0xFF) {
                page_crossed = true;
            }
            return argument + X;
        case CPU_ADDRESSING_MODE::ABSOLUTE_Y:
            if ((argument % 0x100) + Y > 0xFF) {
                page_crossed = true;
            }
            return argument + Y;
        case CPU_ADDRESSING_MODE::INDIRECT:
            return MemoryRead(argument) + (MemoryRead((nes_byte)(argument + 1)) << 8);
        case CPU_ADDRESSING_MODE::INDIRECT_X:
            return MemoryRead((nes_byte)(argument + X)) + (MemoryRead((nes_byte)(argument + X + 1)) << 8);
        case CPU_ADDRESSING_MODE::INDIRECT_Y:
            if (MemoryRead(argument) + Y > 0xFF) {
                page_crossed = true;
            }
            return MemoryRead(argument) + (MemoryRead((nes_byte)(argument + 1)) << 8) + Y;
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

void CPU::StackPush(nes_byte value) {
    MemoryWrite((nes_address) 0x100 + S++, value);
}

nes_byte CPU::StackPull() {
    return MemoryRead((nes_address) 0x100 + --S);
}

void CPU::EvaluateCarry(nes_address value, bool invert) {
    bool set_condition = value > 0xFF;
    if (set_condition != invert) {
        P |= CARRY_FLAG;
    } else {
        P &= ~CARRY_FLAG;
    }
}

void CPU::EvaluateZero(nes_address value) {
    if ((value % 0x100) == 0) {
        P |= ZERO_FLAG;
    } else {
        P &= ~ZERO_FLAG;
    }
}

void CPU::EvaluateOverflow(nes_address value, nes_byte argument, bool invert) {
    bool args_same_sign = (argument & 0x80) == (A & 0x80);
    bool result_different_sign = (A & 0x80) != (value & 0x80);
    if (args_same_sign != invert && result_different_sign) {
        P |= OVERFLOW_FLAG;
    } else {
        P &= ~OVERFLOW_FLAG;
    }
}

void CPU::EvaluateNegative(nes_address value) {
    if ((value & 0x80) != 0) {
        P |= NEGATIVE_FLAG;
    } else {
        P &= ~NEGATIVE_FLAG;
    }
}

void CPU::ADC(nes_byte argument) {
    nes_address result = argument + A;
    if (P & CARRY_FLAG) result++;

    EvaluateCarry(result);
    EvaluateZero(result);
    EvaluateOverflow(result, argument);
    EvaluateNegative(result);

    A = (nes_byte) result;
}

void CPU::AND(nes_byte argument) {
    nes_byte result = argument & A;

    EvaluateZero(result);
    EvaluateNegative(result);

    A = result;
}

void CPU::ASL(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode) {
    nes_address result = argument << 1;

    EvaluateCarry(result);
    EvaluateZero(result);
    EvaluateNegative(result);

    if (mode == CPU_ADDRESSING_MODE::ACCUMULATOR) {
        A = (nes_byte) result;
    } else {
        MemoryWrite(address, (nes_byte) result);
    }

}

void CPU::Branch(nes_byte argument, bool condition, bool& branch_succeeded) {
    if (condition) {
        PC += (char) argument;
        branch_succeeded = true;
    }
}

void CPU::BIT(nes_byte argument) {
    EvaluateZero(argument & A);
    EvaluateNegative(argument);
    if (argument & 0x40) P |= OVERFLOW_FLAG; else P &= ~OVERFLOW_FLAG;
}

void CPU::BRK(nes_byte argument) {
    // TODO: Implement interrupt hijacking
    // http://wiki.nesdev.com/w/index.php/CPU_interrupts
    P |= B_FLAG;
    StackPush((nes_byte) (PC >> 8));
    StackPush((nes_byte) PC);
    StackPush(P);
    PC = MemoryRead(IRQ_BRK_VECTOR_LOW) + (MemoryRead(IRQ_BRK_VECTOR_HIGH) << 8);
    P |= INTERRUPT_DISABLE_FLAG;
}

void CPU::ClearFlag(nes_byte flag) {
    P &= ~flag;
}

void CPU::Compare(nes_byte argument, nes_byte reg) {
    char reg_signed = reg;
    char arg_signed = argument;

    if (reg_signed >= arg_signed) P |= CARRY_FLAG; else P &= ~CARRY_FLAG;
    if (reg_signed == arg_signed) P |= ZERO_FLAG; else P &= ~ZERO_FLAG;
    if (reg_signed < arg_signed) P|= NEGATIVE_FLAG; else P &= ~NEGATIVE_FLAG;
}

void CPU::DEC(nes_byte argument, nes_address address) {
    nes_byte result =  argument - (nes_byte) 1;

    EvaluateZero(result);
    EvaluateNegative(result);

    MemoryWrite(address, result);
}

void CPU::DEX() {
    nes_byte result =  X - (nes_byte) 1;

    EvaluateZero(result);
    EvaluateNegative(result);

    X = result;
}

void CPU::DEY() {
    nes_byte result =  Y - (nes_byte) 1;

    EvaluateZero(result);
    EvaluateNegative(result);

    Y = result;
}

void CPU::EOR(nes_byte argument) {
    nes_byte result = A ^ argument;

    EvaluateZero(result);
    EvaluateNegative(result);

    A = result;
}

void CPU::INC(nes_byte argument, nes_address address) {
    nes_byte result =  argument + (nes_byte) 1;

    EvaluateZero(result);
    EvaluateNegative(result);

    MemoryWrite(address, result);
}

void CPU::INX() {
    nes_byte result =  X + (nes_byte) 1;

    EvaluateZero(result);
    EvaluateNegative(result);

    X = result;
}

void CPU::INY() {
    nes_byte result =  Y + (nes_byte) 1;

    EvaluateZero(result);
    EvaluateNegative(result);

    Y = result;
}

void CPU::JMP(nes_address address) {
    PC = address;
}

void CPU::JSR(nes_address address) {
    StackPush((nes_byte) (PC >> 8));
    StackPush((nes_byte) PC);
    PC = address;
}

void CPU::Load(nes_byte& reg, nes_byte value) {
    EvaluateZero(value);
    EvaluateNegative(value);

    reg = value;
}

void CPU::LSR(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode) {
    nes_byte result = argument >> 1;

    if (argument & 0x01) P |= CARRY_FLAG; else P &= ~CARRY_FLAG;
    EvaluateZero(result);
    EvaluateNegative(result);

    if (mode == CPU_ADDRESSING_MODE::ACCUMULATOR) {
        A = (nes_byte) result;
    } else {
        MemoryWrite(address, (nes_byte) result);
    }
}

void CPU::ORA(nes_byte argument) {
    nes_byte result = argument | A;

    EvaluateZero(result);
    EvaluateNegative(result);

    A = result;
}

void CPU::PullReg(nes_byte &reg) {
    nes_byte result = StackPull();

    EvaluateZero(result);
    EvaluateNegative(result);

    reg = result;
}

void CPU::ROL(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode) {
    nes_byte result = argument << 1;
    if (P & CARRY_FLAG) result |= 0x01;

    if (argument & 0x80) P |= CARRY_FLAG; else P &= ~CARRY_FLAG;
    EvaluateZero(result);
    EvaluateNegative(result);

    if (mode == CPU_ADDRESSING_MODE::ACCUMULATOR) {
        A = (nes_byte) result;
    } else {
        MemoryWrite(address, (nes_byte) result);
    }
}

void CPU::ROR(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode) {
    nes_byte result = argument >> 1;
    if (P & CARRY_FLAG) result |= 0x80;

    if (argument & 0x01) P |= CARRY_FLAG; else P &= ~CARRY_FLAG;
    EvaluateZero(result);
    EvaluateNegative(result);

    if (mode == CPU_ADDRESSING_MODE::ACCUMULATOR) {
        A = (nes_byte) result;
    } else {
        MemoryWrite(address, (nes_byte) result);
    }
}

void CPU::Return(bool is_interrupt) {
    if (is_interrupt) P = StackPull();
    nes_byte lower = StackPull();
    nes_byte upper = StackPull();
    PC = lower + ((nes_address) upper << 8);
}

void CPU::SBC(nes_byte argument) {
    nes_address result = A - argument;
    if (!(P & CARRY_FLAG)) result--;

    EvaluateCarry(result, true);
    EvaluateZero(result);
    EvaluateOverflow(result, argument, true);
    EvaluateNegative(result);

    A = (nes_byte) result;
}

void CPU::SetFlag(nes_byte flag) {
    P |= flag;
}

void CPU::Transfer(nes_byte &from, nes_byte &to, bool flags) {
    to = from;

    if (flags) {
        EvaluateZero(to);
        EvaluateNegative(to);
    }
}
