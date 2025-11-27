#pragma once

#include "Shape3.h"
#include <limits>

class PlaneShape : public Shape3 {

public:

    bool intersect(const cg::Ray3f& ray, const cg::mat4f& transform, cg::Intersection& hit) override {

        Intersection& _hit = static_cast<Intersection&>(hit);

        cg::mat4f invMatrix;

        if (!transform.inverse(invMatrix))
            return false;

        // aqui vai de local pra global

        cg::vec3f localOrigin = invMatrix.transform(ray.origin);

        cg::vec3f localDirection = ((cg::mat3f)invMatrix).transform(ray.direction).normalize();

        cg::Ray3f localRay{ localOrigin, localDirection };

        // calculo da intersecao

        cg::vec3f planeNormal{0, 1, 0};

        float denominator = localDirection.dot(planeNormal);

        if (std::abs(denominator) < 0.0001f) 
            return false;
        

        float t = (planeNormal.dot(-localOrigin)) / denominator;

        if (t <= 0.001f) 
            return false;

        cg::vec3f localPoint = localOrigin + localDirection * t;

        if (std::abs(localPoint.x) > 1.0f || std::abs(localPoint.z) > 1.0f)
            return false; 

        // caso houver intersecao
        
        cg::vec3f point = transform.transform(localPoint);
        
        float distance = (point - ray.origin).length();

        cg::vec3f normal = invMatrix.transposed().transformVector(planeNormal).normalize();

        _hit.distance = distance;
        _hit.point = point;
        _hit.normal = normal;
        
        return true;
        
    }

    cg::Bounds3f bounds() const override {
        return cg::Bounds3f{{-1, -1, -1}, {1, 1, 1}};
    }

};