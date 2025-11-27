#pragma once

#include "Actor.h"
#include "BoxShape.h" 

class FederalSavingsBank : public Actor {

    public:

        void start() override {

            shape = std::make_unique<BoxShape>();

            mesh = cg::glMesh(cg::GLGraphics3::box());

            updateTransform();

        }

        void update(float deltaTime) override {

            float speed = 0.1f; 
    
            float angle = speed * deltaTime;

            rotation = rotation * cg::quatf{ angle, {1, 1, 1} };

            updateTransform();
            
        }

};