#ifndef EDITOR_APP_H_
#define EDITOR_APP_H_

#include "utilities/logger.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/model.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/light_manager.h"
#include "engine/core/window.h"
#include "editor/app_interface.h"
#include "editor/tools/line_renderer.h"

#include <memory>

class InputManager;

class App : public IApp
{
  public:
    App()  = default;
    ~App() = default;

    void onInit();
    void onUpdate(float deltaTime);
    void onRender();
    void initShaders();

    void setInputManager(InputManager* inputManager) { m_inputManager = inputManager; }

  private:
    std::unique_ptr<Shader>       m_shader       = nullptr;
    std::unique_ptr<Model>        m_model        = nullptr;
    std::unique_ptr<Camera>       m_camera       = nullptr;
    std::unique_ptr<LineRenderer> m_lineRenderer = nullptr;
    std::unique_ptr<LightManager> m_lightManager = nullptr;

    InputManager* m_inputManager = nullptr;
    bool          drawLightLines = false;

    static constexpr float DEFAULT_CAMERA_SPEED_MULTIPLIER = 3.0f;
    static constexpr float DEFAULT_ASPECT_RATIO            = 1280.0f / 720.0f;
    static constexpr float DEFAULT_FOV                     = 45.0f;
    static constexpr float DEFAULT_NEAR_PLANE              = 0.1f;
    static constexpr float DEFAULT_FAR_PLANE               = 100.0f;

    void processInput(float deltaTime);
    void setupLights();
};

#endif // EDITOR_APP_H_