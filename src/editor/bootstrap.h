#ifndef EDITOR_BOOTSTRAP_H_
#define EDITOR_BOOTSTRAP_H_

#include "utilities/logger.h"
#include "engine/core/engine.h"
#include "editor/app.h"

#include <memory>

class Bootstrap
{
  public:
    Bootstrap()  = default;
    ~Bootstrap() = default;

    bool init(std::shared_ptr<App> app)
    {
        m_engine = std::make_unique<Engine>();

        if (!m_engine->init(app)) {
            LOG_ERROR("Failed to initialize Engine!");
            return false;
        }

        return true;
    }

    Engine* getEngine() const { return m_engine.get(); }

  private:
    std::unique_ptr<Engine> m_engine = nullptr;
    // std::shared_ptr<App>    m_app    = nullptr;
};

#endif // EDITOR_BOOTSTRAP_H_