#ifndef ENGINE_RENDERER_CAMERA_H_
#define ENGINE_RENDERER_CAMERA_H_

#include "utilities/logger.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CAMERA_MOVEMENT { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera
{
  public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    glm::mat4 getViewMatrix() const
    {
        // LOG_INFO("{}, {}, {}", m_position.x, m_position.y, m_position.z);
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
    void processKeyboard(CAMERA_MOVEMENT direction, float deltaTime);

    float getZoom() const { return m_zoom; }

  private:
    void updateCamera();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

    static constexpr float YAW         = -90.0f;
    static constexpr float SPEED       = 100.5f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM        = 45.0f;
    static constexpr float PITCH       = 0.0f;
};

#endif // ENGINE_RENDERER_CAMERA_H_