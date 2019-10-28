#pragma once

#include <vector>
#include <memory>

#include "MathUtils.h"
#include "Object.h"
#include "Light.h"

constexpr int ray_bounce_limit = 5;

struct Scene {
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Light> light_sources;
    Color background;

public:
    Scene();
    Scene(const Color &background);

   // TODO: figure out how to do this properly
    void add_object(Object *obj);

    void add_light(const Light &light);

    void render(const std::string &filepath, const Scene &scene, int width, int height);

private:
    Color trace(const Vec3d &ray_orig,
                const Vec3d &ray_dir,
                const Scene &scene,
                int hit_depth = 0);
};



