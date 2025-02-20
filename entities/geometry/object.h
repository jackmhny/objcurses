/*
 * object.h
 */

#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "face.h"
#include "material.h"

#include "utils/mathematics.h"

// object (3d model)
class Object {
public:
    Object() = default;

    std::vector<Vec3> vertices;
    std::vector<Face> faces;
    std::vector<Material> materials;

    // load obj file with optional material mtl support
    bool load(const std::string &obj_filename, bool color_support = false);

private:
    bool load_materials(const std::string &mtl_filename);
    std::optional<int> find_material(const std::string &material_name) const;

    bool parse_vertex(const std::string &line);
    bool parse_face(const std::string &line, int current_material);
    bool parse_mtl_file(const std::string &line, const std::string &obj_filename);
    int parse_material(const std::string &line) const;
    bool parse_current_material(const std::string &line, std::string &current_name, Vec3 &current_diffuse, bool &have_active_material);
    static bool parse_diffuse_color(const std::string &line, Vec3 &current_diffuse);
};
