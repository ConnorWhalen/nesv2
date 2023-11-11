#include "NullPPU.h"

std::vector<OutputData>* NullPPU::Serialize() {
	return new std::vector<OutputData>();
}

NullPPU::NullPPU(Display &display, bool mirroring, bool fourScreenMode) : PPU(display, mirroring, fourScreenMode){
	// pass
}

nes_byte NullPPU::Read(nes_address address) {
	return 0x00;
}

void NullPPU::Write(nes_address address, nes_byte value) {
	// pass
}

void NullPPU::Step() {
	// pass
}

bool NullPPU::DoNMI() {
	return false;
}

void NullPPU::OAMDMA(nes_byte value, Part* cpu) {
	// pass
}