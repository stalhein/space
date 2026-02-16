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

constexpr float SPEED = 1000000000.0f;
constexpr float SENSITIVITY = 0.1f;

class Camera
{
public:
    glm::dvec3 position;
    glm::vec3 front, frontXZ, up, right, worldUp;
    float yaw, pitch, speed, sensitivity;
    

    Camera(glm::dvec3 startPosition, float startYaw, float startPitch)
    {
        position = startPosition;
        front = glm::vec3(0.0f);
        frontXZ = glm::vec3(front.x, 0.0f, front.z);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        worldUp = up;
        yaw = startYaw;
        pitch = startPitch;
        speed = SPEED;
        sensitivity = SENSITIVITY;
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix(double RENDER_SCALE)
    {
        return glm::lookAt(glm::vec3(position) * glm::vec3(RENDER_SCALE), glm::vec3(position * RENDER_SCALE) + front, up);
    }

    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = speed * deltaTime;
        if (direction == FORWARDS) position += glm::dvec3(frontXZ * velocity);
        if (direction == BACKWARDS) position -= glm::dvec3(frontXZ * velocity);
        if (direction == UPWARDS) position += glm::dvec3(worldUp * velocity);
        if (direction == DOWNWARDS) position -= glm::dvec3(worldUp * velocity);
        if (direction == RIGHT) position += glm::dvec3(right * velocity);
        if (direction == LEFT) position -= glm::dvec3(right * velocity);
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