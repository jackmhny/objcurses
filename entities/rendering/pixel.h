/*
 * pixel.h
 */

#pragma once

// screen pixel
class Pixel {
public:
    float z;      // depth (z-coordinate)
    char c;       // character
    int material; // material index

    Pixel() : z(0.0f), c(' '), material(-1) {}
    Pixel(float z, char c, int material) : z(z), c(c), material(material) {}
};
