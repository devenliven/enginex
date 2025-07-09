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
    deleteFramebuffer();
    m_pbrShader.reset();
    LOG_INFO("Renderer: Renderer shutdown complete!");
}

void Renderer::beginFrame()
{
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        int width  = (int)viewportSize.x;
        int height = (int)viewportSize.y;

        // Create or resize framebuffer if needed
        if (m_frameBuffer == 0 || m_framebufferWidth != width || m_framebufferHeight != height) {
            createFramebuffer(width, height);
        }

        m_viewportWidth  = width;
        m_viewportHeight = height;

        // Render scene to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glViewport(0, 0, width, height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene(scene);

        // Bind back to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Display the framebuffer texture in ImGui
        ImGui::Image(ImTextureRef{m_colorTexture}, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::Text("Viewport: %dx%d", m_viewportWidth, m_viewportHeight);
    }

    ImGui::End();
}

void Renderer::createFramebuffer(int width, int height)
{
    if (m_frameBuffer != 0) {
        deleteFramebuffer();
    }

    m_framebufferWidth  = width;
    m_framebufferHeight = height;

    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    // Color
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    // Depth
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Framebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::deleteFramebuffer()
{
    if (m_colorTexture) {
        glDeleteTextures(1, &m_colorTexture);
        m_colorTexture = 0;
    }
    if (m_depthTexture) {
        glDeleteTextures(1, &m_depthTexture);
        m_depthTexture = 0;
    }
    if (m_frameBuffer) {
        glDeleteFramebuffers(1, &m_frameBuffer);
        m_frameBuffer = 0;
    }
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