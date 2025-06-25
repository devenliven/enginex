#ifndef ENGINE_H_
#define ENGINE_H_

#include <memory>

#include "window.h"

class App;

class Engine
{
  public:
    Engine() = default;
    ~Engine();

    bool init(std::shared_ptr<App> app);
    void run();

    int setWindowWidth(int width) { m_windowWidth = width; }
    int setWindowHeight(int height) { m_windowHeight = height; }

    int getWindowWidth() const { return m_windowWidth; }
    int getWindowHeight() const { return m_windowHeight; }

  private:
    std::unique_ptr<Window> m_window = nullptr;
    std::shared_ptr<App>    m_app    = nullptr;
    // IApp*                   m_app    = nullptr;

    int m_windowWidth  = 1280;
    int m_windowHeight = 720;

    void handleWindowEvent(WindowEvent event, int param1, int param2, int param3);
};

#endif // ENGINE_H_