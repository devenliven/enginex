#include "pch.h"

#include "engine/renderer/resources/model_resource.h"
#include "common/logger.h"

bool ModelResource::load(const std::string& path)
{
    if (isLoaded()) {
        LOG_WARN("ModelResource: Model already loaded: {}", path);
        return true;
    }

    m_path = path;

    try {
        m_model = std::make_unique<Model>(path);

        m_estimatedMemoryUsage = estimateModelMemoryUsage();
        LOG_INFO("ModelResource: Loaded model {} (estimated {} KB)", path, m_estimatedMemoryUsage);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("ModelResource: Failed to load model: {} - Error: {}", path, e.what());
        return false;
    }
}

void ModelResource::unload()
{
    if (m_model) {
        m_model.reset();
        m_estimatedMemoryUsage = 0;
        LOG_DEBUG("Unloaded model: {}", m_path);
    }
}

size_t ModelResource::getMemoryUsage() const
{
    return m_estimatedMemoryUsage;
}

size_t ModelResource::estimateModelMemoryUsage() const
{
    if (!m_model) return 0;

    size_t totalSize = 0;

    // TODO: THIS

    return totalSize;
}