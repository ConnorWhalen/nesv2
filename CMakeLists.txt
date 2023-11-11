cmake_minimum_required(VERSION 3.6)
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

find_library(SDL_IMAGE SDL2_image)
if (NOT SDL_IMAGE)
    message(FATAL_ERROR "SDL_IMAGE not found")
endif()

find_library(YAML_CPP yaml-cpp)
if (NOT YAML_CPP)
    message(FATAL_ERROR "YAML_CPP not found")
endif()

add_executable(nesv2 main.cpp
        RomParser.cpp RomParser.h PartAssembler.h PartAssembler.cpp Config.cpp Config.h
        UI/TextArea.cpp UI/TextArea.h UI/TextWindow.cpp UI/TextWindow.h
        UI/Window.cpp UI/Window.h UI/GameArea.cpp UI/GameArea.h UI/WindowArea.h
        UI/Button.cpp UI/Button.h
        parts/Part.h parts/OutputData.h parts/CPU.h parts/PPU.h parts/APU.h
        parts/Controllers.h parts/Speakers.h parts/Controllers.cpp
        parts/CPU.cpp parts/PPU.cpp parts/Display.h parts/Display.cpp parts/Colour.h
        parts/Colour.cpp parts/NullPPU.h parts/NullPPU.cpp
        parts/mappers/Mapper.h parts/mappers/Mapper.cpp parts/mappers/M0.cpp
        parts/mappers/M0.h parts/mappers/M2.cpp parts/mappers/M2.h parts/mappers/M1.cpp
        parts/mappers/M1.h
        parts/PartUtilities.h parts/PartUtilities.cpp
        input/KeyboardInput.h input/KeyboardInput.cpp input/Input.h input/TouchInput.cpp
        input/TouchInput.h
        )

set(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "-O0")
SET(CMAKE_C_FLAGS "-O0")

message("${SDL2}")
message("${SDL_TTF}")
message("${SDL_IMAGE}")
message("${YAML_CPP}")
target_link_libraries(nesv2 ${SDL2})
target_link_libraries(nesv2 ${SDL_TTF})
target_link_libraries(nesv2 ${SDL_IMAGE})
target_link_libraries(nesv2 ${YAML_CPP})
include_directories(/usr/local/include)

add_subdirectory(catch_test)
