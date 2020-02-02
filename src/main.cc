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

int main(){
    Material test = test_mat(white);
    Material r = make_diffuse_mat(red);
    r.reflective = true;
    Material g = make_diffuse_mat(green);
    Material b = make_diffuse_mat(blue);
    Material w = make_diffuse_mat(white);
    Material floor = make_diffuse_mat({69/255.0, 37/255.0, 80/255.0});
    Material blue_floor = make_diffuse_mat({120/255.0, 80/255.0, 200/255.0});

    Scene scene{background};

    scene.add_object(new Plane({ 0.0,      1, 0.1}, g, {0, -7, -30}, 100)); 
    // scene.add_object(new Mesh("../assets/polysphere.obj", r));  
    // scene.add_object(new Mesh("/home/ayon/Downloads/dragon_large.obj", r)); 
    // scene.add_object(new Mesh("../assets/mirror.obj", floor)); 
    // scene.add_object(new Sphere({ 5,      0, -30},     1, g)); 
    // scene.add_object(new Sphere({ 0.0,      5, -35},     4.0, r)); 
    scene.add_object(new Sphere({ 5.0,     2, -15},     2.0, r)); 
    // scene.add_object(new Sphere({ 5.0,      0, -25},     3.0, r))); 
    // scene.add_object(new Sphere({-5.5,      0, -20},     0.5, g))); 
    
    scene.add_light(Light({-5,     5, 0}, 300));
    
    int width = 1080/2, height = 720/2;
    std::vector<Color> pixels = scene.render(width, height);
    drawbmp("raycast.bmp", width, height, pixels.data());
}
