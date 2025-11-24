#pragma once

#include "Actor.h"
#include "PlaneShape.h" 

class Ground : public Actor {

public:

    void start() override {

        shape = std::make_unique<PlaneShape>();

        mesh = cg::glMesh(cg::GLGraphics3::box());

    }

};