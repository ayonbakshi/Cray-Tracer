#include <vector>
#include <array>
#include <memory>
#include <algorithm>

#include "MathUtils.h"
#include "Object.h"
#include "KDTree.h"

BBox::BBox(const std::vector<Vec3d> &points) {
    min = points.at(0);
    max = points.at(0);
    
    for(auto &p : points){
        for(int i = 0; i < 3; i++){
            min[i] = std::min(min[i], p[i]);
            max[i] = std::max(max[i], p[i]);
        }
    }
}

bool BBox::intersect(const Vec3d &ray_orig, const Vec3d &ray_dir){
    double tmin = (min[0] - ray_orig[0]) / ray_dir[0]; 
    double tmax = (max[0] - ray_orig[0]) / ray_dir[0]; 

    if (tmin > tmax) std::swap(tmin, tmax); 

    double tymin = (min[1] - ray_orig[1]) / ray_dir[1]; 
    double tymax = (max[1] - ray_orig[1]) / ray_dir[1]; 

    if (tymin > tymax) std::swap(tymin, tymax); 

    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 

    if (tymin > tmin) 
        tmin = tymin; 

    if (tymax < tmax) 
        tmax = tymax; 

    double tzmin = (min[2] - ray_orig[2]) / ray_dir[2]; 
    double tzmax = (max[2] - ray_orig[2]) / ray_dir[2]; 

    if (tzmin > tzmax) std::swap(tzmin, tzmax); 

    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 

    if (tzmin > tmin) 
        tmin = tzmin; 

    if (tzmax < tmax) 
        tmax = tzmax; 

    return true; 
}

KDTree::KDTree(Mesh *mesh): mesh{mesh}
{
    centroids.reserve(mesh->tris.size());
    for(uint i = 0; i < mesh->tris.size(); i++){
        Vec3d centroid = {0,0,0};
        for(int j = 0; j < 3; j++) centroid = centroid + mesh->verts[mesh->tris[i][j]];
        centroid = centroid * (1. / 3.);
        
        centroids.push_back(centroid);
    }

    
    std::vector<int> tri_indices(mesh->tris.size());
    for(uint i = 0; i < mesh->tris.size(); i++) tri_indices[i] = i;
    root = build_tree(tri_indices, 0);
}

BBox KDTree::build_bbox(const std::vector<int> &tri_indices){
    std::vector<Vec3d> points;
    points.reserve(tri_indices.size() * 3);
    for(int i : tri_indices){
        for(int j = 0; j < 3; ++j) points.push_back(mesh->verts[mesh->tris[i][j]]);
    }

    return BBox(points);
}

std::unique_ptr<KDTree::Node> KDTree::build_tree(std::vector<int> &tri_indices, int depth){
    int axis = depth % 3;

    if(tri_indices.size() == 0) return nullptr;
    
    // create node
    std::unique_ptr<Node> node = std::make_unique<Node>(build_bbox(tri_indices));

    // if leaf node
    if(tri_indices.size() == 1){
        node->tri_index = tri_indices[0];
        return node;
    }

    // partition triangles
    std::sort(tri_indices.begin(), tri_indices.end(), 
        [&](const int &a, const int &b)
        { 
        return centroids[a][axis] > centroids[a][axis]; 
        });

    int midpoint = tri_indices.size() / 2;
    std::vector<int> lower = std::vector<int>(tri_indices.begin(), tri_indices.begin() + midpoint);
    std::vector<int> upper = std::vector<int>(tri_indices.begin() + midpoint, tri_indices.end());

    node->left = build_tree(lower, depth + 1);
    node->right = build_tree(upper, depth + 1);

    return node;
}

int KDTree::ray_intersect(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc) const {
    return ray_intersect_helper(ray_orig, ray_dir, dist, hit_loc, root);
}

int KDTree::ray_intersect_helper(const Vec3d &ray_orig,
                             const Vec3d &ray_dir,
                             double &dist,
                             Vec3d &hit_loc,
                             const std::unique_ptr<Node> &node) const
{
    if(!node || !(node->bbox.intersect(ray_orig, ray_dir))) return -1;

    // set dist, hit_loc and return index
    if(node->tri_index != -1){
        double tmp_dist;
        Vec3d tmp_hit_loc;
        if(mesh->ray_triangle_intersection(ray_orig, ray_dir, node->tri_index, tmp_dist, tmp_hit_loc)){
            dist = tmp_dist;
            hit_loc = tmp_hit_loc;
            return node->tri_index;
        } else {
            return -1;
        }
    }

    double dist1 = INF, dist2 = INF;
    Vec3d hit_loc1, hit_loc2;
    int left_tri = ray_intersect_helper(ray_orig, ray_dir, dist1, hit_loc1, node->left);
    int right_tri = ray_intersect_helper(ray_orig, ray_dir, dist2, hit_loc2, node->right);

    if(left_tri == -1 && right_tri == -1) return -1;
    else if(dist1 < dist2){
        dist = dist1;
        hit_loc = hit_loc1;
        return left_tri;
    } else {
        dist = dist2;
        hit_loc = hit_loc2;
        return right_tri;
    }
}

