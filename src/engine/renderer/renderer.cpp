#include "pch.h"

#include "engine/renderer/renderer.h"
#include "engine/renderer/scene.h"
#include "engine/renderer/resources/resource_manager.h"
#include "engine/renderer/shaders/shader.h"
#include "engine/renderer/resources/model_resource.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/lighting/light_manager.h"
#include "common/logger.h"

bool Renderer::initialize()
{
    setupShaders();

    LOG_INFO("Renderer: Renderer initialized succesfully!");
    return true;
}

void Renderer::shutdown()
{
    m_pbrShader.reset();
    LOG_INFO("Renderer: Renderer shutdown complete!");
}

void Renderer::beginFrame()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderScene(Scene* scene)
{
    if (!scene || !m_pbrShader || !m_pbrShader->getShader()) {
        LOG_ERROR("Renderer::renderScene - Invalid scene or shader!");
        return;
    }

    Shader* shader = m_pbrShader->getShader();
    shader->use();

    Camera*   camera     = scene->getCamera();
    glm::mat4 projection = glm::perspective(glm::radians(camera->getZoom()), DEFAULT_ASPECT_RATIO, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
    glm::mat4 view       = camera->getViewMatrix();

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setVec3("viewPos", camera->getPosition());

    scene->getLightManager()->updateShaderUniforms(shader);

    for (const auto& [modelResource, transform] : scene->getModels()) {
        renderModel(modelResource, transform, shader);
    }
}

void Renderer::endFrame()
{
    // Any post rendering cleanup
}

void Renderer::setupShaders()
{
    m_pbrShader = GET_SHADER("pbr");
    if (!m_pbrShader) {
        LOG_ERROR("Renderer: Failed to load PBR shaders!");
    }
}

void Renderer::renderModel(const std::shared_ptr<ModelResource>& modelResource, const glm::mat4& modelMatrix, Shader* shader)
{
    if (!modelResource || !modelResource->isLoaded()) {
        return;
    }

    shader->setMat4("model", modelMatrix);

    Model* model = modelResource->getModel();
    model->draw(shader);
}