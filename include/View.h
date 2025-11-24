#pragma once

#include "imgui.h"
#include "SceneManager.h"
#include <cmath>
#include <algorithm>

class View {

    private:

        SceneManager* manager;

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

            if (ImGui::CollapsingHeader("Troca de cenas")) {

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

                    static cg::vec3f uiColor;
                    static float uiIntensity;
                    static bool initialized = false;

                    if (!initialized || selectionChanged) {

                        cg::vec3f rawColor = {light.color.r, light.color.g, light.color.b};

                        uiIntensity = std::max({rawColor.x, rawColor.y, rawColor.z});
                        
                        if (uiIntensity > 0.001f) 
                            uiColor = rawColor * (1.0f / uiIntensity);

                        else {
                            uiColor = {1.0f, 1.0f, 1.0f}; 
                            uiIntensity = 0.0f;
                        }

                        initialized = true;

                    }
                    
                    
                    cg::vec3f lPos = light.position();

                    if (ImGui::DragFloat3("Posição##L", (float*)&lPos, 0.1f)) 
                        light.setPosition(lPos);
                    

                    bool colorChanged = ImGui::ColorEdit3("Cor##L", (float*)&uiColor);
                    
                    bool intensityChanged = ImGui::DragFloat("Intensidade", &uiIntensity, 1.0f, 0.0f, 10000.0f);

                    if (colorChanged || intensityChanged) 
                        light.color = cg::Color{
                            uiColor.x * uiIntensity,
                            uiColor.y * uiIntensity,
                            uiColor.z * uiIntensity
                        };
                    
                } else 
                    ImGui::Text("Nenhuma luz na cena.");
                
            }
        }


        void materialControl() {

            if (ImGui::CollapsingHeader("Materiais", ImGuiTreeNodeFlags_DefaultOpen)) {

                static int selectedActor = 0;

                auto* scene = manager->getActiveScene();
                
                auto& actors = scene->getActors(); 
                
                int numActors = (int)actors.size();

                if (numActors > 0) {
                    
                    std::string label = actors[selectedActor]->tag;

                    if (ImGui::BeginCombo("Selecionar Ator", label.c_str())) {

                        for (int i = 0; i < numActors; i++) {

                            bool isSelected = (selectedActor == i);

                            std::string itemLabel = actors[i]->tag;
                            
                            if (ImGui::Selectable(itemLabel.c_str(), isSelected)) 
                                selectedActor = i;
                            
                            if (isSelected) 
                                ImGui::SetItemDefaultFocus();

                        }

                        ImGui::EndCombo();

                    }

                    if (selectedActor >= 0 && selectedActor < numActors) {

                        auto& actor = actors[selectedActor];

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

};