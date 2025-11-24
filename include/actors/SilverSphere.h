#pragma once

#include "Actor.h"
#include "SphereShape.h"

class SilverSphere : public Actor {
    
public:

    void start() override {

        tag = "Silver Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
