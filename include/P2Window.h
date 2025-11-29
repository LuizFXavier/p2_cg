#pragma once

#include "graphics/GLRenderWindow3.h"
#include "SceneManager.h"
#include "graphics/Camera.h"
#include "graphics/Color.h"
#include "graphics/GLImage.h"
#include "Intersection.h"
#include "RayCaster.h"
#include "PBRRenderer.h"
#include "RenderMode.h"
#include "View.h"
#include "GameTimer.h"

class P2Window {

  public:

    P2Window(int width, int height);

    void initialize();
    void update(float deltaTime);
    void renderScene();
    void gui();
    bool onMouseLeftPress(int x, int y);

    void resize(int w, int h);

  private:

    int _width, _height;
    
    SceneManager sceneManager;
    std::unique_ptr<Raycaster> raycaster;
    std::unique_ptr<PBRRenderer> pbrRenderer; 
    std::unique_ptr<View> view;
    
    RenderMode currentRenderMode = RenderMode::OpenGL;
    GameTimer rayCastTimer{5000.0f};

}; 