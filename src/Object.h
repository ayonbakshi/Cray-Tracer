#pragma once

#include <algorithm>
#include <vector>
#include <array>

#include "MathUtils.h"
#include "KDTree.h"
#include "Material.h"

class Object {
    public:
        Material material;
        Mat2 mat2;
        Object(const Material &material);
        Object(const Mat2 &mat2);
        virtual bool ray_intersection(const Vec3d &, const Vec3d &, double &, Vec3d &, Vec3d &) const = 0;
        virtual ~Object() {}
};

class Sphere : public Object {
    Vec3d center;
    double radius;

public:
    Sphere(const Vec3d &center, double radius, const Material &material);
    Sphere(const Vec3d &center, double radius, const Mat2 &mat2);
    ~Sphere() {}

    const Vec3d &get_center() const { return center; }
    const double &get_radius() const { return radius; }
    bool ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const;
};

class Plane : public Object {
    Vec3d normal, center;
    double size;

public:
    Plane(const Vec3d &normal, const Vec3d &center, const Mat2 &mat2, double size = INF);
    ~Plane() {}

    bool ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const;
};

class Mesh : public Object {
    std::vector<Vec3d> verts;
    std::vector<std::array<int, 3>> tris;
    std::vector<Vec3d> tri_norms;
    KDTree kdtree;

public:
    Mesh(const std::string &filepath, const Mat2 &mat2);
    ~Mesh() {}

    bool ray_intersection(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc,
                          Vec3d &hit_norm) const;

    friend class KDTree;
};