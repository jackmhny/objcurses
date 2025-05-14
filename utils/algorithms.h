/*
 * algorithms.h
 */

#pragma once

#include <numeric>
#include <optional>
#include <vector>

#include "mathematics.h"

// linear interpolation
float lerp(float a, float b, float t);

// limiting value to range between minimum and maximum value
template<typename T>
T clamp(const T &value, const T &low, const T &high)
{
    return (value < low) ? low : (value > high ? high : value);
}

// polygon triangulation
std::optional<std::vector<size_t>> triangularize(const std::vector<Vec3> &points);

// transformations
float deg2rad(float degree);
float rad2deg(float radian);

float clamp0(float value, float eps = 0.05f);

float deg_norm(float degree);
float rad_norm(float radian);