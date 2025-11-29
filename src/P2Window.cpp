#include "P2Window.h"
#include "SceneLoader.h"
#include "math/Quaternion.h"
#include "math/Real.h"

P2Window::P2Window(int width, int height) : _width(width), _height(height) {}

void P2Window::initialize() {

  raycaster = std::make_unique<Raycaster>(_width, _height);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);

  SceneLoader::load("assets/scenes/central_sphere.yml", sceneManager);
  SceneLoader::load("assets/scenes/tp1.yml", sceneManager);
  SceneLoader::load("assets/scenes/p1.yml", sceneManager);

  sceneManager.setActiveScene("tp1");

  pbrRenderer = std::make_unique<PBRRenderer>();

  view = std::make_unique<View>(&sceneManager);

  pbrRenderer->initialize();

  currentRenderMode = RenderMode::OpenGL;

}

void P2Window::update(const float deltaTime) {

  bool rayAction = rayCastTimer.update(deltaTime);
  
  float dt = currentRenderMode == RenderMode::RayCasting ? rayCastTimer.interval : deltaTime;

  sceneManager.update(dt);

  if (sceneManager.getActiveScene()->name == "central_sphere") {

    auto light = sceneManager.getActiveScene()->getLight(0);

    cg::vec3f p = light->position();

    cg::quatf rotation(0.25f * dt, {0, 1, 0});

    p = rotation.rotate(p);

    light->setPosition(p);

  }

  /*if (currentRenderMode == RenderMode::RayCasting && rayAction)
    raycaster->update();
*/
}

void P2Window::renderScene() {

  auto* scene = sceneManager.getActiveScene();
  
  if (!scene) 
    return;

  cg::Color bg = scene->backgroundColor;
  
  glClearColor(bg.r, bg.g, bg.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (_height > 0) {
    float ratio = ((float)_width / _height);
    scene->camera.setAspectRatio(ratio);
    scene->camera.update(); 
  }

  if (currentRenderMode == RenderMode::OpenGL) {

    if (pbrRenderer) {

      pbrRenderer->setImageSize(_width, _height);
      
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

void P2Window::gui() {

  auto* scene =  sceneManager.getActiveScene();

  ImGui::Begin((std::string("Controles ") + scene->name).c_str());

  if (scene->name == "tp1") {

    view->cameraControl();

    view->lightControl();

    view->materialControl();

  }

  view->renderOptions(currentRenderMode, raycaster.get());

  view->sceneControl();

  ImGui::End();

}

bool P2Window::onMouseLeftPress(int x, int y) {

  if (ImGui::GetIO().WantCaptureMouse)
    return false; 

  auto* scene = sceneManager.getActiveScene();

  if (!scene) 
    return false;

  if (raycaster) {

    raycaster->setImageSize(_width, _height);
    
    Actor* clickedActor = raycaster->pick(*scene, x, y);

    if (clickedActor)  
      view->setSelectedActor(clickedActor);

  }

  return true; 

}

void Raycaster::setImageSize(int width, int height) {
    
  if (image->width() != width || image->height() != height) {

    image = std::make_unique<cg::GLImage>(width, height);

    imageBuffer[0] = cg::ImageBuffer(width, height);
    imageBuffer[1] = cg::ImageBuffer(width, height);
    
    _viewport = Viewport{}; 

  }

}

void P2Window::resize(int w, int h) {

  _width = w;
  _height = h;

}