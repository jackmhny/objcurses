/*
 * algorithms.cpp
 */

#include "algorithms.h"

// helper functions

bool is_in_triangle(const Vec3 &pt, const Vec3 &v1, const Vec3 &v2, const Vec3 &v3)
{
    const Vec3 cross1 = Vec3::cross(v2 - v1, pt - v1);
    const Vec3 cross2 = Vec3::cross(v3 - v2, pt - v2);
    const Vec3 cross3 = Vec3::cross(v1 - v3, pt - v3);

    const bool same_sign = (cross1.z >= 0 && cross2.z >= 0 && cross3.z >= 0) || (cross1.z <= 0 && cross2.z <= 0 && cross3.z <= 0);
    return same_sign;
}

bool is_ear(const size_t i, const std::vector<Vec3> &points, const std::vector<size_t> &indices)
{
    const size_t prev = indices[(i + indices.size() - 1) % indices.size()];
    const size_t curr = indices[i];
    const size_t next = indices[(i + 1) % indices.size()];

    const Vec3 &v1 = points[prev];
    const Vec3 &v2 = points[curr];
    const Vec3 &v3 = points[next];

    // check if angle is convex
    const Vec3 d1 = v2 - v1;
    if (const Vec3 d2 = v3 - v2; Vec3::cross(d1, d2).z <= 0)
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

        if (is_in_triangle(points[indices[j]], v1, v2, v3))
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
            if (is_ear(i, points, indices))
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