#pragma once

#include <vector>
#include <memory>

#include "MathUtils.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"

constexpr int ray_bounce_limit = 10;
constexpr int russian_roulette_start_depth = 5;

struct Scene {
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Light> light_sources;
    Color background;

public:
    Scene(const Color &background = 255);

   // TODO: figure out how to do this properly
    void add_object(Object *obj);

    void add_light(const Light &light);

    void importance_sampling(const Object *closest_object,
                             const Vec3d &ray_dir,
                             const Vec3d &hit_loc,
                             const Vec3d &hit_norm,
                             Vec3d &outLightE);

    const Object *hit_scene(const Vec3d &ray_orig,
                            const Vec3d &ray_dir,
                            Vec3d &hit_loc,
                            Vec3d &hit_norm);

    std::vector<Color> render(const Camera &cam);

private:
    Color trace(const Vec3d &ray_orig,
                const Vec3d &ray_dir,
                int hit_depth = 0);
    Color trace2(const Vec3d &ray_orig,
                 const Vec3d &ray_dir,
                 int hit_depth = 0,
                 bool include_emission = true);
};



