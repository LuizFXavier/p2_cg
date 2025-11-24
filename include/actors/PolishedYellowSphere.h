#pragma once

#include "Actor.h"
#include "SphereShape.h"

class PolishedYellowSphere : public Actor {
    
public:

    void start() override {

        tag = "Polished Yellow Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
