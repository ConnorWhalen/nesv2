cmake_minimum_required(VERSION 3.6)
project(nesv2)

set(CMAKE_CXX_STANDARD 17)

find_library(SDL2 SDL2)
if (NOT SDL2)
    message(FATAL_ERROR "SDL2 not found")
endif()

add_executable(catch_test catch.hpp test_main.cpp
        ../parts/CPU.cpp ../parts/CPU.h ../parts/mappers/M0.cpp ../parts/PPU.cpp
        ../parts/PPU.h ../parts/Display.cpp ../parts/Display.h ../parts/Colour.cpp
        ../parts/Colour.h ../parts/NullDisplay.h ../parts/NullDisplay.cpp
        ../parts/mappers/M0.h ../parts/mappers/Mapper.h ../parts/mappers/Mapper.cpp
        ../parts/mappers/M2.cpp ../parts/mappers/M2.cpp 
        ../parts/PartUtilities.h ../parts/PartUtilities.cpp
        cpu/test_CPU.cpp cpu/test_CPU_Loading.cpp cpu/test_CPU_Branching.cpp
        cpu/test_CPU_Transfers.cpp cpu/cpu_test_utilities.h cpu/cpu_test_utilities.cpp
        ppu/test_PPU.cpp ppu/ppu_test_utilities.h ppu/ppu_test_utilities.cpp
        test_Mapper.cpp mapper_test_utilities.cpp
        mapper_test_utilities.h)


message("${SDL2}")
target_link_libraries(catch_test ${SDL2})
