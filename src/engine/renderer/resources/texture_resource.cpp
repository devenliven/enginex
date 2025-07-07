#include "pch.h"

#include "engine/renderer/resources/texture_resource.h"
#include "common/logger.h"
#include "common/stb_image.h"

#include <filesystem>

TextureResource::~TextureResource()
{
    unload();
}

bool TextureResource::load(const std::string& path)
{
    if (isLoaded()) {
        LOG_WARN("Texture already loaded: {}", path);
        return true;
    }

    m_path = path;

    if (!std::filesystem::exists(path)) {
        LOG_ERROR("Texture file does not exist: {}", path);
        return false;
    }

    LOG_INFO("Loading texture: {}", path);

    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data) {
        LOG_ERROR("stbi_load failed for texture: {} - {}", path, stbi_failure_reason());
        return false;
    }

    LOG_INFO("Texture data loaded: {}x{}x{} channels", m_width, m_height, m_channels);

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    GLenum format = GL_RGB;
    if (m_channels == 1)
        format = GL_RED;
    else if (m_channels == 3)
        format = GL_RGB;
    else if (m_channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    LOG_INFO("Successfully loaded texture: {} (ID: {}, {}x{}, {} channels)", path, m_textureId, m_width, m_height, m_channels);
    return true;
}

void TextureResource::unload()
{
    if (m_textureId != 0) {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
        LOG_DEBUG("Unloaded texture: {}", m_path);
    }
}

size_t TextureResource::getMemoryUsage() const
{
    if (!isLoaded()) return 0;

    // Improve this, it's not very accurate. 1.33f is number pulled from google. Roughly 1.33x base size
    size_t baseSize = m_width * m_height * m_channels;
    return static_cast<size_t>(baseSize * 1.33f);
}