#include "pch.h"

#include "editor/app.h"
#include "utilities/file.h"
#include "engine/core/input/input_manager.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void App::initShaders() {}

void App::onInit()
{
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    m_shader = std::make_unique<Shader>("pbr.vs", "pbr.fs");
    m_model  = std::make_unique<Model>("assets/models/chair/modern_arm_chair_01_1k.gltf");
    // m_model = std::make_unique<Model>("assets/models/toycar/toycar.gltf");
}

void App::onUpdate(float deltaTime)
{
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

    // Set number of lights
    m_shader->setInt("numLights", 3);

    // Key light (main illumination) - warm white
    m_shader->setVec3("lights[0].position", glm::vec3(4.0f, 4.0f, 4.0f));
    m_shader->setVec3("lights[0].color", glm::vec3(1.0f, 0.95f, 0.8f)); // Slightly warm
    m_shader->setFloat("lights[0].intensity", 25.0f);                   // Higher intensity for PBR

    // Fill light (softer, cooler) - reduces harsh shadows
    m_shader->setVec3("lights[1].position", glm::vec3(-3.0f, 2.0f, 3.0f));
    m_shader->setVec3("lights[1].color", glm::vec3(0.8f, 0.9f, 1.0f)); // Cool blue tint
    m_shader->setFloat("lights[1].intensity", 12.0f);

    // Rim/back light - adds definition to edges
    m_shader->setVec3("lights[2].position", glm::vec3(0.0f, -1.0f, -4.0f));
    m_shader->setVec3("lights[2].color", glm::vec3(1.0f, 1.0f, 1.0f)); // Pure white
    m_shader->setFloat("lights[2].intensity", 8.0f);

    m_model->draw(m_shader.get());
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