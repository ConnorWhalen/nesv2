//
// NES-V2: PPU.cpp
//
// Created by Connor Whalen on 2019-12-19.
//

#include <iomanip>
#include <sstream>
#include <string>
#include "PPU.h"
#include "PartUtilities.h"

std::vector<OutputData>* PPU::Serialize() {
    std::stringstream registerStream;
    registerStream << std::setfill('0') << std::setw(2) << std::hex << int(ppuCtrl) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuMask) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuStatus) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(oamAddr) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuScroll) << " "
                   << std::setfill('0') << std::setw(2) << std::hex << int(ppuData);

    std::stringstream internalsStream;
    internalsStream << std::setfill('0') << std::setw(2) << std::hex << int(fineXScroll) << " "
                   << std::setfill('0') << std::setw(4) << std::hex << int(vramAddr) << " "
                   << std::setfill('0') << std::setw(4) << std::hex << int(tempVramAddr) << " "
                   << std::setfill('0') << std::setw(2) << addressLatch;

    std::stringstream paletteStream;
    PartUtilities::serializeBytes(paletteStream, paletteIndices, PALETTE_INDICES_SIZE);

    std::stringstream oamStream;
    PartUtilities::serializeBytes(oamStream, oam, OAM_SIZE);

    std::stringstream nameTableStream;
    PartUtilities::serializeBytes(nameTableStream, nameTable, NAME_TABLE_SIZE);

    std::stringstream patternTableStream;
    PartUtilities::serializeBytes(patternTableStream, patternTable, PATTERN_TABLE_SIZE);

    return new std::vector<OutputData> {
        {"PPU Registers", registerStream.str()},
        {"PPU Internal Registers", internalsStream.str()},
        {"Palette Indices", paletteStream.str()},
        {"OAM Dump", oamStream.str()},
        {"Name Table Dump", nameTableStream.str()},
        {"Pattern Table Dump", patternTableStream.str()},
    };
}

PPU::PPU(Display &display, bool mirroring, bool fourScreenMode)
	: display(display)
{
	// http://wiki.nesdev.com/w/index.php/Cycle_reference_chart#Clock_rates
    this->CYCLES_PER_STEP = 4;
    this->STEPS_PER_TICK = (CLOCK_FREQUENCY/CYCLES_PER_STEP)/FPS;
	ppuCtrl = 0;

    ppuStatus = 0;
    ppuCtrl = 0;
    ppuMask = 0;
    oamAddr = 0;
    ppuScroll = 0;
    ppuAddr = 0;
    ppuData = 0;

	ppuCtrl_nmiOnVblankStart = false;
	ppuCtrl_sixteenMode = false;
	ppuCtrl_backgroundPatternTableOffset = false;
	ppuCtrl_spritePatternTableOffset = false;
	ppuCtrl_vramIncrement32 = false;
    ppuMask_showSprites = false;
    ppuMask_showBackground = false;
    ppuMask_showSpritesLeft8 = false;
    ppuMask_showBackgroundLeft8 = false;
    ppuMask_greyscale = false;

	oddFrame = false;
	scanline = -1;
	clockTick = 0;
	fineXScroll = 0;
	spriteCount = 0;
	hasSpriteZero = false;
	nmiFlag = false;
	addressLatch = false;
	vramAddr = 0;
	tempVramAddr = 0;

	if (fourScreenMode){
		singleSceenMirroring = true;
		verticalMirroring = false;
	} else{
		singleSceenMirroring = false;
		if (mirroring){
			verticalMirroring = true;
		} else{
			verticalMirroring = false;
		}
	}
}

nes_byte PPU::Read(nes_address address) {
	nes_byte ret = 0;
    switch (address % 8) {
        case 0:
			ret = ppuCtrl;
			break;
		case 1:
			ret = ppuMask;
			break;
		case 2:
			ret = ppuStatus;
			ppuStatus &= 0x7F;
			addressLatch = false;
			break;
		case 3:
			ret = oamAddr;
			break;
		case 4:
			ret = oam[oamAddr];
			break;
		case 5:
			ret = ppuScroll;
			break;
		case 6:
			ret = ppuAddr;
			break;
		case 7:
			if ((vramAddr % 0x4000) > 0x3eff) {
				ret = ppuValueAt(vramAddr);
				ppuData = ret;
				// TODO: set PPUDATA to value "under" palette
			} else {
				ret = ppuData;
				ppuData = ppuValueAt(vramAddr);
			}
			if (ppuCtrl_vramIncrement32) vramAddr += 32; else vramAddr += 1;
			break;
	}
	return ret;
}

void PPU::Write(nes_address address, nes_byte value) {
    switch (address % 8) {
        case 0:
			ppuCtrl = value;
			ppuCtrl_nmiOnVblankStart = (value & 0x80);
			// slaveSelect = (value & 0x40);
			ppuCtrl_sixteenMode = (value & 0x20);
			ppuCtrl_backgroundPatternTableOffset = (value & 0x10);
			ppuCtrl_spritePatternTableOffset = (value & 0x08);
			ppuCtrl_vramIncrement32 = (value & 0x04);
			tempVramAddr = (tempVramAddr & 0xf3ff) + ((value & 0x03) << 10);
			break;
		case 1:
			ppuMask = value;
			display.SetColourEmphasis((value & 0x20), (value & 0x40), (value & 0x80));
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
			if (addressLatch) {
				tempVramAddr = (tempVramAddr & 0x0c1f) + ((value & 0xf8) << 2) + ((value & 0x07) << 12);
			} else{
				tempVramAddr = (tempVramAddr & 0xffe0) + ((value & 0xf8) >> 3);
				fineXScroll = value & 0x07;
			}
			addressLatch = !addressLatch;
			break;
		case 6:
			ppuAddr = value;
			if (addressLatch) {
				tempVramAddr = (tempVramAddr & 0xff00) + value;
				vramAddr = tempVramAddr;
			} else{
				tempVramAddr = (tempVramAddr & 0x00ff) + ((value & 0x3f) << 8);
			}
			addressLatch = !addressLatch;
			break;
		case 7:
			setPPUValueAt(vramAddr, value);
			if ((scanline > -2 && scanline < 240) && (ppuMask_showBackground || ppuMask_showSprites)) {
				incXScroll();
				incYScroll();
			}
			if (ppuCtrl_vramIncrement32) vramAddr += 32; else vramAddr += 1;
			break;
	}
}

void PPU::OAMDMA(nes_byte value, Part* cpu) {
	for (int i = 0; i < 256; i++) {
		oam[(i+oamAddr) % 0x100] = cpu->Read(value*0x100 + i);
	}
}

bool PPU::DoNMI() {
	if ((ppuStatus & 0x80) == 0x80 && ppuCtrl_nmiOnVblankStart && !nmiFlag){
		nmiFlag = true;
		return true;
	} else if((ppuStatus & 0x80) == 0x80 && ppuCtrl_nmiOnVblankStart){ // nmiFlag still true
		return false;
	} else{
		nmiFlag = false;
		return false;
	}
}

void PPU::dumpPatternTable() {
	int x = 0;
	int y = 0;
	int tablePointer = 0;
	for (int i = 0; i < PATTERN_TABLE_SIZE*2 && y < 240; i++){
		for (int j = 0; j < 8; j++){
			nes_byte lo_fetch = (ppuValueAt(tablePointer) >> (7-j)) & 0x01;
			nes_byte hi_fetch = j < 7 ? ((ppuValueAt(tablePointer + 8) >> (6-j)) & 0x02) : ((ppuValueAt(tablePointer + 8) << 1) & 0x02);
			nes_byte colour = lo_fetch + hi_fetch;
			SetDisplayPixel(x, y, colour);
			x++;
		}
		x -= 8;
		y++;
		if (++tablePointer % 8 == 0){
			tablePointer += 8;
			x += 8;
			y -= 8;
		}
		if (x == 256){
			x = 0;
			y += 8;
		}
	}
}

void PPU::Step() {
	if (scanline < 240) {
		if ((ppuMask_showBackground || ppuMask_showSprites) && (scanline >= 0 || clockTick >= 256)) {
			if (clockTick == 0) clockTick = 0;// pass
			else if (clockTick % 8 == 1 && (clockTick < 256 || clockTick == 321 || clockTick == 329)) fetchNextTile();
			else if (clockTick % 8 == 0 && (clockTick < 256 || clockTick > 320)) incXScroll();
			else if (clockTick == 256) incYScroll();
			else if (clockTick == 257) vramAddr = (vramAddr & 0x7be0) + (tempVramAddr & 0x041f); // reset x scroll

			if (clockTick == 64) for (int i = 0; i < 0x20; i++) secondaryOam[i] = 0xff;
			else if (clockTick == 65) evaluateSprites();
			else if (clockTick == 320) fetchNextSprites();
			else if (clockTick >= 257 && clockTick < 321) oamAddr = 0;
		}
		if (scanline < 0) { // prerender
			if (clockTick == 1) ppuStatus &= 0x1f; // clear flags
			else if ((ppuMask_showBackground || ppuMask_showSprites) && clockTick >= 280 && clockTick < 305) vramAddr = (vramAddr & 0x041f) + (tempVramAddr & 0x7be0); // reset y scroll
			else if (clockTick == 339 && oddFrame) clockTick++; // Skip last tick
		} else if (scanline < 240) { // visible lines
			DrawPixel(clockTick-1);
		}
	} else if (scanline < 241) { // postrender
		// pass
	} else if (scanline < 261) { // vblank
		if (scanline == 241 && clockTick == 1) {
			ppuStatus |= 0x80; // set vblank flag
		}
	}

	if (++clockTick == 341) {
		clockTick = 0;
		if (++scanline == 261) {
			scanline = -1;
			oddFrame = !oddFrame;
		}
	}
}

void PPU::fetchNextTile() {
	for (int i = 0; i < 16; i++) currentTiles[i] = currentTiles[i + 8];
	nes_byte nameTableFetch = ppuValueAt((vramAddr & 0x0fff) + 0x2000);
	nes_byte attributeTableFetch = ppuValueAt((vramAddr & 0x0c00) + ((vramAddr & 0x0380) >> 4) + ((vramAddr & 0x001c) >> 2) + 0x23c0);
	nes_address patternTableAddress = ((vramAddr & 0x7000) >> 12) + (nameTableFetch << 4);
	if (ppuCtrl_backgroundPatternTableOffset) patternTableAddress += 0x1000;
	nes_byte patternTableLoFetch = ppuValueAt(patternTableAddress);
	nes_byte patternTableHiFetch = ppuValueAt(patternTableAddress + 0x0008);
	for (int i = 0; i < 7; i++) currentTiles[i + 16] = ((patternTableLoFetch >> (7-i)) & 0x1) + ((patternTableHiFetch >> (6-i)) & 0x2);
	// shifting by a negative number results in undefined behaviour
	currentTiles[23] = (patternTableLoFetch & 0x1) + ((patternTableHiFetch << 1) & 0x2);
	nes_byte attributeBitShift = 0;
	if ((vramAddr & 0x0002) == 0x0002) attributeBitShift += 2;
	if ((vramAddr & 0x0040) == 0x0040) attributeBitShift += 4;
	for (int i = 0; i < 8; i++) currentTiles[i + 16] += ((attributeTableFetch >> attributeBitShift) & 0x03) << 2;
	// std::cout<< std::hex << patternTableAddress << " " << int(vramAddr) << " " << int(nameTableFetch) << "\n";
}

void PPU::evaluateSprites() {
	spriteCount = 0;
	hasSpriteZero = false;
	unsigned short primaryIndex = oamAddr; // 0-0xff array index since it can be misaligned
	nes_byte secondaryIndex = 0; // 0-7 sprite number
	nes_byte subIndex = 0;
	nes_byte height = ppuCtrl_sixteenMode ? 16 : 8;
	while (primaryIndex <= 0xfc && secondaryIndex <= 8) {
		secondaryOam[secondaryIndex*4] = oam[primaryIndex];
		if (secondaryOam[secondaryIndex*4] <= scanline && secondaryOam[secondaryIndex*4] > scanline - height) {
			if (primaryIndex == 0) hasSpriteZero = true;
			secondaryOam[secondaryIndex*4 + 1] = oam[primaryIndex + 1];
			secondaryOam[secondaryIndex*4 + 2] = oam[primaryIndex + 2];
			secondaryOam[secondaryIndex*4 + 3] = oam[primaryIndex + 3];
			secondaryIndex++;
			spriteCount++;
		}
		primaryIndex += 4;
	}
	if (secondaryIndex == 8) {
		while (primaryIndex <= 0xfc) {
			if (oam[primaryIndex + subIndex] <= scanline && oam[primaryIndex + subIndex] > scanline - height) {
				ppuStatus |= 0x20; // sprite overflow
				break;
			}
			primaryIndex += 4;
			subIndex = (subIndex + 1) % 4;
		}
	}
}

void PPU::fetchNextSprites() {
	for (int i = 0; i < 0x20; i++) currentSprites[i] = secondaryOam[i];
	for (int i = 0; i < spriteCount; i++) {
		nes_byte yOffset = scanline - currentSprites[i*4];
		nes_address patternTableAddress;
		if (!ppuCtrl_sixteenMode) {
			if ((currentSprites[i*4 + 2] & 0x80) == 0x80) yOffset = 0x07 - yOffset; // V flip
			patternTableAddress = currentSprites[i*4 + 1] << 4;
			if (ppuCtrl_spritePatternTableOffset) patternTableAddress += 0x1000;
		} else {
			if ((currentSprites[i*4 + 2] & 0x80) == 0x80) yOffset = 0x0f - yOffset; // V flip
			if (yOffset > 7) yOffset += 8;
			patternTableAddress = (currentSprites[i*4 + 1] & 0xfe) << 4;
			if ((currentSprites[i*4 + 1] & 0x01) == 0x01) patternTableAddress += 0x1000;
		}
		spriteTiles[i*2] = ppuValueAt(patternTableAddress + yOffset);
		spriteTiles[i*2 + 1] = ppuValueAt(patternTableAddress + yOffset + 8);
		if ((currentSprites[i*4 + 2] & 0x40) == 0x40) { // H flip
			spriteTiles[i*2] = (spriteTiles[i*2] & 0xF0) >> 4 | (spriteTiles[i*2] & 0x0F) << 4;
			spriteTiles[i*2] = (spriteTiles[i*2] & 0xCC) >> 2 | (spriteTiles[i*2] & 0x33) << 2;
			spriteTiles[i*2] = (spriteTiles[i*2] & 0xAA) >> 1 | (spriteTiles[i*2] & 0x55) << 1;
			spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xF0) >> 4 | (spriteTiles[i*2 + 1] & 0x0F) << 4;
			spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xCC) >> 2 | (spriteTiles[i*2 + 1] & 0x33) << 2;
			spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xAA) >> 1 | (spriteTiles[i*2 + 1] & 0x55) << 1;
		}
	}
}

// covers draw routine from execute, spriteZero, drawSprite, backgroundTransparent, spritePriority, spriteHere 
void PPU::DrawPixel(nes_byte x) {
	bool showBackground = ppuMask_showBackground && (x > 7 || ppuMask_showBackgroundLeft8);
	bool showSprites = ppuMask_showSprites && (x > 7 || ppuMask_showSpritesLeft8);
	nes_byte backgroundColor = currentTiles[(x % 8) + fineXScroll];
	// transparent
	// SetDisplayPixel/(x, scanline);
	if (showBackground) {
		// SetDisplayPixel(x, scanline, backgroundColor);
	}
	if (showSprites) {
		nes_byte tile0 = 0;
		nes_byte tile1 = 0;
		nes_byte attribute = 0;
		nes_byte xOffset = 0;
		for (int i = 0; i < spriteCount; i++) {
			if (currentSprites[i*4 + 3] <= x && currentSprites[i*4 + 3] > x - 8 &&
				// sprite priority or background transparent
				((currentSprites[i*4 + 2] & 0x20) == 0x00 || !showBackground || backgroundColor % 4 == 0)) {
				tile0 = spriteTiles[i*2];
				tile1 = spriteTiles[i*2 + 1];
				attribute = currentSprites[i*4 + 2] & 0x03;
				xOffset = x - currentSprites[i*4 + 3];

				nes_byte colourAddress = (tile0 >> (7 - xOffset)) & 0x01;
				if (xOffset < 7) colourAddress += (tile1 >> (6 - xOffset)) & 0x02;
				else colourAddress += (tile1 << 1) & 0x02; // shifting by a negative number results in undefined behaviour
				// colourAddress += (tile1 >> (7 - xOffset)) & 0x02; // I don't think this is supposed to be here
				colourAddress += attribute << 2;
				if (colourAddress % 4 != 0) {
					// SetDisplayPixel(x, scanline, colourAddress | 0x10);
					if (i == 0 && x < 255 && hasSpriteZero && showBackground && backgroundColor % 4 != 0) ppuStatus |= 0x40;
					break;
				}
			}
		}
	}
}

void PPU::SetDisplayPixel(int x, int y, nes_byte colourAddress) {
	nes_byte colourValue = paletteIndices[colourAddress] % 0x3F;
	if (ppuMask_greyscale) colourValue &= 0x30;
	display.SetPixel(x, y, colourValue);
}

void PPU::incXScroll() {
	if ((vramAddr & 0x001f) == 31) {
		vramAddr -= 31;
		vramAddr ^= 0x0400;
	}
	else vramAddr += 1;
}

void PPU::incYScroll() {
	if ((vramAddr & 0x7000) != 0x7000) vramAddr += 0x1000;
	else {
		vramAddr -= 0x7000;
		unsigned short y = (vramAddr & 0x03e0) >> 5;
		if (y == 29) {
			y = 0;
			vramAddr ^= 0x0800;
		} else if (y == 31) {
			y = 0;
		} else {
			y += 1;
		}
		vramAddr = (vramAddr & 0xfc1f) + (y << 5);
	}
}

nes_byte PPU::nameTableValueAt(nes_address address) {
	nes_byte tableNumber = 0;
	if (address > 0x2fff) {
		printf("Invalid Nametable address %x\n", address);
	} else if (singleSceenMirroring) {
		tableNumber = 0;
	} else if (address > 0x2bff) { // logical NT3
		tableNumber = 1;
	} else if (address > 0x27ff) { // logical NT2
		if (verticalMirroring) tableNumber = 0;
		else tableNumber = 1;
	} else if (address > 0x23ff) { // logical NT1
		if (verticalMirroring) tableNumber = 1;
		else tableNumber = 0;
	} else if (address > 0x1fff) { // logical NT0
		tableNumber = 0;
	} else{
		printf("Invalid Nametable address %x\n", address);
	}

	if (tableNumber == 0) {
		return nameTable[address % NAME_TABLE_SIZE];
	} else if (tableNumber == 1) {
		return nameTable[(address % NAME_TABLE_SIZE) + NAME_TABLE_SIZE];
	}
	return 0;
}

void PPU::setNameTableValueAt(nes_address address, nes_byte value) {
	// std::cout << std::hex << "Nametable Write: " << int(value) << " " << int(address) << "\n";
	unsigned char tableNumber = 0;
	if (address > 0x2fff) {
		printf("Invalid Nametable address %x\n", address);
	} else if (singleSceenMirroring) {
		tableNumber = 0;
	} else if (address > 0x2bff) { // logical NT3
		tableNumber = 1;
	} else if (address > 0x27ff) { // logical NT2
		if (verticalMirroring) tableNumber = 0;
		else tableNumber = 1;
	} else if (address > 0x23ff) { // logical NT1
		if (verticalMirroring) tableNumber = 1;
		else tableNumber = 0;
	} else if (address > 0x1fff) { // logical NT0
		tableNumber = 0;
	} else{
		printf("Invalid Nametable address %x\n", address);
	}

	if (tableNumber == 0) {
		nameTable[address % NAME_TABLE_SIZE] = value;
	} else if (tableNumber == 1) {
		nameTable[(address % NAME_TABLE_SIZE) + NAME_TABLE_SIZE] = value;
	}
}

nes_byte PPU::ppuValueAt(nes_address address) {
	address = address % 0x4000;
	nes_byte ret;
	if (address > 0x3eff) {
		if (address % 4 == 0) {
			ret = paletteIndices[0];
		} else{
			ret = paletteIndices[(address-0x3f00) % PALETTE_INDICES_SIZE];
		}
	} else if (address > 0x1fff) {
		ret = nameTableValueAt(0x2000 + (address % 0x1000));
	} else{
		ret = patternTable[address];
	}
	return ret;
}

void PPU::setPPUValueAt(nes_address address, nes_byte value) {
	address = address % 0x4000;
	if (address > 0x3eff) {
		if (address % 4 == 0) {
			paletteIndices[0] = value;
		} else{
			paletteIndices[(address-0x3f00) % PALETTE_INDICES_SIZE] = value;
		}
	} else if (address > 0x1fff) {
		setNameTableValueAt(0x2000 + (address % 0x1000), value);
	} else{
		patternTable[address] = value;
	}
}
