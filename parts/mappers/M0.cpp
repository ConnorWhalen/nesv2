//
// NES-V2: M0.cpp
//
// Created by Connor Whalen on 2019-09-29.
//

#include "M0.h"

#include <iomanip>
#include <sstream>

M0::M0(std::vector<unsigned char> *romBytes, const unsigned char *cartRAM) {
    int s = romBytes->size();
    for (int i = 0; i < MAPPER0_SIZE; i++) {
        bytes[i] = (*romBytes)[i % s];
    }
    for (int i = 0; i < CART_RAM_SIZE; i++) {
        ram[i] = cartRAM[i];
    }

}

std::vector<OutputData>* M0::Serialize() {
    int cols = 32;
    int rows = MAPPER0_SIZE/cols/32;
    std::stringstream stream;
    stream << std::hex;
    for (int i = 0; i < rows; i++) {
        stream << std::setw(4) << std::setfill('0') << i*cols << ": ";
        for (int j = 0; j < cols; j++) {
            stream << std::setw(2) << std::setfill('0') << (int) bytes[i*cols+j] << " ";
        }
        stream << "\n";
    }

    std::stringstream cartRamStream;
    int cart_ram_line_width = 0x20;
    int cart_ram_lines = 0x2000/cart_ram_line_width;
    for (int i = 0; i < cart_ram_lines; i++) {
        cartRamStream << std::setfill('0') << std::setw(4) << std::hex << i*cart_ram_line_width << ": ";
        for (int j = 0; j < cart_ram_line_width; j++) {
            cartRamStream << std::setfill('0') << std::setw(2) << std::hex << int(ram[i*cart_ram_line_width + j]) << " ";
        }
        cartRamStream << "\n";
    }

    return new std::vector<OutputData> {
            {"MAPPER 0 ROM DUMP", stream.str()},
            {"MAPPER 0 RAM DUMP", cartRamStream.str()}
    };
}

nes_byte M0::DoRead(nes_address address) {
    if (address < MAPPER_RAM_REGION_START) {
        printf("ERROR: INVALID MAPPER 0 READ ADDRESS %x\n", address);
    } else if (address < MAPPER_ROM_REGION_START) {
        return ram[address - MAPPER_RAM_REGION_START];
    } else {
        return bytes[address-MAPPER0_SIZE];
    }
}

void M0::DoWrite(nes_address address, nes_byte value) {
    if (address >= MAPPER_RAM_REGION_START && address < MAPPER_ROM_REGION_START) {
        ram[address - MAPPER_RAM_REGION_START] = value;
    } else {
        printf("WARNING: WROTE TO MAPPER 0: Addr %x Value %x\n", address, value);
    }
}
