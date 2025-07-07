#ifndef EDITOR_APP_H_
#define EDITOR_APP_H_

#include "common/forward_dec.h"
#include "editor/app_interface.h"

class InputManager;
class Scene;
class Renderer;

class App : public IApp
{
  public:
    App();
    ~App();

    void onInit();
    void onUpdate(float deltaTime);
    void onRender();

    void setInputManager(InputManager* inputManager) { m_inputManager = inputManager; }

  private:
    std::unique_ptr<Scene>    m_scene;
    std::unique_ptr<Renderer> m_renderer;
    InputManager*             m_inputManager = nullptr;

    static constexpr float DEFAULT_CAMERA_SPEED_MULTIPLIER = 3.0f;

    void processInput(float deltaTime);
    void renderUI();
};

#endif // EDITOR_APP_H_