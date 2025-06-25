#include "pch.h"

#include "app.h"

#include "glad/glad.h"
#include "utilities/file.h"

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

void App::onUpdate(float deltaTime) {}

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

    // glBindVertexArray(m_VAO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// void App::onWindowResize(int newWidth, int newHeight)
// {
//     m_windowWidth  = newWidth;
//     m_windowHeight = newHeight;
// }

// void App::onKeyPressed(KeyCode key, int repeatCount)
// {
//     //
//     if (key == KeyCode::W) {
//         m_camera->processKeyboard(FORWARD, m_deltaTime);
//     }

//     if (key == KeyCode::S) {
//         m_camera->processKeyboard(BACKWARD, m_deltaTime);
//     }

//     if (key == KeyCode::D) {
//         m_camera->processKeyboard(RIGHT, m_deltaTime);
//     }

//     if (key == KeyCode::A) {
//         m_camera->processKeyboard(LEFT, m_deltaTime);
//     }
// }