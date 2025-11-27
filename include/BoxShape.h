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

            Intersection& _hit = static_cast<Intersection&>(hit);

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

                if (globalDist < hit.distance) {

                    _hit.distance = globalDist;
                    _hit.point = ray.origin + ray.direction * hit.distance;
                    
                    cg::vec3f localPoint = localRay.origin + localRay.direction * t;

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
                    _hit.normal = invMatrix.transposed().transformVector(localNormal).normalize();

                    return true;

                }

            }

            return false;

        }

        cg::Bounds3f bounds() const override { 
            return this->_bounds; 
        }
        
        cg::Bounds3f getLocalBounds() const { 
            return _bounds; 
        }

};