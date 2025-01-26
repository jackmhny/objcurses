/*
 * object.h
 */

#pragma once

#include "../utils/mathematics.h"

#include <string>
#include <vector>
#include <array>
#include <optional>

// triangular face
class Face {
public:
    std::array<unsigned int, 3> indices; // indices of vertices
    int material;                        // index of material (-1 for no material)

    explicit Face(unsigned int idx1 = 0, unsigned int idx2 = 0, unsigned int idx3 = 0, int mat = -1): indices{idx1, idx2, idx3}, material(mat) {}
};

// material properties
class Material {
public:
    std::string material_name;
    Vec3 diffuse;               // diffuse color (Kd) - red, green, blue components

    explicit Material(const std::string& name = "", const Vec3& color = {1.0f, 1.0f, 1.0f}): material_name(name), diffuse(color) {}
};

// object (3d model)
class Object {
public:
    Object() = default;

    std::vector<Vec3> vertices;
    std::vector<Face> faces;
    std::vector<Material> materials;

    // load obj file with optional material mtl support
    bool load(const std::string& obj_filename, bool color_support = false);

private:

    void load_materials(const std::string& mtl_filename);
    std::optional<int> find_material(const std::string& material_name);
};