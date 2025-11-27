#pragma once

#include "Shape3.h"

class SphereShape : public Shape3 {

public:

    Intersection intersect(cg::Ray3f& ray, cg::mat4f& transform) override {

        cg::mat4f invMatrix;

        if (!transform.inverse(invMatrix))
            return {};
        
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
            return {}; 

        float t0 = (-b - sqrt(delta)) / (2 * a);
        float t1 = (-b + sqrt(delta)) / (2 * a);
        float t = -1.0f;

        if (t0 > 0.001f) 
            t = t0;

        else if (t1 > 0.001f) 
            t = t1;

        else 
            return {}; 

        // caso houver intersecao

        Intersection intersection;
        
        float len = invMatrix.transformVector(ray.direction).length();

        intersection.distance = t / len;

        intersection.point = ray.origin + ray.direction * intersection.distance;
        
        cg::vec3f localPoint = localRay.origin + localRay.direction * t;
        cg::vec3f localNormal = localPoint.normalize();
        
        intersection.normal = invMatrix.transposed().transformVector(localNormal).normalize();

        return intersection;

    }

    cg::Bounds3f bounds() const override {
        return cg::Bounds3f{{-1, -1, -1}, {1, 1, 1}};
    }

};