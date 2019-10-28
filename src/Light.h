#pragma once

#include "MathUtils.h"

class Light {
    Vec3d location;
    double intensity;

public:
    Light(const Vec3d &location, double intensity): location{location}, intensity{intensity} {}

    const Vec3d &get_location() { return location; }
    const double &get_intensity() { return intensity; }
};