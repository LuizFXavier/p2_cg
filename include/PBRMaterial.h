#pragma once

#include "graphics/Material.h"

class PBRMaterial : public cg::Material {

public:

    float metalness; 

    PBRMaterial(const cg::Color& color = cg::Color::white) 
        : cg::Material(color)
    {
        metalness = 0.0f;
    }

};