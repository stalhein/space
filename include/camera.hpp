#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    FORWARDS,
    BACKWARDS,
    UPWARDS,
    DOWNWARDS,
    RIGHT,
    LEFT
};

constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 10.0f;
constexpr float SENSITIVITY = 0.1f;

class Camera
{
public:
    glm::vec3 position, front, frontXZ, up, right, worldUp;
    float yaw, pitch, speed, sensitivity;
    

    Camera(glm::vec3 position)
    {
        position = position;
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        frontXZ = glm::vec3(front.x, 0.0f, front.z);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        worldUp = up;
        yaw = YAW;
        pitch = PITCH;
        speed = SPEED;
        sensitivity = SENSITIVITY;
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = speed * deltaTime;
        if (direction == FORWARDS) position += frontXZ * velocity;
        if (direction == BACKWARDS) position -= frontXZ * velocity;
        if (direction == UPWARDS) position += worldUp * velocity;
        if (direction == DOWNWARDS) position -= worldUp * velocity;
        if (direction == RIGHT) position += right * velocity;
        if (direction == LEFT) position -= right * velocity;
    }

    void processMouse(float xoffset, float yoffset)
    {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = 0.0f;
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontXZ = glm::normalize(f);
        f.y = sin(glm::radians(pitch));
        front = glm::normalize(f);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }
};