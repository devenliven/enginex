#ifndef ENGINE_RENDERER_LIGHT_H_
#define ENGINE_RENDERER_LIGHT_H_

#include <glm/glm.hpp>
#include <memory>

class Light
{
  public:
    enum Type { DIRECTIONAL = 0, POINT = 1, SPOT = 2 };

    Light(Type type = POINT, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);

    Type             getType() const { return m_type; }
    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getDirection() const { return m_direction; }
    const glm::vec3  getColor() const { return m_color; }
    float            getIntensity() const { return m_intensity; }
    bool             isEnabled() const { return m_enabled; }

    void setPosition(const glm::vec3& position) { m_position = position; }
    void setDirection(const glm::vec3& direction) { m_direction = direction; }
    void setColor(const glm::vec3& color) { m_color = color; }
    void setIntensity(float intensity) { m_intensity = intensity; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    static std::unique_ptr<Light> createDirectionalLight(const glm::vec3& direction, const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);
    static std::unique_ptr<Light> createSunLight(const glm::vec3& direction = glm::vec3(-0.3f, -0.7f, -0.2f));
    static std::unique_ptr<Light> createPointLight(const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);
    static std::unique_ptr<Light> createSpotLight(const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);

  private:
    Type      m_type;
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_color;
    float     m_intensity;
    bool      m_enabled;
};

#endif // ENGINE_RENDERER_LIGHT_H_