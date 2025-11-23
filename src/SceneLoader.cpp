#include "SceneLoader.h"
#include "Ground.h"
#include "SphereShape.h" 
#include "CentralSphere.h"
#include "graphics/GLMesh.h"
#include "graphics/GLGraphics3.h"
#include <fstream>
#include <iostream>
#include "fkYAML/node.hpp"

cg::vec3f readVec3(const fkyaml::node& node) {

    cg::vec3f v;

    if (node.is_sequence() && node.size() >= 3) {
        v.x = node[0].get_value<float>();
        v.y = node[1].get_value<float>();
        v.z = node[2].get_value<float>();
    }

    return v;
    
}

cg::Color readColor(const fkyaml::node& node) {

    cg::vec3f v = readVec3(node);

    return cg::Color{v.x, v.y, v.z};

}

void setActorType(Actor** actor, const std::string& type) {

    if (type == "Ground")
        *actor = new Ground();

    else if (type == "CentralSphere") 
        *actor = new CentralSphere();

}

void SceneLoader::load(const std::string& filename, SceneManager& manager) {
    
    std::ifstream file(filename);

    if (!file.is_open()) {

        std::cout << "Erro ao abrir arquivo: " << filename << std::endl;

        return;

    }

    try {
        
        fkyaml::node root = fkyaml::node::deserialize(file);

        std::string sceneName = "main";

        if (root.contains("name")) 
            sceneName = root["name"].get_value<std::string>();
        
        
        manager.createScene(sceneName);

        auto* scene = manager.getScene(sceneName);

        if (root.contains("camera")) {

            fkyaml::node& camNode = root["camera"];
            
            if (camNode.contains("position"))
                scene->camera.setPosition(readVec3(camNode["position"]));
            
            if (camNode.contains("fov"))  {

                float fov = camNode["fov"].get_value<float>();
                
                scene->camera.setViewAngle(fov);
            
            }

            scene->camera.setDirectionOfProjection(-scene->camera.position());

        }

        if (root.contains("lights") && root["lights"].is_sequence()) {

            for (auto& l : root["lights"]) {

                cg::Light light;

                cg::vec3f pos = readVec3(l["position"]);
                cg::Color color = readColor(l["color"]);

                light.setPosition(pos);
                light.color = color;

                scene->addLight(light);

            }

        }

        if (root.contains("actors") && root["actors"].is_sequence()) {

            for (auto& a : root["actors"]) {

                std::string type = a["type"].get_value<std::string>();

                Actor* actor = nullptr;

                setActorType(&actor, type);

                if (actor) {

                    if (a.contains("position")) 
                        actor->position = readVec3(a["position"]);

                    if (a.contains("scale")) 
                        actor->scale = readVec3(a["scale"]);
                    
                    if (a.contains("material")) {

                        auto& mat = a["material"];

                        if (mat.contains("diffuse")) 
                            actor->material.diffuse = readColor(mat["diffuse"]);

                        if (mat.contains("specular")) 
                            actor->material.specular = readColor(mat["specular"]);
                        
                        if (mat.contains("shine")) 
                            actor->material.shine = mat["shine"].get_value<float>();
                        
                    }
                    
                    scene->addActor(actor);
                }
            }
        }

    } catch (const fkyaml::exception& e) {

        std::cout << "erro ao parsear YAML: " << e.what() << std::endl;

    }

}