#ifndef ENGINE_RENDERER_TEXTURE_RESOURCE_H_
#define ENGINE_RENDERER_TEXTURE_RESOURCE_H_

#include "engine/core/resource.h"
#include <glad/glad.h>

class TextureResource : public IResource
{
  public:
    TextureResource() = default;
    ~TextureResource() override;

    bool load(const std::string& path) override;
    void unload() override;
    bool isLoaded() const override { return m_textureId != 0; }

    uint32_t getTextureId() const { return m_textureId; }
    int      getWidth() const { return m_width; }
    int      getHeight() const { return m_height; }
    int      getChannels() const { return m_channels; }

  private:
    uint32_t m_textureId = 0;
    int      m_width     = 0;
    int      m_height    = 0;
    int      m_channels  = 0;
};

#endif // ENGINE_RENDERER_TEXTURE_RESOURCE_H_