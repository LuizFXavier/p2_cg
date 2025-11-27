#pragma once

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

        if (!shape)
            return {};

        auto b = shape->bounds();

        b.transform(transform);

        return b;

    }

    bool intersect(const cg::Ray3f& ray, Intersection& _hit) const {

        if (!shape) return 
            false;
        
        if (shape->intersect(ray, transform, _hit)) {

            _hit.object = const_cast<Actor*>(this); 

            return true;

        }

        return false;

    }
    
    // Sobrecarga simples para compatibilidade com código antigo (se precisar)
    bool intersect(const cg::Ray3f& ray) const {
        Intersection dummy;
        return intersect(ray, dummy);
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