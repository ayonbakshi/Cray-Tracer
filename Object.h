#pragma once

#include <algorithm>
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <sstream>

#include "MathUtils.h"

struct Material{
    Color color;
};

class Object {
    public:
        Material material;
        Object(){
            material.color = 0;
        }
        Object(Material material): material{material} {};
        virtual bool ray_intersection(const Vec3d &, const Vec3d &, double &, Vec3d &, Vec3d &) const = 0;
        virtual ~Object() {}
};

class Sphere : public Object {
    Vec3d center;
    double radius;

public:
    Sphere(Vec3d center, double radius, Material material):
        Object{material}, center{center}, radius{radius} {}
    ~Sphere() {}

    bool ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const
{
        double t0, t1;
        Vec3d L = center - ray_orig; 
        double tca = L.dot(ray_dir); 
        if (tca < 0) return false;
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
};

class Plane : public Object {
    Vec3d normal, center;
    double size;

public:
    Plane(Vec3d normal, Material material, Vec3d center = 0.0, double size = INF):
        Object{material}, normal{normal}, center{center}, size{size}
    {
        normal.normalize();
    }
    ~Plane() {}

    bool ray_intersection(const Vec3d &ray_orig,
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
};

class Mesh : public Object {
    std::vector<Vec3d> verts;
    std::vector<std::array<int, 3>> tris;

public:
    Mesh(const std::string &filepath, const Material &material):
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
        std::cout << verts.size() << " vertices " << tris.size() << " triangles." << std::endl;
    }
    
    ~Mesh() {}

    bool ray_triangle_intersection(const Vec3d &ray_orig,
                                   const Vec3d &ray_dir,
                                   int tri_index,
                                   double &dist,
                                   Vec3d &hit_loc) const
    {
        const Vec3d vertex0 = verts[tris[tri_index][0]];
        const Vec3d vertex1 = verts[tris[tri_index][1]];  
        const Vec3d vertex2 = verts[tris[tri_index][2]];
        Vec3d edge1, edge2, h, s, q;
        double a,f,u,v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = ray_dir.cross(edge2);
        a = edge1.dot(h);
        if (a > -EPSILON && a < EPSILON)
            return false;    // This ray is parallel to this triangle.
        f = 1.0/a;
        s = ray_orig - vertex0;
        u = f * s.dot(h);
        if (u < 0.0 || u > 1.0)
            return false;
        q = s.cross(edge1);
        v = f * ray_dir.dot(q);
        if (v < 0.0 || u + v > 1.0)
            return false;
        // At this stage we can compute t to find out where the intersection point is on the line.
        double t = f * edge2.dot(q);
        if (t > EPSILON && t < 1/EPSILON) // ray intersection
        {
            dist = t;
            hit_loc = ray_orig + ray_dir * t;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }

    bool ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const
    {
        int closest_tri = -1;
        dist = INF;
        Vec3d tmp_hit_loc;
        for(int i = 0; i < tris.size(); i++){
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
        const Vec3d &vertex0 = verts[tris[closest_tri][0]];
        const Vec3d &vertex1 = verts[tris[closest_tri][1]];  
        const Vec3d &vertex2 = verts[tris[closest_tri][2]];
        Vec3d edge1 = vertex1 - vertex0;
        Vec3d edge2 = vertex2 - vertex0;

        hit_norm = edge1.cross(edge2);

        // JANKY SOLUTION FOR NORMALS FACING CAMERA
        if(hit_norm.dot(ray_dir) > -EPSILON) hit_norm = hit_norm * -1;
        return true;
    }

};