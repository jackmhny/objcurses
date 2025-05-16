/*
 * algorithms.cpp
 */

#include "algorithms.h"

// helper functions

static bool is_in_triangle(const Vec3 &pt, const Vec3 &v1, const Vec3 &v2, const Vec3 &v3, const Vec3 &normal)
{
    const float s1 = Vec3::dot(Vec3::cross(v2 - v1, pt - v1), normal);
    const float s2 = Vec3::dot(Vec3::cross(v3 - v2, pt - v2), normal);
    const float s3 = Vec3::dot(Vec3::cross(v1 - v3, pt - v3), normal);

    const bool same_sign = (s1 >= 0 && s2 >= 0 && s3 >= 0) || (s1 <= 0 && s2 <= 0 && s3 <= 0);

    return same_sign;
}

static bool is_ear(const size_t i, const std::vector<Vec3> &points, const std::vector<size_t> &indices, const Vec3 &normal)
{
    const size_t prev = indices[(i + indices.size() - 1) % indices.size()];
    const size_t curr = indices[i];
    const size_t next = indices[(i + 1) % indices.size()];

    const Vec3 &v1 = points[prev];
    const Vec3 &v2 = points[curr];
    const Vec3 &v3 = points[next];

    // check if angle is convex
    const Vec3 d1 = v2 - v1;
    const Vec3 d2 = v3 - v2;

    if (Vec3::dot(Vec3::cross(d1, d2), normal) <= 0.0f)
    {
        return false; // not convex
    }

    // check for no other points inside triangle
    for (size_t j = 0; j < indices.size(); j++)
    {
        if (j == (i - 1 + indices.size()) % indices.size() || j == i || j == (i + 1) % indices.size())
        {
            continue;
        }

        if (is_in_triangle(points[indices[j]], v1, v2, v3, normal))
        {
            return false; // point inside triangle
        }
    }

    return true; // ear found
}

// main functions

float lerp(const float a, const float b, const float t)
{
    return a + (b - a) * t;
}

std::optional<std::vector<size_t>> triangularize(const std::vector<Vec3> &points)
{
    const size_t n = points.size();
    if (n < 3)
    {
        return std::nullopt; // insufficient points
    }

    const Vec3 normal = Vec3::normal(points);

    if (normal.magnitude() < 1e-6f)
    {
        return std::nullopt; // degenerate polygon
    }

    // list of vertex indexes
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);

    std::vector<size_t> result;

    // ears search
    while (indices.size() > 3)
    {
        bool ear_found = false;

        for (std::size_t i = 0; i < indices.size(); i++)
        {
            if (is_ear(i, points, indices, normal))
            {
                // adding triangle
                size_t prev = indices[(i + indices.size() - 1) % indices.size()];
                size_t curr = indices[i];
                size_t next = indices[(i + 1) % indices.size()];

                result.push_back(prev);
                result.push_back(curr);
                result.push_back(next);

                // removing current ear
                indices.erase(std::next(indices.begin(), static_cast<std::ptrdiff_t>(i)));
                ear_found = true;
                break;
            }
        }

        if (!ear_found)
        {
            return std::nullopt; // no valid ear
        }
    }

    // adding last triangle
    result.push_back(indices[0]);
    result.push_back(indices[1]);
    result.push_back(indices[2]);

    return result;
}

float deg2rad(float degree)
{
    return degree * PI / 180.f;
}

float rad2deg(float radian)
{
    return radian * 180.f / PI;
}

float clamp0(float value, float eps)
{
    return (std::fabs(value) < eps) ? 0.0f : value;
}

float deg_norm(float degree)
{
    degree = std::fmod(degree, 360.0f);
    return degree < 0.0f ? degree + 360.0f : degree;
}

float rad_norm(float radian)
{
    radian = std::fmod(radian, 2 * PI);
    return (radian <= -PI) ? radian + 2 * PI : (radian > PI) ? radian - 2 * PI : radian;
}
