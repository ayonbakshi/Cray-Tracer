#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <queue>

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

bool BBox::intersect(const Vec3d &ray_orig, const Vec3d &ray_dir, double &dist){
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
    
    dist = tmin < EPSILON ? tmax : tmin;

    return true; 
}

KDTree::KDTree(std::vector<Vec3d> *mesh_verts, std::vector<std::array<int, 3>> *mesh_tris):
    mesh_verts{mesh_verts}, mesh_tris{mesh_tris}
{
    centroids.reserve(mesh_tris->size());
    for(uint i = 0; i < mesh_tris->size(); i++){
        Vec3d centroid = {0,0,0};
        for(int j = 0; j < 3; j++) centroid = centroid + (*mesh_verts)[(*mesh_tris)[i][j]];
        centroid = centroid * (1. / 3.);
        
        centroids.push_back(centroid);
    }

    
    std::vector<int> tri_indices(mesh_tris->size());
    for(uint i = 0; i < mesh_tris->size(); i++) tri_indices[i] = i;
    root = build_tree(tri_indices, 0);
}

BBox KDTree::build_bbox(const std::vector<int> &tri_indices){
    std::vector<Vec3d> points;
    points.reserve(tri_indices.size() * 3);
    for(int i : tri_indices){
        for(int j = 0; j < 3; ++j) points.push_back((*mesh_verts)[(*mesh_tris)[i][j]]);
    }

    return BBox(points);
}

std::unique_ptr<KDTree::Node> KDTree::build_tree(std::vector<int> &tri_indices, int depth){
    int axis = depth % 3;

    if(tri_indices.size() == 0) return nullptr;
    
    // create node
    std::unique_ptr<Node> node = std::make_unique<Node>(build_bbox(tri_indices));

    // if leaf node
    if(tri_indices.size() <= leaf_node_size){
        node->tri_indices = tri_indices;
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

bool KDTree::ray_triangle_intersection(const Vec3d &ray_orig,
                                     const Vec3d &ray_dir,
                                     int tri_index,
                                     double &dist,
                                     Vec3d &hit_loc) const
{
    // Möller–Trumbore intersection algorithm from Wikipedia
    const Vec3d &vertex0 = (*mesh_verts)[(*mesh_tris)[tri_index][0]];
    const Vec3d &vertex1 = (*mesh_verts)[(*mesh_tris)[tri_index][1]];  
    const Vec3d &vertex2 = (*mesh_verts)[(*mesh_tris)[tri_index][2]];
    Vec3d edge1 = vertex1 - vertex0, edge2 = vertex2 - vertex0;
    Vec3d h, s, q;
    double a,f,u,v;
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

int KDTree::ray_intersect(const Vec3d &ray_orig,
                          const Vec3d &ray_dir,
                          double &dist,
                          Vec3d &hit_loc) const
{
    return ray_intersect_recursive(ray_orig, ray_dir, dist, hit_loc, root);
}


// I don't think this works lol
int KDTree::ray_intersect_iterative(const Vec3d &ray_orig,
                                    const Vec3d &ray_dir,
                                    double &dist,
                                    Vec3d &hit_loc) const
{
    double tmp_dist = 0;
    if(!root || !(root->bbox.intersect(ray_orig, ray_dir, tmp_dist))) return -1;
    std::priority_queue<QueueElement, std::vector<QueueElement>> heap;
    heap.emplace(root.get(), tmp_dist);

    while (!heap.empty()) {
        const Node *n = heap.top().n; 
        heap.pop();

        if (n->tri_indices.size()) {
            int closest_tri = -1;
            for (int tri_index : n->tri_indices) {
                double tmp_dist;
                Vec3d tmp_hit_loc;
                if(ray_triangle_intersection(ray_orig, ray_dir, tri_index, tmp_dist, tmp_hit_loc)){
                    if (tmp_dist < dist) {
                        dist = tmp_dist;
                        hit_loc = tmp_hit_loc;
                        closest_tri = tri_index;
                    }
                }
            }
            if (closest_tri != -1) return closest_tri;
        } else {
            double tmp_dist;
            if (n->left && n->left->bbox.intersect(ray_orig, ray_dir, tmp_dist)) {
                heap.emplace(n->left.get(), tmp_dist);
            }
            if (n->right && n->right->bbox.intersect(ray_orig, ray_dir, tmp_dist)) {
                heap.emplace(n->right.get(), tmp_dist);
            }
        }
    }
    return -1;
}

int KDTree::ray_intersect_recursive(const Vec3d &ray_orig,
                             const Vec3d &ray_dir,
                             double &dist,
                             Vec3d &hit_loc,
                             const std::unique_ptr<Node> &node) const
{
    double tmp_dist = 0;
    if(!node || !(node->bbox.intersect(ray_orig, ray_dir, tmp_dist))) return -1;

    // set dist, hit_loc and return index
    if(node->tri_indices.size()){
        int closest_tri = -1;
        for (int tri_index : node->tri_indices) {
            double tmp_dist;
            Vec3d tmp_hit_loc;
            if(ray_triangle_intersection(ray_orig, ray_dir, tri_index, tmp_dist, tmp_hit_loc)){
                if (tmp_dist < dist) {
                    dist = tmp_dist;
                    hit_loc = tmp_hit_loc;
                    closest_tri = tri_index;
                }
            }
        }
        return closest_tri;
    }

    double dist1 = INF, dist2 = INF;
    Vec3d hit_loc1, hit_loc2;
    int left_tri = ray_intersect_recursive(ray_orig, ray_dir, dist1, hit_loc1, node->left);
    int right_tri = ray_intersect_recursive(ray_orig, ray_dir, dist2, hit_loc2, node->right);

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

