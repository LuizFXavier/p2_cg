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

        bool intersect(const cg::Ray3f& ray, const cg::mat4f& transform, cg::Intersection& hit) override {

            cg::mat4f invMatrix;

            if (!transform.inverse(invMatrix))
                return false;

            // transformar raio para local
            cg::vec3f localOrigin = invMatrix.transform(ray.origin);
            cg::vec3f localDirection = invMatrix.transformVector(ray.direction).normalize();

            cg::Ray3f localRay{ localOrigin, localDirection };

            float tMin, tMax;

            // não intercepta a caixa
            if (!_bounds.intersect(localRay, tMin, tMax)) 
                return false; 

            // decidindo valor de t
            float t = (tMin > 0.001f) ? tMin : tMax;

            if (t > 0.001f) {

                float scaleFactor = invMatrix.transformVector(ray.direction).length();
                float globalDist = t / scaleFactor;

                if (globalDist < ray.tMax) {

                    hit.distance = globalDist;

                    hit.p = ray.origin + ray.direction * hit.distance;

                    return true;

                }

            }

            return false;

        }

        cg::Bounds3f bounds() const override { 
            return this->_bounds; 
        }

        inline cg::vec3f normal(const cg::vec3f& localPoint) const override {
            
            // margem de erro
            float epsilon = 1.5e-4f;  

            // verifica proximidade com as faces
            if (std::abs(localPoint.x - _bounds.min().x) < epsilon) 
                return {-1, 0, 0};

            else if (std::abs(localPoint.x - _bounds.max().x) < epsilon) 
                return {1, 0, 0};

            else if (std::abs(localPoint.y - _bounds.min().y) < epsilon) 
                return {0, -1, 0};

            else if (std::abs(localPoint.y - _bounds.max().y) < epsilon) 
                return {0, 1, 0};

            else if (std::abs(localPoint.z - _bounds.min().z) < epsilon) 
                return {0, 0, -1};

            else if (std::abs(localPoint.z - _bounds.max().z) < epsilon) 
                return {0, 0, 1};

            return {0, 1, 0};

        }
        
        cg::Bounds3f getLocalBounds() const { 
            return _bounds; 
        }

};