#pragma once 

#include "Actor.h"
#include "graphics/Application.h" 
#include "SphereShape.h"

class CentralSphere : public Actor {

public:

    void start() override {

        shape = std::make_unique<SphereShape>();

        mesh = cg::glMesh(cg::GLGraphics3::sphere());
        
    }

};