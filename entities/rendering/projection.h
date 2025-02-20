/*
 * projection.h
 */

#pragma once

#include "utils/mathematics.h"

// projection of triangle onto screen
class Projection {
public:
    Vec3 p1, p2, p3; // vertices of triangle
    char color;      // color of triangle

    Projection(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, char color) : p1(p1), p2(p2), color(color) {}
};