#pragma once

#include "Shape3.h"
#include "geometry/Bounds3.h"

class BoxShape : public Shape3 {
    
    private:
    
        cg::Bounds3f _bounds;

    public:

        BoxShape() {
            _bounds.set({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
        }

        Intersection intersect(cg::Ray3f& ray, cg::mat4f& transform) override {

            cg::mat4f invMatrix;

            if (!transform.inverse(invMatrix))
                return {};

            // transformar raio para local
            cg::vec3f localOrigin = invMatrix.transform(ray.origin);
            cg::vec3f localDirection = invMatrix.transformVector(ray.direction).normalize();

            cg::Ray3f localRay{ localOrigin, localDirection };

            float tMin, tMax;

            // não intercepta a caixa
            if (!_bounds.intersect(localRay, tMin, tMax)) 
                return {}; 

            // validação se a interseção está à frente do raio
            float t = -1.0f;

            if (tMin > 0.001f) 
                t = tMin;    

            else if (tMax > 0.001f) 
                t = tMax;  

            else 
                return {};                    

            // calcular ponto e normal no espaço global
            Intersection intersection;
            
            cg::vec3f localPoint = localRay.origin + localRay.direction * t;

            intersection.point = transform.transform(localPoint);

            intersection.distance = (intersection.point - ray.origin).length();

            cg::vec3f localNormal = {0, 0, 0};

            // margem de erro
            float epsilon = 1.5e-4f;  

            // verifica proximidade com as faces
            if (std::abs(localPoint.x - _bounds.min().x) < epsilon) 
                localNormal = {-1, 0, 0};

            else if (std::abs(localPoint.x - _bounds.max().x) < epsilon) 
                localNormal = {1, 0, 0};

            else if (std::abs(localPoint.y - _bounds.min().y) < epsilon) 
                localNormal = {0, -1, 0};

            else if (std::abs(localPoint.y - _bounds.max().y) < epsilon) 
                localNormal = {0, 1, 0};

            else if (std::abs(localPoint.z - _bounds.min().z) < epsilon) 
                localNormal = {0, 0, -1};

            else if (std::abs(localPoint.z - _bounds.max().z) < epsilon) 
                localNormal = {0, 0, 1};

            // transforma normal para global 
            intersection.normal = invMatrix.transposed().transformVector(localNormal).normalize();

            return intersection;

        }

        cg::Bounds3f bounds() const override { 
            return this->_bounds; 
        }
        
        cg::Bounds3f getLocalBounds() const { 
            return _bounds; 
        }

};