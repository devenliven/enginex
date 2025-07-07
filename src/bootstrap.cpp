#include "pch.h"

#include "bootstrap.h"
#include "engine/core/input/input_manager.h"

bool Bootstrap::init(std::shared_ptr<IApp> app)
{
    m_engine = std::make_unique<Engine>();

    if (!m_engine->init(app)) {
        LOG_ERROR("Failed to initialize Engine!");
        return false;
    }

    app->setInputManager(m_engine->getInputManager());

    return true;
}