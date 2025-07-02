#include "pch.h"

#include "editor/app.h"
#include "utilities/file.h"
#include "engine/core/input/input_manager.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_win32.h>

void App::initShaders() {}

void App::onInit()
{
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    m_shader = std::make_unique<Shader>("pbr.vs", "pbr.fs");
    m_model  = std::make_unique<Model>("assets/models/chair/modern_arm_chair_01_1k.gltf");
    // m_model = std::make_unique<Model>("assets/models/toycar/toycar.gltf");

    m_lineRenderer = std::make_unique<LineRenderer>();
    if (!m_lineRenderer->initialize()) {
        LOG_ERROR("Failed to initialize line renderer!");
        m_lineRenderer.reset();
    }

    m_lightManager = std::make_unique<LightManager>();
    setupLights();
}

void App::onUpdate(float deltaTime)
{
    float       fps       = 1.0f / deltaTime;
    std::string fpsFormat = std::format("FPS: {}", fps);
    ImDrawList* draw      = ImGui::GetForegroundDrawList();

    draw->AddText(ImVec2(0, 0), ImColor(255, 255, 255, 255), fpsFormat.c_str());
    processInput(deltaTime);
}

void App::onRender()
{

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->use();

    glm::mat4 projection = glm::perspective(glm::radians(m_camera->getZoom()), (float)1280 / (float)720, 0.1f, 100.0f);
    glm::mat4 view       = m_camera->getViewMatrix();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);
    m_shader->setVec3("viewPos", m_camera->getPosition());

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    // model           = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    m_shader->setMat4("model", model);

    m_lightManager->updateShaderUniforms(m_shader.get());
    m_model->draw(m_shader.get());

    if (m_lineRenderer && drawLightLines) {
        m_lineRenderer->beginFrame(projection * view);

        glm::vec3 modelCenter = glm::vec3(0.0f, 0.0f, 0.0f);

        // Use light manager for debug visualization
        m_lightManager->renderDebugVisualization(m_lineRenderer.get(), modelCenter);

        // // Draw coordinate axes
        // m_lineRenderer->drawLine(modelCenter, modelCenter + glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // m_lineRenderer->drawLine(modelCenter, modelCenter + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // m_lineRenderer->drawLine(modelCenter, modelCenter + glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        m_lineRenderer->endFrame();
    }

    {
        ImGui::Begin("Lighting");

        for (int i = 0; i < m_lightManager->getLightCount(); i++) {
            auto light = m_lightManager->getLight(i);
            ImGui::Text("%s Light", light->getTypeName().c_str());

            auto& position = light->getPosition();
            float pos[3]   = {position.x, position.y, position.z};

            std::string label;

            label = std::format("X##%d", i);
            if (ImGui::SliderFloat(label.c_str(), &pos[0], -10.0f, 10.0f)) {
                LOG_INFO("X: {}", pos[0]);
                light->setPosition(glm::vec3(pos[0], pos[1], pos[2]));
            }

            label = std::format("Y##%d", i);
            if (ImGui::SliderFloat(label.c_str(), &pos[1], -10.0f, 10.0f)) {
                LOG_INFO("Y: {}", pos[1]);
                light->setPosition(glm::vec3(pos[0], pos[1], pos[2]));
            }

            label = std::format("Z##%d", i);
            if (ImGui::SliderFloat(label.c_str(), &pos[2], -10.0f, 10.0f)) {
                LOG_INFO("Z: {}", pos[2]);
                light->setPosition(glm::vec3(pos[0], pos[1], pos[2]));
            }
        }

        // auto& position = m_lightManager->getLight(0)->getPosition();
        // float pos[3]   = {position.x, position.y, position.z};

        // if (ImGui::SliderFloat3("X:", pos, -10.0f, 10.0f)) {
        //     m_lightManager->getLight(0)->setPosition(glm::vec3(pos[0], pos[1], pos[2]));
        //     LOG_INFO("Light X: {}", position.x);
        // }

        ImGui::End();
    }
}

void App::setupLights()
{
    // Add a sun light
    // m_lightManager->addLight(Light::createSunLight(glm::vec3(-0.3f, -0.7f, -0.2f)));

    // // // Key light (main illumination) - warm white
    // m_lightManager->addLight(Light::createPointLight(glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(1.0f, 0.95f, 0.8f), 25.0f));

    // // Fill light (softer, cooler) - reduces harsh shadows
    // m_lightManager->addLight(Light::createPointLight(glm::vec3(-3.0f, 2.0f, 3.0f), glm::vec3(0.8f, 0.9f, 1.0f), 12.0f));

    // // Rim/back light - adds definition to edges
    // m_lightManager->addLight(Light::createPointLight(glm::vec3(0.0f, -1.0f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f));

    m_lightManager->addLight(Light::createSpotLight(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 50.0f));
}

void App::processInput(float deltaTime)
{
    if (!m_inputManager || !m_camera) return;

    float speedMultiplier = m_inputManager->isKeyPressed(KeyCode::Shift) ? 3.0f : 1.0f;

    if (m_inputManager->isKeyPressed(KeyCode::W)) {
        m_camera->processKeyboard(FORWARD, deltaTime, speedMultiplier);
    }

    if (m_inputManager->isKeyPressed(KeyCode::S)) {
        m_camera->processKeyboard(BACKWARD, deltaTime, speedMultiplier);
    }

    if (m_inputManager->isKeyPressed(KeyCode::A)) {
        m_camera->processKeyboard(LEFT, deltaTime, speedMultiplier);
    }

    if (m_inputManager->isKeyPressed(KeyCode::D)) {
        m_camera->processKeyboard(RIGHT, deltaTime, speedMultiplier);
    }

    RawMouseInput mouseDelta = m_inputManager->getMouseDelta();
    m_camera->processMouse(mouseDelta.deltaX, mouseDelta.deltaY);
    m_inputManager->resetMouseDelta();
}