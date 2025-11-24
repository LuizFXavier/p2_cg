#pragma once

#include "Actor.h"
#include "SphereShape.h"

class CopperSphere : public Actor {
    
public:

    void start() override {

        tag = "Copper Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
