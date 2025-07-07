#include "pch.h"

#include "engine/core/engine.h"
#include "engine/core/window.h"
#include "engine/core/platform/windows/os.h"
#include "engine/core/input/input_manager.h"
#include "engine/renderer/resources/resource_manager.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_win32.h>

#include "common/logger.h"
#include "common/timer.h"

class App;

Engine::~Engine()
{
    cleanup();

    RESOURCE_MANAGER.clearAll();
    RESOURCE_MANAGER.logStats();
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

void Engine::cleanup()
{
    LOG_INFO("Engine: Shutting down!");

    if (m_window && m_window->getOpenGLContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (m_app) {
        m_app.reset();
    }

    if (m_inputManager) {
        m_inputManager->clearAllStates();
        m_inputManager.reset();
    }

    if (m_window) {
        m_window.reset();
    }

    LOG_INFO("Engine: Shutdown complete!");
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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        m_app->onUpdate(deltaTime);
        m_app->onRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_window->pollEvents();
        m_window->swapBuffers();
    }

    cleanup();
}

void Engine::handleWindowEvent(WindowEvent event, int param1, int param2, int param3)
{
    switch (event) {
        case WindowEvent::Resize: {
            if (m_app) {
                glViewport(0, 0, param1, param2);
                // m_app->onWindowResize(param1, param2);
            }
            break;
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
            break;
        }
        case WindowEvent::MouseButtonPressed: {
            MouseButton button = static_cast<MouseButton>(param1);
            m_inputManager->onMouseButtonDown(button);
            break;
        }
        case WindowEvent::MouseButtonReleased: {
            MouseButton button = static_cast<MouseButton>(param1);
            m_inputManager->onMouseButtonUp(button);
            break;
        }
        case WindowEvent::MouseMoved: {
            RawMouseInput input;
            input.deltaX     = static_cast<float>(param1);
            input.deltaY     = static_cast<float>(param2);
            input.wheelDelta = 0;
            m_inputManager->onMouseInput(input);
            break;
        }
    }
}