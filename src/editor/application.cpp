#include "pch.h"

#include "editor/application.h"
#include "engine/renderer/scene.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/resources/resource_manager.h"
#include "engine/core/input/input_manager.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/lighting/light_manager.h"

App::App()  = default;
App::~App() = default;

void App::onInit()
{
    m_scene = std::make_unique<Scene>();
    if (!m_scene->initialize()) {
        LOG_ERROR("App: Failed to initialize scene!");
        return;
    }

    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->initialize()) {
        LOG_ERROR("App: Failed to initialize renderer!");
        return;
    }

    // Load the chair model
    auto chairModel = GET_MODEL("assets/models/chair/modern_arm_chair_01_1k.gltf");
    if (chairModel) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        m_scene->addModel(chairModel, transform);
    } else {
        LOG_ERROR("App: Failed to load chair model!");
    }

    RESOURCE_MANAGER.logStats();
}

void App::onUpdate(float deltaTime)
{
    float       fps       = 1.0f / deltaTime;
    std::string fpsFormat = std::format("FPS: {}", fps);
    ImDrawList* draw      = ImGui::GetForegroundDrawList();
    draw->AddText(ImVec2(0, 0), ImColor(255, 255, 255, 255), fpsFormat.c_str());

    processInput(deltaTime);
    m_scene->update(deltaTime);
}

void App::onRender()
{
    m_renderer->beginFrame();
    m_renderer->renderScene(m_scene.get());
    m_renderer->endFrame();

    renderUI();
}

void App::processInput(float deltaTime)
{
    if (!m_inputManager || !m_scene->getCamera()) return;

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
        return;
    }

    Camera*     camera          = m_scene->getCamera();
    const float baseSpeed       = camera->getMovementSpeed();
    const float speedMultiplier = m_inputManager->isKeyPressed(KeyCode::Shift) ? DEFAULT_CAMERA_SPEED_MULTIPLIER : 1.0f;
    const float moveSpeed       = baseSpeed * speedMultiplier;

    struct MovementInput {
        KeyCode         key;
        CAMERA_MOVEMENT direction;
    };

    const std::array<MovementInput, 4> movements = {{{KeyCode::W, FORWARD}, {KeyCode::S, BACKWARD}, {KeyCode::A, LEFT}, {KeyCode::D, RIGHT}}};

    for (const auto& movement : movements) {
        if (m_inputManager->isKeyPressed(movement.key)) {
            camera->processKeyboard(movement.direction, deltaTime, moveSpeed);
        }
    }

    RawMouseInput mouseDelta = m_inputManager->getMouseDelta();
    if (mouseDelta.deltaX != 0.0f || mouseDelta.deltaY != 0.0f) {
        camera->processMouse(mouseDelta.deltaX, mouseDelta.deltaY);
    }
    m_inputManager->resetMouseDelta();
}

void App::renderUI()
{
    ImGui::Begin("Lighting");

    LightManager* lightManager = m_scene->getLightManager();
    for (int i = 0; i < lightManager->getLightCount(); i++) {
        auto        light    = lightManager->getLight(i);
        std::string typeName = light->getTypeName();
        ImGui::Text("%s Light", typeName.c_str());

        auto& position = light->getPosition();
        float pos[3]   = {position.x, position.y, position.z};

        std::string label;
        bool        changed = false;

        label = std::format("X [{}]##X{}", typeName, i);
        changed |= ImGui::SliderFloat(label.c_str(), &pos[0], -10.0f, 10.0f);

        label = std::format("Y [{}]##Y{}", typeName, i);
        changed |= ImGui::SliderFloat(label.c_str(), &pos[1], -10.0f, 10.0f);

        label = std::format("Z [{}]##Z{}", typeName, i);
        changed |= ImGui::SliderFloat(label.c_str(), &pos[2], -10.0f, 10.0f);

        if (changed) {
            LOG_INFO("Updated position: {}, {}, {}", pos[0], pos[1], pos[2]);
            light->setPosition(glm::vec3(pos[0], pos[1], pos[2]));
        }
    }

    ImGui::End();
}