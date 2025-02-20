/*
 * material.h
 */

#pragma once

#include <string>

#include "utils/mathematics.h"

// material properties
class Material {
public:
    std::string material_name;
    Vec3 diffuse; // diffuse color (Kd) - red, green, blue components

    Material(const std::string &name, const Vec3 &color) : material_name(name), diffuse(color) {}
};