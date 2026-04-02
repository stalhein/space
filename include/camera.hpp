#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraDirections { FORWARD, BACKWARD, UP, DOWN, RIGHT, LEFT };

constexpr float SPEED = 500.f;
constexpr float SENSITIVITY = 0.1f;

class Camera {
public:
  glm::vec3 position, front, frontXZ, right, up, worldUp;
  float yaw, pitch;

  Camera(glm::vec3 startPosition, float startYaw, float startPitch)
      : position(startPosition), yaw(startYaw), pitch(startPitch),
        front(glm::vec3(0.f)), frontXZ(glm::vec3(0.f)),
        up(glm::vec3(0.f, 1.f, 0.f)), worldUp(up) {
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix(float renderConstant) {
    return glm::lookAt(position * glm::vec3(renderConstant),
                       position * glm::vec3(renderConstant) + front, up);
  }

  void processKeyboard(CameraDirections direction, float deltaTime) {
    float velocity = SPEED * deltaTime;

    if (direction == FORWARD)
      position += frontXZ * velocity;
    if (direction == BACKWARD)
      position -= frontXZ * velocity;
    if (direction == UP)
      position += worldUp * velocity;
    if (direction == DOWN)
      position -= worldUp * velocity;
    if (direction == RIGHT)
      position += right * velocity;
    if (direction == LEFT)
      position -= right * velocity;
  }

  void processMouse(float xoffset, float yoffset) {
    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.f)
      pitch = 89.f;
    if (pitch < -89.f)
      pitch = -89.f;

    updateCameraVectors();
  }

private:
  void updateCameraVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = 0.f;
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontXZ = glm::normalize(f);
    f.y = sin(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }
};
