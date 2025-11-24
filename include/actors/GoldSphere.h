#pragma once

#include "Actor.h"
#include "SphereShape.h"

class GoldSphere : public Actor {
    
public:

    void start() override {

        shape = std::make_unique<SphereShape>();

        position.set(2, 1, 10); 

        scale.set(1.2, 1.2, 1.2);

        material.diffuse = cg::Color{ 0.7f, 0.5f, 0.1f }; 
        material.specular = cg::Color{ 1.0f, 0.8f, 0.4f };
        material.shine = 512.0f;

        updateTransform();

    }

};
