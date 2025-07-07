#ifndef ENGINE_RENDERER_MODEL_RESOURCE_H_
#define ENGINE_RENDERER_MODEL_RESOURCE_H_

#include "engine/core/resource.h"
#include "engine/renderer/geometry/model.h"

#include <memory>

class ModelResource : public IResource
{
  public:
    ModelResource()           = default;
    ~ModelResource() override = default;

    bool load(const std::string& path) override;
    void unload() override;
    bool isLoaded() const override { return m_model != nullptr; }

    Model* getModel() const { return m_model.get(); }

  private:
    std::unique_ptr<Model> m_model;
};

#endif // ENGINE_RENDERER_MODEL_RESOURCE_H_