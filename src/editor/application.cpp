#include "pch.h"

#include "editor/application.h"
#include "engine/renderer/scene.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/resources/resource_manager.h"
#include "engine/core/input/input_manager.h"

#include "common/logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

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
    // auto chairModel = GET_MODEL("assets/models/chair/modern_arm_chair_01_1k.gltf");
    auto chairModel = GET_MODEL("assets/models/korean_fire_extinguisher_01_4k/korean_fire_extinguisher_01_4k.gltf");
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

    renderUI();

    m_renderer->renderSceneToViewport(m_scene.get());
    m_renderer->endFrame();
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
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoUndocking);
    }

    // Menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                // Handle exit
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();

    renderSidebar();
}

void App::renderSidebar()
{
    ImGui::Begin("Sidebar", 0, ImGuiWindowFlags_NoDecoration);

    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Models: %zu", m_scene->getModels().size());
        ImGui::Separator();
    }

    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
        LightManager* lightManager = m_scene->getLightManager();
        auto          lightCount   = lightManager->getLightCount();
        for (int i = 0; i < lightCount; i++) {
            auto        light    = lightManager->getLight(i);
            std::string typeName = light->getTypeName();

            ImGui::SetNextItemOpen(true);
            if (ImGui::TreeNode((typeName + " Light##" + std::to_string(i)).c_str())) {
                auto& position = light->getPosition();
                float pos[3]   = {position.x, position.y, position.z};

                bool changed = false;
                changed |= ImGui::SliderFloat("X", &pos[0], -10.0f, 10.0f);
                changed |= ImGui::SliderFloat("Y", &pos[1], -10.0f, 10.0f);
                changed |= ImGui::SliderFloat("Z", &pos[2], -10.0f, 10.0f);

                if (changed) {
                    light->setPosition(glm::vec3(pos[0], pos[1], pos[2]));
                }

                auto& color  = light->getColor();
                float col[3] = {color.r, color.g, color.b};
                if (ImGui::ColorEdit3("Color", col)) {
                    light->setColor(glm::vec3(col[0], col[1], col[2]));
                }

                float intensity = light->getIntensity();
                if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 50.0f)) {
                    light->setIntensity(intensity);
                }

                ImGui::TreePop();
            }

            if (i < lightCount - 1) {
                ImGui::Separator();
            }
        }
    }

    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        Camera* camera = m_scene->getCamera();
        if (camera) {
            auto pos = camera->getPosition();
            ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            ImGui::Text("Speed: %.2f", camera->getMovementSpeed());
        }
    }

    ImGui::End();
}