#ifndef ENGINE_RENDERER_LIGHT_MANAGER_H_
#define ENGINE_RENDERER_LIGHT_MANAGER_H_

#include "engine/renderer/lighting/light.h"
#include "engine/renderer/shaders/shader.h"
#include "editor/tools/line_renderer.h"

#include <vector>
#include <memory>

class LightManager
{
  public:
    LightManager()  = default;
    ~LightManager() = default;

    void addLight(std::unique_ptr<Light> light);
    void removeLight(size_t index);
    void clearLights();

    size_t                                     getLightCount() const { return m_lights.size(); }
    Light*                                     getLight(size_t index);
    const std::vector<std::unique_ptr<Light>>& getLights() const { return m_lights; }

    void updateShaderUniforms(Shader* shader) const;
    void renderDebugVisualization(LineRenderer* renderer, const glm::vec3& modelCenter) const;

  private:
    std::vector<std::unique_ptr<Light>> m_lights;
    static constexpr size_t             MAX_LIGHTS = 4;
};

#endif // ENGINE_RENDERER_LIGHT_MANAGER_H_