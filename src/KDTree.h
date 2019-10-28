#pragma once

#include <vector>
#include <array>
#include <memory>

#include "MathUtils.h"

class BBox{
    Vec3d min, max;

public:
    BBox(Vec3d min, Vec3d max): min{min}, max{max} {}
    BBox(const std::vector<Vec3d> &points);
    bool intersect(const Vec3d &ray_orig, const Vec3d &ray_dir);
};

class KDTree {
    struct Node{
        BBox bbox;
        std::unique_ptr<Node> left = nullptr, right = nullptr;
        int tri_index = -1;

        Node(const BBox &bbox): bbox{bbox}, left{nullptr}, right{nullptr}, tri_index{-1} {}
    };

    std::vector<Vec3d> *mesh_verts;
    std::vector<std::array<int, 3>> *mesh_tris;

    std::vector<Vec3d> centroids;
    std::unique_ptr<Node> root;

public:
    KDTree(): root{nullptr} {}
    KDTree(std::vector<Vec3d> *mesh_verts, std::vector<std::array<int, 3>> *mesh_tris);

    BBox build_bbox(const std::vector<int> &tri_indices);
    std::unique_ptr<Node> build_tree(std::vector<int> &tri_indices, int depth);


    bool ray_triangle_intersection(const Vec3d &ray_orig,
                                   const Vec3d &ray_dir,
                                   int tri_index,
                                   double &dist,
                                   Vec3d &hit_loc) const;
    int ray_intersect(const Vec3d &ray_orig,
                      const Vec3d &ray_dir,
                      double &dist,
                      Vec3d &hit_loc) const;
    int ray_intersect_helper(const Vec3d &ray_orig,
                             const Vec3d &ray_dir,
                             double &dist,
                             Vec3d &hit_loc,
                             const std::unique_ptr<Node> &node) const;
};

