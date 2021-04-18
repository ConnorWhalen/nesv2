//
// NES-V2: M1.cpp
//
// Created by Connor Whalen on 2019-12-15.
//

#include "M1.h"

#include <iomanip>
#include <sstream>


M1::M1(std::vector<unsigned char> *romBytes, const unsigned char *cartRAM,
       std::vector<unsigned char> *chrBytes) {
    for (int i = 0; i < romBytes->size(); i++) {
        prgRom[i] = (*romBytes)[i];
    }
    for (int i = 0; i < CART_RAM_SIZE; i++) {
        prgRam[i] = cartRAM[i];
    }
    for (int i = 0; i < chrBytes->size(); i++) {
        chr[i] = (*chrBytes)[i];
    }
    this->prgRomBankCount = romBytes->size()/MAPPER1_BANK_SIZE;
    this->prgRomBank1 = 0;
    this->prgRomBank2 = this->prgRomBankCount-1;
    this->shiftReg = 0x10;
    this->controlReg = 0x0C;
    this->chrBankCount = chrBytes->size()/MAPPER1_CHR_BANK_SIZE;
    this->ramEnable = true;
}

std::vector<OutputData>* M1::Serialize() {
    std::stringstream ss;
    ss << std::hex << this->prgRomBank1 << " " << this->prgRomBank2;

    if (prgRam[0] < 0x80) {
        printf("TEST RESULT:\n%s\n", &(prgRam[4]));
    }

    std::stringstream cartRamStream;
    int cart_ram_line_width = 0x20;
    int cart_ram_lines = 0x2000/cart_ram_line_width;
    for (int i = 0; i < cart_ram_lines; i++) {
        cartRamStream << std::setfill('0') << std::setw(4) << std::hex << i*cart_ram_line_width << ": ";
        for (int j = 0; j < cart_ram_line_width; j++) {
            cartRamStream << std::setfill('0') << std::setw(2) << std::hex << int(prgRam[i*cart_ram_line_width + j]) << " ";
        }
        cartRamStream << "\n";
    }

    return new std::vector<OutputData> {
            {"MAPPER 1 PRG ROM BANK NUMBER", ss.str()},
            {"MAPPER 1 RAM DUMP", cartRamStream.str()}
    };
}

nes_byte M1::DoRead(nes_address address) {
//    printf("MAPPER 1 READ ADDRESS %x\n", address);
    if (address < MAPPER_RAM_REGION_START) {
        printf("ERROR: INVALID MAPPER 1 READ ADDRESS %x\n", address);
        return 0;
    } else if (address < MAPPER_ROM_REGION_START) {
        if (ramEnable) {
            return prgRam[address - MAPPER_RAM_REGION_START];
        } else {
            printf("ERROR: MAPPER 1 DISABLED RAM READ %x\n", address);
            return 0;
        }
    } else if (address < MAPPER_ROM_REGION_MIDDLE) {
        return prgRom[prgRomBank1*MAPPER1_BANK_SIZE + (address % MAPPER1_BANK_SIZE)];
    }else {
        return prgRom[prgRomBank2*MAPPER1_BANK_SIZE + (address % MAPPER1_BANK_SIZE)];
    }
}

void M1::DoWrite(nes_address address, nes_byte value) {
//    printf("MAPPER 1 WRITE ADDRESS %x VALUE %x\n", address, value);
    if (address < MAPPER_RAM_REGION_START) {
        printf("ERROR: INVALID MAPPER 1 WRITE ADDRESS %x\n", address);
    } else if (address < MAPPER_ROM_REGION_START) {
        if (ramEnable) {
            prgRam[address - MAPPER_RAM_REGION_START] = value;
        } else {
            printf("ERROR: MAPPER 1 DISABLED RAM WRITE %x\n", address);
        }
    } else {
        DoShift(address, value);
    }
}

nes_byte M1::DoChrRead(nes_address address) {
    return 0; // TODO
}

void M1::DoChrWrite(nes_address address, nes_byte value) {
    // TODO
}

void M1::DoShift(nes_address address, nes_byte value) {
    if (value & 0x80) {
        shiftReg = 0x10;
        ShiftWrite(MAPPER1_CONTROL_REGION_START, 0x0C);
    } else if (shiftReg & 0x01) {
        nes_byte write_value = shiftReg >> 1;
        if (value & 0x01) write_value += 0x10;
        ShiftWrite(address, write_value);
        shiftReg = 0x10;
    } else {
        shiftReg = shiftReg >> 1;
        if (value & 0x01) shiftReg += 0x10;
    }
}

void M1::ShiftWrite(nes_address address, nes_byte value) {
    printf("SHIFT REGISTER WRITE ADDR %x VALUE %x\n", address, value);
    if (address < MAPPER1_CHR0_REGION_START) {
        // CONTROL
        controlReg = value;
        if (!(controlReg & 0x08)) {
            // 32kb switching
        } else if (!(controlReg & 0x04)) {
            // fix first bank
            prgRomBank1 = 0;
        } else {
            prgRomBank2 = prgRomBankCount-1;
        }
    } else if (address < MAPPER1_CHR1_REGION_START) {
        // CHR0
    } else if (address < MAPPER1_PRG_REGION_START) {
        // CHR1
    } else {
        // PRG
        if (!(controlReg & 0x08)) {
            // 32kb bank switching
            prgRomBank1 = (value & 0x0F) % 2;
            prgRomBank2 = (value & 0x0F) | 0x01;
        } else if (!(controlReg & 0x04)) {
            // fix first bank
            prgRomBank2 = (value & 0x0F);
        } else {
            // fix last bank
            prgRomBank1 = (value & 0x0F);
        }
        if (value & 0x10) {
            ramEnable = false;
        } else {
            ramEnable = true;
        }
    }
}
