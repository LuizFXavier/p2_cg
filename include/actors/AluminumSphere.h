#pragma once

#include "Actor.h"
#include "SphereShape.h"

class AluminumSphere : public Actor {
    
public:

    void start() override {

        tag = "Aluminum Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
