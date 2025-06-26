#include "pch.h"

#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    m_front            = glm::vec3(0.0f, 0.0f, -1.0f);
    m_movementSpeed    = SPEED;
    m_mouseSensitivity = SENSITIVITY;
    m_zoom             = ZOOM;
    m_position         = position;
    m_worldUp          = up;
    m_yaw              = yaw;
    m_pitch            = pitch;
    updateCamera();
}

void Camera::updateCamera()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::processKeyboard(CAMERA_MOVEMENT direction, float deltaTime, float speedMultiplier)
{
    float velocity = m_movementSpeed * deltaTime * speedMultiplier;
    if (direction == FORWARD) {
        m_position += m_front * velocity;
    }
    if (direction == BACKWARD) {
        m_position -= m_front * velocity;
    }
    if (direction == LEFT) {
        m_position -= m_right * velocity;
    }
    if (direction == RIGHT) {
        m_position += m_right * velocity;
    }
}

void Camera::processMouse(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch -= yoffset;

    if (constrainPitch) {
        if (m_pitch > 89.0f) {
            m_pitch = 89.0f;
        }

        if (m_pitch < -89.0f) {
            m_pitch = -89.0f;
        }
    }

    updateCamera();
}