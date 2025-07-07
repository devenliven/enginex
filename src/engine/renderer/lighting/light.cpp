#include "pch.h"

#include "engine/renderer/lighting/light.h"

Light::Light(Type type, const glm::vec3& position, const glm::vec3& color, float intensity)
    : m_type(type)
    , m_position(position)
    , m_color(color)
    , m_intensity(intensity)
    , m_enabled(true)
    , m_direction(0.0f, -1.0f, 0.0f)
{
    //
}

std::unique_ptr<Light> Light::createDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
{
    auto light = std::make_unique<Light>(DIRECTIONAL, glm::vec3(0.0f), color, intensity);
    light->setDirection(glm::normalize(direction));
    return light;
}

std::unique_ptr<Light> Light::createSunLight(const glm::vec3& direction)
{
    return createDirectionalLight(direction, glm::vec3(1.0f, 0.95f, 0.8f), 3.0f);
}

std::unique_ptr<Light> Light::createPointLight(const glm::vec3& position, const glm::vec3& color, float intensity)
{
    return std::make_unique<Light>(POINT, position, color, intensity);
}

std::unique_ptr<Light> Light::createSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float intensity)
{
    // TODO: Spotlights may need direction, cutoff angle, and falloff in future.
    auto light = std::make_unique<Light>(SPOT, position, color, intensity);
    light->setDirection(glm::normalize(direction));
    light->setCutoff(glm::cos(glm::radians(13.5f)));
    light->setOuterCuttoff(glm::cos(glm::radians(17.5f)));
    return light;
}