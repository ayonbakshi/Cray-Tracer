#include <math.h>
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

constexpr double INF = 1e10;

template<typename T>
struct Vec3 {
    std::array<T, 3> p;
    Vec3(): p{{0,0,0}} {}
    Vec3(T x, T y, T z): p{{x,y,z}} {}
    Vec3(std::array<T, 3> &arr): p{arr} {}
    T &x() { return p[0]; }
    T &y() { return p[1]; }
    T &z() { return p[2]; }
    T &r() { return p[0]; }
    T &g() { return p[1]; }
    T &b() { return p[2]; }
    void print(){ std::cout << x() << ":" << y() << ":" << z() << std::endl;}

    T norm() const {
        T sum;
        for(const T &t : p) sum += t * t;
        return sqrt(sum);
    }
    void normalize() {
        T mag = norm();
        if(mag != 0)
            for(T &t : p) t /= mag;
    }

    T dot(const Vec3<T> &other) { return p[0] * other.p[0] + p[1] * other.p[1] + p[2] * other.p[2]; }

    Vec3<T> operator*(const double &s) const { return Vec3<T>{s * p[0], s * p[1], s * p[2]}; }
    Vec3<T> operator+(const Vec3<T> &other) const {return Vec3<T>{p[0] + other.p[0], p[1] + other.p[1], p[2] + other.p[2]}; }
    Vec3<T> operator-(const Vec3<T> &other) const {return Vec3<T>{p[0] - other.p[0], p[1] - other.p[1], p[2] - other.p[2]}; }
};

typedef Vec3<double> Vec3d;
typedef Vec3<int> Color;

const Color red{255, 0, 0};
const Color green{0, 255, 0};
const Color blue{0, 0, 255};
const Color black{0, 0, 0};
const Color white{255, 255, 255};
const Color background{160, 255, 253};

struct Material{
    Color color;
};


// TODO add ray_intersect fn for different objects. Return dist, hit loc and surface normal
struct Sphere {
    Vec3d center;
    double radius;
    Material material;

    Sphere(Vec3d center, double radius, Material material):
        center{center}, radius{radius}, material{material} {}

    bool ray_intersection(const Vec3d &ray_orig, const Vec3d &ray_dir, double &dist, Vec3d &hit_loc, Vec3d &hit_norm) const {
        double t0, t1;
        Vec3d L = center - ray_orig; 
        double tca = L.dot(ray_dir); 
        // if (tca < 0) return false;
        double d2 = L.dot(L) - tca * tca; 
        if (d2 > radius * radius) return false; 
        double thc = sqrt(radius * radius - d2); 
        t0 = tca - thc; 
        t1 = tca + thc; 
        

        if (t0 > t1) std::swap(t0, t1);

        if (t0 < 1e-6) { 
            t0 = t1; // if t0 is negative, let's use t1 instead 
            if (t0 < 1e-6) return false; // both t0 and t1 are negative 
        }

        dist = t0;
        hit_loc = ray_orig + (ray_dir * dist);
        hit_norm = hit_loc - center;
        return true; 
    }
};  

struct Scene {
    std::vector<Sphere> objects;
    std::vector<Vec3d> light_sources;
    Color background;
};

Color trace(const Vec3d &ray_orig, const Vec3d &ray_dir, const Scene &scene){
    double min_dist = INF;
    const Sphere *closest_obj = nullptr;
    Vec3d hit_loc, hit_norm;

    // calculate closest sphere
    bool outline = false;
    for(const Sphere &obj : scene.objects){
        double dist = INF;
        Vec3d tmp_hit_loc, tmp_hit_norm;
        if(obj.ray_intersection(ray_orig, ray_dir, dist, tmp_hit_loc, tmp_hit_norm)){
            if (dist < min_dist) { 
                min_dist = dist; 
                closest_obj = &obj;
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
        for(const Sphere &obj : scene.objects){
            double dist;
            Vec3d tmp;
            if(obj.ray_intersection(hit_loc, shadow_dir, dist, tmp, tmp)){
                in_shadow = true;
            }
        }
        
        // if(in_shadow){
        //     return black;
        // } else {
            double factor = std::max(0.0, hit_norm.dot(shadow_dir));
            Color clr = closest_obj->material.color;
            // std::cout << factor << std::endl;
            Color c = closest_obj->material.color * (factor);

            // std::cout << factor << " " << c.r() << " " << c.g() << " " << c.b() << std::endl;
            // std::cout << factor << std::endl;
            return c;
        // }
    }
    return background;
}

#include "writebmp.h"

const int width = 1080, height = 720;
Color pixels[width * height];
void makeppm(const std::string &filepath, int w, int h){
    std::ofstream img{filepath};
    img << "P3" << std::endl;
    img << w << " " << h << std::endl;
    img << "255" << std::endl;

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            Color c = pixels[x + y * w];
            img << c.r() << " " << c.g() << " " << c.b() << std::endl;
        }
    }
}

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

    scene.objects.push_back(Sphere({ 0.0, -10004, -20}, 10000.0, floor)); 
    scene.objects.push_back(Sphere({ 0.0,      0, -20},     4.0, r)); 
    scene.objects.push_back(Sphere({ 0.0,      5, -35},     4.0, g)); 
    scene.objects.push_back(Sphere({ 5.0,     2, -15},     2.0, b)); 
    scene.objects.push_back(Sphere({ 5.0,      0, -25},     3.0, r)); 
    scene.objects.push_back(Sphere({-5.5,      0, -20},     0.5, g)); 
    
    scene.light_sources.push_back({-10,     15, -15}); 
    // light
    // for(double i = 0; i < 20; i += 0.1){
        // scene.light_sources[0].x() = i;
        render(std::string("raycast.bmp"), scene);
    // }
}

