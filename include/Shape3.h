#pragma once

#include "geometry/Ray.h"
#include "Intersection.h"

class Shape3 {

public:

    virtual ~Shape3() = default;

    virtual cg::Bounds3f bounds() const = 0;

    virtual Intersection intersect(cg::Ray3f& ray, cg::mat4f& transform) = 0;

};