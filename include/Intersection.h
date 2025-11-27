#pragma once

#include "geometry/Intersection.h" 
#include "math/Vector3.h"

struct Intersection : public cg::Intersection {

    cg::vec3f normal;
    cg::vec3f point;
    
    Intersection() {
        distance = cg::math::Limits<float>::inf(); 
        object = nullptr;
    }
    
};