//
// NES-V2: Colour.h
//
// Created by Connor Whalen on 2021-05-29.
//

#ifndef COLOUR_H
#define COLOUR_H

#include <iostream>

namespace Colour{
	unsigned char red(unsigned char colourValue);
	unsigned char green(unsigned char colourValue);
	unsigned char blue(unsigned char colourValue);
	void tint(unsigned char& r, unsigned char& g, unsigned char& b, bool red, bool green, bool blue);
}

#endif