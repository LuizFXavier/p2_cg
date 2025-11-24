#pragma once

#include "Actor.h"
#include "SphereShape.h"

class PolishedGreenSphere : public Actor {
    
public:

    void start() override {

        tag = "Polished Green Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
