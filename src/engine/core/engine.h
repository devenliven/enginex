#ifndef ENGINE_H_
#define ENGINE_H_

#include "engine/core/window.h"
#include "editor/app_interface.h"

#include <memory>

class InputManager;

class Engine
{
  public:
    Engine() = default;
    ~Engine();

    bool init(std::shared_ptr<IApp> app);
    void run();
    void cleanup();

    int setWindowWidth(int width) { m_windowWidth = width; }
    int setWindowHeight(int height) { m_windowHeight = height; }

    int getWindowWidth() const { return m_windowWidth; }
    int getWindowHeight() const { return m_windowHeight; }

    InputManager* getInputManager() { return m_inputManager.get(); }

  private:
    std::unique_ptr<Window>       m_window       = nullptr;
    std::shared_ptr<IApp>         m_app          = nullptr;
    std::unique_ptr<InputManager> m_inputManager = nullptr;

    int m_windowWidth  = 1920;
    int m_windowHeight = 1080;

    void handleWindowEvent(WindowEvent event, int param1, int param2, int param3);
};

#endif // ENGINE_H_