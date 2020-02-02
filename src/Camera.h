#pragma once

#include <vector>
#include <memory>

#include "MathUtils.h"

class Camera {
    int width, height;
    double inv_width, inv_height;
    double aspect_ratio;
    double fov, angle;
    Vec3d origin, dir;

public:
    Camera(int width, int height, double fov, Vec3d origin = 0, Vec3d dir = {0,0,-1});
    Vec3d ray_dir_at_pixel(double x, double y);
    const Vec3d &get_origin() { return origin; }
};



