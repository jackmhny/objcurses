/*
 * renderer.h
 */

#pragma once

#include "entities/rendering/buffer.h"
#include "entities/geometry/object.h"
#include "algorithms.h"
#include "config.h"

// renders object into buffer with given view parameters
void render_object(Buffer &buf, const Object &obj, float azimuth, float altitude, float zoom, bool static_light, bool color_support);