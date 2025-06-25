#ifndef INPUT_INPUT_MANAGER_H_
#define INPUT_INPUT_MANAGER_H_

#include "engine/core/win32/os.h"

#include <unordered_map>
#include <vector>
#include <functional>

enum class InputState { Released = 0, Pressed, Held, JustReleased };

class InputManager
{
  public:
    using KeyCallback = std::function<void(KeyCode, InputState)>;

    InputManager()  = default;
    ~InputManager() = default;

    void update();

    void onKeyDown(KeyCode key, int repeatCount);
    void onKeyUp(KeyCode key);

    bool isKeyPressed(KeyCode key) const;
    bool isKeyHeld(KeyCode key) const;
    bool isKeyReleased(KeyCode key) const;
    bool wasKeyJustReleased(KeyCode key) const;
    bool wasKeyJustPressed(KeyCode key) const;

    InputState getKeyState(KeyCode key) const;

    void registerKeyCallback(KeyCode key, KeyCallback callback);
    void unregisterKeyCallback(KeyCode key);

    void                 clearAllStates();
    std::vector<KeyCode> getActiveKeys() const;

  private:
    std::unordered_map<KeyCode, InputState>  m_keyStates;
    std::unordered_map<KeyCode, InputState>  m_previousKeyStates;
    std::unordered_map<KeyCode, KeyCallback> m_keyCallbacks;

    void updateKeyState(KeyCode key, bool isPressed);
    void triggerCallback(KeyCode key, InputState state);
};

#endif // INPUT_INPUT_MANAGER_H_