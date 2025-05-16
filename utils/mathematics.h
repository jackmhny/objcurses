/*
 * matematics.h
 */

#pragma once

#include <cmath>
#include <vector>

#define PI 3.14159265358979323846f

// 3d vector structure
class Vec3 {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z);

    // arithmetic operations
    Vec3 operator+(const Vec3 &other) const;
    Vec3 operator-(const Vec3 &other) const;
    Vec3 operator*(float scalar) const;

    Vec3 &operator+=(const Vec3 &other);
    Vec3 &operator-=(const Vec3 &other);
    Vec3 &operator*=(float scalar);

    [[nodiscard]] Vec3 operator-() const; // negation

    // magnitude of vector - length (modulus) of vector
    [[nodiscard]] float magnitude() const;

    // normalize vector - unit vector with same direction
    [[nodiscard]] Vec3 normalize() const;

    [[nodiscard]] static float dot(const Vec3 &a, const Vec3 &b);               // dot product - scalar product of two vectors
    [[nodiscard]] static Vec3 cross(const Vec3 &a, const Vec3 &b);              // cross product - vector product of two vectors
    [[nodiscard]] static float cosine_similarity(const Vec3 &a, const Vec3 &b); // cosine similarity - cosine of angle between two vectors

    [[nodiscard]] static Vec3 rotate_y(const Vec3 &v, float radians); // rotate around y axis
    [[nodiscard]] static Vec3 rotate_x(const Vec3 &v, float radians); // rotate around x axis

    [[nodiscard]] static Vec3 normal(const std::vector<Vec3> &polygon);     // normal to polygon

    [[nodiscard]] static Vec3 to_screen(const Vec3 &v, float zoom, float logical_x, float logical_y);   // transform to viewport

};
