/*
 * mathematics.cpp
 */

#include "mathematics.h"

Vec3::Vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3 &other) const
{
    return { x + other.x, y + other.y, z + other.z };
}

Vec3 Vec3::operator-(const Vec3 &other) const
{
    return { x - other.x, y - other.y, z - other.z };
}

Vec3 Vec3::operator*(const float scalar) const
{
    return { x * scalar, y * scalar, z * scalar };
}

Vec3 &Vec3::operator+=(const Vec3 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3 &Vec3::operator-=(const Vec3 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3 &Vec3::operator*=(const float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vec3 Vec3::operator-() const
{
    return { -x, -y, -z };
}

float Vec3::magnitude() const
{
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalize() const
{
    const float mag = magnitude();
    return (mag > 0) ? (*this * (1.0f / mag)) : Vec3(0.0f, 0.0f, 0.0f);
}

float Vec3::dot(const Vec3 &a, const Vec3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::cross(const Vec3 &a, const Vec3 &b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float Vec3::cosine_similarity(const Vec3 &a, const Vec3 &b)
{
    const float a_mag = a.magnitude();
    const float b_mag = b.magnitude();
    return (a_mag > 0 && b_mag > 0) ? Vec3::dot(a, b) / (a_mag * b_mag) : 0.0f;
}

Vec3 Vec3::rotate_y(const Vec3 &v, const float radians)
{
    const float cos_theta = std::cos(radians);
    const float sin_theta = std::sin(radians);
    return {
        v.x * cos_theta - v.z * sin_theta,
        v.y,
        v.x * sin_theta + v.z * cos_theta
    };
}

Vec3 Vec3::rotate_x(const Vec3 &v, const float radians)
{
    const float cos_theta = std::cos(radians);
    const float sin_theta = std::sin(radians);
    return {
        v.x,
        v.y * cos_theta - v.z * sin_theta,
        v.y * sin_theta + v.z * cos_theta
    };
}

Vec3 Vec3::normal(const std::vector<Vec3> &polygon)
{
    Vec3 n(0.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < polygon.size(); i++)
    {
        const Vec3& a = polygon[i];
        const Vec3& b = polygon[(i + 1) % polygon.size()];

        n.x += (a.y - b.y) * (a.z + b.z);
        n.y += (a.z - b.z) * (a.x + b.x);
        n.z += (a.x - b.x) * (a.y + b.y);
    }

    return n;
}


Vec3 Vec3::to_screen(const Vec3 &v, float zoom, float logical_x, float logical_y)
{
    return {
        (v.x * zoom + 1.0f) * 0.5f * logical_x,
        (1.0f - v.y * zoom) * 0.5f * logical_y,
        (v.z * zoom + 1.0f) * 0.5f
    };
}
