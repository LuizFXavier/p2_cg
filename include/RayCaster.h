#pragma once

#include "Scene.h"
#include "graphics/Camera.h"
#include "graphics/GLImage.h"
#include "graphics/Image.h"
#include "Intersection.h"
#include <memory>

class Raycaster {

public:

    cg::Camera camera;
    cg::Color backgroundColor{ cg::Color::royalBlue };

    Raycaster(int width, int height);

    void render(Scene& scene);

    cg::GLImage* getImage() const {
        return image.get();
    }

private:

    std::unique_ptr<cg::GLImage> image;
    cg::ImageBuffer imageBuffer; 

    cg::Ray3f generateRay(const cg::Camera& camera, int x, int y);
    
    cg::Color shade(Scene& scene, const cg::Ray3f& ray, const Intersection& hit);

};
