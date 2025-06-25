#ifndef EDITOR_APP_H_
#define EDITOR_APP_H_

#include <memory>

#include "utilities/logger.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/model.h"
#include "engine/renderer/camera.h"

#include "engine/core/window.h"

enum class WindowEvent;

class App
{
  public:
    App() { LOG_INFO("App instantiated!"); }
    ~App() = default;

    void onInit();
    void onUpdate(float deltaTime);
    void onRender();
    void initShaders();

    virtual void onWindowResize(int newWidth, int newHeight);
    virtual void onKeyPressed(KeyCode key, int repeatCount);

  private:
    std::unique_ptr<Shader> m_shader = nullptr;
    std::unique_ptr<Model>  m_model  = nullptr;
    std::unique_ptr<Camera> m_camera = nullptr;

    int   m_windowWidth  = 0;
    int   m_windowHeight = 0;
    float m_deltaTime    = 0;
};

// int main()
// {
//     auto app = std::make_unique<App>();
//     app->run();
// }

#endif // EDITOR_APP_H_