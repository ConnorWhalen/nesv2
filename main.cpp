#include <iostream>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "Config.h"
#include "parts/APU.h"
#include "RomParser.h"
#include "UI/GameWindow.h"
#include "UI/KeyboardInput.h"
#include "UI/TextWindow.h"
#include "UI/TouchInput.h"
#include "PartAssembler.h"

constexpr int TEXT_WINDOW_WIDTH = 500;
constexpr int TEXT_WINDOW_HEIGHT = 700;
constexpr int GAME_WINDOW_WIDTH = 512;
constexpr int GAME_WINDOW_HEIGHT = 480;
constexpr int NES_WIDTH = 256;
constexpr int NES_HEIGHT = 240;
constexpr int FPS = 60;
constexpr int FPS_BUFFER = FPS*4;

bool init();

void close();

TextWindow* textWindow;
GameWindow* gameWindow;

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }

    return success;
}

void close(){
    textWindow->Destroy();
    gameWindow->Destroy();

    SDL_Quit();
}

int main(int argc, char *argv[]) {
    std::string rom_file;
    std::string config_file;
    for (int i = 1; i < argc; i++ ){
        if (!strcmp(argv[i], "--config")) {
            config_file = argv[++i];
        }
        if (!strcmp(argv[i], "--rom")) {
            rom_file = argv[++i];
        }
    }

    Config config(config_file);

    if (rom_file.empty()) {
        rom_file = config.Get("rom_file");
    }

    printf("Loading ROM file %s\n", rom_file.c_str());
    RomParser::RomData *romData = RomParser::parse(rom_file);

    auto keyboard = new KeyboardInput(
            SDL_GetKeyFromName(config.Get("a_button").c_str()),
            SDL_GetKeyFromName(config.Get("b_button").c_str()),
            SDL_GetKeyFromName(config.Get("up_button").c_str()),
            SDL_GetKeyFromName(config.Get("down_button").c_str()),
            SDL_GetKeyFromName(config.Get("left_button").c_str()),
            SDL_GetKeyFromName(config.Get("right_button").c_str()),
            SDL_GetKeyFromName(config.Get("start_button").c_str()),
            SDL_GetKeyFromName(config.Get("select_button").c_str())
    );

    auto touchInput = new TouchInput(
            {0,             0,             100.0f/480.0f, 100.0f/800.0f},
            {0,             100.0f/800.0f, 100.0f/480.0f, 100.0f/800.0f},
            {100.0f/480.0f, 0,             100.0f/480.0f, 100.0f/800.0f},
            {100.0f/480.0f, 100.0f/800.0f, 100.0f/480.0f, 100.0f/800.0f},
            {200.0f/480.0f, 0,             100.0f/480.0f, 100.0f/800.0f},
            {200.0f/480.0f, 100.0f/800.0f, 100.0f/480.0f, 100.0f/800.0f},
            {300.0f/480.0f, 0,             100.0f/480.0f, 100.0f/800.0f},
            {300.0f/480.0f, 100.0f/800.0f, 100.0f/480.0f, 100.0f/800.0f}
    );

    Input *inputs[] {keyboard, touchInput};

    PartAssembler partAssembler;
    Parts *parts = partAssembler.Assemble(
            config.Get("cpu_type"),
            config.Get("ppu_type"),
            config.Get("apu_type"),
            config.Get("controllers_type"),
            config.Get("mapper_type"),
            config.Get("speakers_type"),
            touchInput
    );

    SDL_Color Red = {255, 80, 80};
    SDL_Color White = {255, 255, 255};
    if(!init()) {
        printf("Failed to initialize SDL. Exiting!\n");
        return -1;
    }
    textWindow = new TextWindow(TEXT_WINDOW_WIDTH, TEXT_WINDOW_HEIGHT);
    gameWindow = new GameWindow(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
    Window *windows[] {textWindow, gameWindow};

    // Sample pixels
    Uint32 pixels[NES_WIDTH*NES_HEIGHT];
    for (int i = 0; i < NES_WIDTH*NES_HEIGHT; i++) pixels[i] = 0;
    Uint32 pixelFormatCode;
    SDL_QueryTexture(gameWindow->GetTexture(), &pixelFormatCode, NULL, NULL, NULL);
    SDL_PixelFormat *pixelFormat = SDL_AllocFormat(pixelFormatCode);

    bool quit = false;
    SDL_Event inputEvent;
    int now = SDL_GetTicks();
    int next_frame = now;
    int frame_start = now;
    double fps;
    int fps_index = 0;
    double fps_buffer[FPS_BUFFER];
    for (int i = 0; i < FPS_BUFFER; i++) fps_buffer[i] = 0;
    while (!quit) {
        if (SDL_PollEvent(&inputEvent)) {
            if (inputEvent.type == SDL_QUIT ||
               (inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            } else if (inputEvent.type == SDL_MOUSEWHEEL) {
                textWindow->Offset(inputEvent.wheel.x, inputEvent.wheel.y);
            }
            for (Input *input : inputs) {
                input->Apply(inputEvent);
            }
        } else {
            // Update sample pixels
            double offset = 0;
            Uint8 val = 0;
            for (int i = 0; i < NES_HEIGHT; i++) {
                for (int j = 0; j < NES_WIDTH; j++) {
                    offset = (double)(i+j)/(double)(NES_WIDTH+NES_HEIGHT);
                    val = (Uint8)((double)fps_index*offset);
                    pixels[i*NES_WIDTH+j] = SDL_MapRGB(
                            pixelFormat,
                            (Uint8)((double)fps_index*offset),
                            (Uint8)((double)fps_index*offset),
                            (Uint8)((double)fps_index*offset));
                }
            }

            gameWindow->Flip(pixels, NES_WIDTH, NES_HEIGHT);

            textWindow->Clear();
            for (Part *part : *(parts->asVector)) {
                for (const OutputData &output : *(part->Serialize())) {
                    textWindow->AddText(output.title, Red);
                    textWindow->AddText(output.body, White);
                }
            }
            textWindow->AddText("ROM Data", Red);
            textWindow->AddText(RomParser::serialize(romData), White);

            textWindow->AddText("touch input (each finger's x/y)", Red);
            std::string touchFingers;
            for (int i = 0; i < FINGER_LIMIT*2; i++) {
                if (i%4 == 0 && i > 0) {
                    touchFingers += "\n";
                }
                touchFingers += std::to_string(touchInput->fingers[i]);
                touchFingers += " ";
            }
            textWindow->AddText(touchFingers, White);

            {
                now = SDL_GetTicks();
                if (next_frame > now) {
                    SDL_Delay(next_frame - now);
                }

                textWindow->AddText("Free Ticks", Red);
                textWindow->AddText(std::to_string(next_frame - now), White);
            }

            {
                now = SDL_GetTicks();
                fps = (1000.0 / (now - fps_buffer[fps_index]) * FPS_BUFFER);
                fps_buffer[fps_index++] = now;
                if (fps_index == FPS_BUFFER) fps_index = 0;
                if (fps_index % FPS == 0) {
                    frame_start += 1000;
                }
                next_frame = frame_start + (1000 * (fps_index % FPS)/FPS);
            }

            textWindow->AddText("Frame Rate", Red);
            textWindow->AddText(std::to_string(fps), White);
            for (Window *window : windows) {
                window->Render();
            }
        }
    }

    close();

    return 0;
}
