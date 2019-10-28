#include <cmath>

#include "MathUtils.h"
#include "Raycaster.h"
#include "writebmp.h"

Scene::Scene(): background{255} {}

Scene::Scene(const Color &background): background{background} {}

void Scene::add_object(Object *obj){
    objects.emplace_back(obj);
}

void Scene::add_light(const Vec3d &pos){
    light_sources.push_back(pos);
}

Color Scene::trace(const Vec3d &ray_orig, const Vec3d &ray_dir, const Scene &scene){
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


    Vec3d light_pos = scene.light_sources[0];
    if(closest_obj){
        hit_norm.normalize();
        Vec3d shadow_dir = light_pos - hit_loc;
        shadow_dir.normalize();

        bool in_shadow = false;
        for(auto &obj : scene.objects){
            double dist;
            Vec3d tmp;
            if(obj->ray_intersection(hit_loc, shadow_dir, dist, tmp, tmp)){
                in_shadow = true;
            }
        }

        if(in_shadow){
            return black;
        } else {
            double factor = std::max(0.0, hit_norm.dot(shadow_dir));
            Color c = closest_obj->material.color * factor;
            return c;
        }
    }
    return background;
}

void Scene::render(const std::string &filepath, const Scene &scene, int width, int height){
    std::vector<Color> pixels(width * height);
    
    double inv_width = 1 / double(width), inv_height = 1 / double(height); 
    double fov = 45, aspectratio = width / double(height); 
    double angle = tan(3.141592 * 0.5 * fov / 180.0); 
    // Trace rays
    
    std::vector<int> indices(height * width);
    for(int i = 0; i < height * width; ++i) indices[i] = i;

    auto trace_rays = [&](int i){
        int x = i % width;
        int y = i / width;
        double xx = (2 * ((x + 0.5) * inv_width) - 1) * angle * aspectratio; 
        double yy = (1 - 2 * ((y + 0.5) * inv_height)) * angle; 
        Vec3d raydir(xx, yy, -1); 

        raydir.normalize();

        Color c = trace(Vec3d{}, raydir, scene);
        pixels[x + y * width] = c;

        if(i % (int)(height * width / 100.0 * 10) == 0) std::cout << i / (int)(height * width / 100.0) << std::endl;
    };

    // igl::parallel_for(height * width, trace_rays);
    for(int i = 0; i < height * width; ++i) trace_rays(i);

    drawbmp(filepath.c_str(), width, height, pixels.data()); 
}