#pragma once

#include "MathUtils.h"

class Light {
    Vec3d location;
    Color color;
    double intensity;

public:
    Light(const Vec3d &location, double intensity):
        location{location}, color{255}, intensity{intensity} {}

    const Vec3d &get_location() const { return location; }
    const Color &get_color() const { return color; }
    const double &get_intensity() const { return intensity; }
};