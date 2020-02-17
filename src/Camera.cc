#include "Camera.h"
#include "MathUtils.h"

#include <cmath>

Camera::Camera(int width, int height, double fov, const Vec3d &origin, const Vec3d &dir)
    : width{width}, height{height}, fov{fov * M_PI / 180.0},
    origin{origin}, dir{dir}
    {
        inv_width = 1.0 / width;
        inv_height = 1.0 / height;
        aspect_ratio = width / double(height);
        angle = tan(0.5 * fov);

        calc_axes();
    }

void Camera::move(const Vec3d &new_origin, const Vec3d &new_dir) {
    origin = new_origin;
    dir = new_dir;
    dir.normalize();
    calc_axes();
}

void Camera::move_from_to(const Vec3d &from, const Vec3d &to) {
    move(from, to - from);
}

Vec3d Camera::ray_dir_at_pixel(double x, double y) const {
    double xx = (2 * x * inv_width - 1) * angle * aspect_ratio; 
    double yy = (1 - 2 * y * inv_height) * angle;

    Vec3d img_pt = right * xx + up * yy + dir;
    img_pt.normalize();
    return img_pt;
}

void Camera::calc_axes() {
    // approximately "up"
    Vec3d tmp_up = {0, 1, 0};
    tmp_up.normalize();
    right = dir.cross(tmp_up);
    right.normalize();
    up = right.cross(dir);
}