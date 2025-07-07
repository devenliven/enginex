#ifndef ENGINE_RENDERER_RESOURCE_MANAGER_H_
#define ENGINE_RENDERER_RESOURCE_MANAGER_H_

#include "engine/core/resource.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>
#include <mutex>

class TextureResource;
class ShaderResource;
class ModelResource;

template <typename T> class ResourceCache
{
  public:
    using ResourceCreator = std::function<std::shared_ptr<T>()>;

    std::shared_ptr<T> get(const std::string& path, ResourceCreator creator = nullptr);
    void               remove(const std::string& path);
    void               clear();
    size_t             getCount() const { return m_resources.size(); }

    std::vector<std::string> getLoadedPaths() const;

  private:
    std::unordered_map<std::string, std::weak_ptr<T>> m_resources;
    mutable std::mutex                                m_mutex;

    void cleanup();
};

class ResourceManager
{
  public:
    static ResourceManager& getInstance();

    std::shared_ptr<TextureResource> getTexture(const std::string& path);
    std::shared_ptr<ShaderResource>  getShader(const std::string& name);
    std::shared_ptr<ShaderResource>  getShader(const std::string& vertexPath, const std::string& fragmentPath);
    std::shared_ptr<ModelResource>   getModel(const std::string& path);

    void clearAll();
    void clearTextures();
    void clearShaders();
    void clearModels();

    struct Stats {
        size_t textureCount = 0;
        size_t shaderCount  = 0;
        size_t modelCount   = 0;
    };

    Stats getStats() const;
    void  logStats() const;

  private:
    ResourceManager()  = default;
    ~ResourceManager() = default;

    ResourceCache<TextureResource> m_textureCache;
    ResourceCache<ShaderResource>  m_shaderCache;
    ResourceCache<ModelResource>   m_modelCache;

    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};

#define RESOURCE_MANAGER ResourceManager::getInstance()
#define GET_TEXTURE(path) RESOURCE_MANAGER.getTexture(path)
#define GET_SHADER(name) RESOURCE_MANAGER.getShader(name)
#define GET_MODEL(path) RESOURCE_MANAGER.getModel(path)

#endif // ENGINE_RENDERER_RESOURCE_MANAGER_H_