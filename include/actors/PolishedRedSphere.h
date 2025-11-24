#pragma once

#include "Actor.h"
#include "SphereShape.h"

class PolishedRedSphere : public Actor {
    
public:

    void start() override {

        tag = "Polished Red Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
