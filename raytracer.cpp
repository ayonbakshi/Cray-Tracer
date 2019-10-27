#include <math.h>
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

#include "Object.h"
#include "MathUtils.h"

struct Scene {
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Vec3d> light_sources;
    Color background;
};

Color trace(const Vec3d &ray_orig, const Vec3d &ray_dir, const Scene &scene){
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

#include "writebmp.h"

const int width = 1080, height = 720;
Color pixels[width * height];
void render(const std::string &filepath, const Scene &scene){
    double inv_width = 1 / double(width), inv_height = 1 / double(height); 
    double fov = 45, aspectratio = width / double(height); 
    double angle = tan(3.141592 * 0.5 * fov / 180.0); 
    // Trace rays
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){ 
            double xx = (2 * ((x + 0.5) * inv_width) - 1) * angle * aspectratio; 
            double yy = (1 - 2 * ((y + 0.5) * inv_height)) * angle; 
            Vec3d raydir(xx, yy, -1); 

            raydir.normalize();

            Color c = trace(Vec3d{}, raydir, scene);
            pixels[x + y * width] = c;
        } 
    }

    drawbmp(filepath.c_str(), width, height, pixels); 
}

int main(){
    Scene scene;

    Material r = {red};
    Material g = {green};
    Material b = {blue};
    Material floor = {{69, 37, 80}};
    Material blue_floor = {{120, 80, 200}};

    using std::unique_ptr;
    scene.objects.push_back(unique_ptr<Object>(new Plane({ 0.0,      1, 0.1}, floor, {0, -8, -30}, 100))); 
    scene.objects.push_back(unique_ptr<Object>(new  Mesh("monkey.obj", blue_floor))); 
    // scene.objects.push_back(unique_ptr<Object>(new Sphere({ 0.0,      0, -20},     4.0, r))); 
    // scene.objects.push_back(unique_ptr<Object>(new Sphere({ 0.0,      5, -35},     4.0, g))); 
    // scene.objects.push_back(unique_ptr<Object>(new Sphere({ 5.0,     2, -15},     2.0, b))); 
    // scene.objects.push_back(unique_ptr<Object>(new Sphere({ 5.0,      0, -25},     3.0, r))); 
    // scene.objects.push_back(unique_ptr<Object>(new Sphere({-5.5,      0, -20},     0.5, g))); 
    
    scene.light_sources.push_back({-10,     15, 5}); 
    render(std::string("raycast.bmp"), scene);
}

