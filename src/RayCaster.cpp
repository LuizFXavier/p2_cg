#include "RayCaster.h"
#include "math/Vector3.h"

Raycaster::Raycaster(int width, int height){

    image = std::make_unique<cg::GLImage>(width, height);
    imageBuffer[0] = cg::ImageBuffer(width, height);
    imageBuffer[1] = cg::ImageBuffer(width, height);
}

cg::Ray3f Raycaster::generateRay(const cg::Camera& camera, int x, int y) {
    
    float fov = camera.viewAngle(); 
    float aspect = camera.aspectRatio();
    float d = 1.0f;

    float halfHeight = d * tan(cg::math::toRadians(fov) * 0.5f);
    float halfWidth = halfHeight * aspect;

    float cx = (2.0f * x / image->width() - 1.0f) * halfWidth;
    float cy = (1.0f - 2.0f * y / image->height()) * halfHeight;

    cg::vec3f direction_cam = cg::vec3f{cx, cy, -d}.normalize();

    const auto& camToWorld = camera.cameraToWorldMatrix();
    cg::vec3f directionWorld = camToWorld.transformVector(direction_cam);

    return cg::Ray{ camera.position(), directionWorld };

}

cg::vec3f 
Raycaster::pixelPosition(float x, float y){

    float startX = _viewport.startX, startY = _viewport.startY;
    float dx = _viewport.dx, dy = _viewport.dy;
    
    return (startX + (x * dx)) * _vrc.u + (-startY + (y * dy)) * _vrc.v;

    return cg::vec3f();
}


// Epslon necessário para evitar colisão consigo mesmo
inline constexpr auto
rt_eps(){
  return 1.5e-4f;
}

cg::Color 
Raycaster::shade(const cg::Ray3f& ray, const Intersection& hit) {

    const auto& material = hit.actor->material;

    cg::Color finalColor = material.ambient * _scene->ambientLight;

    auto P = hit.point;
    auto V = ray.direction;

    cg::vec3f N = hit.normal;
    
    cg::Color Od = material.diffuse;
    cg::Color Os = material.specular;

    float Ns = material.shine;

    for (const auto& light : _scene->lights) {

        cg::vec3f L;
        float d;
        
        if (!light.lightVector(P, L, d))
            continue;
        
        L = -L;
        float NL = N.dot(L);
        
        if(NL > 0)
            continue;

        cg::Ray lightRay{ hit.point - L * rt_eps(), light.position() - P};

        if(shadow(lightRay))
            continue;

        if(cg::math::isZero(Od.r)){
            Od.r = Os.r * 0.8f;
        }
        if(cg::math::isZero(Od.g)){
            Od.g = Os.g * 0.8f;
        }
        if(cg::math::isZero(Od.b)){
            Od.b = Os.b * 0.8f;
        }

        cg::vec3f R = L - (2 * (NL)) * N;

        float RV = R.dot(V);

        RV = std::min(0.0f, -RV);

        cg::Color Il = light.lightColor(d);

        finalColor += (Od * Il * (-NL)) + (Os * Il) * pow(-RV, Ns);

    }

    return finalColor;

}


void Raycaster::render(Scene& scene, bool update = false) {

    if((&scene == _scene) && !update)
        return;

    int w = activeBuffer()->width();
    int h = activeBuffer()->height();

    _scene = &scene;
    _camera = &(_scene->camera);

    setCameraConfigs();

    for (int j = 0; j < h; ++j){

        auto y = (float)j + 0.5f;

        for (int i = 0; i < w; ++i) {

            auto x = (float)i + 0.5f;
            
            cg::Color color = shoot(x, y);
            
            (*activeBuffer())(i, j) = color;
        }
    }
    
    image->setData(*(activeBuffer()));
    
}

void
Raycaster::update(){
    _swap = !_swap;

    render(*_scene, true);
}