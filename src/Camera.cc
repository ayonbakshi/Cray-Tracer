#include "Camera.h"
#include "MathUtils.h"

#include <cmath>

Camera::Camera(int width, int height, double fov, Vec3d origin, Vec3d dir)
    : width{width}, height{height}, fov{fov * M_PI / 180.0},
    origin{origin}, dir{dir}
    {
        inv_width = 1.0 / width;
        inv_height = 1.0 / height;
        aspect_ratio = width / double(height);
        angle = tan(0.5 * fov);
    }

Vec3d Camera::ray_dir_at_pixel(double x, double y) {
    double xx = (2 * x * inv_width - 1) * angle * aspect_ratio; 
    double yy = (1 - 2 * y * inv_height) * angle;
    Vec3d raydir(xx, yy, -1);
    raydir.normalize();
    return raydir;
}