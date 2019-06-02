//
// NES-V2: Controllers.cpp
//
// Created by Connor Whalen on 2019-05-18.
//

#include "Controllers.h"

#include <iomanip>
#include <sstream>

Controllers::Controllers(Input *input) {
    this->input = input;
}

std::vector<OutputData>* Controllers::Serialize() {
    std::stringstream inputStream;
    inputStream << std::setfill('0') << std::setw(2) << std::hex << int(this->input->Read());
    return new std::vector<OutputData> {
            {"Controllers Value", inputStream.str()}
    };
}
