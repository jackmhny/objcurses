/*
 * renderer.cpp
 */

#include "renderer.h"

// returns luminance character based on angle between normal and light
char luminance_char(const Vec3 &normal, const Vec3 &light, const std::string &scale = CHARS_LUM)
{
    const float sim = (Vec3::cosine_similarity(normal, light) + 1.0f) * 0.5f;
    const int idx = std::clamp(static_cast<int>(std::round(sim * static_cast<float>(scale.size() - 1))), 0, static_cast<int>(scale.size() - 1));
    return scale[idx];
}

void render_object(Buffer &buf, const Object &obj, float azimuth, float altitude, float zoom, bool static_light, bool color_support)
{
    const float az_cos = std::cos(azimuth);
    const float az_sin = std::sin(azimuth);
    const float al_cos = std::cos(altitude);
    const float al_sin = std::sin(altitude);

    const Vec3 light_dir = Vec3(0.75f, -1.0f, -0.5f).normalize();

    for (const auto &face : obj.faces)
    {
        const Vec3 &v1 = obj.vertices[face.indices[0]];
        const Vec3 &v2 = obj.vertices[face.indices[1]];
        const Vec3 &v3 = obj.vertices[face.indices[2]];

        // rotate around y (azimuth)
        auto rot_y = [az_cos, az_sin](const Vec3 &v) {
            return Vec3::rotate_y(v, std::atan2(-az_sin, az_cos));
        };

        // rotate around x (altitude)
        auto rot_x = [al_cos, al_sin](const Vec3 &v) {
            return Vec3::rotate_x(v, std::atan2(-al_sin, al_cos));
        };

        Vec3 rv1 = rot_x(rot_y(v1));
        Vec3 rv2 = rot_x(rot_y(v2));
        Vec3 rv3 = rot_x(rot_y(v3));

        // screen space
        const float lx = buf.logical_x;
        const float ly = buf.logical_y;

        Vec3 s1 = Vec3::to_screen(rv1, zoom, lx, ly);
        Vec3 s2 = Vec3::to_screen(rv2, zoom, lx, ly);
        Vec3 s3 = Vec3::to_screen(rv3, zoom, lx, ly);

        // shading
        const Vec3 normal = -Vec3::cross(rv2 - rv1, rv3 - rv1).normalize();
        const Vec3 n_for_light = static_light ? -Vec3::cross(v2 - v1, v3 - v1).normalize() : normal;
        const char lum = luminance_char(n_for_light, light_dir, CHARS_LUM);

        buf.draw_projection(Projection(s1, s2, s3, lum), lum, (color_support && face.material.has_value()) ? face.material.value() : -1);
    }
}