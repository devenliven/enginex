#ifndef ENGINE_CORE_APP_INTERFACE_H_
#define ENGINE_CORE_APP_INTERFACE_H_

#include "engine/core/win32/os.h"

class IApp
{
  public:
    virtual ~IApp() = default;

    virtual void onInit()                  = 0;
    virtual void onUpdate(float deltaTime) = 0;
    virtual void onRender()                = 0;

    virtual void onWindowResize(int newWidth, int newHeight) {}
    virtual void onKeyPressed(KeyCode key, int repeatCount) {}
    virtual void onKeyReleased(KeyCode key) {}

    virtual void setInputManager(class InputManager* inputManager) = 0;
};

#endif // ENGINE_CORE_APP_INTERFACE_H_