/*
 * renderer.h
 */

#pragma once

#include "buffer.h"
#include "entities/geometry/object.h"
#include "entities/view/camera.h"
#include "entities/view/light.h"
#include "utils/algorithms.h"
#include "config.h"

class Renderer {
public:
    // renders object into buffer with given view parameters
    static void render(Buffer &buf, const Object &obj, const Camera &cam, const Light  &light, bool static_light, bool color_support) ;

private:
    // returns luminance character based on angle between normal and light
    static char luminance_char(const Vec3 &normal, const Vec3 &light, const std::string &scale = CHARS_LUM);
};
