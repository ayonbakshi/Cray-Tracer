#pragma once

#include <cmath>
#include <vector>

#include "MathUtils.h"
#include "Light.h"

struct Material{
    Color color_a = 0, color_d = 0, color_s = 0;
    double k_a = 0, k_d = 0, k_s = 0, alpha = 0;
    bool reflective = false;

    Color calculate_color(const Vec3d &ray_dir,
                          const Vec3d &hit_loc,
                          const Vec3d &hit_norm,
                          const std::vector<Light> lights) const;
    Vec3d reflected_ray(const Vec3d &ray_dir,
                        const Vec3d &hit_norm) const;
};


// refactor to be polymorphic
struct Mat2{
    enum MatType {Diffuse, Metal, Dielectric};
    MatType type;
    Vec3d albedo;
    Vec3d emissive;
    double roughness;
    double refract_ind;

    bool scatter(const Vec3d &ray_dir,
                 const Vec3d &hit_loc,
                 const Vec3d &hit_norm,
                 Vec3d &attenuation,
                 Vec3d &scattered_dir,
                 bool &include_emission) const;
};