#include "pch.h"

#include "engine/renderer/renderer.h"
#include "engine/renderer/scene.h"
#include "engine/renderer/resources/resource_manager.h"
#include "engine/renderer/shaders/shader.h"
#include "engine/renderer/resources/model_resource.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/lighting/light_manager.h"
#include "common/logger.h"

#include <imgui.h>

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

    Camera* camera = scene->getCamera();

    float     aspectRatio = getViewportAspectRatio();
    glm::mat4 projection  = glm::perspective(glm::radians(camera->getZoom()), aspectRatio, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
    glm::mat4 view        = camera->getViewMatrix();

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

void Renderer::renderSceneToViewport(Scene* scene)
{
    ImGui::Begin("Viewport");

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    if (viewportSize.x > 0 && viewportSize.y > 0) {
        glViewport(0, 0, (int)viewportSize.x, (int)viewportSize.y);

        m_viewportWidth  = (int)viewportSize.x;
        m_viewportHeight = (int)viewportSize.y;

        // Maybe change this to render to a framebuffer texture?
        renderScene(scene);

        ImGui::SetCursorPos(ImVec2(10, 30)); // Position overlay text
        ImGui::Text("Viewport: %dx%d", m_viewportWidth, m_viewportHeight);
    }

    ImGui::End();
}

ImVec2 Renderer::getViewportSize() const
{
    return ImVec2((float)m_viewportWidth, (float)m_viewportHeight);
}

float Renderer::getViewportAspectRatio() const
{
    if (m_viewportHeight > 0) {
        return (float)m_viewportWidth / (float)m_viewportHeight;
    }
    return DEFAULT_ASPECT_RATIO;
}