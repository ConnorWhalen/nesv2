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
constexpr nes_address RESET_VECTOR_LOW = 0xFFFC;
constexpr nes_address RESET_VECTOR_HIGH = 0xFFFD;
constexpr nes_address IRQ_BRK_VECTOR_LOW = 0xFFFE;
constexpr nes_address IRQ_BRK_VECTOR_HIGH = 0xFFFF;

constexpr nes_address RAM_SIZE = 0x800;

constexpr nes_address RAM_SEGMENT_END = 0x2000;
constexpr nes_address PPU_REGISTERS_END = 0x4000;
constexpr nes_address APU_REGISTERS_END = 0x4018;
constexpr nes_address DISABLED_REGION_END = 0x4020;
constexpr nes_address CARTRIDGE_RAM_REGION_END = 0x8000;

constexpr nes_byte CARRY_FLAG = 0x01;
constexpr nes_byte ZERO_FLAG = 0x02;
constexpr nes_byte INTERRUPT_DISABLE_FLAG = 0x04;
constexpr nes_byte DECIMAL_FLAG = 0x08;
constexpr nes_byte OVERFLOW_FLAG = 0x40;
constexpr nes_byte NEGATIVE_FLAG = 0x80;
constexpr nes_byte B_FLAG_INST = 0x30;
constexpr nes_byte B_FLAG_INTR = 0x20;

constexpr nes_byte INITIAL_A = 0x00;
constexpr nes_byte INITIAL_X = 0x00;
constexpr nes_byte INITIAL_Y = 0x00;
constexpr nes_byte INITIAL_S = 0xFD;
constexpr nes_byte INITIAL_P = 0x34;

enum class CPU_ADDRESSING_MODE {
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT_X,
    INDIRECT_Y,
    ZERO_PAGE,
    IMMEDIATE,
    ABSOLUTE,
    INDIRECT,
    ACCUMULATOR,
    IMPLICIT,
    RELATIVE,
};

// "Oops" cycle cases. Situations where an instruction's result affects its timing
enum class OOPS {
    NONE,
    ADD_IF_PAGE_CROSSED,
    BRANCH_INSTRUCTION,
};

enum class OPCODE {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
    JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
    RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
};

class CPU : public Part {
public:
    CPU(Mapper* mapper);
    std::vector<OutputData>* Serialize() override;
    void Step() override;
    void Reset();
    void NMI();
private:
    nes_byte MemoryRead(nes_address address);
    void MemoryWrite(nes_address address, nes_byte value);
    nes_address GetAddress(CPU_ADDRESSING_MODE mode, nes_address argument, bool& page_crossed);
    void Instruction(nes_byte opcode);
    void Execute(OPCODE opcode, CPU_ADDRESSING_MODE addressing_mode, OOPS oops);

    nes_byte A;
    nes_byte X;
    nes_byte Y;
    nes_byte P;
    nes_byte S;
    nes_address PC;
    Mapper* mapper;
    nes_byte RAM[RAM_SIZE];
    int wait_steps;

    void StackPush(nes_byte value);
    nes_byte StackPull();
    void EvaluateCarry(nes_address value, bool invert=false);
    void EvaluateZero(nes_address value);
    void EvaluateOverflow(nes_address value, nes_byte argument, bool invert=false);
    void EvaluateNegative(nes_address value);

    void ADC(nes_byte argument);
    void AND(nes_byte argument);
    void ASL(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode);
    void Branch(nes_byte argument, bool condition, bool& branch_succeeded);
    void BIT(nes_byte argument);
    void BRK(nes_byte argument);
    void ClearFlag(nes_byte flag);
    void Compare(nes_byte argument, nes_byte reg);
    void DEC(nes_byte argument, nes_address address);
    void DEX();
    void DEY();
    void EOR(nes_byte argument);
    void INC(nes_byte argument, nes_address address);
    void INX();
    void INY();
    void JMP(nes_address address);
    void JSR(nes_address address);
    void Load(nes_byte& reg, nes_byte value);
    void LSR(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode);
    void ORA(nes_byte argument);
    void PullReg(nes_byte& reg);
    void ROL(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode);
    void ROR(nes_byte argument, nes_address address, CPU_ADDRESSING_MODE mode);
    void Return(bool is_interrupt);
    void SBC(nes_byte argument);
    void SetFlag(nes_byte flag);
    void Transfer(nes_byte& from, nes_byte& to, bool flags=true);
};

#endif //NESV2_CPU_H
