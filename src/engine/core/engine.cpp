#include "pch.h"

#include "engine/core/engine.h"
#include "engine/core/window.h"
#include "engine/core/win32/os.h"
#include "engine/core/input/input_manager.h"
#include "utilities/logger.h"
#include "utilities/timer.h"

#include <glad/glad.h>

class App;

Engine::~Engine()
{
    //
}

bool Engine::init(std::shared_ptr<IApp> app)
{
    m_window = std::make_unique<Window>();
    if (!m_window) {
        LOG_ERROR("Engine: Failed to construct Window.");
        return false;
    }

    WindowData windowData;
    windowData.title  = "Engine";
    windowData.width  = m_windowWidth;
    windowData.height = m_windowHeight;

    if (!m_window->create(windowData)) {
        LOG_ERROR("Failed to create window!");
        return false;
    }

    m_inputManager = std::make_unique<InputManager>();
    if (!m_inputManager) {
        LOG_ERROR("Failed to initialize Input Manager!");
        return false;
    }

    m_app = app;
    m_window->setEventCallback([this](WindowEvent event, int param1, int param2, int param3) { this->handleWindowEvent(event, param1, param2, param3); });

    return true;
}

void Engine::run()
{
    if (!m_window) {
        return;
    }

    glEnable(GL_DEPTH_TEST);

    Timer timer;

    m_app->onInit();

    while (m_window->isOpen()) {
        float deltaTime = timer.getDeltaTime();

        m_inputManager->update();

        m_app->onUpdate(deltaTime);
        m_app->onRender();

        m_window->pollEvents();
        m_window->swapBuffers();
    }
}

void Engine::handleWindowEvent(WindowEvent event, int param1, int param2, int param3)
{
    switch (event) {
        case WindowEvent::Resize: {
            if (m_app) {
                glViewport(0, 0, param1, param2);
                // m_app->onWindowResize(param1, param2);
            }
        }
        case WindowEvent::KeyPressed: {
            KeyCode key         = static_cast<KeyCode>(param1);
            int     repeatCount = param2;

            m_inputManager->onKeyDown(key, repeatCount);

            if (m_app) {
                // m_app->onKeyPressed(key, repeatCount);
            }
            break;
        }

        case WindowEvent::KeyReleased: {
            KeyCode key = static_cast<KeyCode>(param1);
            m_inputManager->onKeyUp(key);
        }
    }
}