#pragma once

#include "Actor.h"
#include "SphereShape.h"

class PolishedBlueSphere : public Actor {
    
public:

    void start() override {

        tag = "Polished Blue Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
