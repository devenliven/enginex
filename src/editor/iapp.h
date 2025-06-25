#ifndef EDITOR_IAPP_H_
#define EDITOR_IAPP_H_

#include "utilities/logger.h"
#include "engine/core/engine.h"
#include "editor/app.h"

#include <memory>

class IApp
{
  public:
    bool init()
    {
        m_engine = std::make_unique<Engine>();
        m_app    = std::make_shared<App>();

        if (!m_engine->init(m_app)) {
            LOG_ERROR("Failed to initialize Engine!");
            return false;
        }

        return true;
    }

  private:
    std::unique_ptr<Engine> m_engine = nullptr;
    std::shared_ptr<App>    m_app    = nullptr;
};

#endif // EDITOR_IAPP_H_