#include <cmath>
#include <omp.h>

#include "MathUtils.h"
#include "Raycaster.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"

Scene::Scene(): background{255} {}

Scene::Scene(const Color &background): background{background} {}

void Scene::add_object(Object *obj){
    objects.emplace_back(obj);
}

void Scene::add_light(const Light &light){
    light_sources.push_back(light);
}

Color Scene::trace(const Vec3d &ray_orig,
                   const Vec3d &ray_dir,
                   const Scene &scene,
                   int hit_depth){
    if(hit_depth >= ray_bounce_limit) return 0;

    double min_dist = INF;
    const Object *closest_obj = nullptr;
    Vec3d hit_loc, hit_norm;

    // calculate closest sphere
    for(auto &obj : scene.objects){
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
        double factor = 1;
        
        Vec3d light_pos = light_sources[0].get_location();
        Vec3d light_dir = light_pos - hit_loc;
        
        // double r = light_dir.norm();
        // factor *= light_sources[0].get_intensity() / (r * r);

        hit_norm.normalize();
        light_dir.normalize();

        bool in_shadow = false;
        for(auto &obj : scene.objects){
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
                reflected = reflected + trace(hit_loc, ref_ray, scene, hit_depth + 1);
            }
            c = c.mix(reflected * (1.0 / bounces), 0.2);
        }

        return c;
    }
    return background;
}

std::vector<Color> Scene::render(int width, int height){
    std::vector<Color> pixels(width * height);
    
    double fov = 45;
    Camera cam{width, height, fov};

    int samples = 4; // samples per axi
    auto trace_rays = [&](int i){
        int x = i % width;
        int y = i / width;

        // extra samples per pixel
        Color c = 0;
        for(int sx = 1; sx <= samples; ++sx){
            for(int sy = 1; sy <= samples; ++sy){ 
                double x_0 = x + (sx / double(samples + 1));
                double y_0 = y + (sy / double(samples + 1));
                c = c + trace(cam.get_origin(), cam.ray_dir_at_pixel(x_0, y_0), *this);
            }
        }
        pixels[x + y * width] = c * (1.0 / (samples*samples));

        if(i % (int)(height * width / 100.0 * 10) == 0) std::cout << i / (int)(height * width / 100.0) << std::endl;
    };

    #pragma omp parallel for
    for(int i = 0; i < height * width; ++i) trace_rays(i);

    return pixels;
}
