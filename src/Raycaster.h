#pragma once

#include <vector>
#include <memory>

#include "MathUtils.h"
#include "Object.h"

struct Scene {
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Vec3d> light_sources;
    Color background;

public:
    Scene();
    Scene(const Color &background);

   // TODO: figure out how to do this properly
    void add_object(Object *obj);

    void add_light(const Vec3d &pos);

    void render(const std::string &filepath, const Scene &scene, int width, int height);

private:
    Color trace(const Vec3d &ray_orig, const Vec3d &ray_dir, const Scene &scene);
};



