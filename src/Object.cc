#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Object.h"
#include "MathUtils.h"
#include "Material.h"


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

Mesh::Mesh(const std::string &filepath, const Material &material):
    Object{material}
{
    std::ifstream obj_file{filepath};
    std::string line;
    
    while(std::getline(obj_file, line)){
        std::stringstream ss{line};
        std::string temp;

        ss >> temp;
        if(temp == "v"){
            double x,y,z;
            ss >> x >> y >> z;
            verts.push_back({x,y,z});

        } else if (temp == "f"){
            std::string vert_info;
            std::array<int, 3> vert_indices;
            for(int i = 0; i < 3; i++){
                ss >> vert_info;
                int first_slash = vert_info.find("/");
                vert_indices[i] = std::stoi(vert_info.substr(0, first_slash)) - 1;
            }
            tris.push_back(vert_indices);
        }
    }
    
    tri_norms.reserve(tris.size());
    for(uint i = 0; i < tris.size(); i++){
        const Vec3d &vertex0 = verts[tris[i][0]];
        const Vec3d &vertex1 = verts[tris[i][1]];  
        const Vec3d &vertex2 = verts[tris[i][2]];
        Vec3d edge1 = vertex1 - vertex0;
        Vec3d edge2 = vertex2 - vertex0;
        tri_norms.push_back(edge1.cross(edge2));
    }

    
    kdtree = KDTree(&verts, &tris);

    std::cout << "Loaded " << filepath << " with " << verts.size()
        << " verts and " << tris.size() << " tris." << std::endl;
}

#define KDTREE
#ifdef KDTREE

bool Mesh::ray_intersection(const Vec3d &ray_orig,
                            const Vec3d &ray_dir,
                            double &dist,
                            Vec3d &hit_loc,
                            Vec3d &hit_norm) const
{
    int closest_tri = kdtree.ray_intersect(ray_orig, ray_dir, dist, hit_loc);
    if(closest_tri == -1) return false;
    hit_norm = tri_norms[closest_tri];
    return true;
}

#else
bool Mesh::ray_intersection(const Vec3d &ray_orig,
                            const Vec3d &ray_dir,
                            double &dist,
                            Vec3d &hit_loc,
                            Vec3d &hit_norm) const
{
    int closest_tri = -1;
    dist = INF;
    Vec3d tmp_hit_loc;
    for(uint i = 0; i < tris.size(); i++){
        double tri_dist = INF;
        if(ray_triangle_intersection(ray_orig, ray_dir, i, tri_dist, tmp_hit_loc)){
            if(tri_dist < dist){
                closest_tri = i;
                dist = tri_dist;
                hit_loc = tmp_hit_loc;
            }
        }
    }

    if(closest_tri == -1) return false;
    hit_norm = tri_norms[closest_tri];
    return true;
}
#endif