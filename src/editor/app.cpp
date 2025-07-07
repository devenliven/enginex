#include "pch.h"

#include "editor/app.h"
#include "utilities/file.h"
#include "engine/core/input/input_manager.h"
#include "engine/renderer/resource_manager.h"
#include "engine/renderer/model_resource.h"
#include "engine/renderer/shader_resource.h"

void App::initShaders() {}

void App::onInit()
{
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

    m_shader = GET_SHADER("pbr");
    if (!m_shader) {
        LOG_ERROR("App: Failed to load pbr shaders!");
        return;
    }

    m_model = GET_MODEL("assets/models/chair/modern_arm_chair_01_1k.gltf");
    if (!m_model) {
        LOG_ERROR("App: Failed to load model!");
    }

    // m_model  = std::make_unique<Model>("assets/models/chair/modern_arm_chair_01_1k.gltf");
    // m_model        = std::make_unique<Model>("assets/models/corrugated_iron_4k/corrugated_iron_4k.gltf");
    // m_model        = std::make_unique<Model>("assets/models/korean_fire_extinguisher_01_4k/korean_fire_extinguisher_01_4k.gltf");
    m_lineRenderer = std::make_unique<LineRenderer>();
    if (!m_lineRenderer->initialize()) {
        LOG_ERROR("Failed to initialize line renderer!");
        m_lineRenderer.reset();
    }

    m_lightManager = std::make_unique<LightManager>();
    setupLights();

    RESOURCE_MANAGER.logStats();
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

    if (!m_shader || !m_shader->getShader()) {
        LOG_ERROR("App:onRender() - No valid shaders available!");
        return;
    }

    Shader* shader = m_shader->getShader();

    shader->use();

    glm::mat4 projection = glm::perspective(glm::radians(m_camera->getZoom()), DEFAULT_ASPECT_RATIO, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
    glm::mat4 view       = m_camera->getViewMatrix();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setVec3("viewPos", m_camera->getPosition());

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    shader->setMat4("model", model);

    m_lightManager->updateShaderUniforms(shader);

    Model* chairModel = m_model->getModel();
    chairModel->draw(shader);

    {
        ImGui::Begin("Lighting");

        for (int i = 0; i < m_lightManager->getLightCount(); i++) {
            auto        light    = m_lightManager->getLight(i);
            std::string typeName = light->getTypeName();
            ImGui::Text("%s Light", typeName.c_str());

            auto& position = light->getPosition();
            float pos[3]   = {position.x, position.y, position.z};

            std::string label;

            bool changed = false;

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
}

void App::setupLights()
{
    // Add a sun light
    m_lightManager->addLight(Light::createSunLight(glm::vec3(-0.3f, -0.7f, -0.2f)));

    m_lightManager->addLight(Light::createPointLight(glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(1.0f, 0.95f, 0.8f), 25.0f));
    m_lightManager->addLight(Light::createPointLight(glm::vec3(-3.0f, 2.0f, 3.0f), glm::vec3(0.8f, 0.9f, 1.0f), 12.0f));
    m_lightManager->addLight(Light::createPointLight(glm::vec3(0.0f, -1.0f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f));

    // m_lightManager->addLight(Light::createSpotLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

void App::processInput(float deltaTime)
{
    if (!m_inputManager || !m_camera) return;

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
        return;
    }

    const float baseSpeed       = m_camera->getMovementSpeed();
    const float speedMultiplier = m_inputManager->isKeyPressed(KeyCode::Shift) ? DEFAULT_CAMERA_SPEED_MULTIPLIER : 1.0f;
    const float moveSpeed       = baseSpeed * speedMultiplier;

    struct MovementInput {
        KeyCode         key;
        CAMERA_MOVEMENT direction;
    };

    const std::array<MovementInput, 4> movements = {{{KeyCode::W, FORWARD}, {KeyCode::S, BACKWARD}, {KeyCode::A, LEFT}, {KeyCode::D, RIGHT}}};

    for (const auto& movement : movements) {
        if (m_inputManager->isKeyPressed(movement.key)) {
            m_camera->processKeyboard(movement.direction, deltaTime, moveSpeed);
        }
    }

    RawMouseInput mouseDelta = m_inputManager->getMouseDelta();
    if (mouseDelta.deltaX != 0.0f || mouseDelta.deltaY != 0.0f) {
        m_camera->processMouse(mouseDelta.deltaX, mouseDelta.deltaY);
    }
    m_inputManager->resetMouseDelta();
}