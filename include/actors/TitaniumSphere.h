#pragma once

#include "Actor.h"
#include "SphereShape.h"

class TitaniumSphere : public Actor {
    
public:

    void start() override {

        tag = "Titanium Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
