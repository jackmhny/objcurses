/*
 * light.h
 */

#pragma once

#include "utils/mathematics.h"

class Light {
public:
    Vec3 direction;

    // constructors
    Light() : direction(Vec3(0.75f, -1.0f, -0.5f).normalize()) {}
    explicit Light(const Vec3 &dir) : direction(dir.normalize()) {}

};
