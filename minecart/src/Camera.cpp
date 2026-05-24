#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(float screenWidth, float screenHeight,
               glm::vec3 position, glm::vec3 up,
               float yaw, float pitch)
    : m_position(position)
    , m_worldUp(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_fov(45.0f)
    , m_firstMouse(true)
    , m_lastX(screenWidth  / 2.0f)
    , m_lastY(screenHeight / 2.0f)
{
    updateCameraVectors();
}

void Camera::ProcessKeyboard(GLFWwindow* window, float deltaTime) {
    float velocity = SPEED * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_position += velocity * m_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_position -= velocity * m_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_position -= glm::normalize(glm::cross(m_front, m_up)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_position += glm::normalize(glm::cross(m_front, m_up)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_position += velocity * m_up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        m_position -= velocity * m_up;
}

void Camera::ProcessMouseMovement(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
        m_firstMouse = true;
        return;
    }

    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (m_firstMouse) {
        m_lastX      = x;
        m_lastY      = y;
        m_firstMouse = false;
    }

    float xoffset = (x - m_lastX) * SENSITIVITY;
    float yoffset = (m_lastY - y) * SENSITIVITY;
    m_lastX = x;
    m_lastY = y;

    m_yaw += xoffset;
    m_pitch = std::clamp(m_pitch + yoffset, -89.0f, 89.0f);

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}