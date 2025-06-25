#ifndef EDITOR_APP_H_
#define EDITOR_APP_H_

#include <memory>

#include "utilities/logger.h"
#include "editor/bootstrap.h"

#include "engine/renderer/shader.h"
#include "engine/renderer/model.h"
#include "engine/renderer/camera.h"

#include "engine/core/window.h"

class App : public Bootstrap
{
  public:
    App()  = default;
    ~App() = default;

    void onInit();
    void onUpdate(float deltaTime);
    void onRender();
    void initShaders();

    virtual void onKeyDown(KeyCode key, int repeatCount)
    {
        //
    }

    virtual void onKeyPressed(KeyCode key, int repeatCount)
    {
        //
    }

    virtual void onKeyReleased(KeyCode key)
    {
        //
    }

  private:
    std::unique_ptr<Shader> m_shader = nullptr;
    std::unique_ptr<Model>  m_model  = nullptr;
    std::unique_ptr<Camera> m_camera = nullptr;
};

// int main()
// {
//     auto app = std::make_unique<App>();
//     app->run();
// }

#endif // EDITOR_APP_H_