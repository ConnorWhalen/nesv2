cmake_minimum_required(VERSION 3.6)
project(nesv2)

set(CMAKE_CXX_STANDARD 17)

add_executable(catch_test catch.hpp test_CPU.cpp ../parts/CPU.cpp ../parts/CPU.h ../parts/mappers/M0.cpp
        ../parts/mappers/M0.h ../parts/mappers/Mapper.h ../parts/mappers/Mapper.cpp ../parts/mappers/M2.cpp
        ../parts/mappers/M2.cpp cpu_test_utilities.h cpu_test_utilities.cpp test_Mapper.cpp)