#ifndef INPUT_INPUT_MANAGER_H_
#define INPUT_INPUT_MANAGER_H_

#include "engine/core/win32/os.h"

#include <unordered_map>
#include <vector>
#include <functional>

enum class InputState { Released = 0, Pressed, Held, JustReleased };
enum class MouseInputState { Released = 0, Pressed, Held, JustReleased };

class InputManager
{
  public:
    using KeyCallback       = std::function<void(KeyCode, InputState)>;
    using MouseCallback     = std::function<void(MouseButton, MouseInputState)>;
    using MouseMoveCallback = std::function<void(float deltaX, float deltaY)>;

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

    void            onMouseButtonDown(MouseButton button);
    void            onMouseButtonUp(MouseButton button);
    bool            isMouseButtonPressed(MouseButton button) const;
    bool            isMouseButtonHeld(MouseButton button) const;
    bool            wasMouseButtonJustPressed(MouseButton button) const;
    bool            wasMouseButtonJustReleased(MouseButton button) const;
    MouseInputState getMouseButtonState(MouseButton button) const;

    void          onMouseInput(const RawMouseInput& input);
    RawMouseInput getMouseDelta() const;
    void          resetMouseDelta();

    // void setCursorVisible(bool visible);
    // void setCursorLocked(bool locked);

  private:
    std::unordered_map<KeyCode, InputState>  m_keyStates;
    std::unordered_map<KeyCode, InputState>  m_previousKeyStates;
    std::unordered_map<KeyCode, KeyCallback> m_keyCallbacks;

    std::unordered_map<MouseButton, MouseInputState> m_mouseButtonStates;
    std::unordered_map<MouseButton, MouseInputState> m_previousMouseButtonStates;
    std::unordered_map<MouseButton, MouseCallback>   m_mouseButtonCallbacks;

    MouseMoveCallback m_mouseMoveCallback = nullptr;

    float m_mouseDeltaX     = 0;
    float m_mouseDeltaY     = 0;
    int   m_mouseWheelDelta = 0;
    bool  m_cursorLocked    = false;

    void updateKeyState(KeyCode key, bool isPressed);
    void triggerCallback(KeyCode key, InputState state);

    void updateMouseButtonState(MouseButton button, bool isPressed);
    void triggerMouseButtonCallback(MouseButton button, MouseInputState state);
};

#endif // INPUT_INPUT_MANAGER_H_