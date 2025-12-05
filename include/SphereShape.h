#pragma once

#include "Shape3.h"

class SphereShape : public Shape3 {

public:

    bool intersect(const cg::Ray3f& ray, const cg::mat4f& transform, cg::Intersection& hit) override {

        cg::mat4f invMatrix;

        if (!transform.inverse(invMatrix))
            return false;
        
        // aqui vai de local pra global
        
        cg::vec3f localOrigin = invMatrix.transform(ray.origin);

        cg::vec3f localDirection = invMatrix.transformVector(ray.direction).normalize();

        cg::Ray3f localRay{ localOrigin, localDirection };

        // calculo da intersecao

        auto L = localRay.origin;
        auto a = localRay.direction.dot(localRay.direction);
        auto b = 2 * localRay.direction.dot(L);
        auto c = L.dot(L) - 1.0f;

        float delta = (b * b) - (4 * a * c);

        if (delta < 0)
            return false; 

        float t0 = (-b - sqrt(delta)) / (2 * a);
        float t1 = (-b + sqrt(delta)) / (2 * a);
        float t = -1.0f;

        if (t0 > 0.001f) 
            t = t0;

        else if (t1 > 0.001f) 
            t = t1;

        else
            return false;

        float scaleFactor = invMatrix.transformVector(ray.direction).length();
        float globalDist = t / scaleFactor;

        // caso houver intersecao
        if (globalDist < ray.tMax) { 

            hit.distance = globalDist; 

            hit.p = ray.origin + ray.direction * hit.distance;
            
            return true;

        }

        return false;

    }

    cg::Bounds3f bounds() const override {
        return cg::Bounds3f{{-1, -1, -1}, {1, 1, 1}};
    }

    inline cg::vec3f normal(const cg::vec3f& localPoint) const override {
        return localPoint;
    }

};