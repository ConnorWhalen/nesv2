cmake_minimum_required(VERSION 3.9)
project(nesv2)

set(CMAKE_CXX_STANDARD 17)

find_library(SDL2 SDL2)
if (NOT SDL2)
    message(FATAL_ERROR "SDL2 not found")
endif()

find_library(SDL_TTF SDL2_ttf)
if (NOT SDL_TTF)
    message(FATAL_ERROR "SDL_TTF not found")
endif()

find_library(YAML_CPP yaml-cpp)
if (NOT YAML_CPP)
    message(FATAL_ERROR "YAML_CPP not found")
endif()

add_executable(nesv2 main.cpp RomParser.cpp RomParser.h UI/TextArea.cpp UI/TextArea.h UI/TextWindow.cpp UI/TextWindow.h
        parts/Part.h parts/OutputData.h parts/CPU.h parts/PPU.h parts/APU.h parts/Controllers.h parts/Mapper.h
        parts/Speakers.h PartAssembler.h PartAssembler.cpp UI/KeyboardInput.h UI/KeyboardInput.cpp UI/Input.h
        parts/Controllers.cpp Config.cpp Config.h UI/Window.cpp UI/Window.h UI/GameWindow.cpp UI/GameWindow.h
        UI/TouchInput.cpp UI/TouchInput.h)

message("${SDL2}")
message("${SDL_TTF}")
message("${YAML_CPP}")
target_link_libraries(nesv2 ${SDL2})
target_link_libraries(nesv2 ${SDL_TTF})
target_link_libraries(nesv2 ${YAML_CPP})
include_directories(/usr/local/include)
