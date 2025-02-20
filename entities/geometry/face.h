/*
 * face.h
 */

#pragma once

#include <array>
#include <optional>

// triangular face
class Face {
public:
    std::array<unsigned int, 3> indices;    // indices of vertices
    std::optional<int> material;            // index of material

    Face(unsigned int idx1, unsigned int idx2, unsigned int idx3, int mat = -1) : indices{ idx1, idx2, idx3 }, material(mat) {}
};