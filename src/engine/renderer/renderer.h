#ifndef ENGINE_RENDERER_RENDERER_H_
#define ENGINE_RENDERER_RENDERER_H_

#include "engine/renderer/resources/shader_resource.h"

#include <memory>

struct ImVec2;

class Scene;

class Renderer
{
  public:
    Renderer()  = default;
    ~Renderer() = default;

    bool initialize();
    void shutdown();

    void beginFrame();
    void renderScene(Scene* scene);
    void endFrame();
    void renderSceneToViewport(Scene* scene);

    ImVec2 getViewportSize() const;
    float  getViewportAspectRatio() const;

  private:
    std::shared_ptr<ShaderResource> m_pbrShader;

    int m_viewportWidth  = 1280;
    int m_viewportHeight = 720;

    static constexpr float DEFAULT_ASPECT_RATIO = 1280.0f / 720.0f;
    static constexpr float DEFAULT_FOV          = 45.0f;
    static constexpr float DEFAULT_NEAR_PLANE   = 0.1f;
    static constexpr float DEFAULT_FAR_PLANE    = 100.0f;

    void setupShaders();
    void renderModel(const std::shared_ptr<class ModelResource>& model, const glm::mat4& modelMatrix, class Shader* shader);
};

#endif // ENGINE_RENDERER_RENDERER_H_