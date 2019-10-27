#include <math.h>

#include "Object.h"

Object::Object(const Material &material): material{material} {}

Sphere::Sphere(const Vec3d &center, double radius, const Material &material):
    Object{material}, center{center}, radius{radius} {}

bool Sphere::ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const
{
    double t0, t1;
    Vec3d L = center - ray_orig;
    double tca = L.dot(ray_dir); 
    // if (tca < 0) return false;
    double d2 = L.dot(L) - tca * tca; 
    if (d2 > radius * radius) return false; 
    double thc = sqrt(radius * radius - d2); 
    t0 = tca - thc; 
    t1 = tca + thc; 

    if (t0 < EPSILON && t1 < EPSILON) return false;

    dist = std::min(t0, t1);
    hit_loc = ray_orig + (ray_dir * dist);
    hit_norm = hit_loc - center;
    return true; 
}

Plane::Plane(const Vec3d &normal, const Material &material, const Vec3d &center, double size):
    Object{material}, normal{normal}, center{center}, size{size}
{
    this->normal.normalize();
}

bool Plane::ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const
{
    if(std::abs(ray_dir.dot(normal)) < EPSILON) return false;
    
    dist = (center - ray_orig).dot(normal) / ray_dir.dot(normal);
    if(dist < EPSILON) return false;

    hit_loc = ray_orig + (ray_dir * dist);

    Vec3d to_center = center - hit_loc;
    if(size != INF && to_center.dot(to_center) > size * size) return false;

    hit_norm = normal;
    return true;
}

Mesh::Mesh(const std::string &filename){
    std::ifstream infile{filename};
    std::string line;
    while(std::getline(infile, line)){
        
    }
}