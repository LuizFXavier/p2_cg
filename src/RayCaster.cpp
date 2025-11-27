#include <cmath>
#include <algorithm>
#include "RayCaster.h"
#include "math/Vector3.h"

#include "pbrFunctions.h"

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

    switch (lightModel){

        case Raycaster::LightModel::PHONG:
        default:
            return phongLightModel(ray, hit);
            break;

        case Raycaster::LightModel::PBR:
            return pbrLightModel(ray, hit);
        break;
    }

}

cg::Color 
Raycaster::phongLightModel(const cg::Ray3f& ray, const Intersection& hit){

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

cg::Color 
Raycaster::pbrLightModel(const cg::Ray3f& ray, const Intersection& hit){
    
    const auto& material = hit.actor->material;

    cg::Color Lo = cg::Color{0.f, 0.f, 0.f};

    auto P = hit.point;
    auto V = -ray.direction;

    cg::vec3f N = hit.normal;
    
    cg::Color Od = material.diffuse;
    cg::Color Os = material.specular;

    float r = material.shine;
    float m = material.metalness;

    Od = (1.f - m) * Od;

    cg::Color fd = Od * cg::math::inverse(cg::math::pi<float>);

    for(auto& light : _scene->lights){
        cg::vec3f L;
        float d;
        
        if (!light.lightVector(P, L, d))
            continue;

        float attenuation = cg::math::inverse(d * d);
        cg::Color Ll = light.finalColor * attenuation;

        float NL = cg::math::max(0.f, N.dot(L));
        
        if(!cg::math::isPositive(NL))
            continue;

        cg::Ray lightRay{ hit.point - L * rt_eps(), light.position() - P};

        if(shadow(lightRay))
            continue;

        float NV = cg::math::max(0.f, N.dot(V));

        cg::vec3f H = (L + V).versor();

        cg::Color F = fresnelSchlick(Os, L, H);

        float _G = G(N, L, V, r);
        float D = distNormMicrogeometry(N, H, r);

        float denom = (4.f * NL * NV); 
        cg::Color fs = (F * _G * D) * cg::math::inverse(cg::math::max(denom, 0.001f));

        cg::Color kS = F;
        cg::Color kD = cg::Color{1.f, 1.f, 1.f} - kS;

        // f(Ll, V) = fd + fs
        cg::Color f = (kD * fd) + fs;

        // π * somatorio(Ll * f(Ll, V) * (-N * Ll))
        Lo += (f * Ll * NL) * cg::math::pi<float>;

    }

    float invGamma = 1.0f / 2.2f;

    Lo.r = std::pow(Lo.r, invGamma);
    Lo.g = std::pow(Lo.g, invGamma);
    Lo.b = std::pow(Lo.b, invGamma);

    // Clamp para não estourar o branco
    Lo.r = cg::math::min(Lo.r, 1.0f);
    Lo.g = cg::math::min(Lo.g, 1.0f);
    Lo.b = cg::math::min(Lo.b, 1.0f);

    return Lo;
}

void Raycaster::render(Scene& scene, bool update) {

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