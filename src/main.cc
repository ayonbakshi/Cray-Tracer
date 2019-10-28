#include "Object.h"
#include "Light.h"
#include "MathUtils.h"
#include "Raycaster.h"

int main(){
    Material r = {red};
    Material g = {green};
    Material b = {blue};
    Material w = {white};
    Material floor = {{69, 37, 80}, true};
    Material blue_floor = {{120, 80, 200}, true};

    Scene scene{background};

    // scene.add_object(new Plane({ 0.0,      1, 0.1}, floor, {0, -10, -30}, 100)); 
    scene.add_object(new Mesh("../assets/turned_head.obj", r)); 
    scene.add_object(new Mesh("../assets/mirror.obj", floor)); 
    // scene.add_object(new Sphere({ 5,      0, -30},     1, r)); 
    // scene.add_object(new Sphere({ 0.0,      5, -35},     4.0, g)); 
    // scene.add_object(new Sphere({ 5.0,     2, -15},     2.0, b))); 
    // scene.add_object(new Sphere({ 5.0,      0, -25},     3.0, r))); 
    // scene.add_object(new Sphere({-5.5,      0, -20},     0.5, g))); 
    
    scene.add_light(Light({10,     0, -30}, 300));

    
    int width = 1080, height = 720;
    scene.render(std::string("raycast.bmp"), scene, width, height);
}
