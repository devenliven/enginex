#ifndef EDITOR_BOOTSTRAP_H_
#define EDITOR_BOOTSTRAP_H_

#include "utilities/logger.h"
#include "engine/core/engine.h"
#include "editor/app_interface.h"

#include <memory>

class Bootstrap
{
  public:
    Bootstrap()  = default;
    ~Bootstrap() = default;

    bool    init(std::shared_ptr<IApp> app);
    Engine* getEngine() const { return m_engine.get(); }

  private:
    std::unique_ptr<Engine> m_engine = nullptr;
};

#endif // EDITOR_BOOTSTRAP_H_