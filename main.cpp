#include <iostream>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "Config.h"
#include "parts/APU.h"
#include "RomParser.h"
#include "UI/GameArea.h"
#include "input/KeyboardInput.h"
#include "UI/Button.h"
#include "UI/TextWindow.h"
#include "input/TouchInput.h"
#include "PartAssembler.h"

constexpr int TEXT_WINDOW_WIDTH = 500;
constexpr int TEXT_WINDOW_HEIGHT = 700;
constexpr int GAME_WINDOW_WIDTH = 800;
constexpr int GAME_WINDOW_HEIGHT = 480;
constexpr int GAME_WIDTH = 512;
constexpr int GAME_HEIGHT = 480;
constexpr int GAME_TOP = 0;
constexpr int GAME_LEFT = 144;
constexpr int FPS_BUFFER = FPS*4;

bool init();

void close();

TextWindow* textWindow;
Window* gameWindow;

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

    auto aButtonRect = WindowArea::ConstructSDLRect(72, 72, 728, 368);
    auto bButtonRect = WindowArea::ConstructSDLRect(72, 72, 656, 368);
    auto startButtonRect = WindowArea::ConstructSDLRect(72, 72, 728, 224);
    auto selectButtonRect = WindowArea::ConstructSDLRect(72, 72, 656, 224);
    auto upButtonRect = WindowArea::ConstructSDLRect(72, 72, 36, 260);
    auto downButtonRect = WindowArea::ConstructSDLRect(72, 72, 36, 404);
    auto leftButtonRect = WindowArea::ConstructSDLRect(72, 72, 0, 332);
    auto rightButtonRect = WindowArea::ConstructSDLRect(72, 72, 72, 332);
    auto touchInput = new TouchInput(
            TouchInput::SDLRectToTouchRect(aButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(bButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(upButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(downButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(leftButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(rightButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(startButtonRect, 800, 480),
            TouchInput::SDLRectToTouchRect(selectButtonRect, 800, 480)
    );

    Input *inputs[] {keyboard, touchInput};

    PartAssembler partAssembler;
    Parts *parts = partAssembler.Assemble(
            config.Get("cpu_type"),
            config.Get("ppu_type"),
            config.Get("apu_type"),
            config.Get("controllers_type"),
            config.Get("speakers_type"),
            touchInput,
            romData
    );

    SDL_Color Red = {255, 80, 80};
    SDL_Color White = {255, 255, 255};
    if(!init()) {
        printf("Failed to initialize SDL. Exiting!\n");
        return -1;
    }

    textWindow = new TextWindow(TEXT_WINDOW_WIDTH, TEXT_WINDOW_HEIGHT);
    gameWindow = new Window(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
    Window *windows[] {textWindow, gameWindow};

    auto aButton = new Button(aButtonRect, gameWindow->GetRenderer(),
                              "sprites/onButton.png", "sprites/offButton.png",
                              touchInput, A_BUTTON);
    auto bButton = new Button(bButtonRect, gameWindow->GetRenderer(),
                              "sprites/onButton.png", "sprites/offButton.png",
                              touchInput, B_BUTTON);
    auto startButton = new Button(startButtonRect, gameWindow->GetRenderer(),
                                  "sprites/onButton.png", "sprites/offButton.png",
                                  touchInput, START_BUTTON);
    auto selectButton = new Button(selectButtonRect, gameWindow->GetRenderer(),
                                   "sprites/onButton.png", "sprites/offButton.png",
                                   touchInput, SELECT_BUTTON);
    auto upButton = new Button(upButtonRect, gameWindow->GetRenderer(),
                               "sprites/onButton.png", "sprites/offButton.png",
                               touchInput, UP_BUTTON);
    auto downButton = new Button(downButtonRect, gameWindow->GetRenderer(),
                                 "sprites/onButton.png", "sprites/offButton.png",
                                 touchInput, DOWN_BUTTON);
    auto leftButton = new Button(leftButtonRect, gameWindow->GetRenderer(),
                                 "sprites/onButton.png", "sprites/offButton.png",
                                 touchInput, LEFT_BUTTON);
    auto rightButton = new Button(rightButtonRect, gameWindow->GetRenderer(),
                                  "sprites/onButton.png", "sprites/offButton.png",
                                  touchInput, RIGHT_BUTTON);

    auto gameRect = new SDL_Rect;
    gameRect->w = GAME_WIDTH;
    gameRect->h = GAME_HEIGHT;
    gameRect->x = GAME_LEFT;
    gameRect->y = GAME_TOP;
    auto gameArea = new GameArea(gameRect, gameWindow->GetRenderer());
    gameWindow->AddWindowArea(gameArea);
    gameWindow->AddWindowArea(aButton);
    gameWindow->AddWindowArea(bButton);
    gameWindow->AddWindowArea(startButton);
    gameWindow->AddWindowArea(selectButton);
    gameWindow->AddWindowArea(upButton);
    gameWindow->AddWindowArea(downButton);
    gameWindow->AddWindowArea(leftButton);
    gameWindow->AddWindowArea(rightButton);

    // Sample pixels
    Uint32 pixels[NES_WIDTH*NES_HEIGHT];
    for (int i = 0; i < NES_WIDTH*NES_HEIGHT; i++) pixels[i] = 0;
    Uint32 pixelFormatCode;
    SDL_QueryTexture(gameArea->GetTexture(), &pixelFormatCode, nullptr, nullptr, nullptr);
    SDL_PixelFormat *pixelFormat = SDL_AllocFormat(pixelFormatCode);

    bool quit = false;
    SDL_Event inputEvent;
    int now = SDL_GetTicks();
    int next_frame = now;
    int second_start = now;
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

            for (Part *part : *(parts->asVector)) {
                part->Tick();
            }

            gameArea->Flip(pixels, NES_WIDTH, NES_HEIGHT);

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
                    if (next_frame - now < -100) {
                        second_start = now;
                    } else {
                        second_start += 1000;
                    }
                }
                next_frame = second_start + (1000 * (fps_index % FPS)/FPS);
            }

            textWindow->AddText("Frame Rate (Past 4s)", Red);
            textWindow->AddText(std::to_string(fps), White);
            for (Window *window : windows) {
                window->Render();
                window->Render();
            }
        }
    }

    close();

    return 0;
}
