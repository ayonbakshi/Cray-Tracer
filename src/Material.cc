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

        // c = c + diffuse * k_d + specular * k_s;
        c = c + diffuse;
    }
    
    return c;
}

Vec3d Material::reflected_ray(const Vec3d &ray_dir,
                              const Vec3d &hit_norm) const
{
    double fuzz = 0.3;
    Vec3d perfect_reflection = ray_dir - hit_norm * 2 * ray_dir.dot(hit_norm);
    Vec3d r = perfect_reflection + random_in_unit_sphere() * fuzz;
    r.normalize();
    return r;
}

std::vector<Vec3d> Mat2::scattered_rays(const Vec3d &ray_dir,
                                        const Vec3d &hit_loc,
                                        const Vec3d &hit_norm,
                                        Vec3d &attenuation,
                                        Vec3d &outLightE) const
{
    int num_scattered = 1;
    std::vector<Vec3d> ray_dirs;
    ray_dirs.reserve(num_scattered);

    // only do diffuse now
    outLightE = Vec3d(0,0,0);

    if (type == Mat2::Diffuse) {
        attenuation = albedo;

        while (int(ray_dirs.size()) != num_scattered) {
            Vec3d dir = hit_norm + random_in_unit_sphere();
            dir.normalize();
            ray_dirs.push_back(dir);
        }
    } else if (type == Mat2::Metal) {
        attenuation = albedo;

        Vec3d refl = reflect(ray_dir, hit_norm);
        // reflected ray, and random inside of sphere based on roughness
        while (int(ray_dirs.size()) != num_scattered) {
            Vec3d dir = refl + random_in_unit_sphere() * roughness;
            dir.normalize();
            if (dir.dot(hit_norm) > 0)
                ray_dirs.push_back(dir);
        }
    } else if (type == Mat2::Dielectric) {
        attenuation = Vec3d(1.0,1.0,1.0);

        Vec3d outward_normal;
        Vec3d reflected = reflect(ray_dir, hit_norm);
        Vec3d refracted;

        float ni_over_nt;
        float reflect_prob;
        float cosine;
        
        // when ray shoot through object back into vacuum,
        // ni_over_nt = refract_ind, surface normal has to be inverted.
        if (ray_dir.dot(hit_norm) > 0){
            outward_normal = hit_norm * -1;
            ni_over_nt = refract_ind;
            cosine = ray_dir.dot(hit_norm);
        }
        // when ray shoots into object,
        // ni_over_nt = 1 / refract_ind.
        else{
            outward_normal = hit_norm;
            ni_over_nt = 1.0 / refract_ind;
            cosine = -ray_dir.dot(hit_norm);
        }
        

        // refracted ray exists
        if(refract(ray_dir, outward_normal, ni_over_nt, refracted)){
            reflect_prob = schlick(cosine, refract_ind);
        }
        // refracted ray does not exist
        else{
            // total reflection
            reflect_prob = 1.0;
        }

        if(RandomFloat01() < reflect_prob) {
            ray_dirs.push_back(reflected.normalize());
        }
        else {
            ray_dirs.push_back(refracted.normalize());
        }


    }

    return ray_dirs;
}
