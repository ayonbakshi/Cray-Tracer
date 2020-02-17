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
    Vec3d up, right;

public:
    Camera(int width, int height, double fov, const Vec3d &origin = 0, const Vec3d &dir = {0,0,-1});
    void move(const Vec3d &new_origin, const Vec3d &new_dir);
    void move_from_to(const Vec3d &from, const Vec3d &to);
    Vec3d ray_dir_at_pixel(double x, double y) const;
    const Vec3d &get_origin() const { return origin; }
    int get_width() const { return width; }
    int get_height() const { return height; }
private:
    void calc_axes();
};



