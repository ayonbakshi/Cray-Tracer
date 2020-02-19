#include <chrono>

#include "Raycaster.h"
#include "writebmp.h"

Material make_diffuse_mat(const Color &color){
    Material m;
    m.color_d = color;
    m.k_d = 1;
    return m;
}

Material test_mat(const Color &color){
    Material m;
    m.color_d = color;
    m.color_a = white;
    m.color_s = white;

    m.k_d = 0.5;
    m.k_a = 0.2;
    m.k_s = 0.5;
    m.alpha = 16;
    return m;
}

Scene mat1_test_scene(){
    // Material test = test_mat(white);
    // Material r = make_diffuse_mat(white);
    // r.reflective = true;
    // Material g = make_diffuse_mat(green);
    // Material b = make_diffuse_mat(blue);
    // Material w = make_diffuse_mat(white);
    // Material floor = make_diffuse_mat({69/255.0, 37/255.0, 80/255.0});
    // Material blue_floor = make_diffuse_mat({120/255.0, 80/255.0, 200/255.0});
    Scene scene{background};

    // Vec3d to = {0, 2, 0};

    // scene.add_object(new Plane({ 0.0,      1, 0.0}, 0, blue_floor, 100)); 
    // scene.add_object(new Sphere(to, 1, r));

    // scene.add_object(new Plane({ 0.0,      1, 0.1}, {0, -7, -30}, blue_floor, 100)); 
    // scene.add_object(new Mesh("../assets/polysphere.obj", r));  
    // scene.add_object(new Mesh("/home/ayon/Downloads/dragon_large.obj", r)); 
    // scene.add_object(new Mesh("../assets/mirror.obj", floor)); 
    // scene.add_object(new Sphere({ 5,      0, -30},     1, blue_floor)); 
    // scene.add_object(new Sphere({ 0.0,      5, -35},     4.0, r)); 
    // scene.add_object(new Sphere({ 5.0,     2, -15},     2.0, r)); 
    // scene.add_object(new Sphere({ 5.0,      0, -25},     3.0, r))); 
    // scene.add_object(new Sphere({-5.5,      0, -20},     0.5, g))); 
    
    // scene.add_light(Light({-5,     5, 0}, 300));
    return scene;
}

Scene mat2_test_scene() {
    std::vector<Vec3d> sphere_posns = {
        {2,0,-1},
        {0,0,-0.5},
        {-2,0,-1},
        {1,0,0.5},
        {5,1,0},
        {-1,0,0.5},
        {0.f,0.,0.f},
        {-1.7f,1.5f,1.f},
        {1.7f,1.5f,1.f}
    };
    std::vector<double> sphere_r = {
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.2f,
        0.5f,
        0.6f,
        0.3f,
        0.3f
    };

    std::vector<Mat2> mats = {
        { Mat2::Diffuse, Vec3d(0.8f, 0.8f, 0.8f), Vec3d(0,0,0), 0, 0 },
        { Mat2::Diffuse, Vec3d(0.4f, 0.4f, 0.8f), Vec3d(0,0,0), 0, 0 },
        { Mat2::Metal, Vec3d(1, 1, 1), Vec3d(0,0,0), 0, 0 },
        { Mat2::Metal, Vec3d(0.79f, 0.56f, 0.21f), Vec3d(0,0,0), 0.0, 0 },
        { Mat2::Diffuse, Vec3d(0.4f, 0.0f, 0.8f), Vec3d(10,10,20), 0, 0 },
        { Mat2::Metal, Vec3d(0.4f, 0.8f, 0.4f), Vec3d(0,0,0), 0.6f, 0 },
        { Mat2::Dielectric, Vec3d(0.4f, 0.4f, 0.4f), Vec3d(0,0,0), 0, 1.5f },
        { Mat2::Diffuse, Vec3d(0.8f, 0.5f, 0.5f), Vec3d(45,20,20), 0, 0 },
        { Mat2::Diffuse, Vec3d(0.5f, 0.5f, 0.8f), Vec3d(20,20,45), 0, 0 }
    };

    std::vector<int> include = {1, 3, 5, 6, 7, 8};

    Scene scene{{.2, .2, .2}};
    int num_spheres = sphere_posns.size();
    for (int i = 0; i < num_spheres; ++i) {
        if(!std::count(include.begin(), include.end(), i)) continue;
        if(i != 6) continue;
        scene.add_object(new Sphere{sphere_posns[i], sphere_r[i], mats[i]});
    }

    // Mat2 floor_mat = { Mat2::Diffuse, Vec3d(1.f, 1.0f, 1.0f), Vec3d(0,0,0), 0, 0 };
    // scene.add_object(new Plane({ 0.0,      1, 0.0}, {0, -0.5, 0}, floor_mat, 100)); 

    // scene.add_object(new Mesh("../assets/monkey_low.obj", mats[0]));  

    // Vec3d big_sphere_posn = Vec3d(0,-100.5,-1);
    // double big_sphere_radius = 100;
    // Mat2 big_sphere_mat = { Mat2::Diffuse, Vec3d(0.8f, 0.8f, 0.8f), Vec3d(0,0,0), 0, 0 };
    // scene.add_object(new Sphere{big_sphere_posn, big_sphere_radius, big_sphere_mat});

    scene.set_HDRI("../assets/hdrs/sunny.hdr");

    return scene;
}

void zoom(double factor, Vec3d &from, const Vec3d &to) {
    Vec3d dir = from - to;
    from = to + dir * factor;
}

Scene HDRI_test_scene() {
    Scene scene{0};
    scene.set_HDRI("../assets/hdrs/night.hdr");
    scene.set_env_rotation(-0.1);

    // Vec3d sun_pos(0, 10, 0);
    // Mat2 sun_mat = { Mat2::Diffuse, Vec3d(0.8f, 0.8f, 0.8f), 1, 0, 0 };
    // scene.add_object(new Sphere{sun_pos, 1, sun_mat});

    std::vector<Mat2> sphere_mats = {
        { Mat2::Metal, 1, 0, 0, 0 },
        { Mat2::Diffuse, Vec3d(0.8f, 0.8f, 0.8f), 0, 0, 0 },
        { Mat2::Metal, {0.86f, 0.66f, 0.26f}, 0, 0.1, 0 },
        { Mat2::Dielectric, Vec3d(0.8f, 0.f, 0.8f), 0, 0, 1.5 }
    };

    double theta = 0 / 180.0 * M_PI;
    double sphere_r = 0.5;
    double spacing = 2 * sphere_r + 0.2;
    for (int i = 0; i < 4; ++i) {
        if(i != 0 && i != 3) continue;
        double off = (i - 1.5) * spacing;
        scene.add_object(new Sphere{{off*cos(theta), 0, off*sin(theta)}, sphere_r, sphere_mats[i]});
    }

    // table
    Mat2 table_mat = {Mat2::Metal, 0.8, 0, 0.12, 0};
    scene.add_object(new Plane{{0, 1, 0}, {0, -sphere_r-0.05, 0}, table_mat, 3});

    scene.add_object(new Mesh{"../assets/meshes/monkey_low.obj", sphere_mats[2]});

    return scene;
}


void render_turntable(Scene &s, Camera &cam, const std::string &name, const Vec3d &center, double h_off, double rot_r, int num_angles) {
    std::vector<Color> pixels;
    for(int i = 0; i < num_angles; ++i) {
        double pct = double(i) / num_angles;
        double theta = 2 * M_PI * pct;
        Vec3d from = {rot_r * sin(theta), h_off, rot_r * cos(theta)};
        cam.move_from_to(from, center);
        pixels = s.render(cam);

        std::string filename = "anim/ " + std::to_string(i) + name + ".bmp";
        drawbmp(filename.c_str(), cam.get_width(), cam.get_height(), pixels.data());
    }
}

void render_still(Scene &s, Camera &cam, const std::string &name) {
    std::vector<Color> pixels = s.render(cam);
    std::string filename = "stills/ " + name + ".bmp";
    drawbmp(filename.c_str(), cam.get_width(), cam.get_height(), pixels.data());
}

int main(){
    int width = 1280, height = 720;
    double factor = 1.5;
    width *= factor; height *= factor;

    double fov = 45;
    Camera cam{width, height, fov};
    
    Scene scene = HDRI_test_scene();
    Vec3d lookfrom(0,.6,3);
    Vec3d lookat(0,0.3,0);
    zoom(1, lookfrom, lookat);
    cam.move_from_to(lookfrom, lookat);

    auto start = std::chrono::high_resolution_clock::now();
    render_still(scene, cam, "path");
    // render_turntable(scene, cam, "path_anim", 0, 3, 3, 60);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start); 
    std::cout << "took: " << duration.count() / 1000.0 << std::endl;

    // Vec3d to = {0, 2, 0};
    // cam.move_from_to({0, 4, 6}, to);
    // Scene scene = mat1_test_scene();
    // render_turntable(scene, cam, "simple", to, 4, 6, 20);
    // render_still(scene, cam, "simple");
}