#include "Material.h"

Color Material::calculate_color(const Vec3d &ray_dir,
                                const Vec3d &hit_loc,
                                const Vec3d &hit_norm,
                                const std::vector<Light> lights) const
{
    Vec3d to_viewer = ray_dir * -1;
    Color c = 0;

    Color ambient = color_a * k_a;
    c = c + ambient;

    for(auto &light : lights){
        Vec3d light_dir = light.get_location() - hit_loc;
        light_dir.normalize();

        Vec3d R_m = hit_norm * 2 * light_dir.dot(hit_norm) - light_dir;
        R_m.normalize();
        
        Color diffuse = color_d * k_d * std::max(0.0, light_dir.dot(hit_norm));
        Color specular = color_s * k_s * std::pow(std::max(0.0, R_m.dot(to_viewer)), alpha);

        c = c + diffuse * k_d + specular * k_s;
    }
    return c;
}
