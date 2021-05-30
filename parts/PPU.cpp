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

    ppuStatus = 0;
    ppuCtrl = 0;
    ppuMask = 0;
    oamAddr = 0;
    ppuScroll = 0;
    ppuAddr = 0;
    ppuData = 0;

    ppuMask_showSprites = false;
    ppuMask_showBackground = false;
    ppuMask_showSpritesLeft8 = false;
    ppuMask_showBackgroundLeft8 = false;
    ppuMask_greyscale = false;

	oddFrame = false;
	scanline = -1;
	fineXScroll = 0;
	spriteCount = 0;
	hasSpriteZero = false;
}

nes_byte PPU::Read(nes_address address) {
	return 0;
}

void PPU::Write(nes_address address, nes_byte value) {
	address = address % 8;
    switch (address) {
        case 0:
			ppuCtrl = value;
			break;
		case 1:
			ppuMask = value;
			display->SetColourEmphasis((value & 0x20), (value & 0x40), (value & 0x80));
			ppuMask_showSprites = (value & 0x10);
			ppuMask_showBackground = (value & 0x08);
			ppuMask_showSpritesLeft8 = (value & 0x04);
			ppuMask_showBackgroundLeft8 = (value & 0x02);
			ppuMask_greyscale = (value & 0x01);
			break;
		case 2:
			ppuStatus = (value & 0x1f) + (ppuStatus & 0xe0);
			break;
		case 3:
			oamAddr = value;
			break;
		case 4:
			oam[oamAddr++] = value;
			break;
		case 5:
			ppuScroll = value;
			break;
		case 6:
			ppuAddr = value;
			break;
		case 7:
			break;
	}

}

bool PPU::DoNMI() {
	return (ppuCtrl & 0x80);
}

void PPU::Step() {
	// nes_byte colourValue = paletteIndices[colourAddress] & 0x3f;
	// if (greyscale) colourValue &= 0x30;
	// display->SetPixel(0, 0, colourValue);
}

// covers draw routine from execute, spriteZero, drawSprite, backgroundTransparent, spritePriority, spriteHere 
void PPU::DrawPixel(nes_byte x) {
	bool showBackground = ppuMask_showBackground && (x > 7 || ppuMask_showBackgroundLeft8);
	bool showSprites = ppuMask_showSprites && (x > 7 || ppuMask_showSpritesLeft8);
	nes_byte backgroundColor = currentTiles[(x % 8) + fineXScroll];
	// transparent
	SetDisplayPixel(x, scanline);
	if (showBackground) {
		SetDisplayPixel(x, scanline, backgroundColor);
	}
	if (showSprites) {
		unsigned char tile0 = 0;
		unsigned char tile1 = 0;
		unsigned char attribute = 0;
		unsigned char xOffset = 0;
		for (int i = 0; i < spriteCount; i++){
			if (currentSprites[i*4 + 3] <= x && currentSprites[i*4 + 3] > x - 8 &&
				// sprite priority or background transparent
				((currentSprites[i*4 + 2] & 0x20) == 0x20 || !showBackground || backgroundColor % 4 == 0)) {
				tile0 = spriteTiles[i*2];
				tile1 = spriteTiles[i*2 + 1];
				attribute = currentSprites[i*4 + 2] & 0x03;
				xOffset = x - currentSprites[i*4 + 3];

				unsigned char colourAddress = (tile0 >> (7 - xOffset)) & 0x01;
				colourAddress += (tile1 >> (7 - xOffset)) & 0x02;
				colourAddress += attribute << 2;
				if (colourAddress % 4 != 0){
					SetDisplayPixel(x, scanline, colourAddress | 0x10);
					if (i == 0 && x < 255 && hasSpriteZero && showBackground && backgroundColor % 4 != 0) ppuStatus |= 0x40;
					break;
				}
			}
		}
	}
}

void PPU::SetDisplayPixel(int x, int y, nes_byte colourAddress) {
	unsigned char colourValue = paletteIndices[colourAddress] % 0x3F;
	if (ppuMask_greyscale) colourValue &= 0x30;
	display->SetPixel(x, y, colourValue);
}
