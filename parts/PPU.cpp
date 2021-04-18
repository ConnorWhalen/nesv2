//
// NES-V2: PPU.cpp
//
// Created by Connor Whalen on 2019-12-19.
//

#include "PPU.h"

std::vector<OutputData>* PPU::Serialize() {
    return new std::vector<OutputData> {
        {"PPU", "peeeeep"}
    };
}

PPU::PPU() {
	ppuCtrl = 0;
}

nes_byte PPU::Read(nes_address address) {
	return 0;
}

void PPU::Write(nes_address address, nes_byte value) {
	if (address % 0x08 == 0) {
		if ((ppuCtrl & 0x80) && !(value & 0x80)) {
			printf("NMI disabled");
		} else if (!(ppuCtrl & 0x80) && (value & 0x80)) {
			printf("NMI enambled");
		}
		ppuCtrl = value;
	}
}

bool PPU::DoNMI() {
	return (ppuCtrl & 0x80);
}

