//
// NES-V2: PPU.cpp
//
// Created by Connor Whalen on 2019-12-19.
//

#include <string>
#include "PPU.h"

std::vector<OutputData>* PPU::Serialize() {
    return new std::vector<OutputData> {
        // {"Colour", std::string(colour)}
    };
}

PPU::PPU(Display* display) {
	http://wiki.nesdev.com/w/index.php/Cycle_reference_chart#Clock_rates
    this->STEPS_PER_TICK = (CLOCK_FREQUENCY/4)/FPS;
	ppuCtrl = 0;
	this->display = display;

	counter = 0;
	colour = 0;
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

void PPU::Step() {
	// nes_byte colourValue = paletteIndices[colourAddress] & 0x3f;
	// if (greyscale) colourValue &= 0x30;
	// display->SetPixel(0, 0, colourValue);

	if (++counter > STEPS_PER_TICK) {
		counter = 0;
	} else {
		return;
	}

	for (int i = 0; i < NES_HEIGHT; i++) {
		for (int j = 0; j < NES_WIDTH; j++) {
			display->SetPixel(j, i, colour);
		}
	}
	colour++;
}
