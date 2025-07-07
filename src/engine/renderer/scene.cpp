#include "pch.h"

#include "engine/renderer/scene.h"
#include "engine/renderer/lighting/light.h"
#include "common/logger.h"

bool Scene::initialize()
{
    m_camera       = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    m_lightManager = std::make_unique<LightManager>();

    setupDefaultLights();

    LOG_INFO("Scene: Scene initialized succesfully!");
    return true;
}

void Scene::update(float deltaTime)
{
    // Do something with this later, maybe?
}

void Scene::addModel(std::shared_ptr<ModelResource> model, const glm::mat4& transform)
{
    if (model) {
        m_models.emplace_back(model, transform);
        LOG_INFO("Scene: Added model to scene!");
    }
}

void Scene::removeModel(size_t index)
{
    if (index < m_models.size()) {
        m_models.erase(m_models.begin() + index);
        LOG_INFO("Scene: Model removed from scene!");
    }
}

void Scene::setupDefaultLights()
{
    m_lightManager->addLight(Light::createSunLight(glm::vec3(-0.3f, -0.7f, -0.2f)));
    m_lightManager->addLight(Light::createPointLight(glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(1.0f, 0.95f, 0.8f), 25.0f));
    m_lightManager->addLight(Light::createPointLight(glm::vec3(-3.0f, 2.0f, 3.0f), glm::vec3(0.8f, 0.9f, 1.0f), 12.0f));
    m_lightManager->addLight(Light::createPointLight(glm::vec3(0.0f, -1.0f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f));
}