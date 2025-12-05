#pragma once

#include "geometry/Intersection.h"

#include "Actor.h"
#include "LightPBR.h"
#include "graphics/Color.h"
#include "geometry/BVH.h"
#include <string>
#include <vector>
#include <memory>

class Scene {

public:

    cg::Camera camera;

    cg::Color backgroundColor;

    std::string name;
    std::vector<LightPBR> lights;
    cg::Color ambientLight{ cg::Color::darkGray }; 

    cg::Reference<cg::BVH<Actor>> bvh;

    const std::vector<cg::Reference<Actor>>& getActors() const {
        return actors;
    }

private:

    std::vector<cg::Reference<Actor>> actors;

    void addActors() {}

    template<typename T, typename... Rest>
    void addActors(T* first, Rest*... rest) {

        actors.push_back(first);

        addActors(rest...);

    }

public:

    template<typename... Actor>
    Scene(const std::string& name, Actor*... actorList) : name(name), backgroundColor(cg::Color{0.5f, 0.8f, 0.92f}) {

        addActors(actorList...);
        
    }

    void start() {

        for (auto& actor : actors) 
            actor->start();
        
    }

    void update(float deltaTime) {

        for (auto& actor : actors) 
            actor->update(deltaTime);
        
    }

    void render(cg::GLGraphics3& g3) {

        for (auto& actor : actors) 
            actor->render(g3);
        
    }

    void addLight(LightPBR light) {

        lights.push_back(light);

    }

    void addActor(Actor* actor) {

        actors.push_back(actor);

    }

    LightPBR* getLight(size_t index) {
        return &lights[index];
    }

    void buildBVH() {
        
        std::vector<cg::Reference<Actor>> list = actors;

        std::cout << "--- Debugging BVH Inputs ---" << std::endl;
        for (size_t i = 0; i < list.size(); ++i) {
            auto b = list[i]->bounds();
            auto c = b.center();
            // Imprime apenas se for muito suspeito (na origem) ou sempre
            std::cout << "Actor " << i;
            c.print(" Center: "); 
        }
        printf("[buildBVH] actors: %d\n", list.size());

        bvh = new cg::BVH<Actor>(std::move(list), 2, cg::BVHBase::SplitMethod::Median);

        printf("[buildBVH] bvh size: %d\n", bvh->size());        
        
    }

    bool intersect(cg::Ray3f& ray, cg::Intersection& hit) {

        if (bvh) 
            return bvh->intersect(ray, hit); 

        else {

            bool isHit = false;

            for (auto& actor : actors) 
                if (actor->intersect(ray, hit)) 
                    isHit = true;
            
            return isHit;

        }

    }

};