/*
 * buffer.cpp
 */

#include "buffer.h"

// projection methods

Projection Projection::sort_by_x() const
{
    Projection out( *this );

    Vec3 arr[3] = {out.p1, out.p2, out.p3};

    std::sort(arr, arr+3, [](const Vec3 &a, const Vec3 &b){return a.x < b.x;});

    out.p1 = arr[0];
    out.p2 = arr[1];
    out.p3 = arr[2];

    return out;
}

float Projection::limit_y1(const float x) const
{
    if (x <= p1.x) return p1.y;
    if (x >= p3.x) return p3.y;

    if (x <= p2.x)
    {
        const float denominator = p2.x - p1.x;

        if (std::fabs(denominator) < 1e-7f)
            return p1.y;

        const float t = (x - p1.x) / denominator;
        return lerp(p1.y, p2.y, t);
    }
    else
    {
        const float denominator = p3.x - p2.x;

        if (std::fabs(denominator) < 1e-7f)
            return p2.y;

        const float t = (x - p2.x) / denominator;
        return lerp(p2.y, p3.y, t);
    }
}

float Projection::limit_y2(const float x) const
{
    if (x <= p1.x)
        return p1.y;
    if (x >= p3.x)
        return p3.y;

    const float denominator = p3.x - p1.x;

    if (std::fabs(denominator) < 1e-7f)
        return p1.y;

    const float t = (x - p1.x) / denominator;
    return lerp(p1.y, p3.y, t);
}

Vec3 Projection::normal() const
{
    const Vec3 v1 = p2 - p1;
    const Vec3 v2 = p3 - p1;
    const Vec3 n  = Vec3::cross(v1, v2);

    return n.normalize();
}

// buffer methods

Buffer::Buffer(const unsigned int x, const unsigned int y, const float logical_x, const float logical_y) : x(x), y(y), logical_x(logical_x), logical_y(logical_y)
{
    if (x == 0 || y == 0)
    {
        throw std::runtime_error("buffer size must be non-zero");
    }

    dx = logical_x / static_cast<float>(x);
    dy = logical_y / static_cast<float>(y);

    pixels.resize(x * y);

    clear();
}

void Buffer::clear()
{
    for (auto &p : pixels)
    {
        p.z = INFINITY;
        p.c = ' ';
        p.material = std::nullopt;
    }
}

int Buffer::index_x(const float realX) const
{
    int ix = static_cast<int>(realX / dx);
    ix = clamp(ix, 0, static_cast<int>(x) - 1);

    return ix;
}

int Buffer::index_y(const float realY) const
{
    int iy = static_cast<int>(realY / dy);
    iy = clamp(iy, 0, static_cast<int>(y) - 1);

    return iy;
}

float Buffer::depth_at(const Projection &proj, const Vec3 &normal, int xx, int yy) const
{
    const float c_x = (static_cast<float>(xx) + 0.5f) * dx;
    const float c_y = (static_cast<float>(yy) + 0.5f) * dy;

    if (std::fabs(normal.z) < 1e-7f)
    {
        return proj.p1.z;
    }

    const float d_z = normal.x * (c_x - proj.p1.x) + normal.y * (c_y - proj.p1.y);
    const float z  = proj.p1.z - d_z / normal.z;

    return z;
}

void Buffer::draw_projection(const Projection &proj, const char c, int material)
{
    const Projection triangle = proj.sort_by_x();

    const float x_i = triangle.p1.x + dx * 0.5f;
    const float x_f = triangle.p3.x - dx * 0.5f;
    if (x_f < 0.f || x_i > logical_x)
        return;

    const int x_start = index_x(x_i);
    const int x_end   = index_x(x_f);

    const Vec3 normal = triangle.normal();

    for (int xx = x_start; xx <= x_end; xx++)
    {
        const float rx = (static_cast<float>(xx) + 0.5f) * dx;

        float y1 = triangle.limit_y1(rx);
        float y2 = triangle.limit_y2(rx);

        const float y_min = std::min(y1, y2);
        const float y_max = std::max(y1, y2);

        if (y_max < 0.f || y_min > logical_y)
            continue;

        const float y_start_val = y_min + dy * 0.5f;
        const float y_end_val = y_max - dy * 0.5f;

        const int y_start = index_y(y_start_val);
        const int y_end = index_y(y_end_val);

        for (int yy = y_start; yy <= y_end; yy++)
        {
            Pixel &pixel = pixels[yy * x + xx];

            if (const float z = depth_at(triangle, normal, xx, yy); z < pixel.z)
            {
                pixel.z = z;
                pixel.c = c;
                pixel.material = material;
            }
        }
    }
}

void Buffer::printw() const
{
    for (unsigned int row = 0; row < y; row++)
    {
        ::move(static_cast<int>(row), 0);

        for (unsigned int col = 0; col < x; col++)
        {
            const Pixel &pixel = pixels[row * x + col];

            if (const int color = pixel.material.has_value() ? (pixel.material.value() + 1) : 0; color > 0 && color < COLORS && color < COLOR_PAIRS)
            {
                attron(COLOR_PAIR(color));
                ::printw("%c", pixel.c);
                attroff(COLOR_PAIR(color));
            }
            else
            {
                ::printw("%c", pixel.c);
            }
        }
    }
}