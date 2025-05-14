/*
 * main.cpp
 */

#include <ncurses.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "entities/geometry/object.h"
#include "entities/rendering/buffer.h"

// config
const char *CHARS_LUM = " .:-=+*#%@";    // chars
const float CHAR_ASPECT_RATIO = 1.8f;
const float ANGLE_STEP = 5.0f;           // views
const float ZOOM_STEP = 0.05f;
const float ZOOM_MIN = 0.10f;
const float ZOOM_MAX  = 5.00f;

// helpers
char lum_from_normal(const Vec3 &normal, const Vec3 &light, const std::string &scale = CHARS_LUM)
{
    const float sim = (Vec3::cosine_similarity(normal, light) + 1.0f) * 0.5f;
    const int idx  = std::clamp(static_cast<int>(std::round(sim * static_cast<float>(scale.size() - 1))), 0, static_cast<int>(scale.size() - 1));
    return scale[idx];
}

Vec3 to_screen(const Vec3 v, float zoom, float logical_x, float logical_y)
{
    return {
        (v.x * zoom + 1.0f) * 0.5f * logical_x,
        (1.0f - v.y * zoom) * 0.5f * logical_y,
        (v.z * zoom + 1.0f) * 0.5f
    };
}

void normalize_object(std::vector<Vec3> &verts)
{
    if (verts.empty()) return;

    Vec3 vmin = verts[0];
    Vec3 vmax = verts[0];

    for (const auto &v : verts)
    {
        vmin.x = std::min(vmin.x, v.x);
        vmin.y = std::min(vmin.y, v.y);
        vmin.z = std::min(vmin.z, v.z);

        vmax.x = std::max(vmax.x, v.x);
        vmax.y = std::max(vmax.y, v.y);
        vmax.z = std::max(vmax.z, v.z);
    }

    const Vec3 center = (vmin + vmax) * 0.5f;
    const float scale =
        1.0f /
        std::max({vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z, 1e-6f});

    for (auto &v : verts)
    {
        v -= center;
        v *= scale;
    }
}

void init_ncurses_colors(const std::vector<Material> &materials)
{
    if (!has_colors() || !can_change_color())
        return;

    start_color();

    for (size_t i = 0; i < materials.size(); ++i)
    {
        const int pair = static_cast<int>(i) + 1;

        if (pair >= COLORS || pair >= COLOR_PAIRS)
            break;

        const auto &d = materials[i].diffuse; // 0–1
        init_color(pair,
                   static_cast<short>(std::clamp(d.x, 0.0f, 1.0f) * 1000.0f),
                   static_cast<short>(std::clamp(d.y, 0.0f, 1.0f) * 1000.0f),
                   static_cast<short>(std::clamp(d.z, 0.0f, 1.0f) * 1000.0f));
        init_pair(pair, pair, 0);
    }
}

// -----------------------------------------------------------------------------
// core drawing
// -----------------------------------------------------------------------------

void draw_model(Buffer &buf, const Object &obj, float azimuth, float altitude, float zoom, bool static_light, bool color_support)
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

        // rotate around Y (azimuth)
        auto rot_y = [az_cos, az_sin](const Vec3 &v) {
            return Vec3::rotate_y(v, std::atan2(-az_sin, az_cos));
        };

        // rotate around X (altitude)
        auto rot_x = [al_cos, al_sin](const Vec3 &v) {
            return Vec3::rotate_x(v, std::atan2(-al_sin, al_cos));
        };

        Vec3 rv1 = rot_x(rot_y(v1));
        Vec3 rv2 = rot_x(rot_y(v2));
        Vec3 rv3 = rot_x(rot_y(v3));

        // screen space
        const float lx = buf.logical_x;
        const float ly = buf.logical_y;

        Vec3 s1 = to_screen(rv1, zoom, lx, ly);
        Vec3 s2 = to_screen(rv2, zoom, lx, ly);
        Vec3 s3 = to_screen(rv3, zoom, lx, ly);

        // shading
        const Vec3 normal = -Vec3::cross(rv2 - rv1, rv3 - rv1).normalize();
        const Vec3 n_for_light = static_light ? -Vec3::cross(v2 - v1, v3 - v1).normalize() : normal;
        const char lum = lum_from_normal(n_for_light, light_dir, CHARS_LUM);

        buf.draw_projection(Projection(s1, s2, s3, lum), lum, (color_support && face.material.has_value()) ? face.material.value() : -1);
    }
}

// cli
struct Args {
    std::filesystem::path input_file;
    bool  color_support = false;    // -c / --color
    bool  static_light = false;     // -l / --light
};

static void print_help(const char *prog)
{
    std::cout <<
        "Usage: " << prog << " [OPTIONS] <file.obj>\n"
        "\n"
        "Options:\n"
        "  -c, --color          Enable colors from .mtl file\n"
        "  -l, --light          Disable light rotation\n"
        "  -h, --help           Print help\n"
        "\n"
        "Controls:\n"
        "  ←, h, a              Rotate left\n"
        "  →, l, d              Rotate right\n"
        "  ↑, k, w              Rotate up\n"
        "  ↓, j, s              Rotate down\n"
        "  +, i                 Zoom in\n"
        "  -, o                 Zoom out\n"
        "  Tab                  Toggle HUD\n"
        "  q                    Quit\n";
}


static Args parse_args(int argc, char **argv)
{
    Args a;
    for (int i = 1; i < argc; ++i)
    {
        const std::string_view arg{argv[i]};

        // help
        if (arg == "-h" || arg == "--help")
        {
            print_help(argv[0]);
            std::exit(0);
        }

        // flags
        if (arg == "-c" || arg == "--color")
        {
            a.color_support = true;
        }
        else if (arg == "-l" || arg == "--light")
        {
            a.static_light = true;
        }
        else if (arg[0] != '-')
        {
            if (!a.input_file.empty())
            {
                std::cerr << "error: more than one input file\n";
                std::exit(1);
            }
            a.input_file = arg;
        }

        // unknown
        else
        {
            std::cerr << "unknown option: " << arg << '\n';
            std::cerr << "type '--help' for usage\n";
            std::exit(1);
        }
    }

    if (a.input_file.empty())
    {
        std::cerr << "error: no input file\n";
        std::cerr << "type '--help' for usage\n";
        std::exit(1);
    }

    return a;
}

// main
int main(int argc, char **argv)
{
    const Args args = parse_args(argc, argv);

    // load object
    Object obj;
    if (!obj.load(args.input_file.string(), args.color_support))
    {
        return 1;
    }

    // normalize to unit cube
    normalize_object(obj.vertices);

    // init curses
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    timeout(0);

    if (args.color_support)
        init_ncurses_colors(obj.materials);

    // buffer
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    const float logical_y = 2.0f;
    const float logical_x = logical_y * static_cast<float>(cols) / (static_cast<float>(rows) * CHAR_ASPECT_RATIO);

    Buffer buf(static_cast<unsigned int>(cols), static_cast<unsigned int>(rows), logical_x, logical_y);

    // interactive parameters
    float azimuth  = 0.0f;
    float altitude = 0.0f;
    float zoom = std::clamp(1.0f, ZOOM_MIN, ZOOM_MAX);
    bool hud = false;

    while (true)
    {
        buf.clear();

        draw_model(buf, obj, azimuth, altitude, zoom, args.static_light, args.color_support);

        move(0, 0);
        buf.printw();

        if (hud)
        {
            mvprintw(0, 0, "zoom     %5.1f x", zoom);
            mvprintw(1, 0, "azimuth  %5.1f deg", azimuth * 180.0f / M_PI);
            mvprintw(2, 0, "altitude %5.1f deg", altitude * 180.0f / M_PI);
        }

        refresh();

        int ch = getch();

        if (ch == 'q' || ch == 'Q')     // exit
            break;

        if (ch == '\t')                 // hud
            hud = !hud;

        // keys / vim / wasd
        else if (ch == KEY_LEFT || ch == 'h' || ch == 'H' || ch == 'a' || ch == 'A')        // left rotation
            azimuth += ANGLE_STEP * M_PI / 180.0f;

        else if (ch == KEY_RIGHT || ch == 'l' || ch == 'L' || ch == 'd' || ch == 'D')        // right rotation
            azimuth -= ANGLE_STEP * M_PI / 180.0f;

        else if (ch == KEY_UP || ch == 'k' || ch == 'K' || ch == 'w' || ch == 'W')                                                  // up rotation
            altitude = std::min(altitude + ANGLE_STEP * static_cast<float>(M_PI) / 180.0f, static_cast<float>(M_PI_2));

        else if (ch == KEY_DOWN || ch == 'j' || ch == 'J' || ch == 's' || ch == 'S')                                                // down rotation
            altitude = std::max(altitude - ANGLE_STEP * static_cast<float>(M_PI) / 180.0f, -static_cast<float>(M_PI_2));

        // +- / io
        else if (ch == '+' || ch == '=' || ch == 'i' || ch == 'I')      // zoom in
            zoom = std::min(zoom + ZOOM_STEP, ZOOM_MAX);

        else if (ch == '-' || ch == 'o' || ch == 'O')                   // zoom out
            zoom = std::max(zoom - ZOOM_STEP, ZOOM_MIN);

        else if (ch == KEY_RESIZE)          // resize
        {
            getmaxyx(stdscr, rows, cols);
            const float lx = logical_y * static_cast<float>(cols) / (static_cast<float>(rows) * CHAR_ASPECT_RATIO);
            buf = Buffer(static_cast<unsigned int>(cols), static_cast<unsigned int>(rows), lx, logical_y);
        }
    }

    endwin();
    return 0;
}
