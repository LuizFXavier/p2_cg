#pragma once

#include "geometry/Ray.h"
#include "geometry/Intersection.h"
#include "geometry/Bounds3.h"

class Shape3 {

    public:

        virtual ~Shape3() = default;

        virtual cg::Bounds3f bounds() const = 0;

        virtual bool intersect(const cg::Ray3f& ray, const cg::mat4f& transform, cg::Intersection& hit) = 0;

        inline virtual cg::vec3f normal(const cg::vec3f& localPoint) const = 0;

};