#pragma once

#include "imgui.h"
#include "SceneManager.h"
#include "RenderMode.h"
#include <cmath>
#include <algorithm>

class View {

    private:

        SceneManager* manager;

        int selectedActorIndex = 0;

        // callback que precisa pra funcionar o seletor
        static bool vectocGetter(void* vec, int i, const char** outText) {

            auto& vector = *static_cast<std::vector<std::string>*>(vec);

            if (i < 0 || i >= static_cast<int>(vector.size())) 
                return false;

            *outText = vector[i].c_str();

            return true;

        }


    public:

        View(SceneManager* manager): manager{manager} {}

        void setSelectedActor(Actor* actor) {

            auto* scene = manager->getActiveScene();

            if (!scene) 
                return;

            const auto& actors = scene->getActors();

            for (int i = 0; i < actors.size(); ++i) 
                if (actors[i]->tag == actor->tag) {

                    selectedActorIndex = i;

                    return;

                }

            
        }

        void sceneControl() {

            static int currentScene = 0;

            auto sceneNames = manager->getSceneNames();
            
            if (auto* scene = manager->getActiveScene()) {

                for (int i = 0; i < sceneNames.size(); i++)
                    if (sceneNames[i] == scene->name) {

                        currentScene = i;

                        break;

                    } 

            }

            if (ImGui::CollapsingHeader("Troca de cenas", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (ImGui::Combo("cenas", &currentScene, vectocGetter, static_cast<void*>(&sceneNames), sceneNames.size())) {

                    std::string newSceneName = sceneNames[currentScene];

                    manager->setActiveScene(newSceneName);

                }

            }

        }

        void cameraControl() {

            if (ImGui::CollapsingHeader("Câmera", ImGuiTreeNodeFlags_DefaultOpen)) {

                auto* scene = manager->getActiveScene();

                auto& camera = scene->camera;

                cg::vec3f pos = camera.position();
            
                if (ImGui::DragFloat3("Posição", (float*)&pos, 0.1f)) 
                    camera.setPosition(pos);

                float fov = camera.viewAngle();

                if (ImGui::SliderFloat("FOV", &fov, 10.0f, 120.0f)) {
                    camera.setViewAngle(fov);

             }
            
                camera.update(); 

            }

        }
    
        void lightControl() {
            

            if (ImGui::CollapsingHeader("Luzes")) {

                auto* scene = manager->getActiveScene();

                static int selectedLight = 0;

                int numLights = (int)scene->lights.size();
                
                if (numLights > 0) {

                    if (selectedLight >= numLights) 
                        selectedLight = 0;

                    bool selectionChanged = ImGui::SliderInt("Selecionar Luz", &selectedLight, 0, numLights - 1);
                    
                    auto& light = scene->lights[selectedLight];
                    
                    cg::vec3f lPos = light.position();
                    cg::vec3f baseRGB = {light.color.r, light.color.g, light.color.b};
                    float intensity = light.intensity;

                    if (ImGui::DragFloat3("Posição##L", (float*)&lPos, 0.1f)) 
                        light.setPosition(lPos);

                    if (ImGui::ColorEdit3("Cor Base", (float*)&baseRGB)) 
                        light.setBaseColor(cg::Color{baseRGB.x, baseRGB.y, baseRGB.z});
                    
                    if (ImGui::DragFloat("Intensidade ", &intensity, 1.0f, 0.0f, 1000.0f)) 
                        light.setIntensity(intensity);
                    
                } else 
                    ImGui::Text("Nenhuma luz na cena");
                
            }
        }


        void materialControl() {

            if (ImGui::CollapsingHeader("Materiais", ImGuiTreeNodeFlags_DefaultOpen)) {

                auto* scene = manager->getActiveScene();
                
                auto& actors = scene->getActors(); 
                
                int numActors = (int)actors.size();

                if (numActors > 0) {
                    
                    std::string label = actors[selectedActorIndex]->tag;

                    if (ImGui::BeginCombo("Selecionar Ator", label.c_str())) {

                        for (int i = 0; i < numActors; i++) {

                            bool isSelected = (selectedActorIndex == i);

                            std::string itemLabel = actors[i]->tag;
                            
                            if (ImGui::Selectable(itemLabel.c_str(), isSelected)) 
                                selectedActorIndex = i;
                            
                            if (isSelected) 
                                ImGui::SetItemDefaultFocus();

                        }

                        ImGui::EndCombo();

                    }

                    if (selectedActorIndex >= 0 && selectedActorIndex < numActors) {

                        auto& actor = actors[selectedActorIndex];

                        auto& mat = actor->material; 

                        ImGui::Separator();
                        ImGui::Text("Propriedades PBR");

                        cg::vec3f diff = {mat.diffuse.r, mat.diffuse.g, mat.diffuse.b};

                        if (ImGui::ColorEdit3("Difusa", (float*)&diff)) 
                            mat.diffuse = cg::Color{
                                diff.x, 
                                diff.y, 
                                diff.z
                            };

                        cg::vec3f spec = {mat.specular.r, mat.specular.g, mat.specular.b};

                        if (ImGui::ColorEdit3("Especular", (float*)&spec)) 
                            mat.specular = cg::Color{
                                spec.x, 
                                spec.y, 
                                spec.z
                            };
                        
                        float roughness = mat.shine; 

                        if (ImGui::SliderFloat("Rugosidade", &roughness, 0.05f, 1.0f)) 
                            mat.shine = roughness;
                        

                        float metal = mat.metalness;

                        if (ImGui::SliderFloat("Metálico", &metal, 0.0f, 1.0f)) 
                            mat.metalness = metal;
                        
                        
                    }

                }

            }

        }
     
        void renderOptions(RenderMode& currentMode) {

            if (ImGui::CollapsingHeader("Configurações de renderização", ImGuiTreeNodeFlags_DefaultOpen)) {
                
                if (ImGui::RadioButton("OpenGL", currentMode == RenderMode::OpenGL)) 
                    currentMode = RenderMode::OpenGL;
                
                if (ImGui::RadioButton("Ray Casting", currentMode == RenderMode::RayCasting))
                    currentMode = RenderMode::RayCasting;

            }

        }

};