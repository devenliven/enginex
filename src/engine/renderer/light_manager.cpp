#include "pch.h"

#include "engine/renderer/light_manager.h"
#include "utilities/logger.h"

void LightManager::addLight(std::unique_ptr<Light> light)
{
    if (m_lights.size() >= MAX_LIGHTS) {
        LOG_WARN("Cannot add more lights. Maximum of {} lights supported.", MAX_LIGHTS);
        return;
    }

    m_lights.push_back(std::move(light));
}

void LightManager::removeLight(size_t index)
{
    if (index < m_lights.size()) {
        m_lights.erase(m_lights.begin() + index);
    }
}

void LightManager::clearLights()
{
    m_lights.clear();
}

Light* LightManager::getLight(size_t index)
{
    if (index >= m_lights.size()) {
        LOG_WARN("Tried to access invalid light index {}. Light count: {}", index, m_lights.size());
        return nullptr;
    }

    return m_lights[index].get();
}

void LightManager::updateShaderUniforms(Shader* shader) const
{
    if (!shader) return;

    shader->setInt("numLights", static_cast<int>(m_lights.size()));

    for (size_t i = 0; i < m_lights.size() && i < MAX_LIGHTS; ++i) {
        const Light* light = m_lights[i].get();
        if (!light || !light->isEnabled()) continue;

        std::string uniformBase = "lights[" + std::to_string(i) + "]";
        shader->setVec3(uniformBase + ".color", light->getColor());
        shader->setFloat(uniformBase + ".intensity", light->getIntensity());

        switch (light->getType()) {
            case Light::DIRECTIONAL: shader->setVec3(uniformBase + ".direction", light->getDirection()); break;
            case Light::POINT: shader->setVec3(uniformBase + ".position", light->getPosition()); break;
            case Light::SPOT:
                shader->setVec3(uniformBase + ".position", light->getPosition());
                shader->setVec3(uniformBase + ".direction", light->getDirection());
                shader->setFloat(uniformBase + ".cutoff", light->getCutoff());
                shader->setFloat(uniformBase + ".outerCutoff", light->getOuterCutoff());
                break;
        }
    }
}

void LightManager::renderDebugVisualization(LineRenderer* renderer, const glm::vec3& modelCenter) const
{
    if (!renderer) return;

    for (const auto& light : m_lights) {
        if (!light || !light->isEnabled()) continue;

        if (light->getType() == Light::DIRECTIONAL) {
            glm::vec3 directionEnd = modelCenter - light->getDirection() * 5.0f;
            renderer->drawLine(modelCenter, directionEnd, light->getColor());
        } else {
            renderer->drawLine(modelCenter, light->getPosition(), light->getColor());
        }
    }
}