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
    m_model->draw(m_shader.get());
}

void App::processInput(float deltaTime)
{
    if (!m_inputManager || !m_camera) return;

    if (m_inputManager->isKeyPressed(KeyCode::W)) {
        m_camera->processKeyboard(FORWARD, deltaTime);
    }

    if (m_inputManager->isKeyPressed(KeyCode::S)) {
        m_camera->processKeyboard(BACKWARD, deltaTime);
    }

    if (m_inputManager->isKeyPressed(KeyCode::A)) {
        m_camera->processKeyboard(LEFT, deltaTime);
    }

    if (m_inputManager->isKeyPressed(KeyCode::D)) {
        m_camera->processKeyboard(RIGHT, deltaTime);
    }

    m_camera->processMouse(m_inputManager->getMouseDelta().x, m_inputManager->getMouseDelta().y);
}