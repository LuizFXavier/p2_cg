#pragma once

#include "Scene.h"
#include "graphics/Camera.h"
#include "graphics/GLImage.h"
#include "graphics/Image.h"
#include "Intersection.h"
#include <memory>
#include <iostream>
#include <array>

struct Viewport{
    float w;
    float h;
    float startX;
    float startY;
    float dx;
    float dy;
};

class Raycaster {

public:

    Raycaster(int width, int height);

    void render(Scene& scene, bool update = false);

    void update();

    cg::GLImage* getImage() const {
        return image.get();
    }

private:

    std::unique_ptr<cg::GLImage> image;
    std::array<cg::ImageBuffer, 2> imageBuffer;

    bool _swap = false;

    cg::Camera* _camera = nullptr;
    Scene* _scene = nullptr;

    Viewport _viewport;

    struct{
        cg::vec3f u;
        cg::vec3f v;
        cg::vec3f n;
    } _vrc;

    cg::Ray3f _pixelRay;

    void setViewport(float height);

    void setCameraConfigs();

    cg::Color shoot(float x, float y);

    void setPixelRay(float x, float y);

    cg::Ray3f generateRay(const cg::Camera& camera, int x, int y);

    cg::vec3f pixelPosition(float x, float y);
    
    cg::Color shade(const cg::Ray3f& ray, const Intersection& hit);

    bool intersect(cg::Ray3f&, Intersection&); // Verifica se houve intersecção do raio com a cena.

    bool shadow(cg::Ray3f& ray);

    cg::ImageBuffer* activeBuffer(){
        return _swap ? imageBuffer.begin() : imageBuffer.begin() + 1;
    }

    cg::Color trace(cg::Ray3f& ray);

    cg::Color background(){return _scene->backgroundColor;}
};

inline void 
Raycaster::setPixelRay(float x, float y){

    auto p = pixelPosition(x, y);

    int centerX = activeBuffer()->width() / 2;
    int centerY = activeBuffer()->height() / 2;
    

    switch (_camera->projectionType()){
        case cg::Camera::Perspective:
            _pixelRay.direction = (p - _camera->nearPlane() * _vrc.n).versor();
        break;

        case cg::Camera::Parallel:
            _pixelRay.origin = _camera->position() + p;
        break;
    }


}

inline cg::Color
Raycaster::trace(cg::Ray3f& ray){
    Intersection hit;

    return intersect(ray, hit) ? shade(ray, hit) : background();
}

inline bool
Raycaster::intersect(cg::Ray3f& ray, Intersection& hit){
    
    bool found = false;

    hit = _scene->intersect(ray);
    
    return hit.operator bool();
}

inline cg::Color
Raycaster::shoot(float x, float y){
    setPixelRay(x, y);

    cg::Color color = trace(_pixelRay);
    
    // adjust RGB color
    if (color.r > 1.0f)
        color.r = 1.0f;
    if (color.g > 1.0f)
        color.g = 1.0f;
    if (color.b > 1.0f)
        color.b = 1.0f;

    if (!cg::math::isPositive(color.r))
        color.r = 0.f;
    if (!cg::math::isPositive(color.g))
        color.g = 0.f;
    if (!cg::math::isPositive(color.b))
        color.b = 0.f;
    
    return color;
}

inline bool
Raycaster::shadow(cg::Ray3f& ray){
    
    return bool(_scene->intersect(ray));
}

inline void
Raycaster::setCameraConfigs(){

    int w = activeBuffer()->width();
    int h = activeBuffer()->height();

    _camera->setAspectRatio((float)w / h);
    _camera->update();

    const auto& m = _camera->cameraToWorldMatrix();
    
    // Inicialização das coordenadas do vrc para evitar recálculo no traçado de raios
    _vrc.u = m[0];
    _vrc.v = m[1];
    _vrc.n = m[2];

    _vrc.u.print("U: ");
    _vrc.v.print("V: ");
    _vrc.n.print("N: ");

    float fovRadians = cg::math::toRadians(_camera->viewAngle());
    float viewHeight = 2.0f * _camera->nearPlane() * std::tan(fovRadians * 0.5f);

    std::cout << "fovRadians:" << fovRadians << "\n";
    std::cout << "nearPlane:" << _camera->nearPlane() << "\n";
    std::cout << "tan:" << std::tan(fovRadians * 0.5f) << "\n";
    std::cout << "viewHeight:" << viewHeight << "\n";

    _pixelRay.set(_camera->position(), -_vrc.n);
    setViewport(viewHeight);

    std::cout << "Câmera setada\n";
}

inline void
Raycaster::setViewport(float height){
    float iH = activeBuffer()->height(), iW = activeBuffer()->width();
        
    _viewport.h = height;

    float imageRatio = iW / iH;

    _viewport.w = _viewport.h * imageRatio;

    _viewport.dx = _viewport.w / iW;
    _viewport.dy = _viewport.h / iH; // dx == dy sempre

    _viewport.startX = -_viewport.w * 0.5f;
    _viewport.startY =  _viewport.h * 0.5f;

    std::cout << "Viewport:\n";
    std::cout << "w:" << _viewport.w << "\n";
    std::cout << "h:" << _viewport.h << "\n";
    std::cout << "sX:" << _viewport.startX << "\n";
    std::cout << "sY:" << _viewport.startY << "\n";
    std::cout << "dx:" << _viewport.dx << "\n";
    std::cout << "dy:" << _viewport.dy << "\n";
}