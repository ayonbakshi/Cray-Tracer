#include "Object.h"
#include "MathUtils.h"
#include "Raycaster.h"

int main(){
    Material r = {red};
    Material g = {green};
    Material b = {blue};
    Material w = {white};
    Material floor = {{69, 37, 80}};
    Material blue_floor = {{120, 80, 200}};

    Scene scene{background};

    scene.add_object(new Plane({ 0.0,      1, 0.1}, floor, {0, -8, -30}, 100)); 
    scene.add_object(new Mesh("../assets/polysphere.obj", w)); 
    // scene.objects.push_back(e({ 0.0,      0, -20},     4.0, r))); 
    // scene.objects.push_back(e({ 0.0,      5, -35},     4.0, g))); 
    // scene.objects.push_back(e({ 5.0,     2, -15},     2.0, b))); 
    // scene.objects.push_back(e({ 5.0,      0, -25},     3.0, r))); 
    // scene.objects.push_back(e({-5.5,      0, -20},     0.5, g))); 
    
    scene.add_light({-10,     15, 5});

    
    int width = 1080, height = 720;
    scene.render(std::string("raycast.bmp"), scene, width, height);
}
