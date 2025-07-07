#include "pch.h"
#include "engine/renderer/shader_resource.h"
#include "utilities/logger.h"

bool ShaderResource::load(const std::string& path)
{
    return loadFromPaths(path + ".vs", path + ".fs");
}

bool ShaderResource::loadFromPaths(const std::string& vertexPath, const std::string& fragmentPath)
{
    if (isLoaded()) {
        LOG_WARN("ShaderResource: {} + {} is already loaded.", vertexPath, fragmentPath);
        return true;
    }

    m_vertexPath   = vertexPath;
    m_fragmentPath = fragmentPath;
    m_path         = vertexPath + "+" + fragmentPath;

    try {
        m_shader = std::make_unique<Shader>(vertexPath, fragmentPath);
        LOG_INFO("ShaderResrouce: {} + {} loaded.", vertexPath, fragmentPath);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Shader Resource: Failed to load {} + {} - Error: {}", vertexPath, fragmentPath, e.what());
        return false;
    }
}

void ShaderResource::unload()
{
    if (m_shader) {
        m_shader.reset();
        LOG_DEBUG("ShaderResource: {} unloaded", m_path);
    }
}