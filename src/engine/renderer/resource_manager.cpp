#include "pch.h"

#include "engine/renderer/resource_manager.h"
#include "engine/renderer/texture_resource.h"
#include "engine/renderer/shader_resource.h"
#include "engine/renderer/model_resource.h"
#include "utilities/logger.h"

template <typename T> std::shared_ptr<T> ResourceCache<T>::get(const std::string& path, ResourceCreator creator)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    cleanup();

    auto it = m_resources.find(path);
    if (it != m_resources.end()) {
        if (auto resource = it->second.lock()) {
            LOG_DEBUG("ResourceCache: Retrieved cached resource: {}", path);
            return resource;
        } else {
            m_resources.erase(it);
        }
    }

    std::shared_ptr<T> resource;
    if (creator) {
        resource = creator();
    } else {
        resource = std::make_shared<T>();
    }

    if (resource && resource->load(path)) {
        m_resources[path] = resource;
        LOG_INFO("ResourceCache: Loaded new resource: {}", path);
        return resource;
    }

    LOG_ERROR("ResourceCache: Failed to load resource: {}", path);
    return nullptr;
}

template <typename T> void ResourceCache<T>::remove(const std::string& path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto                        it = m_resources.find(path);
    if (it != m_resources.end()) {
        if (auto resource = it->second.lock()) {
            resource->unload();
        }
        m_resources.erase(it);
        LOG_DEBUG("ResourceCache: Removed resource: {}", path);
    }
}

template <typename T> void ResourceCache<T>::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_resources) {
        if (auto resource = pair.second.lock()) {
            resource->unload();
        }
    }
    m_resources.clear();
    LOG_DEBUG("ResourceCache: Cleared all resources from cache!");
}

template <typename T> std::vector<std::string> ResourceCache<T>::getLoadedPaths() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string>    paths;
    for (const auto& pair : m_resources) {
        if (auto resource = pair.second.lock()) {
            paths.push_back(pair.first);
        }
    }
    return paths;
}

template <typename T> void ResourceCache<T>::cleanup()
{
    auto it = m_resources.begin();
    while (it != m_resources.end()) {
        if (it->second.expired()) {
            it = m_resources.erase(it);
        } else {
            ++it;
        }
    }
}

template class ResourceCache<TextureResource>;
template class ResourceCache<ShaderResource>;
template class ResourceCache<ModelResource>;

ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

std::shared_ptr<TextureResource> ResourceManager::getTexture(const std::string& path)
{
    return m_textureCache.get(path);
}

std::shared_ptr<ShaderResource> ResourceManager::getShader(const std::string& name)
{
    return m_shaderCache.get(name);
}

std::shared_ptr<ShaderResource> ResourceManager::getShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string combinedPath = vertexPath + "+" + fragmentPath;
    return m_shaderCache.get(combinedPath, [=]() {
        auto shader = std::make_shared<ShaderResource>();
        return shader;
    });
}

std::shared_ptr<ModelResource> ResourceManager::getModel(const std::string& path)
{
    return m_modelCache.get(path);
}

void ResourceManager::clearAll()
{
    LOG_INFO("ResourceManager: Clearing all resource");
    m_textureCache.clear();
    m_shaderCache.clear();
    m_modelCache.clear();
}

void ResourceManager::clearTextures()
{
    m_textureCache.clear();
}

void ResourceManager::clearShaders()
{
    m_shaderCache.clear();
}

void ResourceManager::clearModels()
{
    m_modelCache.clear();
}

ResourceManager::Stats ResourceManager::getStats() const
{
    Stats stats;
    stats.textureCount = m_textureCache.getCount();
    stats.shaderCount  = m_shaderCache.getCount();
    stats.modelCount   = m_modelCache.getCount();
    return stats;
}

void ResourceManager::logStats() const
{
    auto stats = getStats();
    LOG_INFO("ResourceManager: Current stats:");
    LOG_INFO("  Textures: {}", stats.textureCount);
    LOG_INFO("  Shaders: {}", stats.shaderCount);
    LOG_INFO("  Models: {}", stats.modelCount);
}