#include "pch.h"

#include "engine.h"
#include "window.h"
#include "editor/app.h"

#include "utilities/logger.h"
#include "utilities/timer.h"

#include <glad/glad.h>

Engine::~Engine()
{
    //
}

bool Engine::init(std::shared_ptr<App> app)
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

    m_app = app;

    m_app->onWindowResize(m_windowWidth, m_windowHeight);
    m_window->setEventCallback([this](WindowEvent event, int param1, int param2, int param3) { this->handleWindowEvent(event, param1, param2, param3); });

    run();

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
                m_app->onWindowResize(param1, param2);
            }
        }
        case WindowEvent::KeyPressed: {
            KeyCode key         = static_cast<KeyCode>(param1);
            int     repeatCount = param2;

            if (m_app) {
                m_app->onKeyPressed(key, repeatCount);
            }
            break;
        }
    }
}