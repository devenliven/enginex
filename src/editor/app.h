#ifndef EDITOR_APP_H_
#define EDITOR_APP_H_

#include "utilities/logger.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/model.h"
#include "engine/renderer/camera.h"
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

    InputManager* m_inputManager = nullptr;

    bool m_showLightLines = true;

    std::vector<glm::vec3> m_lightPositions;
    std::vector<glm::vec3> m_lightColors;

    void processInput(float deltaTime);
    void setupLightData();
};

// int main()
// {
//     auto app = std::make_unique<App>();
//     app->run();
// }

#endif // EDITOR_APP_H_