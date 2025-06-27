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
    m_shader = std::make_unique<Shader>("standard.vs", "standard.fs");
    m_model  = std::make_unique<Model>("assets/models/chair/modern_arm_chair_01_1k.gltf");
}

void App::onUpdate(float deltaTime)
{
    processInput(deltaTime);
}

void App::onRender()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->use();

    glm::mat4 projection = glm::perspective(glm::radians(m_camera->getZoom()), (float)1280 / (float)720, 0.1f, 100.0f);
    glm::mat4 view       = m_camera->getViewMatrix();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model           = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    m_shader->setMat4("model", model);

    m_shader->setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
    m_shader->setVec3("lightCol", glm::vec3(1.0f, 1.0f, 1.0f));
    m_shader->setVec3("viewPos", m_camera->getPosition());

    m_shader->setBool("hasTexture", true);

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