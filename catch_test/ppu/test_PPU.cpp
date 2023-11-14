//
// NES-V2: test_PPU.cpp
//
// Created by Connor Whalen on 2022-07-05.
//
#include "../catch.hpp"

#include <SDL2/SDL.h>

#include "ppu_test_utilities.h"

#include "../../parts/CPU.h"
#include "../../parts/NullDisplay.h"
#include "../../parts/PPU.h"
#include "../../parts/Colour.h"
#include "../../parts/mappers/M0.h"

TEST_CASE("PPU functionality") {
    auto romBytes = new std::vector<nes_byte>(MAPPER0_SIZE);
    auto cartRAM = new unsigned char[CART_RAM_SIZE];
    for (int i = 0; i < CART_RAM_SIZE; i++) cartRAM[i] = 0;
    auto chrBytes = new std::vector<nes_byte>(MAPPER_CHR_REGION_SIZE);


    SECTION("PPU INITIALIZATION") {
        NullDisplay display;
        PPU ppu(display, false, false, true);

        auto output = ppu.Serialize();
        REQUIRE(output->at(0).body == getRegisterString(0, 0, 0, 0, 0, 0));
        delete output;
    }

    SECTION("PPU PATTERN TABLE READ/WRITE") {
        NullDisplay display;
        PPU ppu(display, false, false, true);

        nes_byte pattern_data[PATTERN_TABLE_SIZE*2];
        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            pattern_data[i] = nes_byte (rand());
        }

        write_ppu_addr(ppu, 0, false);

        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            ppu.Write(7, pattern_data[i]);
        }

        write_ppu_addr(ppu, 0, true);

        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            REQUIRE(ppu.Read(7) == pattern_data[i]);
        }
    }

    SECTION("PPU PATTERN TABLE INCREMENT 32") {
        NullDisplay display;
        PPU ppu(display, false, false, true);

        nes_byte pattern_data[PATTERN_TABLE_SIZE*2];
        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            pattern_data[i] = nes_byte (rand());
        }

        write_ppu_addr(ppu, 0, false);

        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            ppu.Write(7, pattern_data[i]);
        }

        ppu.Write(0, 0x04);

        write_ppu_addr(ppu, 0, true);

        for (nes_address i = 0; i < (PATTERN_TABLE_SIZE*2)/32; i++) {
            REQUIRE(ppu.Read(7) == pattern_data[i*32]);
        }
    }

    SECTION("PPU PATTERN TABLE DUMP") {
        NullDisplay display;
        PPU ppu(display, true, false, true);

        nes_byte pattern_data[PATTERN_TABLE_SIZE*2];
        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            pattern_data[i] = nes_byte (rand());
        }

        write_ppu_addr(ppu, 0, false);

        for (nes_address i = 0; i < PATTERN_TABLE_SIZE*2; i++) {
            ppu.Write(7, pattern_data[i]);
        }

        write_ppu_addr(ppu, PALETTE_INDICES_START, false);

        nes_byte palette_data[PALETTE_INDICES_SIZE];
        for (nes_address i = 0; i < PALETTE_INDICES_SIZE; i++) {
            palette_data[i] = nes_byte (rand());
        }

        for (nes_address i = 0; i < PALETTE_INDICES_SIZE; i++) {
            if (i % 4 == 0) {
                ppu.Write(7, palette_data[0]);
            } else {
                ppu.Write(7, palette_data[i]);
            }
        }

        // TODO: run the real thing
        // ppu.Write(1, 0x1E);

        // write_ppu_addr(ppu, 0, false);
        // ppu.Write(5, 0x00);
        // ppu.Write(5, 0x00);

        // ppu.Tick();

        ppu.dumpPatternTable();

        Uint32* pixels = display.GetPixels();
        SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);

        for (int i = 0; i < (NES_WIDTH*NES_HEIGHT); i++) {
            int x = i % NES_WIDTH;
            int y = i / NES_WIDTH;

            nes_byte table_value = 0x00;
            int table_pointer = ((y & 0x07)) + ((x & 0xfff8) << 1) + ((y & 0xfff8) << 6);

            if (table_pointer < 0x2000) {
                nes_byte fine_x = x % 8;
                table_value += (pattern_data[table_pointer] >> (7-fine_x)) & 0x01;
                table_value += fine_x < 7 ? ((pattern_data[table_pointer + 8] >> (6-fine_x)) & 0x02) : ((pattern_data[table_pointer + 8] << 1) & 0x02);
            } else {
                table_value = 0;
            }

            nes_byte palette_value = 0;
            if (table_value % 4 == 0) {
                palette_value = palette_data[0] % 0x3F;
            } else {
                palette_value = palette_data[table_value] % 0x3F;
            }

            unsigned char r = Colour::red(palette_value);
            unsigned char g = Colour::green(palette_value);
            unsigned char b = Colour::blue(palette_value);
            REQUIRE(pixels[i] == SDL_MapRGB(pixelFormat, r, g, b));
        }
    }

    SECTION("PPU NAME TABLE HORIZONTAL MIRRORING") {
        NullDisplay display;
        PPU ppu(display, false, false, true);

        nes_byte name_table_data[NAME_TABLE_SIZE*2];
        for (nes_address i = 0; i < NAME_TABLE_SIZE*2; i++) {
            name_table_data[i] = nes_byte (rand());
        }

        write_ppu_addr(ppu, NAME_TABLES_START, false);

        for (nes_address i = 0; i < NAME_TABLE_SIZE; i++) {
            ppu.Write(7, name_table_data[i]);
        }

        write_ppu_addr(ppu, (NAME_TABLES_START + NAME_TABLE_SIZE*2), false);

        for (nes_address i = 0; i < NAME_TABLE_SIZE; i++) {
            ppu.Write(7, name_table_data[i + NAME_TABLE_SIZE]);
        }

        write_ppu_addr(ppu, NAME_TABLES_START, true);

        for (nes_address i = 0; i < NAME_TABLE_SIZE*4; i++) {
            if (i < NAME_TABLE_SIZE*2) {
                REQUIRE(ppu.Read(7) == name_table_data[i % NAME_TABLE_SIZE]);
            } else {
                REQUIRE(ppu.Read(7) == name_table_data[(i % NAME_TABLE_SIZE) + NAME_TABLE_SIZE]);
            }
        }
    }


    SECTION("PPU NAME TABLE VERTICAL MIRRORING") {
        NullDisplay display;
        PPU ppu(display, true, false, true);

        nes_byte name_table_data[NAME_TABLE_SIZE*2];
        for (nes_address i = 0; i < NAME_TABLE_SIZE*2; i++) {
            name_table_data[i] = nes_byte (rand());
        }

        write_ppu_addr(ppu, NAME_TABLES_START, false);

        for (nes_address i = 0; i < NAME_TABLE_SIZE; i++) {
            ppu.Write(7, name_table_data[i]);
        }

        for (nes_address i = 0; i < NAME_TABLE_SIZE; i++) {
            ppu.Write(7, name_table_data[i + NAME_TABLE_SIZE]);
        }
        
        write_ppu_addr(ppu, NAME_TABLES_START, true);

        for (nes_address i = 0; i < NAME_TABLE_SIZE*4; i++) {
            if (i < NAME_TABLE_SIZE) {
                REQUIRE(ppu.Read(7) == name_table_data[(i % NAME_TABLE_SIZE)]);
            } else if (i < NAME_TABLE_SIZE*2) {
                REQUIRE(ppu.Read(7) == name_table_data[(i % NAME_TABLE_SIZE) + NAME_TABLE_SIZE]);
            } else if (i < NAME_TABLE_SIZE*3) {
                REQUIRE(ppu.Read(7) == name_table_data[(i % NAME_TABLE_SIZE)]);
            } else {
                REQUIRE(ppu.Read(7) == name_table_data[(i % NAME_TABLE_SIZE) + NAME_TABLE_SIZE]);
            }
        }
    }

    SECTION("PPU OAMVALUE + OAMADDR") {
        NullDisplay display;
        PPU ppu(display, false, false, true);

        nes_byte oam_data[OAM_SIZE];
        for (nes_address i = 0; i < OAM_SIZE; i++) {
            oam_data[i] = nes_byte (rand());
        }

        ppu.Write(3, 0);

        for (nes_address i = 0; i < OAM_SIZE; i++) {
            REQUIRE(ppu.Read(3) == i);
            ppu.Write(4, oam_data[i]);
            REQUIRE(ppu.Read(3) == (i + 1) % OAM_SIZE);
        }

        for (nes_address i = 0; i < OAM_SIZE; i++) {
            ppu.Write(3, i);
            REQUIRE(ppu.Read(3) == i);
            REQUIRE(ppu.Read(4) == oam_data[i]);
        }
    }

    SECTION("PPU OAMDMA") {
        NullDisplay display;
        PPU ppu(display, false, false, true);

        M0 mapper(*romBytes, cartRAM, *chrBytes, true);
        CPU cpu(mapper, ppu, true);

        nes_byte start_address = 0x22;

        ppu.Write(3, start_address);

        nes_byte oam_data[OAM_SIZE];
        for (nes_address i = 0; i < OAM_SIZE; i++) {
            oam_data[i] = nes_byte (rand());
            cpu.Write(0x200 + i, oam_data[i]);
        }

        ppu.OAMDMA(0x02, &cpu);

        for (nes_address i = 0; i < OAM_SIZE; i++) {
            ppu.Write(3, (i + start_address) % OAM_SIZE);
            REQUIRE(ppu.Read(4) == oam_data[i]);
        }
    }

    // To test:
    // - Palette indices writing
    // - Registers
    // - Output to display
    // - DoNMI
        // test 32 increment mode
}
