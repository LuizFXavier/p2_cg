#pragma once

#include "Actor.h"
#include "LightPBR.h"
#include "graphics/Color.h"
#include <string>
#include <vector>
#include <memory>

class Scene {

public:

    cg::Camera camera;

    std::string name;
    std::vector<LightPBR> lights;
    cg::Color ambientLight{ cg::Color::darkGray }; 

    const std::vector<std::unique_ptr<Actor>>& getActors() const {
        return actors;
    }

private:

    std::vector<std::unique_ptr<Actor>> actors;

    void addActors() {}

    template<typename T, typename... Rest>
    void addActors(T* first, Rest*... rest) {

        actors.push_back(std::unique_ptr<Actor>(first));

        addActors(rest...);

    }

public:

    template<typename... Actor>
    Scene(const std::string& name, Actor*... actorList) : name(name) {

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

        actors.push_back(std::unique_ptr<Actor>(actor));

    }

    LightPBR* getLight(size_t index) {
        return &lights[index];
    }

    Intersection intersect(cg::Ray3f& ray) {

        Intersection closestHit;

        for (auto& actor : actors) {

            Intersection hit = actor->intersect(ray);

            if (hit && (!closestHit || hit.distance < closestHit.distance)) 
                closestHit = hit;
            
        }

        return closestHit;

    }

};