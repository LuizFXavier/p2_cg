#pragma once

#include "Shape3.h"
#include <limits>

class PlaneShape : public Shape3 {

private:

    cg::Bounds3f _bounds;

public:

    PlaneShape() {
        _bounds.set({-1000.0f, -0.001f, -1000.0f}, {1000.0f, 0.001f, 1000.0f});
    }

    bool intersect(const cg::Ray3f& ray, const cg::mat4f& transform, cg::Intersection& hit) override {

        cg::mat4f invMatrix;

        if (!transform.inverse(invMatrix)) 
            return false;

        // transforma raio para espaço local
        cg::vec3f localOrigin = invMatrix.transform(ray.origin);

        cg::vec3f localDirection = invMatrix.transformVector(ray.direction).normalize();
        
        if (std::abs(localDirection.y) < 1e-6f) 
            return false; 
        

        float t = -localOrigin.y / localDirection.y;

        if (t < 0.001f) 
            return false; 

        float scaleFactor = invMatrix.transformVector(ray.direction).length();
        float globalDist = t / scaleFactor;

        // Verifica se é o hit mais próximo
        if (globalDist < ray.tMax) {

            hit.distance = globalDist;

            hit.p = ray.origin + ray.direction * globalDist;
            
            return true;

        }

        return false;
        
    }

    inline cg::vec3f normal(const cg::vec3f& localPoint) const override {
        return cg::vec3f{0.0f, 1.0f, 0.0f};
    }

    cg::Bounds3f bounds() const override {
        return _bounds;
    }

};