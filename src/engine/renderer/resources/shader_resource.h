#ifndef ENGINE_RENDERER_SHADER_RESOURCE_H_
#define ENGINE_RENDERER_SHADER_RESOURCE_H_

#include "engine/core/resource.h"
#include "engine/renderer/shaders/shader.h"
#include <memory>

class ShaderResource : public IResource
{
  public:
    ShaderResource()           = default;
    ~ShaderResource() override = default;

    bool   load(const std::string& path) override;
    void   unload() override;
    bool   isLoaded() const override { return m_shader != nullptr; }
    size_t getMemoryUsage() const override { return sizeof(Shader); }

    Shader* getShader() const { return m_shader.get(); }
    bool    loadFromPaths(const std::string& vertexPath, const std::string& fragmentPath);

  private:
    std::unique_ptr<Shader> m_shader;
    std::string             m_vertexPath;
    std::string             m_fragmentPath;
};

#endif // ENGINE_RENDERER_SHADER_RESOURCE_H_