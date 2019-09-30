//
// NES-V2: Controllers.h
//
// Created by Connor Whalen on 2019-04-21.
//

#ifndef NESV2_CONTROLLERS_H
#define NESV2_CONTROLLERS_H

#include "OutputData.h"
#include "Part.h"
#include "../input/Input.h"

class Controllers : public Part {
public:
    Controllers(Input *input);
    std::vector<OutputData>* Serialize() override;
private:
    Input *input;
};

#endif //NESV2_CONTROLLERS_H
