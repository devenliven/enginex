#include "pch.h"

#include "engine/core/input/input_manager.h"
#include "utilities/logger.h"

void InputManager::update()
{
    LOG_DEBUG("Active keys: {}", getActiveKeys().size());

    m_previousKeyStates = m_keyStates;

    for (auto& [key, state] : m_keyStates) {
        InputState oldState = state;
        switch (state) {
            case InputState::Pressed:
                state = InputState::Held;
                // debugLogStateC5hange(key, oldState, state);
                break;
            case InputState::JustReleased:
                state = InputState::Released;
                // debugLogStateChange(key, oldState, state);
                break;
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
    auto currentState = getKeyState(key);
    LOG_DEBUG("onKeyUp: Key {} ({}), current state: {}", static_cast<int>(key), static_cast<char>(key), static_cast<int>(currentState));
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