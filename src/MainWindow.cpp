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

/////////////////////////////////////////////////////////////////////
//
// MainWindow implementation
// ==========
MainWindow::MainWindow(int width, int height):
  Base{"P2 Wilker & José & Luigi", width, height}
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
  SceneLoader::load("../assets/scenes/p1.yml", sceneManager);

  sceneManager.setActiveScene("tp1");

  pbrRenderer = std::make_unique<PBRRenderer>();

  view = std::make_unique<View>(&sceneManager);

  pbrRenderer->initialize();

  currentRenderMode = RenderMode::OpenGL;

}

void
MainWindow::update()
{

  bool rayAction = rayCastTimer.update(deltaTime());
  
  float dt = currentRenderMode == RenderMode::RayCasting ? rayCastTimer.interval : deltaTime();

  sceneManager.update(dt);

  if (sceneManager.getActiveScene()->name == "central_sphere") {

    auto light = sceneManager.getActiveScene()->getLight(0);

    cg::vec3f p = light->position();

    cg::quatf rotation(0.25f * dt, {0, 1, 0});

    p = rotation.rotate(p);

    light->setPosition(p);

  }

  if (currentRenderMode == RenderMode::RayCasting && rayAction)
    raycaster->update();

}

void
MainWindow::renderScene()
{

  auto* scene = sceneManager.getActiveScene();
  
  if (!scene) 
    return;

  backgroundColor = scene->backgroundColor;
  
  glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  auto* scene =  sceneManager.getActiveScene();

  ImGui::Begin((std::string("Controles ") + scene->name).c_str());

  if (scene->name == "tp1") {

    view->cameraControl();

    view->lightControl();

    view->materialControl();

  }

  view->renderOptions(currentRenderMode);

  view->sceneControl();

  ImGui::End();

}