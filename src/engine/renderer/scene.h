#ifndef ENGINE_RENDERER_SHADERS_SCENE_H_
#define ENGINE_RENDERER_SHADERS_SCENE_H_

#include "engine/renderer/camera.h"
#include "engine/renderer/lighting/light_manager.h"
#include "engine/renderer/resources/model_resource.h"

#include <memory>

class Scene
{
  public:
    Scene()  = default;
    ~Scene() = default;

    bool initialize();
    void update(float deltaTime);

    Camera* getCamera() const { return m_camera.get(); }

    void addModel(std::shared_ptr<ModelResource> model, const glm::mat4& transform = glm::mat4(1.0f));
    void removeModel(size_t index);

    LightManager* getLightManager() const { return m_lightManager.get(); }

    const std::vector<std::pair<std::shared_ptr<ModelResource>, glm::mat4>>& getModels() const { return m_models; }

  private:
    std::unique_ptr<Camera>                                           m_camera;
    std::unique_ptr<LightManager>                                     m_lightManager;
    std::vector<std::pair<std::shared_ptr<ModelResource>, glm::mat4>> m_models;

    void setupDefaultLights();
};

#endif // ENGINE_RENDERER_SHADERS_SCENE_H_