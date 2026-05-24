#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
    public:
        Camera(float screenWidth, float screenHeight,
            glm::vec3 position  = glm::vec3(0.0f, 28.0f, 32.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = -90.0f,
            float pitch = -45.0f);

        void ProcessKeyboard(GLFWwindow* window, float deltaTime);

        void ProcessMouseMovement(GLFWwindow* window, double xpos, double ypos);

        glm::mat4 GetViewMatrix() const;

        float GetFov() const { return m_fov; }
        glm::vec3 GetPosition() const { return m_position; }

    private:
        void updateCameraVectors();

        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_worldUp;

        float m_yaw;
        float m_pitch;
        float m_fov;

        bool  m_firstMouse;
        float m_lastX;
        float m_lastY;

        static constexpr float SPEED = 7.0f;
        static constexpr float SENSITIVITY = 0.1f;
};