/*
 * buffer.h
 */

#pragma once

#include <vector>

#include "pixel.h"
#include "projection.h"

#include "utils/mathematics.h"

// screen buffer
class Buffer {
public:
    unsigned int x, y;          // character buffer size
    float logical_x, logical_y; // logical buffer size
    float dx, dy;               // logical character size
    std::vector<Pixel> pixels;  // pixel Buffer

    Buffer(unsigned int x, unsigned int y, float logical_x, float logical_y) : x(x), y(y), logical_x(logical_x), logical_y(logical_y)
    {
        dx = logical_x / static_cast<float>(x);
        dy = logical_y / static_cast<float>(y);

        pixels.resize(x * y);
    }
};
