#include <cmath>
#include <omp.h>

#include "MathUtils.h"
#include "Raycaster.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"
#include "hdr_utils.h"


Scene::Scene(const Color &background):
    background{background}, use_environment{false} {}

void Scene::add_object(Object *obj){
    objects.emplace_back(obj);
}

void Scene::add_light(const Light &light){
    light_sources.push_back(light);
}

const Object *Scene::hit_scene(const Vec3d &ray_orig,
                               const Vec3d &ray_dir,
                               Vec3d &hit_loc,
                               Vec3d &hit_norm)
{
    double min_dist = INF;
    const Object *closest_obj = nullptr;

    // calculate closest obj
    for(auto &obj : objects){
        double dist = INF;
        Vec3d tmp_hit_loc, tmp_hit_norm;
        if(obj->ray_intersection(ray_orig, ray_dir, dist, tmp_hit_loc, tmp_hit_norm)){
            if (dist < min_dist) {
                min_dist = dist;
                closest_obj = obj.get();
                hit_loc = tmp_hit_loc;
                hit_norm = tmp_hit_norm;
            }
        }
    }
    return closest_obj;
}

void Scene::set_HDRI(const std::string &filepath) {
    use_environment = true;
    bool res = HDRLoader::load(filepath.c_str(), environment);
    if(!res) {
        std::cerr << "Cannot load HDRI: " << filepath << std::endl;
        throw 1;
    }
}

void Scene::set_env_rotation(double theta) {
    environment.theta = theta;
}

Color Scene::get_background(const Vec3d &dir) const {
    return use_environment ? environment.get_pixel(dir) : background;
}

Color Scene::trace(const Vec3d &ray_orig,
                   const Vec3d &ray_dir,
                   int hit_depth){
    if(hit_depth >= ray_bounce_limit) return 0;

    double min_dist = INF;
    const Object *closest_obj = nullptr;
    Vec3d hit_loc, hit_norm;

    // calculate closest sphere
    for(auto &obj : objects){
        double dist = INF;
        Vec3d tmp_hit_loc, tmp_hit_norm;
        if(obj->ray_intersection(ray_orig, ray_dir, dist, tmp_hit_loc, tmp_hit_norm)){
            if (dist < min_dist) {
                min_dist = dist; 
                closest_obj = obj.get();
                hit_loc = tmp_hit_loc;
                hit_norm = tmp_hit_norm;
            }
        }
    }

    if(closest_obj){
        Vec3d light_pos = light_sources[0].get_location();
        Vec3d light_dir = light_pos - hit_loc;
        
        // double r = light_dir.norm();
        // factor *= light_sources[0].get_intensity() / (r * r);

        hit_norm.normalize();
        light_dir.normalize();

        bool in_shadow = false;
        for(auto &obj : objects){
            double dist;
            Vec3d tmp;
            if(obj->ray_intersection(hit_loc, light_dir, dist, tmp, tmp)){
                in_shadow = true;
            }
        }

        Color c;
        if(in_shadow){
            return black;
        } else {
            c = closest_obj->material.calculate_color(ray_dir, hit_loc, hit_norm, light_sources);
        }
        
        if(closest_obj->material.reflective){
            // not complete specular
            int bounces = 3;
            Color reflected = 0;
            for (int i = 0; i < bounces; ++i) {
                Vec3d ref_ray = closest_obj->material.reflected_ray(ray_dir, hit_norm);
                reflected = reflected + trace(hit_loc, ref_ray, hit_depth + 1);
            }
            c = c.mix(reflected * (1.0 / bounces), 0.2);
        }

        return c;
    }
    return background;
}

Color Scene::trace2(const Vec3d &ray_orig,
                    const Vec3d &ray_dir,
                    int hit_depth,
                    bool include_emission)
{
    if (hit_depth >= ray_bounce_limit) return 0;

    Vec3d hit_loc, hit_norm;
    const Object *closest_obj = hit_scene(ray_orig, ray_dir, hit_loc, hit_norm);
    hit_norm.normalize();

    if (closest_obj) {
        Vec3d scattered, attenuation, lightE;
        const Mat2& mat = closest_obj->mat2;
        const Color &alb = mat.albedo;

        Color emissive_col = mat.emissive * include_emission;

        if(mat.scatter(ray_dir, hit_loc, hit_norm, attenuation, scattered, include_emission)){
            importance_sampling(closest_obj, ray_dir, hit_loc, hit_norm, lightE);
            // russian roulette
            double max_albedo = alb[0] > alb[1] && alb[0] > alb[2] ? alb[0] : alb[1] > alb[2] ? alb[1] : alb[2];
            if (hit_depth >= russian_roulette_start_depth||!max_albedo){
                if(random_double_01() < max_albedo) {
                    attenuation = attenuation * (1.0 / max_albedo);
                } else {
                    return emissive_col;
                }
            }

            return emissive_col + lightE + attenuation * trace2(hit_loc, scattered, hit_depth + 1, include_emission);
        } else {
            return emissive_col;
        }
    } else {
        return get_background(ray_dir);
    }
}

Color Scene::trace_iterative(Vec3d ray_orig,
                            Vec3d ray_dir)
{
    //

    Vec3d hit_loc, hit_norm;
    Vec3d attenuation = 1;
    Color c = 0;

    for (int b = 0; b < ray_bounce_limit; ++b) {
        const Object *closest_obj = hit_scene(ray_orig, ray_dir, hit_loc, hit_norm);
        if (closest_obj == nullptr) {
            c = c + attenuation * get_background(ray_dir);
            break;
        }
        hit_norm.normalize();

        const Mat2& mat = closest_obj->mat2;
        if (mat.emissive[0] > 0 || mat.emissive[1] > 0 || mat.emissive[2] > 0) {
            c = c + attenuation * mat.emissive;
        }

        Vec3d wi = mat.sample(ray_dir, hit_norm);

        Vec3d reflectance = mat.eval(wi, ray_dir, hit_norm);
        // double pdf;
        // mat.eval(wi, ray_dir, hit_norm, reflectance, pdf);

        attenuation = attenuation * reflectance;

        // russian roulette
        if (b > russian_roulette_start_depth) {
            double p = std::max(attenuation[0], std::max(attenuation[1], attenuation[2]));
            if (random_double_01() > p) {
                break;
            }
            attenuation = attenuation * (1.0 / p);
        }
        
        ray_orig = hit_loc;
        ray_dir = wi;
    }
    return c;
}

void Scene::importance_sampling(const Object *closest_object,
                                const Vec3d &ray_dir,
                                const Vec3d &hit_loc,
                                const Vec3d &hit_norm,
                                Vec3d &out_light_E)
{
    out_light_E = Vec3d(0,0,0);

    Mat2 mat = closest_object->mat2;
    if (mat.type != Mat2::Diffuse) return;

    for(auto &obj : objects){
        const Mat2 &smat = obj->mat2;
        if (smat.emissive[0] <= 0 && smat.emissive[1] <= 0 && smat.emissive[2] <= 0)
            continue; // skip non-emissive
        if (&mat == &smat)
            continue; // skip self
        
        // this has to be a sphere (or maybe disk) (this is really hacky)
        if (typeid(*obj) != typeid(Sphere)) return;
        const Sphere &s = dynamic_cast<Sphere&>(*obj);

        Vec3d sw = (s.get_center() - hit_loc).normalize();
        Vec3d su = (abs(sw[0])>0.01f ? Vec3d(0,1,0):Vec3d(1,0,0)).cross(sw).normalize();
        Vec3d sv = sw.cross(su);

        // sample sphere by solid angle
        float cos_a_max = sqrt(1.0f - s.get_radius()*s.get_radius() / (hit_loc-s.get_center()).sqrNorm());
        float eps1 = random_double_01(), eps2 = random_double_01();
        float cos_a = 1.0f - eps1 + eps1 * cos_a_max;
        float sin_a = sqrt(1.0f - cos_a*cos_a);
        float phi = 2 * M_PI * eps2;
        Vec3d l = su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a;
        l.normalize();

        
        // shadow ray
        Vec3d light_hit, light_norm;
        // if we hit the current light source
        if (hit_scene(hit_loc, l, light_hit, light_norm) == obj.get()) {
            float omega = 2 * M_PI * (1-cos_a_max);
            
            Vec3d rdir = ray_dir;
            Vec3d nl = hit_norm.dot(rdir) < 0 ? hit_norm : hit_norm * -1;
            out_light_E = out_light_E + (mat.albedo * smat.emissive) * (std::max(0.0, l.dot(nl)) * omega * M_1_PI);
        }
    }
}

#define RANDOM_ANTIALIASING

std::vector<Color> Scene::render(const Camera &cam){
    int width = cam.get_width();
    int height = cam.get_height();

    std::vector<Color> pixels(width * height);

    int samples = 6000;
    double inv_samples = 1.0 / samples;

    auto trace_rays = [&](int i){
        int x = i % width;
        int y = i / width;

        // extra aa_samples per pixel
        Color c = 0;
        for(int s = 0; s < samples; ++s) {
            #ifdef RANDOM_ANTIALIASING
            double x_0 = x + random_double_01();
            double y_0 = y + random_double_01();
            #else
            double x_0 = x + (sx / double(aa_samples + 1));
            double y_0 = y + (sy / double(aa_samples + 1));
            #endif
            // c = c + trace2(cam.get_origin(), cam.ray_dir_at_pixel(x_0, y_0));
            c = c + trace_iterative(cam.get_origin(), cam.ray_dir_at_pixel(x_0, y_0));
        }
        c = c * inv_samples;
        pixels[x + y * width] = c;

        if(i % (int)(height * width / 100.0 * 10) == 0) std::cout << i / (int)(height * width / 100.0) << std::endl;
    };

    #pragma omp parallel for
    for(int i = 0; i < height * width; ++i) trace_rays(i);

    return pixels;
}
