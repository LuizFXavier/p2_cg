#pragma once

#include "geometry/Intersection.h"
#include "graphics/GLGraphics3.h"
#include "math/Matrix4x4.h"
#include "Shape3.h"
#include "graphics/Material.h"
#include "geometry/Ray.h"
#include "graphics/GLMesh.h"
#include "PBRMaterial.h"
#include <memory>

class Actor : public cg::SharedObject {

public:

    std::string tag{"Game Object"};

    cg::vec3f position{0, 0, 0};
    cg::quatf rotation = cg::quatf::identity();
    cg::vec3f scale{1, 1, 1};

    PBRMaterial material{cg::Color::white};
    
    cg::GLMesh* mesh = nullptr;

    virtual ~Actor() = default;

    virtual void start() {}

    virtual void update(float deltaTime) {
        updateTransform(); 
    }
    
    virtual void render(cg::GLGraphics3& g3) {}

    void setMesh(cg::GLMesh* m) {
        mesh = m;
    }

    cg::Bounds3f bounds() const {

        if (!shape) {

            puts("não tem shape\n");

            return {};

        }

        auto b = shape->bounds();

        b.transform(transform);

        return b;

    }

    bool intersect(const cg::Ray3f& ray, cg::Intersection& hit) const {

        if (!shape) 
            return false;
        
        if (shape->intersect(ray, transform, hit)) {

            hit.object = const_cast<Actor*>(this); 

            return true;

        }

        return false;

    }
    
    bool intersect(const cg::Ray3f& ray) const {

        cg::Intersection dummy;

        dummy.distance = cg::math::Limits<float>::inf();

        return intersect(ray, dummy);

    }

    inline cg::vec3f getNormal(const cg::vec3f& globalPoint) const {

        if (!shape) 
            return {0,1,0};

        cg::mat4f invMatrix;

        if (!transform.inverse(invMatrix)) 
            return {0,1,0};

        cg::vec3f localPoint = invMatrix.transform(globalPoint);

        cg::vec3f localNormal = shape->normal(localPoint);

        cg::mat3f normalMatrix = cg::mat3f(invMatrix).transposed();

        return (normalMatrix * localNormal).normalize();

    }
    
    const cg::mat4f& getTransform() const {
        return transform;
    }

protected:

    std::unique_ptr<Shape3> shape;
    cg::mat4f transform;

    void updateTransform() {
        transform = cg::mat4f::TRS(position, rotation, scale);
    }
    
    friend class SceneManager;
    
};