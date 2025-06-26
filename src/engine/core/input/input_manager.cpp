#include "pch.h"

#include "engine/core/input/input_manager.h"
#include "utilities/logger.h"

void InputManager::update()
{
    m_previousKeyStates         = m_keyStates;
    m_previousMouseButtonStates = m_mouseButtonStates;

    for (auto& [key, state] : m_keyStates) {
        switch (state) {
            case InputState::Pressed: state = InputState::Held; break;
            case InputState::JustReleased: state = InputState::Released; break;
            default: break;
        }
    }

    for (auto& [button, state] : m_mouseButtonStates) {
        switch (state) {
            case MouseInputState::Pressed: state = MouseInputState::Held; break;
            case MouseInputState::JustReleased: state = MouseInputState::Released; break;
            default: break;
        }
    }
}

void InputManager::onKeyDown(KeyCode key, int repeatCount)
{
    auto currentState = getKeyState(key);

    if (currentState == InputState::Released || currentState == InputState::JustReleased) {
        m_keyStates[key] = InputState::Pressed;
        triggerCallback(key, InputState::Pressed);
    }
}

void InputManager::onKeyUp(KeyCode key)
{
    m_keyStates[key] = InputState::JustReleased;
    triggerCallback(key, InputState::JustReleased);
}

bool InputManager::isKeyPressed(KeyCode key) const
{
    auto state = getKeyState(key);
    return state == InputState::Pressed || state == InputState::Held;
}

bool InputManager::isKeyHeld(KeyCode key) const
{
    return getKeyState(key) == InputState::Held;
}

bool InputManager::isKeyReleased(KeyCode key) const
{
    auto state = getKeyState(key);
    return state == InputState::Released || state == InputState::JustReleased;
}

bool InputManager::wasKeyJustPressed(KeyCode key) const
{
    return getKeyState(key) == InputState::Pressed;
}

bool InputManager::wasKeyJustReleased(KeyCode key) const
{
    return getKeyState(key) == InputState::JustReleased;
}

InputState InputManager::getKeyState(KeyCode key) const
{
    auto it = m_keyStates.find(key);
    return (it != m_keyStates.end()) ? it->second : InputState::Released;
}

void InputManager::registerKeyCallback(KeyCode key, KeyCallback callback)
{
    m_keyCallbacks[key] = std::move(callback);
}

void InputManager::unregisterKeyCallback(KeyCode key)
{
    m_keyCallbacks.erase(key);
}

void InputManager::clearAllStates()
{
    m_keyStates.clear();
    m_previousKeyStates.clear();
}

std::vector<KeyCode> InputManager::getActiveKeys() const
{
    std::vector<KeyCode> activeKeys;
    for (const auto& [key, state] : m_keyStates) {
        if (state == InputState::Pressed || state == InputState::Held) {
            activeKeys.push_back(key);
        }
    }
    return activeKeys;
}

void InputManager::updateKeyState(KeyCode key, bool isPressed)
{
    if (isPressed) {
        onKeyDown(key, 0);
    } else {
        onKeyUp(key);
    }
}

void InputManager::triggerCallback(KeyCode key, InputState state)
{
    auto it = m_keyCallbacks.find(key);
    if (it != m_keyCallbacks.end()) {
        it->second(key, state);
    }
}

void InputManager::onMouseButtonDown(MouseButton button)
{
    auto currentState = getMouseButtonState(button);
    if (currentState == MouseInputState::Released) {
        m_mouseButtonStates[button] = MouseInputState::Pressed;
        triggerMouseButtonCallback(button, MouseInputState::Pressed);
    }
}

void InputManager::onMouseButtonUp(MouseButton button)
{
    m_mouseButtonStates[button] = MouseInputState::JustReleased;
    triggerMouseButtonCallback(button, MouseInputState::JustReleased);
}

bool InputManager::isMouseButtonPressed(MouseButton button) const
{
    auto state = getMouseButtonState(button);
    return state == MouseInputState::Pressed || state == MouseInputState::Held;
}

bool InputManager::isMouseButtonHeld(MouseButton button) const
{
    auto state = getMouseButtonState(button);
    return state == MouseInputState::Held;
}

bool InputManager::wasMouseButtonJustPressed(MouseButton button) const
{
    return getMouseButtonState(button) == MouseInputState::Pressed;
}

bool InputManager::wasMouseButtonJustReleased(MouseButton button) const
{
    return getMouseButtonState(button) == MouseInputState::Released;
}

MouseInputState InputManager::getMouseButtonState(MouseButton button) const
{
    auto it = m_mouseButtonStates.find(button);
    return (it != m_mouseButtonStates.end()) ? it->second : MouseInputState::Released;
}

void InputManager::onMouseInput(const RawMouseInput& input)
{
    m_mouseDeltaX += input.deltaX;
    m_mouseDeltaY += input.deltaY;
    m_mouseWheelDelta += input.wheelDelta;

    if (m_mouseMoveCallback) {
        m_mouseMoveCallback(input.deltaX, input.deltaY);
    }

    LOG_DEBUG("Raw mouse: dx={}, dy={}", (float)input.deltaX, (float)input.deltaY);
}

MouseDelta InputManager::getMouseDelta() const
{
    // deltaX = m_mouseDeltaX;
    // deltaY = m_mouseDeltaY;
    MouseDelta delta;
    delta.x = m_mouseDeltaX;
    delta.y = m_mouseDeltaY;
    return delta;
}

void InputManager::resetMouseDelta()
{
    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;
}

void InputManager::triggerMouseButtonCallback(MouseButton button, MouseInputState state)
{
    auto it = m_mouseButtonCallbacks.find(button);
    if (it != m_mouseButtonCallbacks.end()) {
        it->second(button, state);
    }
}