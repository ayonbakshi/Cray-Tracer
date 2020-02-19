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
        // c = c + diffuse;
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

Vec3d Mat2::sample(const Vec3d &ray_dir,
                   const Vec3d &hit_norm) const
{
    if (type == Mat2::Diffuse) {
        return (hit_norm + random_in_unit_sphere()).normalize();
    } else if (type == Mat2::Metal) {
        Vec3d refl = reflect(ray_dir, hit_norm);
        return refl + random_in_unit_sphere() * roughness;
    } else if (type == Mat2::Dielectric) {
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

        if(random_double_01() < reflect_prob) {
            return reflected.normalize();
        }
        else {
            return refracted.normalize();
        }
    }

    return 0;
}

void Mat2::eval(const Vec3d &wi,
                const Vec3d &ray_dir,
                const Vec3d &hit_norm,
                Vec3d &reflectance,
                double &pdf) const
{
    if (type == Mat2::Diffuse) {
        reflectance = albedo * M_1_PI;
        pdf = M_1_PI * 0.5; // equally likely to go in any direction in hemisphere (1/2pi)
    } else if (type == Mat2::Metal) {
        reflectance = albedo;
        pdf = 1; // this is not physically accurate. Change to a proper BRDF later
    } else if (type == Mat2::Dielectric) {
        reflectance = 1;
        pdf = 1;
    }
}

Vec3d Mat2::eval(const Vec3d &wi,
                const Vec3d &ray_dir,
                const Vec3d &hit_norm) const
{
    if (type == Mat2::Diffuse) {
        return albedo;// * (wi.dot(hit_norm) * 2);
    } else if (type == Mat2::Metal) {
        return albedo;
    } else if (type == Mat2::Dielectric) {
        return 1;
    }
    return 0;
}

bool Mat2::scatter(const Vec3d &ray_dir,
                    const Vec3d &hit_loc,
                    const Vec3d &hit_norm,
                    Vec3d &attenuation,
                    Vec3d &scattered_ray,
                    bool &include_emission) const
{
    if (type == Mat2::Diffuse) {
        attenuation = albedo;
        scattered_ray = (hit_norm + random_in_unit_sphere()).normalize();
        include_emission = false;
        return true;
    } else if (type == Mat2::Metal) {
        attenuation = albedo;
        Vec3d refl = reflect(ray_dir, hit_norm);
        // reflected ray, and random inside of sphere based on roughness
        scattered_ray = refl + random_in_unit_sphere() * roughness;
        

        // if ray is in hemisphere
        if (scattered_ray.dot(hit_norm) > 0) {
            scattered_ray.normalize();
            return true;
        } else {
            return false;
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

        if(random_double_01() < reflect_prob) {
            scattered_ray = reflected.normalize();
        }
        else {
            scattered_ray = refracted.normalize();
        }
        return true;
    } else {
        return false;
    }
}
