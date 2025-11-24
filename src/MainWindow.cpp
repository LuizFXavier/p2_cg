//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: MainWindow.cpp
// ========
// Source file for cg template window.
//
// Author: Paulo Pagliosa
// Last revision: 07/11/2022

#include "MainWindow.h"
#include "SceneLoader.h"
#include "math/Quaternion.h"
#include "math/Real.h"

// callback que precisa pra funcionar o seletor
static bool vectocGetter(void* vec, int i, const char** outText) {

  auto& vector = *static_cast<std::vector<std::string>*>(vec);

  if (i < 0 || i >= static_cast<int>(vector.size())) 
    return false;

  *outText = vector[i].c_str();

  return true;

}

/////////////////////////////////////////////////////////////////////
//
// MainWindow implementation
// ==========
MainWindow::MainWindow(int width, int height):
  Base{"P2 Wilker e José", width, height}
{

}

void
MainWindow::initialize()
{

  Base::initialize();

  raycaster = std::make_unique<Raycaster>(width(), height());

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);

  SceneLoader::load("../assets/scenes/central_sphere.yml", sceneManager);
  SceneLoader::load("../assets/scenes/tp1.yml", sceneManager);

  sceneManager.setActiveScene("central_sphere");

  pbrRenderer = std::make_unique<PBRRenderer>();

  pbrRenderer->initialize();

  currentRenderMode = RenderMode::OpenGL;

}

void
MainWindow::update()
{
  
  sceneManager.update(deltaTime());

  if (sceneManager.getActiveScene()->name == "central_sphere") {

    auto light = sceneManager.getActiveScene()->getLight(0);

    cg::vec3f p = light->position();

    cg::quatf rotation(5.0f, {0, 1, 0});

    p = rotation.rotate(p);

    light->setPosition(p);

  }

}

void
MainWindow::renderScene()
{
  
  glClearColor(0.5f, 0.8f, 0.92f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto* scene = sceneManager.getActiveScene();
  
  if (!scene) 
    return;

  if (height() > 0) {
    float ratio = (float)width() / (float)height();
    scene->camera.setAspectRatio(ratio);
    scene->camera.update(); 
  }

  if (currentRenderMode == RenderMode::OpenGL) {

    if (pbrRenderer) {

      pbrRenderer->setImageSize(width(), height());
      
      pbrRenderer->render(*scene);

    }

  } 
  else if (currentRenderMode == RenderMode::RayCasting) {

    if (raycaster) {
    
      raycaster->render(*scene);
      
      if (raycaster->getImage())
        raycaster->getImage()->draw(0, 0);
    
    }

  }

}

bool
MainWindow::keyInputEvent(int key, int action, int mods)
{
  return Base::keyInputEvent(key, action, mods);
}

void
MainWindow::gui()
{

  ImGui::Begin("controle de cena");

  static int currentScene = 0;

  auto sceneNames = sceneManager.getSceneNames();
  
  if (auto* scene = sceneManager.getActiveScene()) {

    for (int i = 0; i < sceneNames.size(); i++)
      if (sceneNames[i] == scene->name) {

        currentScene = i;

        break;

      } 

  }

  if (ImGui::Combo("trocar cena", &currentScene, vectocGetter, static_cast<void*>(&sceneNames), sceneNames.size())) {

    std::string newSceneName = sceneNames[currentScene];
    sceneManager.setActiveScene(newSceneName);
    
    auto* newScene = sceneManager.getActiveScene();

    if (newScene) {
      
      float ratio = (float)width() / (float)height();

      newScene->camera.setAspectRatio(ratio);
      newScene->camera.update();

      pbrRenderer = std::make_unique<PBRRenderer>();
      pbrRenderer->initialize();

    }

  }

  ImGui::Separator();

  ImGui::End();

}