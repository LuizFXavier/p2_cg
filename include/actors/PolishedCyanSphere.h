#pragma once

#include "Actor.h"
#include "SphereShape.h"

class PolishedCyanSphere : public Actor {
    
public:

    void start() override {

        tag = "Polished Cyan Sphere";

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());

    }

};
