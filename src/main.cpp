#include "pch.h"

#include "common/logger.h"
#include "editor/application.h"
#include "bootstrap.h"

int main()
{
    std::shared_ptr<IApp> app       = std::make_shared<App>();
    auto                  bootstrap = std::make_unique<Bootstrap>();

    if (!bootstrap->init(app)) {
        LOG_ERROR("Failed to bootstrap App!");
        return -1;
    }

    bootstrap->getEngine()->run();

    return 0;
}